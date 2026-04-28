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

#define private public
#define protected public
#include "allowed_collaboration_service_bundles_plugin.h"
#include "policy_manager.h"
#undef protected
#undef private

#include "array_string_serializer.h"
#include "edm_constants.h"
#include "edm_errors.h"
#include "edm_ipc_interface_code.h"
#include "iplugin_manager.h"
#include "ipolicy_manager.h"
#include "plugin_singleton.h"
#include "utils.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {

const std::string TEST_ADMIN_NAME = "com.edm.test.demo";
const std::string TEST_POLICY_VALUE_TRUE = "true";
const std::string TEST_POLICY_VALUE_FALSE = "false";
const std::vector<std::string> TEST_APP_IDENTIFIERS = {"com.test.app1", "com.test.app2"};

class AllowedCollaborationServiceBundlesPluginTest : public testing::Test {
protected:
    static void SetUpTestSuite(void);

    static void TearDownTestSuite(void);
};

void AllowedCollaborationServiceBundlesPluginTest::SetUpTestSuite(void)
{
    Utils::SetEdmInitialEnv();
}

void AllowedCollaborationServiceBundlesPluginTest::TearDownTestSuite(void)
{
    Utils::ResetTokenTypeAndUid();
    ASSERT_TRUE(Utils::IsOriginalUTEnv());
    std::cout << "now ut process is orignal ut env : " << Utils::IsOriginalUTEnv() << std::endl;
}

/**
 * @tc.name: InitPlugin_Normal_Success
 * @tc.desc: Test AllowedCollaborationServiceBundlesPlugin::InitPlugin function.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, InitPlugin_Normal_Success, TestSize.Level1)
{
    AllowedCollaborationServiceBundlesPlugin plugin;
    auto pluginTemplate = std::make_shared<IPluginTemplate<AllowedCollaborationServiceBundlesPlugin,
        std::vector<std::string>>>();
    plugin.InitPlugin(pluginTemplate);
    EXPECT_EQ(pluginTemplate->GetCode(), EdmInterfaceCode::ALLOWED_COLLABORATION_SERVICE_BUNDLES);
    EXPECT_EQ(pluginTemplate->GetPolicyName(), PolicyName::POLICY_ALLOWED_COLLABORATION_SERVICE_BUNDLES);
    EXPECT_EQ(plugin.maxListSize_, EdmConstants::APPID_MAX_SIZE);
    EXPECT_EQ(plugin.overMaxReturnErrCode_, EdmReturnErrCode::PARAMETER_VERIFICATION_FAILED);
}

/**
 * @tc.name: SetOtherModulePolicy_PrerequisiteSatisfied_Success
 * @tc.desc: Test SetOtherModulePolicy function when disallowed_distributed_transmission is true.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, SetOtherModulePolicy_PrerequisiteSatisfied_Success,
    TestSize.Level1)
{
    std::shared_ptr<PolicyManager> policyManager = std::make_shared<PolicyManager>();
    IPolicyManager::policyManagerInstance_ = policyManager.get();
    ErrCode res = policyManager->SetPolicy(TEST_ADMIN_NAME,
        PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, TEST_POLICY_VALUE_TRUE, TEST_POLICY_VALUE_TRUE,
        DEFAULT_USER_ID);
    ASSERT_EQ(res, ERR_OK);

    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    std::vector<std::string> policyData = TEST_APP_IDENTIFIERS;
    std::vector<std::string> failedData;
    ErrCode ret = plugin.SetOtherModulePolicy(policyData, DEFAULT_USER_ID, failedData);
    EXPECT_EQ(ret, ERR_OK);

    ErrCode clearRes = policyManager->SetPolicy(TEST_ADMIN_NAME,
        PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, "", "", DEFAULT_USER_ID);
    ASSERT_EQ(clearRes, ERR_OK);
    IPolicyManager::policyManagerInstance_ = nullptr;
}

/**
 * @tc.name: SetOtherModulePolicy_PrerequisiteNotSatisfied_Fail
 * @tc.desc: Test SetOtherModulePolicy function when disallowed_distributed_transmission is not true.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, SetOtherModulePolicy_PrerequisiteNotSatisfied_Fail,
    TestSize.Level1)
{
    std::shared_ptr<PolicyManager> policyManager = std::make_shared<PolicyManager>();
    IPolicyManager::policyManagerInstance_ = policyManager.get();
    ErrCode res = policyManager->SetPolicy(TEST_ADMIN_NAME,
        PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, TEST_POLICY_VALUE_FALSE, TEST_POLICY_VALUE_FALSE,
        DEFAULT_USER_ID);
    ASSERT_EQ(res, ERR_OK);

    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    std::vector<std::string> policyData = TEST_APP_IDENTIFIERS;
    std::vector<std::string> failedData;
    ErrCode ret = plugin.SetOtherModulePolicy(policyData, DEFAULT_USER_ID, failedData);
    EXPECT_EQ(ret, EdmReturnErrCode::PREREQUISITES_NOT_SATISFIED_FAILED);

    ErrCode clearRes = policyManager->SetPolicy(TEST_ADMIN_NAME,
        PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, "", "", DEFAULT_USER_ID);
    ASSERT_EQ(clearRes, ERR_OK);
    IPolicyManager::policyManagerInstance_ = nullptr;
}

/**
 * @tc.name: SetOtherModulePolicy_PrerequisiteEmpty_Fail
 * @tc.desc: Test SetOtherModulePolicy function when disallowed_distributed_transmission policy is empty.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, SetOtherModulePolicy_PrerequisiteEmpty_Fail, TestSize.Level1)
{
    std::shared_ptr<PolicyManager> policyManager = std::make_shared<PolicyManager>();
    IPolicyManager::policyManagerInstance_ = policyManager.get();

    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    std::vector<std::string> policyData = TEST_APP_IDENTIFIERS;
    std::vector<std::string> failedData;
    ErrCode ret = plugin.SetOtherModulePolicy(policyData, DEFAULT_USER_ID, failedData);
    EXPECT_EQ(ret, EdmReturnErrCode::PREREQUISITES_NOT_SATISFIED_FAILED);

    IPolicyManager::policyManagerInstance_ = nullptr;
}

/**
 * @tc.name: OnBasicSetPolicy_EmptyList_Success
 * @tc.desc: Test OnBasicSetPolicy function with empty policy data.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, OnBasicSetPolicy_EmptyList_Success, TestSize.Level1)
{
    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    std::vector<std::string> policyData;
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicSetPolicy(policyData, currentData, mergeData, DEFAULT_USER_ID);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: OnBasicSetPolicy_NormalList_Without_Prerequisites
 * @tc.desc: Test OnBasicSetPolicy function with normal policy data.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, OnBasicSetPolicy_NormalList_Without_Prerequisites,
    TestSize.Level1)
{
    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    std::vector<std::string> policyData = TEST_APP_IDENTIFIERS;
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicSetPolicy(policyData, currentData, mergeData, DEFAULT_USER_ID);
    EXPECT_EQ(ret, EdmReturnErrCode::PREREQUISITES_NOT_SATISFIED_FAILED);
}

/**
 * @tc.name: OnBasicSetPolicy_ExceedMaxSize_Fail
 * @tc.desc: Test OnBasicSetPolicy function when policy data size exceeds maximum limit.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, OnBasicSetPolicy_ExceedMaxSize_Fail, TestSize.Level1)
{
    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    plugin.overMaxReturnErrCode_ = EdmReturnErrCode::PARAMETER_VERIFICATION_FAILED;
    std::vector<std::string> policyData(EdmConstants::APPID_MAX_SIZE + 1);
    for (size_t i = 0; i < EdmConstants::APPID_MAX_SIZE + 1; ++i) {
        std::stringstream ss;
        ss << "com.test.app" << i;
        policyData[i] = ss.str();
    }
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicSetPolicy(policyData, currentData, mergeData, DEFAULT_USER_ID);
    EXPECT_EQ(ret, EdmReturnErrCode::PARAMETER_VERIFICATION_FAILED);
}

/**
 * @tc.name: OnBasicRemovePolicy_EmptyList_Success
 * @tc.desc: Test OnBasicRemovePolicy function with empty policy data.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, OnBasicRemovePolicy_EmptyList_Success, TestSize.Level1)
{
    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    std::vector<std::string> policyData;
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicRemovePolicy(policyData, currentData, mergeData, DEFAULT_USER_ID);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: OnBasicRemovePolicy_NormalList_Success
 * @tc.desc: Test OnBasicRemovePolicy function with normal policy data.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, OnBasicRemovePolicy_NormalList_Success, TestSize.Level1)
{
    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    std::vector<std::string> policyData = TEST_APP_IDENTIFIERS;
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicRemovePolicy(policyData, currentData, mergeData, DEFAULT_USER_ID);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: OnBasicAdminRemove_Normal_Success
 * @tc.desc: Test OnBasicAdminRemove function with normal admin removal.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, OnBasicAdminRemove_Normal_Success, TestSize.Level1)
{
    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    std::string adminName = TEST_ADMIN_NAME;
    std::vector<std::string> data = TEST_APP_IDENTIFIERS;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicAdminRemove(adminName, data, mergeData, DEFAULT_USER_ID);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: OnBasicAdminRemove_EmptyData_Success
 * @tc.desc: Test OnBasicAdminRemove function with empty data.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, OnBasicAdminRemove_EmptyData_Success, TestSize.Level1)
{
    AllowedCollaborationServiceBundlesPlugin plugin;
    plugin.maxListSize_ = EdmConstants::APPID_MAX_SIZE;
    std::string adminName = TEST_ADMIN_NAME;
    std::vector<std::string> data;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicAdminRemove(adminName, data, mergeData, DEFAULT_USER_ID);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: GetPlugin_Instance_NotNull
 * @tc.desc: Test GetPlugin function returns non-null instance.
 * @tc.type: FUNC
 */
HWTEST_F(AllowedCollaborationServiceBundlesPluginTest, GetPlugin_Instance_NotNull, TestSize.Level1)
{
    std::shared_ptr<IPlugin> plugin = AllowedCollaborationServiceBundlesPlugin::GetPlugin();
    ASSERT_NE(plugin, nullptr);
    EXPECT_EQ(plugin->GetCode(), EdmInterfaceCode::ALLOWED_COLLABORATION_SERVICE_BUNDLES);
}

} // namespace TEST
} // namespace EDM
} // namespace OHOS