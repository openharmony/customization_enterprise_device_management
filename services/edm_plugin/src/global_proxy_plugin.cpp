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

#include "global_proxy_plugin.h"

#include "edm_ipc_interface_code.h"
#include "edm_json_builder.h"
#include "edm_log.h"
#include "func_code_utils.h"
#include "http_proxy_serializer.h"
#include "iextra_policy_notification.h"
#include "iplugin_manager.h"
#include "net_conn_client.h"
#include "override_interface_name.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(GlobalProxyPlugin::GetPlugin());

void GlobalProxyPlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<GlobalProxyPlugin, NetManagerStandard::HttpProxy>> ptr)
{
    EDMLOGI("GlobalProxyPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::GLOBAL_PROXY, PolicyName::POLICY_GLOBAL_PROXY,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_NETWORK, IPlugin::PermissionType::SUPER_DEVICE_ADMIN, false);
    ptr->SetSerializer(HttpProxySerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&GlobalProxyPlugin::OnSetPolicy, FuncOperateType::SET);
}

ErrCode GlobalProxyPlugin::OnSetPolicy(NetManagerStandard::HttpProxy &httpProxy)
{
    int32_t ret = NetManagerStandard::NetConnClient::GetInstance().SetGlobalHttpProxy(httpProxy);
    EDMLOGI("GlobalProxyPlugin::SetGlobalHttpProxy set result = %{public}d", ret);
    if (ret != ERR_OK) {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    std::string interfaceName = OverrideInterfaceName::NetworkManager::SET_GLOBAL_PROXY_SYNC;
    std::string host = httpProxy.GetHost();
    auto port = httpProxy.GetPort();
    auto exclusionList = httpProxy.GetExclusionList();
    std::vector<std::string> exclusionVec(exclusionList.begin(), exclusionList.end());
    auto userId = httpProxy.GetUserId();
    std::string httpProxyJson = EdmJsonBuilder()
        .Add("host", host)
        .Add("port", port)
        .Add("exclusionList", exclusionVec)
        .Build();
    EdmJsonBuilder jsonBuilder;
    jsonBuilder.AddRawJson("httpProxy", httpProxyJson);
    if (userId != -1) {
        interfaceName = OverrideInterfaceName::NetworkManager::SET_GLOBAL_PROXY_FOR_ACCOUNT;
        jsonBuilder.Add("accountId", userId);
    }
    IExtraPolicyNotification::GetInstance()->NotifyPolicyChanged(interfaceName, jsonBuilder.Build());
    return ERR_OK;
}

ErrCode GlobalProxyPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    NetManagerStandard::HttpProxy httpProxy;
    int32_t accountId = data.ReadInt32();
    httpProxy.SetUserId(accountId);
    int32_t ret = NetManagerStandard::NetConnClient::GetInstance().GetGlobalHttpProxy(httpProxy);
    EDMLOGI("GlobalProxyPlugin::GetGlobalHttpProxy result = %{public}d", ret);
    if (ret == ERR_OK) {
        reply.WriteInt32(ERR_OK);
        if (!httpProxy.Marshalling(reply)) {
            EDMLOGE("GlobalProxyPlugin::httpProxy Marshalling error");
            return EdmReturnErrCode::SYSTEM_ABNORMALLY;
        }
        return ERR_OK;
    } else {
        reply.WriteInt32(EdmReturnErrCode::SYSTEM_ABNORMALLY);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
}
} // namespace EDM
} // namespace OHOS
