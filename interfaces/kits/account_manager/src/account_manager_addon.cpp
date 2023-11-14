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
#include "account_manager_addon.h"

#include "account_info.h"
#include "domain_account_common.h"
#include "edm_log.h"
#include "ohos_account_kits.h"
#include "os_account_info.h"

using namespace OHOS::EDM;

napi_value AccountManagerAddon::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor property[] = {
        DECLARE_NAPI_FUNCTION("disallowAddLocalAccount", DisallowAddLocalAccount),
        DECLARE_NAPI_FUNCTION("disallowAddOsAccountByUser", DisallowAddOsAccountByUser),
        DECLARE_NAPI_FUNCTION("isAddOsAccountByUserDisallowed", IsAddOsAccountByUserDisallowed),
        DECLARE_NAPI_FUNCTION("addOsAccount", AddOsAccount),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(property) / sizeof(property[0]), property));
    return exports;
}

napi_value AccountManagerAddon::DisallowAddLocalAccount(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_DisallowAddLocalAccount called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    bool matchFlag = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object) &&
        MatchValueType(env, argv[ARR_INDEX_ONE], napi_boolean);
    if (argc > ARGS_SIZE_TWO) {
        matchFlag = matchFlag && MatchValueType(env, argv[ARR_INDEX_TWO], napi_function);
    }
    ASSERT_AND_THROW_PARAM_ERROR(env, matchFlag, "parameter type error");
    auto asyncCallbackInfo = new (std::nothrow) AsyncDisallowAddLocalAccountCallbackInfo();
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncDisallowAddLocalAccountCallbackInfo> callbackPtr {asyncCallbackInfo};
    bool ret = ParseElementName(env, asyncCallbackInfo->elementName, argv[ARR_INDEX_ZERO]);
    ASSERT_AND_THROW_PARAM_ERROR(env, ret, "element name param error");
    EDMLOGD("DisallowAddLocalAccount: asyncCallbackInfo->elementName.bundlename %{public}s, "
        "asyncCallbackInfo->abilityname:%{public}s",
        asyncCallbackInfo->elementName.GetBundleName().c_str(),
        asyncCallbackInfo->elementName.GetAbilityName().c_str());
    ret = ParseBool(env, asyncCallbackInfo->isDisallow, argv[ARR_INDEX_ONE]);
    ASSERT_AND_THROW_PARAM_ERROR(env, ret, "isDisallow param error");
    if (argc > ARGS_SIZE_TWO) {
        EDMLOGD("NAPI_DisallowAddLocalAccount argc == ARGS_SIZE_THREE");
        napi_create_reference(env, argv[ARR_INDEX_TWO], NAPI_RETURN_ONE, &asyncCallbackInfo->callback);
    }

    napi_value asyncWorkReturn = HandleAsyncWork(env, asyncCallbackInfo, "DisallowAddLocalAccount",
        NativeDisallowAddLocalAccount, NativeVoidCallbackComplete);
    callbackPtr.release();
    return asyncWorkReturn;
}

napi_value AccountManagerAddon::DisallowAddOsAccountByUser(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_DisallowAddOsAccountByUser called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_THREE, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "parameter admin error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_number), "parameter userid error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_TWO], napi_boolean),
        "parameter disallow error");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "parameter admin parse error");
    int32_t userId;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, userId, argv[ARR_INDEX_ONE]), "parameter userid parse error");
    bool disallow;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseBool(env, disallow, argv[ARR_INDEX_TWO]), "parameter disallow parse error");

    auto accountManagerProxy = AccountManagerProxy::GetAccountManagerProxy();
    if (accountManagerProxy == nullptr) {
        EDMLOGE("can not get AccountManagerProxy");
        return nullptr;
    }
    int32_t ret = accountManagerProxy->DisallowAddOsAccountByUser(elementName, userId, disallow);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        EDMLOGE("NAPI_DisallowAddOsAccountByUser failed!");
    }
    return nullptr;
}

napi_value AccountManagerAddon::IsAddOsAccountByUserDisallowed(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_IsAddOsAccountByUserDisallowed called");
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_TWO, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "parameter admin error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_number), "parameter userid error");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "parameter admin parse error");
    int32_t userId;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, userId, argv[ARR_INDEX_ONE]), "parameter userid parse error");

    auto accountManagerProxy = AccountManagerProxy::GetAccountManagerProxy();
    if (accountManagerProxy == nullptr) {
        EDMLOGE("can not get AccountManagerProxy");
        return nullptr;
    }
    bool isDisallowed;
    int32_t ret = accountManagerProxy->IsAddOsAccountByUserDisallowed(elementName, userId, isDisallowed);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
    }
    napi_value result = nullptr;
    napi_get_boolean(env, isDisallowed, &result);
    return result;
}

napi_value AccountManagerAddon::AddOsAccount(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_AddOsAccount called");
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_THREE, "parameter count error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object), "parameter admin error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_ONE], napi_string), "parameter name error");
    ASSERT_AND_THROW_PARAM_ERROR(env, MatchValueType(env, argv[ARR_INDEX_TWO], napi_number), "parameter type error");

    OHOS::AppExecFwk::ElementName elementName;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseElementName(env, elementName, argv[ARR_INDEX_ZERO]),
        "parameter admin parse error");
    std::string name;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseString(env, name, argv[ARR_INDEX_ONE]), "parameter name parse error");
    int32_t type;
    ASSERT_AND_THROW_PARAM_ERROR(env, ParseInt(env, type, argv[ARR_INDEX_TWO]), "parameter type parse error");
    ASSERT_AND_THROW_PARAM_ERROR(env, !name.empty(), "parameter name is empty");
    ASSERT_AND_THROW_PARAM_ERROR(env, CheckOsAccountType(type), "parameter type unknown");

    auto accountManagerProxy = AccountManagerProxy::GetAccountManagerProxy();
    if (accountManagerProxy == nullptr) {
        EDMLOGE("can not get AccountManagerProxy");
        return nullptr;
    }
    OHOS::AccountSA::OsAccountInfo accountInfo;
    std::string distributedInfoName;
    std::string distributedInfoId;
    int32_t ret = accountManagerProxy->AddOsAccount(elementName, name, type, accountInfo,
        distributedInfoName, distributedInfoId);
    if (FAILED(ret)) {
        napi_throw(env, CreateError(env, ret));
        return nullptr;
    }
    return ConvertOsAccountInfoToJs(env, accountInfo, distributedInfoName, distributedInfoId);
}

void AccountManagerAddon::NativeDisallowAddLocalAccount(napi_env env, void *data)
{
    EDMLOGI("NativeDisallowAddLocalAccount called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AsyncDisallowAddLocalAccountCallbackInfo *asyncCallbackInfo =
        static_cast<AsyncDisallowAddLocalAccountCallbackInfo *>(data);
    auto accountManagerProxy = AccountManagerProxy::GetAccountManagerProxy();
    if (accountManagerProxy == nullptr) {
        EDMLOGE("can not get AccountManagerProxy");
        return;
    }
    asyncCallbackInfo->ret = accountManagerProxy->DisallowAddLocalAccount(asyncCallbackInfo->elementName,
        asyncCallbackInfo->isDisallow);
}

bool AccountManagerAddon::CheckOsAccountType(int32_t type)
{
    if (type >= static_cast<int32_t>(OHOS::AccountSA::OsAccountType::ADMIN)
        && type < static_cast<int32_t>(OHOS::AccountSA::OsAccountType::END)) {
        return true;
    }
    return false;
}

napi_value AccountManagerAddon::ConvertOsAccountInfoToJs(napi_env env, OHOS::AccountSA::OsAccountInfo &info,
    std::string distributedInfoName, std::string distributedInfoId)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));

    // localId
    napi_value idToJs = nullptr;
    NAPI_CALL(env, napi_create_int32(env, info.GetLocalId(), &idToJs));
    NAPI_CALL(env, napi_set_named_property(env, result, "localId", idToJs));

    // localName
    napi_value nameToJs = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, info.GetLocalName().c_str(), NAPI_AUTO_LENGTH, &nameToJs));
    NAPI_CALL(env, napi_set_named_property(env, result, "localName", nameToJs));

    // type
    napi_value typeToJsObj = nullptr;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int>(info.GetType()), &typeToJsObj));
    NAPI_CALL(env, napi_set_named_property(env, result, "type", typeToJsObj));

    // constraints
    napi_value constraintsToJs = nullptr;
    NAPI_CALL(env, napi_create_array(env, &constraintsToJs));
    MakeArrayToJs(env, info.GetConstraints(), constraintsToJs);
    NAPI_CALL(env, napi_set_named_property(env, result, "constraints", constraintsToJs));

    // isVerified
    napi_value isVerifiedToJs = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, info.GetIsVerified(), &isVerifiedToJs));
    NAPI_CALL(env, napi_set_named_property(env, result, "isVerified", isVerifiedToJs));

    // photo
    napi_value photoToJs = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, info.GetPhoto().c_str(), NAPI_AUTO_LENGTH, &photoToJs));
    NAPI_CALL(env, napi_set_named_property(env, result, "photo", photoToJs));

    // createTime
    napi_value createTimeToJs = nullptr;
    NAPI_CALL(env, napi_create_int64(env, info.GetCreateTime(), &createTimeToJs));
    NAPI_CALL(env, napi_set_named_property(env, result, "createTime", createTimeToJs));

    // lastLoginTime
    napi_value lastLoginTimeToJs = nullptr;
    NAPI_CALL(env, napi_create_int64(env, info.GetLastLoginTime(), &lastLoginTimeToJs));
    NAPI_CALL(env, napi_set_named_property(env, result, "lastLoginTime", lastLoginTimeToJs));

    // serialNumber
    napi_value serialNumberToJs = nullptr;
    NAPI_CALL(env, napi_create_int64(env, info.GetSerialNumber(), &serialNumberToJs));
    NAPI_CALL(env, napi_set_named_property(env, result, "serialNumber", serialNumberToJs));

    // isActived
    napi_value isActivedToJs = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, info.GetIsActived(), &isActivedToJs));
    NAPI_CALL(env, napi_set_named_property(env, result, "isActived", isActivedToJs));

    // isCreateCompleted
    napi_value isCreateCompletedToJs = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, info.GetIsCreateCompleted(), &isCreateCompletedToJs));
    NAPI_CALL(env, napi_set_named_property(env, result, "isCreateCompleted", isCreateCompletedToJs));

    // distributedInfo: distributedAccount.DistributedInfo
    napi_value dbInfoToJs = nullptr;
    CreateJsDistributedInfo(env, distributedInfoName, distributedInfoId, dbInfoToJs);
    NAPI_CALL(env, napi_set_named_property(env, result, "distributedInfo", dbInfoToJs));

    // domainInfo: domainInfo.DomainAccountInfo
    OHOS::AccountSA::DomainAccountInfo domainInfo;
    info.GetDomainInfo(domainInfo);
    CreateJsDomainInfo(env, domainInfo, dbInfoToJs);
    NAPI_CALL(env, napi_set_named_property(env, result, "domainInfo", dbInfoToJs));

    return result;
}

napi_value AccountManagerAddon::MakeArrayToJs(napi_env env, const std::vector<std::string> &constraints,
    napi_value jsArray)
{
    uint32_t index = 0;
    for (const auto &item : constraints) {
        napi_value constraint = nullptr;
        NAPI_CALL(env, napi_create_string_utf8(env, item.c_str(), NAPI_AUTO_LENGTH, &constraint));
        NAPI_CALL(env, napi_set_element(env, jsArray, index, constraint));
        index++;
    }
    return jsArray;
}

napi_value AccountManagerAddon::CreateJsDistributedInfo(napi_env env, const std::string distributedInfoName,
    const std::string distributedInfoId, napi_value &result)
{
    napi_create_object(env, &result);
    napi_value value = nullptr;
    // name
    NAPI_CALL(env, napi_create_string_utf8(env, distributedInfoName.c_str(), NAPI_AUTO_LENGTH, &value));
    NAPI_CALL(env, napi_set_named_property(env, result, "name", value));

    // id
    NAPI_CALL(env, napi_create_string_utf8(env, distributedInfoId.c_str(), NAPI_AUTO_LENGTH, &value));
    NAPI_CALL(env, napi_set_named_property(env, result, "id", value));

    // event
    NAPI_CALL(env, napi_create_string_utf8(env, "", 0, &value));
    NAPI_CALL(env, napi_set_named_property(env, result, "event", value));

    // scalableData
    napi_value scalable = nullptr;
    NAPI_CALL(env, napi_create_object(env, &scalable));
    NAPI_CALL(env, napi_set_named_property(env, result, "scalableData", scalable));

    return result;
}

napi_value AccountManagerAddon::CreateJsDomainInfo(napi_env env, const OHOS::AccountSA::DomainAccountInfo &info,
    napi_value &result)
{
    napi_create_object(env, &result);
    napi_value value = nullptr;
    // domain
    NAPI_CALL(env, napi_create_string_utf8(env, info.domain_.c_str(), info.domain_.size(), &value));
    NAPI_CALL(env, napi_set_named_property(env, result, "domain", value));

    // domain accountName
    NAPI_CALL(env, napi_create_string_utf8(env, info.accountName_.c_str(), info.accountName_.size(), &value));
    NAPI_CALL(env, napi_set_named_property(env, result, "accountName", value));

    // domain accountId
    NAPI_CALL(env, napi_create_string_utf8(env, info.accountId_.c_str(), info.accountId_.size(), &value));
    NAPI_CALL(env, napi_set_named_property(env, result, "accountId", value));

    // domain isAuthenticated
    if ((info.status_ == OHOS::AccountSA::DomainAccountStatus::LOGOUT) ||
        (info.status_ >= OHOS::AccountSA::DomainAccountStatus::LOG_END)) {
        NAPI_CALL(env, napi_get_boolean(env, false, &value));
    } else {
        NAPI_CALL(env, napi_get_boolean(env, true, &value));
    }
    NAPI_CALL(env, napi_set_named_property(env, result, "isAuthenticated", value));
    return result;
}

static napi_module g_accountManagerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = AccountManagerAddon::Init,
    .nm_modname = "enterprise.accountManager",
    .nm_priv = ((void *)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void AccountManagerRegister()
{
    napi_module_register(&g_accountManagerModule);
}