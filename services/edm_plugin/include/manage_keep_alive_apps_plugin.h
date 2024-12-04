/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SERVICES_EDM_PLUGIN_INCLUDE_MANAGE_KEEP_ALIVE_APPS_PLUGIN_H
#define SERVICES_EDM_PLUGIN_INCLUDE_MANAGE_KEEP_ALIVE_APPS_PLUGIN_H

#include "ability_manager_interface.h"
#include "bundle_mgr_interface.h"
#include "message_parcel.h"
#include "iplugin.h"
#include "iremote_stub.h"

namespace OHOS {
namespace EDM {
class ManageKeepAliveAppsPlugin : public IPlugin {
public:
    ManageKeepAliveAppsPlugin();
    ErrCode OnHandlePolicy(std::uint32_t funcCode, MessageParcel &data, MessageParcel &reply,
        HandlePolicyData &policyData, int32_t userId) override;
    void OnHandlePolicyDone(std::uint32_t funcCode, const std::string &adminName, bool isGlobalChanged,
        int32_t userId) override{};
    ErrCode OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply, int32_t userId) override;
    ErrCode OnAdminRemove(const std::string &adminName, const std::string &policyData, int32_t userId) override
    {
        return ERR_OK;
    };
    void OnAdminRemoveDone(const std::string &adminName, const std::string &currentJsonData, int32_t userId) override;
    ErrCode AddKeepAliveApps(std::vector<std::string> &keepAliveApps, int32_t userId);
    ErrCode RemoveKeepAliveApps(std::vector<std::string> &keepAliveApps, int32_t userId);

private:
    void ParseErrCode(ErrCode &ret, std::string &errMessage, MessageParcel &reply);
    sptr<AAFwk::IAbilityManager> GetAbilityManager();
    sptr<AppExecFwk::IAppControlMgr> GetAppControlProxy();
};
} // namespace EDM
} // namespace OHOS

#endif /* SERVICES_EDM_PLUGIN_INCLUDE_MANAGE_KEEP_ALIVE_APPS_PLUGIN_H */