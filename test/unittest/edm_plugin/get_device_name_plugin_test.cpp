/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "edm_data_ability_utils_mock.h"
#include "plugin_manager.h"
#include "policy_info.h"
#include "get_device_name_plugin.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
class GetDeviceNamePluginTest : public testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}
};

/**
 * @tc.name: TestOnPolicy
 * @tc.desc: Test OnPolicy function.
 * @tc.type: FUNC
 */
HWTEST_F(GetDeviceNamePluginTest, TestOnPolicy, TestSize.Level1)
{
    EdmDataAbilityUtils utils;
    std::shared_ptr<IPlugin> plugin = GetDeviceNamePlugin::GetPlugin();
    std::string policyValue{"TestString"};
    MessageParcel reply;
    utils.SetResult("test Failed");
    ErrCode code = plugin->OnGetPolicy(policyValue, reply);
    EXPECT_TRUE(code != ERR_OK);
    utils.SetResult("test value nullptr");
    code = plugin->OnGetPolicy(policyValue, reply);
    EXPECT_TRUE(code == ERR_OK);
    utils.SetResult("test success");
    code = plugin->OnGetPolicy(policyValue, reply);
    EXPECT_TRUE(code == ERR_OK);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS