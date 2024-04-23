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

#ifndef SERVICES_EDM_INCLUDE_STRATEGY_REPLACE_EXECUTE_STRATEGY_H
#define SERVICES_EDM_INCLUDE_STRATEGY_REPLACE_EXECUTE_STRATEGY_H

#include "edm_errors.h"
#include "iplugin_execute_strategy.h"

namespace OHOS {
namespace EDM {
class ReplaceExecuteStrategy : public IPluginExecuteStrategy {
public:
    ~ReplaceExecuteStrategy() override = default;
    ErrCode OnGetExecute(std::uint32_t funcCode, std::string &policyData, MessageParcel &data, MessageParcel &reply,
        int32_t userId) override;
    ErrCode OnSetExecute(std::uint32_t funcCode, MessageParcel &data, MessageParcel &reply,
        HandlePolicyData &policyData, int32_t userId) override;
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_INCLUDE_STRATEGY_REPLACE_EXECUTE_STRATEGY_H
