/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef SERVICES_EDM_INCLUDE_QUERY_POLICY_DISABLE_SET_DEVICE_NAME_QUERY_H
#define SERVICES_EDM_INCLUDE_QUERY_POLICY_DISABLE_SET_DEVICE_NAME_QUERY_H

#include "ipolicy_query.h"

namespace OHOS {
namespace EDM {

class DisableSetDeviceNameQuery : public IPolicyQuery {
public:
    ~DisableSetDeviceNameQuery() override = default;

    std::string GetPolicyName() override;

    std::string GetPermission(IPlugin::PermissionType, const std::string &permissionTag) override;

    ErrCode QueryPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply, int32_t userId) override;
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_INCLUDE_QUERY_POLICY_DISABLE_SET_DEVICE_NAME_QUERY_H
