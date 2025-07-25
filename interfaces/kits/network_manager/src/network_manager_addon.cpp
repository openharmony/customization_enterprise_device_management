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
#include "network_manager_addon.h"

#include "edm_constants.h"
#include "edm_ipc_interface_code.h"
#include "edm_log.h"
#include "hisysevent_adapter.h"
#include "iptables_utils.h"
#include "os_account_manager.h"

using namespace OHOS::EDM;
using namespace OHOS::EDM::IPTABLES;

#ifdef NETMANAGER_BASE_EDM_ENABLE
const char *const HOST_PROP_NAME = "host";
const char *const PORT_PROP_NAME = "port";
const char *const PROXY_USER_NAME = "username";
const char *const PROXY_PASSWORD = "password";
const char *const EXCLUSION_LIST_PROP_NAME = "exclusionList";
#endif

const std::set<std::string> REQUIRED_APN_INFO_KEYS = { "apnName", "mcc", "mnc", "apn" };
const std::set<std::string> ALL_APN_INFO_KEYS = {
    "apnName", "mcc", "mnc",
    "apn", "type", "user",
    "proxy", "mmsproxy", "authType", "password"
};
const std::map<std::string, std::string> KEY_TO_FIELD = {
    { "apnName", "profile_name" },
    { "type", "apn_types" },
    { "user", "auth_user" },
    { "proxy", "proxy_ip_address" },
    { "mmsproxy", "mms_ip_address" },
    { "authType", "auth_type" },
    { "mcc", "mcc" },
    { "mnc", "mnc" },
    { "apn", "apn" },
    { "password", "password" }
};
constexpr int32_t PASSWORD_KEY_LENGTH = 8;

void NetworkManagerAddon::CreateFirewallActionObject(napi_env env, napi_value value)
{
    napi_value nAllow;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Action::ALLOW), &nAllow));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "ALLOW", nAllow));
    napi_value nDeny;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Action::DENY), &nDeny));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "DENY", nDeny));
    napi_value nReject;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Action::REJECT), &nReject));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "REJECT", nReject));
}

void NetworkManagerAddon::CreateFirewallProtocolObject(napi_env env, napi_value value)
{
    napi_value nAll;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Protocol::ALL), &nAll));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "ALL", nAll));
    napi_value nTCP;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Protocol::TCP), &nTCP));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "TCP", nTCP));
    napi_value nUDP;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Protocol::UDP), &nUDP));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "UDP", nUDP));
    napi_value nICMP;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Protocol::ICMP), &nICMP));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "ICMP", nICMP));
}

void NetworkManagerAddon::CreateFirewallDirectionObject(napi_env env, napi_value value)
{
    napi_value nInput;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Direction::INPUT), &nInput));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "INPUT", nInput));
    napi_value nOutput;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Direction::OUTPUT), &nOutput));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "OUTPUT", nOutput));
    napi_value nForward;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::Direction::FORWARD), &nForward));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "FORWARD", nForward));
}

void NetworkManagerAddon::CreateFirewallAddMethodObject(napi_env env, napi_value value)
{
    napi_value nAppend;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::AddMethod::APPEND), &nAppend));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "APPEND", nAppend));
    napi_value nInsert;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IPTABLES::AddMethod::INSERT), &nInsert));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "INSERT", nInsert));
}

napi_value NetworkManagerAddon::Init(napi_env env, napi_value exports)
{
    napi_value nFirewallAction = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nFirewallAction));
    CreateFirewallActionObject(env, nFirewallAction);

    napi_value nFirewallProtocol = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nFirewallProtocol));
    CreateFirewallProtocolObject(env, nFirewallProtocol);

    napi_value nFirewallDirection = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nFirewallDirection));
    CreateFirewallDirectionObject(env, nFirewallDirection);

    napi_value nFirewallAddMethod = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nFirewallAddMethod));
    CreateFirewallAddMethodObject(env, nFirewallAddMethod);

    napi_property_descriptor property[] = {
        DECLARE_NAPI_FUNCTION("getAllNetworkInterfaces", GetAllNetworkInterfaces),
        DECLARE_NAPI_FUNCTION("getIpAddress", GetIpAddress),
        DECLARE_NAPI_FUNCTION("getMac", GetMac),
        DECLARE_NAPI_FUNCTION("setNetworkInterfaceDisabled", SetNetworkInterfaceDisabled),
        DECLARE_NAPI_FUNCTION("isNetworkInterfaceDisabled", IsNetworkInterfaceDisabled),

        DECLARE_NAPI_PROPERTY("Action", nFirewallAction),
        DECLARE_NAPI_PROPERTY("Protocol", nFirewallProtocol),
        DECLARE_NAPI_PROPERTY("Direction", nFirewallDirection),
        DECLARE_NAPI_PROPERTY("AddMethod", nFirewallAddMethod),
        DECLARE_NAPI_FUNCTION("addIptablesFilterRule", AddIptablesFilterRule),
        DECLARE_NAPI_FUNCTION("removeIptablesFilterRule", RemoveIptablesFilterRule),
        DECLARE_NAPI_FUNCTION("listIptablesFilterRules", ListIptablesFilterRules),

        DECLARE_NAPI_FUNCTION("addFirewallRule", AddFirewallRule),
        DECLARE_NAPI_FUNCTION("removeFirewallRule", RemoveFirewallRule),
        DECLARE_NAPI_FUNCTION("getFirewallRules", GetFirewallRules),

        DECLARE_NAPI_FUNCTION("addDomainFilterRule", AddDomainFilterRule),
        DECLARE_NAPI_FUNCTION("removeDomainFilterRule", RemoveDomainFilterRule),
        DECLARE_NAPI_FUNCTION("getDomainFilterRules", GetDomainFilterRules),

        DECLARE_NAPI_FUNCTION("setGlobalProxy", SetGlobalHttpProxy),
        DECLARE_NAPI_FUNCTION("getGlobalProxy", GetGlobalHttpProxy),

        DECLARE_NAPI_FUNCTION("getAllNetworkInterfacesSync", GetAllNetworkInterfacesSync),
        DECLARE_NAPI_FUNCTION("getIpAddressSync", GetIpAddressSync),
        DECLARE_NAPI_FUNCTION("getMacSync", GetMacSync),
        DECLARE_NAPI_FUNCTION("setNetworkInterfaceDisabledSync", SetNetworkInterfaceDisabledSync),
        DECLARE_NAPI_FUNCTION("isNetworkInterfaceDisabledSync", IsNetworkInterfaceDisabledSync),
        DECLARE_NAPI_FUNCTION("setGlobalProxySync", SetGlobalHttpProxySync),
        DECLARE_NAPI_FUNCTION("getGlobalProxySync", GetGlobalHttpProxySync),
        DECLARE_NAPI_FUNCTION("setGlobalProxyForAccount", SetGlobalHttpProxyForAccountSync),
        DECLARE_NAPI_FUNCTION("getGlobalProxyForAccount", GetGlobalHttpProxyForAccountSync),
        DECLARE_NAPI_FUNCTION("turnOnMobileData", TurnOnMobileData),
        DECLARE_NAPI_FUNCTION("turnOffMobileData", TurnOffMobileData),
        DECLARE_NAPI_FUNCTION("addApn", AddApn),
        DECLARE_NAPI_FUNCTION("deleteApn", DeleteApn),
        DECLARE_NAPI_FUNCTION("updateApn", UpdateApn),
        DECLARE_NAPI_FUNCTION("setPreferredApn", SetPreferApn),
        DECLARE_NAPI_FUNCTION("queryApn", QueryApn),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(property) / sizeof(property[0]), property));
    return exports;
}

napi_value NetworkManagerAddon::GetAllNetworkInterfaces(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getAllNetworkInterfaces");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "GetAllNetworkInterfaces";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_11;
    return AddonMethodAdapter(env, info, addonMethodSign, NativeGetAllNetworkInterfaces,
        NativeArrayStringCallbackComplete);
}

void NetworkManagerAddon::NativeGetAllNetworkInterfaces(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeGetAllNetworkInterfaces called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return;
    }
    asyncCallbackInfo->ret = networkManagerProxy->GetAllNetworkInterfaces(asyncCallbackInfo->data,
        asyncCallbackInfo->arrayStringRet);
}

void NetworkManagerAddon::GetIpOrMacAddressCommon(AddonMethodSign &addonMethodSign, const std::string &apiVersionTag,
    int32_t policyCode)
{
    addonMethodSign.name = "GetIpOrMacAddress";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::STRING};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    addonMethodSign.apiVersionTag = apiVersionTag;
    addonMethodSign.policyCode = policyCode;
}

napi_value NetworkManagerAddon::GetIpAddress(napi_env env, napi_callback_info info)
{
    EDMLOGI("NetworkManagerAddon::GetIpAddress called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getIpAddress");
    return GetIpOrMacAddress(env, info, EdmInterfaceCode::GET_IP_ADDRESS);
}

napi_value NetworkManagerAddon::GetMac(napi_env env, napi_callback_info info)
{
    EDMLOGI("NetworkManagerAddon::GetMac called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getMac");
    return GetIpOrMacAddress(env, info, EdmInterfaceCode::GET_MAC);
}

napi_value NetworkManagerAddon::GetIpOrMacAddress(napi_env env, napi_callback_info info, int policyCode)
{
    AddonMethodSign addonMethodSign;
    GetIpOrMacAddressCommon(addonMethodSign, EdmConstants::PERMISSION_TAG_VERSION_11, policyCode);
    return AddonMethodAdapter(env, info, addonMethodSign, NativeGetIpOrMacAddress, NativeStringCallbackComplete);
}

void NetworkManagerAddon::NativeGetIpOrMacAddress(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeGetIpOrMacAddress called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return;
    }
    asyncCallbackInfo->ret = networkManagerProxy->GetIpOrMacAddress(asyncCallbackInfo->data,
        asyncCallbackInfo->policyCode, asyncCallbackInfo->stringRet);
}

void NetworkManagerAddon::IsNetworkInterfaceDisabledCommon(AddonMethodSign &addonMethodSign,
    const std::string &apiVersionTag)
{
    addonMethodSign.name = "IsNetworkInterfaceDisabled";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::STRING};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    addonMethodSign.apiVersionTag = apiVersionTag;
}

void NetworkManagerAddon::SetNetworkInterfaceDisabledCommon(AddonMethodSign &addonMethodSign,
    const std::string &apiVersionTag)
{
    auto convertNetworkInterface2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        std::string networkInterface;
        bool isUint = ParseString(env, networkInterface, argv);
        if (!isUint) {
            return false;
        }
        std::vector<std::string> key{networkInterface};
        data.WriteStringVector(key);
        return true;
    };
    auto convertBoolean2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        bool isDisabled;
        bool isUint = ParseBool(env, isDisabled, argv);
        if (!isUint) {
            return false;
        }
        std::vector<std::string> value{isDisabled ? "true" : "false"};
        data.WriteStringVector(value);
        return true;
    };
    addonMethodSign.name = "SetNetworkInterfaceDisabled";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::STRING, EdmAddonCommonType::BOOLEAN};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.apiVersionTag = apiVersionTag;
    addonMethodSign.argsConvert = {nullptr, convertNetworkInterface2Data, convertBoolean2Data};
}

napi_value NetworkManagerAddon::SetNetworkInterfaceDisabled(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setNetworkInterfaceDisabled");
    AddonMethodSign addonMethodSign;
    SetNetworkInterfaceDisabledCommon(addonMethodSign, EdmConstants::PERMISSION_TAG_VERSION_11);
    return AddonMethodAdapter(env, info, addonMethodSign, NativeSetNetworkInterfaceDisabled,
        NativeVoidCallbackComplete);
}

void NetworkManagerAddon::NativeSetNetworkInterfaceDisabled(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeSetNetworkInterfaceDisabled called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return;
    }
    asyncCallbackInfo->ret = networkManagerProxy->SetNetworkInterfaceDisabled(asyncCallbackInfo->data);
}

napi_value NetworkManagerAddon::IsNetworkInterfaceDisabled(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "isNetworkInterfaceDisabled");
    AddonMethodSign addonMethodSign;
    IsNetworkInterfaceDisabledCommon(addonMethodSign, EdmConstants::PERMISSION_TAG_VERSION_11);
    return AddonMethodAdapter(env, info, addonMethodSign, NativeIsNetworkInterfaceDisabled,
        NativeBoolCallbackComplete);
}

void NetworkManagerAddon::NativeIsNetworkInterfaceDisabled(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeIsNetworkInterfaceDisabled called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return;
    }
    asyncCallbackInfo->ret = networkManagerProxy->IsNetworkInterfaceDisabled(asyncCallbackInfo->data,
        asyncCallbackInfo->boolRet);
}

napi_value NetworkManagerAddon::AddIptablesFilterRule(napi_env env, napi_callback_info info)
{
    EDMLOGI("NetworkManagerAddon::AddIptablesFilterRule called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "addIptablesFilterRule");
    auto convertAddFilter2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        IPTABLES::AddFilter filter;
        bool isUint = JsObjToAddFirewallObject(env, argv, filter);
        if (!isUint) {
            return false;
        }
        IPTABLES::IptablesUtils::WriteAddFilterConfig(filter, data);
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "AddIptablesFilterRule";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.argsConvert = {nullptr, convertAddFilter2Data};
    return AddonMethodAdapter(env, info, addonMethodSign, NativeAddIptalbsFilterRule,
        NativeVoidCallbackComplete);
}

bool NetworkManagerAddon::JsObjToAddFirewallObject(napi_env env, napi_value object, IPTABLES::AddFilter &filter)
{
    JsObjectToUint(env, object, "ruleNo", false, filter.ruleNo);
    JsObjectToString(env, object, "srcAddr", false, filter.srcAddr);
    JsObjectToString(env, object, "destAddr", false, filter.destAddr);
    JsObjectToString(env, object, "srcPort", false, filter.srcPort);
    JsObjectToString(env, object, "destPort", false, filter.destPort);
    JsObjectToString(env, object, "uid", false, filter.uid);
    int32_t method = -1;
    if (!JsObjectToInt(env, object, "method", true, method) ||
        !IPTABLES::IptablesUtils::ProcessFirewallMethod(method, filter.method)) {
        EDMLOGE("NAPI_AddIptalbsFilterRule JsObjToAddFirewallObject method trans failed");
        return false;
    }
    int32_t direction = -1;
    if (!JsObjectToInt(env, object, "direction", true, direction) ||
        !IPTABLES::IptablesUtils::ProcessFirewallDirection(direction, filter.direction)) {
        EDMLOGE("NAPI_AddIptalbsFilterRule JsObjToAddFirewallObject direction trans failed");
        return false;
    }
    int32_t action = -1;
    if (!JsObjectToInt(env, object, "action", true, action) ||
        !IPTABLES::IptablesUtils::ProcessFirewallAction(action, filter.action)) {
        EDMLOGE("NAPI_AddIptalbsFilterRule JsObjToAddFirewallObject action trans failed");
        return false;
    }
    int32_t protocol = -1;
    JsObjectToInt(env, object, "protocol", false, protocol);
    IPTABLES::IptablesUtils::ProcessFirewallProtocol(protocol, filter.protocol);
    return true;
}

void NetworkManagerAddon::NativeAddIptalbsFilterRule(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeAddIptalbsFilterRule called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    asyncCallbackInfo->ret = NetworkManagerProxy::GetNetworkManagerProxy()->AddIptablesFilterRule(
        asyncCallbackInfo->data);
}

napi_value NetworkManagerAddon::RemoveIptablesFilterRule(napi_env env, napi_callback_info info)
{
    EDMLOGI("NetworkManagerAddon::RemoveIptablesFilterRule called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "removeIptablesFilterRule");
    auto convertRemoveFilter2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        IPTABLES::RemoveFilter filter;
        bool isUint = JsObjToRemoveFirewallObject(env, argv, filter);
        if (!isUint) {
            return false;
        }
        IPTABLES::IptablesUtils::WriteRemoveFilterConfig(filter, data);
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "RemoveIptablesFilterRule";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.argsConvert = {nullptr, convertRemoveFilter2Data};
    return AddonMethodAdapter(env, info, addonMethodSign, NativeRemoveIptalbsFilterRule,
        NativeVoidCallbackComplete);
}

bool NetworkManagerAddon::JsObjToRemoveFirewallObject(napi_env env, napi_value object, IPTABLES::RemoveFilter &firewall)
{
    JsObjectToString(env, object, "srcAddr", false, firewall.srcAddr);
    JsObjectToString(env, object, "destAddr", false, firewall.destAddr);
    JsObjectToString(env, object, "srcPort", false, firewall.srcPort);
    JsObjectToString(env, object, "destPort", false, firewall.destPort);
    JsObjectToString(env, object, "uid", false, firewall.uid);
    int32_t direction = -1;
    if (!JsObjectToInt(env, object, "direction", true, direction) ||
        !IPTABLES::IptablesUtils::ProcessFirewallDirection(direction, firewall.direction)) {
        EDMLOGE("NAPI_removeIptalbsFilterRule JsObjToRemoveFirewallObject direction trans failed");
        return false;
    }
    int32_t action = -1;
    JsObjectToInt(env, object, "action", false, action);
    IPTABLES::IptablesUtils::ProcessFirewallAction(action, firewall.action);
    int32_t protocol = -1;
    JsObjectToInt(env, object, "protocol", false, protocol);
    IPTABLES::IptablesUtils::ProcessFirewallProtocol(protocol, firewall.protocol);
    return true;
}

void NetworkManagerAddon::NativeRemoveIptalbsFilterRule(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeRemoveIptalbsFilterRule called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    asyncCallbackInfo->ret =
        networkManagerProxy->RemoveIptablesFilterRule(asyncCallbackInfo->data);
}

napi_value NetworkManagerAddon::ListIptablesFilterRules(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "listIptablesFilterRules");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "ListIptablesFilterRules";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    return AddonMethodAdapter(env, info, addonMethodSign, NativeListIptablesFilterRules,
        NativeStringCallbackComplete);
}

void NetworkManagerAddon::NativeListIptablesFilterRules(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeListIptablesFilterRule called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    asyncCallbackInfo->ret = NetworkManagerProxy::GetNetworkManagerProxy()->ListIptablesFilterRules(
        asyncCallbackInfo->data, asyncCallbackInfo->stringRet);
}

napi_value NetworkManagerAddon::AddFirewallRule(napi_env env, napi_callback_info info)
{
    EDMLOGI("AddFirewallRule start");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "addFirewallRule");
    auto convertFirewallRule2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        IPTABLES::FirewallRule rule;
        bool isUint = JsObjToFirewallRule(env, argv, rule);
        if (!isUint) {
            return false;
        }
        IPTABLES::FirewallRuleParcel firewallRuleParcel{rule};
        if (!firewallRuleParcel.Marshalling(data)) {
            EDMLOGE("NetworkManagerAddon::AddOrRemoveFirewallRuleCommon Marshalling rule fail.");
            return false;
        }
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "AddFirewallRule";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.argsConvert = {nullptr, convertFirewallRule2Data};
    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->AddFirewallRule(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

napi_value NetworkManagerAddon::RemoveFirewallRule(napi_env env, napi_callback_info info)
{
    EDMLOGI("RemoveFirewallRule start");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "removeFirewallRule");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    if (argc >= ARGS_SIZE_TWO) {
        bool hasRule = MatchValueType(env, argv[ARR_INDEX_ONE], napi_object);
        ASSERT_AND_THROW_PARAM_ERROR(env, hasRule, "The second parameter must be FirewallRule.");
    }
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "element name param error");
    IPTABLES::FirewallRule rule = {IPTABLES::Direction::INVALID, IPTABLES::Action::INVALID,
        IPTABLES::Protocol::INVALID, "", "", "", "", ""};
    if (argc >= ARGS_SIZE_TWO) {
        ASSERT_AND_THROW_PARAM_ERROR(env, JsObjToFirewallRule(env, argv[ARR_INDEX_ONE], rule),
            "firewallRule param error");
    }
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->RemoveFirewallRule(elementName, rule);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

napi_value NetworkManagerAddon::GetFirewallRules(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getFirewallRules");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "GetFirewallRules";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;

    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }
    std::vector<IPTABLES::FirewallRule> result;
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->GetFirewallRules(adapterAddonData.data, result);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value jsList = nullptr;
    NAPI_CALL(env, napi_create_array_with_length(env, result.size(), &jsList));
    for (size_t i = 0; i < result.size(); i++) {
        napi_value item = FirewallRuleToJsObj(env, result[i]);
        NAPI_CALL(env, napi_set_element(env, jsList, i, item));
    }
    return jsList;
}

bool NetworkManagerAddon::JsObjToFirewallRule(napi_env env, napi_value object, IPTABLES::FirewallRule &rule)
{
    int32_t direction = -1;
    JsObjectToInt(env, object, "direction", false, direction);
    EDMLOGI("JsObjToFirewallRule direction %{public}d", direction);
    IPTABLES::Direction directionEnum = IPTABLES::Direction::INVALID;
    IPTABLES::IptablesUtils::ProcessFirewallDirection(direction, directionEnum);

    int32_t action = -1;
    JsObjectToInt(env, object, "action", false, action);
    EDMLOGI("JsObjToFirewallRule action %{public}d", action);
    IPTABLES::Action actionEnum = IPTABLES::Action::INVALID;
    IPTABLES::IptablesUtils::ProcessFirewallAction(action, actionEnum);

    int32_t protocol = -1;
    JsObjectToInt(env, object, "protocol", false, protocol);
    EDMLOGI("JsObjToFirewallRule protocol %{public}d", protocol);
    IPTABLES::Protocol protocolEnum = IPTABLES::Protocol::INVALID;
    IPTABLES::IptablesUtils::ProcessFirewallProtocol(protocol, protocolEnum);

    std::string srcAddr;
    JsObjectToString(env, object, "srcAddr", false, srcAddr);

    std::string destAddr;
    JsObjectToString(env, object, "destAddr", false, destAddr);

    std::string srcPort;
    JsObjectToString(env, object, "srcPort", false, srcPort);

    std::string destPort;
    JsObjectToString(env, object, "destPort", false, destPort);

    std::string appUid;
    JsObjectToString(env, object, "appUid", false, appUid);
    rule = {directionEnum, actionEnum, protocolEnum, srcAddr, destAddr, srcPort, destPort, appUid};
    return true;
}

napi_value NetworkManagerAddon::FirewallRuleToJsObj(napi_env env, const IPTABLES::FirewallRule &rule)
{
    napi_value jsRule = nullptr;
    NAPI_CALL(env, napi_create_object(env, &jsRule));

    napi_value direction = nullptr;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int32_t>(std::get<FIREWALL_DICECTION_IND>(rule)), &direction));
    napi_value action = nullptr;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int32_t>(std::get<FIREWALL_ACTION_IND>(rule)), &action));
    napi_value protocol = nullptr;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int32_t>(std::get<FIREWALL_PROT_IND>(rule)), &protocol));
    napi_value srcAddr = nullptr;
    std::string srcAddrStr = std::get<FIREWALL_SRCADDR_IND>(rule);
    NAPI_CALL(env, napi_create_string_utf8(env, srcAddrStr.c_str(), srcAddrStr.length(), &srcAddr));
    napi_value destAddr = nullptr;
    std::string destAddrStr = std::get<FIREWALL_DESTADDR_IND>(rule);
    NAPI_CALL(env, napi_create_string_utf8(env, destAddrStr.c_str(), destAddrStr.length(), &destAddr));
    napi_value srcPort = nullptr;
    std::string srcPortStr = std::get<FIREWALL_SRCPORT_IND>(rule);
    NAPI_CALL(env, napi_create_string_utf8(env, srcPortStr.c_str(), srcPortStr.length(), &srcPort));
    napi_value destPort = nullptr;
    std::string destPortStr = std::get<FIREWALL_DESTPORT_IND>(rule);
    NAPI_CALL(env, napi_create_string_utf8(env, destPortStr.c_str(), destPortStr.length(), &destPort));
    napi_value appUid = nullptr;
    std::string appUidStr = std::get<FIREWALL_APPUID_IND>(rule);
    NAPI_CALL(env, napi_create_string_utf8(env, appUidStr.c_str(), appUidStr.length(), &appUid));

    NAPI_CALL(env, napi_set_named_property(env, jsRule, "direction", direction));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "action", action));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "protocol", protocol));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "srcAddr", srcAddr));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "destAddr", destAddr));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "srcPort", srcPort));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "destPort", destPort));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "appUid", appUid));
    return jsRule;
}

napi_value NetworkManagerAddon::AddDomainFilterRule(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "addDomainFilterRule");
    auto convertFirewallRule2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        IPTABLES::DomainFilterRule rule = {IPTABLES::Action::INVALID, "", "", IPTABLES::Direction::INVALID};
        bool isParseOk = JsObjToDomainFilterRule(env, argv, rule);
        if (!isParseOk) {
            return false;
        }
        IPTABLES::DomainFilterRuleParcel domainFilterRuleParcel{rule};
        if (!domainFilterRuleParcel.Marshalling(data)) {
            EDMLOGE("NetworkManagerAddon::AddOrRemoveDomainFilterRuleCommon Marshalling rule fail.");
            return false;
        }
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "AddDomainFilterRule";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.argsConvert = {nullptr, convertFirewallRule2Data};
    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->AddDomainFilterRule(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

napi_value NetworkManagerAddon::RemoveDomainFilterRule(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "removeDomainFilterRule");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    if (argc >= ARGS_SIZE_TWO) {
        bool hasRule = MatchValueType(env, argv[ARR_INDEX_ONE], napi_object);
        ASSERT_AND_THROW_PARAM_ERROR(env, hasRule, "The second parameter must be DomainFilterRule.");
    }

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "element name param error");
    IPTABLES::DomainFilterRule rule = {IPTABLES::Action::INVALID, "", "", IPTABLES::Direction::INVALID};
    if (argc >= ARGS_SIZE_TWO) {
        ASSERT_AND_THROW_PARAM_ERROR(env, JsObjToDomainFilterRule(env, argv[ARR_INDEX_ONE], rule),
            "DomainFilterRule param error");
    }

    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->RemoveDomainFilterRule(elementName, rule);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

napi_value NetworkManagerAddon::GetDomainFilterRules(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getDomainFilterRules");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "GetDomainFilterRules";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;

    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }
    std::vector<IPTABLES::DomainFilterRule> result;
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->GetDomainFilterRules(adapterAddonData.data, result);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value jsList = nullptr;
    NAPI_CALL(env, napi_create_array_with_length(env, result.size(), &jsList));
    for (size_t i = 0; i < result.size(); i++) {
        napi_value item = DomainFilterRuleToJsObj(env, result[i]);
        NAPI_CALL(env, napi_set_element(env, jsList, i, item));
    }
    return jsList;
}

bool NetworkManagerAddon::JsObjToDomainFilterRule(napi_env env, napi_value object, IPTABLES::DomainFilterRule &rule)
{
    int32_t action = -1;
    JsObjectToInt(env, object, "action", false, action);
    IPTABLES::Action actionEnum = IPTABLES::Action::INVALID;
    IPTABLES::IptablesUtils::ProcessFirewallAction(action, actionEnum);

    std::string appUid;
    JsObjectToString(env, object, "appUid", false, appUid);

    std::string domainName;
    JsObjectToString(env, object, "domainName", false, domainName);

    int32_t direction = -1;
    JsObjectToInt(env, object, "direction", false, direction);
    IPTABLES::Direction directionEnum = IPTABLES::Direction::INVALID;
    IPTABLES::IptablesUtils::ProcessFirewallDirection(direction, directionEnum);
    rule = {actionEnum, appUid, domainName, directionEnum};
    return true;
}

napi_value NetworkManagerAddon::DomainFilterRuleToJsObj(napi_env env, const IPTABLES::DomainFilterRule &rule)
{
    napi_value jsRule = nullptr;
    NAPI_CALL(env, napi_create_object(env, &jsRule));

    napi_value direction = nullptr;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int32_t>(std::get<DOMAIN_DIRECTION_IND>(rule)), &direction));
    napi_value action = nullptr;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int32_t>(std::get<DOMAIN_ACTION_IND>(rule)), &action));
    napi_value appUid = nullptr;
    std::string appUidStr = std::get<DOMAIN_APPUID_IND>(rule);
    NAPI_CALL(env, napi_create_string_utf8(env, appUidStr.c_str(), appUidStr.length(), &appUid));
    napi_value domainName = nullptr;
    std::string domainNameStr = std::get<DOMAIN_DOMAINNAME_IND>(rule);
    NAPI_CALL(env, napi_create_string_utf8(env, domainNameStr.c_str(), domainNameStr.length(), &domainName));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "action", action));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "appUid", appUid));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "domainName", domainName));
    NAPI_CALL(env, napi_set_named_property(env, jsRule, "direction", direction));
    return jsRule;
}

napi_value NetworkManagerAddon::SetGlobalHttpProxy(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setGlobalHttpProxy");
#ifdef NETMANAGER_BASE_EDM_ENABLE
    AddonMethodSign addonMethodSign;
    SetGlobalHttpProxyCommon(addonMethodSign);
    return AddonMethodAdapter(env, info, addonMethodSign, NativeSetGlobalHttpProxy, NativeVoidCallbackComplete);
#else
    EDMLOGW("NetworkManagerAddon::SetGlobalHttpProxy Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

#ifdef NETMANAGER_BASE_EDM_ENABLE
bool NetworkManagerAddon::ParseHttpProxyParam(napi_env env, napi_value argv, NetManagerStandard::HttpProxy &httpProxy)
{
    std::string host;
    if (!JsObjectToString(env, argv, HOST_PROP_NAME, true, host)) {
        EDMLOGE("error host value");
        return false;
    }
    std::int32_t port = 0;
    if (!JsObjectToInt(env, argv, PORT_PROP_NAME, true, port)) {
        EDMLOGE("error port value");
        return false;
    }
    OHOS::NetManagerStandard::SecureData username;
    if (!JsObjectToSecureData(env, argv, PROXY_USER_NAME, username)) {
        EDMLOGE("error username value");
        return false;
    }
    OHOS::NetManagerStandard::SecureData password;
    if (!JsObjectToSecureData(env, argv, PROXY_PASSWORD, password)) {
        EDMLOGE("error password value");
        return false;
    }
    if (!username.empty() && !password.empty()) {
        EDMLOGD("NetworkManagerAddon username and password is not empty.");
    } else {
        EDMLOGD("NetworkManagerAddon username or password is empty.");
    }
    std::vector<std::string> exclusionList;
    if (!JsObjectToStringVector(env, argv, EXCLUSION_LIST_PROP_NAME, true, exclusionList)) {
        EDMLOGE("error exclusionList value");
        return false;
    }

    httpProxy.SetHost(host.c_str());
    httpProxy.SetPort(port);
    httpProxy.SetUserName(username);
    httpProxy.SetPassword(password);
    std::list<std::string> dataList;
    for (const auto &item : exclusionList) {
        dataList.emplace_back(item);
    }
    httpProxy.SetExclusionList(dataList);
    return true;
}

bool NetworkManagerAddon::JsObjectToSecureData(napi_env env, napi_value object, const char *paramStr,
    OHOS::NetManagerStandard::SecureData &secureData)
{
    bool hasProperty = false;
    if (napi_has_named_property(env, object, paramStr, &hasProperty) != napi_ok) {
        EDMLOGE("get js property failed.");
        return false;
    }
    if (hasProperty) {
        napi_value prop = nullptr;
        return napi_get_named_property(env, object, paramStr, &prop) == napi_ok &&
            ParseSecureData(env, secureData, prop);
    }
    return true;
}

bool NetworkManagerAddon::ParseSecureData(napi_env env, OHOS::NetManagerStandard::SecureData &secureData,
    napi_value object)
{
    napi_valuetype valuetype;
    if (napi_typeof(env, object, &valuetype) != napi_ok || valuetype != napi_string ||
        !GetSecureDataFromNAPI(env, object, secureData)) {
        EDMLOGE("can not get string value");
        return false;
    }
    return true;
}

bool NetworkManagerAddon::GetSecureDataFromNAPI(napi_env env, napi_value object,
    OHOS::NetManagerStandard::SecureData &secureData)
{
    OHOS::NetManagerStandard::SecureData result;
    size_t size = 0;

    if (napi_get_value_string_utf8(env, object, nullptr, NAPI_RETURN_ZERO, &size) != napi_ok) {
        EDMLOGE("can not get string size");
        return false;
    }
    result.reserve(size + NAPI_RETURN_ONE);
    result.resize(size);
    if (napi_get_value_string_utf8(env, object, result.data(), (size + NAPI_RETURN_ONE), &size) != napi_ok) {
        EDMLOGE("can not get string value");
        return false;
    }
    secureData = result;
    return true;
}

void NetworkManagerAddon::NativeSetGlobalHttpProxy(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeSetGlobalHttpProxycalled");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    asyncCallbackInfo->ret = NetworkManagerProxy::GetNetworkManagerProxy()->SetGlobalHttpProxy(
        asyncCallbackInfo->data);
}

napi_value NetworkManagerAddon::ConvertHttpProxyToJS(napi_env env, const OHOS::NetManagerStandard::HttpProxy &httpProxy)
{
    napi_value proxy = nullptr;
    NAPI_CALL(env, napi_create_object(env, &proxy));
    napi_value host = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, httpProxy.GetHost().c_str(), httpProxy.GetHost().length(), &host));
    napi_value port = nullptr;
    NAPI_CALL(env, napi_create_int32(env, httpProxy.GetPort(), &port));
    napi_value list = nullptr;
    if (httpProxy.GetExclusionList().empty()) {
        NAPI_CALL(env, napi_create_array(env, &list));
    } else {
        std::list<std::string> tempList = httpProxy.GetExclusionList();
        NAPI_CALL(env, napi_create_array_with_length(env, tempList.size(), &list));
        size_t index = 0;
        for (const auto &item : tempList) {
            napi_value ip = nullptr;
            NAPI_CALL(env, napi_create_string_utf8(env, item.c_str(), item.length(), &ip));
            NAPI_CALL(env, napi_set_element(env, list, index++, ip));
        }
    }
    NAPI_CALL(env, napi_set_named_property(env, proxy, HOST_PROP_NAME, host));
    NAPI_CALL(env, napi_set_named_property(env, proxy, PORT_PROP_NAME, port));
    NAPI_CALL(env, napi_set_named_property(env, proxy, EXCLUSION_LIST_PROP_NAME, list));
    return proxy;
}
#endif

napi_value NetworkManagerAddon::GetGlobalHttpProxy(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getGlobalHttpProxy");
#ifdef NETMANAGER_BASE_EDM_ENABLE
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    auto asyncCallbackInfo = new (std::nothrow) AsyncHttpProxyCallbackInfo();
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncHttpProxyCallbackInfo> callbackPtr{asyncCallbackInfo};
    bool matchValue = false;
    if (MatchValueType(env, argv[ARR_INDEX_ZERO], napi_null)) {
        asyncCallbackInfo->hasAdmin = false;
        matchValue = true;
    } else if (MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object)) {
        asyncCallbackInfo->hasAdmin = true;
        matchValue = true;
    }
    if (argc > ARGS_SIZE_ONE) {
        matchValue = matchValue && MatchValueType(env, argv[ARGS_SIZE_ONE], napi_function);
    }
    ASSERT_AND_THROW_PARAM_ERROR(env, matchValue, "parameter type error");
    if (asyncCallbackInfo->hasAdmin) {
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, asyncCallbackInfo->elementName, argv[ARR_INDEX_ZERO]),
            "element name param error");
        EDMLOGD(
            "GetGlobalHttpProxy: asyncCallbackInfo->elementName.bundlename %{public}s, "
            "asyncCallbackInfo->abilityname:%{public}s",
            asyncCallbackInfo->elementName.GetBundleName().c_str(),
            asyncCallbackInfo->elementName.GetAbilityName().c_str());
    }
    if (argc > ARGS_SIZE_ONE) {
        EDMLOGD("NAPI_GetGlobalHttpProxy argc == ARGS_SIZE_TWO");
        napi_create_reference(env, argv[ARGS_SIZE_ONE], NAPI_RETURN_ONE, &asyncCallbackInfo->callback);
    }
    napi_value asyncWorkReturn = HandleAsyncWork(env, asyncCallbackInfo, "NativeGetGlobalHttpProxy",
        NativeGetGlobalHttpProxy, NativeHttpProxyCallbackComplete);
    callbackPtr.release();
    return asyncWorkReturn;
