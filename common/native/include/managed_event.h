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

#ifndef COMMON_NATIVE_INCLUDE_MANAGED_EVENT_H
#define COMMON_NATIVE_INCLUDE_MANAGED_EVENT_H

#include <cstdint>

namespace OHOS {
namespace EDM {
enum class ManagedEvent : uint32_t {
    BUNDLE_ADDED = 0,
    BUNDLE_REMOVED = 1,
    APP_START = 2,
    APP_STOP = 3,
    SYSTEM_UPDATE = 4,
    USER_ADDED = 5,
    USER_SWITCHED = 6,
    USER_REMOVED = 7
};
} // namespace EDM
} // namespace OHOS

#endif // COMMON_NATIVE_INCLUDE_MANAGED_EVENT_H
