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

#import <WCDB/WCTDatabase.h>
#import <WCDB/WCTMigrationInfo.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^WCTMigrationFilterBlock)(WCTMigrationUserInfo*);

typedef void (^WCTMigratedNotificationBlock)(WCTDatabase* _Nonnull, WCTMigrationBaseInfo* _Nullable);

@interface WCTDatabase (Migration)

- (void)filterMigration:(nullable WCDB_ESCAPE WCTMigrationFilterBlock)filter;

- (BOOL)stepMigration;

- (void)enableAutoMigration:(BOOL)flag;

- (void)setNotificationWhenMigrated:(nullable WCDB_ESCAPE WCTMigratedNotificationBlock)onMigrated;

- (BOOL)isMigrated;

@end

NS_ASSUME_NONNULL_END