#else
    EDMLOGW("NetworkManagerAddon::GetGlobalHttpProxy Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

#ifdef NETMANAGER_BASE_EDM_ENABLE
void NetworkManagerAddon::NativeGetGlobalHttpProxy(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeGetGlobalHttpProxy called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AsyncHttpProxyCallbackInfo *asyncCallbackInfo = static_cast<AsyncHttpProxyCallbackInfo *>(data);
    int32_t accountId = -1;
    if (asyncCallbackInfo->hasAdmin) {
        asyncCallbackInfo->ret = NetworkManagerProxy::GetNetworkManagerProxy()->GetGlobalHttpProxy(
            &asyncCallbackInfo->elementName, asyncCallbackInfo->httpProxy, accountId);
    } else {
        asyncCallbackInfo->ret =
            NetworkManagerProxy::GetNetworkManagerProxy()->GetGlobalHttpProxy(nullptr,
                asyncCallbackInfo->httpProxy, accountId);
    }
}

void NetworkManagerAddon::NativeHttpProxyCallbackComplete(napi_env env, napi_status status, void *data)
{
    EDMLOGD("NativeHttpProxyCallbackComplete start");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    auto *asyncCallbackInfo = static_cast<AsyncHttpProxyCallbackInfo *>(data);
    if (asyncCallbackInfo->deferred != nullptr) {
        EDMLOGD("asyncCallbackInfo->deferred != nullptr");
        if (asyncCallbackInfo->ret == ERR_OK) {
            napi_value jsHttpProxy = ConvertHttpProxyToJS(env, asyncCallbackInfo->httpProxy);
            napi_resolve_deferred(env, asyncCallbackInfo->deferred, jsHttpProxy);
        } else {
            napi_reject_deferred(env, asyncCallbackInfo->deferred, CreateError(env, asyncCallbackInfo->ret));
        }
    } else {
        napi_value callbackValue[ARGS_SIZE_TWO] = {0};
        if (asyncCallbackInfo->ret == ERR_OK) {
            napi_get_null(env, &callbackValue[ARR_INDEX_ZERO]);
            callbackValue[ARR_INDEX_ONE] = ConvertHttpProxyToJS(env, asyncCallbackInfo->httpProxy);
        } else {
            callbackValue[ARR_INDEX_ZERO] = CreateError(env, asyncCallbackInfo->ret);
            napi_get_null(env, &callbackValue[ARR_INDEX_ONE]);
        }
        napi_value callback = nullptr;
        napi_value result = nullptr;
        napi_get_reference_value(env, asyncCallbackInfo->callback, &callback);
        napi_call_function(env, nullptr, callback, std::size(callbackValue), callbackValue, &result);
        napi_delete_reference(env, asyncCallbackInfo->callback);
    }
    napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
    delete asyncCallbackInfo;
}
#endif

napi_value NetworkManagerAddon::GetAllNetworkInterfacesSync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetAllNetworkInterfacesSync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getAllNetworkInterfacesSync");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "GetAllNetworkInterfacesSync";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_12;
    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }
    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return nullptr;
    }
    std::vector<std::string> networkInterface;
    int32_t ret = networkManagerProxy->GetAllNetworkInterfaces(adapterAddonData.data, networkInterface);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_array(env, &result));
    ConvertStringVectorToJS(env, networkInterface, result);
    return result;
}

napi_value NetworkManagerAddon::GetIpAddressSync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetIpAddressSync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getIpAddressSync");
    return GetIpOrMacAddressSync(env, info, EdmInterfaceCode::GET_IP_ADDRESS);
}

napi_value NetworkManagerAddon::GetMacSync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetMacSync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getMacSync");
    return GetIpOrMacAddressSync(env, info, EdmInterfaceCode::GET_MAC);
}

napi_value NetworkManagerAddon::GetIpOrMacAddressSync(napi_env env, napi_callback_info info, int policyCode)
{
    AddonMethodSign addonMethodSign;
    GetIpOrMacAddressCommon(addonMethodSign, EdmConstants::PERMISSION_TAG_VERSION_12, policyCode);
    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }
    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return nullptr;
    }
    std::string ipOrMacInfo;
    int32_t ret = networkManagerProxy->GetIpOrMacAddress(adapterAddonData.data, policyCode, ipOrMacInfo);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, ipOrMacInfo.c_str(), NAPI_AUTO_LENGTH, &result));
    return result;
}

napi_value NetworkManagerAddon::SetNetworkInterfaceDisabledSync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetNetworkInterfaceDisabledSync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setNetworkInterfaceDisabledSync");
    AddonMethodSign addonMethodSign;
    SetNetworkInterfaceDisabledCommon(addonMethodSign, EdmConstants::PERMISSION_TAG_VERSION_12);
    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }
    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return nullptr;
    }
    int32_t ret = networkManagerProxy->SetNetworkInterfaceDisabled(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    return nullptr;
}

napi_value NetworkManagerAddon::IsNetworkInterfaceDisabledSync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_IsNetworkInterfaceDisabledSync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "isNetworkInterfaceDisabledSync");
    AddonMethodSign addonMethodSign;
    IsNetworkInterfaceDisabledCommon(addonMethodSign, EdmConstants::PERMISSION_TAG_VERSION_12);
    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }
    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return nullptr;
    }
    bool isDisabled = false;
    int32_t ret = networkManagerProxy->IsNetworkInterfaceDisabled(adapterAddonData.data, isDisabled);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, isDisabled, &result));
    return result;
}

void NetworkManagerAddon::SetGlobalHttpProxyCommon(AddonMethodSign &addonMethodSign)
{
#ifdef NETMANAGER_BASE_EDM_ENABLE
    auto convertHttpProxy2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        NetManagerStandard::HttpProxy httpProxy;
        bool isParseOk = ParseHttpProxyParam(env, argv, httpProxy);
        if (!isParseOk) {
            return false;
        }
        if (!httpProxy.Marshalling(data)) {
            EDMLOGE("NetworkManagerAddon::SetGlobalHttpProxyCommon Marshalling proxy fail.");
            return false;
        }
        int32_t accountId = -1;
        data.WriteInt32(accountId);
        return true;
    };
    addonMethodSign.name = "SetGlobalHttpProxy";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.argsConvert = {nullptr, convertHttpProxy2Data};
#endif
}

void NetworkManagerAddon::SetGlobalHttpProxyCommonForAccount(AddonMethodSign &addonMethodSign)
{
#ifdef NETMANAGER_BASE_EDM_ENABLE
    auto convertHttpProxy2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        NetManagerStandard::HttpProxy httpProxy;
        bool isParseOk = ParseHttpProxyParam(env, argv, httpProxy);
        if (!isParseOk) {
            return false;
        }
        if (!httpProxy.Marshalling(data)) {
            EDMLOGE("NetworkManagerAddon::SetGlobalHttpProxyCommonForAccount Marshalling proxy fail.");
            return false;
        }
        return true;
    };
    addonMethodSign.name = "SetGlobalHttpProxyForAccountSync";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM, EdmAddonCommonType::INT32};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.argsConvert = {nullptr, convertHttpProxy2Data, nullptr};
#endif
}

napi_value NetworkManagerAddon::SetGlobalHttpProxyCommonLogic(napi_env env, napi_callback_info info,
    AddonMethodSign& addonMethodSign)
{
#ifdef NETMANAGER_BASE_EDM_ENABLE
    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }

    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return nullptr;
    }

    int32_t ret = networkManagerProxy->SetGlobalHttpProxy(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::SetGlobalHttpProxySync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetGlobalHttpProxySync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setGlobalHttpProxySync");
#ifdef NETMANAGER_BASE_EDM_ENABLE
    AddonMethodSign addonMethodSign;
    SetGlobalHttpProxyCommon(addonMethodSign);

    return SetGlobalHttpProxyCommonLogic(env, info, addonMethodSign);
#else
    EDMLOGW("NetworkManagerAddon::SetGlobalHttpProxySync Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::SetGlobalHttpProxyForAccountSync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetGlobalHttpProxyForAccountSync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setGlobalHttpProxyForAccountSync");
#if defined(FEATURE_PC_ONLY) && defined(NETMANAGER_BASE_EDM_ENABLE)
    AddonMethodSign addonMethodSign;
    SetGlobalHttpProxyCommonForAccount(addonMethodSign);

    return SetGlobalHttpProxyCommonLogic(env, info, addonMethodSign);
#else
    EDMLOGW("NetworkManagerAddon::SetGlobalHttpProxyForAccountSync Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::GetGlobalHttpProxyCommon(
    napi_env env, napi_value *argv, size_t argc, bool hasAdmin,
    OHOS::AppExecFwk::ElementName &elementName, int32_t accountId)
{
#ifdef NETMANAGER_BASE_EDM_ENABLE
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, CheckGetPolicyAdminParam(env, argv[ARR_INDEX_ZERO], hasAdmin, elementName),
        "param admin need be null or want");

    if (hasAdmin) {
        EDMLOGD("GetGlobalHttpProxySync: elementName.bundleName %{public}s, elementName.abilityName:%{public}s",
            elementName.GetBundleName().c_str(), elementName.GetAbilityName().c_str());
    } else {
        EDMLOGD("GetGlobalHttpProxySync: elementName is null");
    }

    auto networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    if (networkManagerProxy == nullptr) {
        EDMLOGE("can not get GetNetworkManagerProxy");
        return nullptr;
    }

    NetManagerStandard::HttpProxy httpProxy;
    int32_t ret = ERR_OK;
    if (hasAdmin) {
        ret = networkManagerProxy->GetGlobalHttpProxy(&elementName, httpProxy, accountId);
    } else {
        ret = networkManagerProxy->GetGlobalHttpProxy(nullptr, httpProxy, accountId);
    }

    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }

    return ConvertHttpProxyToJS(env, httpProxy);
#else
    EDMLOGW("NetworkManagerAddon::GetGlobalHttpProxyCommon Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::GetGlobalHttpProxySync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetGlobalHttpProxySync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getGlobalHttpProxySync");
#ifdef NETMANAGER_BASE_EDM_ENABLE
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));

    bool hasAdmin = false;
    OHOS::AppExecFwk::ElementName elementName;
    return GetGlobalHttpProxyCommon(env, argv, argc, hasAdmin, elementName, -1);
#else
    EDMLOGW("NetworkManagerAddon::GetGlobalHttpProxy Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::GetGlobalHttpProxyForAccountSync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetGlobalHttpProxyForAccountSync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getGlobalHttpProxyForAccountSync");
#if defined(FEATURE_PC_ONLY) && defined(NETMANAGER_BASE_EDM_ENABLE)
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");

    bool hasAdmin = false;
    int32_t accountId = -1;
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_number),
        "parameter accountId error");
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, accountId, argv[ARR_INDEX_ONE]),
        "parameter accountId parse error");

    return GetGlobalHttpProxyCommon(env, argv, argc, hasAdmin, elementName, accountId);
#else
    EDMLOGW("NetworkManagerAddon::GetGlobalHttpProxyForAccountSync Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::TurnOnMobileData(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_TurnOnMobileData called");
#if defined(CELLULAR_DATA_EDM_ENABLE)
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    bool hasIsForce = MatchValueType(env, argv[ARR_INDEX_ONE], napi_boolean);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasIsForce, "The second parameter must be boolean.");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "element name param error");
    bool isForce;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseBool(env, isForce, argv[ARR_INDEX_ONE]),
        "isForce name param error");
    if (isForce) {
        int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->ForceTurnOnMobileData(elementName);
        if (FAILED(ret)) {
            napi_throw(env, CreateError(env, ret));
        }
    } else {
        AddonMethodSign addonMethodSign;
        addonMethodSign.name = "TurnOnMobileData";
        addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::BOOLEAN};
        addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
        AdapterAddonData adapterAddonData{};
        napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
        if (result == nullptr) {
            return nullptr;
        }
        int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->TurnOnMobileData(adapterAddonData.data);
        if (FAILED(ret)) {
            napi_throw(env, CreateError(env, ret));
        }
    }
    return nullptr;
#else
    EDMLOGW("NetworkManagerAddon::TurnOnMobileData Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::TurnOffMobileData(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_TurnOffMobileData called");
#if defined(CELLULAR_DATA_EDM_ENABLE)
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "TurnOffMobileData";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->TurnOffMobileData(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
#else
    EDMLOGW("NetworkManagerAddon::TurnOffMobileData Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

bool NetworkManagerAddon::CheckParameters(const std::map<std::string, std::string> &parameters)
{
    for (auto &ele : REQUIRED_APN_INFO_KEYS) {
        if (parameters.find(ele) == parameters.end() || parameters.at(ele) == "") {
            EDMLOGE("CheckParameters::Required is null.");
            return false;
        }
    }
    bool allValid = std::all_of(parameters.begin(), parameters.end(), [](auto &i) {
        bool valid = ALL_APN_INFO_KEYS.find(i.first) != ALL_APN_INFO_KEYS.end();
        if (!valid) {
            EDMLOGE("invalid key: %{public}s", i.first.c_str());
        }
        return valid;
    });
    return allValid;
}

void KeyToField(const std::map<std::string, std::string> &parameters, std::map<std::string, std::string> &results)
{
    for (const auto &[key, value] : KEY_TO_FIELD) {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            results[value] = it->second;
        }
    }
}

void FieldToKey(const std::map<std::string, std::string> &parameters, std::map<std::string, std::string> &results)
{
    for (const auto &[key, value] : KEY_TO_FIELD) {
        auto it = parameters.find(value);
        if (it != parameters.end()) {
            results[key] = it->second;
        }
    }
}

void ParametersTransform(const std::map<std::string, std::string> &parameters,
    std::map<std::string, std::string> &results)
{
    results = {
        {"profile_name", parameters.at("profile_name")},
        {"mcc", parameters.at("mcc")},
        {"mnc", parameters.at("mnc")},
        {"apn", parameters.at("apn")},
        {"edited", "1"}
    };

    std::set<std::string> optionalKeys;
    std::set_difference(
        ALL_APN_INFO_KEYS.begin(), ALL_APN_INFO_KEYS.end(),
        REQUIRED_APN_INFO_KEYS.begin(), REQUIRED_APN_INFO_KEYS.end(),
        std::inserter(optionalKeys, optionalKeys.begin())
    );
    for (const auto& ele : optionalKeys) {
        std::string field = KEY_TO_FIELD.at(ele);
        if (parameters.find(field) != parameters.end()) {
            results[field] = parameters.at(field);
        }
    }

    if (parameters.find("apn_types") == parameters.end()) {
        results["apn_types"] = "default";
    }
    if (results.find("auth_type") == results.end() ||
        (results.at("auth_type") != "0" && results.at("auth_type") != "1" &&
        results.at("auth_type") != "2" && results.at("auth_type") != "3")) {
        results["auth_type"] = "-1";
    }
}

static bool ParsePwd(napi_env env, napi_value args, ApnPassword &apnPassword)
{
    napi_valuetype valueType;
    if (napi_typeof(env, args, &valueType) != napi_ok || valueType != napi_object) {
        EDMLOGE("Parameter 'args' must be an object.");
        return false;
    }

    bool hasProperty = false;
    if (napi_has_named_property(env, args, "password", &hasProperty) != napi_ok || !hasProperty) {
        return true;
    }

    napi_value pwdValue;
    if (napi_get_named_property(env, args, "password", &pwdValue) != napi_ok) {
        EDMLOGE("ParsePwd: napi_get_named_property failed.");
        return false;
    }

    if (napi_get_value_string_utf8(env, pwdValue, nullptr, 0, &apnPassword.passwordSize) != napi_ok) {
        EDMLOGE("ParsePwd: napi_get_value_string_utf8 failed.");
        return false;
    }

    if (apnPassword.passwordSize > ApnPassword::MAX_PASSWORD_SIZE) {
        EDMLOGE("ParsePwd: The password length exceeds the limit.");
        return false;
    }

    apnPassword.password = static_cast<char *>(calloc(apnPassword.passwordSize + 1, 1));
    if (!apnPassword.password) {
        EDMLOGE("ParsePwd: calloc failed.");
        return false;
    }

    if (napi_get_value_string_utf8(env, pwdValue, apnPassword.password, apnPassword.passwordSize + 1, nullptr)
        != napi_ok) {
        EDMLOGE("ParsePwd: napi_get_value_string_utf8 failed.");
        return false;
    }

    napi_value pwdKey;
    if (napi_create_string_utf8(env, "password", PASSWORD_KEY_LENGTH, &pwdKey) != napi_ok) {
        EDMLOGE("ParsePwd: napi_create_string_utf8 failed.");
        return false;
    }

    bool deleteSuccess = false;
    if (napi_delete_property(env, args, pwdKey, &deleteSuccess) != napi_ok || !deleteSuccess) {
        EDMLOGE("ParsePwd: napi_delete_property failed.");
        return false;
    }

    return true;
}

napi_value NetworkManagerAddon::AddApn(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_AddApn called");
#if defined(CELLULAR_DATA_EDM_ENABLE)
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    bool hasApnInfo = MatchValueType(env, argv[ARR_INDEX_ONE], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasApnInfo, "The second parameter must be ApnInfo.");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "element name param error");
    ApnPassword apnPassword;
    if (!ParsePwd(env, argv[ARR_INDEX_ONE], apnPassword)) {
        return nullptr;
    }
    std::map<std::string, std::string> apnInfoMap;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseMapStringAndString(env, apnInfoMap, argv[ARR_INDEX_ONE]),
        "apnInfo name param error");
    
    ASSERT_AND_THROW_PARAM_ERROR(env, CheckParameters(apnInfoMap), "Required fields is null");
    std::map<std::string, std::string> apnInfoMapEx;
    KeyToField(apnInfoMap, apnInfoMapEx);

    std::map<std::string, std::string> apnInfoMapTf;
    ParametersTransform(apnInfoMapEx, apnInfoMapTf);
    
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->AddApn(elementName, apnInfoMapTf, apnPassword);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
#else
    EDMLOGW("NetworkManagerAddon::AddApn Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::DeleteApn(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_DeleteApn called");
#if defined(CELLULAR_DATA_EDM_ENABLE)
    auto checkStringIsNull = [](napi_env env, napi_value argv, MessageParcel &data, const AddonMethodSign &methodSign) {
        std::string apnId;
        if (!ParseString(env, apnId, argv) || apnId.empty()) {
            return false;
        }
        return data.WriteString(apnId);
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "DeleteApn";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::STRING};
    addonMethodSign.argsConvert = {nullptr, checkStringIsNull};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->DeleteApn(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
#else
    EDMLOGW("NetworkManagerAddon::DeleteApn Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::UpdateApn(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_UpdateApn called");
#if defined(CELLULAR_DATA_EDM_ENABLE)
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_THREE, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    bool hasApnInfo = MatchValueType(env, argv[ARR_INDEX_ONE], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasApnInfo, "The second parameter must be ApnInfo.");
    bool hasApnId = MatchValueType(env, argv[ARR_INDEX_TWO], napi_string);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasApnId, "The thrid parameter must be ApnId.");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "element name param error");
    ApnPassword apnPassword;
    if (!ParsePwd(env, argv[ARR_INDEX_ONE], apnPassword)) {
        return nullptr;
    }
    std::map<std::string, std::string> apnInfoMap;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseMapStringAndString(env, apnInfoMap, argv[ARR_INDEX_ONE]),
        "apnInfo param error");
    std::map<std::string, std::string> apnInfoMapEx;
    KeyToField(apnInfoMap, apnInfoMapEx);
    ASSERT_AND_THROW_PARAM_ERROR(env, apnInfoMapEx.size() != 0 || apnPassword.password != nullptr,
        "No parameters to update");
    std::string apnId;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, apnId, argv[ARR_INDEX_TWO]) && !apnId.empty(),
        "apnId param error");
    
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->UpdateApn(elementName, apnInfoMapEx, apnId,
        apnPassword);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
#else
    EDMLOGW("NetworkManagerAddon::UpdateApn Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::SetPreferApn(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetPreferApn called");
#if defined(CELLULAR_DATA_EDM_ENABLE)
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    bool hasApnId = MatchValueType(env, argv[ARR_INDEX_ONE], napi_string);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasApnId, "The second parameter must be ApnId.");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "element name param error");

    std::string apnId;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, apnId, argv[ARR_INDEX_ONE]) && !apnId.empty(),
        "apnId param error");

    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->SetPreferApn(elementName, apnId);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
#else
    EDMLOGW("NetworkManagerAddon::SetPreferApn Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value NetworkManagerAddon::ConvertApnInfoToJS(napi_env env, const std::map<std::string, std::string> &apnInfo)
{
    std::map<std::string, std::string> apnInfoEx;
    FieldToKey(apnInfo, apnInfoEx);
    napi_value info = nullptr;
    NAPI_CALL(env, napi_create_object(env, &info));

    for (const auto& iter : apnInfoEx) {
        napi_value napiValue = nullptr;
        std::string key = iter.first;
        std::string value = iter.second;
        NAPI_CALL(env, napi_create_string_utf8(env, value.c_str(), value.length(), &napiValue));
        NAPI_CALL(env, napi_set_named_property(env, info, key.c_str(), napiValue));
    }

    return info;
}

napi_value NetworkManagerAddon::QueryApnInfoById(napi_env env, const OHOS::AppExecFwk::ElementName &admin,
    napi_value param)
{
    std::string apnId;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, apnId, param) && !apnId.empty(), "apnId param error");
    std::map<std::string, std::string> apnInfo;
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->QueryApn(admin, apnId, apnInfo);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }

    return ConvertApnInfoToJS(env, apnInfo);
}

napi_value NetworkManagerAddon::QueryApnIds(napi_env env, const OHOS::AppExecFwk::ElementName &admin, napi_value param)
{
    std::map<std::string, std::string> apnInfo;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseMapStringAndString(env, apnInfo, param),
        "apnInfo param error");
    ASSERT_AND_THROW_PARAM_ERROR(env, apnInfo.find("password") == apnInfo.end(), "pwd can't be used when querying");
    std::map<std::string, std::string> apnInfoEx;
    KeyToField(apnInfo, apnInfoEx);

    std::vector<std::string> apnIds;
    int32_t ret = NetworkManagerProxy::GetNetworkManagerProxy()->QueryApnIds(admin, apnInfoEx, apnIds);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }

    napi_value jsList = nullptr;
    NAPI_CALL(env, napi_create_array_with_length(env, apnIds.size(), &jsList));
    ConvertStringVectorToJS(env, apnIds, jsList);
    return jsList;
}

napi_value NetworkManagerAddon::QueryApn(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_QueryApn called");
#if defined(CELLULAR_DATA_EDM_ENABLE)
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    bool hasApnId = MatchValueType(env, argv[ARR_INDEX_ONE], napi_string);
    bool hasApnInfo = MatchValueType(env, argv[ARR_INDEX_ONE], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, (hasApnId || hasApnInfo), "The second parameter must be ApnId.");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "element name param error");

    if (hasApnId) {
        return QueryApnInfoById(env, elementName, argv[ARR_INDEX_ONE]);
    } else if (hasApnInfo) {
        return QueryApnIds(env, elementName, argv[ARR_INDEX_ONE]);
    }
    return nullptr;
#else
    EDMLOGW("NetworkManagerAddon::SetPreferApn Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

static napi_module g_networkManagerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = NetworkManagerAddon::Init,
    .nm_modname = "enterprise.networkManager",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void NetworkManagerRegister()
{
    napi_module_register(&g_networkManagerModule);
}