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
#include <vector>
#include <map>

#include "cJSON.h"

#include "edm_json_builder.h"

using namespace testing::ext;

namespace OHOS {
namespace EDM {
namespace TEST {
class EdmJsonBuilderTest : public testing::Test {
protected:
    void SetUp() override
    {
        jsonBuilder_ = std::make_shared<EdmJsonBuilder>();
    }

    void TearDown() override
    {
        jsonBuilder_.reset();
    }

    std::shared_ptr<EdmJsonBuilder> jsonBuilder_;
};

/**
 * @tc.name: TestAddString_Success
 * @tc.desc: Test EdmJsonBuilder::Add with string value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddString_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("key", "value").Build();
    EXPECT_TRUE(result.find("\"key\":\"value\"") != std::string::npos);
}

/**
 * @tc.name: TestAddString_EmptyValue_Success
 * @tc.desc: Test EdmJsonBuilder::Add with empty string value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddString_EmptyValue_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("key", "").Build();
    EXPECT_TRUE(result.find("\"key\":\"\"") != std::string::npos);
}

/**
 * @tc.name: TestAddInt32_Success
 * @tc.desc: Test EdmJsonBuilder::Add with int32_t value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddInt32_Success, TestSize.Level1)
{
    int32_t value = 123;
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":123") != std::string::npos);
}

/**
 * @tc.name: TestAddInt32_NegativeValue_Success
 * @tc.desc: Test EdmJsonBuilder::Add with negative int32_t value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddInt32_NegativeValue_Success, TestSize.Level1)
{
    int32_t value = -456;
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":-456") != std::string::npos);
}

/**
 * @tc.name: TestAddInt32_ZeroValue_Success
 * @tc.desc: Test EdmJsonBuilder::Add with zero int32_t value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddInt32_ZeroValue_Success, TestSize.Level1)
{
    int32_t value = 0;
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":0") != std::string::npos);
}

/**
 * @tc.name: TestAddUint32_Success
 * @tc.desc: Test EdmJsonBuilder::Add with uint32_t value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddUint32_Success, TestSize.Level1)
{
    uint32_t value = 789;
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":789") != std::string::npos);
}

/**
 * @tc.name: TestAddInt64_Success
 * @tc.desc: Test EdmJsonBuilder::Add with int64_t value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddInt64_Success, TestSize.Level1)
{
    int64_t value = 1234567890123LL;
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":1234567890123") != std::string::npos);
}

/**
 * @tc.name: TestAddInt64_NegativeValue_Success
 * @tc.desc: Test EdmJsonBuilder::Add with negative int64_t value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddInt64_NegativeValue_Success, TestSize.Level1)
{
    int64_t value = -9876543210LL;
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":-9876543210") != std::string::npos);
}

/**
 * @tc.name: TestAddVector_Success
 * @tc.desc: Test EdmJsonBuilder::Add with vector of strings.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddVector_Success, TestSize.Level1)
{
    std::vector<std::string> list = {"item1", "item2", "item3"};
    std::string result = jsonBuilder_->Add("list", list).Build();
    EXPECT_TRUE(result.find("\"list\":[\"item1\",\"item2\",\"item3\"]") != std::string::npos);
}

/**
 * @tc.name: TestAddVector_EmptyVector_Success
 * @tc.desc: Test EdmJsonBuilder::Add with empty vector.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddVector_EmptyVector_Success, TestSize.Level1)
{
    std::vector<std::string> list = {};
    std::string result = jsonBuilder_->Add("list", list).Build();
    EXPECT_TRUE(result.find("\"list\":[]") != std::string::npos);
}

/**
 * @tc.name: TestAddVector_SingleItem_Success
 * @tc.desc: Test EdmJsonBuilder::Add with vector containing single item.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddVector_SingleItem_Success, TestSize.Level1)
{
    std::vector<std::string> list = {"single"};
    std::string result = jsonBuilder_->Add("list", list).Build();
    EXPECT_TRUE(result.find("\"list\":[\"single\"]") != std::string::npos);
}

/**
 * @tc.name: TestAddMap_Success
 * @tc.desc: Test EdmJsonBuilder::Add with map of strings.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddMap_Success, TestSize.Level1)
{
    std::map<std::string, std::string> map = {{ "key1", "value1" }, { "key2", "value2" }};
    std::string result = jsonBuilder_->Add("map", map).Build();
    EXPECT_TRUE(result.find("\"map\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"key1\":\"value1\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"key2\":\"value2\"") != std::string::npos);
}

/**
 * @tc.name: TestAddMap_EmptyMap_Success
 * @tc.desc: Test EdmJsonBuilder::Add with empty map.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddMap_EmptyMap_Success, TestSize.Level1)
{
    std::map<std::string, std::string> map = {};
    std::string result = jsonBuilder_->Add("map", map).Build();
    EXPECT_TRUE(result.find("\"map\":{}") != std::string::npos);
}

/**
 * @tc.name: TestBuild_Success
 * @tc.desc: Test EdmJsonBuilder::Build returns valid JSON string.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestBuild_Success, TestSize.Level1)
{
    jsonBuilder_->Add("key1", "value1").Add("key2", 123);
    std::string result = jsonBuilder_->Build();
    EXPECT_TRUE(!result.empty());
    EXPECT_TRUE(result.find("\"key1\":\"value1\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"key2\":123") != std::string::npos);
}

/**
 * @tc.name: TestBuild_EmptyBuilder_Success
 * @tc.desc: Test EdmJsonBuilder::Build with empty builder returns empty object.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestBuild_EmptyBuilder_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Build();
    EXPECT_TRUE(!result.empty());
    EXPECT_EQ(result, "{}");
}

/**
 * @tc.name: TestChainedAdd_Success
 * @tc.desc: Test chained Add operations.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestChainedAdd_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("str", "value")
        .Add("int32", 123)
        .Add("uint32", 456U)
        .Add("int64", static_cast<int64_t>(789012345))
        .Build();
    EXPECT_TRUE(result.find("\"str\":\"value\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"int32\":123") != std::string::npos);
    EXPECT_TRUE(result.find("\"uint32\":456") != std::string::npos);
    EXPECT_TRUE(result.find("\"int64\":789012345") != std::string::npos);
}

/**
 * @tc.name: TestComplexJson_Success
 * @tc.desc: Test building complex JSON structure.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestComplexJson_Success, TestSize.Level1)
{
    std::vector<std::string> list = {"a", "b", "c"};
    std::map<std::string, std::string> map = {{ "m1", "v1" }, { "m2", "v2" }};

    std::string result = jsonBuilder_->Add("string", "test")
        .Add("number", 42)
        .Add("list", list)
        .Add("map", map)
        .Build();

    EXPECT_TRUE(result.find("\"string\":\"test\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"number\":42") != std::string::npos);
    EXPECT_TRUE(result.find("\"list\":[\"a\",\"b\",\"c\"]") != std::string::npos);
    EXPECT_TRUE(result.find("\"map\":{") != std::string::npos);
}

/**
 * @tc.name: TestAddWithSpecialCharacters_Success
 * @tc.desc: Test EdmJsonBuilder::Add with special characters in string.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddWithSpecialCharacters_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("key", "value with spaces").Build();
    EXPECT_TRUE(result.find("\"key\":\"value with spaces\"") != std::string::npos);
}

/**
 * @tc.name: TestAddWithUnicodeCharacters_Success
 * @tc.desc: Test EdmJsonBuilder::Add with unicode characters in string.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddWithUnicodeCharacters_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("key", "中文测试").Build();
    EXPECT_TRUE(result.find("\"key\":\"中文测试\"") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_ValidJsonString_Success
 * @tc.desc: Test EdmJsonBuilder::AddRawJson with valid JSON string.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_ValidJsonString_Success, TestSize.Level1)
{
    std::string jsonStr = R"({"innerKey":"innerValue"})";
    std::string result = jsonBuilder_->AddRawJson("rawKey", jsonStr).Build();
    EXPECT_TRUE(result.find("\"rawKey\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"innerKey\":\"innerValue\"") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_EmptyJsonString_Success
 * @tc.desc: Test EdmJsonBuilder::AddRawJson with empty JSON string.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_EmptyJsonString_Success, TestSize.Level1)
{
    std::string jsonStr = "";
    std::string result = jsonBuilder_->AddRawJson("rawKey", jsonStr).Build();
    // Empty string is invalid JSON, should not add the key
    EXPECT_TRUE(result.find("\"rawKey\"") == std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_NestedJsonObject_Success
 * @tc.desc: Test EdmJsonBuilder::AddRawJson with nested JSON object.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_NestedJsonObject_Success, TestSize.Level1)
{
    std::string jsonStr = R"({"level1":{"level2":"value"}})";
    std::string result = jsonBuilder_->AddRawJson("nested", jsonStr).Build();
    EXPECT_TRUE(result.find("\"nested\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"level1\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"level2\":\"value\"") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_JsonArrayString_Success
 * @tc.desc: Test EdmJsonBuilder::AddRawJson with JSON array string.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_JsonArrayString_Success, TestSize.Level1)
{
    std::string jsonStr = R"(["item1","item2","item3"])";
    std::string result = jsonBuilder_->AddRawJson("arrayKey", jsonStr).Build();
    EXPECT_TRUE(result.find("\"arrayKey\":[") != std::string::npos);
    EXPECT_TRUE(result.find("\"item1\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"item2\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"item3\"") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_InvalidJsonString_Success
 * @tc.desc: Test EdmJsonBuilder::AddRawJson with invalid JSON string.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_InvalidJsonString_Success, TestSize.Level1)
{
    std::string jsonStr = "not a valid json";
    std::string result = jsonBuilder_->AddRawJson("invalidKey", jsonStr).Build();
    // Invalid JSON should not be added
    EXPECT_TRUE(result.find("\"invalidKey\"") == std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_NoDoubleEscape_Success
 * @tc.desc: Test AddRawJson does not double escape compared to Add method.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_NoDoubleEscape_Success, TestSize.Level1)
{
    // Using Add with a JSON string will escape the quotes
    std::string jsonStr = R"({"key":"value"})";
    
    // AddRawJson should parse and add the JSON object directly
    std::string resultRaw = jsonBuilder_->AddRawJson("rawJson", jsonStr).Build();
    EXPECT_TRUE(resultRaw.find("\"rawJson\":{") != std::string::npos);
    EXPECT_TRUE(resultRaw.find("\"key\":\"value\"") != std::string::npos);
    // Should NOT contain escaped quotes like \"key\":\"value\"
    EXPECT_TRUE(resultRaw.find(R"(\"key\")") == std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_CompareWithAdd_Success
 * @tc.desc: Test AddRawJson vs Add method behavior difference.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_CompareWithAdd_Success, TestSize.Level1)
{
    std::string jsonStr = R"({"innerKey":"innerValue"})";
    
    // Create two builders for comparison
    auto builderAdd = std::make_shared<EdmJsonBuilder>();
    auto builderRaw = std::make_shared<EdmJsonBuilder>();
    
    // Using Add: the jsonStr will be treated as a plain string and escaped
    std::string resultAdd = builderAdd->Add("data", jsonStr).Build();
    
    // Using AddRawJson: the jsonStr will be parsed as JSON object
    std::string resultRaw = builderRaw->AddRawJson("data", jsonStr).Build();
    
    // Add should escape the content as a string value
    EXPECT_TRUE(resultAdd.find("\"data\":\"{") != std::string::npos);
    
    // AddRawJson should parse it as a JSON object
    EXPECT_TRUE(resultRaw.find("\"data\":{") != std::string::npos);
    EXPECT_TRUE(resultRaw.find("\"innerKey\":\"innerValue\"") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_ChainedCalls_Success
 * @tc.desc: Test AddRawJson with chained method calls.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_ChainedCalls_Success, TestSize.Level1)
{
    std::string jsonObj = R"({"name":"test"})";
    std::string jsonArray = R"([1,2,3])";
    
    std::string result = jsonBuilder_->Add("id", 123)
        .AddRawJson("obj", jsonObj)
        .AddRawJson("arr", jsonArray)
        .Build();
    
    EXPECT_TRUE(result.find("\"id\":123") != std::string::npos);
    EXPECT_TRUE(result.find("\"obj\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"name\":\"test\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"arr\":[1,2,3]") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_ComplexNestedJson_Success
 * @tc.desc: Test AddRawJson with complex nested JSON structure.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_ComplexNestedJson_Success, TestSize.Level1)
{
    std::string complexJson = R"({
        "user": {
            "name": "John",
            "age": 30,
            "hobbies": ["reading", "coding"]
        },
        "active": true
    })";
    
    std::string result = jsonBuilder_->AddRawJson("data", complexJson).Build();
    EXPECT_TRUE(result.find("\"data\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"user\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"name\":\"John\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"hobbies\":[") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_EmptyObject_Success
 * @tc.desc: Test AddRawJson with empty JSON object.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_EmptyObject_Success, TestSize.Level1)
{
    std::string jsonStr = "{}";
    std::string result = jsonBuilder_->AddRawJson("emptyObj", jsonStr).Build();
    EXPECT_TRUE(result.find("\"emptyObj\":{}") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_EmptyArray_Success
 * @tc.desc: Test AddRawJson with empty JSON array.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_EmptyArray_Success, TestSize.Level1)
{
    std::string jsonStr = "[]";
    std::string result = jsonBuilder_->AddRawJson("emptyArr", jsonStr).Build();
    EXPECT_TRUE(result.find("\"emptyArr\":[]") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_NullValue_Success
 * @tc.desc: Test AddRawJson with JSON containing null value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_NullValue_Success, TestSize.Level1)
{
    std::string jsonStr = R"({"key":null})";
    std::string result = jsonBuilder_->AddRawJson("nullObj", jsonStr).Build();
    EXPECT_TRUE(result.find("\"nullObj\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"key\":null") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_NumericValues_Success
 * @tc.desc: Test AddRawJson with JSON containing numeric values.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_NumericValues_Success, TestSize.Level1)
{
    std::string jsonStr = R"({"int":42,"float":3.14,"negative":-100})";
    std::string result = jsonBuilder_->AddRawJson("numbers", jsonStr).Build();
    EXPECT_TRUE(result.find("\"numbers\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"int\":42") != std::string::npos);
    EXPECT_TRUE(result.find("\"float\":3.14") != std::string::npos);
    EXPECT_TRUE(result.find("\"negative\":-100") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_BooleanValues_Success
 * @tc.desc: Test AddRawJson with JSON containing boolean values.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_BooleanValues_Success, TestSize.Level1)
{
    std::string jsonStr = R"({"enabled":true,"disabled":false})";
    std::string result = jsonBuilder_->AddRawJson("bools", jsonStr).Build();
    EXPECT_TRUE(result.find("\"bools\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"enabled\":true") != std::string::npos);
    EXPECT_TRUE(result.find("\"disabled\":false") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_WithUnicodeCharacters_Success
 * @tc.desc: Test AddRawJson with JSON containing unicode characters.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_WithUnicodeCharacters_Success, TestSize.Level1)
{
    std::string jsonStr = R"({"name":"中文测试","emoji":"😀"})";
    std::string result = jsonBuilder_->AddRawJson("unicode", jsonStr).Build();
    EXPECT_TRUE(result.find("\"unicode\":{") != std::string::npos);
    EXPECT_TRUE(result.find("\"name\":\"中文测试\"") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_MalformedJson_Success
 * @tc.desc: Test AddRawJson with malformed JSON string.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_MalformedJson_Success, TestSize.Level1)
{
    // Missing closing brace
    std::string jsonStr = R"({"key":"value")";
    std::string result = jsonBuilder_->AddRawJson("malformed", jsonStr).Build();
    // Malformed JSON should not be added
    EXPECT_TRUE(result.find("\"malformed\"") == std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_OnlyKeyNoValue_Success
 * @tc.desc: Test AddRawJson with JSON string having only key.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_OnlyKeyNoValue_Success, TestSize.Level1)
{
    std::string jsonStr = R"({"key":})";
    std::string result = jsonBuilder_->AddRawJson("invalid", jsonStr).Build();
    // Invalid JSON should not be added
    EXPECT_TRUE(result.find("\"invalid\"") == std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_MultipleRawJson_Success
 * @tc.desc: Test multiple AddRawJson calls in sequence.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_MultipleRawJson_Success, TestSize.Level1)
{
    std::string json1 = R"({"a":1})";
    std::string json2 = R"({"b":2})";
    std::string json3 = R"([1,2,3])";
    
    std::string result = jsonBuilder_->AddRawJson("obj1", json1)
        .AddRawJson("obj2", json2)
        .AddRawJson("arr", json3)
        .Build();
    
    EXPECT_TRUE(result.find("\"obj1\":{\"a\":1}") != std::string::npos);
    EXPECT_TRUE(result.find("\"obj2\":{\"b\":2}") != std::string::npos);
    EXPECT_TRUE(result.find("\"arr\":[1,2,3]") != std::string::npos);
}

/**
 * @tc.name: TestAddRawJson_MixedWithAdd_Success
 * @tc.desc: Test AddRawJson mixed with Add method calls.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddRawJson_MixedWithAdd_Success, TestSize.Level1)
{
    std::string jsonObj = R"({"nested":"value"})";
    std::vector<std::string> list = {"x", "y", "z"};
    
    std::string result = jsonBuilder_->Add("name", "test")
        .Add("count", 100)
        .AddRawJson("metadata", jsonObj)
        .Add("list", list)
        .Build();
    
    EXPECT_TRUE(result.find("\"name\":\"test\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"count\":100") != std::string::npos);
    EXPECT_TRUE(result.find("\"metadata\":{\"nested\":\"value\"}") != std::string::npos);
    EXPECT_TRUE(result.find("\"list\":[\"x\",\"y\",\"z\"]") != std::string::npos);
}

/**
 * @tc.name: TestAddConstCharPtr_Success
 * @tc.desc: Test EdmJsonBuilder::Add with const char* value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddConstCharPtr_Success, TestSize.Level1)
{
    const char* value = "test_value";
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":\"test_value\"") != std::string::npos);
}

/**
 * @tc.name: TestAddConstCharPtr_EmptyValue_Success
 * @tc.desc: Test EdmJsonBuilder::Add with empty const char* value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddConstCharPtr_EmptyValue_Success, TestSize.Level1)
{
    const char* value = "";
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":\"\"") != std::string::npos);
}

/**
 * @tc.name: TestAddConstCharPtr_Nullptr_Success
 * @tc.desc: Test EdmJsonBuilder::Add with nullptr const char* value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddConstCharPtr_Nullptr_Success, TestSize.Level1)
{
    const char* value = nullptr;
    std::string result = jsonBuilder_->Add("key", value).Build();
    // nullptr should not add the key to JSON
    EXPECT_TRUE(result.find("\"key\"") == std::string::npos);
}

/**
 * @tc.name: TestAddConstCharPtr_ChainedCalls_Success
 * @tc.desc: Test EdmJsonBuilder::Add const char* with chained calls.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddConstCharPtr_ChainedCalls_Success, TestSize.Level1)
{
    const char* value1 = "first";
    const char* value2 = "second";
    const char* value3 = "third";
    
    std::string result = jsonBuilder_->Add("key1", value1)
        .Add("key2", value2)
        .Add("key3", value3)
        .Build();
    
    EXPECT_TRUE(result.find("\"key1\":\"first\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"key2\":\"second\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"key3\":\"third\"") != std::string::npos);
}

/**
 * @tc.name: TestAddConstCharPtr_MixedWithOtherTypes_Success
 * @tc.desc: Test EdmJsonBuilder::Add const char* mixed with other types.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddConstCharPtr_MixedWithOtherTypes_Success, TestSize.Level1)
{
    const char* charValue = "char_ptr_value";
    std::string stringValue = "string_value";
    
    std::string result = jsonBuilder_->Add("charKey", charValue)
        .Add("stringKey", stringValue)
        .Add("intKey", 123)
        .Build();
    
    EXPECT_TRUE(result.find("\"charKey\":\"char_ptr_value\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"stringKey\":\"string_value\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"intKey\":123") != std::string::npos);
}

/**
 * @tc.name: TestAddConstCharPtr_WithSpecialCharacters_Success
 * @tc.desc: Test EdmJsonBuilder::Add const char* with special characters.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddConstCharPtr_WithSpecialCharacters_Success, TestSize.Level1)
{
    const char* value = "value with spaces and symbols!@#$%";
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":\"value with spaces and symbols!@#$%\"") != std::string::npos);
}

/**
 * @tc.name: TestAddConstCharPtr_WithUnicodeCharacters_Success
 * @tc.desc: Test EdmJsonBuilder::Add const char* with unicode characters.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddConstCharPtr_WithUnicodeCharacters_Success, TestSize.Level1)
{
    const char* value = "中文测试";
    std::string result = jsonBuilder_->Add("key", value).Build();
    EXPECT_TRUE(result.find("\"key\":\"中文测试\"") != std::string::npos);
}

/**
 * @tc.name: TestAddBool_True_Success
 * @tc.desc: Test EdmJsonBuilder::Add with true bool value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddBool_True_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("enabled", true).Build();
    EXPECT_TRUE(result.find("\"enabled\":true") != std::string::npos);
}

/**
 * @tc.name: TestAddBool_False_Success
 * @tc.desc: Test EdmJsonBuilder::Add with false bool value.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddBool_False_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("disabled", false).Build();
    EXPECT_TRUE(result.find("\"disabled\":false") != std::string::npos);
}

/**
 * @tc.name: TestAddBool_ChainedCalls_Success
 * @tc.desc: Test EdmJsonBuilder::Add bool with chained calls.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddBool_ChainedCalls_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("flag1", true)
        .Add("flag2", false)
        .Add("flag3", true)
        .Build();
    
    EXPECT_TRUE(result.find("\"flag1\":true") != std::string::npos);
    EXPECT_TRUE(result.find("\"flag2\":false") != std::string::npos);
    EXPECT_TRUE(result.find("\"flag3\":true") != std::string::npos);
}

/**
 * @tc.name: TestAddBool_MixedWithOtherTypes_Success
 * @tc.desc: Test EdmJsonBuilder::Add bool mixed with other types.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddBool_MixedWithOtherTypes_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("name", "test")
        .Add("count", 42)
        .Add("active", true)
        .Add("deleted", false)
        .Build();
    
    EXPECT_TRUE(result.find("\"name\":\"test\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"count\":42") != std::string::npos);
    EXPECT_TRUE(result.find("\"active\":true") != std::string::npos);
    EXPECT_TRUE(result.find("\"deleted\":false") != std::string::npos);
}

/**
 * @tc.name: TestAddBool_ComplexJson_Success
 * @tc.desc: Test EdmJsonBuilder::Add bool in complex JSON structure.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddBool_ComplexJson_Success, TestSize.Level1)
{
    std::vector<std::string> list = {"item1", "item2"};
    std::map<std::string, std::string> map = {{"k1", "v1"}};
    
    std::string result = jsonBuilder_->Add("string", "value")
        .Add("number", 100)
        .Add("boolTrue", true)
        .Add("boolFalse", false)
        .Add("list", list)
        .Add("map", map)
        .Build();
    
    EXPECT_TRUE(result.find("\"string\":\"value\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"number\":100") != std::string::npos);
    EXPECT_TRUE(result.find("\"boolTrue\":true") != std::string::npos);
    EXPECT_TRUE(result.find("\"boolFalse\":false") != std::string::npos);
    EXPECT_TRUE(result.find("\"list\":[\"item1\",\"item2\"]") != std::string::npos);
    EXPECT_TRUE(result.find("\"map\":{") != std::string::npos);
}

/**
 * @tc.name: TestAddBool_MultipleTrueValues_Success
 * @tc.desc: Test EdmJsonBuilder::Add with multiple true bool values.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddBool_MultipleTrueValues_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("enabled1", true)
        .Add("enabled2", true)
        .Add("enabled3", true)
        .Build();
    
    EXPECT_TRUE(result.find("\"enabled1\":true") != std::string::npos);
    EXPECT_TRUE(result.find("\"enabled2\":true") != std::string::npos);
    EXPECT_TRUE(result.find("\"enabled3\":true") != std::string::npos);
}

/**
 * @tc.name: TestAddBool_MultipleFalseValues_Success
 * @tc.desc: Test EdmJsonBuilder::Add with multiple false bool values.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddBool_MultipleFalseValues_Success, TestSize.Level1)
{
    std::string result = jsonBuilder_->Add("disabled1", false)
        .Add("disabled2", false)
        .Add("disabled3", false)
        .Build();
    
    EXPECT_TRUE(result.find("\"disabled1\":false") != std::string::npos);
    EXPECT_TRUE(result.find("\"disabled2\":false") != std::string::npos);
    EXPECT_TRUE(result.find("\"disabled3\":false") != std::string::npos);
}

/**
 * @tc.name: TestAddBool_WithConstCharPtr_Success
 * @tc.desc: Test EdmJsonBuilder::Add bool mixed with const char*.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddBool_WithConstCharPtr_Success, TestSize.Level1)
{
    const char* name = "test_item";
    
    std::string result = jsonBuilder_->Add("name", name)
        .Add("active", true)
        .Add("visible", false)
        .Build();
    
    EXPECT_TRUE(result.find("\"name\":\"test_item\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"active\":true") != std::string::npos);
    EXPECT_TRUE(result.find("\"visible\":false") != std::string::npos);
}

/**
 * @tc.name: TestAddBool_AllTypesChained_Success
 * @tc.desc: Test EdmJsonBuilder::Add all types in chained calls.
 * @tc.type: FUNC
 */
HWTEST_F(EdmJsonBuilderTest, TestAddBool_AllTypesChained_Success, TestSize.Level1)
{
    const char* charPtr = "char_value";
    std::string str = "string_value";
    std::vector<std::string> list = {"a", "b"};
    std::map<std::string, std::string> map = {{"mk", "mv"}};
    
    std::string result = jsonBuilder_->Add("charPtr", charPtr)
        .Add("string", str)
        .Add("int32", 123)
        .Add("uint32", 456U)
        .Add("int64", static_cast<int64_t>(789012))
        .Add("boolTrue", true)
        .Add("boolFalse", false)
        .Add("list", list)
        .Add("map", map)
        .Build();
    
    EXPECT_TRUE(result.find("\"charPtr\":\"char_value\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"string\":\"string_value\"") != std::string::npos);
    EXPECT_TRUE(result.find("\"int32\":123") != std::string::npos);
    EXPECT_TRUE(result.find("\"uint32\":456") != std::string::npos);
    EXPECT_TRUE(result.find("\"int64\":789012") != std::string::npos);
    EXPECT_TRUE(result.find("\"boolTrue\":true") != std::string::npos);
    EXPECT_TRUE(result.find("\"boolFalse\":false") != std::string::npos);
    EXPECT_TRUE(result.find("\"list\":[\"a\",\"b\"]") != std::string::npos);
    EXPECT_TRUE(result.find("\"map\":{") != std::string::npos);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS