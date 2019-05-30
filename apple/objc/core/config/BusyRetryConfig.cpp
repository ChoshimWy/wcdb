/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <WCDB/Assertion.hpp>
#include <WCDB/BusyRetryConfig.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/Handle.hpp>
#include <WCDB/Time.hpp>

namespace WCDB {

BusyRetryConfig::BusyRetryConfig()
: Config(), m_identifier(String::formatted("Busy-%p", this))
{
    Global::shared().setNotificationForLockEvent(
    m_identifier,
    std::bind(&BusyRetryConfig::willLock, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(&BusyRetryConfig::lockDidChange, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(
    &BusyRetryConfig::willShmLock, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
    std::bind(&BusyRetryConfig::shmLockDidChange,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3,
              std::placeholders::_4));
}

BusyRetryConfig::~BusyRetryConfig()
{
    Global::shared().setNotificationForLockEvent(
    m_identifier, nullptr, nullptr, nullptr, nullptr);
}

bool BusyRetryConfig::invoke(Handle* handle)
{
    handle->setNotificationWhenBusy(std::bind(
    &BusyRetryConfig::onBusy, this, std::placeholders::_1, std::placeholders::_2));
    return true;
}

bool BusyRetryConfig::uninvoke(Handle* handle)
{
    handle->setNotificationWhenBusy(nullptr);
    return true;
}

bool BusyRetryConfig::onBusy(const String& path, int numberOfTimes)
{
    WCDB_UNUSED(path);

    Trying& trying = *m_tryings.getOrCreate();
    WCTInnerAssert(trying.valid());
    if (numberOfTimes == 0) {
        trying.retrying(BusyRetryTimeOut);
    }
    return getOrCreateState(trying.getPath()).wait(trying);
}

#pragma mark - State
BusyRetryConfig::Expecting::Expecting() : m_category(Category::None)
{
}

bool BusyRetryConfig::Expecting::valid() const
{
    return m_category == Category::Pager || m_category == Category::Shm;
}

void BusyRetryConfig::Expecting::expecting(ShmLockType type, int mask)
{
    m_category = Category::Shm;
    m_shmType = type;
    m_shmMask = mask;
}

void BusyRetryConfig::Expecting::expecting(PagerLockType type)
{
    m_category = Category::Pager;
    WCTInnerAssert(type != PagerLockType::None);
    m_pagerType = type;
}

bool BusyRetryConfig::Expecting::satisfied(PagerLockType type) const
{
    WCTInnerAssert(valid());
    bool satisfied = true;
    if (m_category == Category::Pager) {
        switch (m_pagerType) {
        case PagerLockType::Reserved:
            satisfied = type < PagerLockType::Reserved;
            break;
        case PagerLockType::Pending:
            // fallthrough
        case PagerLockType::Exclusive:
            satisfied = type == PagerLockType::None;
            break;
        case PagerLockType::Shared:
            satisfied = type < PagerLockType::Pending;
            break;
        default:
            WCTInnerAssert(false);
            break;
        }
    }
    return satisfied;
}

bool BusyRetryConfig::Expecting::satisfied(int sharedMask, int exclusiveMask) const
{
    WCTInnerAssert(valid());
    bool satisified = true;
    if (m_category == Category::Shm) {
        switch (m_shmType) {
        case ShmLockType::Shared:
            satisified = (m_shmMask & exclusiveMask) == 0;
            break;
        default:
            WCTInnerAssert(m_shmType == ShmLockType::Exclusive);
            satisified
            = (m_shmMask & sharedMask) == 0 && (m_shmMask & exclusiveMask) == 0;
            break;
        }
    }
    return satisified;
}

BusyRetryConfig::State::ShmMask::ShmMask() : shared(0), exclusive(0)
{
}

BusyRetryConfig::State& BusyRetryConfig::getOrCreateState(const String& path)
{
    WCTInnerAssert(!path.empty());
    {
        SharedLockGuard lockGuard(m_statesLock);
        auto iter = m_states.find(path);
        if (iter != m_states.end()) {
            return iter->second;
        }
    }
    {
        LockGuard lockGuard(m_statesLock);
        return m_states[path];
    }
}

BusyRetryConfig::State::State() : m_pagerType(PagerLockType::None)
{
}

void BusyRetryConfig::State::updatePagerLock(PagerLockType type)
{
    std::lock_guard<std::mutex> lockGuard(m_lock);
    if (m_pagerType != type) {
        bool notify = type < m_pagerType;
        m_pagerType = type;
        if (notify) {
            tryNotify();
        }
    }
}

void BusyRetryConfig::State::updateShmLock(void* identifier, int sharedMask, int exclusiveMask)
{
    std::lock_guard<std::mutex> lockGuard(m_lock);
    bool notify = false;
    if (sharedMask == 0 && exclusiveMask == 0) {
        m_shmMasks.erase(identifier);
        notify = true;
    } else {
        State::ShmMask& mask = m_shmMasks[identifier];
        notify = sharedMask < mask.shared || exclusiveMask < mask.exclusive;
        mask.shared = sharedMask;
        mask.exclusive = exclusiveMask;
    }
    if (notify) {
        tryNotify();
    }
}

bool BusyRetryConfig::State::shouldWait(const Expecting& expecting) const
{
    bool wait = false;
    if (!expecting.satisfied(m_pagerType)) {
        wait = true;
    } else {
        for (const auto& iter : m_shmMasks) {
            if (!expecting.satisfied(iter.second.shared, iter.second.exclusive)) {
                wait = true;
                break;
            }
        }
    }
    return wait;
}

bool BusyRetryConfig::State::wait(Trying& trying)
{
    std::unique_lock<std::mutex> lockGuard(m_lock);
    while (shouldWait(trying)) {
        double remainingTimeForRetring = trying.remainingTimeForRetring();
        if (remainingTimeForRetring > 0) {
            Thread currentThread = Thread::current();
            // main thread first
            m_waitings.insert(Thread::isMain() ? 0 : 1, currentThread, trying);

            SteadyClock before = SteadyClock::now();
            m_conditional.wait_for(lockGuard, remainingTimeForRetring);

            double cost = SteadyClock::now().timeIntervalSinceSteadyClock(before);
            trying.retried(cost);

            m_waitings.erase(currentThread);
        } else {
            return false;
        }
    }
    return true;
}

void BusyRetryConfig::State::tryNotify()
{
    const auto& elements = m_waitings.elements();
    for (auto iter = elements.begin(); iter != elements.end(); ++iter) {
        if (!shouldWait(iter->value)) {
            m_conditional.notify(iter->key);
        }
    }
}

#pragma mark - Trying
BusyRetryConfig::Trying::Trying()
: Expecting(), m_elapsedTime(0), m_timeout(std::numeric_limits<int>::max())
{
}

void BusyRetryConfig::Trying::retrying(double timeout)
{
    m_timeout = timeout;
    m_elapsedTime = 0;
}

void BusyRetryConfig::Trying::retried(double cost)
{
    m_elapsedTime += cost;
}

double BusyRetryConfig::Trying::remainingTimeForRetring() const
{
    return m_timeout - m_elapsedTime;
}

void BusyRetryConfig::Trying::expecting(const String& path, ShmLockType type, int mask)
{
    WCTInnerAssert(!path.empty());
    m_path = path;
    Expecting::expecting(type, mask);
}

void BusyRetryConfig::Trying::expecting(const String& path, PagerLockType type)
{
    WCTInnerAssert(!path.empty());
    m_path = path;
    Expecting::expecting(type);
}

bool BusyRetryConfig::Trying::valid() const
{
    return !m_path.empty() && Expecting::valid();
}

const String& BusyRetryConfig::Trying::getPath() const
{
    return m_path;
}

#pragma mark - Lock Event
void BusyRetryConfig::willLock(const String& path, PagerLockType type)
{
    m_tryings.getOrCreate()->expecting(path, type);
}

void BusyRetryConfig::lockDidChange(const String& path, PagerLockType type)
{
    getOrCreateState(path).updatePagerLock(type);
}

void BusyRetryConfig::willShmLock(const String& path, ShmLockType type, int mask)
{
    m_tryings.getOrCreate()->expecting(path, type, mask);
}

void BusyRetryConfig::shmLockDidChange(const String& path, void* identifier, int sharedMask, int exclusiveMask)
{
    getOrCreateState(path).updateShmLock(identifier, sharedMask, exclusiveMask);
}

} // namespace WCDB
