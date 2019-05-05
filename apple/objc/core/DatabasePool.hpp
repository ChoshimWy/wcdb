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

#ifndef __WCDB_DATABASE_POOL_HPP
#define __WCDB_DATABASE_POOL_HPP

#include <WCDB/Database.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/Path.hpp>

namespace WCDB {

typedef Recyclable<Database*> RecyclableDatabase;

class DatabasePoolEvent {
public:
    virtual ~DatabasePoolEvent() = 0;

protected:
    virtual void onDatabaseCreated(Database* database) = 0;
    friend class DatabasePool;
};

class DatabasePool final {
#pragma mark - DatabasePool
public:
    DatabasePool(DatabasePoolEvent* event);

    RecyclableDatabase getOrCreate(const String& path);
    RecyclableDatabase get(const String& path);

    void purge();

protected:
    struct ReferencedDatabase {
        ReferencedDatabase(std::shared_ptr<Database>&& database);
        ReferencedDatabase(ReferencedDatabase&& other);
        std::shared_ptr<Database> database;
        std::atomic<int> reference;
    };
    typedef struct ReferencedDatabase ReferencedDatabase;

    RecyclableDatabase get(const std::map<String, ReferencedDatabase>::iterator& iter);
    void flowBack(Database* database);

    std::map<String, ReferencedDatabase> m_databases; //path->{database, reference}
    SharedLock m_lock;

    DatabasePoolEvent* m_event;
};

} //namespace WCDB

#endif /* __WCDB_DATABASE_POOL_HPP */
