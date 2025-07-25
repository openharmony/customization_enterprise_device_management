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

#include "admin_manager.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>

#include "admin_container.h"
#include "directory_ex.h"
#include "edm_constants.h"
#include "edm_log.h"

namespace OHOS {
namespace EDM {
std::shared_ptr<AdminManager> AdminManager::instance_;
std::once_flag AdminManager::flag_;
const std::string POLICY_ALLOW_ALL = "allow_all";

std::shared_ptr<AdminManager> AdminManager::GetInstance()
{
    std::call_once(flag_, []() {
        if (instance_ == nullptr) {
            instance_.reset(new (std::nothrow) AdminManager());
        }
    });
    return instance_;
}

AdminManager::AdminManager()
{
    EDMLOGI("AdminManager::AdminManager");
}

AdminManager::~AdminManager()
{
    EDMLOGI("AdminManager::~AdminManager");
}

bool AdminManager::GetAdminByUserId(int32_t userId, std::vector<std::shared_ptr<Admin>> &userAdmin)
{
    return AdminContainer::GetInstance()->GetAdminCopyByUserId(userId, userAdmin);
}

void AdminManager::GetAdminBySubscribeEvent(ManagedEvent event,
    std::unordered_map<int32_t, std::vector<std::shared_ptr<Admin>>> &subscribeAdmins)
{
    return AdminContainer::GetInstance()->GetAdminCopyBySubscribeEvent(event, subscribeAdmins);
}

ErrCode AdminManager::SetAdminValue(int32_t userId, const Admin &adminItem)
{
    std::shared_ptr<Admin> getAdmin = GetAdminByPkgName(adminItem.adminInfo_.packageName_, userId);
    if (getAdmin != nullptr) {
        return UpdateAdmin(getAdmin, userId, adminItem);
    }
    auto adminPoliciesStorageRdb = AdminPoliciesStorageRdb::GetInstance();
    if (adminPoliciesStorageRdb == nullptr) {
        EDMLOGE("AdminManager::SetAdminValue get adminPoliciesStorageRdb failed.");
        return ERR_GET_STORAGE_RDB_FAILED;
    }
    if (!adminPoliciesStorageRdb->InsertAdmin(userId, adminItem)) {
        EDMLOGE("AdminManager::SetAdminValue insert failed.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    AdminContainer::GetInstance()->SetAdminByUserId(userId, adminItem);
    return ERR_OK;
}

std::shared_ptr<Admin> AdminManager::GetAdminByPkgName(const std::string &packageName, int32_t userId)
{
    std::shared_ptr<Admin> subOrSuperOrByodAdmin;
    if (userId != EdmConstants::DEFAULT_USER_ID &&
        SUCCEEDED(GetSubOrSuperOrByodAdminByPkgName(packageName, subOrSuperOrByodAdmin))) {
        EDMLOGD("GetAdminByPkgName::get sub-super or super or byod admin: %{public}s", packageName.c_str());
        return subOrSuperOrByodAdmin;
    }
    std::vector<std::shared_ptr<Admin>> userAdmin;
    if (!GetAdminByUserId(userId, userAdmin)) {
        EDMLOGW("GetAdminByPkgName::get userId Admin failed.");
        return nullptr;
    }
    for (const auto &item : userAdmin) {
        if (item->adminInfo_.packageName_ == packageName) {
            return item;
        }
    }
    EDMLOGD("GetAdminByPkgName::get admin failed. admin size = %{public}u, packageName = %{public}s",
        (uint32_t)userAdmin.size(), packageName.c_str());
    return nullptr;
}

ErrCode AdminManager::DeleteAdmin(const std::string &packageName, int32_t userId)
{
    if (!AdminContainer::GetInstance()->HasAdmin(userId)) {
        EDMLOGW("DeleteAdmin::get userId Admin failed.");
        return ERR_EDM_UNKNOWN_ADMIN;
    }
    auto adminPoliciesStorageRdb = AdminPoliciesStorageRdb::GetInstance();
    if (adminPoliciesStorageRdb == nullptr) {
        EDMLOGE("AdminManager::DeleteAdmin get adminPoliciesStorageRdb failed.");
        return ERR_GET_STORAGE_RDB_FAILED;
    }
    if (!adminPoliciesStorageRdb->DeleteAdmin(userId, packageName)) {
        EDMLOGW("delete admin (%{public}s) failed!", packageName.c_str());
        return ERR_EDM_DEL_ADMIN_FAILED;
    }
    AdminContainer::GetInstance()->DeleteAdmin(packageName, userId);
    return ERR_OK;
}

ErrCode AdminManager::UpdateAdmin(std::shared_ptr<Admin> getAdmin, int32_t userId, const Admin &adminItem)
{
    if (getAdmin == nullptr) {
        EDMLOGW("UpdateAdmin::get null admin, never get here");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    if (getAdmin->GetAdminType() != AdminType::NORMAL && getAdmin->GetAdminType() != adminItem.GetAdminType()) {
        EDMLOGE("AdminManager::UpdateAdmin sub-super or delegated admin can not update to another type.");
        return EdmReturnErrCode::ADMIN_EDM_PERMISSION_DENIED;
    }
    if (getAdmin->GetAdminType() == AdminType::NORMAL && adminItem.GetAdminType() != AdminType::NORMAL &&
        adminItem.GetAdminType() != AdminType::ENT) {
        EDMLOGE("AdminManager::UpdateAdmin normal admin can not update to sub-super admin or delegated admin.");
        return EdmReturnErrCode::ADMIN_EDM_PERMISSION_DENIED;
    }
    auto adminPoliciesStorageRdb = AdminPoliciesStorageRdb::GetInstance();
    if (adminPoliciesStorageRdb == nullptr) {
        EDMLOGE("AdminManager::UpdateAdmin get adminPoliciesStorageRdb failed.");
        return ERR_GET_STORAGE_RDB_FAILED;
    }
    if (!adminPoliciesStorageRdb->UpdateAdmin(userId, adminItem)) {
        EDMLOGW("UpdateAdmin::update admin failed.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    AdminContainer::GetInstance()->UpdateAdmin(userId, getAdmin->adminInfo_.packageName_,
        ADMIN_TYPE | ENTI_NFO | PERMISSION | ACCESSIBLE_POLICIES | RUNNING_MODE, adminItem);
    return ERR_OK;
}

ErrCode AdminManager::ReplaceSuperAdminByPackageName(const std::string &packageName, const Admin &newAdmin)
{
    auto adminPoliciesStorageRdb = AdminPoliciesStorageRdb::GetInstance();
    if (adminPoliciesStorageRdb == nullptr) {
        EDMLOGE("AdminManager::ReplaceSuperAdminByAdmin get adminPoliciesStorageRdb failed.");
        return ERR_GET_STORAGE_RDB_FAILED;
    }
    std::vector<std::string> subAdmins;
    GetSubSuperAdminsByParentName(packageName, subAdmins);
    for (const auto& subAdmin : subAdmins) {
        if (!adminPoliciesStorageRdb->UpdateParentName(subAdmin, packageName,
            newAdmin.adminInfo_.packageName_)) {
            EDMLOGE("AdminManager::ReplaceSuperAdminByPackageName UpdateParentName failed.");
            return ERR_SET_PARENT_ADMIN_FAILED;
        }
    }

    if (!adminPoliciesStorageRdb->ReplaceAdmin(packageName, EdmConstants::DEFAULT_USER_ID, newAdmin)) {
        EDMLOGW("ReplaceSuperAdminByPackageName::update admin failed.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }

    AdminContainer::GetInstance()->UpdateAdmin(EdmConstants::DEFAULT_USER_ID, packageName,
        PACKAGE_NAME | CLASS_NAME, newAdmin);
    AdminContainer::GetInstance()->UpdateParentAdminName(EdmConstants::DEFAULT_USER_ID, packageName,
        newAdmin.adminInfo_.packageName_);
    return ERR_OK;
}

// success is returned as long as there is a super administrator
bool AdminManager::IsSuperAdminExist()
{
    std::vector<std::shared_ptr<Admin>> userAdmin;
    bool ret = GetAdminByUserId(EdmConstants::DEFAULT_USER_ID, userAdmin);
    if (!ret) {
        EDMLOGD("IsSuperAdminExist::not find byod or super Admin");
        return false;
    }
    return std::any_of(userAdmin.begin(), userAdmin.end(),
        [](const std::shared_ptr<Admin> &admin) {
            return admin->adminInfo_.adminType_ == AdminType::ENT && !admin->adminInfo_.isDebug_;
        });
}

bool AdminManager::IsByodAdminExist()
{
    std::vector<std::shared_ptr<Admin>> userAdmin;
    bool ret = GetAdminByUserId(EdmConstants::DEFAULT_USER_ID, userAdmin);
    if (!ret) {
        EDMLOGD("IsByodAdminExist::not find byod or super Admin");
        return false;
    }
    return std::any_of(userAdmin.begin(), userAdmin.end(),
        [](const std::shared_ptr<Admin> &admin) {
            return admin->adminInfo_.adminType_ == AdminType::BYOD;
        });
}

bool AdminManager::IsSuperAdmin(const std::string &bundleName)
{
    std::shared_ptr<Admin> admin = GetAdminByPkgName(bundleName, EdmConstants::DEFAULT_USER_ID);
    if (admin == nullptr) {
        EDMLOGW("IsSuperAdmin: admin == nullptr.");
        return false;
    }
    if (admin->adminInfo_.adminType_ == AdminType::ENT) {
        EDMLOGW("IsSuperAdmin: admin->adminInfo_.adminType_ == AdminType::ENT.");
        return true;
    }
    return false;
}

bool AdminManager::IsAdminExist()
{
    return AdminContainer::GetInstance()->IsAdminExist();
}

bool AdminManager::IsByodAdmin(const std::string &bundleName, int32_t userId)
{
    std::shared_ptr<Admin> admin = GetAdminByPkgName(bundleName, userId);
    if (admin == nullptr) {
        EDMLOGW("IsByodAdmin: admin == nullptr.");
        return false;
    }
    if (admin->adminInfo_.adminType_ == AdminType::BYOD) {
        EDMLOGI("IsByodAdmin: admin is BYOD.");
        return true;
    }
    return false;
}

bool AdminManager::IsSuperOrSubSuperAdmin(const std::string &bundleName)
{
    std::shared_ptr<Admin> superAdmin;
    superAdmin = GetAdminByPkgName(bundleName, EdmConstants::DEFAULT_USER_ID);
    if (superAdmin == nullptr) {
        return false;
    }
    return superAdmin->adminInfo_.adminType_ == AdminType::ENT ||
        superAdmin->adminInfo_.adminType_ == AdminType::SUB_SUPER_ADMIN;
}

/*
 * There are different administrator types according to the input parameters.
 * Returns a list of package names
 */
void AdminManager::GetEnabledAdmin(AdminType role, std::vector<std::string> &packageNameList, int32_t userId)
{
    packageNameList.clear();
    std::vector<std::shared_ptr<Admin>> userAdmin;
    bool ret = GetAdminByUserId(userId, userAdmin);
    if (!ret) {
        EDMLOGW("GetEnabledAdmin::not find enabled Admin.");
        return;
    }
    EDMLOGD("AdminManager:GetEnabledAdmin adminType: %{public}d , admin size: %{public}zu", role, userAdmin.size());
    if (static_cast<int32_t>(role) > static_cast<int32_t>(AdminType::BYOD) ||
        static_cast<int32_t>(role) < static_cast<int32_t>(AdminType::NORMAL)) {
        EDMLOGD("there is no admin(%{public}u) device manager package name list!", role);
        return;
    }

    for (const auto &item : userAdmin) {
        if (item->adminInfo_.adminType_ == role) {
            std::string adminName = item->adminInfo_.packageName_ + "/" + item->adminInfo_.className_;
            packageNameList.push_back(adminName);
        }
    }
}

ErrCode AdminManager::GetSubOrSuperOrByodAdminByPkgName(const std::string &subAdminName,
    std::shared_ptr<Admin> &subOrSuperOrByodAdmin)
{
    std::vector<std::shared_ptr<Admin>> userAdmin;
    if (!GetAdminByUserId(EdmConstants::DEFAULT_USER_ID, userAdmin)) {
        EDMLOGW("GetSubOrSuperOrByodAdminByPkgName::not find Admin under default user id");
        return ERR_EDM_SUPER_ADMIN_NOT_FOUND;
    }
    auto adminItem = std::find_if(userAdmin.begin(), userAdmin.end(), [&](const std::shared_ptr<Admin> &admin) {
        return admin->adminInfo_.packageName_ == subAdminName && (admin->GetAdminType() == AdminType::ENT ||
            admin->GetAdminType() == AdminType::SUB_SUPER_ADMIN);
    });
    if (adminItem == userAdmin.end()) {
        EDMLOGW("GetSubOrSuperOrByodAdminByPkgName::not find sub-super admin or super or byod Admin");
        return ERR_EDM_SUPER_ADMIN_NOT_FOUND;
    }
    subOrSuperOrByodAdmin = *adminItem;
    return ERR_OK;
}

void AdminManager::GetAdmins(std::vector<std::shared_ptr<Admin>> &admins, int32_t currentUserId)
{
    std::vector<std::shared_ptr<Admin>> userAdmin;
    GetAdminByUserId(EdmConstants::DEFAULT_USER_ID, userAdmin);
    std::copy_if(userAdmin.begin(), userAdmin.end(), std::back_inserter(admins), [&](std::shared_ptr<Admin> admin) {
        return admin->adminInfo_.adminType_ == AdminType::ENT || admin->adminInfo_.adminType_ == AdminType::BYOD;
    });
    GetAdminByUserId(currentUserId, userAdmin);
    std::copy_if(userAdmin.begin(), userAdmin.end(), std::back_inserter(admins), [&](std::shared_ptr<Admin> admin) {
        return admin->adminInfo_.adminType_ == AdminType::NORMAL;
    });
}

ErrCode AdminManager::GetSubSuperAdminsByParentName(const std::string &parentName, std::vector<std::string> &subAdmins)
{
    if (subAdmins.size() > 0) {
        subAdmins.clear();
    }
    std::vector<std::shared_ptr<Admin>> userAdmin;
    if (!GetAdminByUserId(EdmConstants::DEFAULT_USER_ID, userAdmin)) {
        EDMLOGE("GetSubSuperAdminsByParentName::not find Admin under default user id.");
        return ERR_EDM_SUPER_ADMIN_NOT_FOUND;
    }
    for (const auto &admin : userAdmin) {
        if ((admin->GetAdminType() == AdminType::SUB_SUPER_ADMIN ||
            admin->GetAdminType() == AdminType::VIRTUAL_ADMIN) &&
            admin->GetParentAdminName() == parentName) {
            subAdmins.push_back(admin->adminInfo_.packageName_);
        }
    }
    return ERR_OK;
}

ErrCode AdminManager::GetEntInfo(const std::string &packageName, EntInfo &entInfo, int32_t userId)
{
    std::vector<std::shared_ptr<Admin>> userAdmin;
    bool ret = GetAdminByUserId(userId, userAdmin);
    if (!ret) {
        EDMLOGW("GetEntInfo::not find Admin.");
        return ERR_EDM_UNKNOWN_ADMIN;
    }
    for (const auto &item : userAdmin) {
        if (item->adminInfo_.packageName_ == packageName) {
            entInfo = item->adminInfo_.entInfo_;
            return ERR_OK;
        }
    }
    return ERR_EDM_UNKNOWN_ADMIN;
}

ErrCode AdminManager::SetEntInfo(const std::string &packageName, const EntInfo &entInfo, int32_t userId)
{
    std::vector<std::shared_ptr<Admin>> userAdmin;
    bool ret = GetAdminByUserId(userId, userAdmin);
    if (!ret) {
        EDMLOGW("SetEntInfo::not find Admin.");
        return ERR_EDM_UNKNOWN_ADMIN;
    }
    auto adminPoliciesStorageRdb = AdminPoliciesStorageRdb::GetInstance();
    if (adminPoliciesStorageRdb == nullptr) {
        EDMLOGE("AdminManager::SetEntInfo get adminPoliciesStorageRdb failed.");
        return ERR_GET_STORAGE_RDB_FAILED;
    }
    for (auto &item : userAdmin) {
        if (item->adminInfo_.packageName_ == packageName &&
            adminPoliciesStorageRdb->UpdateEntInfo(userId, packageName, entInfo)) {
            Admin tempAdmin;
            tempAdmin.adminInfo_.entInfo_ = entInfo;
            AdminContainer::GetInstance()->UpdateAdmin(userId, packageName, ENTI_NFO, tempAdmin);
            return ERR_OK;
        }
    }
    return ERR_EDM_UNKNOWN_ADMIN;
}

ErrCode AdminManager::SaveSubscribeEvents(const std::vector<uint32_t> &events, const std::string &bundleName,
    int32_t userId)
{
    std::shared_ptr<Admin> admin = GetAdminByPkgName(bundleName, userId);
    if (admin == nullptr) {
        return ERR_EDM_UNKNOWN_ADMIN;
    }
    auto adminPoliciesStorageRdb = AdminPoliciesStorageRdb::GetInstance();
    if (adminPoliciesStorageRdb == nullptr) {
        EDMLOGE("AdminManager::SaveSubscribeEvents get adminPoliciesStorageRdb failed.");
        return ERR_GET_STORAGE_RDB_FAILED;
    }
    size_t eventsNumber = admin->adminInfo_.managedEvents_.size();
    for (const auto &event : events) {
        std::vector<ManagedEvent> managedEvents = admin->adminInfo_.managedEvents_;
        ManagedEvent subEvent = static_cast<ManagedEvent>(event);
        if (std::find(managedEvents.begin(), managedEvents.end(), subEvent) == managedEvents.end()) {
            admin->adminInfo_.managedEvents_.push_back(subEvent);
        }
    }
    if (admin->adminInfo_.managedEvents_.size() > eventsNumber &&
        !adminPoliciesStorageRdb->UpdateManagedEvents(userId, admin->adminInfo_.packageName_,
            admin->adminInfo_.managedEvents_)) {
        return ERR_EDM_UNKNOWN_ADMIN;
    }
    AdminContainer::GetInstance()->UpdateAdmin(userId, admin->adminInfo_.packageName_, MANAGED_EVENTS, *admin);
    return ERR_OK;
}

ErrCode AdminManager::RemoveSubscribeEvents(const std::vector<uint32_t> &events, const std::string &bundleName,
    int32_t userId)
{
    std::shared_ptr<Admin> admin = GetAdminByPkgName(bundleName, userId);
    if (admin == nullptr) {
        return ERR_EDM_UNKNOWN_ADMIN;
    }
    auto adminPoliciesStorageRdb = AdminPoliciesStorageRdb::GetInstance();
    if (adminPoliciesStorageRdb == nullptr) {
        EDMLOGE("AdminManager::RemoveSubscribeEvents get adminPoliciesStorageRdb failed.");
        return ERR_GET_STORAGE_RDB_FAILED;
    }

    size_t eventsNumber = admin->adminInfo_.managedEvents_.size();
    auto iter = std::remove_if(admin->adminInfo_.managedEvents_.begin(), admin->adminInfo_.managedEvents_.end(),
        [&](ManagedEvent managedEvent) {
            return std::find(events.begin(), events.end(), static_cast<uint32_t>(managedEvent)) != events.end();
        });
    admin->adminInfo_.managedEvents_.erase(iter, admin->adminInfo_.managedEvents_.end());

    if (admin->adminInfo_.managedEvents_.size() < eventsNumber &&
        !adminPoliciesStorageRdb->UpdateManagedEvents(userId, admin->adminInfo_.packageName_,
            admin->adminInfo_.managedEvents_)) {
        return ERR_EDM_UNKNOWN_ADMIN;
    }
    AdminContainer::GetInstance()->UpdateAdmin(userId, admin->adminInfo_.packageName_, MANAGED_EVENTS, *admin);
    return ERR_OK;
}

ErrCode AdminManager::GetPoliciesByVirtualAdmin(const std::string &bundleName, const std::string &parentName,
    std::vector<std::string> &policies)
{
    policies.clear();
    std::shared_ptr<Admin> virtualAdmin = GetAdminByPkgName(bundleName, EdmConstants::DEFAULT_USER_ID);
    if (virtualAdmin == nullptr) {
        return EdmReturnErrCode::PARAM_ERROR;
    }
    if (virtualAdmin->GetAdminType() != AdminType::VIRTUAL_ADMIN || virtualAdmin->GetParentAdminName() != parentName) {
        EDMLOGE("GetPoliciesByVirtualAdmin the administrator does not have permission to get delegated policies.");
        return EdmReturnErrCode::ADMIN_EDM_PERMISSION_DENIED;
    }
    policies = virtualAdmin->adminInfo_.accessiblePolicies_;
    return ERR_OK;
}

void AdminManager::GetVirtualAdminsByPolicy(const std::string &policyName, const std::string &parentName,
    std::vector<std::string> &bundleNames)
{
    bundleNames.clear();
    std::vector<std::shared_ptr<Admin>> admins;
    GetAdminByUserId(EdmConstants::DEFAULT_USER_ID, admins);
    for (auto adminItem : admins) {
        if (adminItem->GetAdminType() != AdminType::VIRTUAL_ADMIN || adminItem->GetParentAdminName() != parentName) {
            continue;
        }
        auto policies = adminItem->adminInfo_.accessiblePolicies_;
        if (std::find(policies.begin(), policies.end(), policyName) != policies.end()) {
            bundleNames.emplace_back(adminItem->adminInfo_.packageName_);
        }
    }
}

bool AdminManager::HasPermissionToHandlePolicy(std::shared_ptr<Admin> admin, const std::string &policyName)
{
    if (admin->GetAdminType() != AdminType::VIRTUAL_ADMIN) {
        return true;
    }
    auto policies = admin->adminInfo_.accessiblePolicies_;
    bool hasAllowAll = std::find(policies.begin(), policies.end(), POLICY_ALLOW_ALL) != policies.end();
    return hasAllowAll || std::find(policies.begin(), policies.end(), policyName) != policies.end();
}

std::shared_ptr<Admin> AdminManager::GetSuperAdmin()
{
    std::vector<std::shared_ptr<Admin>> userAdmin;
    if (AdminContainer::GetInstance()->GetAdminCopyByUserId(EdmConstants::DEFAULT_USER_ID, userAdmin)) {
        auto item = std::find_if(userAdmin.begin(), userAdmin.end(),
            [&](const std::shared_ptr<Admin>& admin) { return admin->GetAdminType() == AdminType::ENT; });
        if (item != userAdmin.end()) {
            return *item;
        }
    }
    return nullptr;
}

// init
void AdminManager::Init()
{
    auto adminPoliciesStorageRdb = AdminPoliciesStorageRdb::GetInstance();
    if (adminPoliciesStorageRdb != nullptr) {
        AdminContainer::GetInstance()->InitAdmins(adminPoliciesStorageRdb->QueryAllAdmin());
    } else {
        EDMLOGE("AdminManager::Init failed.");
    }
}

void AdminManager::Dump()
{
    AdminContainer::GetInstance()->Dump();
}

void AdminManager::ClearAdmins()
{
    AdminContainer::GetInstance()->ClearAdmins();
}

void AdminManager::InsertAdmins(int32_t userId, std::vector<std::shared_ptr<Admin>> admins)
{
    AdminContainer::GetInstance()->InsertAdmins(userId, admins);
}
} // namespace EDM
} // namespace OHOS
