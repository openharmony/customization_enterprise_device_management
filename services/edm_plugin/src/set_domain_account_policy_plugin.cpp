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

#include "set_domain_account_policy_plugin.h"

#ifdef OS_ACCOUNT_EDM_ENABLE
#include "domain_account_client.h"
#include "domain_account_common.h"
#endif
#include "domain_account_policy.h"
#include "edm_ipc_interface_code.h"
#include "edm_log.h"
#include "func_code_utils.h"
#include "iplugin_manager.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(std::make_shared<SetDomainAccountPolicyPlugin>());

SetDomainAccountPolicyPlugin::SetDomainAccountPolicyPlugin()
{
    policyCode_ = EdmInterfaceCode::DOMAIN_ACCOUNT_POLICY;
    policyName_ = "set_domain_account_policy";
    permissionConfig_.typePermissions.emplace(IPlugin::PermissionType::SUPER_DEVICE_ADMIN,
        "ohos.permission.ENTERPRISE_SET_ACCOUNT_POLICY");
    permissionConfig_.apiType = IPlugin::ApiType::PUBLIC;
    needSave_ = false;
}

ErrCode SetDomainAccountPolicyPlugin::OnHandlePolicy(std::uint32_t funcCode, MessageParcel &data, MessageParcel &reply,
    HandlePolicyData &policyData, int32_t userId)
{
    uint32_t typeCode = FUNC_TO_OPERATE(funcCode);
    FuncOperateType type = FuncCodeUtils::ConvertOperateType(typeCode);
    if (type == FuncOperateType::SET) {
        return SetPowerPolicy(data);
    }
    return EdmReturnErrCode::SYSTEM_ABNORMALLY;
}

ErrCode SetDomainAccountPolicyPlugin::SetPowerPolicy(MessageParcel &data)
{
    OHOS::AccountSA::DomainAccountInfo domainAccountInfo;
    if (!domainAccountInfo.ReadFromParcel(data)) {
        EDMLOGE("SetDomainAccountPolicyPlugin domainAccountInfo ReadFromParcel error");
        return EdmReturnErrCode::PARAM_ERROR;
    }
    DomainAccountPolicy domainAccountPolicy;
    if (!DomainAccountPolicy::Unmarshalling(data, domainAccountPolicy)) {
        EDMLOGE("SetDomainAccountPolicyPlugin domainAccountpolicy Unmarshalling error");
        return EdmReturnErrCode::PARAM_ERROR;
    }
    if (!domainAccountPolicy.CheckParameterValidity()) {
        EDMLOGE("SetDomainAccountPolicyPlugin domainAccountpolicy param invalid");
        return EdmReturnErrCode::PARAM_ERROR;
    }
    std::string policy;
    if (!domainAccountPolicy.ConvertDomainAccountPolicyToJsStr(policy)) {
        EDMLOGE("SetDomainAccountPolicyPlugin ConvertDomainAccountPolicyToJsStr error");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    int32_t ret = OHOS::AccountSA::DomainAccountClient::GetInstance().SetAccountPolicy(domainAccountInfo, policy);
    if (FAILED(ret)) {
        EDMLOGE("SetDomainAccountPolicyPlugin SetAccountPolicy error");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    return ERR_OK;
}

ErrCode SetDomainAccountPolicyPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    OHOS::AccountSA::DomainAccountInfo domainAccountInfo;
    if (!domainAccountInfo.ReadFromParcel(data)) {
        EDMLOGE("SetDomainAccountPolicyPlugin::OnGetPolicy domainAccountInfo ReadFromParcel error");
        return EdmReturnErrCode::PARAM_ERROR;
    }
    std::string policy;
    int32_t ret = OHOS::AccountSA::DomainAccountClient::GetInstance().GetAccountPolicy(domainAccountInfo, policy);
    if (FAILED(ret)) {
        EDMLOGE("SetDomainAccountPolicyPlugin::OnGetPolicy GetAccountPolicy error");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    DomainAccountPolicy domainAccountPolicy;
    if (!DomainAccountPolicy::JsStrToDomainAccountPolicy(policy, domainAccountPolicy)) {
        EDMLOGE("SetDomainAccountPolicyPlugin::OnGetPolicy JsStrToDomainAccountPolicy error");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    if (!domainAccountPolicy.Marshalling(reply)) {
        EDMLOGE("SetDomainAccountPolicyPlugin::OnGetPolicy Marshalling error");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS