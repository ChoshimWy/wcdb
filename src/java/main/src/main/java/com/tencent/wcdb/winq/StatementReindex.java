// Created by qiuwenchen on 2023/6/12.
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

package com.tencent.wcdb.winq;

import com.tencent.wcdb.base.CppObject;

public class StatementReindex extends Statement {
    @Override
    protected CPPType getType() {
        return CPPType.ReindexSTMT;
    }

    public StatementReindex() {
        cppObj = createCppObj();
    }

    private native long createCppObj();

    public StatementReindex reindexCollation(String collation) {
        configCollation(cppObj, collation);
        return this;
    }

    private native void configCollation(long self, String collation);

    public StatementReindex reindexTable(String table) {
        configTable(cppObj, table);
        return this;
    }

    private native void configTable(long self, String table);

    public StatementReindex reindex(String index) {
        configIndex(cppObj, index);
        return this;
    }

    private native void configIndex(long self, String indexName);

    public StatementReindex of(String schemaName) {
        configSchema(cppObj, CPPType.String.ordinal(), 0, schemaName);
        return this;
    }

    public StatementReindex of(Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private native void configSchema(long self, int type, long object, String path);
}
