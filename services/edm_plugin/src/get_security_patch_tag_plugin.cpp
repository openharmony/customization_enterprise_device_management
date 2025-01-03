/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "get_security_patch_tag_plugin.h"

#include "edm_ipc_interface_code.h"
#include "parameter.h"
#include "string_serializer.h"
#include "plugin_manager.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = PluginManager::GetInstance()->AddPlugin(GetSecurityPatchTagPlugin::GetPlugin());

void GetSecurityPatchTagPlugin::InitPlugin(std::shared_ptr<IPluginTemplate<GetSecurityPatchTagPlugin, std::string>> ptr)
{
    EDMLOGI("GetSecurityPatchTagPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::GET_SECURITY_PATCH_TAG, "get_security_patch_tag",
        "ohos.permission.ENTERPRISE_MANAGE_SECURITY", IPlugin::PermissionType::SUPER_DEVICE_ADMIN, false);
    ptr->SetSerializer(StringSerializer::GetInstance());
}

ErrCode GetSecurityPatchTagPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data,
    MessageParcel &reply, int32_t userId)
{
    EDMLOGI("GetSecurityPatchTagPlugin OnGetPolicy.");
    const char* securityPatchTagPtr = GetSecurityPatchTag();
    if (securityPatchTagPtr == nullptr) {
        EDMLOGE("GetSecurityPatchTagPlugin GetSecurityPatchTag Failed. GetSecurityPatchTag is nullptr.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    std::string securityPatchTag = securityPatchTagPtr;
    reply.WriteInt32(ERR_OK);
    reply.WriteString(securityPatchTag);
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS
