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
#include <string>

#include "policy_changed_event.h"

using namespace testing::ext;

namespace OHOS {
namespace EDM {
namespace TEST {
const std::string TEST_INTERFACE_NAME = "setPasswordPolicy";
const std::string TEST_ADMIN_NAME = "com.edm.test.demo";
const std::string TEST_PARAMETERS = "{}";
const int64_t TEST_TIMESTAMP = 1234567890;

class PolicyChangedEventTest : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;

    std::shared_ptr<PolicyChangedEvent> policyChangedEvent_;
};

void PolicyChangedEventTest::SetUp()
{
    policyChangedEvent_ = std::make_shared<PolicyChangedEvent>();
}

void PolicyChangedEventTest::TearDown()
{
    policyChangedEvent_.reset();
}

/**
 * @tc.name: TestConstructorWithParameters_Success
 * @tc.desc: Test PolicyChangedEvent constructor with parameters.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestConstructorWithParameters_Success, TestSize.Level1)
{
    PolicyChangedEvent event(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    EXPECT_EQ(event.GetInterfaceName(), TEST_INTERFACE_NAME);
    EXPECT_EQ(event.GetAdminName(), TEST_ADMIN_NAME);
    EXPECT_EQ(event.GetParameters(), TEST_PARAMETERS);
    EXPECT_EQ(event.GetTimestamp(), TEST_TIMESTAMP);
}

/**
 * @tc.name: TestDefaultConstructor_Success
 * @tc.desc: Test PolicyChangedEvent default constructor.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestDefaultConstructor_Success, TestSize.Level1)
{
    PolicyChangedEvent event;
    EXPECT_EQ(event.GetInterfaceName(), "");
    EXPECT_EQ(event.GetAdminName(), "");
    EXPECT_EQ(event.GetParameters(), "");
    EXPECT_EQ(event.GetTimestamp(), 0);
}

/**
 * @tc.name: TestSetPolicyChangedEvent_Success
 * @tc.desc: Test SetPolicyChangedEvent function.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestSetPolicyChangedEvent_Success, TestSize.Level1)
{
    policyChangedEvent_->SetPolicyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    EXPECT_EQ(policyChangedEvent_->GetInterfaceName(), TEST_INTERFACE_NAME);
    EXPECT_EQ(policyChangedEvent_->GetAdminName(), TEST_ADMIN_NAME);
    EXPECT_EQ(policyChangedEvent_->GetParameters(), TEST_PARAMETERS);
    EXPECT_EQ(policyChangedEvent_->GetTimestamp(), TEST_TIMESTAMP);
}

/**
 * @tc.name: TestSetPolicyChangedEvent_EmptyStrings
 * @tc.desc: Test SetPolicyChangedEvent function with empty strings.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestSetPolicyChangedEvent_EmptyStrings, TestSize.Level1)
{
    policyChangedEvent_->SetPolicyChangedEvent("", "", "", 0);
    EXPECT_EQ(policyChangedEvent_->GetInterfaceName(), "");
    EXPECT_EQ(policyChangedEvent_->GetAdminName(), "");
    EXPECT_EQ(policyChangedEvent_->GetParameters(), "");
    EXPECT_EQ(policyChangedEvent_->GetTimestamp(), 0);
}

/**
 * @tc.name: TestSetPolicyChangedEvent_NegativeTimestamp
 * @tc.desc: Test SetPolicyChangedEvent function with negative timestamp.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestSetPolicyChangedEvent_NegativeTimestamp, TestSize.Level1)
{
    policyChangedEvent_->SetPolicyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, -1);
    EXPECT_EQ(policyChangedEvent_->GetInterfaceName(), TEST_INTERFACE_NAME);
    EXPECT_EQ(policyChangedEvent_->GetAdminName(), TEST_ADMIN_NAME);
    EXPECT_EQ(policyChangedEvent_->GetParameters(), TEST_PARAMETERS);
    EXPECT_EQ(policyChangedEvent_->GetTimestamp(), -1);
}

/**
 * @tc.name: TestGetInterfaceName_Success
 * @tc.desc: Test GetInterfaceName function.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestGetInterfaceName_Success, TestSize.Level1)
{
    policyChangedEvent_->SetPolicyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    std::string interfaceName = policyChangedEvent_->GetInterfaceName();
    EXPECT_EQ(interfaceName, TEST_INTERFACE_NAME);
}

/**
 * @tc.name: TestGetAdminName_Success
 * @tc.desc: Test GetAdminName function.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestGetAdminName_Success, TestSize.Level1)
{
    policyChangedEvent_->SetPolicyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    std::string adminName = policyChangedEvent_->GetAdminName();
    EXPECT_EQ(adminName, TEST_ADMIN_NAME);
}

/**
 * @tc.name: TestGetTimestamp_Success
 * @tc.desc: Test GetTimestamp function.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestGetTimestamp_Success, TestSize.Level1)
{
    policyChangedEvent_->SetPolicyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    int64_t timestamp = policyChangedEvent_->GetTimestamp();
    EXPECT_EQ(timestamp, TEST_TIMESTAMP);
}

/**
 * @tc.name: TestMarshalling_Success
 * @tc.desc: Test Marshalling function with valid data.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestMarshalling_Success, TestSize.Level1)
{
    PolicyChangedEvent event(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    MessageParcel parcel;
    bool result = event.Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: TestMarshalling_EmptyStrings
 * @tc.desc: Test Marshalling function with empty strings.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestMarshalling_EmptyStrings, TestSize.Level1)
{
    PolicyChangedEvent event("", "", "", 0);
    MessageParcel parcel;
    bool result = event.Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: TestUnmarshalling_Success
 * @tc.desc: Test Unmarshalling function with valid data.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestUnmarshalling_Success, TestSize.Level1)
{
    PolicyChangedEvent originalEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    MessageParcel parcel;
    originalEvent.Marshalling(parcel);
    parcel.RewindRead(0);

    PolicyChangedEvent unmarshalledEvent;
    bool result = PolicyChangedEvent::Unmarshalling(parcel, unmarshalledEvent);
    EXPECT_TRUE(result);
    EXPECT_EQ(unmarshalledEvent.GetInterfaceName(), TEST_INTERFACE_NAME);
    EXPECT_EQ(unmarshalledEvent.GetAdminName(), TEST_ADMIN_NAME);
    EXPECT_EQ(unmarshalledEvent.GetParameters(), TEST_PARAMETERS);
    EXPECT_EQ(unmarshalledEvent.GetTimestamp(), TEST_TIMESTAMP);
}

/**
 * @tc.name: TestUnmarshalling_EmptyStrings
 * @tc.desc: Test Unmarshalling function with empty strings.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestUnmarshalling_EmptyStrings, TestSize.Level1)
{
    PolicyChangedEvent originalEvent("", "", "", 0);
    MessageParcel parcel;
    originalEvent.Marshalling(parcel);
    parcel.RewindRead(0);

    PolicyChangedEvent unmarshalledEvent;
    bool result = PolicyChangedEvent::Unmarshalling(parcel, unmarshalledEvent);
    EXPECT_TRUE(result);
    EXPECT_EQ(unmarshalledEvent.GetInterfaceName(), "");
    EXPECT_EQ(unmarshalledEvent.GetAdminName(), "");
    EXPECT_EQ(unmarshalledEvent.GetParameters(), "");
    EXPECT_EQ(unmarshalledEvent.GetTimestamp(), 0);
}

/**
 * @tc.name: TestMarshallingAndUnmarshalling_LargeTimestamp
 * @tc.desc: Test Marshalling and Unmarshalling with large timestamp value.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestMarshallingAndUnmarshalling_LargeTimestamp, TestSize.Level1)
{
    int64_t largeTimestamp = 9223372036854775807LL; // INT64_MAX
    PolicyChangedEvent originalEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, largeTimestamp);
    MessageParcel parcel;
    originalEvent.Marshalling(parcel);
    parcel.RewindRead(0);

    PolicyChangedEvent unmarshalledEvent;
    bool result = PolicyChangedEvent::Unmarshalling(parcel, unmarshalledEvent);
    EXPECT_TRUE(result);
    EXPECT_EQ(unmarshalledEvent.GetTimestamp(), largeTimestamp);
}

/**
 * @tc.name: TestMarshallingAndUnmarshalling_LongStrings
 * @tc.desc: Test Marshalling and Unmarshalling with long strings.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestMarshallingAndUnmarshalling_LongStrings, TestSize.Level1)
{
    std::string longInterfaceName = "this_is_a_very_long_interface_name_for_testing_purpose";
    std::string longAdminName = "this_is_a_very_long_admin_name_for_testing_purpose_com.edm.test.demo";
    std::string longParameters = "{\"key\":\"this_is_a_very_long_parameters_value_for_testing\"}";
    PolicyChangedEvent originalEvent(longInterfaceName, longAdminName, longParameters, TEST_TIMESTAMP);
    MessageParcel parcel;
    originalEvent.Marshalling(parcel);
    parcel.RewindRead(0);

    PolicyChangedEvent unmarshalledEvent;
    bool result = PolicyChangedEvent::Unmarshalling(parcel, unmarshalledEvent);
    EXPECT_TRUE(result);
    EXPECT_EQ(unmarshalledEvent.GetInterfaceName(), longInterfaceName);
    EXPECT_EQ(unmarshalledEvent.GetAdminName(), longAdminName);
    EXPECT_EQ(unmarshalledEvent.GetParameters(), longParameters);
}

/**
 * @tc.name: TestSetAndGetMultipleTimes
 * @tc.desc: Test SetPolicyChangedEvent and Get methods multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(PolicyChangedEventTest, TestSetAndGetMultipleTimes, TestSize.Level1)
{
    // First set
    policyChangedEvent_->SetPolicyChangedEvent("interface1", "admin1", "{}", 1000);
    EXPECT_EQ(policyChangedEvent_->GetInterfaceName(), "interface1");
    EXPECT_EQ(policyChangedEvent_->GetAdminName(), "admin1");
    EXPECT_EQ(policyChangedEvent_->GetParameters(), "{}");
    EXPECT_EQ(policyChangedEvent_->GetTimestamp(), 1000);

    // Second set - overwrite
    policyChangedEvent_->SetPolicyChangedEvent("interface2", "admin2", "{\"key\":\"value\"}", 2000);
    EXPECT_EQ(policyChangedEvent_->GetInterfaceName(), "interface2");
    EXPECT_EQ(policyChangedEvent_->GetAdminName(), "admin2");
    EXPECT_EQ(policyChangedEvent_->GetParameters(), "{\"key\":\"value\"}");
    EXPECT_EQ(policyChangedEvent_->GetTimestamp(), 2000);

    // Third set - overwrite
    policyChangedEvent_->SetPolicyChangedEvent(TEST_INTERFACE_NAME, TEST_ADMIN_NAME, TEST_PARAMETERS, TEST_TIMESTAMP);
    EXPECT_EQ(policyChangedEvent_->GetInterfaceName(), TEST_INTERFACE_NAME);
    EXPECT_EQ(policyChangedEvent_->GetAdminName(), TEST_ADMIN_NAME);
    EXPECT_EQ(policyChangedEvent_->GetParameters(), TEST_PARAMETERS);
    EXPECT_EQ(policyChangedEvent_->GetTimestamp(), TEST_TIMESTAMP);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS