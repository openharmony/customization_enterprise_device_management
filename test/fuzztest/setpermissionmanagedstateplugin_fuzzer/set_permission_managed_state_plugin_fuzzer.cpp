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

#include "set_permission_managed_state_plugin_fuzzer.h"

#include <system_ability_definition.h>

#define private public
#include "set_permission_managed_state_plugin.h"
#undef private
#include "common_fuzzer.h"
#include "edm_ipc_interface_code.h"
#include "func_code.h"
#include "ienterprise_device_mgr.h"
#include "message_parcel.h"
#include "permission_managed_state_info.h"
#include "utils.h"

namespace OHOS {
namespace EDM {
constexpr size_t MIN_SIZE = 32;

extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    TEST::Utils::SetEdmPermissions();
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return 0;
    }
    if (size < MIN_SIZE) {
        return 0;
    }
    int32_t pos = 0;
    int32_t stringSize = size / 12;
    SetPermissionManagedStatePlugin plugin;
    std::map<std::string, PermissionManagedStateInfo> dataMap;
    PermissionManagedStateInfo info;
    info.tokenId = CommonFuzzer::GetU32Data(data);
    info.permissionName = CommonFuzzer::GetString(data, pos, stringSize, size);
    info.permissionNames.push_back(info.permissionName);
    info.managedState = CommonFuzzer::GetU32Data(data);
    std::string key = CommonFuzzer::GetString(data, pos, stringSize, size);
    dataMap[key] = info;
    
    std::map<std::string, PermissionManagedStateInfo> currentData;
    std::map<std::string, PermissionManagedStateInfo> mergeData;
    int32_t userId = CommonFuzzer::GetU32Data(data);
    plugin.OnSetPolicy(dataMap, currentData, mergeData, userId);
    
    std::string adminName = CommonFuzzer::GetString(data, pos, stringSize, size);
    plugin.OnAdminRemove(adminName, currentData, mergeData, userId);
    return 0;
}
} // namespace EDM
} // namespace OHOS