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

#include "device_settings_proxy.h"
#include "edm_sys_manager_mock.h"
#include "enterprise_device_mgr_stub_mock.h"
#include "utils.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
const std::string ADMIN_PACKAGENAME = "com.edm.test.demo";
class DeviceSettingsProxyTest : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;

    static void TearDownTestSuite(void);
    std::shared_ptr<DeviceSettingsProxy> deviceSettingsProxy = nullptr;
    std::shared_ptr<EdmSysManager> edmSysManager_ = nullptr;
    sptr<EnterpriseDeviceMgrStubMock> object_ = nullptr;
};

void DeviceSettingsProxyTest::SetUp()
{
    deviceSettingsProxy = DeviceSettingsProxy::GetDeviceSettingsProxy();
    edmSysManager_ = std::make_shared<EdmSysManager>();
    object_ = new (std::nothrow) EnterpriseDeviceMgrStubMock();
    edmSysManager_->RegisterSystemAbilityOfRemoteObject(ENTERPRISE_DEVICE_MANAGER_SA_ID, object_);
    Utils::SetEdmServiceEnable();
}

void DeviceSettingsProxyTest::TearDown()
{
    deviceSettingsProxy.reset();
    edmSysManager_->UnregisterSystemAbilityOfRemoteObject(ENTERPRISE_DEVICE_MANAGER_SA_ID);
    object_ = nullptr;
    Utils::SetEdmServiceDisable();
}

void DeviceSettingsProxyTest::TearDownTestSuite()
{
    ASSERT_FALSE(Utils::GetEdmServiceState());
    std::cout << "EdmServiceState : " << Utils::GetEdmServiceState() << std::endl;
}

/**
 * @tc.name: TestGetScreenOffTimeSuc
 * @tc.desc: Test GetGetScreenOffTime func.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceSettingsProxyTest, TestGetScreenOffTimeSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeIntSendRequestGetPolicy));
    int32_t info = 0;
    int32_t ret = deviceSettingsProxy->GetScreenOffTime(admin, info);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(info == 0);
}

/**
 * @tc.name: TestGetScreenOffTimeFail
 * @tc.desc: Test GetGetScreenOffTime func.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceSettingsProxyTest, TestGetScreenOffTimeFail, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    int32_t info = 0;
    int32_t ret = deviceSettingsProxy->GetScreenOffTime(admin, info);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestInstallUserCertificateSuc
 * @tc.desc: Test InstallUserCertificate func.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceSettingsProxyTest, TestInstallUserCertificateSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestSetPolicy));
    std::vector<uint8_t> certArray;
    std::string alias = "alias";
    std::string stringRet;
    std::string innerCodeMsg;
    int32_t ret = deviceSettingsProxy->InstallUserCertificate(admin, certArray, alias, stringRet, innerCodeMsg);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestInstallUserCertificateFail
 * @tc.desc: Test InstallUserCertificate func.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceSettingsProxyTest, TestInstallUserCertificateFail, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    std::vector<uint8_t> certArray;
    std::string alias = "alias";
    std::string stringRet;
    std::string innerCodeMsg;
    int32_t ret = deviceSettingsProxy->InstallUserCertificate(admin, certArray, alias, stringRet, innerCodeMsg);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestUninstallUserCertificateSuc
 * @tc.desc: Test UninstallUserCertificate func.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceSettingsProxyTest, TestUninstallUserCertificateSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestSetPolicy));
    std::vector<uint8_t> certArray;
    std::string alias = "alias";
    std::string stringRet;
    std::string innerCodeMsg;
    int32_t ret = deviceSettingsProxy->UninstallUserCertificate(admin, alias, innerCodeMsg);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestUninstallUserCertificateFail
 * @tc.desc: Test UninstallUserCertificate func.
 * @tc.type: FUNC
 */
HWTEST_F(DeviceSettingsProxyTest, TestUninstallUserCertificateFail, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    std::vector<uint8_t> certArray;
    std::string alias = "alias";
    std::string stringRet;
    std::string innerCodeMsg;
    int32_t ret = deviceSettingsProxy->UninstallUserCertificate(admin, alias, innerCodeMsg);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS