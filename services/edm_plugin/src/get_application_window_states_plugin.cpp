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

#include "get_application_window_states_plugin.h"

#include "edm_bundle_manager_impl.h"
#include "edm_constants.h"
#include "edm_ipc_interface_code.h"
#include "edm_os_account_manager_impl.h"
#include "edm_log.h"
#include "iplugin_manager.h"
#include "session_manager_lite.h"
#include "string_serializer.h"
#include "system_ability_definition.h"
#include "window_state_info.h"

namespace OHOS {
namespace EDM {
// LCOV_EXCL_START
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(GetApplicationWindowStatesPlugin::GetPlugin());

void GetApplicationWindowStatesPlugin::InitPlugin(std::shared_ptr<IPluginTemplate<GetApplicationWindowStatesPlugin,
    std::string>> ptr)
{
    EDMLOGI("GetApplicationWindowStatesPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::GET_APPLICATION_WINDOW_STATES,
        PolicyName::POLICY_GET_APPLICATION_WINDOW_STATES,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_APPLICATION,
        IPlugin::PermissionType::SUPER_DEVICE_ADMIN, false);
    ptr->SetSerializer(StringSerializer::GetInstance());
}

ErrCode GetApplicationWindowStatesPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data,
    MessageParcel &reply, int32_t userId)
{
    EDMLOGI("GetApplicationWindowStatesPlugin OnGetPolicy");
    std::string bundleName = data.ReadString();
    int32_t appIndex = data.ReadInt32();
    int32_t currentUserId = GetCurrentUserId();
    auto bundleMgr = std::make_shared<EdmBundleManagerImpl>();
    if (!bundleMgr->IsBundleInstalled(bundleName, currentUserId, appIndex)) {
        EDMLOGE("GetApplicationWindowStatesPlugin GetApplicationWindowStates failed");
        return EdmReturnErrCode::PARAMETER_VERIFICATION_FAILED;
    }
    auto wmsProxy = OHOS::Rosen::SessionManagerLite::GetInstance().GetSceneSessionManagerLiteProxy();
    if (wmsProxy == nullptr) { //LCOV_EXCL_BR_LINE
        EDMLOGE("GetApplicationWindowStatesPlugin  wmsproxy is nullptr");
        return EdmReturnErrCode::PARAMETER_VERIFICATION_FAILED;
    }
    Rosen::ApplicationInfo appInfo;
    appInfo.bundleName = bundleName;
    appInfo.appIndex = appIndex;
    std::vector<Rosen::AppWindowShowingInfo> windowInfos;
    OHOS::Rosen::WMError ret = wmsProxy->GetAppWindowShowingInfosByBundleName(appInfo, windowInfos);
    if (ret != OHOS::Rosen::WMError::WM_OK) { //LCOV_EXCL_BR_LINE
        EDMLOGE("GetApplicationWindowStatesPlugin GetApplicationWindowStates failed, ret: %{public}d", ret);
        return EdmReturnErrCode::PARAMETER_VERIFICATION_FAILED;
    }
    std::vector<WindowStateInfo> windowStateInfos;
    for (const auto &item : windowInfos) {
        WindowStateInfo windowInfo;
        windowInfo.windowId = item.persistentId;
        uint32_t state = item.sessionState;
        if (state > static_cast<uint32_t>(WindowState::BACKGROUND)) {
            EDMLOGE(" GetApplicationWindowStates failed, invalid window state: %{public}d", state);
            return EdmReturnErrCode::PARAMETER_VERIFICATION_FAILED;
        }
        windowInfo.state = WindowState(state);
        windowInfo.isOnDock = item.isShowOnDock;
        windowInfo.name = item.windowName;
        windowStateInfos.emplace_back(windowInfo);
    }
    reply.WriteInt32(ERR_OK);
    WindowStateInfoHandle::WriteWindowStateInfoVector(reply, windowStateInfos);
    return ERR_OK;
}

int32_t GetApplicationWindowStatesPlugin::GetCurrentUserId()
{
    std::vector<int32_t> ids;
    ErrCode ret = std::make_shared<EdmOsAccountManagerImpl>()->QueryActiveOsAccountIds(ids);
    if (FAILED(ret) || ids.empty()) {
        EDMLOGE("GetApplicationWindowStatesPlugin GetCurrentUserId failed");
        return -1;
    }
    EDMLOGD("GetApplicationWindowStatesPlugin GetCurrentUserId");
    return (ids.at(0));
}
// LCOV_EXCL_STOP
} // namespace EDM
} // namespace OHOS