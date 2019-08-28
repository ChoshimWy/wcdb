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

#import <WCDB/Macro.hpp>
#import <WCDB/WCTCommon.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Close block
 */
typedef void (^WCTCloseBlock)(void);

@interface WCTDatabase : NSObject

@property (atomic, assign) WCTTag tag;

@property (readonly) NSString *path;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;

- (instancetype)initWithPath:(NSString *)path NS_DESIGNATED_INITIALIZER;

- (void)close:(WCDB_NO_ESCAPE WCTCloseBlock)onClosed;

- (WCTError *)error;

@end

NS_ASSUME_NONNULL_END
