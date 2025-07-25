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
#include "wifi_manager_addon.h"

#include "edm_constants.h"
#include "edm_log.h"
#include "hisysevent_adapter.h"
#include "message_parcel_utils.h"
#include "securec.h"

#include "napi_edm_adapter.h"
using namespace OHOS::EDM;

void WifiManagerAddon::CreateWifiSecurityTypeObject(napi_env env, napi_value value)
{
    napi_value nInvalid;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(SecurityType::SEC_TYPE_INVALID), &nInvalid));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_INVALID", nInvalid));
    napi_value nOpen;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(SecurityType::SEC_TYPE_OPEN), &nOpen));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_OPEN", nOpen));
    napi_value nWep;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(SecurityType::SEC_TYPE_WEP), &nWep));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_WEP", nWep));
    napi_value nPsk;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(SecurityType::SEC_TYPE_PSK), &nPsk));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_PSK", nPsk));
    napi_value nSae;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(SecurityType::SEC_TYPE_SAE), &nSae));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_SAE", nSae));
    napi_value nEap;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(SecurityType::SEC_TYPE_EAP), &nEap));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_EAP", nEap));
    napi_value nEapSuiteB;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
        static_cast<int32_t>(SecurityType::SEC_TYPE_EAP_SUITE_B), &nEapSuiteB));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_EAP_SUITE_B", nEapSuiteB));
    napi_value nOwe;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(SecurityType::SEC_TYPE_OWE), &nOwe));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_OWE", nOwe));
    napi_value nWapiCert;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
        static_cast<int32_t>(SecurityType::SEC_TYPE_WAPI_CERT), &nWapiCert));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_WAPI_CERT", nWapiCert));
    napi_value nWapiPsk;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
        static_cast<int32_t>(SecurityType::SEC_TYPE_WAPI_PSK), &nWapiPsk));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "WIFI_SEC_TYPE_WAPI_PSK", nWapiPsk));
}

void WifiManagerAddon::CreateIpTypeObject(napi_env env, napi_value value)
{
    napi_value nStatic;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IpType::STATIC), &nStatic));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "STATIC", nStatic));
    napi_value nDhcp;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(IpType::DHCP), &nDhcp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "DHCP", nDhcp));
    napi_value nUnknown;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env,
        static_cast<int32_t>(IpType::UNKNOWN), &nUnknown));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "UNKNOWN", nUnknown));
}

void WifiManagerAddon::CreateEapMethodObject(napi_env env, napi_value value)
{
    napi_value nNone;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(EapMethod::EAP_NONE), &nNone));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "EAP_NONE", nNone));
    napi_value nPeap;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(EapMethod::EAP_PEAP), &nPeap));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "EAP_PEAP", nPeap));
    napi_value nTls;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(EapMethod::EAP_TLS), &nTls));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "EAP_TLS", nTls));
    napi_value nTtls;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(EapMethod::EAP_TTLS), &nTtls));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "EAP_TTLS", nTtls));
    napi_value nPwd;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(EapMethod::EAP_PWD), &nPwd));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "EAP_PWD", nPwd));
    napi_value nSim;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(EapMethod::EAP_SIM), &nSim));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "EAP_SIM", nSim));
    napi_value nAka;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(EapMethod::EAP_AKA), &nAka));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "EAP_AKA", nAka));
    napi_value nAkaPrime;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(EapMethod::EAP_AKA_PRIME), &nAkaPrime));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "EAP_AKA_PRIME", nAkaPrime));
    napi_value nUnauthTls;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(EapMethod::EAP_UNAUTH_TLS), &nUnauthTls));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "EAP_UNAUTH_TLS", nUnauthTls));
}

void WifiManagerAddon::CreatePhase2MethodObject(napi_env env, napi_value value)
{
#ifdef WIFI_EDM_ENABLE
    napi_value nNone;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(Wifi::Phase2Method::NONE), &nNone));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PHASE2_NONE", nNone));
    napi_value nPap;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(Wifi::Phase2Method::PAP), &nPap));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PHASE2_PAP", nPap));
    napi_value nMSChap;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(Wifi::Phase2Method::MSCHAP), &nMSChap));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PHASE2_MSCHAP", nMSChap));
    napi_value nMSChapV2;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(Wifi::Phase2Method::MSCHAPV2), &nMSChapV2));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PHASE2_MSCHAPV2", nMSChapV2));
    napi_value nGtc;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(Wifi::Phase2Method::GTC), &nGtc));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PHASE2_GTC", nGtc));
    napi_value nSim;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(Wifi::Phase2Method::SIM), &nSim));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PHASE2_SIM", nSim));
    napi_value nAka;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(Wifi::Phase2Method::AKA), &nAka));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PHASE2_AKA", nAka));
    napi_value nAkaPrime;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(Wifi::Phase2Method::AKA_PRIME), &nAkaPrime));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "PHASE2_AKA_PRIME", nAkaPrime));
#endif
}

napi_value WifiManagerAddon::Init(napi_env env, napi_value exports)
{
    napi_value nWifiSecurityType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nWifiSecurityType));
    CreateWifiSecurityTypeObject(env, nWifiSecurityType);

    napi_value nIpType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nIpType));
    CreateIpTypeObject(env, nIpType);

    napi_value nEapMethod = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nEapMethod));
    CreateEapMethodObject(env, nEapMethod);

    napi_value nPhase2Method = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nPhase2Method));
    CreatePhase2MethodObject(env, nPhase2Method);

    napi_property_descriptor property[] = {
        DECLARE_NAPI_FUNCTION("isWifiActive", IsWifiActive),
        DECLARE_NAPI_FUNCTION("setWifiProfile", SetWifiProfile),
        DECLARE_NAPI_FUNCTION("setWifiDisabled", SetWifiDisabled),
        DECLARE_NAPI_FUNCTION("isWifiDisabled", IsWifiDisabled),
        DECLARE_NAPI_FUNCTION("isWifiActiveSync", IsWifiActiveSync),
        DECLARE_NAPI_FUNCTION("setWifiProfileSync", SetWifiProfileSync),
        DECLARE_NAPI_FUNCTION("addAllowedWifiList", AddAllowedWifiList),
        DECLARE_NAPI_FUNCTION("removeAllowedWifiList", RemoveAllowedWifiList),
        DECLARE_NAPI_FUNCTION("getAllowedWifiList", GetAllowedWifiList),
        DECLARE_NAPI_FUNCTION("addDisallowedWifiList", AddDisallowedWifiList),
        DECLARE_NAPI_FUNCTION("removeDisallowedWifiList", RemoveDisallowedWifiList),
        DECLARE_NAPI_FUNCTION("getDisallowedWifiList", GetDisallowedWifiList),
        DECLARE_NAPI_FUNCTION("turnOnWifi", TurnOnWifi),
        DECLARE_NAPI_FUNCTION("turnOffWifi", TurnOffWifi),

        DECLARE_NAPI_PROPERTY("WifiSecurityType", nWifiSecurityType),
        DECLARE_NAPI_PROPERTY("IpType", nIpType),
        DECLARE_NAPI_PROPERTY("EapMethod", nEapMethod),
        DECLARE_NAPI_PROPERTY("Phase2Method", nPhase2Method)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(property) / sizeof(property[0]), property));
    return exports;
}

napi_value WifiManagerAddon::IsWifiActive(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "isWifiActive");
    return IsWifiActiveHandler(env, info, NativeIsWifiActive);
}

napi_value WifiManagerAddon::SetWifiDisabled(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setWifiDisabled");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "setWifiDisabled";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::BOOLEAN};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_11;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t ret = WifiManagerProxy::GetWifiManagerProxy()->SetWifiDisabled(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

napi_value WifiManagerAddon::IsWifiDisabled(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "isWifiDisabled");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "isWifiDisabled";
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT_NULL};
    addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_11;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t ret = ERR_OK;
    bool isDisabled = false;
    ret = WifiManagerProxy::GetWifiManagerProxy()->IsWifiDisabled(adapterAddonData.data, isDisabled);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, isDisabled, &result));
    return result;
}

napi_value WifiManagerAddon::SetWifiProfile(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setWifiProfile");
    return SetWifiProfileHandler(env, info, NativeSetWifiProfile);
}

napi_value WifiManagerAddon::AddAllowedWifiList(napi_env env, napi_callback_info info)
{
    EDMLOGI("WifiManagerAddon::AddAllowedWifiList called");
    return AddOrRemoveAllowedWifiList(env, info, true);
}

napi_value WifiManagerAddon::RemoveAllowedWifiList(napi_env env, napi_callback_info info)
{
    EDMLOGI("WifiManagerAddon::RemoveAllowedWifiList called");
    return AddOrRemoveAllowedWifiList(env, info, false);
}

napi_value WifiManagerAddon::AddOrRemoveAllowedWifiList(napi_env env, napi_callback_info info, bool isAdd)
{
#ifdef WIFI_EDM_ENABLE
    auto convertWifiList2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        std::vector<WifiId> wifiIds;
        if (!ParseWifiInfoArray(env, wifiIds, argv, true)) {
            EDMLOGE("parameter type parse error");
            return false;
        }
        data.WriteUint32(wifiIds.size());
        for (const auto &wifiId : wifiIds) {
            if (!wifiId.Marshalling(data)) {
                EDMLOGE("wifiManagerProxy AddOrRemoveAllowedWifiList: write parcel failed!");
                return false;
            }
        }
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.argsConvert = {nullptr, convertWifiList2Data};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.name = (isAdd ? "addAllowedWifiList" : "removeAllowedWifiList");
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    auto wifiManagerProxy = WifiManagerProxy::GetWifiManagerProxy();
    if (wifiManagerProxy == nullptr) {
        EDMLOGE("can not get usbManagerProxy");
        return nullptr;
    }
    int32_t ret = ERR_OK;
    if (isAdd) {
        ret = wifiManagerProxy->AddOrRemoveWifiList(adapterAddonData.data,
            FuncOperateType::SET, EdmInterfaceCode::ALLOWED_WIFI_LIST);
    } else {
        ret = wifiManagerProxy->AddOrRemoveWifiList(adapterAddonData.data,
            FuncOperateType::REMOVE, EdmInterfaceCode::ALLOWED_WIFI_LIST);
    }
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
#else
    EDMLOGW("WifiManagerAddon::AddOrRemoveAllowedWifiList Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
#endif
    return nullptr;
}

bool WifiManagerAddon::ParseWifiInfoArray(napi_env env, std::vector<WifiId> &wifiIds, napi_value object, bool isAllowed)
{
    bool isArray = false;
    napi_is_array(env, object, &isArray);
    if (!isArray) {
        return false;
    }
    uint32_t arrayLength = 0;
    napi_get_array_length(env, object, &arrayLength);
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value value = nullptr;
        napi_get_element(env, object, i, &value);
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, value, &valueType);
        if (valueType != napi_object) {
            wifiIds.clear();
            return false;
        }
        WifiId wifiId;
        if (!GetWifiIdFromNAPI(env, value, wifiId, isAllowed)) {
            wifiIds.clear();
            return false;
        }
        wifiIds.push_back(wifiId);
    }
    return true;
}

bool WifiManagerAddon::GetWifiIdFromNAPI(napi_env env, napi_value value, WifiId &wifiId, bool isAllowed)
{
    std::string ssid;
    if (!JsObjectToString(env, value, "ssid", true, ssid)) {
        EDMLOGE("AddOrRemoveWifiList ssid parse error!");
        return false;
    }
    if (ssid.empty() || ssid.size() > EdmConstants::WIFI_SSID_MAX_LENGTH) {
        EDMLOGE("AddOrRemoveWifiList ssid is empty or too large!");
        return false;
    }
    std::string bssid;
    if (!JsObjectToString(env, value, "bssid", isAllowed, bssid)) {
        EDMLOGE("AddOrRemoveAllowedWifiList bssid parse error!");
        return false;
    }
    if (isAllowed && bssid.empty()) {
        EDMLOGE("AddOrRemoveAllowedWifiList bssid parse error!");
        return false;
    }
    if (!bssid.empty() && bssid.size() != EdmConstants::WIFI_BSSID_LENGTH) {
        EDMLOGE("AddOrRemoveAllowedWifiList bssid parse error!");
        return false;
    }
    wifiId.SetSsid(ssid);
    wifiId.SetBssid(bssid);
    EDMLOGD("GetWifiIdFromNAPI ssid: %{public}s, bssid: %{public}s", ssid.c_str(), bssid.c_str());
    return true;
}

napi_value WifiManagerAddon::GetWifiList(napi_env env, napi_callback_info info, EdmInterfaceCode policyCode)
{
#ifdef WIFI_EDM_ENABLE
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "getDisallowedWifiList";
    if (policyCode == EdmInterfaceCode::ALLOWED_WIFI_LIST) {
        addonMethodSign.name = "getAllowedWifiList";
    }
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    auto wifiManagerProxy = WifiManagerProxy::GetWifiManagerProxy();
    if (wifiManagerProxy == nullptr) {
        EDMLOGE("can not get wifiManagerProxy");
        return nullptr;
    }
    std::vector<WifiId> wifiIds;
    int32_t ret = wifiManagerProxy->GetWifiList(adapterAddonData.data, wifiIds, policyCode);
    EDMLOGI("WifiManagerAddon::GetWifiList wifiIds return size: %{public}zu", wifiIds.size());
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value jsList = nullptr;
    NAPI_CALL(env, napi_create_array_with_length(env, wifiIds.size(), &jsList));
    for (size_t i = 0; i < wifiIds.size(); i++) {
        napi_value item = WifiIdToJsObj(env, wifiIds[i]);
        NAPI_CALL(env, napi_set_element(env, jsList, i, item));
    }
    return jsList;
#else
    EDMLOGW("WifiManagerAddon::GetWifiList Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value WifiManagerAddon::GetAllowedWifiList(napi_env env, napi_callback_info info)
{
    return GetWifiList(env, info, EdmInterfaceCode::ALLOWED_WIFI_LIST);
}

napi_value WifiManagerAddon::AddDisallowedWifiList(napi_env env, napi_callback_info info)
{
    EDMLOGI("WifiManagerAddon::AddDisallowedWifiList called");
    return AddOrRemoveDisallowedWifiList(env, info, true);
}

napi_value WifiManagerAddon::RemoveDisallowedWifiList(napi_env env, napi_callback_info info)
{
    EDMLOGI("WifiManagerAddon::RemoveDisallowedWifiList called");
    return AddOrRemoveDisallowedWifiList(env, info, false);
}

napi_value WifiManagerAddon::AddOrRemoveDisallowedWifiList(napi_env env, napi_callback_info info, bool isAdd)
{
#ifdef WIFI_EDM_ENABLE
    auto convertWifiList2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        std::vector<WifiId> wifiIds;
        if (!ParseWifiInfoArray(env, wifiIds, argv, false)) {
            EDMLOGE("parameter type parse error");
            return false;
        }
        data.WriteUint32(wifiIds.size());
        for (const auto &wifiId : wifiIds) {
            if (!wifiId.Marshalling(data)) {
                EDMLOGE("wifiManagerProxy AddOrRemoveDisallowedWifiList: write parcel failed!");
                return false;
            }
        }
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.argsConvert = {nullptr, convertWifiList2Data};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.name = (isAdd ? "addDisallowedWifiList" : "removeDisallowedWifiList");
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    auto wifiManagerProxy = WifiManagerProxy::GetWifiManagerProxy();
    if (wifiManagerProxy == nullptr) {
        EDMLOGE("can not get usbManagerProxy");
        return nullptr;
    }
    int32_t ret = ERR_OK;
    if (isAdd) {
        ret = wifiManagerProxy->AddOrRemoveWifiList(adapterAddonData.data,
            FuncOperateType::SET, EdmInterfaceCode::DISALLOWED_WIFI_LIST);
    } else {
        ret = wifiManagerProxy->AddOrRemoveWifiList(adapterAddonData.data,
            FuncOperateType::REMOVE, EdmInterfaceCode::DISALLOWED_WIFI_LIST);
    }
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
#else
    EDMLOGW("WifiManagerAddon::AddOrRemoveDisallowedWifiList Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
#endif
    return nullptr;
}

napi_value WifiManagerAddon::GetDisallowedWifiList(napi_env env, napi_callback_info info)
{
    return GetWifiList(env, info, EdmInterfaceCode::DISALLOWED_WIFI_LIST);
}

napi_value WifiManagerAddon::WifiIdToJsObj(napi_env env, const WifiId &wifiId)
{
    napi_value value = nullptr;
    NAPI_CALL(env, napi_create_object(env, &value));

    napi_value ssid = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, wifiId.GetSsid().c_str(), NAPI_AUTO_LENGTH, &ssid));
    NAPI_CALL(env, napi_set_named_property(env, value, "ssid", ssid));

    napi_value bssid = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, wifiId.GetBssid().c_str(), NAPI_AUTO_LENGTH, &bssid));
    NAPI_CALL(env, napi_set_named_property(env, value, "bssid", bssid));

    return value;
}

