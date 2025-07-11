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

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "edm_sys_manager_mock.h"
#include "enterprise_device_mgr_proxy.h"
#include "enterprise_device_mgr_stub_mock.h"
#include "func_code.h"
#include "system_ability_definition.h"
#include "utils.h"

using namespace testing::ext;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
namespace {
constexpr int32_t DEFAULT_USERID = 100;
constexpr int32_t FUNC_CODE_ERR = -222;
}
class EnterpriseDeviceMgrProxyTest : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;

    static void TearDownTestSuite(void);
    std::shared_ptr<EnterpriseDeviceMgrProxy> enterpriseDeviceMgrProxyTest = nullptr;
    std::shared_ptr<EdmSysManager> edmSysManager_ = nullptr;
    sptr<EnterpriseDeviceMgrStubMock> object_ = nullptr;
};

void EnterpriseDeviceMgrProxyTest::SetUp()
{
    enterpriseDeviceMgrProxyTest = EnterpriseDeviceMgrProxy::GetInstance();
    edmSysManager_ = std::make_shared<EdmSysManager>();
    object_ = new (std::nothrow) EnterpriseDeviceMgrStubMock();
    edmSysManager_->RegisterSystemAbilityOfRemoteObject(ENTERPRISE_DEVICE_MANAGER_SA_ID, object_);
    Utils::SetEdmServiceEnable();
}

void EnterpriseDeviceMgrProxyTest::TearDown()
{
    EnterpriseDeviceMgrProxy::DestroyInstance();
    edmSysManager_->UnregisterSystemAbilityOfRemoteObject(ENTERPRISE_DEVICE_MANAGER_SA_ID);
    object_ = nullptr;
    Utils::SetEdmServiceDisable();
}

void EnterpriseDeviceMgrProxyTest::TearDownTestSuite()
{
    ASSERT_FALSE(Utils::GetEdmServiceState());
    std::cout << "EdmServiceState : " << Utils::GetEdmServiceState() << std::endl;
}

/**
 * @tc.name: TestEnableAdminSuc
 * @tc.desc: Test EnableAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestEnableAdminSuc, TestSize.Level0)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EntInfo entInfo("test", "this is test");
    EXPECT_CALL(*object_, EnableAdmin(_, _, _, _))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->EnableAdmin(admin, entInfo, AdminType::NORMAL, DEFAULT_USERID);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestEnableAdminFail
 * @tc.desc: Test EnableAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestEnableAdminFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EntInfo entInfo("test", "this is test");
    EXPECT_CALL(*object_, EnableAdmin(_, _, _, _))
        .Times(1)
        .WillOnce(Return(EdmReturnErrCode::SYSTEM_ABNORMALLY));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->EnableAdmin(admin, entInfo, AdminType::NORMAL, DEFAULT_USERID);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestSetEnterpriseInfoSuc
 * @tc.desc: Test SetEnterpriseInfo func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetEnterpriseInfoSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EntInfo entInfo("test", "this is test");
    EXPECT_CALL(*object_, SetEnterpriseInfo(_, _))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetEnterpriseInfo(admin, entInfo);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestSetEnterpriseInfoFail
 * @tc.desc: Test SetEnterpriseInfo func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetEnterpriseInfoFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EntInfo entInfo("test", "this is test");
    EXPECT_CALL(*object_, SetEnterpriseInfo(_, _))
        .Times(1)
        .WillOnce(Return(EdmReturnErrCode::SYSTEM_ABNORMALLY));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetEnterpriseInfo(admin, entInfo);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestGetEnterpriseInfoEntInfo
 * @tc.desc: Test GetEnterpriseInfo func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetEnterpriseInfoEntInfo, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EXPECT_CALL(*object_, GetEnterpriseInfo(_, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeGetEnterpriseInfo));
    EntInfo entInfo1;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetEnterpriseInfo(admin, entInfo1);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestGetEnterpriseInfoFail
 * @tc.desc: Test GetEnterpriseInfo func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetEnterpriseInfoFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EntInfo entInfo1;
    EXPECT_CALL(*object_, GetEnterpriseInfo(_, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeGetEnterpriseInfoFail));
    enterpriseDeviceMgrProxyTest->GetEnterpriseInfo(admin, entInfo1);
    EXPECT_TRUE(entInfo1.enterpriseName.size() == 0);
    EXPECT_TRUE(entInfo1.description.size() == 0);
}

/**
 * @tc.name: TestIsAdminEnabledFail
 * @tc.desc: Test IsAdminEnabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsAdminEnabledFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EXPECT_CALL(*object_, IsAdminEnabled(_, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeIsAdminEnabledFail));
    bool ret = true;
    enterpriseDeviceMgrProxyTest->IsAdminEnabled(admin, DEFAULT_USERID, ret);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: TestGetEnabledAdminReplyFail
 * @tc.desc: Test GetEnabledAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetEnabledAdminReplyFail, TestSize.Level1)
{
    EXPECT_CALL(*object_, GetEnabledAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    std::vector<std::string> enabledAdminList1;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetEnabledAdmin(AdminType::NORMAL, enabledAdminList1);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestGetEnabledAdminFail
 * @tc.desc: Test GetEnabledAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetEnabledAdminFail, TestSize.Level1)
{
    std::vector<std::string> enabledAdminList1;
    EXPECT_CALL(*object_, GetEnabledAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetEnabledAdmin(AdminType::NORMAL, enabledAdminList1);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestIsPolicyDisabledSuc
 * @tc.desc: Test IsPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsPolicyDisabledSuc, TestSize.Level1)
{
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequest));
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    bool isDisabled = false;
    enterpriseDeviceMgrProxyTest->IsPolicyDisabled(nullptr, funcCode, isDisabled);
    EXPECT_TRUE(!isDisabled);
}

/**
 * @tc.name: TestIsPolicyDisabledFail
 * @tc.desc: Test IsPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsPolicyDisabledFail, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    bool isDisabled = false;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestFail));
    enterpriseDeviceMgrProxyTest->IsPolicyDisabled(nullptr, funcCode, isDisabled);
    EXPECT_TRUE(!isDisabled);
}

/**
 * @tc.name: TestIsPolicyDisabledFuncCodeFail
 * @tc.desc: Test IsPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsPolicyDisabledFuncCodeFail, TestSize.Level1)
{
    bool isDisabled = false;
    enterpriseDeviceMgrProxyTest->IsPolicyDisabled(nullptr, FUNC_CODE_ERR, isDisabled);
    EXPECT_TRUE(!isDisabled);
}

/**
 * @tc.name: TestIsPolicyDisabledWithTagSuc
 * @tc.desc: Test IsPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsPolicyDisabledWithTagSuc, TestSize.Level1)
{
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequest));
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    bool isDisabled = false;
    enterpriseDeviceMgrProxyTest->IsPolicyDisabled(nullptr, funcCode, isDisabled, "version_11");
    EXPECT_TRUE(!isDisabled);
}

/**
 * @tc.name: TestIsPolicyDisabledWithTagFail
 * @tc.desc: Test IsPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsPolicyDisabledWithTagFail, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    bool isDisabled = false;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestFail));
    enterpriseDeviceMgrProxyTest->IsPolicyDisabled(nullptr, funcCode, isDisabled, "version_11");
    EXPECT_TRUE(!isDisabled);
}

/**
 * @tc.name: TestIsPolicyDisabledWithTagFuncCodeFail
 * @tc.desc: Test IsPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsPolicyDisabledWithTagFuncCodeFail, TestSize.Level1)
{
    bool isDisabled = false;
    enterpriseDeviceMgrProxyTest->IsPolicyDisabled(nullptr, FUNC_CODE_ERR, isDisabled, "version_11");
    EXPECT_TRUE(!isDisabled);
}

/**
 * @tc.name: TestSetPolicyDisabledSuc
 * @tc.desc: Test SetPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetPolicyDisabledSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequest));
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::DISABLED_PRINTER);
    bool isDisabled = true;
    int32_t ret = enterpriseDeviceMgrProxyTest->SetPolicyDisabled(admin, funcCode, isDisabled);
    EXPECT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestSetPolicyDisabledFail
 * @tc.desc: Test SetPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetPolicyDisabledFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::DISABLED_PRINTER);
    bool isDisabled = true;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestFail));
    int32_t ret = enterpriseDeviceMgrProxyTest->SetPolicyDisabled(admin, funcCode, isDisabled);
    EXPECT_TRUE(ret != ERR_OK);
}

/**
 * @tc.name: TestSetPolicyDisabledFuncCodeFail
 * @tc.desc: Test SetPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetPolicyDisabledFuncCodeFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    bool isDisabled = true;
    int32_t ret = enterpriseDeviceMgrProxyTest->SetPolicyDisabled(admin, FUNC_CODE_ERR, isDisabled);
    EXPECT_TRUE(ret != ERR_OK);
}

/**
 * @tc.name: TestSetPolicyDisabledWithTagSuc
 * @tc.desc: Test SetPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetPolicyDisabledWithTagSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequest));
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::DISABLED_PRINTER);
    bool isDisabled = true;
    int32_t ret = enterpriseDeviceMgrProxyTest->SetPolicyDisabled(admin, funcCode, isDisabled, "version_11");
    EXPECT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestSetPolicyDisabledWithTagFail
 * @tc.desc: Test SetPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetPolicyDisabledWithTagFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::DISABLED_PRINTER);
    bool isDisabled = true;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestFail));
    int32_t ret = enterpriseDeviceMgrProxyTest->SetPolicyDisabled(admin, funcCode, isDisabled, "version_11");
    EXPECT_TRUE(ret != ERR_OK);
}

/**
 * @tc.name: TestSetPolicyDisabledWithTagFuncCodeFail
 * @tc.desc: Test SetPolicyDisabled func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetPolicyDisabledWithTagFuncCodeFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    bool isDisabled = true;
    int32_t ret = enterpriseDeviceMgrProxyTest->SetPolicyDisabled(admin, FUNC_CODE_ERR, isDisabled, "version_11");
    EXPECT_TRUE(ret != ERR_OK);
}

/**
 * @tc.name: TestGetPolicyValueSuc
 * @tc.desc: Test GetPolicyValue func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetPolicyValueSuc, TestSize.Level1)
{
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequest));
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    std::string policyData;
    MessageParcel data;
    data.WriteInt32(HAS_USERID);
    data.WriteInt32(DEFAULT_USER_ID);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteInt32(WITHOUT_ADMIN);
    bool ret = enterpriseDeviceMgrProxyTest->GetPolicyValue(data, funcCode, policyData);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: TestGetPolicyValueReplyFail
 * @tc.desc: Test GetPolicyValue func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetPolicyValueReplyFail, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    std::string policyData;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestReplyFail));
    MessageParcel data;
    data.WriteInt32(HAS_USERID);
    data.WriteInt32(DEFAULT_USER_ID);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteInt32(WITHOUT_ADMIN);
    bool ret = enterpriseDeviceMgrProxyTest->GetPolicyValue(data, funcCode, policyData);
    EXPECT_TRUE(!ret);
}

/**
 * @tc.name: TestHandleDevicePolicyFuncCodeFail
 * @tc.desc: Test HandleDevicePolicy func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestHandleDevicePolicyFuncCodeFail, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestFail));
    MessageParcel data;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->HandleDevicePolicy(funcCode, data);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestDisableAdminSuc
 * @tc.desc: Test DisableAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestDisableAdminSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EXPECT_CALL(*object_, DisableAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->DisableAdmin(admin, DEFAULT_USERID);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestDisableAdminFail
 * @tc.desc: Test DisableAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestDisableAdminFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EXPECT_CALL(*object_, DisableAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->DisableAdmin(admin, DEFAULT_USERID);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestGetPolicyArrayEnableAdmin
 * @tc.desc: Test GetPolicyArray func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetPolicyArrayEnableAdmin, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestEnableAdmin));
    std::vector<std::string> policyArrayData;
    enterpriseDeviceMgrProxyTest->GetPolicyArray(nullptr, funcCode, policyArrayData);
    EXPECT_TRUE(policyArrayData.size() == 1);
}

/**
 * @tc.name: TestGetPolicyArrayFail
 * @tc.desc: Test GetPolicyArray func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetPolicyArrayFail, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    std::vector<std::string> policyArrayData;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestFail));
    bool ret = enterpriseDeviceMgrProxyTest->GetPolicyArray(nullptr, funcCode, policyArrayData);
    EXPECT_TRUE(!ret);
}

/**
 * @tc.name: TestGetPolicyArraySuc
 * @tc.desc: Test GetPolicyArray func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetPolicyArraySuc, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    std::vector<std::string> policyArrayData;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequest));
    bool ret = enterpriseDeviceMgrProxyTest->GetPolicyArray(nullptr, funcCode, policyArrayData);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: TestGetPolicyMapSuc
 * @tc.desc: Test GetPolicyMap func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetPolicyMapSuc, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    std::map<std::string, std::string> policyMapData;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequest));
    bool ret = enterpriseDeviceMgrProxyTest->GetPolicyMap(nullptr, funcCode, policyMapData);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: TestGetPolicyMapReplyFail
 * @tc.desc: Test GetPolicyMap func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetPolicyMapReplyFail, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    std::map<std::string, std::string> policyMapData;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestReplyFail));
    bool ret = enterpriseDeviceMgrProxyTest->GetPolicyMap(nullptr, funcCode, policyMapData);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: TestGetPolicyMapEnableAdminNotEqual
 * @tc.desc: Test GetPolicyMap func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetPolicyMapEnableAdminNotEqual, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    std::map<std::string, std::string> policyMapData;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestEnableAdmin));
    bool ret = enterpriseDeviceMgrProxyTest->GetPolicyMap(nullptr, funcCode, policyMapData);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: TestGetPolicyMapEnableAdminSizeEqual
 * @tc.desc: Test GetPolicyMap func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetPolicyMapEnableAdminSizeEqual, TestSize.Level1)
{
    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    std::map<std::string, std::string> policyMapData;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestMapEnableAdminTwoSuc));
    bool ret = enterpriseDeviceMgrProxyTest->GetPolicyMap(nullptr, funcCode, policyMapData);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: TestIsSuperAdminReturnFail
 * @tc.desc: Test IsSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsSuperAdminReturnFail, TestSize.Level1)
{
    EXPECT_CALL(*object_, IsSuperAdmin(_, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeIsSuperAdminFail));
    bool ret = true;
    std::string bundleName = "com.edm.test.demo";
    enterpriseDeviceMgrProxyTest->IsSuperAdmin(bundleName, ret);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: TestIsByodAdminReturnFail
 * @tc.desc: Test IsByodAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsByodAdminReturnFail, TestSize.Level1)
{
    EXPECT_CALL(*object_, IsByodAdmin(_, _))
    .Times(1)
    .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeIsByodAdminFail));
    bool ret = true;
    AppExecFwk::ElementName admin;
    enterpriseDeviceMgrProxyTest->IsByodAdmin(admin, ret);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: TestGetEnabledSuperAdminReturnFail
 * @tc.desc: Test GetEnabledSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetEnabledSuperAdminReturnFail, TestSize.Level1)
{
    EXPECT_CALL(*object_, GetEnabledAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    std::string enabledAdmin;
    enterpriseDeviceMgrProxyTest->GetEnabledSuperAdmin(enabledAdmin);
    EXPECT_TRUE(enabledAdmin.size() == 0);
}

/**
 * @tc.name: TestGetEnabledSuperAdminReturnReplyFail
 * @tc.desc: Test GetEnabledSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetEnabledSuperAdminReturnReplyFail, TestSize.Level1)
{
    std::string enabledAdmin;
    EXPECT_CALL(*object_, GetEnabledAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    enterpriseDeviceMgrProxyTest->GetEnabledSuperAdmin(enabledAdmin);
    EXPECT_TRUE(enabledAdmin.size() == 0);
}

/**
 * @tc.name: TestGetEnabledSuperAdminReturnEnableAdmin
 * @tc.desc: Test GetEnabledSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetEnabledSuperAdminReturnEnableAdmin, TestSize.Level1)
{
    std::string enabledAdmin;
    EXPECT_CALL(*object_, GetEnabledAdmin(_, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeGetEnabledAdmin));
    enterpriseDeviceMgrProxyTest->GetEnabledSuperAdmin(enabledAdmin);
    GTEST_LOG_(INFO) << "mock enabledAdmin enabledAdmin item :" << enabledAdmin;
    EXPECT_TRUE(enabledAdmin.size() > 0);
}

/**
 * @tc.name: TestDisableSuperAdminReturnSuc
 * @tc.desc: Test DisableSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestDisableSuperAdminReturnSuc, TestSize.Level1)
{
    EXPECT_CALL(*object_, DisableSuperAdmin(_))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->DisableSuperAdmin("com.edm.test.demo");
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestDisableSuperAdminReturnFail
 * @tc.desc: Test DisableSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestDisableSuperAdminReturnFail, TestSize.Level1)
{
    EXPECT_CALL(*object_, DisableSuperAdmin(_))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->DisableSuperAdmin("com.edm.test.demo");
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestHandleManagedEventReturnFail
 * @tc.desc: Test HandleManagedEvent func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestHandleManagedEventReturnFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    const std::vector<uint32_t> events;
    EXPECT_CALL(*object_, UnsubscribeManagedEvent(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->HandleManagedEvent(admin, events, false);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestHandleManagedEventSuc
 * @tc.desc: Test HandleManagedEvent func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestHandleManagedEventSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    const std::vector<uint32_t> events;
    EXPECT_CALL(*object_, SubscribeManagedEvent(_, _))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->HandleManagedEvent(admin, events, true);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestAuthorizeAdminEdmDisable
 * @tc.desc: Test AuthorizeAdmin without enable edm service func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestAuthorizeAdminEdmDisable, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    OHOS::AppExecFwk::ElementName admin;
    std::string bundleName = "com.edm.test.demo";
    ErrCode ret = enterpriseDeviceMgrProxyTest->AuthorizeAdmin(admin, bundleName);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestAuthorizeAdminIpcFail
 * @tc.desc: Test AuthorizeAdmin func with ipc failed.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestAuthorizeAdminIpcFail, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    EXPECT_CALL(*object_, AuthorizeAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_INVALID_VALUE));
    std::string bundleName = "com.edm.test.demo";
    ErrCode errVal = enterpriseDeviceMgrProxyTest->AuthorizeAdmin(admin, bundleName);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestAuthorizeAdminReplyFail
 * @tc.desc: Test AuthorizeAdmin func with reply failed.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestAuthorizeAdminReplyFail, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    EXPECT_CALL(*object_, AuthorizeAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    std::string bundleName = "com.edm.test.demo";
    ErrCode errVal = enterpriseDeviceMgrProxyTest->AuthorizeAdmin(admin, bundleName);
    EXPECT_TRUE(errVal == ERR_PROXY_SENDREQUEST_FAIL);
}

/**
 * @tc.name: TestAuthorizeAdminSuccess
 * @tc.desc: Test AuthorizeAdmin func success.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestAuthorizeAdminSuccess, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    EXPECT_CALL(*object_, AuthorizeAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    std::string bundleName = "com.edm.test.demo";
    ErrCode errVal = enterpriseDeviceMgrProxyTest->AuthorizeAdmin(admin, bundleName);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestDisableAdmin
 * @tc.desc: Test DisableAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestDisableAdmin, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EntInfo entInfo("test", "this is test");

    EXPECT_CALL(*object_, EnableAdmin(_, _, _, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->EnableAdmin(admin, entInfo, AdminType::NORMAL, DEFAULT_USERID);
    EXPECT_TRUE(errVal != ERR_OK);

    EXPECT_CALL(*object_, SetEnterpriseInfo(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    errVal = enterpriseDeviceMgrProxyTest->SetEnterpriseInfo(admin, entInfo);
    EXPECT_TRUE(errVal != ERR_OK);

    EntInfo entInfo1;
    enterpriseDeviceMgrProxyTest->GetEnterpriseInfo(admin, entInfo1);
    EXPECT_TRUE(entInfo1.enterpriseName.size() == 0);
    EXPECT_TRUE(entInfo1.description.size() == 0);

    bool ret = false;
    enterpriseDeviceMgrProxyTest->IsAdminEnabled(admin, DEFAULT_USERID, ret);
    EXPECT_FALSE(ret);

    std::vector<std::string> enabledAdminList;
    enterpriseDeviceMgrProxyTest->GetEnabledAdmins(enabledAdminList);
    EXPECT_TRUE(enabledAdminList.empty());

    std::vector<std::string> enabledAdminList1;
    enterpriseDeviceMgrProxyTest->GetEnabledAdmin(AdminType::NORMAL, enabledAdminList1);
    EXPECT_TRUE(enabledAdminList1.empty());

    int funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::SET_DATETIME);
    bool isDisabled = false;
    enterpriseDeviceMgrProxyTest->IsPolicyDisabled(nullptr, funcCode, isDisabled);
    EXPECT_FALSE(isDisabled);

    EXPECT_CALL(*object_, DisableAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    errVal = enterpriseDeviceMgrProxyTest->DisableAdmin(admin, DEFAULT_USERID);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestDisableSuperAdmin
 * @tc.desc: Test DisableSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestDisableSuperAdmin, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EntInfo entInfo("test", "this is test");

    EXPECT_CALL(*object_, EnableAdmin(_, _, _, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->EnableAdmin(admin, entInfo, AdminType::ENT, DEFAULT_USERID);
    EXPECT_TRUE(errVal != ERR_OK);

    bool ret = false;
    std::string bundleName = "com.edm.test.demo";
    enterpriseDeviceMgrProxyTest->IsSuperAdmin(bundleName, ret);
    EXPECT_FALSE(ret);

    std::string enabledAdmin;
    enterpriseDeviceMgrProxyTest->GetEnabledSuperAdmin(enabledAdmin);
    EXPECT_TRUE(enabledAdmin.size() == 0);

    ret = enterpriseDeviceMgrProxyTest->IsSuperAdminExist();
    EXPECT_FALSE(ret);

    EXPECT_CALL(*object_, DisableSuperAdmin(_))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    errVal = enterpriseDeviceMgrProxyTest->DisableSuperAdmin("com.edm.test.demo");
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestGetSuperAdminSuccess
 * @tc.desc: Test GetSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetSuperAdminSuccess, TestSize.Level1)
{
    std::string bundleName;
    std::string abilityName;
    EXPECT_CALL(*object_, GetSuperAdmin(_, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeGetSuperAdmin));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetSuperAdmin(bundleName, abilityName);
    EXPECT_TRUE(errVal == ERR_OK);
    EXPECT_TRUE(bundleName == RETURN_STRING);
    EXPECT_TRUE(abilityName == RETURN_STRING);
}

/**
 * @tc.name: TestGetSuperAdminFail
 * @tc.desc: Test GetSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetSuperAdminFail, TestSize.Level1)
{
    std::string bundleName;
    std::string abilityName;
    EXPECT_CALL(*object_, GetSuperAdmin(_, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetSuperAdmin(bundleName, abilityName);
    EXPECT_TRUE(errVal != ERR_OK);
    EXPECT_TRUE(bundleName.empty());
    EXPECT_TRUE(abilityName.empty());
}

/**
 * @tc.name: TestSetDelegatedPoliciesWithEdmDisable
 * @tc.desc: Test SetDelegatedPolicies without enable edm service func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetDelegatedPoliciesWithEdmDisable, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    OHOS::AppExecFwk::ElementName admin;
    std::string bundleName = "com.edm.test.demo";
    std::vector<std::string> policies;
    ErrCode ret = enterpriseDeviceMgrProxyTest->SetDelegatedPolicies(admin, bundleName, policies);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestSetDelegatedPoliciesIpcFail
 * @tc.desc: Test SetDelegatedPolicies func with ipc failed.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetDelegatedPoliciesIpcFail, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    std::string bundleName = "com.edm.test.demo";
    std::vector<std::string> policies;
    EXPECT_CALL(*object_, SetDelegatedPolicies(testing::An<const AppExecFwk::ElementName &>(),
        testing::An<const std::string &>(), testing::An<const std::vector<std::string> &>()))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetDelegatedPolicies(admin, bundleName, policies);
    EXPECT_TRUE(errVal == ERR_PROXY_SENDREQUEST_FAIL);
}

/**
 * @tc.name: TestSetDelegatedPoliciesSuccess
 * @tc.desc: Test SetDelegatedPolicies func success.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetDelegatedPoliciesSuccess, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    std::string bundleName = "com.edm.test.demo";
    std::vector<std::string> policies;
    EXPECT_CALL(*object_, SetDelegatedPolicies(testing::An<const AppExecFwk::ElementName &>(),
        testing::An<const std::string &>(), testing::An<const std::vector<std::string> &>()))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetDelegatedPolicies(admin, bundleName, policies);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestGetDelegatedPoliciesWithEdmDisable
 * @tc.desc: Test GetDelegatedPolicies without enable edm service func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetDelegatedPoliciesWithEdmDisable, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    OHOS::AppExecFwk::ElementName admin;
    std::string bundleName = "com.edm.test.demo";
    std::vector<std::string> result;
    ErrCode ret = enterpriseDeviceMgrProxyTest->GetDelegatedPolicies(admin, bundleName,
        EdmInterfaceCode::GET_DELEGATED_POLICIES, result);
    ASSERT_TRUE(ret == EdmReturnErrCode::ADMIN_INACTIVE);
}

/**
 * @tc.name: TestGetDelegatedPoliciesIpcFail
 * @tc.desc: Test GetDelegatedPolicies func with ipc failed.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetDelegatedPoliciesIpcFail, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    std::string bundleName = "com.edm.test.demo";
    EXPECT_CALL(*object_, GetDelegatedPolicies(_, _, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    std::vector<std::string> result;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetDelegatedPolicies(admin, bundleName,
        EdmInterfaceCode::GET_DELEGATED_POLICIES, result);
    EXPECT_TRUE(errVal == ERR_PROXY_SENDREQUEST_FAIL);
}

/**
 * @tc.name: TestGetDelegatedPoliciesSuccess
 * @tc.desc: Test GetDelegatedPolicies func success.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetDelegatedPoliciesSuccess, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    std::string bundleName = "com.edm.test.demo";
    EXPECT_CALL(*object_, GetDelegatedPolicies(_, _, _))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    std::vector<std::string> result;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetDelegatedPolicies(admin, bundleName,
        EdmInterfaceCode::GET_DELEGATED_POLICIES, result);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestReplaceSuperAdminSuc
 * @tc.desc: Test ReplaceSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, ReplaceSuperAdminSuc, TestSize.Level1)
{
    AppExecFwk::ElementName oldAdmin;
    AppExecFwk::ElementName newAdmin;
    bool isKeepPolicy = false;
    EXPECT_CALL(*object_, ReplaceSuperAdmin(_, _, _))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->ReplaceSuperAdmin(oldAdmin, newAdmin, isKeepPolicy);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestReplaceSuperAdminFail
 * @tc.desc: Test ReplaceSuperAdmin func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestReplaceSuperAdminFail, TestSize.Level1)
{
    AppExecFwk::ElementName oldAdmin;
    AppExecFwk::ElementName newAdmin;
    bool isKeepPolicy = false;
    EXPECT_CALL(*object_, ReplaceSuperAdmin(_, _, _))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->ReplaceSuperAdmin(oldAdmin, newAdmin, isKeepPolicy);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestGetAdminsSucc
 * @tc.desc: Test GetAdmins func succ.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetAdminsSucc, TestSize.Level1)
{
    EXPECT_CALL(*object_, GetAdmins(_))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeGetAdmins));
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetAdmins(wants);
    ASSERT_TRUE(errVal == ERR_OK);
    int32_t wantSize = wants.size();
    ASSERT_TRUE(wantSize == 1);
    std::shared_ptr<AAFwk::Want> want = wants[0];
    ASSERT_TRUE(want != nullptr);
    std::string bundleName = want->GetStringParam("bundleName");
    EXPECT_TRUE(bundleName == "com.edm.test.demo");
    std::string abilityName = want->GetStringParam("abilityName");
    EXPECT_TRUE(abilityName == "test.ability");
    int32_t adminType = want->GetIntParam("adminType", -1);
    EXPECT_TRUE(adminType == static_cast<int32_t>(AdminType::BYOD));
}

/**
 * @tc.name: TestGetAdminsIpcFail
 * @tc.desc: Test GetAdmins func with ipc failed.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetAdminsIpcFail, TestSize.Level1)
{
    EXPECT_CALL(*object_, GetAdmins(_))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetAdmins(wants);
    EXPECT_TRUE(errVal == ERR_PROXY_SENDREQUEST_FAIL);
}

/**
 * @tc.name: TestGetAdminsWithEdmDisable
 * @tc.desc: Test GetAdmins without enable edm service func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestGetAdminsWithEdmDisable, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->GetAdmins(wants);
    ASSERT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestCheckAndGetAdminProvisionInfoSucc
 * @tc.desc: Test CheckAndGetAdminProvisionInfo func succ.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestCheckAndGetAdminProvisionInfoSucc, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(),
            &EnterpriseDeviceMgrStubMock::InvokeSendRequestCheckAndGetAdminProvisionInfo));
    std::string bundleName;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->CheckAndGetAdminProvisionInfo(admin, bundleName);
    ASSERT_TRUE(errVal == ERR_OK);
    ASSERT_TRUE(bundleName == "com.edm.test.demo");
}

/**
 * @tc.name: TestCheckAndGetAdminProvisionInfoFail
 * @tc.desc: Test CheckAndGetAdminProvisionInfo func with ipc failed.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestCheckAndGetAdminProvisionInfoIpcFail, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName admin;
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestFail));
    std::string bundleName;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->CheckAndGetAdminProvisionInfo(admin, bundleName);
    EXPECT_TRUE(errVal == EdmReturnErrCode::SYSTEM_ABNORMALLY);
}

/**
 * @tc.name: TestIsEdmEnabledSuc
 * @tc.desc: Test IsEdmEnabled with enable edm service func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsEdmEnabledSuc, TestSize.Level0)
{
    bool errVal = enterpriseDeviceMgrProxyTest->IsEdmEnabled();
    ASSERT_TRUE(errVal);
}

/**
 * @tc.name: TestIsEdmEnabledWithEdmDisable
 * @tc.desc: Test IsEdmEnabled without enable edm service func.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestIsEdmEnabledWithEdmDisable, TestSize.Level1)
{
    Utils::SetEdmServiceDisable();
    bool errVal = enterpriseDeviceMgrProxyTest->IsEdmEnabled();
    ASSERT_FALSE(errVal);
}

/**
 * @tc.name: TestSetDelegatedPolicies
 * @tc.desc: Test SetDelegatedPolicies failed.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetDelegatedPolicies, TestSize.Level1)
{
    std::string bundleName = "com.edm.test.demo";
    std::vector<std::string> policies;
    int32_t userId = 100;
    EXPECT_CALL(*object_, SetDelegatedPolicies(testing::An<const std::string &>(),
        testing::An<const std::vector<std::string> &>(), testing::An<int32_t>()))
        .Times(1)
        .WillOnce(Return(ERR_PROXY_SENDREQUEST_FAIL));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetDelegatedPolicies(bundleName, policies, userId);
    EXPECT_TRUE(errVal == ERR_PROXY_SENDREQUEST_FAIL);
}

/**
 * @tc.name: TestSetDelegatedPoliciesIsSuccess
 * @tc.desc: Test SetDelegatedPolicies func success.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetDelegatedPoliciesIsSuccess, TestSize.Level1)
{
    std::string bundleName = "com.edm.test.demo";
    std::vector<std::string> policies;
    int32_t userId = 100;
    EXPECT_CALL(*object_, SetDelegatedPolicies(testing::An<const std::string &>(),
        testing::An<const std::vector<std::string> &>(), testing::An<int32_t>()))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetDelegatedPolicies(bundleName, policies, userId);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestSetAdminRunningModeFail
 * @tc.desc: Test SetAdminRunningMode fail.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetAdminRunningModeFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EXPECT_CALL(*object_, SetAdminRunningMode(_, _))
        .Times(1)
        .WillOnce(Return(EdmReturnErrCode::SYSTEM_ABNORMALLY));
    uint32_t runningMode = 0;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetAdminRunningMode(admin, runningMode);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestSetAdminRunningModeSuc
 * @tc.desc: Test SetAdminRunningMode success.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetAdminRunningModeSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName("com.edm.test.demo");
    admin.SetAbilityName("com.edm.test.demo.Ability");
    EXPECT_CALL(*object_, SetAdminRunningMode(_, _))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    uint32_t runningMode = 0;
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetAdminRunningMode(admin, runningMode);
    EXPECT_TRUE(errVal == ERR_OK);
}

/**
 * @tc.name: TestSetBundleInstallPoliciesFail
 * @tc.desc: Test SetBundleInstallPolicies fail.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetBundleInstallPoliciesFail, TestSize.Level1)
{
    std::vector<std::string> bundles = { "test" };
    int32_t userId = 100;
    int32_t policyType = 1;
    EXPECT_CALL(*object_, SetBundleInstallPolicies(_, _, _))
        .Times(1)
        .WillOnce(Return(EdmReturnErrCode::SYSTEM_ABNORMALLY));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetBundleInstallPolicies(bundles, userId, policyType);
    EXPECT_TRUE(errVal != ERR_OK);
}

/**
 * @tc.name: TestSetBundleInstallPoliciesSuc
 * @tc.desc: Test SetBundleInstallPolicies success.
 * @tc.type: FUNC
 */
HWTEST_F(EnterpriseDeviceMgrProxyTest, TestSetBundleInstallPoliciesSuc, TestSize.Level1)
{
    std::vector<std::string> bundles = { "test" };
    int32_t userId = 100;
    int32_t policyType = 1;
    EXPECT_CALL(*object_, SetBundleInstallPolicies(_, _, _))
        .Times(1)
        .WillOnce(Return(ERR_OK));
    ErrCode errVal = enterpriseDeviceMgrProxyTest->SetBundleInstallPolicies(bundles, userId, policyType);
    EXPECT_TRUE(errVal == ERR_OK);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS
