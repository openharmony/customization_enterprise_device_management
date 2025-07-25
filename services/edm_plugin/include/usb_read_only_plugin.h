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

#ifndef SERVICES_EDM_PLUGIN_INCLUDE_USB_READ_ONLY_PLUGIN_H
#define SERVICES_EDM_PLUGIN_INCLUDE_USB_READ_ONLY_PLUGIN_H

#include "plugin_singleton.h"
#include "istorage_manager.h"
#include "usb_interface_type.h"

namespace OHOS {
namespace EDM {
class UsbReadOnlyPlugin : public IPlugin {
public:
    UsbReadOnlyPlugin();

    ErrCode OnHandlePolicy(std::uint32_t funcCode, MessageParcel &data, MessageParcel &reply,
        HandlePolicyData &policyData, int32_t userId) override;
    void OnHandlePolicyDone(std::uint32_t funcCode, const std::string &adminName, bool isGlobalChanged,
        int32_t userId) override{};
    ErrCode OnAdminRemove(const std::string &adminName, const std::string &policyData,
        const std::string &mergeData, int32_t userId) override;
    void OnAdminRemoveDone(const std::string &adminName, const std::string &currentJsonData, int32_t userId) override{};
    ErrCode OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply, int32_t userId) override;
    ErrCode GetOthersMergePolicyData(const std::string &adminName, int32_t userId,
        std::string &othersMergePolicyData) override;

private:
    ErrCode SetUsbStorageAccessPolicy(int32_t accessPolicy, int32_t userId);
    ErrCode HasConflictPolicy(int32_t accessPolicy, const std::string &allowUsbDevice, bool &hasConflict);
    void GetDisallowedUsbDeviceTypes(std::vector<USB::UsbDeviceType> &usbDeviceTypes);
    ErrCode DealDisablePolicy(std::vector<USB::UsbDeviceType> usbDeviceTypes);
    ErrCode DealReadPolicy(int32_t accessPolicy, const std::string &allowUsbDevice,
        std::vector<USB::UsbDeviceType> usbDeviceTypes);
    OHOS::sptr<OHOS::StorageManager::IStorageManager> GetStorageManager();
    ErrCode ReloadUsbDevice();
    bool IsStorageDisabledByDisallowedPolicy();
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_PLUGIN_INCLUDE_USB_READ_ONLY_PLUGIN_H
