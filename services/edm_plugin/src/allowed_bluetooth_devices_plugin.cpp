/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "allowed_bluetooth_devices_plugin.h"

#include "array_string_serializer.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "edm_errors.h"
#include "edm_ipc_interface_code.h"
#include "parameters.h"
#include "iplugin_manager.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(AllowedBluetoothDevicesPlugin::GetPlugin());
const char *const PERSIST_BLUETOOTH_CONTROL = "persist.edm.prohibit_bluetooth";
const char *const BLUETOOTH_WHITELIST_CHANGED_EVENT = "com.ohos.edm.bluetoothdeviceschanged";

void AllowedBluetoothDevicesPlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<AllowedBluetoothDevicesPlugin, std::vector<std::string>>> ptr)

{
    EDMLOGI("AllowedBluetoothDevicesPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::ALLOWED_BLUETOOTH_DEVICES, PolicyName::POLICY_ALLOWED_BLUETOOTH_DEVICES,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_BLUETOOTH, IPlugin::PermissionType::SUPER_DEVICE_ADMIN, true);
    ptr->SetSerializer(ArrayStringSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&AllowedBluetoothDevicesPlugin::OnBasicSetPolicy, FuncOperateType::SET);
    ptr->SetOnHandlePolicyDoneListener(&AllowedBluetoothDevicesPlugin::OnChangedPolicyDone, FuncOperateType::SET);
    ptr->SetOnHandlePolicyListener(&AllowedBluetoothDevicesPlugin::OnBasicRemovePolicy, FuncOperateType::REMOVE);
    ptr->SetOnHandlePolicyDoneListener(&AllowedBluetoothDevicesPlugin::OnChangedPolicyDone, FuncOperateType::REMOVE);
    ptr->SetOnAdminRemoveDoneListener(&AllowedBluetoothDevicesPlugin::OnAdminRemoveDone);
    maxListSize_ = EdmConstants::BLUETOOTH_LIST_MAX_SIZE;
}

ErrCode AllowedBluetoothDevicesPlugin::SetOtherModulePolicy(const std::vector<std::string> &data, int32_t userId,
    std::vector<std::string> &failedData)
{
    EDMLOGI("AllowedBluetoothDevicesPlugin OnSetPolicy");
    bool isDisabled = system::GetBoolParameter(PERSIST_BLUETOOTH_CONTROL, false);
    if (isDisabled) {
        EDMLOGE("AllowedBluetoothDevicesPlugin OnSetPolicy failed, because bluetooth disabled.");
        return EdmReturnErrCode::CONFIGURATION_CONFLICT_FAILED;
    }
    auto policyManager = IPolicyManager::GetInstance();
    std::string bluetoothDevicesPolicy;
    policyManager->GetPolicy("", PolicyName::POLICY_DISALLOWED_BLUETOOTH_DEVICES, bluetoothDevicesPolicy);
    if (!bluetoothDevicesPolicy.empty()) {
        EDMLOGE("bluetoothDevices policy conflict! Has another bluetoothDevicesPolicy");
        return EdmReturnErrCode::CONFIGURATION_CONFLICT_FAILED;
    }
    return ERR_OK;
}

void AllowedBluetoothDevicesPlugin::OnChangedPolicyDone(bool isGlobalChanged)
{
    if (!isGlobalChanged) {
        return;
    }
    NotifyBluetoothDevicesChanged();
}

void AllowedBluetoothDevicesPlugin::NotifyBluetoothDevicesChanged()
{
    EDMLOGD("AllowedBluetoothDevicesPlugin NotifyBluetoothDevicesChanged.");
    AAFwk::Want want;
    want.SetAction(BLUETOOTH_WHITELIST_CHANGED_EVENT);
    EventFwk::CommonEventData eventData;
    eventData.SetWant(want);
    if (!EventFwk::CommonEventManager::PublishCommonEvent(eventData)) {
        EDMLOGE("NotifyBluetoothDevicesChanged failed.");
    }
}

void AllowedBluetoothDevicesPlugin::OnAdminRemoveDone() {
    EDMLOGI("AllowedBluetoothDevicesPlugin OnAdminRemoveDone NotifyBluetoothDevicesChanged.");
    NotifyBluetoothDevicesChanged();
}

} // namespace EDM
} // namespace OHOS