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

#include "array_usb_device_id_serializer.h"
#include "edm_constants.h"
#include "json/json.h"
#include "usb_device_id.h"

namespace OHOS {
namespace EDM {

std::vector<UsbDeviceId> ArrayUsbDeviceIdSerializer::SetUnionPolicyData(std::vector<UsbDeviceId> &data,
    std::vector<UsbDeviceId> &currentData)
{
    std::vector<UsbDeviceId> mergeData;
    std::sort(data.begin(), data.end(), Comp());
    std::sort(currentData.begin(), currentData.end(), Comp());
    std::set_union(data.begin(), data.end(), currentData.begin(), currentData.end(), back_inserter(mergeData), Comp());
    return mergeData;
}

std::vector<UsbDeviceId> ArrayUsbDeviceIdSerializer::SetDifferencePolicyData(std::vector<UsbDeviceId> &data,
    std::vector<UsbDeviceId> &currentData)
{
    std::vector<UsbDeviceId> mergeData;
    std::sort(data.begin(), data.end(), Comp());
    std::sort(currentData.begin(), currentData.end(), Comp());
    std::set_difference(currentData.begin(), currentData.end(), data.begin(), data.end(), back_inserter(mergeData),
        Comp());
    return mergeData;
}

bool ArrayUsbDeviceIdSerializer::Deserialize(const std::string &jsonString, std::vector<UsbDeviceId> &dataObj)
{
    if (jsonString.empty()) {
        return true;
    }
    Json::Value root;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> charReader(builder.newCharReader());
    std::string err;
    if (!charReader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.length(), &root, &err)) {
        return false;
    }
    if (!root.isArray()) {
        EDMLOGE("ArrayUsbDeviceIdSerializer Deserialize root is not array");
        return false;
    }
    if (root.size() > EdmConstants::ALLOWED_USB_DEVICES_MAX_SIZE) {
        EDMLOGE("ArrayUsbDeviceIdSerializer Deserialize data size=[%{public}u] is too large", root.size());
        return false;
    }
    dataObj = std::vector<UsbDeviceId>(root.size());

    for (std::uint32_t i = 0; i < root.size(); ++i) {
        const Json::Value& item = root[i];

        if (!item.isMember("vendorId") || !item.isMember("productId") ||
            !item["vendorId"].isConvertibleTo(Json::intValue) || !item["productId"].isConvertibleTo(Json::intValue)) {
            EDMLOGE("ArrayUsbDeviceIdSerializer Deserialize invalid data at index %{public}u", i);
            return false;
        }
        UsbDeviceId value;
        value.SetVendorId(item["vendorId"].asInt());
        value.SetProductId(item["productId"].asInt());
        dataObj[i] = value;
    }
    return true;
}

bool ArrayUsbDeviceIdSerializer::Serialize(const std::vector<UsbDeviceId> &dataObj, std::string &jsonString)
{
    if (dataObj.empty()) {
        jsonString = "";
        return true;
    }
    Json::Value arrayData(Json::arrayValue);
    for (const auto& item : dataObj) {
        Json::Value root;
        root["vendorId"] = item.GetVendorId();
        root["productId"] = item.GetProductId();
        arrayData.append(root);
    }
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "    ";
    jsonString = Json::writeString(builder, arrayData);
    return true;
}

bool ArrayUsbDeviceIdSerializer::GetPolicy(MessageParcel &data, std::vector<UsbDeviceId> &result)
{
    uint32_t size = data.ReadUint32();
    if (size > EdmConstants::ALLOWED_USB_DEVICES_MAX_SIZE) {
        EDMLOGE("ArrayUsbDeviceIdSerializer:GetPolicy size=[%{public}u] is too large", size);
        return false;
    }
    for (uint32_t i = 0; i < size; i++) {
        UsbDeviceId usbDeviceId;
        if (!UsbDeviceId::Unmarshalling(data, usbDeviceId)) {
            EDMLOGE("ArrayUsbDeviceIdSerializer::GetPolicy read parcel fail");
            return false;
        }
        result.emplace_back(usbDeviceId);
    }
    return true;
}

bool ArrayUsbDeviceIdSerializer::WritePolicy(MessageParcel &reply, std::vector<UsbDeviceId> &result)
{
    std::for_each(result.begin(), result.end(), [&](const auto usbDeviceId) {
        usbDeviceId.Marshalling(reply);
    });
    return true;
}

bool ArrayUsbDeviceIdSerializer::MergePolicy(std::vector<std::vector<UsbDeviceId>> &data,
    std::vector<UsbDeviceId> &result)
{
    std::set<UsbDeviceId> stData;
    for (const auto &dataItem : data) {
        for (const auto &item : dataItem) {
            stData.insert(item);
        }
    }
    result.assign(stData.begin(), stData.end());
    return true;
}
} // namespace EDM
} // namespace OHOS