/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include <stdint.h>

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
    GET_SUPER_ADMIN_WANT_INFO = 13,
    SET_DELEGATED_POLICIES = 14,
    GET_DELEGATED_POLICIES = 15,
    GET_DELEGATED_BUNDLE_NAMES = 16,
    REPLACE_SUPER_ADMIN = 17,
    GET_ADMINS = 18,
    SET_ADMIN_RUNNING_MODE = 19,
    SET_DELEGATED_POLICIES_OVERRIDE = 20,
    SET_BUNDLE_INSTALL_POLICIES = 21,

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
    LOCATION_POLICY = 1042,
    DISABLE_USB = 1043,
    ALLOWED_USB_DEVICES = 1044,
    MANAGE_AUTO_START_APPS = 1045,
    DISABLE_WIFI = 1046,
    DISABLE_MICROPHONE = 1047,
    DISABLE_BLUETOOTH = 1048,
    FINGERPRINT_AUTH = 1049,
    GET_DEVICE_INFO = 1050,
    PASSWORD_POLICY = 1051,
    ALLOWED_BLUETOOTH_DEVICES = 1052,
    OPERATE_DEVICE = 1053,
    CLIPBOARD_POLICY = 1054,
    SET_OTA_UPDATE_POLICY = 1055,
    NOTIFY_UPGRADE_PACKAGES = 1056,
    DISALLOWED_TETHERING = 1057,
    INACTIVE_USER_FREEZE = 1058,
    DISALLOWED_USB_DEVICES = 1059,
    SNAPSHOT_SKIP = 1060,
    WATERMARK_IMAGE = 1061,
    DISABLE_CAMERA = 1062,
    MANAGE_KEEP_ALIVE_APPS = 1063,
    MANAGED_BROWSER_POLICY = 1064,
    GET_ADMINPROVISION_INFO = 1065,
    DISABLE_MTP_CLIENT = 1066,
    DISABLE_MTP_SERVER = 1067,
    DOMAIN_ACCOUNT_POLICY = 1068,
    DISABLE_MAINTENANCE_MODE = 1069,
	ALLOWED_WIFI_LIST = 1070,
    DISALLOWED_WIFI_LIST = 1071,
    SWITCH_BLUETOOTH = 1072,
    DISALLOWED_SMS = 1073,
    DISALLOWED_MMS = 1074,
    GET_BUNDLE_INFO_LIST = 1075,
    DISABLE_BACKUP_AND_RESTORE = 1076,
    SWITCH_WIFI = 1077,
    DISALLOWED_BLUETOOTH_PROTOCOLS = 1078,
    DISALLOWED_BLUETOOTH_DEVICES = 1079,
    DISALLOW_MODIFY_APN = 1080,
    TURNONOFF_MOBILE_DATA = 1081,
    DISABLE_REMOTE_DESK = 1082,
    DISABLE_REMOTE_DIAGNOSIS = 1083,
    CLEAR_UP_APPLICATION_DATA = 1084,
    DISABLE_USER_MTP_CLIENT = 1085,
    SET_APN_INFO = 1086,
    DISALLOWED_SIM = 1087,
    DISALLOWED_MOBILE_DATA = 1088,
    DISALLOW_MODIFY_ETHERNET_IP = 1089,
    DISABLE_SET_BIOMETRICS_AND_SCREENLOCK = 1090,
    DISABLE_SET_DEVICE_NAME = 1091,
    DISABLE_SAMBA_CLIENT = 1092,
    DISABLE_SAMBA_SERVER = 1093,
    SET_KIOSK_FEATURE = 1094,
    DISALLOW_POWER_LONG_PRESS = 1095,
    ALLOWED_KIOSK_APPS = 1096,
    IS_APP_KIOSK_ALLOWED = 1097,
    DISALLOWED_NFC = 1098,
    DISALLOWED_AIRPLANE_MODE = 1099,
    ALLOWED_INSTALL_APP_TYPE = 1100,
    DISALLOW_VPN = 1101,
    SET_AUTO_UNLOCK_AFTER_REBOOT = 1102,
    DISALLOWED_USB_STORAGE_DEVICE_WRITE = 1103,
    DISALLOWED_DISTRIBUTED_TRANSMISSION = 1104,
    DISALLOWED_SUDO = 1105,
    SET_INSTALL_LOCAL_ENTERPRISE_APP_ENABLED = 1106,
    DISALLOWED_NOTIFICATION = 1107,
    TELEPHONY_CALL_POLICY = 1108,
    DISALLOWED_TELEPHONY_CALL = 1109,
    DISALLOWED_NEARLINK_PROTOCOLS = 1110,
    DISABLE_PRIVATE_SPACE = 1111,
	DISALLOWED_EXPORT_RECOVERY_KEY = 1112,
    DISABLED_PRINT = 1113,
    PERMISSION_MANAGED_STATE = 1114,
    SET_WALL_PAPER = 1115,
    POLICY_CODE_END = 3000,
};
} // namespace EDM
} // namespace OHOS
#endif // INTERFACES_INNER_API_INCLUDE_EDM_IPC_INTERFACE_CODE_H
