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


#include "shutdown_plugin_test.h"
#include "edm_ipc_interface_code.h"
#include "plugin_singleton.h"
#include "utils.h"

using namespace testing::ext;

namespace OHOS {
namespace EDM {
namespace TEST {
void ShutdownPluginTest::SetUpTestSuite(void)
{
    Utils::SetEdmInitialEnv();
}

void ShutdownPluginTest::TearDownTestSuite(void)
{
    Utils::ResetTokenTypeAndUid();
    ASSERT_TRUE(Utils::IsOriginalUTEnv());
    std::cout << "now ut process is orignal ut env : " << Utils::IsOriginalUTEnv() << std::endl;
}

/**
 * @tc.name: TestShutdown
 * @tc.desc: Test ShutdownPlugin::OnSetPolicy function success.
 * @tc.type: FUNC
 */
HWTEST_F(ShutdownPluginTest, TestShutdown, TestSize.Level1)
{
    GTEST_LOG_(WARNING) << "warning: to avoid affectd others case, TestShutdown case will not run.";
    ASSERT_TRUE(true);
    return;
    ShutdownPlugin plugin;
    ErrCode ret = plugin.OnSetPolicy();
    ASSERT_TRUE(ret == ERR_OK);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS