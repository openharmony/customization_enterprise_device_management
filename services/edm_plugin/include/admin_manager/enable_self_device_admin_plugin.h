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

#ifndef SERVICES_EDM_PLUGIN_INCLUDE_ENABLE_SELF_DEVICE_ADMIN_PLUGIN_H
#define SERVICES_EDM_PLUGIN_INCLUDE_ENABLE_SELF_DEVICE_ADMIN_PLUGIN_H

#include "enable_self_device_admin_param.h"
#include "plugin_singleton.h"

namespace OHOS {
namespace EDM {
class EnableSelfDeviceAdminPlugin : public PluginSingleton<EnableSelfDeviceAdminPlugin, EnableSelfDeviceAdminParam> {
public:
    void InitPlugin(std::shared_ptr<IPluginTemplate<EnableSelfDeviceAdminPlugin,
        EnableSelfDeviceAdminParam>> ptr) override;

    ErrCode OnSetPolicy();
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_PLUGIN_INCLUDE_ENABLE_SELF_DEVICE_ADMIN_PLUGIN_H
