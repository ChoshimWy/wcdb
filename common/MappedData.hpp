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

#include <WCDB/HighWater.hpp>
#include <WCDB/Recyclable.hpp>
#include <WCDB/SharedThreadedErrorProne.hpp>
#include <WCDB/UnsafeData.hpp>

namespace WCDB {

class MappedData final : public UnsafeData {
public:
    MappedData();
    MappedData(unsigned char* mapped, size_t size);
    MappedData(const MappedData& other);
    MappedData(MappedData&& other);
    MappedData& operator=(const MappedData& other);
    MappedData& operator=(MappedData&& other);
    virtual ~MappedData() = default;

    MappedData subdata(size_t size) const;
    MappedData subdata(off_t offset, size_t size) const;
    void unmap();

    static const MappedData& null();

    static ssize_t getMappedHighWater();

protected:
    static ShareableHighWater& sharedHighWater();

    static void unmapData(UnsafeData& data);
    static void unmapBuffer(unsigned char* buffer, size_t size);

    MappedData(const UnsafeData& data, const Recyclable<UnsafeData>& mapped);
    Recyclable<UnsafeData> m_mapped;
};

} // namespace WCDB
