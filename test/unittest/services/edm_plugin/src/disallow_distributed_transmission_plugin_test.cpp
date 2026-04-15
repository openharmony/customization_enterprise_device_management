/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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
#include "disallow_distributed_transmission_plugin.h"
#include "policy_manager.h"
#undef protected
#undef private

#include "edm_constants.h"
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
const std::string TEST_BUNDLE_NAME = "testBundleName";
const std::string TEST_ADMIN_NAME = "com.edm.test.demo";
const std::string TEST_POLICY_VALUE = "true";
class DisallowDistributedTransmissionPluginTest : public testing::Test {
protected:
    static void SetUpTestSuite(void);

    static void TearDownTestSuite(void);
};

void DisallowDistributedTransmissionPluginTest::SetUpTestSuite(void)
{
    Utils::SetEdmInitialEnv();
}

void DisallowDistributedTransmissionPluginTest::TearDownTestSuite(void)
{
    Utils::ResetTokenTypeAndUid();
    ASSERT_TRUE(Utils::IsOriginalUTEnv());
    std::cout << "now ut process is orignal ut env : " << Utils::IsOriginalUTEnv() << std::endl;
}

/**
 * @tc.name: DisallowDistributedTransmissionOnSetPolicy
 * @tc.desc: Test DisallowDistributedTransmissionPlugin::OnSetPolicy function.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionPluginTest, DisallowDistributedTransmissionOnSetPolicy, TestSize.Level1)
{
    DisallowDistributedTransmissionPlugin plugin;
    bool data = true;
    bool currentData;
    bool mergeData;
    ErrCode ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    data = false;
    ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: DisallowDistributedTransmissionAdminRemove
 * @tc.desc: Test DisallowDistributedTransmissionPlugin::OnAdminRemove function.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionPluginTest, DisallowDistributedTransmissionAdminRemove, TestSize.Level1)
{
    DisallowDistributedTransmissionPlugin plugin;
    bool data = false;
    bool mergeData = true;
    ErrCode ret = plugin.OnAdminRemove(TEST_BUNDLE_NAME, data, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    data = true;
    mergeData = false;
    ret = plugin.OnAdminRemove(TEST_BUNDLE_NAME, data, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestDisallowDistributedTransmissionOnSetPolicyConflict
 * @tc.desc: Test DisallowDistributedTransmissionPlugin::OnSetPolicy function when conflict exists.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionPluginTest, TestDisallowDistributedTransmissionOnSetPolicyConflict,
    TestSize.Level1)
{
    std::shared_ptr<PolicyManager> policyManager = std::make_shared<PolicyManager>();
    IPolicyManager::policyManagerInstance_ = policyManager.get();
    ErrCode res = policyManager->SetPolicy(TEST_ADMIN_NAME, PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION_FULL,
        TEST_POLICY_VALUE, TEST_POLICY_VALUE, DEFAULT_USER_ID);
    ASSERT_TRUE(res == ERR_OK);

    DisallowDistributedTransmissionPlugin plugin;
    bool data = true;
    bool currentData;
    bool mergeData;
    ErrCode ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == EdmReturnErrCode::CONFIGURATION_CONFLICT_FAILED);

    ErrCode clearRes = policyManager->SetPolicy(TEST_ADMIN_NAME,
        PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION_FULL, "", "", DEFAULT_USER_ID);
    ASSERT_TRUE(clearRes == ERR_OK);
    IPolicyManager::policyManagerInstance_ = nullptr;
}

/**
 * @tc.name: TestDisallowDistributedTransmissionHasConflictPolicyWithConflict
 * @tc.desc: Test DisallowDistributedTransmissionPlugin::HasConflictPolicy when conflict exists.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionPluginTest, TestDisallowDistributedTransmissionHasConflictPolicyWithConflict,
    TestSize.Level1)
{
    std::shared_ptr<PolicyManager> policyManager = std::make_shared<PolicyManager>();
    IPolicyManager::policyManagerInstance_ = policyManager.get();
    ErrCode res = policyManager->SetPolicy(TEST_ADMIN_NAME, PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION_FULL,
        TEST_POLICY_VALUE, TEST_POLICY_VALUE, DEFAULT_USER_ID);
    ASSERT_TRUE(res == ERR_OK);

    DisallowDistributedTransmissionPlugin plugin;
    bool hasConflict = plugin.HasConflictPolicy(DEFAULT_USER_ID);
    ASSERT_TRUE(hasConflict == true);

    ErrCode clearRes = policyManager->SetPolicy(TEST_ADMIN_NAME,
        PolicyName::POLICY_DISALLOWED_DISTRIBUTED_TRANSMISSION_FULL, "", "", DEFAULT_USER_ID);
    ASSERT_TRUE(clearRes == ERR_OK);
    IPolicyManager::policyManagerInstance_ = nullptr;
}

/**
 * @tc.name: TestDisallowDistributedTransmissionHasConflictPolicyWithoutConflict
 * @tc.desc: Test DisallowDistributedTransmissionPlugin::HasConflictPolicy when no conflict exists.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowDistributedTransmissionPluginTest, TestDisallowDistributedTransmissionHasConflictPolicyWithoutConflict,
    TestSize.Level1)
{
    std::shared_ptr<PolicyManager> policyManager = std::make_shared<PolicyManager>();
    IPolicyManager::policyManagerInstance_ = policyManager.get();

    DisallowDistributedTransmissionPlugin plugin;
    bool hasConflict = plugin.HasConflictPolicy(DEFAULT_USER_ID);
    ASSERT_TRUE(hasConflict == false);

    IPolicyManager::policyManagerInstance_ = nullptr;
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS
