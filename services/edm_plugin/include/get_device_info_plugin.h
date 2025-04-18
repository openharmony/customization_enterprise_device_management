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

#ifndef SERVICES_EDM_PLUGIN_INCLUDE_GET_DEVICE_INFO_PLUGIN_H
#define SERVICES_EDM_PLUGIN_INCLUDE_GET_DEVICE_INFO_PLUGIN_H

#include "cJSON.h"

#include "iexternal_manager_factory.h"
#include "plugin_singleton.h"
#include "string_serializer.h"

namespace OHOS {
namespace EDM {
class GetDeviceInfoPlugin : public PluginSingleton<GetDeviceInfoPlugin, std::string> {
public:
    void InitPlugin(std::shared_ptr<IPluginTemplate<GetDeviceInfoPlugin, std::string>> ptr) override;

    ErrCode OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply, int32_t userId) override;

protected:
    virtual std::shared_ptr<IExternalManagerFactory> GetExternalManagerFactory();

private:
    ErrCode GetDeviceName(MessageParcel &reply);
    ErrCode GetDeviceSerial(MessageParcel &reply);
    ErrCode GetSimInfo(MessageParcel &reply);
    bool GetSimInfoBySlotId(int32_t slotId, cJSON *simJson);

    std::shared_ptr<IExternalManagerFactory> externalManagerFactory_ = nullptr;
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_PLUGIN_INCLUDE_GET_DEVICE_INFO_PLUGIN_H