#ifdef WIFI_EDM_ENABLE
void WifiManagerAddon::NativeSetWifiProfile(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeSetWifiProfile called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    auto wifiManagerProxy = WifiManagerProxy::GetWifiManagerProxy();
    if (wifiManagerProxy == nullptr) {
        EDMLOGE("can not get WifiManagerProxy");
        return;
    }
    asyncCallbackInfo->ret = wifiManagerProxy->SetWifiProfile(asyncCallbackInfo->data);
}
#endif

void WifiManagerAddon::NativeIsWifiActive(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeIsWifiActive called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    auto wifiManagerProxy = WifiManagerProxy::GetWifiManagerProxy();
    if (wifiManagerProxy == nullptr) {
        EDMLOGE("can not get WifiManagerProxy");
        return;
    }
    asyncCallbackInfo->ret = wifiManagerProxy->IsWifiActive(asyncCallbackInfo->data,
        asyncCallbackInfo->boolRet);
}
#ifdef WIFI_EDM_ENABLE
bool WifiManagerAddon::JsObjToDeviceConfig(napi_env env, napi_value object, Wifi::WifiDeviceConfig &config,
    WifiPassword &pwd)
{
    int32_t type = static_cast<int32_t>(SecurityType::SEC_TYPE_INVALID);
    int32_t ipType = static_cast<int32_t>(IpType::UNKNOWN);
    /* "creatorUid" "disableReason" "randomMacType" "randomMacAddr" is not supported currently */
    std::tuple<int, bool> charArrayProp = {WIFI_PASSWORD_LEN, true};
    std::vector<char> ret;
    if (!JsObjectToString(env, object, "ssid", true, config.ssid) ||
        !JsObjectToString(env, object, "bssid", false, config.bssid) ||
        !JsObjectToCharArray(env, object, "preSharedKey", charArrayProp, ret) ||
        !JsObjectToBool(env, object, "isHiddenSsid", false, config.hiddenSSID) ||
        !JsObjectToInt(env, object, "securityType", true, type) ||
        !JsObjectToInt(env, object, "netId", false, config.networkId) ||
        !JsObjectToInt(env, object, "ipType", false, ipType) ||
        !ProcessIpType(ipType, env, object, config.wifiIpConfig)) {
        return false;
    }
    if (ret.size() != 0) {
        pwd.preSharedKey = (char*) malloc(ret.size());
        if (pwd.preSharedKey == nullptr) {
            memset_s(ret.data(), ret.size(), '\0', ret.size());
            return false;
        }
        if (strncpy_s(pwd.preSharedKey, ret.size(), ret.data(), ret.size()) != ERR_OK) {
            memset_s(ret.data(), ret.size(), '\0', ret.size());
            return false;
        }
        memset_s(ret.data(), ret.size(), '\0', ret.size());
        pwd.preSharedKeySize = ret.size();
    }
    if (!ConvertEncryptionMode(type, config, pwd)) {
        return false;
    }
    if (type == static_cast<int32_t>(SecurityType::SEC_TYPE_EAP)) {
        return ProcessEapConfig(env, object, config.wifiEapConfig, pwd);
    }
    return true;
}

bool WifiManagerAddon::ConvertEncryptionMode(int32_t securityType, Wifi::WifiDeviceConfig &config, WifiPassword &pwd)
{
    switch (securityType) {
        case static_cast<int32_t>(SecurityType::SEC_TYPE_OPEN):
            config.keyMgmt = Wifi::KEY_MGMT_NONE;
            break;
        case static_cast<int32_t>(SecurityType::SEC_TYPE_WEP):
            config.keyMgmt = Wifi::KEY_MGMT_WEP;
            pwd.wepKey = (char*) malloc((pwd.preSharedKeySize + NAPI_RETURN_ONE) * sizeof(char));
            if (pwd.wepKey == nullptr) {
                return false;
            }
            if (strncpy_s(pwd.wepKey, pwd.preSharedKeySize + NAPI_RETURN_ONE,
                pwd.preSharedKey, pwd.preSharedKeySize) != ERR_OK) {
                return false;
            }
            pwd.wepKeySize = pwd.preSharedKeySize;
            EdmUtils::ClearCharArray(pwd.preSharedKey, pwd.preSharedKeySize);
            pwd.preSharedKeySize = 0;
            config.wepTxKeyIndex = 0;
            break;
        case static_cast<int32_t>(SecurityType::SEC_TYPE_PSK):
            config.keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
            break;
        case static_cast<int32_t>(SecurityType::SEC_TYPE_SAE):
            config.keyMgmt = Wifi::KEY_MGMT_SAE;
            break;
        case static_cast<int32_t>(SecurityType::SEC_TYPE_EAP):
            config.keyMgmt = Wifi::KEY_MGMT_EAP;
            break;
        default:
            config.keyMgmt = Wifi::KEY_MGMT_NONE;
            break;
    }
    return true;
}

bool WifiManagerAddon::ProcessIpType(int32_t ipType, napi_env env, napi_value object, Wifi::WifiIpConfig &ipConfig)
{
    switch (ipType) {
        case static_cast<int32_t>(IpType::DHCP):
            ipConfig.assignMethod = Wifi::AssignIpMethod::DHCP;
            break;
        case static_cast<int32_t>(IpType::STATIC):
            ipConfig.assignMethod = Wifi::AssignIpMethod::STATIC;
            break;
        default:
            ipConfig.assignMethod = Wifi::AssignIpMethod::UNASSIGNED;
            break;
    }
    if (ipType == static_cast<int32_t>(IpType::STATIC) && !ConfigStaticIp(env, object, ipConfig)) {
        return false;
    }
    return true;
}

bool WifiManagerAddon::ConfigStaticIp(napi_env env, napi_value object, Wifi::WifiIpConfig &ipConfig)
{
    napi_value staticIp;
    napi_value dnsServers;
    napi_value primaryDns;
    napi_value secondDns;
    if (!GetJsProperty(env, object, "staticIp", staticIp) ||
        !JsObjectToUint(env, staticIp, "ipAddress", true,
        ipConfig.staticIpAddress.ipAddress.address.addressIpv4) ||
        !JsObjectToUint(env, staticIp, "gateway", true,
        ipConfig.staticIpAddress.gateway.addressIpv4) ||
        !JsObjectToInt(env, staticIp, "prefixLength", true,
        ipConfig.staticIpAddress.ipAddress.prefixLength) ||
        !GetJsProperty(env, staticIp, "dnsServers", dnsServers)) {
        return false;
    }
    ipConfig.staticIpAddress.ipAddress.address.family = 0;
    uint32_t arrayLength = 0;
    const uint32_t DNS_NUM = 2;
    napi_get_array_length(env, dnsServers, &arrayLength);
    if (arrayLength != DNS_NUM) {
        EDMLOGI("ConfigStaticIp, It needs two dns servers.");
        return false;
    }
    napi_get_element(env, dnsServers, 0, &primaryDns);
    napi_get_element(env, dnsServers, 1, &secondDns);
    napi_get_value_uint32(env, primaryDns, &ipConfig.staticIpAddress.dnsServer1.addressIpv4);
    napi_get_value_uint32(env, secondDns, &ipConfig.staticIpAddress.dnsServer2.addressIpv4);
    return true;
}

bool WifiManagerAddon::ProcessEapConfig(napi_env env, napi_value object, Wifi::WifiEapConfig &eapConfig,
    WifiPassword &pwd)
{
    napi_value napiEap;
    int32_t eapMethod = static_cast<int32_t>(EapMethod::EAP_NONE);
    if (!GetJsProperty(env, object, "eapProfile", napiEap) ||
        !JsObjectToInt(env, napiEap, "eapMethod", true, eapMethod)) {
        return false;
    }
    switch (eapMethod) {
        case static_cast<int32_t>(EapMethod::EAP_PEAP):
            ProcessEapPeapConfig(env, napiEap, eapConfig, pwd);
            break;
        case static_cast<int32_t>(EapMethod::EAP_TLS):
            ProcessEapTlsConfig(env, napiEap, eapConfig);
            break;
        default:
            EDMLOGI("EapMethod: %{public}d unsupported", eapMethod);
            return false;
    }
    return true;
}

bool WifiManagerAddon::ProcessEapPeapConfig(napi_env env, napi_value object, Wifi::WifiEapConfig &eapConfig,
    WifiPassword &pwd)
{
    eapConfig.eap = Wifi::EAP_METHOD_PEAP;
    int32_t phase2 = static_cast<int32_t>(Wifi::Phase2Method::NONE);
    std::tuple<int, bool> charArrayProp = {WIFI_PASSWORD_LEN, true};
    std::vector<char> ret;
    if (!JsObjectToString(env, object, "identity", true, eapConfig.identity) ||
        !JsObjectToCharArray(env, object, "password", charArrayProp, ret) ||
        !JsObjectToInt(env, object, "phase2Method", true, phase2)) {
        return false;
    }
    if (ret.size() != 0) {
        pwd.password = (char*) malloc(ret.size());
        if (pwd.password == nullptr) {
            memset_s(ret.data(), ret.size(), '\0', ret.size());
            return false;
        }
        if (strncpy_s(pwd.password, ret.size(), ret.data(), ret.size()) != ERR_OK) {
            memset_s(ret.data(), ret.size(), '\0', ret.size());
            return false;
        }
        memset_s(ret.data(), ret.size(), '\0', ret.size());
        pwd.preSharedKeySize = ret.size();
    }
    MessageParcelUtils::ProcessPhase2Method(phase2, eapConfig);
    return true;
}

bool WifiManagerAddon::ProcessEapTlsConfig(napi_env env, napi_value object, Wifi::WifiEapConfig &eapConfig)
{
    eapConfig.eap = Wifi::EAP_METHOD_TLS;
    std::tuple<int, bool> charArrayProp = {WIFI_PASSWORD_LEN, true};
    std::vector<char> ret;
    if (!JsObjectToString(env, object, "identity", true, eapConfig.identity) ||
        !JsObjectToCharArray(env, object, "certPassword", charArrayProp, ret) ||
        !JsObjectToU8Vector(env, object, "certEntry", eapConfig.certEntry)) {
        return false;
    }
    if (ret.size() != 0) {
        if (strncpy_s(eapConfig.certPassword, ret.size(), ret.data(), ret.size()) != ERR_OK) {
            memset_s(ret.data(), ret.size(), '\0', ret.size());
            return false;
        }
        memset_s(ret.data(), ret.size(), '\0', ret.size());
    }
    return true;
}
#endif

napi_value WifiManagerAddon::IsWifiActiveSync(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "isWifiActiveSync");
    return IsWifiActiveHandler(env, info, nullptr);
}

napi_value WifiManagerAddon::IsWifiActiveHandler(napi_env env,
    napi_callback_info info, napi_async_execute_callback execute)
{
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "isWifiActive";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_12;
    if (execute != nullptr) {
        addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_11;
        return AddonMethodAdapter(env, info, addonMethodSign, execute, NativeBoolCallbackComplete);
    }
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    auto wifiManagerProxy = WifiManagerProxy::GetWifiManagerProxy();
    if (wifiManagerProxy == nullptr) {
        EDMLOGE("can not get WifiManagerProxy");
        return nullptr;
    }
    bool isActive = false;
    int32_t ret = wifiManagerProxy->IsWifiActive(adapterAddonData.data, isActive);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, isActive, &result));
    return result;
}

napi_value WifiManagerAddon::SetWifiProfileSync(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setWifiProfileSync");
    return SetWifiProfileHandler(env, info, nullptr);
}

napi_value WifiManagerAddon::SetWifiProfileHandler(napi_env env,
    napi_callback_info info, napi_async_execute_callback execute)
{
#ifdef WIFI_EDM_ENABLE
    auto convertWifiDeviceConfigAndPwd2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        Wifi::WifiDeviceConfig config;
        WifiPassword pwd;
        bool parseRet = JsObjToDeviceConfig(env, argv, config, pwd);
        if (!parseRet) {
            napi_throw(env, CreateError(env, EdmReturnErrCode::PARAM_ERROR, "parameter profile parse error"));
            return false;
        }
        MessageParcelUtils::WriteWifiDeviceConfig(config, data, pwd);
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "setWifiProfile";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.argsConvert = {nullptr, convertWifiDeviceConfigAndPwd2Data};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_12;
    if (execute != nullptr) {
        addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_11;
        return AddonMethodAdapter(env, info, addonMethodSign, execute, NativeVoidCallbackComplete);
    }
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    auto wifiManagerProxy = WifiManagerProxy::GetWifiManagerProxy();
    if (wifiManagerProxy == nullptr) {
        EDMLOGE("can not get WifiManagerProxy");
        return nullptr;
    }
    int32_t ret = wifiManagerProxy->SetWifiProfile(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
#else
    EDMLOGW("WifiManagerAddon::SetWifiProfileSync Unsupported Capabilities.");
    napi_throw(env, CreateError(env, EdmReturnErrCode::INTERFACE_UNSUPPORTED));
    return nullptr;
#endif
}

napi_value WifiManagerAddon::TurnOnWifi(napi_env env, napi_callback_info info)
{
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "TurnOnWifi";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::BOOLEAN};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t ret = WifiManagerProxy::GetWifiManagerProxy()->TurnOnWifi(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

napi_value WifiManagerAddon::TurnOffWifi(napi_env env, napi_callback_info info)
{
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "TurnOffWifi";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t ret = WifiManagerProxy::GetWifiManagerProxy()->TurnOffWifi(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

static napi_module g_wifiManagerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = WifiManagerAddon::Init,
    .nm_modname = "enterprise.wifiManager",
    .nm_priv = ((void *)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void WifiManagerRegister()
{
    napi_module_register(&g_wifiManagerModule);
}