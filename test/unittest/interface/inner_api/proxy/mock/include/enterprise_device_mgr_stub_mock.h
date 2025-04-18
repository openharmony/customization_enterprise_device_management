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

#ifndef EDM_UNIT_TEST_ENTERPRISE_DEVICE_MGR_STUB_MOCK_H
#define EDM_UNIT_TEST_ENTERPRISE_DEVICE_MGR_STUB_MOCK_H

#include <gmock/gmock.h>
#include <iremote_stub.h>

#include <map>
#include <vector>

#include "ienterprise_device_mgr.h"
#include "string_ex.h"

namespace OHOS {
namespace EDM {
constexpr int ERR_PROXY_SENDREQUEST_FAIL = 111;
const std::string RETURN_STRING = "test_string";
const std::string ELEMENT_STRING = "com.example.myapplication/MainAbility";
const std::string UPGRADE_VERSION = "version_1.0";
constexpr int32_t UPGRADE_FAILED_CODE = -1;
const std::string UPGRADE_FAILED_MESSAGE = "upgrade failed";
const std::string AUTH_DATA = "auth data";
class EnterpriseDeviceMgrStubMock : public IRemoteStub<IEnterpriseDeviceMgr> {
public:
    EnterpriseDeviceMgrStubMock() = default;

    virtual ~EnterpriseDeviceMgrStubMock() = default;

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequest code :" << code;
        code_ = code;
        reply.WriteInt32(ERR_OK);
        return 0;
    }

    int InvokeSendRequestGetEnterpriseInfo(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestEnableAdmin(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int InvokeSendRequestMapEnableAdminTwoSuc(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestSetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int InvokeSendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int InvokeBoolSendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int InvokeBoolSendRequestGetFirewallRule(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestGetDomainFilterRules(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeIntSendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int InvokeSendRequestGetPolicyForWriteFileToStream(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestSetPolicyInstallFail(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeArrayStringSendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestGetErrPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int InvokeSendRequestGetPolicyExceedsMax(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeArrayElementSendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeHttpProxySendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestParamError(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int InvokeBluetoothProxySendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeAccountProxySendRequestAddOsAccount(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeAllowedUsbDevicesSendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeDisallowedUsbDevicesSendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestGetSuperAdmin(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int InvokeSendRequestGetOTAUpdatePolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestGetUpgradeResult(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestGetUpdateAuthData(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option);

    int InvokeSendRequestGetPasswordPolicy(uint32_t code, MessageParcel &data,
        MessageParcel &reply, MessageOption &option);

    int InvokeSendRequestGetManagedBrowserPolicy(uint32_t code, MessageParcel &data,
        MessageParcel &reply, MessageOption &option);

    int InvokeSendRequestGetAdmins(uint32_t code, MessageParcel &data,
        MessageParcel &reply, MessageOption &option);

    int InvokeSendRequestCheckAndGetAdminProvisionInfo(uint32_t code, MessageParcel &data,
        MessageParcel &reply, MessageOption &option);

    int InvokeSendRequestReplyFail(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestReplyFail code :" << code;
        code_ = code;
        reply.WriteInt32(ERR_PROXY_SENDREQUEST_FAIL);
        return 0;
    }

    int InvokeSendRequestFail(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestFail code :" << code;
        code_ = code;
        return ERR_PROXY_SENDREQUEST_FAIL;
    }

    ErrCode EnableAdmin(AppExecFwk::ElementName &admin, EntInfo &entInfo, AdminType type, int32_t userId) override
    {
        return 0;
    }

    ErrCode DisableAdmin(AppExecFwk::ElementName &admin, int32_t userId) override { return 0; }

    ErrCode DisableSuperAdmin(const std::string &bundleName) override { return 0; }

    ErrCode HandleDevicePolicy(uint32_t code, AppExecFwk::ElementName &admin, MessageParcel &data, MessageParcel &reply,
        int32_t userId) override
    {
        return 0;
    }

    ErrCode GetDevicePolicy(uint32_t code, MessageParcel &data, MessageParcel &reply, int32_t userId) override
    {
        return 0;
    }

    ErrCode CheckAndGetAdminProvisionInfo(uint32_t code, MessageParcel &data, MessageParcel &reply,
        int32_t userId) override
    {
        return 0;
    }

    ErrCode GetAdmins(std::vector<std::shared_ptr<AAFwk::Want>> &wants) override
    {
        return 0;
    }

    ErrCode GetEnabledAdmin(AdminType type, std::vector<std::string> &enabledAdminList) override { return 0; }

    ErrCode GetEnterpriseInfo(AppExecFwk::ElementName &admin, MessageParcel &reply) override { return 0; }

    ErrCode SetEnterpriseInfo(AppExecFwk::ElementName &admin, EntInfo &entInfo) override { return 0; }

    ErrCode SubscribeManagedEvent(const AppExecFwk::ElementName &admin, const std::vector<uint32_t> &events) override
    {
        return 0;
    }

    ErrCode UnsubscribeManagedEvent(const AppExecFwk::ElementName &admin, const std::vector<uint32_t> &events) override
    {
        return 0;
    }

    bool IsSuperAdmin(const std::string &bundleName) override { return false; }

    bool IsAdminEnabled(AppExecFwk::ElementName &admin, int32_t userId) override { return false; }

    ErrCode AuthorizeAdmin(const AppExecFwk::ElementName &admin, const std::string &bundleName) override
    {
        return ERR_OK;
    }

    ErrCode SetDelegatedPolicies(const std::string &parentAdminName, const std::string &bundleName,
        const std::vector<std::string> &policies) override
    {
        return ERR_OK;
    }

    ErrCode GetDelegatedPolicies(const std::string &parentAdminName, const std::string &bundleName,
        std::vector<std::string> &policies) override
    {
        return ERR_OK;
    }

    ErrCode GetDelegatedBundleNames(const std::string &parentAdminName, const std::string &policyName,
        std::vector<std::string> &bundleNames) override
    {
        return ERR_OK;
    }

    ErrCode GetSuperAdmin(MessageParcel &reply) override
    {
        return ERR_OK;
    }

    ErrCode ReplaceSuperAdmin(AppExecFwk::ElementName &oldAdmin, AppExecFwk::ElementName &newAdmin,
        bool isKeepPolicy) override
    {
        return ERR_OK;
    }

    ErrCode SetAdminRunningMode(AppExecFwk::ElementName &admin, uint32_t runningMode)
    {
        return ERR_OK;
    }

    uint32_t code_ = 0;
};
} // namespace EDM
} // namespace OHOS
#endif // EDM_UNIT_TEST_ENTERPRISE_DEVICE_MGR_STUB_MOCK_H