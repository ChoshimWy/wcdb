// Created by chenqiuwen on 2023/5/3.
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

#include "DatabaseJNI.h"
#include "DatabaseBridge.h"
#include <assert.h>

#define WCDBJNITryGetDatabaseMethodId(name, signature, action)                        \
    static jmethodID g_methodId = NULL;                                               \
    if (g_methodId == NULL) {                                                         \
        g_methodId                                                                    \
        = (*env)->GetStaticMethodID(env, WCDBJNIGetDatabaseClass(), name, signature); \
        if (g_methodId == NULL) {                                                     \
            assert(0);                                                                \
            action;                                                                   \
        }                                                                             \
    }

jlong WCDBJNIDatabaseObjectMethod(getError, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return (jlong) WCDBDatabaseGetError(selfStruct).innerValue;
}

jlong WCDBJNIDatabaseObjectMethod(getTag, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return (jlong) WCDBDatabaseGetTag(selfStruct);
}

void WCDBJNIDatabaseObjectMethod(setTag, jlong self, jlong tag)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBDatabaseSetTag(selfStruct, tag);
}

jstring WCDBJNIDatabaseObjectMethod(getPath, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNICreateJStringAndReturn(WCDBDatabaseGetPath(selfStruct));
}

typedef struct StringEnumeratorContext {
    JNIEnv* env;
    jobject array;
    jclass arrayClass;
} StringEnumeratorContext;

void WCDBJNIStringEnumerator(StringEnumeratorContext* context, const char* string)
{
    JNIEnv* env = context->env;
    WCDBJNIGetObjectMethodId(g_addMethod, context->arrayClass, "add", "(Ljava/lang/Object;)Z");
    if (g_addMethod == NULL) {
        return;
    }
    WCDBJNICreateJavaString(string);
    (*env)->CallBooleanMethod(env, context->array, g_addMethod, jstring);
}

jobject WCDBJNIDatabaseObjectMethod(getPaths, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNIFindClass(g_arrayClass, "java/util/ArrayList", return NULL);
    WCDBJNIGetObjectMethodId(g_arrayInit, g_arrayClass, "<init>", "()V");
    jobject arrayList = (*env)->NewObject(env, g_arrayClass, g_arrayInit);
    StringEnumeratorContext context;
    context.env = env;
    context.array = arrayList;
    context.arrayClass = g_arrayClass;
    WCDBDatabaseGetPaths2(selfStruct, &context, (WCDBStringEnumerater) WCDBJNIStringEnumerator);
    return arrayList;
}

jlong WCDBJNIDatabaseObjectMethod(getHandle, jlong self, jboolean writeHint)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return (jlong) WCDBDatabaseGetHandle(selfStruct, writeHint).innerValue;
}

jboolean WCDBJNIDatabaseObjectMethod(canOpen, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseCanOpen(selfStruct);
}

jboolean WCDBJNIDatabaseObjectMethod(isOpened, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseIsOpened(selfStruct);
}

jboolean WCDBJNIDatabaseObjectMethod(isBlockaded, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseIsBlockaded(selfStruct);
}

typedef struct CloseDatabaseContext {
    JNIEnv* env;
    jobject callback;
} CloseDatabaseContext;

void WCDBJNIDatabaseCloseCallback(CloseDatabaseContext* context)
{
    JNIEnv* env = context->env;
    WCDBJNITryGetDatabaseMethodId(
    "onClose", "(" WCDBJNIDatabaseSignature "$CloseCallBack;)V", return );
    (*env)->CallStaticVoidMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, context->callback);
}

void WCDBJNIDatabaseObjectMethod(close, jlong self, jobject callback)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    if (callback != NULL) {
        CloseDatabaseContext context;
        context.env = env;
        context.callback = callback;
        WCDBDatabaseClose2(
        selfStruct, &context, (WCDBDatabaseCloseCallback) WCDBJNIDatabaseCloseCallback);
    } else {
        WCDBDatabaseClose2(selfStruct, NULL, NULL);
    }
}

void WCDBJNIDatabaseObjectMethod(blockade, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBDatabaseBlockade(selfStruct);
}

void WCDBJNIDatabaseObjectMethod(unblockade, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBDatabaseUnblockade(selfStruct);
}

void WCDBJNIDatabaseObjectMethod(purge, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBDatabasePurge(selfStruct);
}

void WCDBJNIDatabaseObjectMethod(configCipher, jlong self, jbyteArray cipherKey, jint pageSize, jint cipherVersion)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNIGetByteArrayCritical(cipherKey);
    WCDBDatabaseConfigCipher(
    selfStruct, cipherKeyArray, cipherKeyLength, pageSize, cipherVersion);
    WCDBJNIReleaseByteArrayCritical(cipherKey);
}

bool WCDBJNIDatabaseConfig(jobject config, CPPHandle handle)
{
    WCDBJNITryGetEnvOr(return false);
    WCDBJNITryGetDatabaseMethodId(
    "onConfig", "(J" WCDBJNIDatabaseSignature "$Config;)Z", return false);
    jboolean ret = (*env)->CallStaticBooleanMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, (jlong) handle.innerValue, config);
    if ((*env)->ExceptionCheck(env)) {
        ret = false;
    }
    WCDBJNITryDetach;
    return ret;
}

void WCDBJNIDatabaseObjectMethod(
config, jlong self, jstring name, jobject invocation, jobject unInvocation, jint priority)
{
    WCDBJNITryGetVM;
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNICreateGlobalRel(invocation);
    WCDBJNICreateGlobalRel(unInvocation);
    WCDBJNIGetString(name);
    WCDBDatabaseConfig2(selfStruct,
                        nameString,
                        invocation != NULL ? WCDBJNIDatabaseConfig : NULL,
                        invocation,
                        unInvocation != NULL ? WCDBJNIDatabaseConfig : NULL,
                        unInvocation,
                        priority,
                        WCDBJNIDestructContext);
    WCDBJNIReleaseString(name);
}

void WCDBJNIDatabasePerformanceTrace(jobject tracer,
                                     long tag,
                                     const char* path,
                                     unsigned long long handleId,
                                     const char* sql,
                                     double cost)
{
    WCDBJNITryGetEnvOr(return );
    WCDBJNITryGetDatabaseMethodId("onTracePerformance",
                                  "(" WCDBJNIDatabaseSignature "$PerformanceTracer;J" WCDBJNIStringSignature
                                  "J" WCDBJNIStringSignature "D)V",
                                  return );
    WCDBJNICreateJavaString(path);
    WCDBJNICreateJavaString(sql);
    (*env)->CallStaticVoidMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, tracer, (jlong) tag, jpath, (jlong) handleId, jsql, (jdouble) cost);
    WCDBJNITryDetach;
}

void WCDBJNIDatabaseClassMethod(globalTracePerformance, jobject tracer)
{
    WCDBJNITryGetVM;
    WCDBJNICreateGlobalRel(tracer);
    WCDBDatabaseGlobalTracePerformance2(
    tracer != NULL ? WCDBJNIDatabasePerformanceTrace : NULL, tracer, WCDBJNIDestructContext);
}

void WCDBJNIDatabaseObjectMethod(tracePerformance, jlong self, jobject tracer)
{
    WCDBJNITryGetVM;
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNICreateGlobalRel(tracer);
    WCDBDatabaseTracePerformance2(
    selfStruct, tracer != NULL ? WCDBJNIDatabasePerformanceTrace : NULL, tracer, WCDBJNIDestructContext);
}

void WCDBJNIDatabaseSQLTrace(
jobject tracer, long tag, const char* path, unsigned long long handleId, const char* sql)
{
    WCDBJNITryGetEnvOr(return );
    WCDBJNITryGetDatabaseMethodId("onTraceSQL",
                                  "(" WCDBJNIDatabaseSignature "$SQLTracer;J" WCDBJNIStringSignature
                                  "J" WCDBJNIStringSignature ")V",
                                  return );
    WCDBJNICreateJavaString(path);
    WCDBJNICreateJavaString(sql);
    (*env)->CallStaticVoidMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, tracer, (jlong) tag, jpath, (jlong) handleId, jsql);
    WCDBJNITryDetach;
}

void WCDBJNIDatabaseClassMethod(globalTraceSQL, jobject tracer)
{
    WCDBJNITryGetVM;
    WCDBJNICreateGlobalRel(tracer);
    WCDBDatabaseGlobalTraceSQL2(
    tracer != NULL ? WCDBJNIDatabaseSQLTrace : NULL, tracer, WCDBJNIDestructContext);
}

void WCDBJNIDatabaseObjectMethod(traceSQL, jlong self, jobject tracer)
{
    WCDBJNITryGetVM;
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNICreateGlobalRel(tracer);
    WCDBDatabaseTraceSQL2(
    selfStruct, tracer != NULL ? WCDBJNIDatabaseSQLTrace : NULL, tracer, WCDBJNIDestructContext);
}

void WCDBJNIDatabaseErrorTrace(jobject tracer, CPPError error)
{
    WCDBJNITryGetEnvOr(return );
    WCDBJNITryGetDatabaseMethodId(
    "onTraceException", "(" WCDBJNIDatabaseSignature "$ExceptionTracer;J)V", return );
    (*env)->CallStaticVoidMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, tracer, (jlong) error.innerValue);
    WCDBJNITryDetach;
}

void WCDBJNIDatabaseClassMethod(globalTraceError, jobject tracer)
{
    WCDBJNITryGetVM;
    WCDBJNICreateGlobalRel(tracer);
    WCDBDatabaseGlobalTraceError2(
    tracer != NULL ? WCDBJNIDatabaseErrorTrace : NULL, tracer, WCDBJNIDestructContext);
}

void WCDBJNIDatabaseObjectMethod(traceError, jlong self, jobject tracer)
{
    WCDBJNITryGetVM;
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNICreateGlobalRel(tracer);
    WCDBDatabaseTraceError2(
    selfStruct, tracer != NULL ? WCDBJNIDatabaseErrorTrace : NULL, tracer, WCDBJNIDestructContext);
}

void WCDBJNIDatabaseOperationTrace(jobject tracer, CPPDatabase database, long operation, const void* info)
{
    WCDBJNITryGetEnvOr(return );
    WCDBJNITryGetDatabaseMethodId(
    "onTraceOperation", "(" WCDBJNIDatabaseSignature "$OperationTracer;JIJ)V", return );
    (*env)->CallStaticVoidMethod(env,
                                 WCDBJNIGetDatabaseClass(),
                                 g_methodId,
                                 tracer,
                                 (jlong) database.innerValue,
                                 (jint) operation,
                                 (jlong) info);
    WCDBJNITryDetach;
}

void WCDBJNIDatabaseClassMethod(globalTraceOperation, jobject tracer)
{
    WCDBJNITryGetVM;
    WCDBJNICreateGlobalRel(tracer);
    WCDBDatabaseGlobalTraceOperation(
    (tracer != NULL ? (WCDBOperationTracer) WCDBJNIDatabaseOperationTrace : NULL),
    tracer,
    WCDBJNIDestructContext);
}

typedef struct JNIEnumerateInfoContext {
    JNIEnv* env;
    jobject object;
} JNIEnumerateInfoContext;

void WCDBJNIDatabaseEnumerateInfoCallback(JNIEnumerateInfoContext* context,
                                          const char* key,
                                          CPPCommonValue value)
{
    JNIEnv* env = context->env;
    jlong intValue = 0;
    double doubleValue = 0;
    const char* stringValue = NULL;
    switch (value.type) {
    case WCDBBridgedType_Int:
        intValue = (jlong) value.intValue;
        break;
    case WCDBBridgedType_Double:
        doubleValue = value.doubleValue;
        break;
    case WCDBBridgedType_String:
        stringValue = (const char*) value.intValue;
        break;
    default:
        break;
    }
    WCDBJNITryGetDatabaseMethodId("onEnumerateInfo",
                                  "(Ljava/util/HashMap;" WCDBJNIStringSignature
                                  "IJD" WCDBJNIStringSignature ")V",
                                  return );
    WCDBJNICreateJavaString(key);
    WCDBJNICreateJavaString(stringValue);
    (*env)->CallStaticVoidMethod(env,
                                 WCDBJNIGetDatabaseClass(),
                                 g_methodId,
                                 context->object,
                                 jkey,
                                 (int) value.type,
                                 intValue,
                                 doubleValue,
                                 jstringValue);
}

void WCDBJNIDatabaseClassMethod(enumerateInfo, jobject javaInfo, jlong cppInfo)
{
    JNIEnumerateInfoContext context;
    context.object = javaInfo;
    context.env = env;
    WCDBEnumerateStringViewMap((const void*) cppInfo,
                               (void*) &context,
                               (StringViewMapEnumerator) WCDBJNIDatabaseEnumerateInfoCallback);
}

jboolean WCDBJNIDatabaseObjectMethod(removeFiles, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseRemoveFile(selfStruct);
}

jboolean WCDBJNIDatabaseObjectMethod(moveFile, jlong self, jstring destination)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNIGetString(destination);
    jboolean ret = WCDBDatabaseMoveFile(selfStruct, destinationString);
    WCDBJNIReleaseString(destination);
    return ret;
}

jlong WCDBJNIDatabaseObjectMethod(getFileSize, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    OptionalUInt64 size = WCDBDatabaseGetFileSize(selfStruct);
    return size.hasValue ? size.value : -1;
}

void WCDBJNIDatabaseCorrupted(jobject notification, CPPDatabase database)
{
    WCDBJNITryGetEnvOr(return );
    WCDBJNITryGetDatabaseMethodId(
    "onCorrupted", "(" WCDBJNIDatabaseSignature "$CorruptionNotification;J)V", return );
    (*env)->CallStaticVoidMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, notification, (jlong) database.innerValue);
    WCDBJNITryDetach;
}

void WCDBJNIDatabaseObjectMethod(setNotificationWhenCorrupted, jlong self, jobject notification)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNITryGetVM;
    WCDBJNICreateGlobalRel(notification);
    WCDBDatabaseSetNotificationWhenCorrupted2(
    selfStruct, notification != NULL ? WCDBJNIDatabaseCorrupted : NULL, notification, WCDBJNIDestructContext);
}

jboolean WCDBJNIDatabaseObjectMethod(checkIfCorrupted, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseCheckIfCorrupted(selfStruct);
}

jboolean WCDBJNIDatabaseObjectMethod(checkIfIsAlreadyCorrupted, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseCheckIsAlreadyCorrupted(selfStruct);
}

void WCDBJNIDatabaseObjectMethod(enableAutoBackup, jlong self, jboolean enable)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBDatabaseEnableAutoBackup(selfStruct, enable);
}

jboolean WCDBJNIDatabaseObjectMethod(backup, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseBackup(selfStruct);
}

bool WCDBJNIDatabaseTableShouldBeBackup(jobject filter, const char* table)
{
    WCDBJNITryGetEnvOr(return false);
    WCDBJNITryGetDatabaseMethodId("checkTableShouldBeBackup",
                                  "(" WCDBJNIDatabaseSignature
                                  "$BackupFilter;" WCDBJNIStringSignature ")Z",
                                  return false);
    WCDBJNICreateJavaString(table);
    bool ret = (*env)->CallStaticBooleanMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, filter, jtable);
    if ((*env)->ExceptionCheck(env)) {
        ret = false;
    }
    WCDBJNITryDetach;
    return ret;
}

void WCDBJNIDatabaseObjectMethod(filterBackup, jlong self, jobject tableShouldBeBackup)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNITryGetVM;
    WCDBJNICreateGlobalRel(tableShouldBeBackup);
    WCDBDatabaseFilterBackup2(
    selfStruct,
    tableShouldBeBackup != NULL ? WCDBJNIDatabaseTableShouldBeBackup : NULL,
    tableShouldBeBackup,
    WCDBJNIDestructContext);
}

jboolean WCDBJNIDatabaseObjectMethod(deposit, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseDeposit(selfStruct);
}

jboolean WCDBJNIDatabaseObjectMethod(removeDepositedFiles, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseRemoveDepositedFiles(selfStruct);
}

jboolean WCDBJNIDatabaseObjectMethod(containDepositedFiles, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseContainDepositedFiles(selfStruct);
}

void WCDBJNIDatabaseOnRetrievePorgressUpdate(jobject monitor, double percentage, double increment)
{
    WCDBJNITryGetEnvOr(return );
    WCDBJNITryGetDatabaseMethodId("onRetrieveProgressUpdate",
                                  "(" WCDBJNIDatabaseSignature "$RetrieveProgressMonitor;DD)V",
                                  return );
    (*env)->CallStaticVoidMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, monitor, (jdouble) percentage, (jdouble) increment);
    WCDBJNITryDetach;
}

jdouble WCDBJNIDatabaseObjectMethod(retrieve, jlong self, jobject onProgressUpdate)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNITryGetVM;
    WCDBJNICreateGlobalRel(onProgressUpdate);
    return WCDBDatabaseRetrieve2(selfStruct,
                                 onProgressUpdate != NULL ?
                                 (WCDBRetrieveProgressMonitor) WCDBJNIDatabaseOnRetrievePorgressUpdate :
                                 NULL,
                                 onProgressUpdate,
                                 WCDBJNIDestructContext);
}

jboolean WCDBJNIDatabaseObjectMethod(passiveCheckpoint, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabasePassiveCheckpoint(selfStruct);
}

jboolean WCDBJNIDatabaseObjectMethod(truncateCheckpoint, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseTruncateCheckpoint(selfStruct);
}

void WCDBJNIDatabaseFilterMigrate(jobject filter, const char* table, void* info, WCDBMigrationInfoSetter setter)
{
    WCDBJNITryGetEnvOr(return );
    WCDBJNITryGetDatabaseMethodId("filterMigrate",
                                  "(" WCDBJNIDatabaseSignature
                                  "$MigrationFilter;JJ" WCDBJNIStringSignature ")V",
                                  return );
    WCDBJNICreateJavaString(table);
    (*env)->CallStaticVoidMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, filter, (jlong) setter, (jlong) info, jtable);
    WCDBJNITryDetach;
}

void WCDBJNIDatabaseObjectMethod(
addMigrationSource, jlong self, jstring sourcePath, jbyteArray cipherKey, jobject filter)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNICreateGlobalRel(filter);
    WCDBJNIGetString(sourcePath);
    WCDBJNIGetByteArrayCritical(cipherKey);
    WCDBDatabaseAddMigration(selfStruct,
                             sourcePathString,
                             cipherKeyArray,
                             cipherKeyLength,
                             filter != NULL ? WCDBJNIDatabaseFilterMigrate : NULL,
                             filter,
                             WCDBJNIDestructContext);
    WCDBJNIReleaseByteArrayCritical(cipherKey);
    WCDBJNIReleaseString(sourcePath);
}

void WCDBJNIDatabaseClassMethod(setMigrationInfo, jlong infoSetter, jlong info, jstring sourceTable, jlong filterCondition)
{
    WCDBJNIGetStringCritical(sourceTable);
    WCDBJNIBridgeStruct(CPPExpression, filterCondition);
    ((WCDBMigrationInfoSetter) infoSetter)((void*) info, sourceTableString, filterConditionStruct);
    WCDBJNIReleaseStringCritical(sourceTable);
}

jboolean WCDBJNIDatabaseObjectMethod(stepMigration, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseStepMigration(selfStruct);
}

void WCDBJNIDatabaseObjectMethod(enableAutoMigration, jlong self, jboolean flag)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBDatabaseEnableAutoMigration(selfStruct, flag);
}

void WCDBJNIDatabaseOnTableMigrate(jobject notification,
                                   CPPDatabase database,
                                   const char* table,
                                   const char* sourceTable)
{
    WCDBJNITryGetEnvOr(return );
    WCDBJNITryGetDatabaseMethodId(
    "onTableMigrated",
    "(" WCDBJNIDatabaseSignature
    "$MigrationNotification;J" WCDBJNIStringSignature WCDBJNIStringSignature ")V",
    return );
    WCDBJNICreateJavaString(table);
    WCDBJNICreateJavaString(sourceTable);
    (*env)->CallStaticVoidMethod(
    env, WCDBJNIGetDatabaseClass(), g_methodId, notification, (jlong) database.innerValue, jtable, jsourceTable);
    WCDBJNITryDetach;
}

void WCDBJNIDatabaseObjectMethod(setNotificationWhenMigrated, jlong self, jobject onMigrated)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    WCDBJNITryGetVM;
    WCDBJNICreateGlobalRel(onMigrated);
    WCDBDatabaseSetNotificationWhenMigrated2(
    selfStruct, onMigrated != NULL ? WCDBJNIDatabaseOnTableMigrate : NULL, onMigrated, WCDBJNIDestructContext);
}

jboolean WCDBJNIDatabaseObjectMethod(isMigrated, jlong self)
{
    WCDBJNIBridgeStruct(CPPDatabase, self);
    return WCDBDatabaseIsMigrated(selfStruct);
}
