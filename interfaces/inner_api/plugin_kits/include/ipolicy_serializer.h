/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef SERVICES_EDM_INCLUDE_EDM_IPOLICY_SERIALIZER_H
#define SERVICES_EDM_INCLUDE_EDM_IPOLICY_SERIALIZER_H

#include <algorithm>
#include <message_parcel.h>
#include <set>
#include <string>
#include <string_ex.h>

#ifndef JSON_NOEXCEPTION
#define JSON_NOEXCEPTION
#endif
#include "cJSON.h"
#include "cjson_check.h"
#include "singleton.h"

#include "edm_constants.h"
#include "edm_log.h"

namespace OHOS {
namespace EDM {
/*
 * Policy data serialize interface
 *
 * @tparam DT policy data type,like vector,map,int...
 */
template<class DT>
class IPolicySerializer {
public:
    /*
     * Deserialize a JSON string into a DT object.
     *
     * @param jsonString JSON string
     * @param dataObj DT object
     * @return true indicates that the operation is successful.
     */
    virtual bool Deserialize(const std::string &jsonString, DT &dataObj) = 0;

    /*
     * Serializes a DT object into a JSON string.
     *
     * @param dataObj DT object
     * @param jsonString JSON string
     * @return true indicates that the operation is successful.
     */
    virtual bool Serialize(const DT &dataObj, std::string &jsonString) = 0;

    /*
     * Obtains DT object data from the MessageParcel object.
     *
     * @param reply MessageParcel
     * @param result DT object
     * @return true indicates that the operation is successful.
     */
    virtual bool GetPolicy(MessageParcel &data, DT &result) = 0;

    /*
     * Write DT object data to MessageParcel
     *
     * @param reply MessageParcel
     * @param result DT object
     * @return true indicates that the operation is successful.
     */
    virtual bool WritePolicy(MessageParcel &reply, DT &result) = 0;

    /*
     * Obtain the final data from all DT data set by the admin.
     *
     * @param adminValuesArray DT data collection
     * @param result The end result
     * @return true indicates that the operation is successful.
     */
    virtual bool MergePolicy(std::vector<DT> &adminValuesArray, DT &result) = 0;

    virtual ~IPolicySerializer() = default;
};

/*
 * Policy data serialize interface
 *
 * @tparam DT policy data type in vector.
 * @tparam T_ARRAY policy data type,like vector<string>,vector<map>...
 */
template<typename DT, typename T_ARRAY = std::vector<DT>>
class ArraySerializer : public IPolicySerializer<T_ARRAY> {
public:
    bool Deserialize(const std::string &jsonString, T_ARRAY &dataObj) override;

    bool Serialize(const T_ARRAY &dataObj, std::string &jsonString) override;

    bool GetPolicy(MessageParcel &data, T_ARRAY &result) override;

    bool WritePolicy(MessageParcel &reply, T_ARRAY &result) override;

    bool MergePolicy(std::vector<T_ARRAY> &data, T_ARRAY &result) override;

    virtual void Deduplication(T_ARRAY &dataObj);

protected:
    std::shared_ptr<IPolicySerializer<DT>> serializerInner_;
};

template<typename DT, typename T_ARRAY>
bool ArraySerializer<DT, T_ARRAY>::Deserialize(const std::string &jsonString, T_ARRAY &dataObj)
{
    if (jsonString.empty()) {
        return true;
    }

    cJSON *root = cJSON_Parse(jsonString.c_str());
    if (root == nullptr) {
        EDMLOGE("ArraySerializer Deserialize json to vector error");
        return false;
    }

    if (!cJSON_IsArray(root)) {
        EDMLOGE("Input JSON is not an array.");
        cJSON_Delete(root);
        return false;
    }

    const int array_size = cJSON_GetArraySize(root);
    if (array_size > EdmConstants::DEFAULT_LOOP_MAX_SIZE) {
        EDMLOGE("Array size exceeds maximum limit.");
        cJSON_Delete(root);
        return false;
    }

    dataObj = std::vector<DT>(array_size);
    for (int i = 0; i < array_size; ++i) {
        cJSON *item = cJSON_GetArrayItem(root, i);
        if (item == nullptr) {
            EDMLOGE("Failed to get array item");
            cJSON_Delete(root);
            return false;
        }

        char *itemJson = cJSON_PrintUnformatted(item);
        if (itemJson == nullptr) {
            EDMLOGE("Failed to serialize JSON item at index");
            cJSON_Delete(root);
            return false;
        }

        DT value;
        if (!serializerInner_->Deserialize(itemJson, value)) {
            cJSON_free(itemJson);
            cJSON_Delete(root);
            return false;
        }

        dataObj.at(i) = value;
        cJSON_free(itemJson);
    }

    cJSON_Delete(root);
    return true;
}

template<typename DT, typename T_ARRAY>
bool ArraySerializer<DT, T_ARRAY>::Serialize(const T_ARRAY &dataObj, std::string &jsonString)
{
    if (dataObj.empty()) {
        jsonString = "";
        return true;
    }

    cJSON *arrayData = nullptr;
    CJSON_CREATE_ARRAY_AND_CHECK(arrayData, false);

    for (std::uint32_t i = 0; i < dataObj.size(); ++i) {
        std::string itemJson;
        DT item = dataObj.at(i);
        if (!serializerInner_->Serialize(item, itemJson)) {
            cJSON_Delete(arrayData);
            return false;
        }
        
        cJSON *itemJsonObj = cJSON_CreateString(itemJson.c_str());
        if (itemJsonObj == nullptr) {
            cJSON_Delete(arrayData);
            return false;
        }
        CJSON_ADD_ITEM_TO_ARRAY_AND_CHECK_AND_CLEAR(itemJsonObj, arrayData, false);
    }

    char *jsonStr = cJSON_Print(arrayData);
    cJSON_Delete(arrayData);
    
    if (jsonStr == nullptr) {
        return false;
    }
    
    jsonString = jsonStr;
    cJSON_free(jsonStr);
    return true;
}

template<typename DT, typename T_ARRAY>
bool ArraySerializer<DT, T_ARRAY>::GetPolicy(MessageParcel &data, T_ARRAY &result)
{
    std::vector<std::string> readVector;
    if (!data.ReadStringVector(&readVector)) {
        return false;
    }
    // Data will be appended to result, and the original data of result will not be deleted.
    for (const auto &itemJson : readVector) {
        DT item;
        if (!itemJson.empty()) {
            if (!serializerInner_->Deserialize(itemJson, item)) {
                return false;
            }
            result.push_back(item);
        }
    }
    Deduplication(result);
    return true;
}

template<typename DT, typename T_ARRAY>
bool ArraySerializer<DT, T_ARRAY>::WritePolicy(MessageParcel &reply, T_ARRAY &result)
{
    std::vector<std::string> writeVector;
    for (const auto &item : result) {
        std::string itemJson;
        if (!serializerInner_->Serialize(item, itemJson)) {
            return false;
        }
        writeVector.push_back(itemJson);
    }
    return reply.WriteStringVector(writeVector);
}

template<typename DT, typename T_ARRAY>
bool ArraySerializer<DT, T_ARRAY>::MergePolicy(std::vector<T_ARRAY> &data, T_ARRAY &result)
{
    std::set<DT> stData;
    for (const auto &dataItem : data) {
        for (const auto &item : dataItem) {
            stData.insert(item);
        }
    }
    result.assign(stData.begin(), stData.end());
    return true;
}

template<typename DT, typename T_ARRAY>
void ArraySerializer<DT, T_ARRAY>::Deduplication(T_ARRAY &dataObj)
{
    std::sort(dataObj.begin(), dataObj.end());
    auto iter = std::unique(dataObj.begin(), dataObj.end());
    dataObj.erase(iter, dataObj.end());
}
} // namespace EDM
} // namespace OHOS

#endif // SERVICES_EDM_INCLUDE_EDM_IPOLICY_SERIALIZER_H
