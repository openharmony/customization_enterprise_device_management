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

#include <gtest/gtest.h>
#include <string>
#include <system_ability_definition.h>
#include <vector>

#include "edm_sys_manager_mock.h"
#include "enterprise_device_mgr_stub_mock.h"
#include "security_manager_proxy.h"
#include "utils.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
const std::string ADMIN_PACKAGENAME = "com.edm.test.demo";
class SecurityManagerProxyTest : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;

    static void TearDownTestSuite(void);
    std::shared_ptr<SecurityManagerProxy> proxy_ = nullptr;
    std::shared_ptr<EdmSysManager> edmSysManager_ = nullptr;
    sptr<EnterpriseDeviceMgrStubMock> object_ = nullptr;
};

void SecurityManagerProxyTest::SetUp()
{
    proxy_ = SecurityManagerProxy::GetSecurityManagerProxy();
    edmSysManager_ = std::make_shared<EdmSysManager>();
    object_ = new (std::nothrow) EnterpriseDeviceMgrStubMock();
    edmSysManager_->RegisterSystemAbilityOfRemoteObject(ENTERPRISE_DEVICE_MANAGER_SA_ID, object_);
    Utils::SetEdmServiceEnable();
}

void SecurityManagerProxyTest::TearDown()
{
    proxy_.reset();
    edmSysManager_->UnregisterSystemAbilityOfRemoteObject(ENTERPRISE_DEVICE_MANAGER_SA_ID);
    object_ = nullptr;
    Utils::SetEdmServiceDisable();
}

void SecurityManagerProxyTest::TearDownTestSuite()
{
    ASSERT_FALSE(Utils::GetEdmServiceState());
    std::cout << "EdmServiceState : " << Utils::GetEdmServiceState() << std::endl;
}

/**
 * @tc.name: TestGetSecurityPatchTagSuc
 * @tc.desc: Test GetSecurityPatchTag success func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetSecurityPatchTagSuc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPolicy));
    std::string res;
    int32_t ret = proxy_->GetSecurityPatchTag(admin, res);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(res == RETURN_STRING);
}

/**
 * @tc.name: TestGetSecurityPatchTagFail
 * @tc.desc: Test GetSecurityPatchTag without enable edm service func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetSecurityPatchTagFail, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    std::string res;
    int32_t ret = proxy_->GetSecurityPatchTag(admin, res);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestGetDeviceEncryptionStatusSuc
 * @tc.desc: Test GetDeviceEncryptionStatus success func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetDeviceEncryptionStatusSuc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeBoolSendRequestGetPolicy));
    DeviceEncryptionStatus deviceEncryptionStatus;
    int32_t ret = proxy_->GetDeviceEncryptionStatus(admin, deviceEncryptionStatus);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(deviceEncryptionStatus.isEncrypted == true);
}

/**
 * @tc.name: TestGetDeviceEncryptionStatusFail
 * @tc.desc: Test GetDeviceEncryptionStatus without enable edm service func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetDeviceEncryptionStatusFail, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    DeviceEncryptionStatus deviceEncryptionStatus;
    int32_t ret = proxy_->GetDeviceEncryptionStatus(admin, deviceEncryptionStatus);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestGetRootCheckStatusSuc
 * @tc.desc: Test TestGetRootCheckStatusSuc success func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetRootCheckStatusSuc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPolicy));
    std::string res;
    int32_t ret = proxy_->GetRootCheckStatus(admin, res);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(res == RETURN_STRING);
}

/**
 * @tc.name: TesGetRootCheckStatusFail
 * @tc.desc: Test GetRootCheckStatus without enable edm service func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetRootCheckStatusFail, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    std::string res;
    int32_t ret = proxy_->GetRootCheckStatus(admin, res);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestSetPasswordPolicySuc
 * @tc.desc: Test SetPasswordPolicy success func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestSetPasswordPolicySuc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    PasswordPolicy policy;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestSetPolicy));
    int32_t ret = proxy_->SetPasswordPolicy(admin, policy);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestGetPasswordPolicySuc
 * @tc.desc: Test GetPasswordPolicy success func without admin.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetPasswordPolicySucWithoutAdmin, TestSize.Level1)
{
    PasswordPolicy policy;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPolicy));
    int32_t ret = proxy_->GetPasswordPolicy(policy);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestGetPasswordPolicySuc
 * @tc.desc: Test GetPasswordPolicy success func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetPasswordPolicySuc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    PasswordPolicy policy;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPasswordPolicy));
    int32_t ret = proxy_->GetPasswordPolicy(admin, policy);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(policy.complexityReg == "test_complexityReg");
    ASSERT_TRUE(policy.validityPeriod == 1);
    ASSERT_TRUE(policy.additionalDescription == "test_additionalDescription");
}

/**
 * @tc.name: TestGetPasswordPolicyFail
 * @tc.desc: Test GetPasswordPolicy fail func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetPasswordPolicyFail, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    PasswordPolicy policy;
    int32_t ret = proxy_->GetPasswordPolicy(admin, policy);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestSetAppClipboardPolicySuc
 * @tc.desc: Test SetAppClipboardPolicy success func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestSetAppClipboardPolicySuc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestSetPolicy));
    int32_t ret = proxy_->SetAppClipboardPolicy(admin, 123, 1);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestGetAppClipboardPolicySuc
 * @tc.desc: Test GetAppClipboardPolicy success func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetAppClipboardPolicySuc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    std::string policy;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPolicy));
    int32_t ret = proxy_->GetAppClipboardPolicy(admin, 123, policy);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestGetAppClipboardPolicyFail
 * @tc.desc: Test GetAppClipboardPolicy fail func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestGetAppClipboardPolicyFail, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    std::string policy;
    int32_t ret = proxy_->GetAppClipboardPolicy(admin, 123, policy);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestSetWatermarkImageSuc
 * @tc.desc: Test SetWatermarkImage success func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestSetWatermarkImageSuc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestSetPolicy));
    std::string bundleName = "testBundleName";
    Media::InitializationOptions opt;
    opt.size.width = 100;
    opt.size.height = 100;
    auto pixelMap = Media::PixelMap::Create(opt);
    uint8_t* data = (uint8_t*)malloc(200);
    pixelMap->WritePixels(data, 200);
    int32_t ret = proxy_->SetWatermarkImage(admin, bundleName,
        std::shared_ptr<Media::PixelMap>(std::move(pixelMap)), 100);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestCancelWatermarkImageSec
 * @tc.desc: Test CancelWatermarkImage success func.
 * @tc.type: FUNC
 */
HWTEST_F(SecurityManagerProxyTest, TestCancelWatermarkImageSuc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestSetPolicy));
    std::string bundleName = "testBundleName";
    int32_t ret = proxy_->CancelWatermarkImage(admin, bundleName, 100);
    ASSERT_TRUE(ret == ERR_OK);
}

} // namespace TEST
} // namespace EDM
} // namespace OHOS
