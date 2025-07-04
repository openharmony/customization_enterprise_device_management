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

#include "get_auto_unlock_after_reboot_query.h"

#include "edm_constants.h"
#include "parameters.h"

namespace OHOS {
namespace EDM {
std::string GetAutoUnlockAfterRebootQuery::GetPolicyName()
{
    return PolicyName::POLICY_SET_AUTO_UNLOCK_AFTER_REBOOT;
}

std::string GetAutoUnlockAfterRebootQuery::GetPermission(IPlugin::PermissionType, const std::string &permissionTag)
{
    return EdmPermission::PERMISSION_ENTERPRISE_MANAGE_SYSTEM;
}

ErrCode GetAutoUnlockAfterRebootQuery::QueryPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    return GetBoolPolicy(policyData, reply);
}
} // namespace EDM
} // namespace OHOS