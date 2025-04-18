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
#include "snapshot_skip_plugin.h"
#include "edm_constants.h"
#include "edm_ipc_interface_code.h"
#include "iplugin_manager.h"
#include "plugin_singleton.h"
#include "utils.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
const std::string TEST_BUNDLE_NAME = "testBundleName";
const std::string TEST_PACKAGE_NAME = "testPackageName";

class SnapshotSkipPluginTest : public testing::Test {
protected:
    static void SetUpTestSuite(void);

    static void TearDownTestSuite(void);
};

void SnapshotSkipPluginTest::SetUpTestSuite(void)
{
    Utils::SetEdmInitialEnv();
}

void SnapshotSkipPluginTest::TearDownTestSuite(void)
{
    Utils::ResetTokenTypeAndUid();
    ASSERT_TRUE(Utils::IsOriginalUTEnv());
    std::cout << "now ut process is orignal ut env : " << Utils::IsOriginalUTEnv() << std::endl;
}

/**
 * @tc.name: SnapshotSkipPlugin
 * @tc.desc: Test SnapshotSkipPlugin::OnSetPolicy when data is empty.
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotSkipPluginTest, TestOnSetPolicyWhenInputDataEmpty, TestSize.Level1)
{
    SnapshotSkipPlugin plugin;
    std::vector<std::string> data;
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestSnapshotSkipPlugin
 * @tc.desc: Test SnapshotSkipPlugin::OnSetPolicy when data is over limit.
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotSkipPluginTest, TestOnSetPolicyWhenInputDataOverLimit, TestSize.Level1)
{
    SnapshotSkipPlugin plugin;
    std::vector<std::string> data(EdmConstants::DISALLOW_LIST_FOR_ACCOUNT_MAX_SIZE + 1, TEST_BUNDLE_NAME);
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == EdmReturnErrCode::PARAM_ERROR);
}

/**
 * @tc.name: TestSnapshotSkipPlugin
 * @tc.desc: Test SnapshotSkipPlugin::OnSetPolicy when data is over limit.
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotSkipPluginTest, TestOnSetPolicyWhenUnionDataOverLimit, TestSize.Level1)
{
    SnapshotSkipPlugin plugin;
    std::vector<std::string> data(EdmConstants::DISALLOW_LIST_FOR_ACCOUNT_MAX_SIZE, TEST_PACKAGE_NAME);
    std::vector<std::string> currentData(EdmConstants::DISALLOW_LIST_FOR_ACCOUNT_MAX_SIZE, TEST_BUNDLE_NAME);
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == EdmReturnErrCode::PARAM_ERROR);
}

/**
 * @tc.name: TestSnapshotSkipPlugin
 * @tc.desc: Test SnapshotSkipPlugin::OnRemovePolicy when data is empty.
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotSkipPluginTest, TestOnRemovePolicyWhenInputDataEmpty, TestSize.Level1)
{
    SnapshotSkipPlugin plugin;
    std::vector<std::string> data;
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnRemovePolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestSnapshotSkipPlugin
 * @tc.desc: Test SnapshotSkipPlugin::OnRemovePolicy when data is over limit.
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotSkipPluginTest, TestOnRemovePolicyWhenInputDataOverLimit, TestSize.Level1)
{
    SnapshotSkipPlugin plugin;
    std::vector<std::string> data(EdmConstants::DISALLOW_LIST_FOR_ACCOUNT_MAX_SIZE + 1, TEST_BUNDLE_NAME);
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnRemovePolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == EdmReturnErrCode::PARAM_ERROR);
}

/**
 * @tc.name: TestSnapshotSkipPlugin
 * @tc.desc: Test SnapshotSkipPlugin::OnGetPolicy function.
 * @tc.type: FUNC
 */
HWTEST_F(SnapshotSkipPluginTest, TestOnGetPolicy, TestSize.Level1)
{
    SnapshotSkipPlugin plugin;
    std::string policyData;
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = plugin.OnGetPolicy(policyData, data, reply, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS