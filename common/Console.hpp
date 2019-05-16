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

#ifndef __WCDB_CONSOLE_HPP
#define __WCDB_CONSOLE_HPP

#include <WCDB/Debug.hpp>
#include <WCDB/Macro.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/String.hpp>
#include <atomic>
#include <functional>

namespace WCDB {

class Error;

class Console final {
public:
    Console() = delete;
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

    static void initialize();

    static void debug();
    static void release();
    static bool debuggable();

    static void errored(const Notifier::Callback& callback);

#if WCDB_ASSERTION
    static void
    fatal(const String& message, const char* file, int line, const char* function);
#else  // WCDB_ASSERTION
    static void fatal(const String& message);
#endif // WCDB_ASSERTION

#if WCDB_TRACE
    static void
    trace(const String& message, const char* file, int line, const char* function);
#endif

private:
    static std::nullptr_t _initialize();

    static void report(const Error& error);
    static void print(const String& message);
    static void breakpoint() WCDB_USED WCDB_NO_INLINE;
    static std::atomic<bool>& debuggableValue();
};

} // namespace WCDB

#endif /* __WCDB_CONSOLE_HPP */
