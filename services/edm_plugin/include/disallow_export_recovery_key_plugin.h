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

#ifndef SERVICES_EDM_PLUGIN_INCLUDE_DISALLOW_EXPORT_RECOVERY_KEY_PLUGIN_H
#define SERVICES_EDM_PLUGIN_INCLUDE_DISALLOW_EXPORT_RECOVERY_KEY_PLUGIN_H

#include "plugin_singleton.h"

namespace OHOS {
namespace EDM {
class DisallowExportRecoveryKeyPlugin : public PluginSingleton<DisallowExportRecoveryKeyPlugin, bool> {
public:
    void InitPlugin(std::shared_ptr<IPluginTemplate<DisallowExportRecoveryKeyPlugin, bool>> ptr) override;
    ErrCode OnSetPolicy(bool &data, bool &currentData, bool &mergePolicy, int32_t userId);
    ErrCode OnAdminRemove(const std::string &adminName, bool &data, bool &mergeData, int32_t userId);

private:
    ErrCode SetExportRecoveryKeyPolicy(bool policy, int32_t userId);
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_PLUGIN_INCLUDE_DISALLOW_EXPORT_RECOVERY_KEY_PLUGIN_H
