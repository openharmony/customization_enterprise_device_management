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

#include "disallowed_device_sudo_plugin.h"

#include "bool_serializer.h"
#include "edm_ipc_interface_code.h"
#include "iplugin_manager.h"
#include "parameters.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(DisallowedDeviceSudoPlugin::GetPlugin());

void DisallowedDeviceSudoPlugin::InitPlugin(std::shared_ptr<IPluginTemplate<DisallowedDeviceSudoPlugin, bool>> ptr)
{
    EDMLOGI("DisallowedDeviceSudoPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::DISALLOWED_DEVICE_SUDO, PolicyName::POLICY_DISALLOWED_DEVICE_SUDO,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_RESTRICTIONS, IPlugin::PermissionType::SUPER_DEVICE_ADMIN, true);
    ptr->SetSerializer(BoolSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisallowedDeviceSudoPlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnAdminRemoveListener(&DisallowedDeviceSudoPlugin::OnAdminRemove);
    persistParam_ = "persist.edm.sudo_disable";
}

ErrCode DisallowedDeviceSudoPlugin::CheckConflictPolicy(bool data, int32_t userId)
{
    std::string policyData;
    IPolicyManager::GetInstance()->GetPolicy("", PolicyName::POLICY_DISABLED_SUDO, policyData, userId);
    return policyData == TRUE_VALUE ? EdmReturnErrCode::CONFIGURATION_CONFLICT_FAILED : ERR_OK;
}
} // namespace EDM
} // namespace OHOS
