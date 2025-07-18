/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "utils.h"
#include "parameters.h"

namespace OHOS {
namespace EDM {
namespace TEST {
uint64_t Utils::selfTokenId_ = 0;
const std::string SET_EDM_SERVICE = "persist.edm.edm_enable";
const std::string EDM_MANAGE_DATETIME_PERMISSION = "ohos.permission.SET_TIME";
const std::string EDM_FACTORY_RESET_PERMISSION = "ohos.permission.FACTORY_RESET";
const std::string EDM_MANAGE_LOCAL_ACCOUNTS_PERMISSION = "ohos.permission.MANAGE_LOCAL_ACCOUNTS";
const std::string EDM_MANAGE_SECURE_SETTINGS_PERMISSION = "ohos.permission.MANAGE_SECURE_SETTINGS";
const std::string PERMISSION_ENTERPRISE_SUBSCRIBE_MANAGED_EVENT = "ohos.permission.ENTERPRISE_SUBSCRIBE_MANAGED_EVENT";
const std::string PERMISSION_MANAGE_ENTERPRISE_DEVICE_ADMIN = "ohos.permission.MANAGE_ENTERPRISE_DEVICE_ADMIN";
const std::string PERMISSION_SET_ENTERPRISE_INFO = "ohos.permission.SET_ENTERPRISE_INFO";
const std::string PERMISSION_ACCESS_CERT_MANAGER_INTERNAL = "ohos.permission.ACCESS_CERT_MANAGER_INTERNAL";
const std::string PERMISSION_ACCESS_CERT_MANAGER = "ohos.permission.ACCESS_CERT_MANAGER";
const std::string PERMISSION_ACCESS_BLUETOOTH = "ohos.permission.ACCESS_BLUETOOTH";
const std::string PERMISSION_MANAGE_BLUETOOTH = "ohos.permission.MANAGE_BLUETOOTH";
const std::string PERMISSION_NETSYS_INTERNAL = "ohos.permission.NETSYS_INTERNAL";
const std::string PERMISSION_INSTALL_ENTERPRISE_NORMAL_BUNDLE = "ohos.permission.INSTALL_ENTERPRISE_NORMAL_BUNDLE";
const std::string PERMISSION_INSTALL_ENTERPRISE_MDM_BUNDLE = "ohos.permission.INSTALL_ENTERPRISE_MDM_BUNDLE";
const std::string PERMISSION_INSTALL_SELF_BUNDLE = "ohos.permission.INSTALL_SELF_BUNDLE";
const std::string PERMISSION_UNINSTALL_BUNDLE = "ohos.permission.UNINSTALL_BUNDLE";
const std::string PERMISSION_RUNNING_STATE_OBSERVER = "ohos.permission.RUNNING_STATE_OBSERVER";
const std::string PERMISSION_ENTERPRISE_MANAGE_SECURITY = "ohos.permission.ENTERPRISE_MANAGE_SECURITY";
const std::string PERMISSION_ENTERPRISE_MANAGE_BLUETOOTH = "ohos.permission.ENTERPRISE_MANAGE_BLUETOOTH";
const std::string PERMISSION_ENTERPRISE_MANAGE_LOCATION = "ohos.permission.ENTERPRISE_MANAGE_LOCATION";
const std::string PERMISSION_ENTERPRISE_ACCESS_SCREEN_LOCK_INNER = "ohos.permission.ACCESS_SCREEN_LOCK_INNER";
const std::string PERMISSION_REBOOT = "ohos.permission.REBOOT";
const std::string PERMISSION_MANAGE_APP_BOOT_INTERNAL = "ohos.permission.MANAGE_APP_BOOT_INTERNAL";
const std::string PERMISSION_CONNECTIVITY_INTERNAL = "ohos.permission.CONNECTIVITY_INTERNAL";
const std::string PERMISSION_MANAGE_USB_CONFIG = "ohos.permission.MANAGE_USB_CONFIG";
const std::string PERMISSION_STORAGE_MANAGER = "ohos.permission.STORAGE_MANAGER";
const std::string PERMISSION_MOUNT_UNMOUNT_MANAGER = "ohos.permission.MOUNT_UNMOUNT_MANAGER";
const std::string PERMISSION_GET_BUNDLE_INFO = "ohos.permission.GET_BUNDLE_INFO";
const std::string PERSIST_BLUETOOTH_CONTROL = "persist.edm.prohibit_bluetooth";
const std::string PERMISSION_ACCESS_USER_TRUSTED_CERT = "ohos.permission.ACCESS_USER_TRUSTED_CERT";
const std::string PERMISSION_ACCESS_USER_AUTH_INTERNAL = "ohos.permission.ACCESS_USER_AUTH_INTERNAL";
const std::string PERMISSION_SET_WIFI_INFO = "ohos.permission.SET_WIFI_INFO";
const std::string PERMISSION_GET_WIFI_INFO = "ohos.permission.GET_WIFI_INFO";
const std::string PERMISSION_MANAGE_WIFI_CONNECTION = "ohos.permission.MANAGE_WIFI_CONNECTION";
const std::string PERMISSION_MANAGE_SETTINGS = "ohos.permission.MANAGE_SETTINGS";
const std::string PERMISSION_MICROPHONE_CONTROL = "ohos.permission.MICROPHONE_CONTROL";
const std::string PERMISSION_NETWORK_POLICY_MANAGE = "ohos.permission.MANAGE_NET_STRATEGY";
const std::string PERMISSION_CAMERA = "ohos.permission.CAMERA";
const std::string PERMISSION_CAMERA_CONTROL = "ohos.permission.CAMERA_CONTROL";
const std::string PERMISSION_MANAGE_APP_KEEP_ALIVE_INTERNAL = "ohos.permission.MANAGE_APP_KEEP_ALIVE_INTERNAL";
const std::string PERMISSION_MANAGE_EDM_POLICY = "ohos.permission.MANAGE_EDM_POLICY";
const std::string PERMISSION_GET_TELEPHONY_STATE = "ohos.permission.GET_TELEPHONY_STATE";
const std::string PERMISSION_SET_TELEPHONY_STATE = "ohos.permission.SET_TELEPHONY_STATE";
const std::string PERMISSION_GET_NETWORK_STATE = "ohos.permission.GET_NETWORK_STATE";
const std::string PERMISSION_MANAGE_APN_SETTING = "ohos.permission.MANAGE_APN_SETTING";
const std::string PERMISSION_MANAGE_VPN = "ohos.permission.MANAGE_VPN";
void Utils::ExecCmdSync(const std::string &cmd)
{
    FILE *fp = popen(cmd.c_str(), "r");
    if (fp != nullptr) {
        pclose(fp);
        fp = nullptr;
    }
}

void Utils::SetNativeTokenTypeAndPermissions(const char* permissions[], int size)
{
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = size,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = permissions,
        .acls = nullptr,
        .aplStr = "system_basic",
    };

