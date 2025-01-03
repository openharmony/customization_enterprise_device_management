/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef SERVICES_EDM_INCLUDE_EDM_ENTERPRISE_DEVICE_MGR_ABILITY_H
#define SERVICES_EDM_INCLUDE_EDM_ENTERPRISE_DEVICE_MGR_ABILITY_H

#include <chrono>
#include <condition_variable>
#include <memory>
#include <string>

#include "admin_manager.h"
#include "common_event_subscriber.h"
#include "enterprise_admin_proxy.h"
#include "enterprise_device_mgr_stub.h"
#include "hilog/log.h"
#include "plugin_manager.h"
#include "policy_manager.h"
#include "policy_struct.h"
#include "security_report.h"
#include "system_ability.h"

namespace OHOS {
namespace EDM {
class EnterpriseDeviceMgrAbility : public SystemAbility, public EnterpriseDeviceMgrStub {
    DECLARE_SYSTEM_ABILITY(EnterpriseDeviceMgrAbility);

public:
    EnterpriseDeviceMgrAbility();
    DISALLOW_COPY_AND_MOVE(EnterpriseDeviceMgrAbility);
    ~EnterpriseDeviceMgrAbility() override;
    static sptr<EnterpriseDeviceMgrAbility> GetInstance();

    ErrCode EnableAdmin(AppExecFwk::ElementName &admin, EntInfo &entInfo, AdminType type, int32_t userId) override;
    ErrCode DisableAdmin(AppExecFwk::ElementName &admin, int32_t userId) override;
    ErrCode DisableSuperAdmin(const std::string &bundleName) override;
    ErrCode HandleDevicePolicy(uint32_t code, AppExecFwk::ElementName &admin, MessageParcel &data, MessageParcel &reply,
        int32_t userId) override;
    ErrCode GetDevicePolicy(uint32_t code, MessageParcel &data, MessageParcel &reply, int32_t userId) override;
    ErrCode GetEnabledAdmin(AdminType type, std::vector<std::string> &enabledAdminList) override;
    ErrCode GetEnterpriseInfo(AppExecFwk::ElementName &admin, MessageParcel &reply) override;
    ErrCode SetEnterpriseInfo(AppExecFwk::ElementName &admin, EntInfo &entInfo) override;
    ErrCode SubscribeManagedEvent(const AppExecFwk::ElementName &admin, const std::vector<uint32_t> &events) override;
    ErrCode UnsubscribeManagedEvent(const AppExecFwk::ElementName &admin, const std::vector<uint32_t> &events) override;
    ErrCode AuthorizeAdmin(const AppExecFwk::ElementName &admin, const std::string &bundleName) override;
    ErrCode SetDelegatedPolicies(const std::string &parentAdminName, const std::string &bundleName,
        const std::vector<std::string> &policies) override;
    ErrCode GetDelegatedPolicies(const std::string &parentAdminName, const std::string &bundleName,
        std::vector<std::string> &policies) override;
    ErrCode GetDelegatedBundleNames(const std::string &parentAdminName, const std::string &policyName,
        std::vector<std::string> &bundleNames) override;
    bool IsSuperAdmin(const std::string &bundleName) override;
    bool IsAdminEnabled(AppExecFwk::ElementName &admin, int32_t userId) override;
    void ConnectAbilityOnSystemEvent(const std::string &bundleName, ManagedEvent event, int32_t userId = 100);
    std::unordered_map<std::string,
        std::function<void(EnterpriseDeviceMgrAbility *that, const EventFwk::CommonEventData &data)>>
        commonEventFuncMap_;
    std::unordered_map<int32_t,
        std::function<void(EnterpriseDeviceMgrAbility *that, int32_t systemAbilityId, const std::string &deviceId)>>
        addSystemAbilityFuncMap_;
    ErrCode GetSuperAdmin(MessageParcel &reply) override;

protected:
    void OnStart() override;
    void OnStop() override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

private:
    void AddCommonEventFuncMap();
    void AddOnAddSystemAbilityFuncMap();
    bool SubscribeAppState();
    bool UnsubscribeAppState();
    void NotifyAdminEnabled(bool isEnabled);
    ErrCode CheckCallingUid(const std::string &bundleName);
    ErrCode RemoveAdminItem(const std::string &adminName, const std::string &policyName, const std::string &policyValue,
        int32_t userId);
    ErrCode RemoveAdminAndAdminPolicy(const std::string &adminName, int32_t userId);
    ErrCode RemoveAdmin(const std::string &adminName, int32_t userId);
    ErrCode RemoveAdminPolicy(const std::string &adminName, int32_t userId);
    ErrCode RemoveSubSuperAdminAndAdminPolicy(const std::string &bundleName);
    ErrCode RemoveSuperAdminAndAdminPolicy(const std::string &bundleName);
    ErrCode RemoveSubOrSuperAdminAndAdminPolicy(const std::string &bundleName,
        const std::vector<int32_t> &nonDefaultUserIds);
    ErrCode GetAllPermissionsByAdmin(const std::string &bundleInfoName, AdminType adminType, int32_t userId,
        std::vector<std::string> &permissionList);
    int32_t GetCurrentUserId();
    ErrCode HandleApplicationEvent(const std::vector<uint32_t> &events, bool subscribe);
    ErrCode UpdateDeviceAdmin(AppExecFwk::ElementName &admin);
    ErrCode VerifyEnableAdminCondition(AppExecFwk::ElementName &admin, AdminType type, int32_t userId, bool isDebug);
    ErrCode VerifyManagedEvent(const AppExecFwk::ElementName &admin, const std::vector<uint32_t> &events);
    ErrCode UpdateDevicePolicy(uint32_t code, const std::string &bundleName, MessageParcel &data, MessageParcel &reply,
        int32_t userId);
    ErrCode CheckCallerPermission(std::shared_ptr<Admin> admin, const std::string &permission, bool isNeedSuperAdmin);
    ErrCode CheckAndUpdatePermission(std::shared_ptr<Admin> admin, Security::AccessToken::AccessTokenID tokenId,
        const std::string &permission, int32_t userId);
    ErrCode CheckDelegatedPolicies(std::shared_ptr<Admin> admin, const std::vector<std::string> &policies);
    ErrCode CheckSystemCalling(IPlugin::ApiType apiType, const std::string &permissionTag);
    ErrCode CheckHandlePolicyPermission(FuncOperateType operateType, const std::string &bundleName,
        const std::string &policyName, const std::string &permissionName, int32_t userId);
#ifdef COMMON_EVENT_SERVICE_EDM_ENABLE
    std::shared_ptr<EventFwk::CommonEventSubscriber> CreateEnterpriseDeviceEventSubscriber(
        EnterpriseDeviceMgrAbility &listener);
#endif
    void OnCommonEventUserRemoved(const EventFwk::CommonEventData &data);
    void OnCommonEventPackageAdded(const EventFwk::CommonEventData &data);
    void OnCommonEventPackageRemoved(const EventFwk::CommonEventData &data);
    bool ShouldUnsubscribeAppState(const std::string &adminName, int32_t userId);
    bool CheckManagedEvent(uint32_t event);
    void OnAppManagerServiceStart();
    void OnAbilityManagerServiceStart();
    void OnCommonEventServiceStart();
#ifdef PASTEBOARD_EDM_ENABLE
    void OnPasteboardServiceStart();
#endif
#ifdef NET_MANAGER_BASE_EDM_ENABLE
    void OnNetManagerBaseServiceStart();
    void HandleDisallowedNetworkInterface(const std::map<std::string, std::string> policyMap);
#endif
#ifdef USERIAM_EDM_ENABLE
    void OnUserAuthFrameworkStart();
#endif
#ifdef USB_EDM_ENABLE
    void OnUsbServiceStart();
#endif
    void OnRenderSystemStart();
    void OnWindowManagerServiceStart();
    void CreateSecurityContent(const std::string &bundleName, const std::string &abilityName,
        uint32_t code, const std::string &policyName, ErrCode errorCode);
    void InitAllAdmins();
    void InitAllPlugins();
    void InitAllPolices();
    void RemoveAllDebugAdmin();
    void AddSystemAbilityListeners();
    void ConnectAbilityOnSystemUpdate(const UpdateInfo &updateInfo);
    void OnCommonEventSystemUpdate(const EventFwk::CommonEventData &data);
    std::shared_ptr<IEdmBundleManager> GetBundleMgr();
    std::shared_ptr<IEdmAppManager> GetAppMgr();
    std::shared_ptr<IEdmOsAccountManager> GetOsAccountMgr();
    ErrCode DoDisableAdmin(const std::string &bundleName, int32_t userId, AdminType adminType);
    void UnloadPluginTask();
    void SetPasswordPolicy();
    void SetFingerprintPolicy();
    
    static std::mutex mutexLock_;
    static sptr<EnterpriseDeviceMgrAbility> instance_;
    std::shared_ptr<PolicyManager> policyMgr_;
    std::shared_ptr<AdminManager> adminMgr_;
    std::shared_ptr<PluginManager> pluginMgr_;
    std::unordered_set<std::string> allowDelegatedPolicies_;
    bool registerToService_ = false;
    std::shared_ptr<EventFwk::CommonEventSubscriber> commonEventSubscriber = nullptr;
    sptr<AppExecFwk::IApplicationStateObserver> appStateObserver_;
    bool pluginHasInit_ = false;
    bool notifySignal_ = false;
    std::chrono::system_clock::time_point lastCallTime_;
    std::condition_variable waitSignal_;
    std::mutex waitMutex_;
};
#ifdef COMMON_EVENT_SERVICE_EDM_ENABLE
class EnterpriseDeviceEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    EnterpriseDeviceEventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscribeInfo,
        EnterpriseDeviceMgrAbility &listener);
    ~EnterpriseDeviceEventSubscriber() override = default;

    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

private:
    EnterpriseDeviceMgrAbility &listener_;
};
#endif
} // namespace EDM
} // namespace OHOS
#endif // SERVICES_EDM_INCLUDE_EDM_ENTERPRISE_DEVICE_MGR_ABILITY_H
