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

#include "disallow_distributed_transmission_full_plugin.h"

#include <ipc_skeleton.h>
#include "bool_serializer.h"
#include "edm_constants.h"
#include "edm_errors.h"
#include "edm_ipc_interface_code.h"
#include "edm_log.h"
#include "iplugin_manager.h"
#include "ipolicy_manager.h"
#include "os_account_manager.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(
    DisallowDistributedTransmissionFullPlugin::GetPlugin());
const std::string CONSTRAINT_DISTRIBUTED_TRANSMISSION = "constraint.distributed.transmission";

void DisallowDistributedTransmissionFullPlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<DisallowDistributedTransmissionFullPlugin, bool>> ptr)
{
    EDMLOGI("DisallowDistributedTransmissionFullPlugin InitPlugin...");
    ptr->InitAttribute(
        EdmInterfaceCode::DISALLOWED_DISTRIBUTED_TRANSMISSION_FULL,
        PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION_FULL,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_RESTRICTIONS,
        IPlugin::PermissionType::SUPER_DEVICE_ADMIN,
        true);
    ptr->SetSerializer(BoolSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisallowDistributedTransmissionFullPlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnAdminRemoveListener(&DisallowDistributedTransmissionFullPlugin::OnAdminRemove);
}

ErrCode DisallowDistributedTransmissionFullPlugin::OnSetPolicy(
    bool &data, bool &currentData, bool &mergeData, int32_t userId)
{
    EDMLOGI("DisallowDistributedTransmissionFullPlugin::OnSetPolicy, data: %{public}d, currentData: %{public}d, "
            "mergeData: %{public}d", data, currentData, mergeData);
    if (mergeData) {
        currentData = data;
        return ERR_OK;
    }
    if (HasConflictPolicy(userId)) {
        EDMLOGE("DisallowDistributedTransmissionFullPlugin::OnSetPolicy HasConflictPolicy failed");
        return EdmReturnErrCode::CONFIGURATION_CONFLICT_FAILED;
    }
    int32_t ret = SetDistributedTransmissionFullPolicy(data, userId);
    if (FAILED(ret)) {
        EDMLOGE("DisallowDistributedTransmissionFullPlugin::OnSetPolicy Failed, ret: %{public}d", ret);
        return EdmReturnErrCode::PARAMETER_VERIFICATION_FAILED;
    }
    currentData = data;
    mergeData = data;
    return ERR_OK;
}

ErrCode DisallowDistributedTransmissionFullPlugin::OnAdminRemove(
    const std::string &adminName, bool &data, bool &mergeData, int32_t userId)
{
    EDMLOGI("DisallowDistributedTransmissionFullPlugin::OnAdminRemove, adminName: %{public}s, data: %{public}d, "
            "mergeData: %{public}d", adminName.c_str(), data, mergeData);
    if (mergeData) {
        return ERR_OK;
    }
    if (data) {
        ErrCode ret = SetDistributedTransmissionFullPolicy(false, userId);
        if (FAILED(ret)) {
            EDMLOGE("DisallowDistributedTransmissionFullPlugin::OnAdminRemove Failed, ret: %{public}d", ret);
        }
    }
    return ERR_OK;
}

ErrCode DisallowDistributedTransmissionFullPlugin::SetDistributedTransmissionFullPolicy(bool policy, int32_t userId)
{
    EDMLOGI("DisallowDistributedTransmissionFullPlugin::SetDistributedTransmissionFullPolicy, "
            "policy: %{public}d", policy);
    std::vector<std::string> constraints;
    constraints.emplace_back(CONSTRAINT_DISTRIBUTED_TRANSMISSION);
    ErrCode ret = AccountSA::OsAccountManager::SetSpecificOsAccountConstraints(
        constraints, policy, userId, EdmConstants::DEFAULT_USER_ID, true);
    EDMLOGI("DisallowDistributedTransmissionFullPlugin SetSpecificOsAccountConstraints ret: %{public}d", ret);
    return ret;
}

bool DisallowDistributedTransmissionFullPlugin::HasConflictPolicy(int32_t userId)
{
    auto policyManager = IPolicyManager::GetInstance();
    std::string policyValue;
    policyManager->GetPolicy("", PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, policyValue, userId);
    if (!policyValue.empty()) {
        EDMLOGE("DisallowDistributedTransmissionFullPlugin::HasConflictPolicy policy conflict! "
                 "disallow_distributed_transmission is already set");
        return true;
    }
    return false;
}
} // namespace EDM
} // namespace OHOS
