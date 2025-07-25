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
#include "iplugin_manager.h"
#include "func_code_utils.h"
#include "edm_ipc_interface_code.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(std::make_shared<SetApnPlugin>());
const std::set<std::string> ALL_APN_INFO_FIELD = {
    "profile_name", "mcc", "mnc",
    "apn", "apn_types", "auth_user",
    "proxy_ip_address", "mms_ip_address", "auth_type",
};

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

    return ApnUtils::ApnInsert(apnInfo, apnUtilsPassword);
}

ErrCode SetApnPlugin::HandleUpdate(MessageParcel &data)
{
    EDMLOGI("SetApnPlugin::HandleUpdate start");
    std::string apnId;
    ApnUtilsPassword apnUtilsPassword;
    if (data.ReadString(apnId)) {
        if (apnId.empty()) {
            return EdmReturnErrCode::PARAM_ERROR;
        }
        std::map<std::string, std::string> apnInfo = ParserApnMap(data, apnUtilsPassword);
        if (apnInfo.size() == 0 && apnUtilsPassword.password == nullptr) {
            return EdmReturnErrCode::PARAM_ERROR;
        }
        return ApnUtils::ApnUpdate(apnInfo, apnId, apnUtilsPassword);
    } else {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
}

ErrCode SetApnPlugin::HandleSetPrefer(MessageParcel &data)
{
    EDMLOGI("SetApnPlugin::HandleSetPrefer start");
    std::string apnId;
    if (data.ReadString(apnId)) {
        if (apnId.empty()) {
            return EdmReturnErrCode::PARAM_ERROR;
        }
        return ApnUtils::ApnSetPrefer(apnId);
    } else {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
}

ErrCode SetApnPlugin::HandleRemove(MessageParcel &data)
{
    EDMLOGI("SetApnPlugin::HandleRemove start");
    std::string apnId;
    if (data.ReadString(apnId)) {
        if (apnId.empty()) {
            return EdmReturnErrCode::PARAM_ERROR;
        }
        return ApnUtils::ApnDelete(apnId);
    } else {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
}

std::map<std::string, std::string> SetApnPlugin::ParserApnMap(MessageParcel &data, ApnUtilsPassword &apnPassword)
{
    EDMLOGI("SetApnPlugin::ParserApnMap start");
    std::map<std::string, std::string> result;
    int32_t mapSize = -1;
    if (!data.ReadInt32(mapSize) || static_cast<uint32_t>(mapSize) > EdmConstants::SetApn::MAX_MAP_SIZE) {
        EDMLOGE("ParserApnMap size error");
        return {};
    }
    std::vector<std::string> keys(mapSize);
    for (int32_t idx = 0; idx < mapSize; ++idx) {
        keys[idx] = data.ReadString();
    }
    std::vector<std::string> values(mapSize);
    for (int32_t idx = 0; idx < mapSize; ++idx) {
        values[idx] = data.ReadString();
    }
    for (int32_t idx = 0; idx < mapSize; ++idx) {
        result[keys[idx]] = values[idx];
    }
    int32_t pwdLen = data.ReadInt32();
    if (pwdLen >= 0) {
        apnPassword.passwordSize = static_cast<int32_t>(pwdLen);
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
    int32_t size = 0;
    for (const auto &[key, value] : apnInfo) {
        if (ALL_APN_INFO_FIELD.find(key) != ALL_APN_INFO_FIELD.end()) {
            size++;
        }
    }
    reply.WriteInt32(ERR_OK);
    reply.WriteInt32(size);
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
    reply.WriteInt32(static_cast<int32_t>(result.size()));
    for (const auto &ele : result) {
        reply.WriteString(ele);
    }
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS