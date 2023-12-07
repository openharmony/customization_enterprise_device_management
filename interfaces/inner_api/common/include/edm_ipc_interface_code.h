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

#ifndef INTERFACES_INNER_API_INCLUDE_EDM_IPC_INTERFACE_CODE_H
#define INTERFACES_INNER_API_INCLUDE_EDM_IPC_INTERFACE_CODE_H

/* SAID:1601 */
namespace OHOS {
namespace EDM {
enum EdmInterfaceCode : uint32_t {
    ADD_DEVICE_ADMIN = 1,
    REMOVE_DEVICE_ADMIN = 2,
    GET_PERMISSION_REQUEST = 3,
    REMOVE_SUPER_ADMIN = 4,
    GET_ENABLED_ADMIN = 5,
    GET_ENT_INFO = 6,
    SET_ENT_INFO = 7,
    IS_SUPER_ADMIN = 8,
    IS_ADMIN_ENABLED = 9,
    SUBSCRIBE_MANAGED_EVENT = 10,
    UNSUBSCRIBE_MANAGED_EVENT = 11,
    AUTHORIZE_ADMIN = 12,

    SET_DATETIME = 1001,
    GET_DEVICE_SERIAL = 1002,
    GET_DISPLAY_VERSION = 1003,
    GET_DEVICE_NAME = 1004,
    RESET_FACTORY = 1005,
    DISALLOW_ADD_LOCAL_ACCOUNT = 1006,
    IS_WIFI_ACTIVE = 1007,
    GET_NETWORK_INTERFACES = 1008,
    GET_IP_ADDRESS = 1009,
    GET_MAC = 1010,
    ALLOWED_INSTALL_BUNDLES = 1011,
    DISALLOW_MODIFY_DATETIME = 1012,
    SET_WIFI_PROFILE = 1013,
    DISALLOW_PRINTING = 1014,
    DISALLOW_RUNNING_BUNDLES = 1015,
    DISABLED_NETWORK_INTERFACE = 1016,
    DISALLOWED_INSTALL_BUNDLES = 1017,
    SCREEN_OFF_TIME = 1018,
    DISALLOWED_UNINSTALL_BUNDLES = 1019,
    UNINSTALL = 1020,
    DISABLED_PRINTER = 1021,
    DISABLED_HDC = 1022,
    IPTABLES_RULE = 1023,
    SET_BROWSER_POLICIES = 1024,
    GLOBAL_PROXY = 1025,
    USB_READ_ONLY = 1026,
    INSTALL_CERTIFICATE = 1027,
    INSTALL = 1028,
    POWER_POLICY = 1029,
    NTP_SERVER = 1030,
    GET_SECURITY_PATCH_TAG = 1031,
    GET_DEVICE_ENCRYPTION_STATUS = 1032,
    LOCK_SCREEN = 1033,
    FIREWALL_RULE = 1034,
    DOMAIN_FILTER_RULE = 1035,
    SHUTDOWN = 1036,
    REBOOT = 1037,
    DISALLOW_ADD_OS_ACCOUNT_BY_USER = 1038,
    ADD_OS_ACCOUNT = 1039,
    GET_BLUETOOTH_INFO = 1040,
    DISALLOW_SCREEN_SHOT = 1041,
    LOCATION_POLICY = 1042,
    DISABLE_USB = 1043,
    ALLOWED_USB_DEVICES = 1044,
    MANAGE_AUTO_START_APPS = 1045,
    DISABLE_WIFI = 1046,
    DISABLE_MICROPHONE = 1047,
    DISABLE_BLUETOOTH = 1048,
    FINGERPRINT_AUTH = 1049,
    POLICY_CODE_END = 3000,
};
} // namespace EDM
} // namespace OHOS
#endif // INTERFACES_INNER_API_INCLUDE_EDM_IPC_INTERFACE_CODE_H
