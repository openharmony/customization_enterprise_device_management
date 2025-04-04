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

#include "edm_data_ability_utils.h"

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "edm_constants.h"
#include "edm_log.h"
#include "edm_sys_manager.h"
#include "system_ability_definition.h"
#include "uri.h"

namespace OHOS {
namespace EDM {
const std::string SETTINGS_DATA_FIELD_KEYWORD = "KEYWORD";
const std::string SETTINGS_DATA_FIELD_VALUE = "VALUE";
constexpr const char *SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
const std::string SETTINGS_DATA_BASE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";

ErrCode EdmDataAbilityUtils::GetStringFromSettingsDataShare(const std::string &key, std::string &value)
{
    return GetStringFromSettingsDataShare(SETTINGS_DATA_BASE_URI, key, value);
}

ErrCode EdmDataAbilityUtils::GetIntFromSettingsDataShare(const std::string &key, int32_t &result)
{
    return GetIntFromSettingsDataShare(SETTINGS_DATA_BASE_URI, key, result);
}

ErrCode EdmDataAbilityUtils::UpdateSettingsData(const std::string &key, const std::string &value)
{
    return UpdateSettingsData(SETTINGS_DATA_BASE_URI, key, value);
}

ErrCode EdmDataAbilityUtils::GetStringFromSettingsDataShare(
    const std::string &strUri, const std::string &key, std::string &value)
{
    EDMLOGD("EdmDataAbilityUtils::GetStringFromSettingsDataShare enter.");
    sptr<IRemoteObject> remoteObject = EdmSysManager::GetRemoteObjectOfSystemAbility(ENTERPRISE_DEVICE_MANAGER_SA_ID);
    auto dataShareHelper = DataShare::DataShareHelper::Creator(remoteObject, strUri, SETTINGS_DATA_EXT_URI);
    if (dataShareHelper == nullptr) {
        EDMLOGE("EdmDataAbilityUtils::Acquire dataShareHelper failed.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    Uri uri(strUri);
    std::vector<std::string> columns{SETTINGS_DATA_FIELD_VALUE};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_FIELD_KEYWORD, key);
    auto resultset = dataShareHelper->Query(uri, predicates, columns);
    if (resultset == nullptr) {
        EDMLOGE("EdmDataAbilityUtils::GetStringFromDataAbility query fail.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    int32_t numRows = 0;
    resultset->GetRowCount(numRows);
    if (numRows <= 0) {
        EDMLOGD("EdmDataAbilityUtils::GetStringFromDataAbility row zero.");
        return ERR_OK;
    }
    int columnIndex = 0;
    resultset->GoToFirstRow();
    resultset->GetColumnIndex(SETTINGS_DATA_FIELD_VALUE, columnIndex);
    resultset->GetString(columnIndex, value);
    resultset->Close();
    dataShareHelper->Release();
    return ERR_OK;
}

ErrCode EdmDataAbilityUtils::GetIntFromSettingsDataShare(
    const std::string &strUri, const std::string &key, int32_t &result)
{
    std::string valueStr;
    if (FAILED(GetStringFromSettingsDataShare(strUri, key, valueStr))) {
        EDMLOGE("EdmDataAbilityUtils::GetIntFromSettingsDataShare fail");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    if (valueStr.empty()) {
        EDMLOGE("EdmDataAbilityUtils::GetIntFromSettingsDataShare empty.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    char* endptr;
    errno = 0;
    result = strtol(valueStr.c_str(), &endptr, EdmConstants::DECIMAL);
    if (errno == ERANGE || endptr == valueStr.c_str() || *endptr != '\0') {
        EDMLOGE("EdmDataAbilityUtils::GetIntFromSettingsDataShare strtol error.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    return ERR_OK;
}

ErrCode EdmDataAbilityUtils::UpdateSettingsData(
    const std::string &baseUri, const std::string &key, const std::string &value)
{
    sptr<IRemoteObject> remoteObject = EdmSysManager::GetRemoteObjectOfSystemAbility(ENTERPRISE_DEVICE_MANAGER_SA_ID);
    auto dataShareHelper = DataShare::DataShareHelper::Creator(remoteObject, baseUri, SETTINGS_DATA_EXT_URI);
    if (dataShareHelper == nullptr) {
        EDMLOGE("EdmDataAbilityUtils::Acquire dataShareHelper failed.");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    EDMLOGD("UpdateSettingsData key = %{public}s", key.c_str());
    std::string strUri = baseUri + "&key=" + key;
    OHOS::Uri uri(strUri);
    OHOS::DataShare::DataShareValuesBucket bucket;
    bucket.Put(SETTINGS_DATA_FIELD_KEYWORD, key);
    bucket.Put(SETTINGS_DATA_FIELD_VALUE, value);

    OHOS::DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_FIELD_KEYWORD, key);

    std::vector<std::string> columns;
    columns.push_back(SETTINGS_DATA_FIELD_VALUE);
    auto resultset = dataShareHelper->Query(uri, predicates, columns);
    int numRows = 0;
    if (resultset == nullptr) {
        EDMLOGD("UpdateSettingsData nullptr Insert branch");
        dataShareHelper->Insert(uri, bucket);
        dataShareHelper->Release();
        return ERR_OK;
    }
    resultset->GetRowCount(numRows);
    resultset->Close();
    if (numRows <= 0) {
        EDMLOGD("UpdateSettingsData Insert branch");
        dataShareHelper->Insert(uri, bucket);
    } else {
        EDMLOGD("UpdateSettingsData Update branch");
        dataShareHelper->Update(uri, predicates, bucket);
    }
    dataShareHelper->Release();
    return ERR_OK;
}

} // namespace EDM
} // namespace OHOS
