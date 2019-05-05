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
#include <WCDB/Error.hpp>
#include <WCDB/SQLiteFTS3Tokenizer.h>
#include <WCDB/TokenizerModules.hpp>

namespace WCDB {

void TokenizerModules::add(const String& name, const TokenizerModule& module)
{
    WCTRemedialAssert(!name.empty(), "Name of module can't be null.", return;);
    LockGuard lockGuard(m_lock);
    m_modules[name] = module;
}

std::pair<bool, TokenizerModule> TokenizerModules::get(const String& name) const
{
    SharedLockGuard lockGuard(m_lock);
    bool exists = false;
    TokenizerModule module;
    auto iter = m_modules.find(name);
    if (iter != m_modules.end()) {
        exists = true;
        module = iter->second;
    }
    return { exists, module };
}

} // namespace WCDB
