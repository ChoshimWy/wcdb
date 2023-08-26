// Created by qiuwenchen on 2023/3/31.
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

#pragma once

#include "WCDBJNI.h"

#define WCDBJNIExpressionFuncName(funcName) WCDBJNI(Expression, funcName)
#define WCDBJNIExpressionObjectMethod(funcName, ...)                           \
    WCDBJNIObjectMethod(Expression, funcName, __VA_ARGS__)
#define WCDBJNIExpressionClassMethodWithNoArg(funcName)                        \
    WCDBJNIClassMethodWithNoArg(Expression, funcName)
#define WCDBJNIExpressionClassMethod(funcName, ...)                            \
    WCDBJNIClassMethod(Expression, funcName, __VA_ARGS__)

jlong WCDBJNIExpressionObjectMethod(create, int type, jlong object);

jlong WCDBJNIExpressionClassMethod(createWithFunction, jstring func);
jlong WCDBJNIExpressionClassMethod(createWithExistStatement, jlong select);
jlong WCDBJNIExpressionClassMethod(createWithNotExistStatement, jlong select);

void WCDBJNIExpressionObjectMethod(setWithSchema,
                                   jlong expression,
                                   WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIExpressionObjectMethod(setArgument,
                                   jlong expression,
                                   WCDBJNICommonValueParameter(argument));

void WCDBJNIExpressionObjectMethod(invoke, jlong expression);
void WCDBJNIExpressionObjectMethod(invokeAll, jlong expression);

void WCDBJNIExpressionObjectMethod(distinct, jlong expression);

jlong WCDBJNIExpressionClassMethod(cast, WCDBJNIObjectOrStringParameter(expression));
void WCDBJNIExpressionObjectMethod(as, jlong expression, jint type);

jlong WCDBJNIExpressionClassMethod(caseWithExp, WCDBJNIObjectOrStringParameter(expression));
void WCDBJNIExpressionObjectMethod(setWithWhenExp,
                                   jlong expression,
                                   WCDBJNICommonValueParameter(when));
void WCDBJNIExpressionObjectMethod(setWithThenExp,
                                   jlong expression,
                                   WCDBJNICommonValueParameter(then));
void WCDBJNIExpressionObjectMethod(setWithElseExp,
                                   jlong expression,
                                   WCDBJNICommonValueParameter(else_));

void WCDBJNIExpressionObjectMethod(escapeWith, jlong expression, jstring content);

jlong WCDBJNIExpressionObjectMethod(createWithWindowFunction, jstring func);
void WCDBJNIExpressionObjectMethod(filter, jlong expression, jlong condition);
void WCDBJNIExpressionObjectMethod(overWindowDef, jlong expression, jlong def);
void WCDBJNIExpressionObjectMethod(overWindow, jlong expression, jstring window);