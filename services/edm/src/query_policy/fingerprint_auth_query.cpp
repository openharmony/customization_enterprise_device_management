/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "fingerprint_auth_query.h"

#include "edm_log.h"
#include "fingerprint_policy_serializer.h"

namespace OHOS {
namespace EDM {
std::string FingerprintAuthQuery::GetPolicyName()
{
    return PolicyName::POLICY_FINGERPRINT_AUTH;
}

std::string FingerprintAuthQuery::GetPermission(IPlugin::PermissionType, const std::string &permissionTag)
{
    return EdmPermission::PERMISSION_ENTERPRISE_MANAGE_RESTRICTIONS;
}

ErrCode FingerprintAuthQuery::QueryPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    EDMLOGI("FingerprintAuthQuery OnGetPolicy");
    auto serializer_ = FingerprintPolicySerializer::GetInstance();
    FingerprintPolicy policy;
    serializer_->Deserialize(policyData, policy);
    std::string type = data.ReadString();
    bool isDisallow = false;
    if (type == EdmConstants::FINGERPRINT_AUTH_TYPE) {
        isDisallow = policy.globalDisallow;
    } else if (type == EdmConstants::DISALLOW_FOR_ACCOUNT_TYPE) {
        int32_t accountId = data.ReadInt32();
        auto it = policy.accountIds.find(accountId);
        isDisallow = (it != policy.accountIds.end());
    }
    reply.WriteInt32(ERR_OK);
    reply.WriteBool(isDisallow);
    EDMLOGI("FingerprintAuthPlugin OnGetPolicy result %{public}d", isDisallow);
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS