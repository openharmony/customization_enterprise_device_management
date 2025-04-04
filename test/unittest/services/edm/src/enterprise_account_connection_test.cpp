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

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "enterprise_admin_stub_mock.h"
#include "enterprise_account_connection.h"
#include "func_code.h"
#include "managed_event.h"
using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
constexpr uint32_t COMMAND_ON_ADMIN_CODE = 3;
constexpr int32_t DEFAULT_USER_ID = 100;
constexpr int32_t TEST_USER_ID = 101;
class EnterpriseAccountConnectionTest : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override {}

    std::shared_ptr<EnterpriseAccountConnection> enterpriseAccountConnectionTest {nullptr};
    sptr<EnterpriseAdminStubMock> remoteObject {nullptr};
};

void EnterpriseAccountConnectionTest::SetUp()
{
    remoteObject = new (std::nothrow) EnterpriseAdminStubMock();
}

/**
 * @tc.name: TestOnAbilityConnectDone01
 * @tc.desc: Test EnterpriseAccountConnection::OnAbilityConnectDone func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseAccountConnectionTest, TestOnAbilityConnectDone01, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = 0;
    enterpriseAccountConnectionTest =
        std::make_shared<EnterpriseAccountConnection>(connectWant,
        static_cast<uint32_t>(ManagedEvent::USER_ADDED), DEFAULT_USER_ID, TEST_USER_ID);
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(remoteObject.GetRefPtr(), &EnterpriseAdminStubMock::InvokeSendRequest));
    enterpriseAccountConnectionTest->OnAbilityConnectDone(admin, remoteObject, resultCode);
    enterpriseAccountConnectionTest->OnAbilityDisconnectDone(admin, resultCode);
    EXPECT_TRUE(remoteObject->code_ == IEnterpriseAdmin::COMMAND_ON_ACCOUNT_ADDED);
}

/**
 * @tc.name: TestOnAbilityConnectDone02
 * @tc.desc: Test EnterpriseAccountConnection::OnAbilityConnectDone func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseAccountConnectionTest, TestOnAbilityConnectDone02, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = 0;
    enterpriseAccountConnectionTest =
        std::make_shared<EnterpriseAccountConnection>(connectWant,
        static_cast<uint32_t>(ManagedEvent::USER_SWITCHED), DEFAULT_USER_ID, TEST_USER_ID);
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(remoteObject.GetRefPtr(), &EnterpriseAdminStubMock::InvokeSendRequest));
    enterpriseAccountConnectionTest->OnAbilityConnectDone(admin, remoteObject, resultCode);
    enterpriseAccountConnectionTest->OnAbilityDisconnectDone(admin, resultCode);
    EXPECT_TRUE(remoteObject->code_ == IEnterpriseAdmin::COMMAND_ON_ACCOUNT_SWITCHED);
}

/**
 * @tc.name: TestOnAbilityConnectDone03
 * @tc.desc: Test EnterpriseAccountConnection::OnAbilityConnectDone func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseAccountConnectionTest, TestOnAbilityConnectDone03, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = 0;
    enterpriseAccountConnectionTest =
        std::make_shared<EnterpriseAccountConnection>(connectWant,
        static_cast<uint32_t>(ManagedEvent::USER_REMOVED), DEFAULT_USER_ID, TEST_USER_ID);
    EXPECT_CALL(*remoteObject, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(remoteObject.GetRefPtr(), &EnterpriseAdminStubMock::InvokeSendRequest));
    enterpriseAccountConnectionTest->OnAbilityConnectDone(admin, remoteObject, resultCode);
    enterpriseAccountConnectionTest->OnAbilityDisconnectDone(admin, resultCode);
    EXPECT_TRUE(remoteObject->code_ == IEnterpriseAdmin::COMMAND_ON_ACCOUNT_REMOVED);
}

/**
 * @tc.name: TestOnAbilityConnectDone04
 * @tc.desc: Test EnterpriseAccountConnection::OnAbilityConnectDone func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseAccountConnectionTest, TestOnAbilityConnectDone04, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    AAFwk::Want connectWant;
    connectWant.SetElementName("com.edm.test.demo", "com.edm.test.demo.Ability");
    int32_t resultCode = 0;
    enterpriseAccountConnectionTest = std::make_shared<EnterpriseAccountConnection>(connectWant,
        COMMAND_ON_ADMIN_CODE, DEFAULT_USER_ID, TEST_USER_ID);
    enterpriseAccountConnectionTest->OnAbilityConnectDone(admin, remoteObject, resultCode);
    enterpriseAccountConnectionTest->OnAbilityDisconnectDone(admin, resultCode);
    EXPECT_TRUE(remoteObject->code_ != IEnterpriseAdmin::COMMAND_ON_BUNDLE_REMOVED);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS