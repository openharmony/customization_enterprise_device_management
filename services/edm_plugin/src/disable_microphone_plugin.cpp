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

#include "disable_microphone_plugin.h"

#include "audio_system_manager.h"
#include "bool_serializer.h"
#include "edm_ipc_interface_code.h"
#include "parameters.h"
#include "plugin_manager.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = PluginManager::GetInstance()->AddPlugin(DisableMicrophonePlugin::GetPlugin());
const std::string PARAM_EDM_MIC_DISABLE = "persist.edm.mic_disable";
constexpr int32_t AUDIO_SET_MICROPHONE_MUTE_SUCCESS = 0;
constexpr int32_t ERR_PRIVACY_POLICY_CHECK_FAILED = 13100019;
void DisableMicrophonePlugin::InitPlugin(std::shared_ptr<IPluginTemplate<DisableMicrophonePlugin, bool>> ptr)
{
    EDMLOGI("DisableMicrophonePlugin InitPlugin...");
    std::map<IPlugin::PermissionType, std::string> typePermissions;
    typePermissions.emplace(IPlugin::PermissionType::SUPER_DEVICE_ADMIN,
        "ohos.permission.ENTERPRISE_MANAGE_RESTRICTIONS");
    typePermissions.emplace(IPlugin::PermissionType::BYOD_DEVICE_ADMIN,
        "ohos.permission.PERSONAL_MANAGE_RESTRICTIONS");
    IPlugin::PolicyPermissionConfig config = IPlugin::PolicyPermissionConfig(typePermissions, IPlugin::ApiType::PUBLIC);
    ptr->InitAttribute(EdmInterfaceCode::DISABLE_MICROPHONE, "disable_microphone", config, true);
    ptr->SetSerializer(BoolSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisableMicrophonePlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnAdminRemoveListener(&DisableMicrophonePlugin::OnAdminRemove);
}

ErrCode DisableMicrophonePlugin::OnSetPolicy(bool &isDisallow)
{
    EDMLOGI("DisableMicrophonePlugin OnSetPolicy...isDisallow = %{public}d", isDisallow);
    auto audioGroupManager = OHOS::AudioStandard::AudioSystemManager::GetInstance()
        ->GetGroupManager(OHOS::AudioStandard::DEFAULT_VOLUME_GROUP_ID);
    if (audioGroupManager == nullptr) {
        EDMLOGE("DisableMicrophonePlugin audioGroupManager null");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    int32_t ret = audioGroupManager
        ->SetMicrophoneMutePersistent(isDisallow, OHOS::AudioStandard::PolicyType::EDM_POLICY_TYPE);
    if (ret == AUDIO_SET_MICROPHONE_MUTE_SUCCESS || (!isDisallow && ret == ERR_PRIVACY_POLICY_CHECK_FAILED)) {
        std::string disableStr = isDisallow ? "true" : "false";
        system::SetParameter(PARAM_EDM_MIC_DISABLE, disableStr);
        return ERR_OK;
    }
    EDMLOGE("DisableMicrophonePlugin DisableMicrophone result %{public}d", ret);
    return EdmReturnErrCode::SYSTEM_ABNORMALLY;
}

ErrCode DisableMicrophonePlugin::OnAdminRemove(const std::string &adminName, bool &data, int32_t userId)
{
    EDMLOGI("DisableMicrophonePlugin OnAdminRemove %{public}d...", data);
    if (!data) {
        return ERR_OK;
    }
    bool reset = false;
    return OnSetPolicy(reset);
}

ErrCode DisableMicrophonePlugin::OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    bool isMicDisabled = system::GetBoolParameter(PARAM_EDM_MIC_DISABLE, false);
    EDMLOGI("DisableMicrophonePlugin OnGetPolicy isMicDisabled = %{public}d", isMicDisabled);
    reply.WriteInt32(ERR_OK);
    reply.WriteBool(isMicDisabled);
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS
