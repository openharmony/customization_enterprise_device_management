/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "set_apn_plugin.h"
 
#include <memory>

#include "edm_constants.h"
#include "edm_ipc_interface_code.h"
#include "edm_json_builder.h"
#include "func_code_utils.h"
#include "iextra_policy_notification.h"
#include "iplugin_manager.h"
#include "override_interface_name.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(std::make_shared<SetApnPlugin>());
const std::set<std::string> ALL_APN_INFO_FIELD = {
    "profile_name", "mcc", "mnc",
    "apn", "apn_types", "auth_user",
    "proxy_ip_address", "mms_ip_address", "auth_type",
};

const std::map<std::string, std::string> FIELD_TO_KEY = {
    { "profile_name", "apnName" },
    { "apn_types", "type" },
    { "auth_user", "user" },
    { "proxy_ip_address", "proxy" },
    { "mms_ip_address", "mmsproxy" },
    { "auth_type", "authType" },
    { "mcc", "mcc" },
    { "mnc", "mnc" },
    { "apn", "apn" }
};
const char* const KEY_APN_INFO = "apnInfo";
const char* const KEY_APN_NAME = "apnName";
const char* const KEY_APN_ID = "apnId";
const char* const KEY_PROFILE_NAME = "profile_name";

SetApnPlugin::SetApnPlugin()
{
    policyCode_ = EdmInterfaceCode::SET_APN_INFO;
    policyName_ = PolicyName::POLICY_SET_APN_INFO;
    permissionConfig_.typePermissions.emplace(IPlugin::PermissionType::SUPER_DEVICE_ADMIN,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_APN);
    permissionConfig_.apiType = IPlugin::ApiType::PUBLIC;
    needSave_ = false;
}
 
ErrCode SetApnPlugin::OnHandlePolicy(std::uint32_t funcCode, MessageParcel &data, MessageParcel &reply,
    HandlePolicyData &policyData, int32_t userId)
{
    EDMLOGI("SetApnPlugin::OnHandlePolicy start");
    uint32_t typeCode = FUNC_TO_OPERATE(funcCode);
    FuncOperateType type = FuncCodeUtils::ConvertOperateType(typeCode);
    if (type == FuncOperateType::SET) {
        const std::string flag = data.ReadString();
        if (flag == EdmConstants::SetApn::ADD_FLAG) {
            return HandleAdd(data);
        } else if (flag == EdmConstants::SetApn::UPDATE_FLAG) {
            return HandleUpdate(data);
        } else if (flag == EdmConstants::SetApn::SET_PREFER_FLAG) {
            return HandleSetPrefer(data);
        } else {
            return EdmReturnErrCode::SYSTEM_ABNORMALLY;
        }
    } else if (type == FuncOperateType::REMOVE) {
        return HandleRemove(data);
    } else {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
}

ErrCode SetApnPlugin::HandleAdd(MessageParcel &data)
{
    EDMLOGI("SetApnPlugin::HandleAdd start");
    ApnUtilsPassword apnUtilsPassword;
    std::map<std::string, std::string> apnInfo = ParserApnMap(data, apnUtilsPassword);
    if (apnInfo.size() == 0) {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }

    if (apnUtilsPassword.passwordSize > ApnPassword::MAX_PASSWORD_SIZE) {
        EDMLOGE("SetApnPlugin::HandleAdd password size over");
        return EdmReturnErrCode::PARAM_ERROR;
    }
    auto ret = ApnUtils::ApnInsert(apnInfo, apnUtilsPassword);
    if (ret == ERR_OK) {
        NotifyPolicyChanged(OverrideInterfaceName::NetworkManager::ADD_APN, ApnUtils::ApnQuery(apnInfo), apnInfo);
    }
    return ret;
}

ErrCode SetApnPlugin::HandleUpdate(MessageParcel &data)
{
    EDMLOGI("SetApnPlugin::HandleUpdate start");
    std::string apnId;
    ApnUtilsPassword apnUtilsPassword;
    if (!data.ReadString(apnId)) {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    if (apnId.empty()) {
        return EdmReturnErrCode::PARAM_ERROR;
    }
    std::map<std::string, std::string> apnInfo = ParserApnMap(data, apnUtilsPassword);
    if (apnInfo.size() == 0 && apnUtilsPassword.password == nullptr) {
        return EdmReturnErrCode::PARAM_ERROR;
    }
    if (apnUtilsPassword.passwordSize > ApnPassword::MAX_PASSWORD_SIZE) {
        EDMLOGE("SetApnPlugin::HandleUpdate password size over");
        return EdmReturnErrCode::PARAM_ERROR;
    }
    auto ret = ApnUtils::ApnUpdate(apnInfo, apnId, apnUtilsPassword);
    if (ret == ERR_OK) {
        auto info = ConvertToJsKey(apnInfo);
        std::string params = EdmJsonBuilder()
            .Add(KEY_APN_INFO, info)
            .Add(KEY_APN_ID, apnId)
            .Build();
        IExtraPolicyNotification::GetInstance()->NotifyPolicyChanged(OverrideInterfaceName::NetworkManager::UPDATE_APN,
            params);
    }
    return ret;
}

ErrCode SetApnPlugin::HandleSetPrefer(MessageParcel &data)
{
    EDMLOGI("SetApnPlugin::HandleSetPrefer start");
    std::string apnId;
    if (!data.ReadString(apnId)) {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    if (apnId.empty()) {
        return EdmReturnErrCode::PARAM_ERROR;
    }
    auto ret = ApnUtils::ApnSetPrefer(apnId);
    if (ret == ERR_OK) {
        auto apnInfo = ApnUtils::ApnQuery(apnId);
        NotifyPolicyChanged(OverrideInterfaceName::NetworkManager::SET_PREFERRED_APN, {apnId}, apnInfo);
    }
    return ret;
}

ErrCode SetApnPlugin::HandleRemove(MessageParcel &data)
{
    EDMLOGI("SetApnPlugin::HandleRemove start");
    std::string apnId;
    if (!data.ReadString(apnId)) {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    if (apnId.empty()) {
        return EdmReturnErrCode::PARAM_ERROR;
    }
    auto apnInfo = ApnUtils::ApnQuery(apnId);
    auto ret = ApnUtils::ApnDelete(apnId);
    if (ret == ERR_OK) {
        NotifyPolicyChanged(OverrideInterfaceName::NetworkManager::DELETE_APN, {apnId}, apnInfo);
    }
    return ret;
}

std::map<std::string, std::string> SetApnPlugin::ParserApnMap(MessageParcel &data, ApnUtilsPassword &apnPassword)
{
    EDMLOGI("SetApnPlugin::ParserApnMap start");
    std::map<std::string, std::string> result;
    uint32_t mapSize = 0;
    if (!data.ReadUint32(mapSize) || mapSize > EdmConstants::SetApn::MAX_MAP_SIZE) {
        EDMLOGE("ParserApnMap size error");
        return {};
    }
    std::vector<std::string> keys(mapSize);
    for (uint32_t idx = 0; idx < mapSize; ++idx) {
        keys[idx] = data.ReadString();
    }
    std::vector<std::string> values(mapSize);
    for (uint32_t idx = 0; idx < mapSize; ++idx) {
        values[idx] = data.ReadString();
    }
    for (uint32_t idx = 0; idx < mapSize; ++idx) {
        result[keys[idx]] = values[idx];
    }
    int32_t pwdLen = data.ReadInt32();
    if (pwdLen >= 0) {
        apnPassword.passwordSize = static_cast<uint32_t>(pwdLen);
        apnPassword.password = const_cast<char *>(data.ReadCString());
    }
    return result;
}

void SetApnPlugin::GenerateApnMap(std::map<std::string, std::string> &apnInfo, MessageParcel &reply)
{
    EDMLOGI("SetApnPlugin::GenerateApnMap start");
    if (apnInfo.size() > EdmConstants::SetApn::MAX_MAP_SIZE) {
        EDMLOGE("GenerateApnMap size error");
        reply.WriteInt32(EdmReturnErrCode::PARAM_ERROR);
        return;
    }
    uint32_t size = 0;
    for (const auto &[key, value] : apnInfo) {
        if (ALL_APN_INFO_FIELD.find(key) != ALL_APN_INFO_FIELD.end()) {
            size++;
        }
    }
    reply.WriteInt32(ERR_OK);
    reply.WriteUint32(size);
    for (const auto &[key, value] : apnInfo) {
        if (ALL_APN_INFO_FIELD.find(key) != ALL_APN_INFO_FIELD.end()) {
            reply.WriteString(key);
        }
    }
    for (const auto &[key, value] : apnInfo) {
        if (ALL_APN_INFO_FIELD.find(key) != ALL_APN_INFO_FIELD.end()) {
            reply.WriteString(value);
        }
    }
}

ErrCode SetApnPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply, int32_t userId)
{
    EDMLOGI("SetApnPlugin::OnGetPolicy start");
    const std::string flag = data.ReadString();
    if (flag == EdmConstants::SetApn::QUERY_ID_FLAG) {
        return QueryId(data, reply);
    } else if (flag == EdmConstants::SetApn::QUERY_INFO_FLAG) {
        return QueryInfo(data, reply);
    } else {
        reply.WriteInt32(EdmReturnErrCode::SYSTEM_ABNORMALLY);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    return ERR_OK;
}

ErrCode SetApnPlugin::QueryInfo(MessageParcel &data, MessageParcel &reply)
{
    EDMLOGI("SetApnPlugin::QueryInfo start");
    std::string apnId;
    if (data.ReadString(apnId)) {
        if (apnId.empty()) {
            return EdmReturnErrCode::PARAM_ERROR;
        }
        std::map<std::string, std::string> apnInfo = ApnUtils::ApnQuery(apnId);
        GenerateApnMap(apnInfo, reply);
        return ERR_OK;
    } else {
        reply.WriteInt32(EdmReturnErrCode::SYSTEM_ABNORMALLY);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
}

ErrCode SetApnPlugin::QueryId(MessageParcel &data, MessageParcel &reply)
{
    EDMLOGI("SetApnPlugin::QueryId start");
    ApnUtilsPassword apnUtilsPassword;
    std::map<std::string, std::string> apnInfo = ParserApnMap(data, apnUtilsPassword);
    std::vector<std::string> result = ApnUtils::ApnQuery(apnInfo);
    reply.WriteInt32(ERR_OK);
    reply.WriteUint32(result.size());
    for (const auto &ele : result) {
        reply.WriteString(ele);
    }
    return ERR_OK;
}

void SetApnPlugin::NotifyPolicyChanged(const std::string &functionName, const std::vector<std::string> &apnIds,
    std::map<std::string, std::string> &apnInfo)
{
    EdmJsonBuilder jsonBuilder = EdmJsonBuilder();
    if (!apnIds.empty()) {
        jsonBuilder.Add(KEY_APN_ID, apnIds[apnIds.size() - 1]);
    }
    if (apnInfo.find(KEY_PROFILE_NAME) != apnInfo.end()) {
        jsonBuilder.Add(KEY_APN_NAME, apnInfo[KEY_PROFILE_NAME]);
    }
    IExtraPolicyNotification::GetInstance()->NotifyPolicyChanged(functionName, jsonBuilder.Build());
}

std::map<std::string, std::string> SetApnPlugin::ConvertToJsKey(std::map<std::string, std::string> &apniInfo)
{
    std::map<std::string, std::string> results;
    for (const auto &[key, value] : FIELD_TO_KEY) {
        auto it = apniInfo.find(key);
        if (it != apniInfo.end()) {
            results[value] = it->second;
        }
    }
    return results;
}
} // namespace EDM
} // namespace OHOS