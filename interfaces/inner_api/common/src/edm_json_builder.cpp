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

#include "edm_json_builder.h"

namespace OHOS {
namespace EDM {
EdmJsonBuilder::EdmJsonBuilder() : root_(cJSON_CreateObject()) {}

EdmJsonBuilder::~EdmJsonBuilder()
{
    if (root_) {
        cJSON_Delete(root_);
        root_ = nullptr;
    }
}

EdmJsonBuilder& EdmJsonBuilder::Add(const std::string& key, const std::string& value)
{
    if (root_ != nullptr) {
        cJSON_AddStringToObject(root_, key.c_str(), value.c_str());
    }
    return *this;
}

EdmJsonBuilder& EdmJsonBuilder::Add(const std::string& key, int32_t value)
{
    if (root_ != nullptr) {
        cJSON_AddNumberToObject(root_, key.c_str(), value);
    }
    return *this;
}

EdmJsonBuilder& EdmJsonBuilder::Add(const std::string& key, uint32_t value)
{
    if (root_ != nullptr) {
        cJSON_AddNumberToObject(root_, key.c_str(), value);
    }
    return *this;
}

EdmJsonBuilder& EdmJsonBuilder::Add(const std::string& key, int64_t value)
{
    if (root_ != nullptr) {
        cJSON_AddNumberToObject(root_, key.c_str(), static_cast<double>(value));
    }
    return *this;
}

EdmJsonBuilder& EdmJsonBuilder::Add(const std::string& key, const std::vector<std::string>& list)
{
    if (root_ == nullptr) {
        return *this;
    }
    cJSON* arr = cJSON_CreateArray();
    if (arr == nullptr) {
        return *this;
    }
    for (const auto& item : list) {
        cJSON_AddItemToArray(arr, cJSON_CreateString(item.c_str()));
    }
    cJSON_AddItemToObject(root_, key.c_str(), arr);
    return *this;
}

EdmJsonBuilder& EdmJsonBuilder::Add(const std::string& key, const std::map<std::string, std::string>& map)
{
    if (root_ == nullptr) {
        return *this;
    }
    cJSON* obj = cJSON_CreateObject();
    if (obj == nullptr) {
        return *this;
    }
    for (const auto& pair : map) {
        cJSON_AddStringToObject(obj, pair.first.c_str(), pair.second.c_str());
    }
    cJSON_AddItemToObject(root_, key.c_str(), obj);
    return *this;
}

EdmJsonBuilder& EdmJsonBuilder::AddRawJson(const std::string& key, const std::string& jsonStr)
{
    if (root_ == nullptr) {
        return *this;
    }
    cJSON* obj = cJSON_Parse(jsonStr.c_str());
    if (obj != nullptr) {
        cJSON_AddItemToObject(root_, key.c_str(), obj);
    }
    return *this;
}

std::string EdmJsonBuilder::Build()
{
    if (root_ == nullptr) {
        return "";
    }
    char* jsonStr = cJSON_PrintUnformatted(root_);
    if (jsonStr == nullptr) {
        return "";
    }
    std::string result(jsonStr);
    cJSON_free(jsonStr);
    return result;
}
} // namespace EDM
} // namespace OHOS
