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

#ifndef INTERFACES_INNER_API_APPLICATION_MANAGER_INCLUDE_WINDOW_STATE_INFO_H
#define INTERFACES_INNER_API_APPLICATION_MANAGER_INCLUDE_WINDOW_STATE_INFO_H

#include <string>
#include "message_parcel.h"

namespace OHOS {
namespace EDM {
enum WindowState : uint32_t {
    DISCONNECT = 0,
    CONNECT = 1,
    FOREGROUND = 2,
    ACTIVE = 3,
    INACTIVE = 4,
    BACKGROUND = 5
};
struct WindowStateInfo {
    int32_t windowId;
    WindowState state;
    bool isOnDock;
    std::string name;
};

class WindowStateInfoHandle {
public:
    static bool ReadWindowStateInfoVector(MessageParcel &reply, std::vector<WindowStateInfo> &windowStateInfos);
    static bool ReadWindowStateInfo(MessageParcel &reply, WindowStateInfo &windowStateInfo);
    static bool WriteWindowStateInfoVector(MessageParcel &data, const std::vector<WindowStateInfo> &windowStateInfos);
    static bool WriteWindowStateInfo(MessageParcel &data, const WindowStateInfo &windowState);
};
} // namespace EDM
} // namespace OHOS

#endif // INTERFACES_INNER_API_APPLICATION_MANAGER_INCLUDE_WINDOW_STATE_INFO_H