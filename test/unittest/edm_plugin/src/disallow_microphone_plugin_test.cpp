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

#include "disallow_microphone_plugin.h"

#include <gtest/gtest.h>

#include "edm_ipc_interface_code.h"
#include "iplugin_manager.h"
#include "plugin_singleton.h"
#include "utils.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
class DisallowMicrophonePluginTest : public testing::Test {
protected:
    static void SetUpTestSuite(void);

    static void TearDownTestSuite(void);
};

void DisallowMicrophonePluginTest::SetUpTestSuite(void)
{
    Utils::SetEdmInitialEnv();
}

void DisallowMicrophonePluginTest::TearDownTestSuite(void)
{
    Utils::ResetTokenTypeAndUid();
    ASSERT_TRUE(Utils::IsOriginalUTEnv());
    std::cout << "now ut process is orignal ut env : " << Utils::IsOriginalUTEnv() << std::endl;
}

/**
 * @tc.name: TestDisallowMicrophonePluginTestSet
 * @tc.desc: Test DisallowMicrophonePluginTest::OnSetPolicy function.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowMicrophonePluginTest, TestDisallowMicrophonePluginTestSet, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(false);
    std::shared_ptr<IPlugin> plugin = DisallowMicrophonePlugin::GetPlugin();
    std::string policyData{"false"};
    std::uint32_t funcCode =
        POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::DISALLOW_MICROPHONE);
    bool isChanged = false;
    ErrCode ret = plugin->OnHandlePolicy(funcCode, data, reply, policyData, isChanged, DEFAULT_USER_ID);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(isChanged);
}

/**
 * @tc.name: TestDisallowMicrophonePluginTestGet
 * @tc.desc: Test DisallowMicrophonePluginTest::OnGetPolicy function.
 * @tc.type: FUNC
 */
HWTEST_F(DisallowMicrophonePluginTest, TestDisallowMicrophonePluginTestGet, TestSize.Level1)
{
    std::shared_ptr<IPlugin> plugin = DisallowMicrophonePlugin::GetPlugin();
    std::string policyData{"false"};
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = plugin->OnGetPolicy(policyData, data, reply, DEFAULT_USER_ID);
    int32_t flag = ERR_INVALID_VALUE;
    ASSERT_TRUE(reply.ReadInt32(flag) && (flag == ERR_OK));
    bool result = false;
    reply.ReadBool(result);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_FALSE(result);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS
