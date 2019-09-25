//
// Created by sanhuazhang on 2019/05/02
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

#import <WCDB/WCTCommon.h>
#import <WCDB/WCTDatabase.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Trigger when error occurs
 */
typedef void (^WCTErrorTraceBlock)(WCTError *);

/**
 Trigger when a transaction or a normal sql ends.
 */
typedef void (^WCTPerformanceTraceBlock)(NSArray<WCTPerformanceFootprint *> *, double /* cost */);

/**
 Trigger when a SQL is executed.
 */
typedef void (^WCTSQLTraceBlock)(NSString * /* sql */);

@interface WCTDatabase (Monitor)

/**
 @brief You can register a reporter to monitor all errors.
 
 [WCTStatisticsglobalTraceError:^(WCTError* error) {
 NSLog(@"%@", error);
 }];
 
 @param block block
 @see WCTErrorTraceBlock
 */
+ (void)globalTraceError:(nullable WCDB_ESCAPE WCTErrorTraceBlock)block;

/**
 @brief You can register a tracer to monitor the performance of all SQLs.
 It returns 
 1. The collection of SQLs and the executions count of each SQL.
 2. Time consuming in nanoseconds.
 3. Tag of database.
 Note that:
 1. You should register trace before all db operations. 
 2. Global tracer will be recovered by db tracer.
 
 [WCTDatabase globalTracePerformance:^(WCTTag tag, NSDictionary<NSString*, NSNumber*>* sqls, NSInteger cost) {
 NSLog(@"Tag: %d", tag);
 [sqls enumerateKeysAndObjectsUsingBlock:^(NSString *sql, NSNumber *count, BOOL *) {
 NSLog(@"SQL: %@ Count: %d", sql, count.intValue);
 }];
 NSLog(@"Total cost %ld nanoseconds", (long)cost);
 }];
 
 @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
 @param trace trace
 @see WCTPerformanceTraceBlock
 @see [WCTDatabase setNotification:]
 */
+ (void)globalTracePerformance:(nullable WCDB_ESCAPE WCTPerformanceTraceBlock)trace;

/**
 @brief You can register a tracer to monitor the execution of all SQLs.
 It returns a prepared or executed SQL.
 Note that you should register trace before all db operations. 
 
 [WCTDatabase globalTraceSQL:^(NSString* sql) {
 NSLog(@"SQL: %@", sql);
 }];
 
 @warning Tracer may cause wcdb performance degradation, according to your needs to choose whether to open.
 @see WCTSQLTraceBlock
 @param trace trace
 */
+ (void)globalTraceSQL:(nullable WCDB_ESCAPE WCTSQLTraceBlock)trace;

- (void)traceSQL:(nullable WCDB_ESCAPE WCTSQLTraceBlock)trace;

- (void)tracePerformance:(nullable WCDB_ESCAPE WCTPerformanceTraceBlock)trace;

@end

NS_ASSUME_NONNULL_END
