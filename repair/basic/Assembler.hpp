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

#include <WCDB/String.hpp>
#include <map>

namespace WCDB {

class Error;

namespace Repair {

class Cell;

class Assembler {
public:
    Assembler();
    virtual ~Assembler() = 0;

    virtual void setPath(const String &path) = 0;
    virtual const String &getPath() const = 0;

    virtual bool markAsAssembling() = 0;
    virtual bool markAsAssembled() = 0;

    virtual bool markAsMilestone() = 0;
    virtual bool assembleTable(const String &tableName, const String &sql) = 0;
    virtual bool assembleSequence(const String &tableName, int64_t sequence) = 0;
    virtual bool assembleCell(const Cell &cell) = 0;
    void markDuplicatedAsIgnorable(bool ignorable);

    virtual bool assembleSQL(const String &sql) = 0;

    virtual const Error &getError() const = 0;

protected:
    bool isDuplicatedIgnorable() const;

private:
    bool m_duplicatedIgnorable;
};

class AssemblerHolder {
public:
    AssemblerHolder();
    virtual ~AssemblerHolder() = 0;
    void setAssembler(Assembler *assembler);

protected:
    Assembler *m_assembler;
};

} //namespace Repair

} //namespace WCDB
