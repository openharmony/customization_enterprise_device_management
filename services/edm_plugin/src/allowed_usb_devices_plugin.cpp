/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "allowed_usb_devices_plugin.h"

#include <algorithm>
#include <system_ability_definition.h>
#include "array_usb_device_id_serializer.h"
#include "edm_constants.h"
#include "edm_ipc_interface_code.h"
#include "edm_sys_manager.h"
#include "edm_utils.h"
#include "usb_device.h"
#include "usb_device_id.h"
#include "usb_policy_utils.h"
#include "usb_srv_client.h"
#include "plugin_manager.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = PluginManager::GetInstance()->AddPlugin(AllowUsbDevicesPlugin::GetPlugin());

void AllowUsbDevicesPlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<AllowUsbDevicesPlugin, std::vector<UsbDeviceId>>> ptr)
{
    EDMLOGI("AllowUsbDevicesPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::ALLOWED_USB_DEVICES, "allowed_usb_devices",
        "ohos.permission.ENTERPRISE_MANAGE_USB", IPlugin::PermissionType::SUPER_DEVICE_ADMIN, true);
    ptr->SetSerializer(ArrayUsbDeviceIdSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&AllowUsbDevicesPlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnHandlePolicyListener(&AllowUsbDevicesPlugin::OnRemovePolicy, FuncOperateType::REMOVE);
    ptr->SetOnAdminRemoveListener(&AllowUsbDevicesPlugin::OnAdminRemove);
}

ErrCode AllowUsbDevicesPlugin::OnSetPolicy(std::vector<UsbDeviceId> &data,
    std::vector<UsbDeviceId> &currentData, int32_t userId)
{
    EDMLOGI("AllowUsbDevicesPlugin OnSetPolicy userId = %{public}d", userId);
    if (data.empty()) {
        EDMLOGW("AllowUsbDevicesPlugin OnSetPolicy data is empty");
        return ERR_OK;
    }
    if (data.size() > EdmConstants::ALLOWED_USB_DEVICES_MAX_SIZE) {
        EDMLOGE("AllowUsbDevicesPlugin OnSetPolicy data size=[%{public}zu] is too large", data.size());
        return EdmReturnErrCode::PARAM_ERROR;
    }
    if (HasConflictPolicy()) {
        return EdmReturnErrCode::CONFIGURATION_CONFLICT_FAILED;
    }

    std::vector<UsbDeviceId> mergeData = ArrayUsbDeviceIdSerializer::GetInstance()->SetUnionPolicyData(data,
        currentData);
    if (mergeData.size() > EdmConstants::ALLOWED_USB_DEVICES_MAX_SIZE) {
        EDMLOGE("AllowUsbDevicesPlugin OnSetPolicy union data size=[%{public}zu] is too large", mergeData.size());
        return EdmReturnErrCode::PARAM_ERROR;
    }
    ErrCode errCode = UsbPolicyUtils::AddAllowedUsbDevices(mergeData);
    if (errCode != ERR_OK) {
        return errCode;
    }
    currentData = mergeData;
    return ERR_OK;
}

bool AllowUsbDevicesPlugin::HasConflictPolicy()
{
    auto policyManager = IPolicyManager::GetInstance();
    std::string disableUsbPolicy;
    policyManager->GetPolicy("", "disable_usb", disableUsbPolicy);
    if (disableUsbPolicy == "true") {
        EDMLOGE("AllowUsbDevicesPlugin POLICY CONFLICT! Usb is disabled.");
        return true;
    }

    std::string usbStoragePolicy;
    policyManager->GetPolicy("", "usb_read_only", usbStoragePolicy);
    if (usbStoragePolicy == std::to_string(EdmConstants::STORAGE_USB_POLICY_DISABLED)) {
        EDMLOGE("AllowUsbDevicesPlugin POLICY CONFLICT! usbStoragePolicy is disabled.");
        return true;
    }

    std::string disallowUsbDevicePolicy;
    policyManager->GetPolicy("", "disallowed_usb_devices", disallowUsbDevicePolicy);
    if (!disallowUsbDevicePolicy.empty()) {
        EDMLOGE("AllowUsbDevicesPlugin POLICY CONFLICT! disallowedUsbDevice: %{public}s",
            disallowUsbDevicePolicy.c_str());
        return true;
    }
    return false;
}

ErrCode AllowUsbDevicesPlugin::OnRemovePolicy(std::vector<UsbDeviceId> &data,
    std::vector<UsbDeviceId> &currentData, int32_t userId)
{
    EDMLOGD("AllowUsbDevicesPlugin OnRemovePolicy userId : %{public}d:", userId);
    if (data.empty()) {
        EDMLOGW("AllowUsbDevicesPlugin OnRemovePolicy data is empty:");
        return ERR_OK;
    }
    if (data.size() > EdmConstants::ALLOWED_USB_DEVICES_MAX_SIZE) {
        EDMLOGE("AllowUsbDevicesPlugin OnRemovePolicy input data is too large");
        return EdmReturnErrCode::PARAM_ERROR;
    }
    std::vector<UsbDeviceId> mergeData =
        ArrayUsbDeviceIdSerializer::GetInstance()->SetDifferencePolicyData(data, currentData);
    if (mergeData.empty()) {
        auto &srvClient = OHOS::USB::UsbSrvClient::GetInstance();
        std::vector<OHOS::USB::UsbDevice> allDevices;
        int32_t getRet = srvClient.GetDevices(allDevices);
        if (getRet == EdmConstants::USB_ERRCODE_INTERFACE_NO_INIT) {
            EDMLOGW("AllowUsbDevicesPlugin OnRemovePolicy: getDevices failed! USB interface not init!");
        }
        if (getRet != ERR_OK && getRet != EdmConstants::USB_ERRCODE_INTERFACE_NO_INIT) {
            EDMLOGE("AllowUsbDevicesPlugin OnRemovePolicy getDevices failed: %{public}d", getRet);
            return EdmReturnErrCode::SYSTEM_ABNORMALLY;
        }
        EDMLOGI("AllowUsbDevicesPlugin OnRemovePolicy: clear to empty, enable all.");
        std::for_each(allDevices.begin(), allDevices.end(), [&](const auto usbDevice) {
            if (srvClient.ManageDevice(usbDevice.GetVendorId(), usbDevice.GetProductId(), false) != ERR_OK) {
                EDMLOGW("AllowUsbDevicesPlugin OnRemovePolicy ManageDevice vid: %{public}d, pid: %{public}d failed!",
                    usbDevice.GetVendorId(), usbDevice.GetProductId());
            }
        });
        currentData = mergeData;
        return ERR_OK;
    }
    EDMLOGI("AllowUsbDevicesPlugin OnRemovePolicy: remove data size: %{public}zu", data.size());
    auto &srvClient = OHOS::USB::UsbSrvClient::GetInstance();
    std::for_each(data.begin(), data.end(), [&](const auto usbDeviceId) {
        if (srvClient.ManageDevice(usbDeviceId.GetVendorId(), usbDeviceId.GetProductId(), true) != ERR_OK) {
            EDMLOGW("AllowUsbDevicesPlugin OnRemovePolicy ManageDevice vid: %{public}d, pid: %{public}d failed!",
                usbDeviceId.GetVendorId(), usbDeviceId.GetProductId());
        }
    });
    currentData = mergeData;
    return ERR_OK;
}

ErrCode AllowUsbDevicesPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    EDMLOGI("AllowUsbDevicesPlugin OnGetPolicy policyData : %{public}s, userId : %{public}d",
        policyData.c_str(), userId);
    std::vector<UsbDeviceId> usbDeviceIds;
    ArrayUsbDeviceIdSerializer::GetInstance()->Deserialize(policyData, usbDeviceIds);
    reply.WriteInt32(ERR_OK);
    reply.WriteUint32(usbDeviceIds.size());
    for (const auto &usbDeviceId : usbDeviceIds) {
        EDMLOGD("AllowUsbDevicesPlugin OnGetPolicy: currentData: vid: %{public}d, pid: %{public}d",
            usbDeviceId.GetVendorId(), usbDeviceId.GetProductId());
        if (!usbDeviceId.Marshalling(reply)) {
            EDMLOGE("AllowUsbDevicesPlugin OnGetPolicy: write parcel failed!");
            return EdmReturnErrCode::SYSTEM_ABNORMALLY;
        }
    }
    return ERR_OK;
}

ErrCode AllowUsbDevicesPlugin::OnAdminRemove(const std::string &adminName, std::vector<UsbDeviceId> &data,
    int32_t userId)
{
    EDMLOGD("AllowUsbDevicesPlugin OnAdminRemove");
    if (data.empty()) {
        EDMLOGW("AllowUsbDevicesPlugin OnRemovePolicy data is empty:");
        return ERR_OK;
    }
    return UsbPolicyUtils::SetUsbDisabled(false);
}
} // namespace EDM
} // namespace OHOS
