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

#ifndef SERVICES_EDM_PLUGIN_INCLUDE_DISALLOWED_SIM_PLUGIN_H
#define SERVICES_EDM_PLUGIN_INCLUDE_DISALLOWED_SIM_PLUGIN_H

#include "bit_serializer.h"
#include "plugin_singleton.h"
   
namespace OHOS {
namespace EDM {
class DisallowedSimPlugin : public PluginSingleton<DisallowedSimPlugin, uint32_t> {
public:
    void InitPlugin(std::shared_ptr<IPluginTemplate<DisallowedSimPlugin, uint32_t>> ptr) override;
    ErrCode OnSetPolicy(uint32_t &data, uint32_t &currentData, uint32_t &mergeData, int32_t userId);
    ErrCode OnRemovePolicy(uint32_t &data, uint32_t &currentData, uint32_t &mergeData, int32_t userId);
    ErrCode OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply, int32_t userId) override;
    ErrCode OnAdminRemove(const std::string &adminName, uint32_t &data, uint32_t &mergeData, int32_t userId);
};
} // namespace EDM
} // namespace OHOS
   
#endif // SERVICES_EDM_PLUGIN_INCLUDE_DISALLOWED_SIM_PLUGIN_H