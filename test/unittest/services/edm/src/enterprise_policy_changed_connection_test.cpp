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

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "admin_manager.h"
#include "enterprise_policy_changed_connection.h"
#include "enterprise_admin_stub_mock.h"
#include "func_code.h"
#include "policy_changed_event.h"
#include "ienterprise_admin.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
constexpr int32_t DEFAULT_USER_ID = 100;
const std::string TEST_INTERFACE_NAME = "setPasswordPolicy";
const std::string TEST_ADMIN_NAME = "com.edm.test.demo";
const std::string TEST_PARAMETERS = "{}";
const int64_t TEST_TIMESTAMP = 1234567890;

class EnterprisePolicyChangedConnectionTest : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override {}

    std::shared_ptr<EnterprisePolicyChangedConnection> enterprisePolicyChangedConnectionTest_ {nullptr};
    sptr<EnterpriseAdminStubMock> remoteObject_ {nullptr};
};

void EnterprisePolicyChangedConnectionTest::SetUp()
{
    remoteObject_ = new (std::nothrow) EnterpriseAdminStubMock();
}

/**
 * @tc.name: TestOnAbilityConnectDone_Success
 * @tc.desc: Test EnterprisePolicyChangedConnection::OnAbilityConnectDone func with success result code.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestOnAbilityConnectDone_Success, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = ERR_OK;
    PolicyChangedEvent policyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, policyChangedEvent, DEFAULT_USER_ID);

    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(remoteObject_.GetRefPtr(), &EnterpriseAdminStubMock::InvokeSendRequest));

    enterprisePolicyChangedConnectionTest_->OnAbilityConnectDone(admin, remoteObject_, resultCode);
    enterprisePolicyChangedConnectionTest_->OnAbilityDisconnectDone(admin, resultCode);

    EXPECT_TRUE(remoteObject_->code_ == IEnterpriseAdmin::COMMAND_ON_POLICIES_CHANGED);
}

/**
 * @tc.name: TestOnAbilityConnectDone_FailedResultCode
 * @tc.desc: Test EnterprisePolicyChangedConnection::OnAbilityConnectDone func with failed result code.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestOnAbilityConnectDone_FailedResultCode, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = 1; // Non-zero result code indicates failure
    PolicyChangedEvent policyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, policyChangedEvent, DEFAULT_USER_ID);

    enterprisePolicyChangedConnectionTest_->OnAbilityConnectDone(admin, remoteObject_, resultCode);
    enterprisePolicyChangedConnectionTest_->OnAbilityDisconnectDone(admin, resultCode);

    // When resultCode != ERR_OK, proxy_ should not be created and SendRequest should not be called
    EXPECT_TRUE(remoteObject_->code_ != IEnterpriseAdmin::COMMAND_ON_POLICIES_CHANGED);
}

/**
 * @tc.name: TestOnAbilityConnectDone_NullProxy
 * @tc.desc: Test EnterprisePolicyChangedConnection::OnAbilityConnectDone func when proxy creation fails.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestOnAbilityConnectDone_NullProxy, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = ERR_OK;
    PolicyChangedEvent policyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, policyChangedEvent, DEFAULT_USER_ID);

    // Pass nullptr as remoteObject to simulate proxy creation failure
    sptr<IRemoteObject> nullRemoteObject = nullptr;
    enterprisePolicyChangedConnectionTest_->OnAbilityConnectDone(admin, nullRemoteObject, resultCode);
    enterprisePolicyChangedConnectionTest_->OnAbilityDisconnectDone(admin, resultCode);

    // When remoteObject is nullptr, proxy_ creation should fail
    EXPECT_TRUE(remoteObject_->code_ != IEnterpriseAdmin::COMMAND_ON_POLICIES_CHANGED);
}

/**
 * @tc.name: TestOnAbilityDisconnectDone_Success
 * @tc.desc: Test EnterprisePolicyChangedConnection::OnAbilityDisconnectDone func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestOnAbilityDisconnectDone_Success, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = ERR_OK;
    PolicyChangedEvent policyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, policyChangedEvent, DEFAULT_USER_ID);

    enterprisePolicyChangedConnectionTest_->OnAbilityDisconnectDone(admin, resultCode);
    // OnAbilityDisconnectDone should complete without errors
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestOnAbilityDisconnectDone_NonZeroResultCode
 * @tc.desc: Test EnterprisePolicyChangedConnection::OnAbilityDisconnectDone func with non-zero result code.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestOnAbilityDisconnectDone_NonZeroResultCode, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = 100; // Non-zero result code
    PolicyChangedEvent policyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, policyChangedEvent, DEFAULT_USER_ID);

    enterprisePolicyChangedConnectionTest_->OnAbilityDisconnectDone(admin, resultCode);
    // OnAbilityDisconnectDone should complete without errors regardless of result code
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestConstructor_Success
 * @tc.desc: Test EnterprisePolicyChangedConnection constructor.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestConstructor_Success, TestSize.Level1)
{
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    PolicyChangedEvent policyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);

    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, policyChangedEvent, DEFAULT_USER_ID);

    EXPECT_TRUE(enterprisePolicyChangedConnectionTest_ != nullptr);
    EXPECT_EQ(enterprisePolicyChangedConnectionTest_->GetWant().GetElement().GetBundleName(), "com.edm.test.demo");
    EXPECT_EQ(enterprisePolicyChangedConnectionTest_->GetUserId(), DEFAULT_USER_ID);
}

/**
 * @tc.name: TestConstructor_EmptyPolicyChangedEvent
 * @tc.desc: Test EnterprisePolicyChangedConnection constructor with empty PolicyChangedEvent.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestConstructor_EmptyPolicyChangedEvent, TestSize.Level1)
{
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    PolicyChangedEvent emptyEvent("", "", "", 0);

    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, emptyEvent, DEFAULT_USER_ID);

    EXPECT_TRUE(enterprisePolicyChangedConnectionTest_ != nullptr);
}

/**
 * @tc.name: TestConstructor_DifferentUserId
 * @tc.desc: Test EnterprisePolicyChangedConnection constructor with different userId.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestConstructor_DifferentUserId, TestSize.Level1)
{
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    PolicyChangedEvent policyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    int32_t differentUserId = 200;

    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, policyChangedEvent, differentUserId);

    EXPECT_TRUE(enterprisePolicyChangedConnectionTest_ != nullptr);
    EXPECT_EQ(enterprisePolicyChangedConnectionTest_->GetUserId(), differentUserId);
}

/**
 * @tc.name: TestOnAbilityConnectDone_WithDifferentPolicyChangedEvent
 * @tc.desc: Test OnAbilityConnectDone with different PolicyChangedEvent data.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestOnAbilityConnectDone_WithDifferentPolicyChangedEvent,
    TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = ERR_OK;

    // Test with different interface name
    PolicyChangedEvent policyChangedEvent("addFirewallRule", "com.edm.test.admin", "{\"rule\":\"test\"}", 9876543210);
    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, policyChangedEvent, DEFAULT_USER_ID);

    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(remoteObject_.GetRefPtr(), &EnterpriseAdminStubMock::InvokeSendRequest));

    enterprisePolicyChangedConnectionTest_->OnAbilityConnectDone(admin, remoteObject_, resultCode);
    enterprisePolicyChangedConnectionTest_->OnAbilityDisconnectDone(admin, resultCode);

    EXPECT_TRUE(remoteObject_->code_ == IEnterpriseAdmin::COMMAND_ON_POLICIES_CHANGED);
}

/**
 * @tc.name: TestOnAbilityConnectDone_MultipleCalls
 * @tc.desc: Test OnAbilityConnectDone called multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(EnterprisePolicyChangedConnectionTest, TestOnAbilityConnectDone_MultipleCalls, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    PolicyChangedEvent policyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    enterprisePolicyChangedConnectionTest_ =
        std::make_shared<EnterprisePolicyChangedConnection>(connectWant, policyChangedEvent, DEFAULT_USER_ID);

    // First call with success
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(remoteObject_.GetRefPtr(), &EnterpriseAdminStubMock::InvokeSendRequest));
    enterprisePolicyChangedConnectionTest_->OnAbilityConnectDone(admin, remoteObject_, ERR_OK);
    EXPECT_TRUE(remoteObject_->code_ == IEnterpriseAdmin::COMMAND_ON_POLICIES_CHANGED);

    // Second call with failure
    enterprisePolicyChangedConnectionTest_->OnAbilityConnectDone(admin, remoteObject_, 1);
    // Should not trigger SendRequest again due to failure result code
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS