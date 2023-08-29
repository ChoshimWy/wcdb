// Created by qiuwenchen on 2023/4/12.
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

public class StatementInsert extends Statement {
    @Override
    protected CPPType getType() {
        return CPPType.InsertSTMT;
    }

    public StatementInsert() {
        cppObj = createCppObj();
    }

    private static native long createCppObj();

    public StatementInsert with(CommonTableExpression expression) {
        return with(new CommonTableExpression[]{expression});
    }

    public StatementInsert with(CommonTableExpression[] expressions) {
        if(expressions == null || expressions.length == 0) {
            return this;
        }
        long[] cppExps = new long[expressions.length];
        for(int i = 0; i < expressions.length; i++) {
            cppExps[i] = CppObject.get(expressions[i]);
        }
        configWith(cppObj, cppExps);
        return this;
    }

    public StatementInsert withRecursive(CommonTableExpression expression) {
        return withRecursive(new CommonTableExpression[]{expression});
    }

    public StatementInsert withRecursive(CommonTableExpression[] expressions) {
        if(expressions == null || expressions.length == 0) {
            return this;
        }
        long[] cppExps = new long[expressions.length];
        for(int i = 0; i < expressions.length; i++) {
            cppExps[i] = CppObject.get(expressions[i]);
        }
        configWith(cppObj, cppExps);
        configRecursive(cppObj);
        return this;
    }

    private static native void configWith(long self, long[] expressions);

    private static native void configRecursive(long self);

    public StatementInsert insertInto(String tableName) {
        configTableName(cppObj, tableName);
        return this;
    }

    private static native void configTableName(long self, String tableName);

    public StatementInsert of(String schemaName) {
        configSchema(cppObj, CPPType.String.ordinal(), 0, schemaName);
        return this;
    }

    public StatementInsert of(Schema schema) {
        configSchema(cppObj, Identifier.getCppType(schema), CppObject.get(schema), null);
        return this;
    }

    private static native void configSchema(long self, int type, long schema, String schemaName);

    public StatementInsert orReplace() {
        configConflictAction(cppObj, ConflictAction.Replace.ordinal());
        return this;
    }

    public StatementInsert orRollback() {
        configConflictAction(cppObj, ConflictAction.Rollback.ordinal());
        return this;
    }

    public StatementInsert orAbort() {
        configConflictAction(cppObj, ConflictAction.Abort.ordinal());
        return this;
    }

    public StatementInsert orFail() {
        configConflictAction(cppObj, ConflictAction.Fail.ordinal());
        return this;
    }

    public StatementInsert orIgnore() {
        configConflictAction(cppObj, ConflictAction.Ignore.ordinal());
        return this;
    }
    private static native void configConflictAction(long self, int action);

    public StatementInsert as(String alias) {
        configAlias(cppObj, alias);
        return this;
    }

    private static native void configAlias(long self, String alias);

    public StatementInsert column(Column column) {
        if(column == null) {
            return this;
        }
        return columns(new Column[]{column});
    }

    public StatementInsert column(String columnName) {
        if(columnName == null || columnName.length() == 0) {
            return this;
        }
        return columns(new String[]{columnName});
    }

    public StatementInsert columns(Column[] columns) {
        if(columns == null || columns.length == 0) {
            return this;
        }
        long[] cppColumns = new long[columns.length];
        for(int i = 0; i < columns.length; i++) {
            cppColumns[i] = CppObject.get(columns[i]);
        }
        configColumns(cppObj, CPPType.Column.ordinal(), cppColumns, null);
        return this;
    }

    public StatementInsert columns(String[] columnNames) {
        if(columnNames == null || columnNames.length == 0) {
            return this;
        }
        configColumns(cppObj, CPPType.String.ordinal(), null, columnNames);
        return this;
    }

    private native void configColumns(long self, int type, long[] columns, String[] columnNames);

    public StatementInsert valuesWithBindParameters(int parametersCount) {
        configValuesWithBindParameters(cppObj, parametersCount);
        return this;
    }

    private native void configValuesWithBindParameters(long self, int parametersCount);

    public StatementInsert value(Object value) {
        return values(new Object[]{value});
    }

    public StatementInsert values(Object[] values) {
        if(values == null || values.length == 0) {
            return this;
        }

        MultiTypeArray array = new MultiTypeArray(values);

        configValues(cppObj, array.types, array.longValues, array.doubleValues, array.stringValues);
        return this;
    }

    private native void configValues(long self, int[] types, long[] longValues, double[] doubleValues, String[] stringValues);

    public StatementInsert values(StatementSelect select) {
        configValues(cppObj, CppObject.get(select));
        return this;
    }

    private native void configValues(long self, long select);

    public StatementInsert defaultValues() {
        configDefaultValues(cppObj);
        return this;
    }

    private native void configDefaultValues(long self);

    public StatementInsert upsert(Upsert upsert) {
        configUpsert(cppObj, CppObject.get(upsert));
        return this;
    }

    private native void configUpsert(long self, long upsert);
}
