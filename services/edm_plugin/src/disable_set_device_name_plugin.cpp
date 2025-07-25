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
#include "disable_set_device_name_plugin.h"

#include "bool_serializer.h"
#include "edm_constants.h"
#include "edm_ipc_interface_code.h"
#include "edm_log.h"
#include "iplugin_manager.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT =
    IPluginManager::GetInstance()->AddPlugin(DisableSetDeviceNamePlugin::GetPlugin());

void DisableSetDeviceNamePlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<DisableSetDeviceNamePlugin, bool>> ptr)
{
    EDMLOGI("DisableSetDeviceNamePlugin InitPlugin...");
    std::map<IPlugin::PermissionType, std::string> typePermissions;
    typePermissions.emplace(
        IPlugin::PermissionType::SUPER_DEVICE_ADMIN, EdmPermission::PERMISSION_ENTERPRISE_SET_USER_RESTRICTION);
    IPlugin::PolicyPermissionConfig config =
        IPlugin::PolicyPermissionConfig(typePermissions, IPlugin::ApiType::PUBLIC);
    ptr->InitAttribute(EdmInterfaceCode::DISABLE_SET_DEVICE_NAME,
        PolicyName::POLICY_SET_DEVICE_NAME, config, true);
    ptr->SetSerializer(BoolSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisableSetDeviceNamePlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnAdminRemoveListener(&DisableSetDeviceNamePlugin::OnAdminRemove);
    persistParam_ = "persist.edm.set_device_name_disable";
}
} // namespace EDM
} // namespace OHOS
