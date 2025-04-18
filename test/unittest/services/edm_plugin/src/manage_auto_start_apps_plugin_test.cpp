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

#define protected public
#include "manage_auto_start_apps_plugin_test.h"
#undef protected

#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "if_system_ability_manager.h"
#include "iremote_stub.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "system_ability_definition.h"

#include "array_string_serializer.h"
#include "edm_constants.h"
#include "edm_log.h"
#include "edm_sys_manager.h"
#include "install_plugin.h"
#include "uninstall_plugin.h"
#include "utils.h"

using namespace testing::ext;

namespace OHOS {
namespace EDM {
namespace TEST {
const std::string RIGHT_TEST_BUNDLE = "com.example.l3jsdemo/com.example.l3jsdemo.MainAbility";
const std::string ERROR_TEST_BUNDLE = "com.example.l3jsdemo/com.example.l3jsdemo.ErrorAbility";
const std::string INVALID_TEST_BUNDLE = "com.example.l3jsdemo.com.example.l3jsdemo.ErrorAbility";
const std::string HAP_FILE_PATH = "/data/test/resource/enterprise_device_management/hap/right.hap";
const std::string BOOT_OEM_MODE = "const.boot.oemmode";
const std::string DEVELOP_PARAM = "rd";
const std::string USER_MODE = "user";
void ManageAutoStartAppsPluginTest::SetUpTestSuite(void)
{
    Utils::SetEdmInitialEnv();
}

void ManageAutoStartAppsPluginTest::TearDownTestSuite(void)
{
    Utils::ResetTokenTypeAndUid();
    ASSERT_TRUE(Utils::IsOriginalUTEnv());
    std::cout << "now ut process is orignal ut env : " << Utils::IsOriginalUTEnv() << std::endl;
}

/**
 * @tc.name: TestOnSetPolicySucWithNullData
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnSetPolicy when data is empty.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnSetPolicySucWithNullData, TestSize.Level1)
{
    ManageAutoStartAppsPlugin plugin;
    plugin.maxListSize_ = EdmConstants::AUTO_START_APPS_MAX_SIZE;
    std::vector<std::string> data;
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestOnSetPolicyFailWithbundleExceededLimit
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnSetPolicy when budle is not existed.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnSetPolicyFailWithbundleExceededLimit, TestSize.Level1)
{
    ManageAutoStartAppsPlugin plugin;
    plugin.maxListSize_ = EdmConstants::AUTO_START_APPS_MAX_SIZE;
    std::vector<std::string> data;
    for (int i = 0; i < 15; i++) {
        std::string str = "test/test" + std::to_string(i);
        data.push_back(str);
    }
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == EdmReturnErrCode::PARAM_ERROR);
}

/**
 * @tc.name: TestOnSetPolicyFailWithbundleNotExist
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnSetPolicy when budle is not existed.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnSetPolicyFailWithbundleNotExist, TestSize.Level1)
{
    ManageAutoStartAppsPlugin plugin;
    plugin.maxListSize_ = EdmConstants::AUTO_START_APPS_MAX_SIZE;
    std::vector<std::string> data = {RIGHT_TEST_BUNDLE};
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == EdmReturnErrCode::PARAM_ERROR);
}

/**
 * @tc.name: TestOnSetPolicyFailWithInvalidData
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnSetPolicy when data is invalid.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnSetPolicyFailWithInvalidData, TestSize.Level1)
{
    ManageAutoStartAppsPlugin plugin;
    plugin.maxListSize_ = EdmConstants::AUTO_START_APPS_MAX_SIZE;
    std::vector<std::string> data = {INVALID_TEST_BUNDLE};
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == EdmReturnErrCode::PARAM_ERROR);
}

/**
 * @tc.name: TestOnSetPolicySuc
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnSetPolicy when budle is existed.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnSetPolicySuc, TestSize.Level1)
{
    std::string developDeviceParam = system::GetParameter(BOOT_OEM_MODE, USER_MODE);
    if (developDeviceParam == DEVELOP_PARAM) {
        InstallPlugin installPlugin;
        InstallParam param = {{HAP_FILE_PATH}, DEFAULT_USER_ID, 0};
        MessageParcel reply;
        ErrCode ret = installPlugin.OnSetPolicy(param, reply);
        EXPECT_TRUE(ret == ERR_OK);

        ManageAutoStartAppsPlugin plugin;
        plugin.maxListSize_ = EdmConstants::AUTO_START_APPS_MAX_SIZE;
        std::vector<std::string> data = {RIGHT_TEST_BUNDLE, ERROR_TEST_BUNDLE, INVALID_TEST_BUNDLE};
        std::vector<std::string> currentData;
        std::vector<std::string> mergeData;
        ret = plugin.OnBasicSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
        EXPECT_TRUE(ret == ERR_OK);

        std::string policyData;
        ArrayStringSerializer::GetInstance()->Serialize(currentData, policyData);
        MessageParcel parcel;
        MessageParcel getReply;
        ret = plugin.OnGetPolicy(policyData, parcel, getReply, DEFAULT_USER_ID);
        std::vector<std::string> res;
        EXPECT_TRUE(ret == ERR_OK);
        EXPECT_TRUE(getReply.ReadInt32() == ERR_OK);
        getReply.ReadStringVector(&res);
        EXPECT_TRUE(res.size() >= 1);
        EXPECT_TRUE(std::find(res.begin(), res.end(), RIGHT_TEST_BUNDLE) != res.end());

        std::vector<std::string> removeData = {RIGHT_TEST_BUNDLE, ERROR_TEST_BUNDLE, INVALID_TEST_BUNDLE};
        mergeData.clear();
        ret = plugin.OnBasicRemovePolicy(removeData, currentData, mergeData, DEFAULT_USER_ID);
        EXPECT_TRUE(ret == ERR_OK);

        MessageParcel removeReply;
        ArrayStringSerializer::GetInstance()->Serialize(currentData, policyData);
        ret = plugin.OnGetPolicy(policyData, parcel, removeReply, DEFAULT_USER_ID);
        std::vector<std::string> afterRemove;
        EXPECT_TRUE(ret == ERR_OK);
        EXPECT_TRUE(removeReply.ReadInt32() == ERR_OK);
        removeReply.ReadStringVector(&afterRemove);
        EXPECT_TRUE(afterRemove.size() == 0);

        UninstallPlugin uninstallPlugin;
        UninstallParam uninstallParam = {"com.example.l3jsdemo", DEFAULT_USER_ID, false};
        MessageParcel uninstallReply;
        ret = uninstallPlugin.OnSetPolicy(uninstallParam, uninstallReply);
        EXPECT_TRUE(ret == ERR_OK);
    }
}

/**
 * @tc.name: TestOnGetPolicySuc
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnGetPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnGetPolicySuc, TestSize.Level1)
{
    ManageAutoStartAppsPlugin plugin;
    std::string policyData = RIGHT_TEST_BUNDLE;
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = plugin.OnGetPolicy(policyData, data, reply, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(reply.ReadInt32() == ERR_OK);
}

/**
 * @tc.name: TestOnRemovePolicySucWithNullData
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnRemovePolicy when data is empty.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnRemovePolicySucWithNullData, TestSize.Level1)
{
    ManageAutoStartAppsPlugin plugin;
    plugin.maxListSize_ = EdmConstants::AUTO_START_APPS_MAX_SIZE;
    std::vector<std::string> data;
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicRemovePolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestOnRemovePolicyFileWithErrBundle
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnRemovePolicy when budle is not existed.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnRemovePolicyFileWithErrBundle, TestSize.Level1)
{
    ManageAutoStartAppsPlugin plugin;
    plugin.maxListSize_ = EdmConstants::AUTO_START_APPS_MAX_SIZE;
    std::vector<std::string> data = {ERROR_TEST_BUNDLE};
    std::vector<std::string> currentData;
    std::vector<std::string> mergeData;
    ErrCode ret = plugin.OnBasicRemovePolicy(data, currentData, mergeData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestOnRemovePolicySuc
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnSetPolicy when data is invalid.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnRemovePolicySuc, TestSize.Level1)
{
    std::string developDeviceParam = system::GetParameter(BOOT_OEM_MODE, USER_MODE);
    if (developDeviceParam == DEVELOP_PARAM) {
        InstallPlugin installPlugin;
        InstallParam param = {{HAP_FILE_PATH}, DEFAULT_USER_ID, 0};
        MessageParcel reply;
        ErrCode ret = installPlugin.OnSetPolicy(param, reply);
        EXPECT_TRUE(ret == ERR_OK);

        ManageAutoStartAppsPlugin plugin;
        plugin.maxListSize_ = EdmConstants::AUTO_START_APPS_MAX_SIZE;
        std::vector<std::string> data = {RIGHT_TEST_BUNDLE};
        std::vector<std::string> currentData;
        std::vector<std::string> mergeData;
        ret = plugin.OnBasicSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
        EXPECT_TRUE(ret == ERR_OK);

        data = {INVALID_TEST_BUNDLE};
        currentData = {INVALID_TEST_BUNDLE};
        ret = plugin.OnBasicRemovePolicy(data, currentData, mergeData, DEFAULT_USER_ID);
        EXPECT_TRUE(ret == EdmReturnErrCode::PARAM_ERROR);

        data = {RIGHT_TEST_BUNDLE};
        currentData = {RIGHT_TEST_BUNDLE};
        ret = plugin.OnBasicRemovePolicy(data, currentData, mergeData, DEFAULT_USER_ID);
        EXPECT_TRUE(ret == ERR_OK);

        UninstallPlugin uninstallPlugin;
        UninstallParam uninstallParam = {"com.example.l3jsdemo", DEFAULT_USER_ID, false};
        MessageParcel uninstallReply;
        ret = uninstallPlugin.OnSetPolicy(uninstallParam, uninstallReply);
        EXPECT_TRUE(ret == ERR_OK);
    }
}

/**
 * @tc.name: TestOnRemovePolicySucAlreadyUninstall
 * @tc.desc: Test ManageAutoStartAppsPlugin::OnSetPolicy when hap already uninstall.
 * @tc.type: FUNC
 */
HWTEST_F(ManageAutoStartAppsPluginTest, TestOnRemovePolicySucAlreadyUninstall, TestSize.Level1)
{
    std::string developDeviceParam = system::GetParameter(BOOT_OEM_MODE, USER_MODE);
    if (developDeviceParam == DEVELOP_PARAM) {
        InstallPlugin installPlugin;
        InstallParam param = {{HAP_FILE_PATH}, DEFAULT_USER_ID, 0};
        MessageParcel reply;
        ErrCode ret = installPlugin.OnSetPolicy(param, reply);
        EXPECT_TRUE(ret == ERR_OK);

        ManageAutoStartAppsPlugin plugin;
        plugin.maxListSize_ = EdmConstants::AUTO_START_APPS_MAX_SIZE;
        std::vector<std::string> data = {RIGHT_TEST_BUNDLE};
        std::vector<std::string> currentData;
        std::vector<std::string> mergeData;
        ret = plugin.OnBasicSetPolicy(data, currentData, mergeData, DEFAULT_USER_ID);
        EXPECT_TRUE(ret == ERR_OK);

        UninstallPlugin uninstallPlugin;
        UninstallParam uninstallParam = {"com.example.l3jsdemo", DEFAULT_USER_ID, false};
        MessageParcel uninstallReply;
        ret = uninstallPlugin.OnSetPolicy(uninstallParam, uninstallReply);
        EXPECT_TRUE(ret == ERR_OK);

        data = {RIGHT_TEST_BUNDLE};
        mergeData.clear();
        ret = plugin.OnBasicRemovePolicy(data, currentData, mergeData, DEFAULT_USER_ID);
        EXPECT_TRUE(ret == ERR_OK);
    }
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS