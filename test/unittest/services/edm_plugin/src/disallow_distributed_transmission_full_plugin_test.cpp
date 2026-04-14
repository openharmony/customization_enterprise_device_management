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
#include "disallow_distributed_transmission_full_plugin.h"
#undef protected
#undef private

#include "edm_constants.h"
#include "edm_data_ability_utils_mock.h"
#include "edm_ipc_interface_code.h"
#include "iplugin_manager.h"
#include "ipolicy_manager.h"
#include "utils.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
const std::string TEST_BUNDLE_NAME = "testBundleName";
class DisallowDistributedTransmissionFullPluginTest : public testing::Test {
protected:
    static void SetUpTestSuite(void);

    static void TearDownTestSuite(void);
};

void DisallowDistributedTransmissionFullPluginTest::SetUpTestSuite(void)
{
    Utils::SetEdmInitialEnv();
}

void DisallowDistributedTransmissionFullPluginTest::TearDownTestSuite(void)
{
    Utils::ResetTokenTypeAndUid();
    ASSERT_TRUE(Utils::IsOriginalUTEnv());
    std::cout << "now ut process is orignal ut env : " << Utils::IsOriginalUTEnv() << std::endl;
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullOnSetPolicyTrue
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::OnSetPolicy function with true value.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullOnSetPolicyTrue, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    bool data = true;
    bool currentData;
    bool mergeData;
    ErrCode ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(currentData == true);
    ASSERT_TRUE(mergeData == true);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullOnSetPolicyFalse
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::OnSetPolicy function with false value.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullOnSetPolicyFalse, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    bool data = false;
    bool currentData;
    bool mergeData;
    ErrCode ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(currentData == false);
    ASSERT_TRUE(mergeData == false);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullOnSetPolicyWithMerge
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::OnSetPolicy function with mergeData true.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullOnSetPolicyWithMerge, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    bool data = true;
    bool currentData = false;
    bool mergeData = true;
    ErrCode ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(currentData == true);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullOnSetPolicyConflict
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::OnSetPolicy function when conflict exists.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullOnSetPolicyConflict, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    // Mock the policy manager to return a non-empty value for disallow_distributed_transmission
    auto policyManager = IPolicyManager::GetInstance();
    std::string policyValue = "true";
    policyManager->SetPolicy("", PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, policyValue, DEFAULT_USER_ID);

    bool data = true;
    bool currentData;
    bool mergeData;
    ErrCode ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == EdmReturnErrCode::CONFIGURATION_CONFLICT_FAILED);

    // Clean up
    policyValue = "";
    policyManager->SetPolicy("", PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, policyValue, DEFAULT_USER_ID);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullOnAdminRemoveWithMerge
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::OnAdminRemove function with mergeData true.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullOnAdminRemoveWithMerge, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    bool data = true;
    bool mergeData = true;
    ErrCode ret = plugin.OnAdminRemove(TEST_BUNDLE_NAME, data, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullOnAdminRemoveWithDataTrue
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::OnAdminRemove function with data true.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullOnAdminRemoveWithDataTrue, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    bool data = true;
    bool mergeData = false;
    ErrCode ret = plugin.OnAdminRemove(TEST_BUNDLE_NAME, data, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullOnAdminRemoveWithDataFalse
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::OnAdminRemove function with data false.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullOnAdminRemoveWithDataFalse(false);
    bool mergeData = false;
    ErrCode ret = plugin.OnAdminRemove(TEST_BUNDLE_NAME, data, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullHasConflictPolicyWithConflict
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::HasConflictPolicy when conflict exists.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullHasConflictPolicyWithConflict, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    // Mock the policy manager to return a non-empty value for disallow_distributed_transmission
    auto policyManager = IPolicyManager::GetInstance();
    std::string policyValue = "true";
    policyManager->SetPolicy("", PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, policyValue, DEFAULT_USER_ID);

    bool hasConflict = plugin.HasConflictPolicy(DEFAULT_USER_ID);
    ASSERT_TRUE(hasConflict == true);

    // Clean up
    policyValue = "";
    policyManager->SetPolicy("", PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, policyValue, DEFAULT_USER_ID);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullHasConflictPolicyWithoutConflict
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::HasConflictPolicy when no conflict exists.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullHasConflictPolicyWithoutConflict, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    // Ensure the policy manager returns empty value for disallow_distributed_transmission
    auto policyManager = IPolicyManager::GetInstance();
    std::string policyValue = "";
    policyManager->SetPolicy("", PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION, policyValue, DEFAULT_USER_ID);

    bool hasConflict = plugin.HasConflictPolicy(DEFAULT_USER_ID);
    ASSERT_TRUE(hasConflict == false);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullSetDistributedTransmissionFullPolicyTrue
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::SetDistributedTransmissionFullPolicy with true.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullSetDistributedTransmissionFullPolicyTrue, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    ErrCode ret = plugin.SetDistributedTransmissionFullPolicy(true, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionFullSetDistributedTransmissionFullPolicyFalse
 * @tc.desc: Test DisallowDistributedTransmissionFullPlugin::SetDistributedTransmissionFullPolicy with false.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionFullPluginTest, TestDisallowDistributedTransmissionFullSetDistributedTransmissionFullPolicyFalse, TestSize.Level1)
{
    DisallowDistributedTransmissionFullPlugin plugin;
    ErrCode ret = plugin.SetDistributedTransmissionFullPolicy(false, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS
