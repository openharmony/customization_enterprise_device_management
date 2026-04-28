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

#include "allowed_collaboration_service_bundles_plugin.h"

#include "array_string_serializer.h"
#include "edm_constants.h"
#include "edm_ipc_interface_code.h"
#include "edm_log.h"
#include "iplugin_manager.h"

namespace OHOS {
namespace EDM {

const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(
    AllowedCollaborationServiceBundlesPlugin::GetPlugin());

void AllowedCollaborationServiceBundlesPlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<AllowedCollaborationServiceBundlesPlugin, std::vector<std::string>>> ptr)
{
    EDMLOGI("AllowedCollaborationServiceBundlesPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::ALLOWED_COLLABORATION_SERVICE_BUNDLES,
        PolicyName::POLICY_ALLOWED_COLLABORATION_SERVICE_BUNDLES,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_APPLICATION,
        IPlugin::PermissionType::SUPER_DEVICE_ADMIN, true);
    ptr->SetSerializer(ArrayStringSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&AllowedCollaborationServiceBundlesPlugin::OnBasicSetPolicy, FuncOperateType::SET);
    ptr->SetOnHandlePolicyListener(&AllowedCollaborationServiceBundlesPlugin::OnBasicRemovePolicy,
        FuncOperateType::REMOVE);
    ptr->SetOnAdminRemoveListener(&AllowedCollaborationServiceBundlesPlugin::OnBasicAdminRemove);
    maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    overMaxReturnErrCode_ = EdmReturnErrCode::PARAMETER_VERIFICATION_FAILED;
}

ErrCode AllowedCollaborationServiceBundlesPlugin::SetOtherModulePolicy(const std::vector<std::string> &data,
    int32_t userId, std::vector<std::string> &failedData)
{
    EDMLOGI("AllowedCollaborationServiceBundlesPlugin SetOtherModulePolicy");
    auto policyManager = IPolicyManager::GetInstance();
    std::string policyValue;
    policyManager->GetPolicy("", PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, policyValue, userId);
    if (policyValue != "true") {
        EDMLOGE("AllowedDistributeAbilityConnPlugin disallowed_distributed_transmission is not true.");
        return EdmReturnErrCode::PREREQUISITES_NOT_SATISFIED_FAILED;
    }
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS