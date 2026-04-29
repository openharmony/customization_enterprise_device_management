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

#include "domain_filter_rule_plugin.h"

#include "edm_ipc_interface_code.h"
#include "edm_json_builder.h"
#include "edm_log.h"
#include "func_code_utils.h"
#include "domain_filter_rule_serializer.h"
#include "iextra_policy_notification.h"
#include "iptables_manager.h"
#include "iplugin_manager.h"
#include "iptables_factory.h"
#include "override_interface_name.h"

using namespace OHOS::EDM::IPTABLES;

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(DomainFilterRulePlugin::GetPlugin());

void DomainFilterRulePlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<DomainFilterRulePlugin, IPTABLES::DomainFilterRuleParcel>> ptr)
{
    EDMLOGI("DomainFilterRulePlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::DOMAIN_FILTER_RULE, PolicyName::POLICY_DOMAIN_FILTER_RULE,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_NETWORK, IPlugin::PermissionType::SUPER_DEVICE_ADMIN, false);
    ptr->SetSerializer(DomainFilterRuleSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DomainFilterRulePlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnHandlePolicyListener(&DomainFilterRulePlugin::OnRemovePolicy, FuncOperateType::REMOVE);
}

ErrCode DomainFilterRulePlugin::OnSetPolicy(IPTABLES::DomainFilterRuleParcel &ruleParcel)
{
    auto rule = ruleParcel.GetRule();
    IPTABLES::Family family = std::get<DOMAIN_FAMILY_IND>(rule);
    std::shared_ptr<IptablesManager> iptablesManager =
        IPTABLES::IptablesFactory::GetInstance()->CreateIptablesManagerForFamily(family);
    if (!iptablesManager->HasInit()) {
        iptablesManager->Init();
    }
    auto ret = iptablesManager->AddDomainFilterRule(ruleParcel);
    if (ret == ERR_OK) {
        IExtraPolicyNotification::GetInstance()->NotifyPolicyChanged(
            OverrideInterfaceName::NetworkManager::ADD_DOMAIN_FILTER_RULE, GetParams(rule));
    }
    return ret;
}

ErrCode DomainFilterRulePlugin::OnRemovePolicy(IPTABLES::DomainFilterRuleParcel &ruleParcel)
{
    auto rule = ruleParcel.GetRule();
    IPTABLES::Family family = std::get<DOMAIN_FAMILY_IND>(rule);
    std::shared_ptr<IptablesManager> iptablesManager =
        IPTABLES::IptablesFactory::GetInstance()->CreateIptablesManagerForFamily(family);
    if (!iptablesManager->HasInit()) {
        iptablesManager->Init();
    }
    auto ret = iptablesManager->RemoveDomainFilterRules(ruleParcel);
    if (ret == ERR_OK) {
        IExtraPolicyNotification::GetInstance()->NotifyPolicyChanged(
            OverrideInterfaceName::NetworkManager::REMOVE_DOMAIN_FILTER_RULE, GetParams(rule));
    }
    return ret;
}

std::string DomainFilterRulePlugin::GetParams(IPTABLES::DomainFilterRule rule)
{
    std::string domainFilterRuleJson = EdmJsonBuilder()
        .Add("domainName", std::get<DOMAIN_DOMAINNAME_IND>(rule))
        .Add("appUid", std::get<DOMAIN_APPUID_IND>(rule))
        .Add("action", static_cast<int32_t>(std::get<DOMAIN_ACTION_IND>(rule)))
        .Add("direction", static_cast<int32_t>(std::get<DOMAIN_DIRECTION_IND>(rule)))
        .Add("family", static_cast<int32_t>(std::get<DOMAIN_FAMILY_IND>(rule)))
        .Add("logType", static_cast<int32_t>(std::get<DOMAIN_LOGTYPE_IND>(rule)))
        .Build();
    return EdmJsonBuilder().AddRawJson("domainFilterRule", domainFilterRuleJson).Build();
}

ErrCode DomainFilterRulePlugin::OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    EDMLOGI("DomainFilterRulePlugin OnGetPolicy.");
    reply.WriteInt32(ERR_OK);
    std::shared_ptr<IptablesManager> ipv4tablesManager =
        IPTABLES::IptablesFactory::GetInstance()->CreateIptablesManagerForFamily(IPTABLES::Family::IPV4);
    std::shared_ptr<IptablesManager> ipv6tablesManager =
        IPTABLES::IptablesFactory::GetInstance()->CreateIptablesManagerForFamily(IPTABLES::Family::IPV6);
    if (!ipv4tablesManager->HasInit() && !ipv6tablesManager->HasInit()) {
        ipv4tablesManager->Init();
        ipv6tablesManager->Init();
        reply.WriteInt32(0);
    } else {
        std::vector<DomainFilterRuleParcel> list;
        if (ipv4tablesManager->HasInit()) {
            ipv4tablesManager->GetDomainFilterRules(list);
        }
        if (ipv6tablesManager->HasInit()) {
            ipv6tablesManager->GetDomainFilterRules(list);
        }
        reply.WriteInt32(list.size());
        for (auto const &item : list) {
            item.Marshalling(reply);
        }
    }
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS
