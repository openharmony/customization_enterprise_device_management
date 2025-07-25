/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "lock_screen_plugin.h"
#include "screenlock_manager.h"
#include "edm_ipc_interface_code.h"
#include "int_serializer.h"
#include "iplugin_manager.h"

namespace OHOS {
namespace EDM {

const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(LockScreenPlugin::GetPlugin());

void LockScreenPlugin::InitPlugin(std::shared_ptr<IPluginTemplate<LockScreenPlugin, int32_t>> ptr)
{
    EDMLOGI("LockScreenPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::LOCK_SCREEN, PolicyName::POLICY_LOCK_SCREEN,
        EdmPermission::PERMISSION_ENTERPRISE_LOCK_DEVICE, IPlugin::PermissionType::SUPER_DEVICE_ADMIN, false);
    ptr->SetSerializer(IntSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&LockScreenPlugin::OnSetPolicy, FuncOperateType::SET);
}

ErrCode LockScreenPlugin::OnSetPolicy(int32_t &userId)
{
    EDMLOGD("LockScreenPlugin OnSetPolicy.");
    int32_t ret = ScreenLock::ScreenLockManager::GetInstance()->Lock(userId);
    if (ret != ScreenLock::E_SCREENLOCK_OK) {
        EDMLOGE("LockScreenPlugin:OnSetPolicy send request fail. %{public}d", ret);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS
