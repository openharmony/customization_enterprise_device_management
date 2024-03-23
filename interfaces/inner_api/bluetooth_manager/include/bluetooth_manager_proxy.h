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

#ifndef INTERFACES_INNER_API_BLUETOOTH_MANAGER_INCLUDE_BLUETOOTH_MANAGER_PROXY_H
#define INTERFACES_INNER_API_BLUETOOTH_MANAGER_INCLUDE_BLUETOOTH_MANAGER_PROXY_H

#include "enterprise_device_mgr_proxy.h"

namespace OHOS {
namespace EDM {
struct BluetoothInfo {
    std::string name;
    int32_t state = 0;
    int32_t connectionState = 0;
};

class BluetoothManagerProxy {
public:
    static std::shared_ptr<BluetoothManagerProxy> GetBluetoothManagerProxy();
    int32_t GetBluetoothInfo(const AppExecFwk::ElementName &admin, BluetoothInfo &bluetoothInfo);
    int32_t SetBluetoothDisabled(const AppExecFwk::ElementName &admin, bool disabled);
    int32_t IsBluetoothDisabled(const AppExecFwk::ElementName *admin, bool &result);
    int32_t SetBluetoothWhitelist(const AppExecFwk::ElementName &admin, std::vector<std::string> &whitelist);
    int32_t GetBluetoothWhitelist(const AppExecFwk::ElementName &admin, std::vector<std::string> &whitelist);
    int32_t GetBluetoothWhitelist(std::vector<std::string> &whitelist);
    int32_t RemoveBluetoothWhitelist(const AppExecFwk::ElementName &admin, std::vector<std::string> &whitelist);

private:
    static std::shared_ptr<BluetoothManagerProxy> instance_;
    static std::mutex mutexLock_;
};
} // namespace EDM
} // namespace OHOS
#endif // INTERFACES_INNER_API_BLUETOOTH_MANAGER_INCLUDE_BLUETOOTH_MANAGER_PROXY_H