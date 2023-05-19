/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef COMMON_NATIVE_INCLUDE_EDM_DATA_ABILITY_UTILS_H
#define COMMON_NATIVE_INCLUDE_EDM_DATA_ABILITY_UTILS_H

#include <iostream>
#include <string>
#include "edm_errors.h"

namespace OHOS {
namespace EDM {
class EdmDataAbilityUtils {
public:
    static const std::string SETTINGS_DATA_BASE_URI;

    static ErrCode GetStringFromDataShare(const std::string &dataBaseUri, const std::string &key, std::string &value);
    static ErrCode GetIntFromDataShare(const std::string &dataBaseUri, const std::string &key, int32_t &result);
};
} // namespace EDM
} // namespace OHOS
#endif // COMMON_NATIVE_INCLUDE_EDM_DATA_ABILITY_UTILS_H