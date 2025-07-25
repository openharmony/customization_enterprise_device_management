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
#include "device_settings_addon.h"

#include "cJSON.h"
#include "cjson_check.h"
#include "datetime_manager_proxy.h"
#include "edm_constants.h"
#include "edm_log.h"
#include "hisysevent_adapter.h"
#include "napi_edm_adapter.h"

using namespace OHOS::EDM;

void DeviceSettingsAddon::CreatePowerSceneObject(napi_env env, napi_value value)
{
    napi_value nTimeOut;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, static_cast<uint32_t>(PowerScene::TIME_OUT), &nTimeOut));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "TIME_OUT", nTimeOut));
}

void DeviceSettingsAddon::CreatePowerPolicyActionObject(napi_env env, napi_value value)
{
    napi_value nActionNone;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_uint32(env, static_cast<uint32_t>(PowerPolicyAction::NONE), &nActionNone));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "NONE", nActionNone));
    napi_value nActionAutoSuspend;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_uint32(env, static_cast<uint32_t>(PowerPolicyAction::AUTO_SUSPEND), &nActionAutoSuspend));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "AUTO_SUSPEND", nActionAutoSuspend));
    napi_value nActionForceSuspend;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_uint32(env, static_cast<uint32_t>(PowerPolicyAction::FORCE_SUSPEND), &nActionForceSuspend));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "FORCE_SUSPEND", nActionForceSuspend));
    napi_value nActionHibernate;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_uint32(env, static_cast<uint32_t>(PowerPolicyAction::HIBERNATE), &nActionHibernate));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "HIBERNATE", nActionHibernate));
    napi_value nActionShutDown;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_uint32(env, static_cast<uint32_t>(PowerPolicyAction::SHUTDOWN), &nActionShutDown));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SHUTDOWN", nActionShutDown));
}

napi_value DeviceSettingsAddon::Init(napi_env env, napi_value exports)
{
    napi_value nTimeOut = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nTimeOut));
    CreatePowerSceneObject(env, nTimeOut);
    napi_value nPolicyAction = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nPolicyAction));
    CreatePowerPolicyActionObject(env, nPolicyAction);

    napi_property_descriptor property[] = {
        DECLARE_NAPI_FUNCTION("setScreenOffTime", SetScreenOffTime),
        DECLARE_NAPI_FUNCTION("getScreenOffTime", GetScreenOffTime),
        DECLARE_NAPI_FUNCTION("setPowerPolicy", SetPowerPolicy),
        DECLARE_NAPI_FUNCTION("getPowerPolicy", GetPowerPolicy),
        DECLARE_NAPI_FUNCTION("installUserCertificate", InstallUserCertificate),
        DECLARE_NAPI_FUNCTION("uninstallUserCertificate", UninstallUserCertificate),
        DECLARE_NAPI_PROPERTY("PowerScene", nTimeOut),
        DECLARE_NAPI_PROPERTY("PowerPolicyAction", nPolicyAction),
        DECLARE_NAPI_FUNCTION("setValue", SetValue),
        DECLARE_NAPI_FUNCTION("getValue", GetValue),
        DECLARE_NAPI_FUNCTION("setHomeWallpaper", SetHomeWallPaper),
        DECLARE_NAPI_FUNCTION("setUnlockWallpaper", SetUnlockWallPaper)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(property) / sizeof(property[0]), property));
    return exports;
}

napi_value DeviceSettingsAddon::SetPowerPolicy(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetPowerPolicy called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setPowerPolicy");
    auto convertPowerScene2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        PowerScene powerScene;
        bool isUint = JsObjToPowerScene(env, argv, powerScene);
        if (!isUint) {
            return false;
        }
        data.WriteUint32(static_cast<uint32_t>(powerScene));
        return true;
    };
    auto convertPowerPolicy2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        PowerPolicy powerPolicy;
        bool isUint = JsObjToPowerPolicy(env, argv, powerPolicy);
        if (!isUint) {
            return false;
        }
        if (!powerPolicy.Marshalling(data)) {
            EDMLOGE("DeviceSettingsProxy::SetPowerPolicy Marshalling proxy fail.");
            return false;
        }
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "SetPowerPolicy";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM, EdmAddonCommonType::CUSTOM};
    addonMethodSign.argsConvert = {nullptr, convertPowerScene2Data, convertPowerPolicy2Data};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t ret = DeviceSettingsProxy::GetDeviceSettingsProxy()->SetPowerPolicy(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        EDMLOGE("SetPowerPolicy failed!");
    }
    return nullptr;
}

napi_value DeviceSettingsAddon::GetPowerPolicy(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetPowerPolicy called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getPowerPolicy");
    auto convertPowerScene2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        PowerScene powerScene;
        bool isUint = JsObjToPowerScene(env, argv, powerScene);
        if (!isUint) {
            return false;
        }
        data.WriteUint32(static_cast<uint32_t>(powerScene));
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "GetPowerPolicy";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.argsConvert = {nullptr, convertPowerScene2Data};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    AdapterAddonData adapterAddonData{};
    if (JsObjectToData(env, info, addonMethodSign, &adapterAddonData) == nullptr) {
        return nullptr;
    }
    PowerPolicy powerPolicy;
    int32_t ret = DeviceSettingsProxy::GetDeviceSettingsProxy()->GetPowerPolicy(adapterAddonData.data, powerPolicy);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        EDMLOGE("SetPowerPolicy failed!");
        return nullptr;
    }
    napi_value result = ConvertPolicyPolicyToJs(env, powerPolicy);
    return result;
}

bool DeviceSettingsAddon::JsObjToPowerScene(napi_env env, napi_value object, PowerScene &powerScene)
{
    uint32_t powerInt = 0;
    if (ParseUint(env, powerInt, object) && powerInt == static_cast<uint32_t>(PowerScene::TIME_OUT)) {
        powerScene = PowerScene(powerInt);
        return true;
    }
    return false;
}

bool DeviceSettingsAddon::JsObjToPowerPolicy(napi_env env, napi_value object, PowerPolicy &powerPolicy)
{
    uint32_t action = 0;
    if (!JsObjectToUint(env, object, "powerPolicyAction", true, action)) {
        EDMLOGE("SetPowerPolicy powerPolicyAction parse error!");
        return false;
    }
    uint32_t delayTime = 0;
    if (!JsObjectToUint(env, object, "delayTime", true, delayTime)) {
        EDMLOGE("SetPowerPolicy delayTime parse error!");
        return false;
    }
    powerPolicy.SetDelayTime(delayTime);
    if (!powerPolicy.SetPowerPolicyAction(action)) {
        return false;
    }
    return true;
}

napi_value DeviceSettingsAddon::ConvertPolicyPolicyToJs(napi_env env, PowerPolicy &powerPolicy)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));
    napi_value action = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, static_cast<uint32_t>(powerPolicy.GetPowerPolicyAction()), &action));
    napi_value delayTime = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, powerPolicy.GetDelayTime(), &delayTime));
    NAPI_CALL(env, napi_set_named_property(env, result, "powerPolicyAction", action));
    NAPI_CALL(env, napi_set_named_property(env, result, "delayTime", delayTime));
    return result;
}

napi_value DeviceSettingsAddon::SetScreenOffTime(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetScreenOffTime called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setScreenOffTime");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "SetScreenOffTime";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::INT32};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_11;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t ret = DeviceSettingsProxy::GetDeviceSettingsProxy()->SetScreenOffTime(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        EDMLOGE("SetScreenOffTime failed!");
    }
    return nullptr;
}

napi_value DeviceSettingsAddon::GetScreenOffTime(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetScreenOffTime called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getScreenOffTime");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "GetScreenOffTime";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    addonMethodSign.apiVersionTag = EdmConstants::PERMISSION_TAG_VERSION_11;
    return AddonMethodAdapter(env, info, addonMethodSign, NativeGetScreenOffTime, NativeNumberCallbackComplete);
}

void DeviceSettingsAddon::NativeGetScreenOffTime(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeGetScreenOffTime called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    asyncCallbackInfo->ret = DeviceSettingsProxy::GetDeviceSettingsProxy()->GetScreenOffTime(
        asyncCallbackInfo->data, asyncCallbackInfo->intRet);
}

napi_value DeviceSettingsAddon::InstallUserCertificate(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_InstallUserCertificate called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "installUserCertificate");
    auto convertCertBlob2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        CertBlob certBlob;
        bool retCertBlob = ParseCertBlob(env, argv, certBlob);
        if (!retCertBlob) {
            return false;
        }
        data.WriteUInt8Vector(certBlob.certArray);
        data.WriteString(certBlob.alias);
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "InstallUserCertificate";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.argsConvert = {nullptr, convertCertBlob2Data};
    return AddonMethodAdapter(env, info, addonMethodSign, NativeInstallUserCertificate, NativeStringCallbackComplete);
}

void DeviceSettingsAddon::NativeInstallUserCertificate(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeInstallUserCertificate called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    asyncCallbackInfo->ret = DeviceSettingsProxy::GetDeviceSettingsProxy()->InstallUserCertificate(
        asyncCallbackInfo->data, asyncCallbackInfo->stringRet, asyncCallbackInfo->innerCodeMsg);
}

napi_value DeviceSettingsAddon::UninstallUserCertificate(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_UninstallUserCertificate called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "uninstallUserCertificate");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "InstallUserCertificate";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::STRING};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    return AddonMethodAdapter(env, info, addonMethodSign, NativeUninstallUserCertificate, NativeVoidCallbackComplete);
}

void DeviceSettingsAddon::NativeUninstallUserCertificate(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeUninstallUserCertificate called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    asyncCallbackInfo->ret = DeviceSettingsProxy::GetDeviceSettingsProxy()->UninstallUserCertificate(
        asyncCallbackInfo->data, asyncCallbackInfo->innerCodeMsg);
}

bool DeviceSettingsAddon::ParseCertBlob(napi_env env, napi_value object, CertBlob &certBlob)
{
    napi_valuetype type = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, object, &type), false);
    if (type != napi_object) {
        EDMLOGE("type of param certblob is not object");
        return false;
    }
    if (!JsObjectToU8Vector(env, object, "inData", certBlob.certArray)) {
        EDMLOGE("uint8Array to vector failed");
        return false;
    }
    return JsObjectToString(env, object, "alias", true, certBlob.alias);
}

napi_value DeviceSettingsAddon::SetValue(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetValue called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setValue");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = { nullptr };
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_THREE, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "parameter admin error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_string), "parameter item error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_TWO], napi_string), "parameter value error");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "element name param error");
    std::string item;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, item, argv[ARR_INDEX_ONE]), "param 'item' error");
    std::string value;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, value, argv[ARR_INDEX_TWO]), "param 'value' error");
    int32_t ret = ERR_OK;
    auto proxy = DeviceSettingsProxy::GetDeviceSettingsProxy();
    if (item == EdmConstants::DeviceSettings::SCREEN_OFF) {
        int32_t screenOffTime = 0;
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseStringToInt(value, screenOffTime), "param 'screenOffTime' error");
        ret = proxy->SetScreenOffTime(elementName, screenOffTime, EdmConstants::PERMISSION_TAG_VERSION_12);
    } else if (item == EdmConstants::DeviceSettings::POWER_POLICY) {
        PowerScene powerScene;
        ASSERT_AND_THROW_PARAM_ERROR(env, JsStrToPowerScene(env, value, powerScene), "param 'powerScene' error");
        PowerPolicy powerPolicy;
        ASSERT_AND_THROW_PARAM_ERROR(env, JsStrToPowerPolicy(env, value, powerPolicy), "param 'powerPolicy' error");
        ret = proxy->SetPowerPolicy(elementName, powerScene, powerPolicy);
    } else if (item == EdmConstants::DeviceSettings::DATE_TIME) {
        int64_t dateTime = 0;
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseStringToLong(value, dateTime), "param 'dateTime' error");
        ret = DatetimeManagerProxy::GetDatetimeManagerProxy()->SetDateTime(elementName, dateTime,
            EdmConstants::PERMISSION_TAG_VERSION_12);
    } else {
        ret = EdmReturnErrCode::INTERFACE_UNSUPPORTED;
    }
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        EDMLOGE("SetValue failed! item is %{public}s", item.c_str());
    }
    return nullptr;
}

napi_value DeviceSettingsAddon::GetValue(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetValue called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getValue");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "parameter admin error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_string), "parameter item error");
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "element name param error");
    std::string item;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, item, argv[ARR_INDEX_ONE]), "param 'item' error");
    int32_t ret = ERR_OK;
    std::string stringRet;
    auto proxy = DeviceSettingsProxy::GetDeviceSettingsProxy();
    if (item == EdmConstants::DeviceSettings::SCREEN_OFF) {
        int32_t screenOffTime;
        ret = proxy->GetScreenOffTime(elementName, screenOffTime, EdmConstants::PERMISSION_TAG_VERSION_12);
        stringRet = std::to_string(screenOffTime);
    } else if (item == EdmConstants::DeviceSettings::POWER_POLICY) {
        PowerScene powerScene = PowerScene::TIME_OUT;
        PowerPolicy powerPolicy;
        ret = proxy->GetPowerPolicy(elementName, powerScene, powerPolicy);
        if (SUCCEEDED(ret)) {
            ret = ConvertPowerPolicyToJsStr(env, powerScene, powerPolicy, stringRet);
        }
    } else {
        ret = EdmReturnErrCode::INTERFACE_UNSUPPORTED;
    }
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        EDMLOGE("GetValue failed! item is %{public}s", item.c_str());
        return nullptr;
    }
    napi_value result;
    NAPI_CALL(env, napi_create_string_utf8(env, stringRet.c_str(), stringRet.size(), &result));
    return result;
}

bool DeviceSettingsAddon::JsStrToPowerScene(napi_env env, std::string jsStr, PowerScene &powerScene)
{
    cJSON *json = cJSON_Parse(jsStr.c_str());
    if (json == nullptr) {
        return false;
    }
    cJSON *itemPowerScene = cJSON_GetObjectItem(json, "powerScene");
    if (!cJSON_IsNumber(itemPowerScene)) {
        cJSON_Delete(json);
        return false;
    }
    powerScene = PowerScene(itemPowerScene->valueint);
    cJSON_Delete(json);
    return true;
}

bool DeviceSettingsAddon::JsStrToPowerPolicy(napi_env env, std::string jsStr, PowerPolicy &powerPolicy)
{
    cJSON *json = cJSON_Parse(jsStr.c_str());
    if (json == nullptr) {
        return false;
    }
    cJSON *itemPowerPolicy = cJSON_GetObjectItem(json, "powerPolicy");
    if (!cJSON_IsObject(itemPowerPolicy)) {
        cJSON_Delete(json);
        return false;
    }
    cJSON *delayTime = cJSON_GetObjectItem(itemPowerPolicy, "delayTime");
    if (!cJSON_IsNumber(delayTime)) {
        cJSON_Delete(json);
        return false;
    }
    powerPolicy.SetDelayTime(delayTime->valueint);
    cJSON *powerPolicyAction = cJSON_GetObjectItem(itemPowerPolicy, "powerPolicyAction");
    if (!cJSON_IsNumber(powerPolicyAction)) {
        cJSON_Delete(json);
        return false;
    }
    bool setActionRet = powerPolicy.SetPowerPolicyAction(powerPolicyAction->valueint);
    cJSON_Delete(json);
    return setActionRet;
}

int32_t DeviceSettingsAddon::ConvertPowerPolicyToJsStr(napi_env env, PowerScene &powerScene, PowerPolicy &powerPolicy,
    std::string &info)
{
    cJSON *json = nullptr;
    CJSON_CREATE_OBJECT_AND_CHECK(json, EdmReturnErrCode::SYSTEM_ABNORMALLY);
    cJSON *powerPoilcyJs = nullptr;
    CJSON_CREATE_OBJECT_AND_CHECK_AND_CLEAR(powerPoilcyJs, EdmReturnErrCode::SYSTEM_ABNORMALLY, json);
    cJSON_AddNumberToObject(powerPoilcyJs, "powerPolicyAction",
        static_cast<uint32_t>(powerPolicy.GetPowerPolicyAction()));
    cJSON_AddNumberToObject(powerPoilcyJs, "delayTime", powerPolicy.GetDelayTime());
    cJSON_AddNumberToObject(json, "powerScene", static_cast<uint32_t>(powerScene));
    bool ret = cJSON_AddItemToObject(json, "powerPolicy", powerPoilcyJs);
    if (!ret) {
        cJSON_Delete(json);
        cJSON_Delete(powerPoilcyJs);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    char *jsonStr = cJSON_PrintUnformatted(json);
    if (jsonStr == nullptr) {
        cJSON_Delete(json);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    info = std::string(jsonStr);
    cJSON_Delete(json);
    cJSON_free(jsonStr);
    return ERR_OK;
}

napi_value DeviceSettingsAddon::SetHomeWallPaper(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetHomeWallPaper called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "SetHomeWallPaper");
    return SetWallPaper(env, info, true);
}

napi_value DeviceSettingsAddon::SetUnlockWallPaper(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetUnlockWallPaper called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "SetUnlockWallPaper");
    return SetWallPaper(env, info, false);
}

napi_value DeviceSettingsAddon::SetWallPaper(napi_env env, napi_callback_info info, bool isHomeWallPaper)
{
    auto convertFd2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        int32_t fd = -1;
        if (!ParseInt(env, fd, argv)) {
            return false;
        }
        data.WriteFileDescriptor(fd);
        return true;
    };

    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "SetWallPaper";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.argsConvert = {nullptr, convertFd2Data};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.apiVersionTag = WITHOUT_PERMISSION_TAG;
    if (isHomeWallPaper) {
        return AddonMethodAdapter(env, info, addonMethodSign, NativeSetHomeWallPaper, NativeVoidCallbackComplete);
    }
    return AddonMethodAdapter(env, info, addonMethodSign, NativeSetUnlockWallPaper, NativeVoidCallbackComplete);
}

void DeviceSettingsAddon::NativeSetHomeWallPaper(napi_env env, void* data)
{
    NativeSetWallPaper(env, data, true);
}

void DeviceSettingsAddon::NativeSetUnlockWallPaper(napi_env env, void* data)
{
    NativeSetWallPaper(env, data, false);
}

void DeviceSettingsAddon::NativeSetWallPaper(napi_env env, void* data, bool isHomeWallPaper)
{
    EDMLOGI("NativeSetWallPaper called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AdapterAddonData *asyncCallbackInfo = static_cast<AdapterAddonData *>(data);
    asyncCallbackInfo->data.WriteBool(isHomeWallPaper);
    int32_t ret = DeviceSettingsProxy::GetDeviceSettingsProxy()->SetWallPaper(asyncCallbackInfo->data,
        asyncCallbackInfo->innerCodeMsg);
    if (FAILED(ret)) {
        asyncCallbackInfo->ret = ret;
    }
}

static napi_module g_deviceSettingsModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = DeviceSettingsAddon::Init,
    .nm_modname = "enterprise.deviceSettings",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void DeviceSettingsRegister()
{
    napi_module_register(&g_deviceSettingsModule);
}