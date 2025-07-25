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

#include "admin_policies_storage_rdb_fuzzer.h"

#include "cJSON.h"
#include "common_fuzzer.h"
#include "edm_ipc_interface_code.h"
#include "func_code.h"
#include "get_data_template.h"
#include "message_parcel.h"
#define private public
#include "admin_policies_storage_rdb.h"
#undef private
#include "ienterprise_device_mgr.h"

namespace OHOS {
namespace EDM {
constexpr size_t MIN_SIZE = 64;

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return 0;
    }
    if (size < MIN_SIZE) {
        return 0;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<AdminPoliciesStorageRdb> adminPoliciesStorageRdb = AdminPoliciesStorageRdb::GetInstance();
    int32_t userId = CommonFuzzer::GetU32Data(data);
    std::string fuzzString(reinterpret_cast<const char*>(data), size);
    ManagedEvent event = GetData<ManagedEvent>();
    Admin admin;
    AdminInfo fuzzAdminInfo;
    EntInfo entInfo;
    entInfo.enterpriseName = fuzzString;
    entInfo.description = fuzzString;
    fuzzAdminInfo.packageName_ = fuzzString;
    fuzzAdminInfo.className_ = fuzzString;
    fuzzAdminInfo.entInfo_ = entInfo;
    fuzzAdminInfo.permission_ = { fuzzString };
    fuzzAdminInfo.managedEvents_ = { event };
    fuzzAdminInfo.parentAdminName_ = fuzzString;
    admin.adminInfo_ = fuzzAdminInfo;
    std::vector<std::string> permissions = { fuzzString };
    adminPoliciesStorageRdb->InsertAdmin(userId, admin);
    adminPoliciesStorageRdb->UpdateAdmin(userId, admin);
    adminPoliciesStorageRdb->CreateInsertValuesBucket(userId, admin);
    std::string packageName(reinterpret_cast<const char*>(data), size);
    std::string currentParentName = fuzzString;
    std::string targetParentName = fuzzString;
    std::string stringInfo = fuzzString;
    std::vector<std::string> info = { fuzzString };
    adminPoliciesStorageRdb->DeleteAdmin(userId, packageName);
    adminPoliciesStorageRdb->UpdateEntInfo(userId, packageName, entInfo);
    std::vector<ManagedEvent> managedEvents = {event};
    adminPoliciesStorageRdb->UpdateManagedEvents(userId, packageName, managedEvents);
    adminPoliciesStorageRdb->ReplaceAdmin(packageName, userId, admin);
    adminPoliciesStorageRdb->UpdateParentName(packageName, currentParentName, targetParentName);
    adminPoliciesStorageRdb->SetAdminStringInfo(stringInfo, info);
    std::shared_ptr<NativeRdb::ResultSet> resultSet;
    std::shared_ptr<Admin> item = std::make_shared<Admin>(admin);
    adminPoliciesStorageRdb->SetAdminItems(resultSet, item);
    adminPoliciesStorageRdb->SetManagedEventStr(resultSet, item);
    adminPoliciesStorageRdb->QueryAllAdmin();
    return 0;
}
} // namespace EDM
} // namespace OHOS