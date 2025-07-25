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

#include "allowed_app_distribution_types_query.h"

#include "array_int_serializer.h"
#include "edm_log.h"

namespace OHOS {
namespace EDM {
std::string AllowedAppDistributionTypesQuery::GetPolicyName()
{
    return PolicyName::POLICY_ALLOWED_INSTALL_APP_TYPE;
}

std::string AllowedAppDistributionTypesQuery::GetPermission(IPlugin::PermissionType, const std::string &permissionTag)
{
    return EdmPermission::PERMISSION_ENTERPRISE_SET_BUNDLE_INSTALL_POLICY;
}

ErrCode AllowedAppDistributionTypesQuery::QueryPolicy(std::string &policyData, MessageParcel &data,
    MessageParcel &reply, int32_t userId)
{
    EDMLOGI("AllowedAppDistributionTypesQuery OnGetPolicy");
    std::vector<int32_t> policy;
    ArrayIntSerializer::GetInstance()->Deserialize(policyData, policy);
    reply.WriteInt32(ERR_OK);
    reply.WriteInt32Vector(policy);
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS