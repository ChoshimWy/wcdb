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

#import <WCDB/Assertion.hpp>
#import <WCDB/Core.h>
#import <WCDB/SQLite.h>
#import <WCDB/WCTDatabase+Config.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTHandle+Private.h>

static_assert((int) WCTConfigPriorityHigh == (int) WCDB::Configs::Priority::High, "");
static_assert((int) WCTConfigPriorityDefault == (int) WCDB::Configs::Priority::Default, "");
static_assert((int) WCTConfigPriorityLow == (int) WCDB::Configs::Priority::Low, "");

@implementation WCTDatabase (Config)

- (void)setCipherKey:(NSData*)cipherKey
{
    _database->setConfig(WCDB::CipherConfigName,
                         std::shared_ptr<WCDB::Config>(new WCDB::CipherConfig(WCDB::UnsafeData::immutable((const unsigned char*) cipherKey.bytes, (size_t) cipherKey.length), WCDB::CipherConfigDefaultPageSize)),
                         WCDB::Configs::Priority::Highest);
}

- (void)setCipherKey:(NSData*)cipherKey
   andCipherPageSize:(int)cipherPageSize
{
    _database->setConfig(WCDB::CipherConfigName,
                         std::shared_ptr<WCDB::Config>(new WCDB::CipherConfig(WCDB::UnsafeData::immutable((const unsigned char*) cipherKey.bytes, (size_t) cipherKey.length), cipherPageSize)),
                         WCDB::Configs::Priority::Highest);
}

- (void)setConfig:(WCTConfigBlock)nsInvocation
 withUninvocation:(WCTConfigBlock)nsUninvocation
          forName:(NSString*)name
     withPriority:(WCTConfigPriority)priority
{
    WCTRemedialAssert(nsInvocation != nil, "Use [removeConfigForName:] instead.", return;);
    WCDB::CustomConfig::Invocation invocation = [nsInvocation, self](WCDB::Handle* handle) -> bool {
        return nsInvocation([[WCTHandle alloc] initWithDatabase:self andUnsafeHandle:handle]);
    };
    WCDB::CustomConfig::Invocation uninvocation = nullptr;
    if (nsUninvocation != nil) {
        uninvocation = [nsUninvocation, self](WCDB::Handle* handle) -> bool {
            return nsUninvocation([[WCTHandle alloc] initWithDatabase:self andUnsafeHandle:handle]);
        };
    }
    _database->setConfig(name, std::shared_ptr<WCDB::Config>(new WCDB::CustomConfig(invocation, uninvocation)), priority);
}

- (void)setConfig:(WCTConfigBlock)nsInvocation
 withUninvocation:(WCTConfigBlock)nsUninvocation
          forName:(NSString*)name
{
    return [self setConfig:nsInvocation withUninvocation:nsUninvocation forName:name withPriority:WCTConfigPriorityDefault];
}

- (void)removeConfigForName:(NSString*)name
{
    _database->removeConfig(name);
}

@end
