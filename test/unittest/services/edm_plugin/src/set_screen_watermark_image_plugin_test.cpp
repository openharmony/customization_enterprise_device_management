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
#include "edm_constants.h"
#include "edm_ipc_interface_code.h"
#include "parameters.h"
#include "screen_watermark_image_serializer.h"
#include "set_screen_watermark_image_plugin.h"
#include "utils.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {

class SetScreenWatermarkImagePluginTest : public testing::Test {
protected:
    static void SetUpTestSuite(void);

    static void TearDownTestSuite(void);
};

void SetScreenWatermarkImagePluginTest::SetUpTestSuite(void)
{
    Utils::SetEdmInitialEnv();
}

void SetScreenWatermarkImagePluginTest::TearDownTestSuite(void)
{
    Utils::ResetTokenTypeAndUid();
    ASSERT_TRUE(Utils::IsOriginalUTEnv());
    std::cout << "now ut process is original ut env : " << Utils::IsOriginalUTEnv() << std::endl;
}

/**
 * @tc.name: TestOnHandlePolicyRemoveEmpty
 * @tc.desc: Test SetScreenWatermarkImagePlugin::OnHandlePolicy function when remove with empty policy.
 * @tc.type: FUNC
 */
HWTEST_F(SetScreenWatermarkImagePluginTest, TestOnHandlePolicyRemoveEmpty, TestSize.Level1)
{
    SetScreenWatermarkImagePlugin plugin;
    std::uint32_t funcCode =
        POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::REMOVE, EdmInterfaceCode::SCREEN_WATERMARK_IMAGE);
    MessageParcel data;
    MessageParcel reply;
    HandlePolicyData policyData;
    policyData.policyData = "";
    ErrCode ret = plugin.OnHandlePolicy(funcCode, data, reply, policyData, 100);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestOnHandlePolicyRemove
 * @tc.desc: Test SetScreenWatermarkImagePlugin::OnHandlePolicy function when remove with valid policy.
 * @tc.type: FUNC
 */
HWTEST_F(SetScreenWatermarkImagePluginTest, TestOnHandlePolicyRemove, TestSize.Level1)
{
    SetScreenWatermarkImagePlugin plugin;
    std::uint32_t funcCode =
        POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::REMOVE, EdmInterfaceCode::SCREEN_WATERMARK_IMAGE);
    MessageParcel data;
    MessageParcel reply;
    HandlePolicyData policyData;
    WatermarkImageType currentData{"testFileName", 400, 400};
    auto serializer = ScreenWatermarkImageSerializer::GetInstance();
    serializer->Serialize(currentData, policyData.policyData);
    ErrCode ret = plugin.OnHandlePolicy(funcCode, data, reply, policyData, 100);
    ASSERT_TRUE(ret == EdmReturnErrCode::SYSTEM_ABNORMALLY);
}

/**
 * @tc.name: TestOnHandlePolicyInvalidOperateType
 * @tc.desc: Test SetScreenWatermarkImagePlugin::OnHandlePolicy function with invalid operate type.
 * @tc.type: FUNC
 */
HWTEST_F(SetScreenWatermarkImagePluginTest, TestOnHandlePolicyInvalidOperateType, TestSize.Level1)
{
    SetScreenWatermarkImagePlugin plugin;
    std::uint32_t funcCode =
        POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::GET, EdmInterfaceCode::SCREEN_WATERMARK_IMAGE);
    MessageParcel data;
    MessageParcel reply;
    HandlePolicyData policyData;
    ErrCode ret = plugin.OnHandlePolicy(funcCode, data, reply, policyData, 100);
    ASSERT_TRUE(ret == EdmReturnErrCode::SYSTEM_ABNORMALLY);
}

/**
 * @tc.name: TestOnAdminRemoveEmpty
 * @tc.desc: Test SetScreenWatermarkImagePlugin::OnAdminRemove function with empty policy.
 * @tc.type: FUNC
 */
HWTEST_F(SetScreenWatermarkImagePluginTest, TestOnAdminRemoveEmpty, TestSize.Level1)
{
    SetScreenWatermarkImagePlugin plugin;
    std::string adminName = "testAdmin";
    std::string policyData = "";
    std::string mergeData = "";
    int32_t userId = 100;
    ErrCode ret = plugin.OnAdminRemove(adminName, policyData, mergeData, userId);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestOnAdminRemoveWithPolicy
 * @tc.desc: Test SetScreenWatermarkImagePlugin::OnAdminRemove function with valid policy.
 * @tc.type: FUNC
 */
HWTEST_F(SetScreenWatermarkImagePluginTest, TestOnAdminRemoveWithPolicy, TestSize.Level1)
{
    SetScreenWatermarkImagePlugin plugin;
    std::string adminName = "testAdmin";
    WatermarkImageType currentData{"testFileName", 400, 400};
    auto serializer = ScreenWatermarkImageSerializer::GetInstance();
    std::string policyData;
    serializer->Serialize(currentData, policyData);
    std::string mergeData = policyData;
    int32_t userId = 100;
    ErrCode ret = plugin.OnAdminRemove(adminName, policyData, mergeData, userId);
    ASSERT_EQ(ret, EdmReturnErrCode::SYSTEM_ABNORMALLY);
}

/**
 * @tc.name: TestGetOthersMergePolicyData
 * @tc.desc: Test SetScreenWatermarkImagePlugin::GetOthersMergePolicyData function.
 * @tc.type: FUNC
 */
HWTEST_F(SetScreenWatermarkImagePluginTest, TestGetOthersMergePolicyData, TestSize.Level1)
{
    SetScreenWatermarkImagePlugin plugin;
    std::string adminName = "testAdmin";
    int32_t userId = 100;
    std::string othersMergePolicyData;
    ErrCode ret = plugin.GetOthersMergePolicyData(adminName, userId, othersMergePolicyData);
    ASSERT_TRUE(ret == ERR_OK);
}

/**
 * @tc.name: TestSerializerDeserializeEmpty
 * @tc.desc: Test ScreenWatermarkImageSerializer::Deserialize function with empty policy.
 * @tc.type: FUNC
 */
HWTEST_F(SetScreenWatermarkImagePluginTest, TestSerializerDeserializeEmpty, TestSize.Level1)
{
    auto serializer = ScreenWatermarkImageSerializer::GetInstance();
    WatermarkImageType dataObj;
    std::string policy = "";
    bool ret = serializer->Deserialize(policy, dataObj);
    ASSERT_TRUE(ret);
    ASSERT_TRUE(dataObj.fileName.empty());
}

/**
 * @tc.name: TestSerializerSerialize
 * @tc.desc: Test ScreenWatermarkImageSerializer::Serialize function.
 * @tc.type: FUNC
 */
HWTEST_F(SetScreenWatermarkImagePluginTest, TestSerializerSerialize, TestSize.Level1)
{
    auto serializer = ScreenWatermarkImageSerializer::GetInstance();
    WatermarkImageType dataObj{"testFileName", 400, 400};
    std::string policy;
    bool ret = serializer->Serialize(dataObj, policy);
    ASSERT_TRUE(ret);
    ASSERT_FALSE(policy.empty());
}

/**
 * @tc.name: TestSerializerDeserializeValid
 * @tc.desc: Test ScreenWatermarkImageSerializer::Deserialize function with valid policy.
 * @tc.type: FUNC
 */
HWTEST_F(SetScreenWatermarkImagePluginTest, TestSerializerDeserializeValid, TestSize.Level1)
{
    auto serializer = ScreenWatermarkImageSerializer::GetInstance();
    WatermarkImageType dataObj{"testFileName", 400, 400};
    std::string policy;
    serializer->Serialize(dataObj, policy);
    
    WatermarkImageType result;
    bool ret = serializer->Deserialize(policy, result);
    ASSERT_TRUE(ret);
    ASSERT_TRUE(result.fileName == "testFileName");
    ASSERT_TRUE(result.width == 400);
    ASSERT_TRUE(result.height == 400);
}

} // namespace TEST
} // namespace EDM
} // namespace OHOS