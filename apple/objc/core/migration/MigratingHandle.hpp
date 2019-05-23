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

#pragma once

#include <WCDB/Handle.hpp>
#include <WCDB/Migration.hpp>

namespace WCDB {

class MigrationInfo;

class MigratingHandle final : public Handle, public Migration::Binder {
#pragma mark - Initialize
public:
    MigratingHandle(Migration &migration);
    ~MigratingHandle();

#pragma mark - Binder
protected:
    bool bindInfos(const std::map<String, RecyclableMigrationInfo> &migratings) override final;

private:
    bool rebindViews(const std::map<String, RecyclableMigrationInfo> &migratings);
    bool rebindSchemas(const std::map<String, RecyclableMigrationInfo> &migratings);

#pragma mark - Info Initializer
protected:
    std::tuple<bool, bool, std::set<String>>
    getColumnsOfUserInfo(const MigrationUserInfo &userInfo) override final;
    std::pair<bool, bool> sourceTableExists(const MigrationUserInfo &userInfo) override final;
    String getDatabasePath() const override final;

#pragma mark - Migration
public:
    bool prepare(const Statement &statement) override final;
    bool isPrepared() override final;
    void finalize() override final;

    bool done() override final;
    bool step() override final;
    void reset() override final;

    void bindInteger32(const Integer32 &value, int index) override final;
    void bindInteger64(const Integer64 &value, int index) override final;
    void bindDouble(const Float &value, int index) override final;
    void bindText(const Text &value, int index) override final;
    void bindBLOB(const BLOB &value, int index) override final;
    void bindNull(int index) override final;

    Integer32 getInteger32(int index) override final;
    Integer64 getInteger64(int index) override final;
    Float getDouble(int index) override final;
    Text getText(int index) override final;
    BLOB getBLOB(int index) override final;

    ColumnType getType(int index) override final;
    const UnsafeString getOriginColumnName(int index) override final;
    const UnsafeString getColumnName(int index) override final;
    const UnsafeString getColumnTableName(int index) override final;

    bool isStatementReadonly() override final;
    int getNumberOfColumns() override final;

protected:
    bool realExecute(const std::list<Statement> &statements);
    bool realStep();
    std::pair<bool, std::list<Statement>> process(const Statement &statement);
    bool tryFallbackToUnionedView(Syntax::Schema &schema, String &table);
    bool tryFallbackToSourceTable(Syntax::Schema &schema, String &table);

#ifdef DEBUG
    bool m_processing;
#endif
    HandleStatement *m_additionalStatement;

#pragma mark - Migrate
protected:
    // For Insert Statement Only
    bool isMigratedPrepared();
    // only conflict clause will be used
    bool prepareMigrate(const String &table, Syntax::ConflictAction conflictAction);
    bool stepMigration(const int64_t &rowid);
    void finalizeMigrate();
    void resetMigrate();

private:
    HandleStatement *m_migrateStatement;
    HandleStatement *m_removeMigratedStatement;
};

} //namespace WCDB
