/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SERVICES_EDM_INCLUDE_UTILS_BIT_SERIALIZER_H
#define SERVICES_EDM_INCLUDE_UTILS_BIT_SERIALIZER_H

#include "uint_serializer.h"
#include "ipolicy_serializer.h"
#include "singleton.h"

namespace OHOS {
namespace EDM {
/*
 * Policy data serializer of type uintBIT.
 */
class BitSerializer : public UIntSerializer {
public:
    bool MergePolicy(std::vector<uint32_t> &data, uint32_t &result) override;
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_INCLUDE_UTILS_BIT_SERIALIZER_H
