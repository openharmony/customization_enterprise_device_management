/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef COMMON_NATIVE_INCLUDE_POLICY_CHANGED_EVENT_H
#define COMMON_NATIVE_INCLUDE_POLICY_CHANGED_EVENT_H

#include <string>
#include <cstdint>

#include "message_parcel.h"

namespace OHOS {
namespace EDM {
class PolicyChangedEvent {
public:
    PolicyChangedEvent() = default;

    PolicyChangedEvent(const std::string &interfaceName, const std::string &adminName, const std::string &parameters,
        int64_t timestamp);

    bool Marshalling(MessageParcel &parcel) const;

    static bool Unmarshalling(MessageParcel &parcel, PolicyChangedEvent &policyChangedEvent);

    void SetPolicyChangedEvent(const std::string &interfaceName, const std::string &adminName,
        const std::string &parameters, int64_t timestamp);

    std::string GetInterfaceName() const;

    std::string GetAdminName() const;

    std::string GetParameters() const;

    int64_t GetTimestamp() const;

private:
    std::string interfaceName_;

    std::string adminName_;

    std::string parameters_;

    int64_t timestamp_ = 0;
};

} // namespace EDM
} // namespace OHOS

#endif // COMMON_NATIVE_INCLUDE_POLICY_CHANGED_EVENT_H
