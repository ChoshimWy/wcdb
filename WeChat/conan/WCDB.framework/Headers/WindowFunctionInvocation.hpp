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

#ifndef __WCDB_WINDOW_FUNCTION_INVOCATION_HPP
#define __WCDB_WINDOW_FUNCTION_INVOCATION_HPP

#include <WCDB/SQL.hpp>

namespace WCDB {

class WindowFunctionInvocation
: public TypedSyntax<Syntax::WindowFunctionInvocation, SQL> {
public:
    using TypedSyntax<Syntax::WindowFunctionInvocation, SQL>::TypedSyntax;
    explicit WindowFunctionInvocation(const String& name);

    WindowFunctionInvocation& invoke();
    WindowFunctionInvocation& invokeAll();
    WindowFunctionInvocation& argument(const Expression& expression);
    WindowFunctionInvocation& arguments(const Expressions& expressions);

    WindowFunctionInvocation& filter(const Filter& filter);

    WindowFunctionInvocation& over(const WindowDef& windowDef);
    WindowFunctionInvocation& over(const String& window);
};

} // namespace WCDB

#endif /* __WCDB_WINDOW_FUNCTION_INVOCATION_HPP */
