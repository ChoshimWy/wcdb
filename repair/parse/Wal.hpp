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

#ifndef __WCDB_WAL_HPP
#define __WCDB_WAL_HPP

#include <WCDB/FileHandle.hpp>
#include <WCDB/Initializeable.hpp>
#include <WCDB/PagerRelated.hpp>
#include <WCDB/Shm.hpp>
#include <map>
#include <set>

namespace WCDB {

namespace Repair {

class Wal final : public PagerRelated, public Initializeable {
#pragma mark - Initialize
public:
    Wal(Pager *pager);

    const String &getPath() const;
    static constexpr const int headerSize = 32;

protected:
    FileHandle m_fileHandle;
    friend class WalRelated;
    MappedData acquireData(off_t offset, size_t size);

#pragma mark - Page
public:
    bool containsPage(int pageno) const;
    MappedData acquirePageData(int pageno);
    MappedData acquirePageData(int pageno, off_t offset, size_t size);
    int getMaxPageno() const;

protected:
    // pageno -> frameno
    std::map<int, int> m_pages2Frames;

#pragma mark - Wal
public:
    void setShmLegality(bool flag);
    void setMaxAllowedFrame(int maxFrame);
    int getNumberOfFrames() const;
    const std::pair<uint32_t, uint32_t> &getSalt() const;
    int getPageSize() const;

protected:
    std::pair<uint32_t, uint32_t>
    calculateChecksum(const MappedData &data,
                      const std::pair<uint32_t, uint32_t> &checksum) const;

    size_t m_fileSize;
    uint32_t m_truncate;
    int m_maxAllowedFrame;
    int m_maxFrame;
    bool m_isNativeChecksum;
    std::pair<uint32_t, uint32_t> m_salt;
    bool m_shmLegality;
    Shm m_shm;

#pragma mark - Frame
public:
    int getFrameSize() const;
    MappedData acquireFrameData(int frameno);

#pragma mark - Initializeable
protected:
    bool doInitialize() override final;
    static bool isBigEndian();

#pragma mark - Error
public:
    void hint() const;

protected:
    void markAsCorrupted(int frame, const String &message);
    //    void markAsError(Error::Code code);

#pragma mark - Dispose
public:
    int getDisposedPages() const;
    void dispose();

protected:
    std::set<int> m_disposedPages;
};

} //namespace Repair

} //namespace WCDB

#endif /* __WCDB_WAL_HPP */
