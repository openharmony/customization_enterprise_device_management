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

#ifndef INTERFACES_INNER_API_BUNDLE_MANAGER_INCLUDE_INSTALL_ERROR_CODES_H
#define INTERFACES_INNER_API_BUNDLE_MANAGER_INCLUDE_INSTALL_ERROR_CODES_H

#include <string>
#include <vector>

#include "edm_errors.h"

namespace OHOS {
namespace EDM {
static const std::vector<uint32_t> INSTALL_ERROR_CODES = {
    EdmReturnErrCode::APPLICATION_INSTALL_FAILED,
    EdmReturnErrCode::INSTALL_APP_INSUFFICIENT_DISK_SPACE,
    EdmReturnErrCode::INSTALL_APP_ENTERPRISE_DISALLOWED,
    EdmReturnErrCode::INSTALL_APP_PARSE_FAILED,
    EdmReturnErrCode::INSTALL_APP_SIGNATURE_VERIFY_FAILED,
    EdmReturnErrCode::INSTALL_APP_PATH_INVALID_OR_TOO_LARGE,
    EdmReturnErrCode::INSTALL_APPS_CONFIGURATION_MISMATCH,
    EdmReturnErrCode::INSTALL_APP_ISOLATION_MODE_NOT_SUPPORTED,
    EdmReturnErrCode::INSTALL_APP_VERSION_TOO_EARLY,
    EdmReturnErrCode::INSTALL_APP_VERSION_CODE_NOT_GREATER,
    EdmReturnErrCode::INSTALL_APP_DEPENDANT_MODULE_NOT_EXIST,
    EdmReturnErrCode::INSTALL_APP_USER_ID_NOT_FOUND,
    EdmReturnErrCode::INSTALL_APP_OVERLAY_CHECK_FAILED,
    EdmReturnErrCode::INSTALL_APP_MISSING_REQUIRED_PERMISSIONS_FOR_HSP,
    EdmReturnErrCode::INSTALL_APP_SHARED_LIBRARIES_NOT_ALLOWED,
    EdmReturnErrCode::INSTALL_APP_URI_INCORRECT,
    EdmReturnErrCode::INSTALL_APP_PERMISSION_CONFIGURATION_INCORRECT,
    EdmReturnErrCode::INSTALL_APP_CODE_SIGNATURE_VERIFICATION_FAILURE,
    EdmReturnErrCode::INSTALL_APP_ENTERPRISE_DEVICE_VERIFICATION_FAILURE
};
} // namespace EDM
} // namespace OHOS
#endif // INTERFACES_INNER_API_BUNDLE_MANAGER_INCLUDE_INSTALL_ERROR_CODES_H
