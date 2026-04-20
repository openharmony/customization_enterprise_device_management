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

#include "query_traffic_stats_plugin_fuzzer.h"

#include <system_ability_definition.h>

#define protected public
#define private public
#include "query_traffic_stats_plugin.h"
#undef protected
#undef private
#include "common_fuzzer.h"
#include "edm_ipc_interface_code.h"
#include "func_code.h"
#include "ienterprise_device_mgr.h"
#include "message_parcel.h"
#include "net_stats_utils.h"
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
    int32_t stringSize = size / 10;
    uint32_t code = EdmInterfaceCode::QUERY_TRAFFIC_STATS;
    code = POLICY_FUNC_CODE(static_cast<uint32_t>(FuncOperateType::GET), code);

    AppExecFwk::ElementName admin;
    admin.SetBundleName(CommonFuzzer::GetString(data, pos, stringSize, size));
    admin.SetAbilityName(CommonFuzzer::GetString(data, pos, stringSize, size));
    MessageParcel parcel;
    parcel.WriteInterfaceToken(IEnterpriseDeviceMgrIdl::GetDescriptor());
    parcel.WriteInt32(HAS_USERID);
    parcel.WriteInt32(USER_ID);
    parcel.WriteInt32(0);
    parcel.WriteParcelable(&admin);
    
    NetStatsNetwork networkInfo;
    networkInfo.type = CommonFuzzer::GetU32Data(data) % 3;
    networkInfo.startTime = CommonFuzzer::GetLong(data, pos, size);
    networkInfo.endTime = CommonFuzzer::GetLong(data, pos, size);
    networkInfo.simId = CommonFuzzer::GetU32Data(data);
    networkInfo.bundleName = CommonFuzzer::GetString(data, pos, stringSize, size);
    networkInfo.accountId = CommonFuzzer::GetU32Data(data);
    networkInfo.appIndex = CommonFuzzer::GetU32Data(data);
    NetStatsNetwork::Marshalling(parcel, networkInfo);
    
    CommonFuzzer::OnRemoteRequestFuzzerTest(code, data, size, parcel);

    QueryTrafficStatsPlugin plugin;
    MessageParcel dataParcel;
    MessageParcel reply;
    std::string policyData;
    NetStatsNetwork::Marshalling(dataParcel, networkInfo);
    int32_t userId = CommonFuzzer::GetU32Data(data);
    plugin.OnGetPolicy(policyData, dataParcel, reply, userId);
    return 0;
}
} // namespace EDM
} // namespace OHOS