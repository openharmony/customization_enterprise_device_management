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

#include "security_manager_addon.h"

#include <fcntl.h>
#include <fstream>
#include "cJSON.h"
#include "cjson_check.h"
#include "device_settings_proxy.h"
#include "edm_constants.h"
#include "edm_log.h"
#include "napi_edm_adapter.h"
#include "pixel_map_napi.h"

using namespace OHOS::EDM;

constexpr int64_t MAX_VALIDITY_PERIOD = 31536000000000; // 60 * 60 * 24 * 365 * 1000 * 1000
constexpr int32_t MAX_WATERMARK_IMAGE_SIZE = 512000; // 500 * 1024
static const std::string VALIDITY_PERIOD_OUT_OF_RANGE_ERROR = "validityPeriod out of range!";

napi_value SecurityManagerAddon::Init(napi_env env, napi_value exports)
{
    napi_value nClipboardPolicy = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nClipboardPolicy));
    CreateClipboardPolicyObject(env, nClipboardPolicy);
    napi_property_descriptor property[] = {
        DECLARE_NAPI_FUNCTION("getSecurityPatchTag", GetSecurityPatchTag),
        DECLARE_NAPI_FUNCTION("getDeviceEncryptionStatus", GetDeviceEncryptionStatus),
        DECLARE_NAPI_FUNCTION("setPasswordPolicy", SetPasswordPolicy),
        DECLARE_NAPI_FUNCTION("getPasswordPolicy", GetPasswordPolicy),
        DECLARE_NAPI_FUNCTION("getSecurityStatus", GetSecurityStatus),
        DECLARE_NAPI_FUNCTION("installUserCertificate", InstallUserCertificate),
        DECLARE_NAPI_FUNCTION("uninstallUserCertificate", UninstallUserCertificate),
        DECLARE_NAPI_FUNCTION("getUserCertificates", GetUserCertificates),
        DECLARE_NAPI_FUNCTION("setAppClipboardPolicy", SetAppClipboardPolicy),
        DECLARE_NAPI_FUNCTION("getAppClipboardPolicy", GetAppClipboardPolicy),
        DECLARE_NAPI_FUNCTION("setWatermarkImage", SetWatermarkImage),
        DECLARE_NAPI_FUNCTION("cancelWatermarkImage", CancelWatermarkImage),
        DECLARE_NAPI_PROPERTY("ClipboardPolicy", nClipboardPolicy),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(property) / sizeof(property[0]), property));
    return exports;
}

napi_value SecurityManagerAddon::GetSecurityPatchTag(napi_env env, napi_callback_info info)
{
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "GetSecurityPatchTag";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    auto securityManagerProxy = SecurityManagerProxy::GetSecurityManagerProxy();
    int32_t ret = securityManagerProxy->GetSecurityPatchTag(adapterAddonData.data, adapterAddonData.stringRet);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value securityPatchTag;
    NAPI_CALL(env, napi_create_string_utf8(env, adapterAddonData.stringRet.c_str(),
        adapterAddonData.stringRet.size(), &securityPatchTag));
    return securityPatchTag;
}

napi_value SecurityManagerAddon::GetDeviceEncryptionStatus(napi_env env, napi_callback_info info)
{
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "GetDeviceEncryptionStatus";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    DeviceEncryptionStatus deviceEncryptionStatus;
    auto securityManagerProxy = SecurityManagerProxy::GetSecurityManagerProxy();
    int32_t ret = securityManagerProxy->GetDeviceEncryptionStatus(adapterAddonData.data, deviceEncryptionStatus);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    return ConvertDeviceEncryptionStatus(env, deviceEncryptionStatus);
}

napi_value SecurityManagerAddon::ConvertDeviceEncryptionStatus(napi_env env,
    DeviceEncryptionStatus &deviceEncryptionStatus)
{
    napi_value objDeviceEncryptionStatus = nullptr;
    NAPI_CALL(env, napi_create_object(env, &objDeviceEncryptionStatus));
    napi_value nIsEncrypted;
    NAPI_CALL(env, napi_get_boolean(env, deviceEncryptionStatus.isEncrypted, &nIsEncrypted));
    NAPI_CALL(env, napi_set_named_property(env, objDeviceEncryptionStatus, "isEncrypted", nIsEncrypted));
    return objDeviceEncryptionStatus;
}

napi_value SecurityManagerAddon::SetPasswordPolicy(napi_env env, napi_callback_info info)
{
    auto convertpasswordPolicy2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
            PasswordPolicy policy;
            if (!JsObjectToString(env, argv, "complexityRegex", false, policy.complexityReg)) {
                EDMLOGE("Parameter passwordPolicy error");
                return false;
            }
            if (!JsObjectToLong(env, argv, "validityPeriod", false, policy.validityPeriod)) {
                EDMLOGE("Parameter passwordPolicy error");
                return false;
            }
            if (policy.validityPeriod > MAX_VALIDITY_PERIOD || policy.validityPeriod < 0) {
                napi_throw(env, CreateError(env, EdmReturnErrCode::PARAM_ERROR, VALIDITY_PERIOD_OUT_OF_RANGE_ERROR));
                return false;
            }
            if (!JsObjectToString(env, argv, "additionalDescription", false, policy.additionalDescription)) {
                EDMLOGE("Parameter passwordPolicy error");
                return false;
            }
            data.WriteString(policy.complexityReg);
            data.WriteInt64(policy.validityPeriod);
            data.WriteString(policy.additionalDescription);
            return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "SetPasswordPolicy";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.argsConvert = {nullptr, convertpasswordPolicy2Data};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t retCode = SecurityManagerProxy::GetSecurityManagerProxy()->SetPasswordPolicy(adapterAddonData.data);
    if (FAILED(retCode)) {
        napi_throw(env, CreateError(env, retCode));
    }
    return nullptr;
}

napi_value SecurityManagerAddon::GetPasswordPolicy(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetPasswordPolicy called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_value thisArg = nullptr;
    void* data = nullptr;
    OHOS::AppExecFwk::ElementName elementName;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));

    PasswordPolicy policy;
    int32_t retCode = EdmReturnErrCode::SYSTEM_ABNORMALLY;
    if (argc >= ARGS_SIZE_ONE) {
        ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "admin type error");
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "Parameter admin error");
        retCode = SecurityManagerProxy::GetSecurityManagerProxy()->GetPasswordPolicy(elementName, policy);
    } else {
        retCode = SecurityManagerProxy::GetSecurityManagerProxy()->GetPasswordPolicy(policy);
    }

    if (FAILED(retCode)) {
        napi_throw(env, CreateError(env, retCode));
        return nullptr;
    }
    napi_value ret;
    napi_value complexityReg;
    napi_value validityPeriod;
    napi_value additionalDescription;
    NAPI_CALL(env, napi_create_object(env, &ret));
    NAPI_CALL(env, napi_create_string_utf8(env, policy.complexityReg.c_str(), NAPI_AUTO_LENGTH, &complexityReg));
    NAPI_CALL(env, napi_create_int64(env, policy.validityPeriod, &validityPeriod));
    NAPI_CALL(env,
        napi_create_string_utf8(env, policy.additionalDescription.c_str(), NAPI_AUTO_LENGTH, &additionalDescription));
    NAPI_CALL(env, napi_set_named_property(env, ret, "complexityRegex", complexityReg));
    NAPI_CALL(env, napi_set_named_property(env, ret, "validityPeriod", validityPeriod));
    NAPI_CALL(env, napi_set_named_property(env, ret, "additionalDescription", additionalDescription));
    return ret;
}

napi_value SecurityManagerAddon::GetSecurityStatus(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetSecurityStatus called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = { nullptr };
    napi_value thisArg = nullptr;
    void* data = nullptr;
    OHOS::AppExecFwk::ElementName elementName;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "parameter type error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_string),
        "The second parameter must be string.");
    bool boolret = ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]);
    ASSERT_AND_THROW_PARAM_ERROR(env, boolret, "element name param error");
    EDMLOGD(
        "EnableAdmin: elementName.bundlename %{public}s, "
        "elementName.abilityname:%{public}s",
        elementName.GetBundleName().c_str(),
        elementName.GetAbilityName().c_str());
    std::string item;
    boolret = ParseString(env, item, argv[ARR_INDEX_ONE]);
    ASSERT_AND_THROW_PARAM_ERROR(env, boolret, "param 'item' parse error");
    std::string stringRet;
    auto securityManagerProxy = SecurityManagerProxy::GetSecurityManagerProxy();
    int32_t ret = ERR_OK;
    if (item == EdmConstants::SecurityManager::PATCH) {
        ret = securityManagerProxy->GetSecurityPatchTag(elementName, stringRet);
    } else if (item == EdmConstants::SecurityManager::ENCRYPTION) {
        DeviceEncryptionStatus deviceEncryptionStatus;
        ret = securityManagerProxy->GetDeviceEncryptionStatus(elementName, deviceEncryptionStatus);
        if (SUCCEEDED(ret)) {
            ret = ConvertDeviceEncryptionToJson(env, deviceEncryptionStatus, stringRet);
        }
    } else if (item == EdmConstants::SecurityManager::ROOT) {
        ret = securityManagerProxy->GetRootCheckStatus(elementName, stringRet);
    } else {
        ret = EdmReturnErrCode::INTERFACE_UNSUPPORTED;
    }
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value status;
    NAPI_CALL(env, napi_create_string_utf8(env, stringRet.c_str(), stringRet.size(), &status));
    return status;
}

int32_t SecurityManagerAddon::ConvertDeviceEncryptionToJson(napi_env env,
    DeviceEncryptionStatus &deviceEncryptionStatus, std::string &stringRet)
{
    cJSON *json = nullptr;
    CJSON_CREATE_OBJECT_AND_CHECK(json, EdmReturnErrCode::SYSTEM_ABNORMALLY);
    cJSON_AddBoolToObject(json, "isEncrypted", deviceEncryptionStatus.isEncrypted);
    char *jsonStr = cJSON_PrintUnformatted(json);
    if (jsonStr == nullptr) {
        cJSON_Delete(json);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    stringRet = std::string(jsonStr);
    cJSON_Delete(json);
    cJSON_free(jsonStr);
    return ERR_OK;
}

napi_value SecurityManagerAddon::InstallUserCertificateSync(napi_env env, napi_value argv,
    AsyncCertCallbackInfo *asyncCallbackInfo)
{
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv, napi_number),
        "accountId type error");
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, asyncCallbackInfo->certblobCA.accountId,
        argv), "Parameter accountId error");
    auto securityManagerProxy = SecurityManagerProxy::GetSecurityManagerProxy();
    int32_t ret = securityManagerProxy->InstallUserCertificate(asyncCallbackInfo->elementName,
        asyncCallbackInfo->certblobCA, asyncCallbackInfo->stringRet,
        asyncCallbackInfo->innerCodeMsg);
    if (FAILED(ret)) {
        if (asyncCallbackInfo->innerCodeMsg.empty()) {
            napi_throw(env, CreateError(env, ret));
        } else {
            napi_throw(env, CreateError(env, ret, asyncCallbackInfo->innerCodeMsg));
        }
        return nullptr;
    }
    napi_value status;
    NAPI_CALL(env, napi_create_string_utf8(env, asyncCallbackInfo->stringRet.c_str(),
        asyncCallbackInfo->stringRet.size(), &status));
    return status;
}

napi_value SecurityManagerAddon::InstallUserCertificate(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_InstallUserCertificate called");
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    bool matchFlag = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, matchFlag, "parameter want error");
    matchFlag = MatchValueType(env, argv[ARR_INDEX_ONE], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, matchFlag, "parameter certblob error");
    auto asyncCallbackInfo = new (std::nothrow) AsyncCertCallbackInfo();
    if (asyncCallbackInfo == nullptr) {
        napi_throw(env, CreateError(env, EdmReturnErrCode::SYSTEM_ABNORMALLY));
        return nullptr;
    }
    std::unique_ptr<AsyncCertCallbackInfo> callbackPtr{asyncCallbackInfo};
    bool retAdmin = ParseElementName(env, asyncCallbackInfo->elementName, argv[ARR_INDEX_ZERO]);
    ASSERT_AND_THROW_PARAM_ERROR(env, retAdmin, "element name param error");
    bool retCertBlob = ParseCertBlob(env, argv[ARR_INDEX_ONE], asyncCallbackInfo);
    ASSERT_AND_THROW_PARAM_ERROR(env, retCertBlob, "element cert blob error");
    if (argc > ARGS_SIZE_TWO) {
        return InstallUserCertificateSync(env, argv[ARR_INDEX_TWO], asyncCallbackInfo);
    }
    napi_value asyncWorkReturn = HandleAsyncWork(env, asyncCallbackInfo, "InstallUserCertificate",
        NativeInstallUserCertificate, NativeStringCallbackComplete);
    callbackPtr.release();
    return asyncWorkReturn;
}

void SecurityManagerAddon::NativeInstallUserCertificate(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeInstallUserCertificate called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AsyncCertCallbackInfo *asyncCallbackInfo = static_cast<AsyncCertCallbackInfo *>(data);
    auto securityManagerProxy = SecurityManagerProxy::GetSecurityManagerProxy();
    asyncCallbackInfo->ret = securityManagerProxy->InstallUserCertificate(asyncCallbackInfo->elementName,
        asyncCallbackInfo->certblobCA, asyncCallbackInfo->stringRet, asyncCallbackInfo->innerCodeMsg);
}

napi_value SecurityManagerAddon::UninstallUserCertificate(napi_env env, napi_callback_info info)
{
    auto convertCertBlob2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
            std::string alias;
            if (!ParseString(env, alias, argv)) {
                EDMLOGE("element alias error");
                return false;
            }
            data.WriteString(alias);
            return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "UninstallUserCertificate";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.argsConvert = {nullptr, convertCertBlob2Data};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    return AddonMethodAdapter(env, info, addonMethodSign, NativeUninstallUserCertificate, NativeVoidCallbackComplete);
}

void SecurityManagerAddon::NativeUninstallUserCertificate(napi_env env, void *data)
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

napi_value SecurityManagerAddon::GetUserCertificates(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetUserCertificates called");
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "GetUserCertificates";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::INT32};
    addonMethodSign.methodAttribute = MethodAttribute::GET;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    std::vector<std::string> uriList;
    auto securityManagerProxy = SecurityManagerProxy::GetSecurityManagerProxy();
    int32_t ret = securityManagerProxy->GetUserCertificates(adapterAddonData.data, uriList);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value jsUriList = nullptr;
    NAPI_CALL(env, napi_create_array(env, &jsUriList));
    ConvertStringVectorToJS(env, uriList, jsUriList);
    return jsUriList;
}

bool SecurityManagerAddon::ParseCertBlob(napi_env env, napi_value object, AsyncCertCallbackInfo *asyncCertCallbackInfo)
{
    napi_valuetype type = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, object, &type), false);
    if (type != napi_object) {
        EDMLOGE("type of param certblob is not object");
        return false;
    }
    if (!JsObjectToU8Vector(env, object, "inData", asyncCertCallbackInfo->certblobCA.certArray)) {
        EDMLOGE("uint8Array to vector failed");
        return false;
    }
    return JsObjectToString(env, object, "alias", true, asyncCertCallbackInfo->certblobCA.alias);
}

napi_value SecurityManagerAddon::SetAppClipboardPolicy(napi_env env, napi_callback_info info)
{
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "SetAppClipboardPolicy";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::INT32, EdmAddonCommonType::INT32};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t retCode =
        SecurityManagerProxy::GetSecurityManagerProxy()->SetAppClipboardPolicy(adapterAddonData.data);
    if (FAILED(retCode)) {
        napi_throw(env, CreateError(env, retCode));
    }
    return nullptr;
}

napi_value SecurityManagerAddon::GetAppClipboardPolicy(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetAppClipboardPolicy called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "admin type error");

    OHOS::AppExecFwk::ElementName elementName;
    int32_t tokenId = 0;
    std::string policy;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "Parameter admin error");

    if (argc > ARGS_SIZE_ONE) {
        ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_number), "tokenId type error");
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, tokenId, argv[ARR_INDEX_ONE]),
        "Parameter tokenId error");
    }
    int32_t retCode =
        SecurityManagerProxy::GetSecurityManagerProxy()->GetAppClipboardPolicy(elementName, tokenId, policy);
    if (FAILED(retCode)) {
        napi_throw(env, CreateError(env, retCode));
        return nullptr;
    }
    napi_value clipboardPolicy;
    NAPI_CALL(env, napi_create_string_utf8(env, policy.c_str(), NAPI_AUTO_LENGTH, &clipboardPolicy));
    return clipboardPolicy;
}

void SecurityManagerAddon::CreateClipboardPolicyObject(napi_env env, napi_value value)
{
    napi_value nDefault;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(ClipboardPolicy::DEFAULT), &nDefault));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "DEFAULT", nDefault));
    napi_value nInApp;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(ClipboardPolicy::IN_APP), &nInApp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "IN_APP", nInApp));
    napi_value nLocalDevice;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ClipboardPolicy::LOCAL_DEVICE), &nLocalDevice));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "LOCAL_DEVICE", nLocalDevice));
    napi_value nCrossDevice;
    NAPI_CALL_RETURN_VOID(env,
        napi_create_int32(env, static_cast<int32_t>(ClipboardPolicy::CROSS_DEVICE), &nCrossDevice));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "CROSS_DEVICE", nCrossDevice));
}

napi_value SecurityManagerAddon::SetWatermarkImage(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetWatermarkImage called");
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_FOUR, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "admin type error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_string), "bundleName type error");
    napi_valuetype imageValueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[ARR_INDEX_TWO], &imageValueType));
    ASSERT_AND_THROW_PARAM_ERROR(env, imageValueType == napi_object || imageValueType == napi_string,
        "image type error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_THREE], napi_number),
        "accountId type error");

    OHOS::AppExecFwk::ElementName elementName;
    std::string bundleName;
    std::shared_ptr<Media::PixelMap> pixelMap;
    int32_t accountId = -1;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "Parameter admin error");
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, bundleName, argv[ARR_INDEX_ONE]),
        "Parameter bundleName error");
    if (imageValueType == napi_object) {
        pixelMap = Media::PixelMapNapi::GetPixelMap(env, argv[ARR_INDEX_TWO]);
    } else {
        std::string url;
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, url, argv[ARR_INDEX_TWO]),
            "Parameter pixelMap error");
        pixelMap = Decode(url);
    }
    ASSERT_AND_THROW_PARAM_ERROR(env, pixelMap != nullptr &&
        pixelMap->GetByteCount() <= MAX_WATERMARK_IMAGE_SIZE, "Parameter pixelMap error");
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, accountId, argv[ARR_INDEX_THREE]),
        "Parameter accountId error");
    int32_t retCode =
        SecurityManagerProxy::GetSecurityManagerProxy()
        ->SetWatermarkImage(elementName, bundleName, pixelMap, accountId);
    if (FAILED(retCode)) {
        napi_throw(env, CreateError(env, retCode));
    }
    return nullptr;
}

napi_value SecurityManagerAddon::CancelWatermarkImage(napi_env env, napi_callback_info info)
{
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "CancelWatermarkImage";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::STRING, EdmAddonCommonType::INT32};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    int32_t retCode =
        SecurityManagerProxy::GetSecurityManagerProxy()->CancelWatermarkImage(adapterAddonData.data);
    if (FAILED(retCode)) {
        napi_throw(env, CreateError(env, retCode));
    }
    return nullptr;
}

std::shared_ptr<OHOS::Media::PixelMap> SecurityManagerAddon::Decode(const std::string url)
{
    char canonicalPath[PATH_MAX + 1] = { 0 };
    if (url.size() > PATH_MAX || realpath(url.c_str(), canonicalPath) == nullptr) {
        EDMLOGE("realpath error");
        return nullptr;
    }
    int fd = open(canonicalPath, O_RDONLY);
    if (fd == -1) {
        EDMLOGE("Decode Open file fail!");
        return nullptr;
    }
    uint32_t ret = ERR_INVALID_VALUE;
    std::shared_ptr<int> fdPtr(&fd, [](int *fd) {
        close(*fd);
        *fd = -1;
    });
    Media::SourceOptions sourceOption;
    auto imageSourcePtr = Media::ImageSource::CreateImageSource(*fdPtr, sourceOption, ret);
    if (ret != ERR_OK || imageSourcePtr == nullptr) {
        EDMLOGE("CreateImageSource error");
        return nullptr;
    }
    Media::DecodeOptions option;
    auto pixelMapPtr = imageSourcePtr->CreatePixelMap(option, ret);
    if (ret != ERR_OK) {
        EDMLOGE("CreatePixelMap error");
        return nullptr;
    }
    return std::shared_ptr<Media::PixelMap>(std::move(pixelMapPtr));
}

static napi_module g_securityModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = SecurityManagerAddon::Init,
    .nm_modname = "enterprise.securityManager",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void SecurityManagerRegister()
{
    napi_module_register(&g_securityModule);
}
