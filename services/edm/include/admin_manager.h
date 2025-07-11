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

#ifndef SERVICES_EDM_INCLUDE_ADMIN_MANAGER_H
#define SERVICES_EDM_INCLUDE_ADMIN_MANAGER_H

#include <map>
#include <memory>
#include <unordered_map>

#include "admin.h"
#include "admin_policies_storage_rdb.h"
#include "ent_info.h"

namespace OHOS {
namespace EDM {
class AdminManager : public std::enable_shared_from_this<AdminManager> {
public:
    static std::shared_ptr<AdminManager> GetInstance();
    bool GetAdminByUserId(int32_t userId, std::vector<std::shared_ptr<Admin>> &userAdmin);
    void GetAdminBySubscribeEvent(ManagedEvent event,
        std::unordered_map<int32_t, std::vector<std::shared_ptr<Admin>>> &subscribeAdmins);
    std::shared_ptr<Admin> GetAdminByPkgName(const std::string &packageName, int32_t userId);
    ErrCode DeleteAdmin(const std::string &packageName, int32_t userId);
    ErrCode UpdateAdmin(std::shared_ptr<Admin> getAdmin, int32_t userId, const Admin &adminItem);
    bool IsSuperAdminExist();
    bool IsByodAdminExist();
    bool IsSuperAdmin(const std::string &bundleName);
    bool IsAdminExist();
    bool IsByodAdmin(const std::string &bundleName, int32_t userId);
    bool IsSuperOrSubSuperAdmin(const std::string &bundleName);
    bool HasPermissionToHandlePolicy(std::shared_ptr<Admin> admin, const std::string &policyName);
    void GetEnabledAdmin(AdminType role, std::vector<std::string> &packageNameList, int32_t userId);
    std::shared_ptr<Admin> GetSuperAdmin();
    void Init();
    ErrCode SetAdminValue(int32_t userId, const Admin &adminItem);
    ErrCode GetEntInfo(const std::string &packageName, EntInfo &entInfo, int32_t userId);
    ErrCode SetEntInfo(const std::string &packageName, const EntInfo &entInfo, int32_t userId);
    ErrCode SaveSubscribeEvents(const std::vector<uint32_t> &events, const std::string &bundleName, int32_t userId);
    ErrCode RemoveSubscribeEvents(const std::vector<uint32_t> &events, const std::string &bundleName, int32_t userId);
    ErrCode GetPoliciesByVirtualAdmin(const std::string &bundleName, const std::string &parentName,
        std::vector<std::string> &policies);
    void GetVirtualAdminsByPolicy(const std::string &policyName, const std::string &parentName,
        std::vector<std::string> &bundleNames);
    ErrCode GetSubOrSuperOrByodAdminByPkgName(const std::string &subAdminName,
        std::shared_ptr<Admin> &subOrSuperOrByodAdmin);
    void GetAdmins(std::vector<std::shared_ptr<Admin>> &admins, int32_t currentUserId);
    bool GetAdminsByTypeAndUserId(AdminType type, std::vector<std::shared_ptr<Admin>> &admins, int32_t userId);
    ErrCode GetSubSuperAdminsByParentName(const std::string &parentName, std::vector<std::string> &subAdmins);
    ErrCode ReplaceSuperAdminByPackageName(const std::string &packageName, const Admin &newAdmin);
    ~AdminManager();
    void Dump();
    void ClearAdmins();
    void InsertAdmins(int32_t userId, std::vector<std::shared_ptr<Admin>> admins);

private:
    AdminManager();

    static std::once_flag flag_;
    static std::shared_ptr<AdminManager> instance_;
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_INCLUDE_ADMIN_MANAGER_H
