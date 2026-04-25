/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "enterprise_policy_changed_connection.h"
#include "enterprise_admin_proxy.h"
#include "edm_log.h"

namespace OHOS {
namespace EDM {
// LCOV_EXCL_START
EnterprisePolicyChangedConnection::EnterprisePolicyChangedConnection(const AAFwk::Want &want,
    const PolicyChangedEvent &policyChangedEvent, int32_t userId) : IEnterpriseConnection(want,
    static_cast<uint32_t>(IEnterpriseAdmin::COMMAND_ON_POLICIES_CHANGED), userId),
    policyChangedEvent_(policyChangedEvent)
{}

void EnterprisePolicyChangedConnection::OnAbilityConnectDone(const AppExecFwk::ElementName& element,
    const sptr<IRemoteObject>& remoteObject, int32_t resultCode)
{
    if (resultCode != ERR_OK || remoteObject == nullptr) {
        EDMLOGE("EnterprisePolicyChangedConnection connect failed: %{public}d", resultCode);
        return;
    }

    proxy_ = (new (std::nothrow) EnterpriseAdminProxy(remoteObject));
    if (proxy_ == nullptr) {
        EDMLOGE("EnterprisePolicyChangedConnection create proxy failed");
        return;
    }

    proxy_->OnAdminPolicyChanged(policyChangedEvent_);
}

void EnterprisePolicyChangedConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName& element,
    int32_t resultCode)
{
    EDMLOGI("EnterprisePolicyChangedConnection::OnAbilityDisconnectDone result: %{public}d", resultCode);
}
// LCOV_EXCL_STOP
} // namespace EDM
} // namespace OHOS
