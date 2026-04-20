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

#include "allowed_notification_bundles_plugin_fuzzer.h"

#include <system_ability_definition.h>

#define private public
#include "allowed_notification_bundles_plugin.h"
#undef private
#include "allowed_notification_bundles_type.h"
#include "common_fuzzer.h"
#include "edm_ipc_interface_code.h"
#include "func_code.h"
#include "ienterprise_device_mgr.h"
#include "message_parcel.h"
#include "utils.h"

namespace OHOS {
namespace EDM {
constexpr size_t MIN_SIZE = 32;
constexpr int32_t HAS_USERID = 1;
constexpr int32_t USER_ID = 100;

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
    for (uint32_t operateType = static_cast<uint32_t>(FuncOperateType::GET);
        operateType <= static_cast<uint32_t>(FuncOperateType::REMOVE); operateType++) {
        uint32_t code = EdmInterfaceCode::ALLOWED_NOTIFICATION_BUNDLES;
        code = POLICY_FUNC_CODE(operateType, code);

        AppExecFwk::ElementName admin;
        admin.SetBundleName(CommonFuzzer::GetString(data, pos, stringSize, size));
        admin.SetAbilityName(CommonFuzzer::GetString(data, pos, stringSize, size));
        MessageParcel parcel;
        parcel.WriteInterfaceToken(IEnterpriseDeviceMgrIdl::GetDescriptor());
        parcel.WriteInt32(HAS_USERID);
        parcel.WriteInt32(USER_ID);
        if (operateType) {
            parcel.WriteParcelable(&admin);
            std::vector<AllowedNotificationBundlesType> bundlesTypes;
            AllowedNotificationBundlesType bundlesType;
            bundlesType.userId = CommonFuzzer::GetU32Data(data);
            std::set<std::string> trustList;
            trustList.insert(CommonFuzzer::GetString(data, pos, stringSize, size));
            bundlesType.trustList = trustList;
            bundlesTypes.push_back(bundlesType);
            parcel.WriteInt32(static_cast<int32_t>(bundlesTypes.size()));
            for (const auto& item : bundlesTypes) {
                parcel.WriteInt32(item.userId);
                parcel.WriteInt32(static_cast<int32_t>(item.trustList.size()));
                for (const auto& bundle : item.trustList) {
                    parcel.WriteString(bundle);
                }
            }
        } else {
            parcel.WriteInt32(0);
            parcel.WriteParcelable(&admin);
        }
        CommonFuzzer::OnRemoteRequestFuzzerTest(code, data, size, parcel);
    }

    auto plugin = AllowedNotificationBundlesPlugin::GetPlugin();
    std::vector<AllowedNotificationBundlesType> dataVec;
    AllowedNotificationBundlesType bundlesType;
    bundlesType.userId = CommonFuzzer::GetU32Data(data);
    std::set<std::string> trustList;
    trustList.insert(CommonFuzzer::GetString(data, pos, stringSize, size));
    bundlesType.trustList = trustList;
    dataVec.push_back(bundlesType);
    
    std::vector<AllowedNotificationBundlesType> currentData;
    std::vector<AllowedNotificationBundlesType> mergeData;
    int32_t userId = CommonFuzzer::GetU32Data(data);
    plugin->OnSetPolicy(dataVec, currentData, mergeData, userId);
    plugin->OnRemovePolicy(dataVec, currentData, mergeData, userId);
    
    std::string adminName = CommonFuzzer::GetString(data, pos, stringSize, size);
    plugin->OnAdminRemove(adminName, currentData, mergeData, userId);
    return 0;
}
} // namespace EDM
} // namespace OHOS