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

#import "CoreConst.h"
#import <TestCase/TestCase.h>
#import <set>
#import <sys/stat.h>
#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
#import <UIKit/UIKit.h>
#endif

@interface ObservationTests : CRUDTestCase

@end

@implementation ObservationTests

- (void)test_corrupted_notification
{
    [self insertPresetObjects];

    TestCaseResult* result = [TestCaseResult no];
    [self.database setNotificationWhenCorrupted:^BOOL(WCTDatabase* database) {
        if (![NSThread isMainThread] && database.isBlockaded) {
            [result makeYES];
        }
        return [self.database removeFiles];
    }];

    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    // trigger corruption
    TestCaseAssertTrue([self.table getObjects] == nil);
    [NSThread sleepForTimeInterval:1.0];
    TestCaseAssertResultYES(result);
}

- (void)test_feature_repeat_corrupted_notify_when_failed
{
    [self insertPresetObjects];

    TestCaseResult* result = [TestCaseResult no];
    [self.database setNotificationWhenCorrupted:^BOOL(WCTDatabase* database) {
        WCDB_UNUSED(database)
        [result makeYES];
        return NO;
    }];

    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);

    // trigger corruption
    TestCaseAssertTrue([self.table getObjects] == nil);
    [NSThread sleepForTimeInterval:1.0];
    TestCaseAssertResultYES(result);
    [result makeNO];

    // trigger corruption
    TestCaseAssertTrue([self.table getObjects] == nil);
    [NSThread sleepForTimeInterval:1.0];
    TestCaseAssertResultNO(result);
    [NSThread sleepForTimeInterval:WCDB::ObservationQueueTimeIntervalForReinvokingCorruptedEvent];
    TestCaseAssertResultYES(result);

    TestCaseAssertTrue([self.database removeFiles]);
}

- (void)test_is_corrupted
{
    [self insertPresetObjects];

    TestCaseAssertFalse([self.database isCorrupted]);
    TestCaseAssertTrue([self tryToMakeHeaderCorrupted]);
    TestCaseAssertFalse([self.database isCorrupted]);

    // trigger corruption
    TestCaseAssertTrue([self.table getObjects] == nil);
    TestCaseAssertTrue([self.database isCorrupted]);

    TestCaseAssertTrue([self.database removeFiles]);
    TestCaseAssertFalse([self.database isCorrupted]);
}

- (void)test_feature_purge_will_not_clear_active_handle
{
    // acquire handle and keep it
    WCTHandle* handle = [self.database getHandle];
    TestCaseAssertTrue([handle execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);

    TestCaseAssertTrue([self.database isOpened]);
    [self.database purge];
    TestCaseAssertTrue([self.database isOpened]);
}

#if TARGET_OS_IPHONE && !TARGET_OS_WATCH
- (void)test_feature_auto_purge_due_to_memory_warning
{
    __block TestCaseResult* tested = [TestCaseResult no];
    [WCTDatabase globalTraceError:^(WCTError* error) {
        TestCaseLog(@"%@", error);
        if (error.level == WCTErrorLevelWarning
            && error.code == WCTErrorCodeWarning) {
            [tested makeYES];
        }
    }];

    TestCaseAssertTrue([self.database canOpen]);
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];

    while (tested.isNO) {
    }

    TestCaseAssertFalse([self.database isOpened]);

    // sleep for other tests
    [NSThread sleepForTimeInterval:WCDB::ObservationQueueTimeIntervalForPurgingAgain];
}

- (void)test_feature_auto_purge_will_not_be_too_frequent
{
    __block TestCaseResult* tested = [TestCaseResult no];
    [WCTDatabase globalTraceError:^(WCTError* error) {
        TestCaseLog(@"%@", error);
        if (error.level == WCTErrorLevelWarning
            && error.code == WCTErrorCodeWarning) {
            [tested makeYES];
        }
    }];

    TestCaseAssertTrue([self.database canOpen]);
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    while (tested.isNO) {
    }
    [tested makeNO];
    TestCaseAssertFalse([self.database isOpened]);

    TestCaseAssertTrue([self.database canOpen]);
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    [NSThread sleepForTimeInterval:2];
    TestCaseAssertResultNO(tested);
    TestCaseAssertTrue([self.database isOpened]);

    [NSThread sleepForTimeInterval:WCDB::ObservationQueueTimeIntervalForPurgingAgain];
    [[NSNotificationCenter defaultCenter] postNotificationName:UIApplicationDidReceiveMemoryWarningNotification object:nil];
    while (tested.isNO) {
    }
    TestCaseAssertFalse([self.database isOpened]);

    // sleep for other tests
    [NSThread sleepForTimeInterval:WCDB::ObservationQueueTimeIntervalForPurgingAgain];
}
#endif

- (void)test_feature_auto_purge_due_to_file_descriptors_warning
{
    int maxAllowedNumberOfFileDescriptors = getdtablesize();
    int numberOfFileDescriptorsForTooManyFileDescriptors = int(maxAllowedNumberOfFileDescriptors * WCDB::ObservationQueueRateForTooManyFileDescriptors);

    NSString* dummy = [self.directory stringByAppendingPathComponent:@"dummy"];
    TestCaseAssertTrue([self.fileManager createFileAtPath:dummy contents:nil attributes:nil]);

    // open files until it meet the threshold
    int numberOfAlreadyOpenedFileDescriptors = 0;
    struct stat stats;
    for (int i = 0; i <= maxAllowedNumberOfFileDescriptors; ++i) {
        int rc = fstat(i, &stats);
        if (rc == 0 || errno != EBADF) {
            ++numberOfAlreadyOpenedFileDescriptors;
        }
    }

    std::set<int> openedFileDescriptor;
    do {
        int fd = open(dummy.UTF8String, O_RDONLY);
        TestCaseAssertTrue(fd >= 0);
        openedFileDescriptor.emplace(fd);
    } while (openedFileDescriptor.size() + numberOfAlreadyOpenedFileDescriptors < numberOfFileDescriptorsForTooManyFileDescriptors);

    // purge will be done after a few moment.
    __block TestCaseResult* tested = [TestCaseResult no];
    [WCTDatabase globalTraceError:^(WCTError* error) {
        TestCaseLog(@"%@", error);
        if (error.level == WCTErrorLevelWarning
            && error.code == WCTErrorCodeWarning) {
            [tested makeYES];
        }
    }];
    TestCaseAssertTrue([self.database canOpen]);
    while (tested.isNO) {
    }

    for (const auto& fd : openedFileDescriptor) {
        TestCaseAssertTrue(close(fd) == 0);
    }

    // sleep for other tests
    [NSThread sleepForTimeInterval:WCDB::ObservationQueueTimeIntervalForPurgingAgain];
}

- (void)test_feature_auto_purge_due_to_too_many_file_descriptors
{
    TestCaseAssertTrue([self.database canOpen]);
    [self.database close];

    NSString* dummy = [self.directory stringByAppendingPathComponent:@"dummy"];
    TestCaseAssertTrue([self.fileManager createFileAtPath:dummy contents:nil attributes:nil]);

    // open files until it failed
    std::set<int> openedFileDescriptor;
    do {
        int fd = open(dummy.UTF8String, O_RDONLY);
        if (fd > 0) {
            openedFileDescriptor.emplace(fd);
        } else {
            TestCaseAssertTrue(errno == EMFILE);
            break;
        }
    } while (YES);

    // purge will be done after a few moment.
    __block TestCaseResult* tested = [TestCaseResult no];
    [WCTDatabase globalTraceError:^(WCTError* error) {
        TestCaseLog(@"%@", error);
        if (error.level == WCTErrorLevelWarning
            && error.code == WCTErrorCodeWarning) {
            [tested makeYES];
        }
    }];
    TestCaseAssertFalse([self.database canOpen]);
    while (tested.isNO) {
    }

    for (const auto& fd : openedFileDescriptor) {
        TestCaseAssertTrue(close(fd) == 0);
    }

    // sleep for other tests
    [NSThread sleepForTimeInterval:WCDB::ObservationQueueTimeIntervalForPurgingAgain];
}

@end
