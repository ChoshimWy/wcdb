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

#ifndef __WCDB_SYNTAX_TABLECONSTRAINT_HPP
#define __WCDB_SYNTAX_TABLECONSTRAINT_HPP

#include <WCDB/SyntaxColumn.hpp>
#include <WCDB/SyntaxExpression.hpp>
#include <WCDB/SyntaxForeignKeyClause.hpp>
#include <WCDB/SyntaxIdentifier.hpp>
#include <WCDB/SyntaxIndexedColumn.hpp>

namespace WCDB {

namespace Syntax {

class TableConstraint final : public Identifier {
#pragma mark - Lang
public:
    String name;
    WCDB_SYNTAX_MAIN_UNION_ENUM(PrimaryKey, Unique, Check, ForeignKey, );

    std::list<IndexedColumn> indexedColumns;
    WCDB_SYNTAX_ENUM_UNION(Conflict, conflict);

    Expression expression;

    std::list<Column> columns;
    ForeignKeyClause foreignKeyClause;

#pragma mark - Identifier
public:
    static constexpr const Type type = Type::TableConstraint;
    Type getType() const override final;
    String getValidDescription() const override final;
    void iterate(const Iterator& iterator, bool& stop) override final;
};

} // namespace Syntax

} // namespace WCDB

#endif /* __WCDB_SYNTAX_TABLECONSTRAINT_HPP */
