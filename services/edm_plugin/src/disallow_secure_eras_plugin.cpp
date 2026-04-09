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
#include "disallow_secure_eras_plugin.h"

#include "bool_serializer.h"
#include "edm_ipc_interface_code.h"
#include "parameters.h"
#include "iplugin_manager.h"
 
namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(DisallowSecureErasPlugin::GetPlugin());
 
void DisallowSecureErasPlugin::InitPlugin(std::shared_ptr<IPluginTemplate<DisallowSecureErasPlugin, bool>>
    ptr)
{
    EDMLOGI("DisallowSecureErasPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::POLICY_CODE_END + EdmConstants::PolicyCode::DISABLE_SECURE_ERAS,
        PolicyName::POLICY_DISALLOW_SECURE_ERAS,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_RESTRICTIONS, IPlugin::PermissionType::SUPER_DEVICE_ADMIN, true);
    ptr->SetSerializer(BoolSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisallowSecureErasPlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnAdminRemoveListener(&DisallowSecureErasPlugin::OnAdminRemove);
}
 
ErrCode DisallowSecureErasPlugin::SetOtherModulePolicy(bool data, int32_t userId)
{
    const char* value = data ? "1" : "0";
    if (!system::SetParameter("persist.edm.secure_eras_disable", value)) {
        EDMLOGE("set disallow secure eras: %{public}s failed.", value);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    return ERR_OK;
}

ErrCode DisallowSecureErasPlugin::RemoveOtherModulePolicy(int32_t userId)
{
    if (!system::SetParameter("persist.edm.secure_eras_disable", "0")) {
        EDMLOGE("set disallow secure eras false failed.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS
