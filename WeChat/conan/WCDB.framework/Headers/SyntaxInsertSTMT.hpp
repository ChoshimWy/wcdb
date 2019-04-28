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

#ifndef __WCDB_SYNTAX_INSERT_STMT_HPP
#define __WCDB_SYNTAX_INSERT_STMT_HPP

#include <WCDB/SyntaxIdentifier.hpp>

namespace WCDB {

namespace Syntax {

class InsertSTMT final : public Identifier {
#pragma mark - Lang
public:
    WithClause withClause;
    WCDB_SYNTAX_ENUM_UNION(ConflictAction, conflictAction);
    Schema schema;
    String table;
    String alias;
    std::list<Column> columns;

    WCDB_SYNTAX_MAIN_UNION_ENUM(Values, Select, Default, );
    std::list<std::list<Expression>> expressionsValues;
    SelectSTMT select;

    UpsertClause upsertClause;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::InsertSTMT;
    Type getType() const override final;
    String getValidDescription() const override final;
    String getValidDescription(bool skipSchema) const;
    void iterate(const Iterator& iterator, bool& stop) override final;

#pragma mark - Utility
public:
    bool isTargetingSameTable(const InsertSTMT& other) const;
};

} // namespace Syntax

} // namespace WCDB

#endif /* __WCDB_SYNTAX_INSERT_STMT_HPP */
