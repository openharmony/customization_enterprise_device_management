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

#ifndef COMMON_EXTERNAL_INCLUDE_EDM_ACCESS_TOKEN_MANAGER_IMPL_MOCK_H
#define COMMON_EXTERNAL_INCLUDE_EDM_ACCESS_TOKEN_MANAGER_IMPL_MOCK_H

#include <gmock/gmock.h>

#include "iedm_access_token_manager.h"

namespace OHOS {
namespace EDM {
class EdmAccessTokenManagerImplMock : public IEdmAccessTokenManager {
public:
    ~EdmAccessTokenManagerImplMock() override = default;
    MOCK_METHOD(bool, IsDebug, (), (override));
    MOCK_METHOD(bool, IsNativeCall, (), (override));
    MOCK_METHOD(bool, IsSystemAppCall, (), (override));
    MOCK_METHOD(bool, IsSystemAppOrNative, (), (override));
    MOCK_METHOD(bool, VerifyCallingPermission, (Security::AccessToken::AccessTokenID tokenId,
        const std::string &permissionName), (override));
    MOCK_METHOD(bool, GetAccessTokenId, (int32_t userId, const std::string &appId, int32_t appIndex,
        Security::AccessToken::AccessTokenID &accessTokenId), (override));
};
} // namespace EDM
} // namespace OHOS

#endif // COMMON_EXTERNAL_INCLUDE_EDM_ACCESS_TOKEN_MANAGER_IMPL_MOCK_H