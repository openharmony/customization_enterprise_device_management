/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef EDM_UNIT_TEST_EDM_PLUGIN_EDM_DATA_ABILITY_UTILS_H
#define EDM_UNIT_TEST_EDM_PLUGIN_EDM_DATA_ABILITY_UTILS_H

#include <gmock/gmock.h>
#include <iostream>
#include <string>
#include "edm_errors.h"

namespace OHOS {
namespace EDM {
class EdmDataAbilityUtils {
public:
    static ErrCode GetStringFromSettingsDataShare(const std::string &key, std::string &value);
    static ErrCode GetIntFromSettingsDataShare(const std::string &key, int32_t &value);
    static ErrCode GetStringFromSettingsDataShare(const std::string &settingsDataUri, const std::string &key,
        std::string &value);
    static ErrCode GetIntFromSettingsDataShare(const std::string &strUri, const std::string &key, int32_t &value);
    static void SetResult(const std::string &result);
private:
    static std::string result_;
};
} // namespace EDM
} // namespace OHOS
#endif // EDM_UNIT_TEST_EDM_PLUGIN_EDM_DATA_ABILITY_UTILS_H