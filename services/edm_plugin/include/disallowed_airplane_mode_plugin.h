/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef SERVICES_EDM_PLUGIN_INCLUDE_DISALLOWED_AIRPLANE_MODE_H
#define SERVICES_EDM_PLUGIN_INCLUDE_DISALLOWED_AIRPLANE_MODE_H

#include "basic_bool_plugin.h"
#include "plugin_singleton.h"

namespace OHOS {
namespace EDM {
class DisallowedAirplaneModePlugin : public PluginSingleton<DisallowedAirplaneModePlugin, bool>,
    public BasicBoolPlugin {
public:
    void InitPlugin(std::shared_ptr<IPluginTemplate<DisallowedAirplaneModePlugin, bool>> ptr) override;

private:
    ErrCode SetOtherModulePolicy(bool data, int32_t userId) override;

    ErrCode RemoveOtherModulePolicy(int32_t userId) override
    {
        return ERR_OK;
    }
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_PLUGIN_INCLUDE_DISALLOWED_AIRPLANE_MODE_H
