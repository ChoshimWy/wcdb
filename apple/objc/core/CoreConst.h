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

namespace WCDB {

#pragma mark - Async Queue
static constexpr const double AsyncQueueTimeOutForExiting = 10.0;

#pragma mark - Operation Queue
static constexpr const char* OperationQueueName = "com.Tencent.WCDB.Queue.Operation";
static constexpr double OperationQueueTimeIntervalForRetringAfterFailure = 5.0;
#pragma mark - Operation Queue - Migration
static constexpr const double OperationQueueTimeIntervalForMigration = 2.0;
static constexpr const int OperationQueueTolerableFailuresForMigration = 5;
#pragma mark - Operation Queue - Purge
static constexpr const double OperationQueueTimeIntervalForPurgingAgain = 10.0;
static constexpr const double OperationQueueRateForTooManyFileDescriptors = 0.7;
#pragma mark - Operation Queue - Checkpoint
static constexpr const int OperationQueueFramesThresholdForCriticalCheckpoint
= 4 * 1024 * 1024 / 4096; // 4 MB / Default Page Size = 1024
static constexpr const double OperationQueueTimeIntervalForCriticalCheckpoint = 0.0;
static constexpr const double OperationQueueTimeIntervalForNonCriticalCheckpoint = 30.0;
#pragma mark - Operation Queue - Backup
static constexpr const double OperationQueueTimeIntervalForBackup = 1.0;

#pragma mark - Config - Auto Checkpoint
static constexpr const char* AutoCheckpointConfigName = "com.Tencent.WCDB.Config.AutoCheckpoint";
#pragma mark - Config - Auto Backup
static constexpr const char* AutoBackupConfigName = "com.Tencent.WCDB.Config.AutoBackup";
#pragma mark - Config - Auto Migrate
static constexpr const char* AutoMigrateConfigName = "com.Tencent.WCDB.Config.AutoMigrate";
#pragma mark - Config - Basic
static constexpr const char* BasicConfigName = "com.Tencent.WCDB.Config.Basic";
static constexpr const int BasicConfigBusyRetryMaxAllowedNumberOfTimes = 3;
#pragma mark - Config - Busy Retry
static constexpr const char* BusyRetryConfigName = "com.Tencent.WCDB.Config.BusyRetry";
static constexpr const double BusyRetryTimeOutForMainThread = 2.0;
static constexpr const double BusyRetryTimeOutForSubThread = 10.0;
#pragma mark - Config - Cipher
static constexpr const char* CipherConfigName = "com.Tencent.WCDB.Config.Cipher";
static constexpr const int CipherConfigDefaultPageSize = 4096;
#pragma mark - Config - Global SQL Trace
static constexpr const char* GlobalSQLTraceConfigName = "com.Tencent.WCDB.Config.GlobalSQLTrace";
#pragma mark - Config - Global Performance Trace
static constexpr const char* GlobalPerformanceTraceConfigName
= "com.Tencent.WCDB.Config.GlobalPerformanceTrace";
#pragma mark - Config - Performance Trace
static constexpr const char* PerformanceTraceConfigName
= "com.Tencent.WCDB.Config.PerformanceTrace";
#pragma mark - Config - SQL Trace
static constexpr const char* SQLTraceConfigName = "com.Tencent.WCDB.Config.SQLTrace";
#pragma mark - Config - Tokenize
static constexpr const char* TokenizeConfigPrefix = "com.Tencent.WCDB.Config.Tokenize.";

#pragma mark - Notifier
static constexpr const char* NotifierPreprocessorName = "com.Tencent.WCDB.Notifier.PreprocessTag";
static constexpr const char* NotifierLoggerName = "com.Tencent.WCDB.Notifier.Log";

#pragma mark - Handle Pool
static constexpr const int HandlePoolMaxAllowedNumberOfHandles = 32;
static constexpr const unsigned int HandlePoolNumberOfSlots = 8;

#pragma mark - Migrate
static constexpr const double MigrateMaxExpectingDuration = 0.01;
static constexpr const double MigrateMaxInitializeDuration = 0.005;

#pragma mark - Error - Key
static constexpr const char* ErrorStringKeyAction = "Action";

#define WCDB_ERROR_STRING_KEY_SOURCE "Source";
static constexpr const char* ErrorStringKeySource = WCDB_ERROR_STRING_KEY_SOURCE;

#define WCDB_ERROR_STRING_KEY_PATH "Path"
static constexpr const char* ErrorStringKeyPath = WCDB_ERROR_STRING_KEY_PATH;

#define WCDB_ERROR_STRING_KEY_SQL "SQL"
static constexpr const char* ErrorStringKeySQL = WCDB_ERROR_STRING_KEY_SQL;

#define WCDB_ERROR_INT_KEY_TAG "Tag"
static constexpr const char* ErrorIntKeyTag = WCDB_ERROR_INT_KEY_TAG;

#define WCDB_ERROR_INT_KEY_EXTCODE "ExtCode"
static constexpr const char* ErrorIntKeyExtCode = WCDB_ERROR_INT_KEY_EXTCODE;

#pragma mark - Error - Action
static constexpr const char* ErrorActionMigrate = "Migrate";
static constexpr const char* ErrorActionBackup = "Backup";
static constexpr const char* ErrorActionCheckpoint = "Checkpoint";
static constexpr const char* ErrorActionIntegrity = "Integrity";
static constexpr const char* ErrorActionAssembler = "Assembler";

#pragma mark - Error - Source
static constexpr const char* ErrorSourceSQLite = "SQLite";
static constexpr const char* ErrorSourceRepair = "Repair";
static constexpr const char* ErrorSourceSystem = "System";
static constexpr const char* ErrorSourceAssertion = "Assertion";
static constexpr const char* ErrorSourceTrace = "Trace";
static constexpr const char* ErrorSourceNative = "Native";

#pragma mark - Tag
static constexpr const int TagInvalidValue = 0;

#pragma mark - Constraint
static_assert(OperationQueueTimeIntervalForMigration > OperationQueueTimeIntervalForCriticalCheckpoint,
              "");
static_assert(OperationQueueTimeIntervalForMigration > MigrateMaxExpectingDuration, "");

} // namespace WCDB
