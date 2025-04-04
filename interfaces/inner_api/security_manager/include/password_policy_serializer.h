/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNER_API_SECURITY_MANAGER_INCLUDE_PASSWORD_POLICY_SERIALIZER_H
#define INTERFACES_INNER_API_SECURITY_MANAGER_INCLUDE_PASSWORD_POLICY_SERIALIZER_H

#include "ipolicy_serializer.h"
#include "password_policy.h"

namespace OHOS {
namespace EDM {

/*
 * Policy data serializer of type int.
 */
class PasswordSerializer : public IPolicySerializer<PasswordPolicy>, public DelayedSingleton<PasswordSerializer> {
public:
    bool Deserialize(const std::string &policy, PasswordPolicy &dataObj) override;

    bool Serialize(const PasswordPolicy &dataObj, std::string &policy) override;

    bool GetPolicy(MessageParcel &data, PasswordPolicy &result) override;

    bool WritePolicy(MessageParcel &reply, PasswordPolicy &result) override;

    bool MergePolicy(std::vector<PasswordPolicy> &data, PasswordPolicy &result) override;
};
} // namespace EDM
} // namespace OHOS

#endif // INTERFACES_INNER_API_SECURITY_MANAGER_INCLUDE_PASSWORD_POLICY_SERIALIZER_H
