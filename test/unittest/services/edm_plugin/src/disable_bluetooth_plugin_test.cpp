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

#define private public
#include "disable_bluetooth_plugin.h"
#undef private

#include "bluetooth_def.h"
#include "bluetooth_errorcode.h"
#include "bluetooth_host.h"
#include "parameters.h"
#include "edm_ipc_interface_code.h"
#include "iplugin_manager.h"
#include "plugin_singleton.h"
#include "utils.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
const std::string PERSIST_BLUETOOTH_CONTROL = "persist.edm.prohibit_bluetooth";
class DisableBluetoothPluginTest : public testing::Test {
protected:
    static void SetUpTestSuite(void);

    static void TearDownTestSuite(void);
};

void DisableBluetoothPluginTest::SetUpTestSuite(void)
{
    Utils::SetEdmServiceEnable();
    Utils::SetEdmInitialEnv();
}

void DisableBluetoothPluginTest::TearDownTestSuite(void)
{
    Utils::SetEdmServiceDisable();
    Utils::ResetTokenTypeAndUid();
    ASSERT_TRUE(Utils::IsOriginalUTEnv());
    Bluetooth::BluetoothHost::GetDefaultHost().Close();
    std::cout << "now ut process is orignal ut env : " << Utils::IsOriginalUTEnv() << std::endl;
}

/**
 * @tc.name: TestDisableBluetoothPluginTestCloseSetTrue
 * @tc.desc: Test DisableBluetoothPluginTest::OnSetPolicy function.
 * @tc.type: FUNC
 */
HWTEST_F(DisableBluetoothPluginTest, TestDisableBluetoothPluginTestCloseSetTrue, TestSize.Level1)
{
    if (Bluetooth::BluetoothHost::GetDefaultHost().IsBrEnabled()) {
        Bluetooth::BluetoothHost::GetDefaultHost().DisableBt();
    }
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    std::shared_ptr<IPlugin> plugin = DisableBluetoothPlugin::GetPlugin();
    std::uint32_t funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::DISABLE_BLUETOOTH);
    HandlePolicyData handlePolicyData{"false", "", false};
    ErrCode ret = plugin->OnHandlePolicy(funcCode, data, reply, handlePolicyData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(handlePolicyData.isChanged);
    ASSERT_TRUE(OHOS::system::GetBoolParameter(PERSIST_BLUETOOTH_CONTROL, false));
}

/**
 * @tc.name: TestDisableBluetoothPluginTestSetFalse
 * @tc.desc: Test DisableBluetoothPluginTest::OnSetPolicy function.
 * @tc.type: FUNC
 */
HWTEST_F(DisableBluetoothPluginTest, TestDisableBluetoothPluginTestSetFalse, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(false);
    std::shared_ptr<IPlugin> plugin = DisableBluetoothPlugin::GetPlugin();
    std::uint32_t funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::DISABLE_BLUETOOTH);
    HandlePolicyData handlePolicyData{"false", "", false};
    ErrCode ret = plugin->OnHandlePolicy(funcCode, data, reply, handlePolicyData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(handlePolicyData.isChanged);
    ASSERT_FALSE(OHOS::system::GetBoolParameter(PERSIST_BLUETOOTH_CONTROL, true));
}

/**
 * @tc.name: TestDisableBluetoothPluginTestOpenSetTrue
 * @tc.desc: Test DisableBluetoothPluginTest::OnSetPolicy function.
 * @tc.type: FUNC
 */
HWTEST_F(DisableBluetoothPluginTest, TestDisableBluetoothPluginTestOpenSetTrue, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(false);
    std::shared_ptr<IPlugin> plugin = DisableBluetoothPlugin::GetPlugin();
    std::uint32_t funcCode = POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::DISABLE_BLUETOOTH);
    HandlePolicyData handlePolicyData{"false", "", false};
    ErrCode ret = plugin->OnHandlePolicy(funcCode, data, reply, handlePolicyData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(handlePolicyData.isChanged);
    ASSERT_FALSE(OHOS::system::GetBoolParameter(PERSIST_BLUETOOTH_CONTROL, true));
    Bluetooth::BluetoothHost::GetDefaultHost().EnableBle();
    data.WriteBool(true);
    handlePolicyData.isChanged = false;
    ret = plugin->OnHandlePolicy(funcCode, data, reply, handlePolicyData, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(handlePolicyData.isChanged);
    ASSERT_TRUE(OHOS::system::GetBoolParameter(PERSIST_BLUETOOTH_CONTROL, false));
}

/**
 * @tc.name: TestDisableBluetoothPluginTestOnAdminRemove
 * @tc.desc: Test DisableBluetoothPluginTest::OnAdminRemove function.
 * @tc.type: FUNC
 */
HWTEST_F(DisableBluetoothPluginTest, TestDisableBluetoothPluginTestOnAdminRemove, TestSize.Level1)
{
    std::shared_ptr<IPlugin> plugin = DisableBluetoothPlugin::GetPlugin();
    std::string currentPolicy = "true";
    std::string mergePolicy = "false";
    ErrCode ret = plugin->OnAdminRemove("test", currentPolicy, mergePolicy, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_FALSE(OHOS::system::GetBoolParameter(PERSIST_BLUETOOTH_CONTROL, true));
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS