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

#include "application_manager_addon.h"

#include "clear_up_application_data_param.h"
#include "edm_constants.h"
#include "edm_log.h"
#include "hisysevent_adapter.h"
#include "kiosk_feature.h"
#include "napi_edm_adapter.h"
#ifdef OS_ACCOUNT_EDM_ENABLE
#include "os_account_manager.h"
#endif

using namespace OHOS::EDM;

const std::u16string DESCRIPTOR = u"ohos.edm.IEnterpriseDeviceMgr";
napi_value ApplicationManagerAddon::Init(napi_env env, napi_value exports)
{
    napi_value nKioskFeature = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nKioskFeature));
    CreateKioskFeatureObject(env, nKioskFeature);

    napi_property_descriptor property[] = {
        DECLARE_NAPI_FUNCTION("addDisallowedRunningBundles", AddDisallowedRunningBundles),
        DECLARE_NAPI_FUNCTION("removeDisallowedRunningBundles", RemoveDisallowedRunningBundles),
        DECLARE_NAPI_FUNCTION("getDisallowedRunningBundles", GetDisallowedRunningBundles),
        DECLARE_NAPI_FUNCTION("addAutoStartApps", AddAutoStartApps),
        DECLARE_NAPI_FUNCTION("removeAutoStartApps", RemoveAutoStartApps),
        DECLARE_NAPI_FUNCTION("getAutoStartApps", GetAutoStartApps),
        DECLARE_NAPI_FUNCTION("addDisallowedRunningBundlesSync", AddDisallowedRunningBundlesSync),
        DECLARE_NAPI_FUNCTION("removeDisallowedRunningBundlesSync", RemoveDisallowedRunningBundlesSync),
        DECLARE_NAPI_FUNCTION("getDisallowedRunningBundlesSync", GetDisallowedRunningBundlesSync),
        DECLARE_NAPI_FUNCTION("setKioskFeatures", SetKioskFeatures),
        DECLARE_NAPI_FUNCTION("addKeepAliveApps", AddKeepAliveApps),
        DECLARE_NAPI_FUNCTION("removeKeepAliveApps", RemoveKeepAliveApps),
        DECLARE_NAPI_FUNCTION("getKeepAliveApps", GetKeepAliveApps),
        DECLARE_NAPI_FUNCTION("clearUpApplicationData", ClearUpApplicationData),
        DECLARE_NAPI_FUNCTION("setAllowedKioskApps", SetAllowedKioskApps),
        DECLARE_NAPI_FUNCTION("getAllowedKioskApps", GetAllowedKioskApps),
        DECLARE_NAPI_FUNCTION("isAppKioskAllowed", IsAppKioskAllowed),
        DECLARE_NAPI_PROPERTY("KioskFeature", nKioskFeature),
        DECLARE_NAPI_FUNCTION("isModifyKeepAliveAppsDisallowed", IsModifyKeepAliveAppsDisallowed),
        DECLARE_NAPI_FUNCTION("isModifyAutoStartAppsDisallowed", IsModifyAutoStartAppsDisallowed),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(property) / sizeof(property[0]), property));
    return exports;
}

napi_value ApplicationManagerAddon::IsModifyKeepAliveAppsDisallowed(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_IsModifyKeepAliveAppsDisallowed called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "isModifyKeepAliveAppsDisallowed");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_THREE, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "Parameter elementName error");
    
    int32_t userId = 0;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, userId, argv[ARR_INDEX_ONE]), "Parameter userId error");
    EDMLOGD(
        "EnableAdmin: elementName.bundlename %{public}s, "
        "elementName.abilityname:%{public}s",
        elementName.GetBundleName().c_str(), elementName.GetAbilityName().c_str());
    
    std::string keepAliveApp;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, keepAliveApp, argv[ARR_INDEX_TWO]) && keepAliveApp.size() > 0,
        "Parameter keepAliveApps error");

    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    bool isModifyKeepAliveAppsDisallowed = true;
    int32_t ret = applicationManagerProxy->IsModifyKeepAliveAppsDisallowed(elementName, keepAliveApp,
        userId, isModifyKeepAliveAppsDisallowed);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value napiIsModify = nullptr;
    napi_get_boolean(env, isModifyKeepAliveAppsDisallowed, &napiIsModify);
    return napiIsModify;
}

napi_value ApplicationManagerAddon::AddKeepAliveApps(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_AddKeepAliveApps called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "addKeepAliveApps");
    return AddOrRemoveKeepAliveApps(env, info, "AddKeepAliveApps");
}

napi_value ApplicationManagerAddon::RemoveKeepAliveApps(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_RemoveKeepAliveApps called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "removeKeepAliveApps");
    return AddOrRemoveKeepAliveApps(env, info, "RemoveKeepAliveApps");
}

napi_value ApplicationManagerAddon::AddOrRemoveKeepAliveApps(napi_env env, napi_callback_info info,
    std::string function)
{
    EDMLOGI("NAPI_AddOrRemoveKeepAliveApps called");
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_THREE, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "Parameter elementName error");
    std::vector<std::string> keepAliveApps;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseStringArray(env, keepAliveApps, argv[ARR_INDEX_ONE]),
        "Parameter keepAliveApps error");
    int32_t userId = 0;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, userId, argv[ARR_INDEX_TWO]), "Parameter userId error");
    bool disallowModify = true;
    if (argc >= ARGS_SIZE_FOUR) {
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseBool(env, disallowModify, argv[ARR_INDEX_THREE]),
            "Parameter disallowModify error");
        EDMLOGI("NAPI_AddOrRemoveKeepAliveApps called disallowModify: %{public}d", disallowModify);
    }
    EDMLOGD(
        "EnableAdmin: elementName.bundlename %{public}s, "
        "elementName.abilityname:%{public}s",
        elementName.GetBundleName().c_str(), elementName.GetAbilityName().c_str());
    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    int32_t ret = 0;
    std::string retMessage;
    if (function == "AddKeepAliveApps") {
        ret = applicationManagerProxy->AddKeepAliveApps(elementName, keepAliveApps, disallowModify, userId, retMessage);
    } else {
        ret = applicationManagerProxy->RemoveKeepAliveApps(elementName, keepAliveApps, userId);
    }
    if (FAILED(ret)) {
        if (ret == EdmReturnErrCode::ADD_KEEP_ALIVE_APP_FAILED) {
            napi_throw(env, CreateErrorWithInnerCode(env, ret, retMessage));
        } else {
            napi_throw(env, CreateError(env, ret));
        }
    }
    return nullptr;
}

napi_value ApplicationManagerAddon::GetKeepAliveApps(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetKeepAliveApps called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getKeepAliveApps");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "Parameter elementName error");
    int32_t userId = 0;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, userId, argv[ARR_INDEX_ONE]), "Parameter userId error");
    EDMLOGD(
        "EnableAdmin: elementName.bundlename %{public}s, "
        "elementName.abilityname:%{public}s",
        elementName.GetBundleName().c_str(), elementName.GetAbilityName().c_str());

    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    std::vector<std::string> keepAliveApps;
    int32_t ret = applicationManagerProxy->GetKeepAliveApps(elementName, keepAliveApps, userId);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value napiKeepAliveApps = nullptr;
    napi_create_array(env, &napiKeepAliveApps);
    ConvertStringVectorToJS(env, keepAliveApps, napiKeepAliveApps);
    return napiKeepAliveApps;
}

napi_value ApplicationManagerAddon::AddAutoStartApps(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "addAutoStartApps");
    return AddOrRemoveAutoStartApps(env, info, "AddAutoStartApps");
}

napi_value ApplicationManagerAddon::RemoveAutoStartApps(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "removeAutoStartApps");
    return AddOrRemoveAutoStartApps(env, info, "RemoveAutoStartApps");
}

napi_value ApplicationManagerAddon::AddOrRemoveAutoStartApps(napi_env env, napi_callback_info info,
    std::string function)
{
    EDMLOGI("NAPI_AddOrRemoveAutoStartApps called");
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "Parameter elementName error");
    std::vector<AppExecFwk::ElementName> autoStartApps;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementArray(env, autoStartApps, argv[ARR_INDEX_ONE]),
        "Parameter autoStartApps error");
    int32_t userId = 0;
    AccountSA::OsAccountManager::GetOsAccountLocalIdFromProcess(userId);
    if ((argc >= ARGS_SIZE_FOUR && function == "AddAutoStartApps") ||
        (argc >= ARGS_SIZE_THREE && function == "removeAutoStartApps")) {
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, userId, argv[ARR_INDEX_TWO]), "Parameter userId error");
    }
    MessageParcel parcelData;
    parcelData.WriteInterfaceToken(DESCRIPTOR);
    parcelData.WriteInt32(HAS_USERID);
    parcelData.WriteInt32(userId);
    parcelData.WriteParcelable(&elementName);
    parcelData.WriteString(WITHOUT_PERMISSION_TAG);
    std::vector<std::string> autoStartAppsString;
    for (size_t i = 0; i < autoStartApps.size(); i++) {
        std::string appWant = autoStartApps[i].GetBundleName() + "/" + autoStartApps[i].GetAbilityName();
        autoStartAppsString.push_back(appWant);
    }
    parcelData.WriteStringVector(autoStartAppsString);
    bool disallowModify = true;
    if (argc >= ARGS_SIZE_FOUR) {
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseBool(env, disallowModify, argv[ARR_INDEX_THREE]),
            "Parameter disallowModify error");
        EDMLOGI("NAPI_AddOrRemoveAutoStartApps called disallowModify: %{public}d", disallowModify);
    }
    parcelData.WriteBool(disallowModify);
    int32_t ret = ApplicationManagerProxy::GetApplicationManagerProxy()->AddOrRemoveAutoStartApps(
        parcelData, function == "AddAutoStartApps");
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

napi_value ApplicationManagerAddon::GetAutoStartApps(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetAutoStartApps called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getAutoStartApps");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "Parameter elementName error");
    int32_t userId = 0;
    AccountSA::OsAccountManager::GetOsAccountLocalIdFromProcess(userId);
    if (argc >= ARGS_SIZE_TWO) {
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, userId, argv[ARR_INDEX_ONE]), "Parameter userId error");
    }
    MessageParcel parcelData;
    SetBaseDataForGetPolicy(userId, parcelData);
    parcelData.WriteInt32(HAS_ADMIN);
    parcelData.WriteParcelable(&elementName);
    parcelData.WriteString(OHOS::EDM::EdmConstants::AutoStart::GET_MANAGE_AUTO_START_APPS_BUNDLE_INFO);
    std::vector<OHOS::AppExecFwk::ElementName> autoStartApps;
    int32_t ret = ApplicationManagerProxy::GetApplicationManagerProxy()->GetAutoStartApps(
        parcelData, autoStartApps);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value napiAutoStartApps = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiAutoStartApps));
    size_t idx = 0;
    for (const auto &element : autoStartApps) {
        napi_value objAutoStartApps = nullptr;
        NAPI_CALL(env, napi_create_object(env, &objAutoStartApps));
        napi_value napi_bundleName;
        napi_value napi_abilityName;
        NAPI_CALL(env, napi_create_string_utf8(env, element.GetBundleName().c_str(),
            element.GetBundleName().size(), &napi_bundleName));
        NAPI_CALL(env, napi_create_string_utf8(env, element.GetAbilityName().c_str(),
            element.GetAbilityName().size(), &napi_abilityName));
        NAPI_CALL(env, napi_set_named_property(env, objAutoStartApps, "bundleName", napi_bundleName));
        NAPI_CALL(env, napi_set_named_property(env, objAutoStartApps, "abilityName", napi_abilityName));
        napi_set_element(env, napiAutoStartApps, idx, objAutoStartApps);
        idx++;
    }
    return napiAutoStartApps;
}

void ApplicationManagerAddon::SetBaseDataForGetPolicy(int32_t userId, MessageParcel &data)
{
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(HAS_USERID);
    data.WriteInt32(userId);
    data.WriteString(WITHOUT_PERMISSION_TAG);
}

napi_value ApplicationManagerAddon::IsModifyAutoStartAppsDisallowed(napi_env env, napi_callback_info info)
{
    EDMLOGI("IsModifyAutoStartAppsDisallowed called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "isModifyAutoStartAppsDisallowed");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_THREE, "parameter count error");
    bool hasAdmin = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    ASSERT_AND_THROW_PARAM_ERROR(env, hasAdmin, "The first parameter must be want.");
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "Parameter elementName error");
    OHOS::AppExecFwk::ElementName autoStartApp;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, autoStartApp, argv[ARR_INDEX_ONE]),
        "Parameter autoStartApp error");
    int32_t userId = 0;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, userId, argv[ARR_INDEX_TWO]), "Parameter userId error");
    MessageParcel parcelData;
    parcelData.WriteInterfaceToken(DESCRIPTOR);
    parcelData.WriteInt32(HAS_USERID);
    parcelData.WriteInt32(userId);
    parcelData.WriteString(WITHOUT_PERMISSION_TAG);
    parcelData.WriteInt32(HAS_ADMIN);
    parcelData.WriteParcelable(&elementName);
    parcelData.WriteString(OHOS::EDM::EdmConstants::AutoStart::GET_MANAGE_AUTO_START_APP_DISALLOW_MODIFY);
    std::string appWant = autoStartApp.GetBundleName() + "/" + autoStartApp.GetAbilityName();
    parcelData.WriteString(appWant);

    bool isModifyAutoStartAppDisallowed = true;
    int32_t ret = ApplicationManagerProxy::GetApplicationManagerProxy()->IsModifyAutoStartAppsDisallowed(
        parcelData, isModifyAutoStartAppDisallowed);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value napiIsModify = nullptr;
    napi_get_boolean(env, isModifyAutoStartAppDisallowed, &napiIsModify);
    return napiIsModify;
}

napi_value ApplicationManagerAddon::GetDisallowedRunningBundles(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetDisallowedRunningBundles called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getDisallowedRunningBundles");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    bool hasCallback = false;
    bool hasUserId = false;
    auto asyncCallbackInfo = new (std::nothrow) AsyncDisallowedRunningBundlesCallbackInfo();
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncDisallowedRunningBundlesCallbackInfo> callbackPtr {asyncCallbackInfo};
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "Parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, CheckAdminWithUserIdParamType(env, argc, argv, hasCallback, hasUserId),
        "Parameter type error");
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, asyncCallbackInfo->elementName, argv[ARR_INDEX_ZERO]),
        "Parameter want error");

    EDMLOGD("GetDisallowedRunningBundles::asyncCallbackInfo->elementName.bundlename %{public}s, "
        "asyncCallbackInfo->abilityname:%{public}s",
        asyncCallbackInfo->elementName.GetBundleName().c_str(),
        asyncCallbackInfo->elementName.GetAbilityName().c_str());
    if (hasUserId) {
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, asyncCallbackInfo->userId, argv[ARR_INDEX_ONE]),
            "Parameter user id error");
    } else {
#ifdef OS_ACCOUNT_EDM_ENABLE
        AccountSA::OsAccountManager::GetOsAccountLocalIdFromProcess(asyncCallbackInfo->userId);
#endif
    }
    if (hasCallback) {
        ASSERT_AND_THROW_PARAM_ERROR(env,
            ParseCallback(env, asyncCallbackInfo->callback,
                argc <= ARR_INDEX_THREE ? argv[argc - 1] : argv[ARR_INDEX_TWO]),
            "Parameter callback error");
    }
    napi_value asyncWorkReturn = HandleAsyncWork(env, asyncCallbackInfo, "GetDisallowedRunningBundles",
        NativeGetDisallowedRunningBundles, NativeArrayStringCallbackComplete);
    callbackPtr.release();
    return asyncWorkReturn;
}

void ApplicationManagerAddon::NativeGetDisallowedRunningBundles(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeGetDisallowedRunningBundles called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AsyncDisallowedRunningBundlesCallbackInfo *asyncCallbackInfo =
        static_cast<AsyncDisallowedRunningBundlesCallbackInfo *>(data);
    auto proxy_ = ApplicationManagerProxy::GetApplicationManagerProxy();
    if (proxy_ == nullptr) {
        EDMLOGE("can not get EnterpriseDeviceMgrProxy");
        return;
    }

    asyncCallbackInfo->ret = proxy_->GetDisallowedRunningBundles(asyncCallbackInfo->elementName,
        asyncCallbackInfo->userId, asyncCallbackInfo->arrayStringRet);
}

napi_value ApplicationManagerAddon::AddDisallowedRunningBundles(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "addDisallowedRunningBundles");
    return AddOrRemovellowedRunningBundles(env, info, "AddDisallowedRunningBundles", NativeAddDisallowedRunningBundles);
}

napi_value ApplicationManagerAddon::RemoveDisallowedRunningBundles(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "removeDisallowedRunningBundles");
    return AddOrRemovellowedRunningBundles(env, info, "RemoveDisallowedRunningBundles",
        NativeRemoveDisallowedRunningBundles);
}

void ApplicationManagerAddon::NativeAddDisallowedRunningBundles(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeAddDisallowedRunningBundles called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AsyncDisallowedRunningBundlesCallbackInfo *asyncCallbackInfo =
        static_cast<AsyncDisallowedRunningBundlesCallbackInfo *>(data);
    auto applicationManagerProxy_ = ApplicationManagerProxy::GetApplicationManagerProxy();
    if (applicationManagerProxy_ == nullptr) {
        EDMLOGE("can not get applicationManagerProxy");
        return;
    }
    asyncCallbackInfo->ret = applicationManagerProxy_->AddDisallowedRunningBundles(asyncCallbackInfo->elementName,
        asyncCallbackInfo->appIds, asyncCallbackInfo->userId);
}

bool ApplicationManagerAddon::CheckAddDisallowedRunningBundlesParamType(napi_env env, size_t argc,
    napi_value* argv, bool &hasCallback, bool &hasUserId)
{
    if (!MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object) || !MatchValueType(env, argv[ARR_INDEX_ONE],
        napi_object)) {
        EDMLOGE("CheckAddDisallowedRunningBundlesParamType admin or array type check failed");
        return false;
    }
    EDMLOGI("CheckAddDisallowedRunningBundlesParamType argc = %{public}zu", argc);
    if (argc == ARGS_SIZE_TWO) {
        hasCallback = false;
        hasUserId = false;
        EDMLOGI("hasCallback = false; hasUserId = false;");
        return true;
    }

    if (argc == ARGS_SIZE_THREE) {
        if (MatchValueType(env, argv[ARR_INDEX_TWO], napi_function)) {
            hasCallback = true;
            hasUserId = false;
            EDMLOGI("hasCallback = true; hasUserId = false;");
            return true;
        } else {
            hasCallback = false;
            hasUserId = true;
            EDMLOGI("hasCallback = false;  hasUserId = true;");
            return MatchValueType(env, argv[ARR_INDEX_TWO], napi_number);
        }
    }
    hasCallback = true;
    hasUserId = true;
    EDMLOGI("hasCallback = true; hasUserId = true;");
    return MatchValueType(env, argv[ARR_INDEX_TWO], napi_number) &&
        MatchValueType(env, argv[ARR_INDEX_THREE], napi_function);
}

napi_value ApplicationManagerAddon::AddOrRemovellowedRunningBundles(napi_env env, napi_callback_info info,
    const std::string &workName, napi_async_execute_callback execute)
{
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    bool hasCallback = false;
    bool hasUserId = false;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    auto asyncCallbackInfo = new (std::nothrow) AsyncDisallowedRunningBundlesCallbackInfo();
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncDisallowedRunningBundlesCallbackInfo> callbackPtr {asyncCallbackInfo};
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "Parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env,
        CheckAddDisallowedRunningBundlesParamType(env, argc, argv, hasCallback, hasUserId), "Parameter type error");
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, asyncCallbackInfo->elementName, argv[ARR_INDEX_ZERO]),
        "Parameter want error");
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseStringArray(env, asyncCallbackInfo->appIds, argv[ARR_INDEX_ONE]),
        "Parameter bundles error");
    ASSERT_AND_THROW_PARAM_ERROR(env, asyncCallbackInfo->appIds.size() <= EdmConstants::APPID_MAX_SIZE,
        "Parameter bundles too large");
    EDMLOGD("EnableAdmin::asyncCallbackInfo->elementName.bundlename %{public}s, "
        "asyncCallbackInfo->abilityname:%{public}s",
        asyncCallbackInfo->elementName.GetBundleName().c_str(),
        asyncCallbackInfo->elementName.GetAbilityName().c_str());
    if (hasUserId) {
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, asyncCallbackInfo->userId, argv[ARR_INDEX_TWO]),
            "Parameter user id error");
    } else {
#ifdef OS_ACCOUNT_EDM_ENABLE
        AccountSA::OsAccountManager::GetOsAccountLocalIdFromProcess(asyncCallbackInfo->userId);
#endif
    }
    if (hasCallback) {
        ASSERT_AND_THROW_PARAM_ERROR(env,
            ParseCallback(env, asyncCallbackInfo->callback,
                argc <= ARR_INDEX_FOUR ? argv[argc - 1] : argv[ARR_INDEX_THREE]),
            "Parameter callback error");
    }
    napi_value asyncWorkReturn = HandleAsyncWork(env, asyncCallbackInfo, workName,
        execute, NativeVoidCallbackComplete);
    callbackPtr.release();
    return asyncWorkReturn;
}

void ApplicationManagerAddon::NativeRemoveDisallowedRunningBundles(napi_env env, void *data)
{
    EDMLOGI("NativeRemoveDisallowedRunningBundles called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AsyncDisallowedRunningBundlesCallbackInfo *asyncCallbackInfo =
        static_cast<AsyncDisallowedRunningBundlesCallbackInfo *>(data);
    auto applicationManagerProxy_ = ApplicationManagerProxy::GetApplicationManagerProxy();
    if (applicationManagerProxy_ == nullptr) {
        EDMLOGE("can not get ApplicationManagerProxy");
        return;
    }
    asyncCallbackInfo->ret = applicationManagerProxy_->RemoveDisallowedRunningBundles(asyncCallbackInfo->elementName,
        asyncCallbackInfo->appIds, asyncCallbackInfo->userId);
}

napi_value ApplicationManagerAddon::AddDisallowedRunningBundlesSync(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "addDisallowedRunningBundlesSync");
    return AddOrRemoveDisallowedRunningBundlesSync(env, info, true);
}

napi_value ApplicationManagerAddon::RemoveDisallowedRunningBundlesSync(napi_env env, napi_callback_info info)
{
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "removeDisallowedRunningBundlesSync");
    return AddOrRemoveDisallowedRunningBundlesSync(env, info, false);
}

napi_value ApplicationManagerAddon::AddOrRemoveDisallowedRunningBundlesSync(napi_env env, napi_callback_info info,
    bool isAdd)
{
    EDMLOGI("NAPI_AddOrRemoveDisallowedRunningBundlesSync called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));

    bool hasAccountId = (argc == ARGS_SIZE_THREE);
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "parameter admin error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_object), "parameter appIds error");
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "parameter admin parse error");
    std::vector<std::string> appIds;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseStringArray(env, appIds, argv[ARR_INDEX_ONE]),
        "parameter appIds parse error");
    EDMLOGD("AddOrRemoveDisallowedRunningBundlesSync: "
        "elementName.bundleName %{public}s, elementName.abilityName:%{public}s",
        elementName.GetBundleName().c_str(), elementName.GetAbilityName().c_str());
    int32_t accountId = 0;
    if (hasAccountId) {
        ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_TWO], napi_number),
            "parameter accountId error");
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, accountId, argv[ARR_INDEX_TWO]),
            "parameter accountId parse error");
    } else {
#ifdef OS_ACCOUNT_EDM_ENABLE
        AccountSA::OsAccountManager::GetOsAccountLocalIdFromProcess(accountId);
#endif
    }

    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    if (applicationManagerProxy == nullptr) {
        EDMLOGE("can not get applicationManagerProxy");
        return nullptr;
    }
    int32_t ret = ERR_OK;
    if (isAdd) {
        ret = applicationManagerProxy->AddDisallowedRunningBundles(elementName, appIds, accountId, true);
    } else {
        ret = applicationManagerProxy->RemoveDisallowedRunningBundles(elementName, appIds, accountId, true);
    }
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

napi_value ApplicationManagerAddon::GetDisallowedRunningBundlesSync(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetDisallowedRunningBundlesSync called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "getDisallowedRunningBundlesSync");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    bool hasAccountId = (argc == ARGS_SIZE_TWO);
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "parameter admin error");
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "parameter admin parse error");
    EDMLOGD("GetDisallowedRunningBundlesSync: elementName.bundleName %{public}s, elementName.abilityName:%{public}s",
        elementName.GetBundleName().c_str(), elementName.GetAbilityName().c_str());
    int32_t accountId = 0;
    if (hasAccountId) {
        ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_number),
            "parameter accountId error");
        ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, accountId, argv[ARR_INDEX_ONE]),
            "parameter accountId parse error");
    } else {
#ifdef OS_ACCOUNT_EDM_ENABLE
        AccountSA::OsAccountManager::GetOsAccountLocalIdFromProcess(accountId);
#endif
    }

    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    if (applicationManagerProxy == nullptr) {
        EDMLOGE("can not get applicationManagerProxy");
        return nullptr;
    }
    std::vector<std::string> appIds;
    int32_t ret = applicationManagerProxy->GetDisallowedRunningBundles(elementName, accountId, appIds, true);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_array(env, &result));
    ConvertStringVectorToJS(env, appIds, result);
    return result;
}

void ApplicationManagerAddon::CreateKioskFeatureObject(napi_env env, napi_value value)
{
    napi_value nAllowNotificationCenter;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env,
        static_cast<uint32_t>(KioskFeature::ALLOW_NOTIFICATION_CENTER), &nAllowNotificationCenter));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "ALLOW_NOTIFICATION_CENTER",
        nAllowNotificationCenter));

    napi_value nAllowContorlCenter;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env,
        static_cast<uint32_t>(KioskFeature::ALLOW_CONTROL_CENTER), &nAllowContorlCenter));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "ALLOW_CONTROL_CENTER", nAllowContorlCenter));
}

napi_value ApplicationManagerAddon::SetKioskFeatures(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetKioskFeatures called");
    HiSysEventAdapter::ReportEdmEvent(ReportType::EDM_FUNC_EVENT, "setKioskFeatures");
    auto convertKioskFeature2Data = [](napi_env env, napi_value argv, MessageParcel &data,
        const AddonMethodSign &methodSign) {
        std::vector<int32_t> kioskFeatures;
        bool parseRet = ParseIntArray(env, kioskFeatures, argv);
        if (!parseRet) {
            EDMLOGE("NAPI_SetKioskFeatures ParseIntArray fail");
            return false;
        }
        data.WriteInt32Vector(kioskFeatures);
        return true;
    };
    AddonMethodSign addonMethodSign;
    addonMethodSign.name = "SetKioskFeatures";
    addonMethodSign.argsType = {EdmAddonCommonType::ELEMENT, EdmAddonCommonType::CUSTOM};
    addonMethodSign.methodAttribute = MethodAttribute::HANDLE;
    addonMethodSign.argsConvert = {nullptr, convertKioskFeature2Data};
    AdapterAddonData adapterAddonData{};
    napi_value result = JsObjectToData(env, info, addonMethodSign, &adapterAddonData);
    if (result == nullptr) {
        return nullptr;
    }
    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    int32_t ret = applicationManagerProxy->SetKioskFeatures(adapterAddonData.data);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        EDMLOGE("SetKioskFeatures failed!");
    }
    return nullptr;
}

napi_value ApplicationManagerAddon::ClearUpApplicationData(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_ClearUpApplicationData called");
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = { nullptr };
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));

    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_FOUR, "Parameter count error.");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "Parameter admin error.");
    ASSERT_AND_THROW_PARAM_ERROR(
        env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_string), "Parameter bundleName error.");
    ASSERT_AND_THROW_PARAM_ERROR(
        env, MatchValueType(env, argv[ARR_INDEX_TWO], napi_number), "Parameter appIndex error.");
    ASSERT_AND_THROW_PARAM_ERROR(
        env, MatchValueType(env, argv[ARR_INDEX_THREE], napi_number), "Parameter accountId error.");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(
        env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]), "Parameter elementName parse error.");

    ClearUpApplicationDataParam param;
    ASSERT_AND_THROW_PARAM_ERROR(
        env, ParseString(env, param.bundleName, argv[ARR_INDEX_ONE]), "Parameter bundleName parse error.");
    ASSERT_AND_THROW_PARAM_ERROR(
        env, ParseInt(env, param.appIndex, argv[ARR_INDEX_TWO]), "Parameter appIndex parse error.");
    ASSERT_AND_THROW_PARAM_ERROR(
        env, ParseInt(env, param.userId, argv[ARR_INDEX_THREE]), "Parameter accountId parse error.");
    EDMLOGD("EnableAdmin: elementName.bundlename %{public}s, "
            "elementName.abilityname:%{public}s",
        elementName.GetBundleName().c_str(), elementName.GetAbilityName().c_str());

    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    int32_t ret = applicationManagerProxy->ClearUpApplicationData(elementName, param);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }

    return nullptr;
}

napi_value ApplicationManagerAddon::SetAllowedKioskApps(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_SetAllowedKioskApps called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(
        env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "The first parameter must be want.");
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(
        env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]), "Parameter elementName error");
    std::vector<std::string> appIdentifiers;
    ASSERT_AND_THROW_PARAM_ERROR(
        env, ParseStringArray(env, appIdentifiers, argv[ARR_INDEX_ONE]), "Parameter appIdentifiers error");
    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    int32_t ret = applicationManagerProxy->SetAllowedKioskApps(elementName, appIdentifiers);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    return nullptr;
}

napi_value ApplicationManagerAddon::GetAllowedKioskApps(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetAllowedKioskApps called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(
        env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "The first parameter must be want.");
    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(
        env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]), "Parameter elementName error");
    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    std::vector<std::string> appIdentifiers;
    int32_t ret = applicationManagerProxy->GetAllowedKioskApps(elementName, appIdentifiers);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value napiAppIdentifiers = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiAppIdentifiers));
    ConvertStringVectorToJS(env, appIdentifiers, napiAppIdentifiers);
    return napiAppIdentifiers;
}

napi_value ApplicationManagerAddon::IsAppKioskAllowed(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_IsAppKioskAllowed called");
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(
        env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_string), "Parameter appIdentifier error.");
    std::string appIdentifier;
    ASSERT_AND_THROW_PARAM_ERROR(
        env, ParseString(env, appIdentifier, argv[ARR_INDEX_ZERO]), "Parameter appIdentifier parse error");
    auto applicationManagerProxy = ApplicationManagerProxy::GetApplicationManagerProxy();
    bool isAllowed = false;
    int32_t ret = applicationManagerProxy->IsAppKioskAllowed(appIdentifier, isAllowed);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, isAllowed, &result));
    return result;
}

static napi_module g_applicationManagerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = ApplicationManagerAddon::Init,
    .nm_modname = "enterprise.applicationManager",
    .nm_priv = ((void *)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void ApplicationManagerRegister()
{
    napi_module_register(&g_applicationManagerModule);
}