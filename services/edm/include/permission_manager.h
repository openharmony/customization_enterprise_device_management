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

#ifndef SERVICES_EDM_INCLUDE_EDM_PERMISSION_MANAGER_H
#define SERVICES_EDM_INCLUDE_EDM_PERMISSION_MANAGER_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "singleton.h"

#include "admin_type.h"
#include "iplugin.h"

namespace OHOS {
namespace EDM {
class PermissionManager : public DelayedSingleton<PermissionManager> {
DECLARE_DELAYED_SINGLETON(PermissionManager)
public:
    ErrCode AddPermission(const std::string &permission, IPlugin::PermissionType permissionType, std::uint32_t code);
    void GetAdminGrantedPermission(const std::vector<std::string> &permissions, AdminType adminType,
        std::vector<std::string> &reqPermission);

private:
    AdminType PermissionTypeToAdminType(IPlugin::PermissionType permissionType);
    std::map<std::string, AdminType> permissions_;
    std::map<std::string, std::set<std::uint32_t>> permissionToCodes_;
};
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_INCLUDE_EDM_PERMISSION_MANAGER_H
