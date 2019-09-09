//
// Created by sanhuazhang on 2019/05/02
//

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

#include <WCDB/Syntax.h>
#include <WCDB/SyntaxAssertion.hpp>

namespace WCDB {

namespace Syntax {

WindowFunctionInvocation::~WindowFunctionInvocation() = default;

bool WindowFunctionInvocation::isValid() const
{
    return !name.empty();
}

#pragma mark - Identifier
Identifier::Type WindowFunctionInvocation::getType() const
{
    return type;
}

bool WindowFunctionInvocation::describle(std::ostringstream& stream) const
{
    stream << name << "(";
    if (!expressions.empty()) {
        stream << expressions;
    } else if (wildcard) {
        stream << "*";
    }
    stream << ") ";
    if (filter.isValid()) {
        stream << filter << " ";
    }
    stream << "OVER";
    if (!window.empty()) {
        stream << space << window;
    } else {
        stream << windowDef;
    }
    return true;
}

void WindowFunctionInvocation::iterate(const Iterator& iterator, bool& stop)
{
    Identifier::iterate(iterator, stop);
    if (!expressions.empty()) {
        listIterate(expressions, iterator, stop);
    }
    if (filter.isValid()) {
        recursiveIterate(filter, iterator, stop);
    }
    if (window.empty()) {
        recursiveIterate(windowDef, iterator, stop);
    }
}

} // namespace Syntax

} // namespace WCDB