    infoInstance.processName = "edm";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    seteuid(ROOT_UID);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    seteuid(EDM_UID);
}

void Utils::SetEdmInitialEnv()
{
    selfTokenId_ = GetSelfTokenID();
    const char* permissions[] = {
        EDM_MANAGE_DATETIME_PERMISSION.c_str(),
        EDM_FACTORY_RESET_PERMISSION.c_str(),
        EDM_MANAGE_LOCAL_ACCOUNTS_PERMISSION.c_str(),
        EDM_MANAGE_SECURE_SETTINGS_PERMISSION.c_str(),
        PERMISSION_ENTERPRISE_SUBSCRIBE_MANAGED_EVENT.c_str(),
        PERMISSION_MANAGE_ENTERPRISE_DEVICE_ADMIN.c_str(),
        PERMISSION_SET_ENTERPRISE_INFO.c_str(),
        PERMISSION_ACCESS_BLUETOOTH.c_str(),
        PERMISSION_MANAGE_BLUETOOTH.c_str(),
        PERMISSION_ACCESS_CERT_MANAGER_INTERNAL.c_str(),
        PERMISSION_ACCESS_CERT_MANAGER.c_str(),
        PERMISSION_NETSYS_INTERNAL.c_str(),
        PERMISSION_INSTALL_ENTERPRISE_MDM_BUNDLE.c_str(),
        PERMISSION_INSTALL_ENTERPRISE_NORMAL_BUNDLE.c_str(),
        PERMISSION_INSTALL_SELF_BUNDLE.c_str(),
        PERMISSION_UNINSTALL_BUNDLE.c_str(),
        PERMISSION_RUNNING_STATE_OBSERVER.c_str(),
        PERMISSION_ENTERPRISE_MANAGE_SECURITY.c_str(),
        PERMISSION_ENTERPRISE_MANAGE_BLUETOOTH.c_str(),
        PERMISSION_ENTERPRISE_MANAGE_LOCATION.c_str(),
        PERMISSION_ENTERPRISE_ACCESS_SCREEN_LOCK_INNER.c_str(),
        PERMISSION_REBOOT.c_str(),
        PERMISSION_MANAGE_APP_BOOT_INTERNAL.c_str(),
        PERMISSION_CONNECTIVITY_INTERNAL.c_str(),
        PERMISSION_MANAGE_USB_CONFIG.c_str(),
        PERMISSION_STORAGE_MANAGER.c_str(),
        PERMISSION_MOUNT_UNMOUNT_MANAGER.c_str(),
        PERMISSION_GET_BUNDLE_INFO.c_str(),
        PERMISSION_ACCESS_USER_TRUSTED_CERT.c_str(),
        PERMISSION_ACCESS_USER_AUTH_INTERNAL.c_str(),
        PERMISSION_GET_WIFI_INFO.c_str(),
        PERMISSION_SET_WIFI_INFO.c_str(),
        PERMISSION_MANAGE_WIFI_CONNECTION.c_str(),
        PERMISSION_MANAGE_SETTINGS.c_str(),
        PERMISSION_MICROPHONE_CONTROL.c_str(),
        PERMISSION_NETWORK_POLICY_MANAGE.c_str(),
        PERMISSION_CAMERA.c_str(),
        PERMISSION_CAMERA_CONTROL.c_str(),
        PERMISSION_MANAGE_APP_KEEP_ALIVE_INTERNAL.c_str(),
        PERMISSION_MANAGE_EDM_POLICY.c_str(),
        PERMISSION_GET_TELEPHONY_STATE.c_str(),
        PERMISSION_SET_TELEPHONY_STATE.c_str(),
        PERMISSION_GET_NETWORK_STATE.c_str(),
        PERMISSION_MANAGE_APN_SETTING.c_str(),
        PERMISSION_MANAGE_VPN.c_str()
    };
    Utils::SetNativeTokenTypeAndPermissions(permissions, sizeof(permissions) / sizeof(permissions[0]));
    seteuid(Utils::EDM_UID);
}

void Utils::ResetTokenTypeAndUid()
{
    seteuid(Utils::ROOT_UID);
    SetSelfTokenID(selfTokenId_);
}

bool Utils::IsOriginalUTEnv()
{
    return Utils::ROOT_UID == geteuid() && selfTokenId_ == GetSelfTokenID();
}

bool Utils::GetEdmServiceState()
{
    std::string edmParaValue = system::GetParameter("persist.edm.edm_enable", "false");
    return edmParaValue == "true";
}

void Utils::SetEdmServiceEnable()
{
    system::SetParameter(SET_EDM_SERVICE, "true");
}

void Utils::SetEdmServiceDisable()
{
    system::SetParameter(SET_EDM_SERVICE, "false");
}

void Utils::SetBluetoothEnable()
{
    system::SetParameter(PERSIST_BLUETOOTH_CONTROL, "false");
}

void Utils::SetBluetoothDisable()
{
    system::SetParameter(PERSIST_BLUETOOTH_CONTROL, "true");
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS
