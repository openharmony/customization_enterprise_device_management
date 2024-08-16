/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "edm_log.h"
#include "enhance_execute_strategy.h"
#include "func_code.h"
#include "iplugin_template_test.h"
#include "plugin_manager.h"
#include "plugin_manager_test.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
class EnhanceExecuteStrategyTest : public testing::Test {
public:
    void SetUp() override {}

    void TearDown() override {}
};

/**
 * @tc.name: TestOnGetExecuteWhenFirstIfEstablished
 * @tc.desc: Test OnGetExecute func When the First If condition is met.
 * @tc.type: FUNC
 */
HWTEST_F(EnhanceExecuteStrategyTest, TestOnGetExecuteWhenFirstIfEstablished, TestSize.Level1)
{
    std::string policyData("policyData");
    MessageParcel data;
    MessageParcel reply;
    int32_t userId = 0;

    std::uint32_t funcCode = 0;
    std::unique_ptr<EnhanceExecuteStrategy> enhanceExecuteStrategy = std::make_unique<EnhanceExecuteStrategy>();
    ErrCode eRet = enhanceExecuteStrategy->OnGetExecute(funcCode, policyData, data, reply, userId);
    ASSERT_TRUE(eRet == ERR_EDM_GET_POLICY_FAILED);
}

/**
 * @tc.name: TestOnGetExecuteWhenSecondIfEstablished
 * @tc.desc: Test OnGetExecute func When the Second If condition is met.
 * @tc.type: FUNC
 */
HWTEST_F(EnhanceExecuteStrategyTest, TestOnGetExecuteWhenSecondIfEstablished, TestSize.Level1)
{
    std::string policyData("policyData");
    MessageParcel data;
    MessageParcel reply;
    int32_t userId = 0;

    std::unique_ptr<EnhanceExecuteStrategy> enhanceExecuteStrategy = std::make_unique<EnhanceExecuteStrategy>();
    std::shared_ptr<IPlugin> plugin = std::make_shared<TestPlugin>();
    std::uint32_t policyCode = plugin->GetCode();
    std::uint32_t funcCode = FuncCodeUtils::CreateFuncCode(static_cast<std::uint32_t>(FuncFlag::POLICY_FLAG),
        static_cast<std::uint32_t>(FuncOperateType::GET), policyCode);
    bool bRet = PluginManager::GetInstance()->AddPlugin(plugin);
    ASSERT_TRUE(bRet);
    ErrCode eRet = enhanceExecuteStrategy->OnGetExecute(funcCode, policyData, data, reply, userId);
    ASSERT_TRUE(eRet == ERR_OK);
}

/**
 * @tc.name: TestOnGetExecuteWhenThirdIfEstablished
 * @tc.desc: Test OnGetExecute func When the Third If condition is met.
 * @tc.type: FUNC
 */
HWTEST_F(EnhanceExecuteStrategyTest, TestOnGetExecuteWhenThirdIfEstablished, TestSize.Level1)
{
    std::string policyData("policyData");
    MessageParcel data;
    MessageParcel reply;
    int32_t userId = 0;

    std::unique_ptr<EnhanceExecuteStrategy> enhanceExecuteStrategy = std::make_unique<EnhanceExecuteStrategy>();
    std::shared_ptr<IPlugin> plugin = std::make_shared<TestPlugin>();
    std::uint32_t policyCode = plugin->GetCode();
    std::uint32_t funcCode = FuncCodeUtils::CreateFuncCode(static_cast<std::uint32_t>(FuncFlag::POLICY_FLAG),
        static_cast<std::uint32_t>(FuncOperateType::GET), policyCode);
    bool bRet = PluginManager::GetInstance()->AddPlugin(plugin);
    ASSERT_TRUE(bRet);
    std::shared_ptr<IPlugin> extensionPlugin = std::make_shared<TestExtensionPlugin>();
    std::uint32_t extpolicyCode = extensionPlugin->GetCode();
    bRet = PluginManager::GetInstance()->AddExtensionPlugin(extensionPlugin, extpolicyCode, ExecuteStrategy::ENHANCE);
    ASSERT_TRUE(bRet);
    ErrCode eRet = enhanceExecuteStrategy->OnGetExecute(funcCode, policyData, data, reply, userId);
    ASSERT_TRUE(eRet == ERR_OK);
}

/**
 * @tc.name: TestOnSetExecuteWhenFirstIfEstablished
 * @tc.desc: Test OnSetExecute func When the First If condition is met.
 * @tc.type: FUNC
 */
HWTEST_F(EnhanceExecuteStrategyTest, TestOnSetExecuteWhenFirstIfEstablished, TestSize.Level1)
{
    HandlePolicyData policyData;
    MessageParcel data;
    MessageParcel reply;
    int32_t userId = 0;

    std::uint32_t funcCode = 0;
    std::unique_ptr<EnhanceExecuteStrategy> enhanceExecuteStrategy = std::make_unique<EnhanceExecuteStrategy>();
    ErrCode eRet = enhanceExecuteStrategy->OnSetExecute(funcCode, data, reply, policyData, userId);
    ASSERT_TRUE(eRet == ERR_EDM_HANDLE_POLICY_FAILED);
}

/**
 * @tc.name: TestOnSetExecuteWhenSecondIfEstablished
 * @tc.desc: Test OnSetExecute func When the Second If condition is met.
 * @tc.type: FUNC
 */
HWTEST_F(EnhanceExecuteStrategyTest, TestOnSetExecuteWhenSecondIfEstablished, TestSize.Level1)
{
    HandlePolicyData policyData;
    MessageParcel data;
    MessageParcel reply;
    int32_t userId = 0;

    std::unique_ptr<EnhanceExecuteStrategy> enhanceExecuteStrategy = std::make_unique<EnhanceExecuteStrategy>();
    std::shared_ptr<IPlugin> plugin = std::make_shared<TestPlugin>();
    std::uint32_t policyCode = plugin->GetCode();
    std::uint32_t funcCode = FuncCodeUtils::CreateFuncCode(static_cast<std::uint32_t>(FuncFlag::POLICY_FLAG),
        static_cast<std::uint32_t>(FuncOperateType::SET), policyCode);
    bool bRet = PluginManager::GetInstance()->AddPlugin(plugin);
    ASSERT_TRUE(bRet);
    ErrCode eRet = enhanceExecuteStrategy->OnSetExecute(funcCode, data, reply, policyData, userId);
    ASSERT_TRUE(eRet == ERR_OK);
}

/**
 * @tc.name: TestOnSetExecuteWhenThirdIfEstablished
 * @tc.desc: Test OnSetExecute func When the Third If condition is met.
 * @tc.type: FUNC
 */
HWTEST_F(EnhanceExecuteStrategyTest, TestOnSetExecuteWhenThirdIfEstablished, TestSize.Level1)
{
    HandlePolicyData policyData;
    MessageParcel data;
    MessageParcel reply;
    int32_t userId = 0;

    std::unique_ptr<EnhanceExecuteStrategy> enhanceExecuteStrategy = std::make_unique<EnhanceExecuteStrategy>();
    std::shared_ptr<IPlugin> plugin = std::make_shared<TestPlugin>();
    std::uint32_t policyCode = plugin->GetCode();
    std::uint32_t funcCode = FuncCodeUtils::CreateFuncCode(static_cast<std::uint32_t>(FuncFlag::POLICY_FLAG),
        static_cast<std::uint32_t>(FuncOperateType::SET), policyCode);
    bool bRet = PluginManager::GetInstance()->AddPlugin(plugin);
    ASSERT_TRUE(bRet);
    std::shared_ptr<IPlugin> extensionPlugin = std::make_shared<TestExtensionPlugin>();
    std::uint32_t extpolicyCode = extensionPlugin->GetCode();
    bRet = PluginManager::GetInstance()->AddExtensionPlugin(extensionPlugin, extpolicyCode, ExecuteStrategy::ENHANCE);
    ASSERT_TRUE(bRet);
    ErrCode eRet = enhanceExecuteStrategy->OnSetExecute(funcCode, data, reply, policyData, userId);
    ASSERT_TRUE(eRet == ERR_OK);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS