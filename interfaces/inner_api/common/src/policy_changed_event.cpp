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

#include "policy_changed_event.h"

#include "parcel_macro.h"

namespace OHOS {
namespace EDM {
PolicyChangedEvent::PolicyChangedEvent(const std::string &interfaceName, const std::string &adminName,
    const std::string &parameters, int64_t timestamp): interfaceName_(interfaceName), adminName_(adminName),
    parameters_(parameters), timestamp_(timestamp) {}

bool PolicyChangedEvent::Marshalling(MessageParcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, interfaceName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, adminName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, parameters_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int64, parcel, timestamp_);
    return true;
}

bool PolicyChangedEvent::Unmarshalling(MessageParcel &parcel, PolicyChangedEvent &event)
{
    std::string interfaceName = parcel.ReadString();
    std::string adminName = parcel.ReadString();
    std::string parameters = parcel.ReadString();
    int64_t timestamp = parcel.ReadInt64();
    event.SetPolicyChangedEvent(interfaceName, adminName, parameters, timestamp);
    return true;
}

void PolicyChangedEvent::SetPolicyChangedEvent(const std::string &interfaceName, const std::string &adminName,
    const std::string &parameters, int64_t timestamp)
{
    interfaceName_ = interfaceName;
    adminName_ = adminName;
    parameters_ = parameters;
    timestamp_ = timestamp;
}

std::string PolicyChangedEvent::GetInterfaceName() const
{
    return interfaceName_;
}

std::string PolicyChangedEvent::GetAdminName() const
{
    return adminName_;
}

std::string PolicyChangedEvent::GetParameters() const
{
    return parameters_;
}

int64_t PolicyChangedEvent::GetTimestamp() const
{
    return timestamp_;
}
} // namespace EDM
} // namespace OHOS
