/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#ifndef SERVICES_EDM_INCLUDE_UTILS_PASSWORD_POLICY_UTILS_H
#define SERVICES_EDM_INCLUDE_UTILS_PASSWORD_POLICY_UTILS_H
 
#include <fstream>
#include <string>
 
#include "cJSON.h"
 
#include "password_policy.h"
 
namespace OHOS {
namespace EDM {
class PasswordPolicyUtils {
public:
    PasswordPolicyUtils();
    ~PasswordPolicyUtils();
    bool UpdatePasswordPolicy(PasswordPolicy &policy);
    bool GetPasswordPolicy(PasswordPolicy &policy);
 
private:
    cJSON *root_;
    bool CreateConfigDir(const std::string &dir);
    bool LoadConfig();
    bool SaveConfig();
};
}  // namespace EDM
}  // namespace OHOS
#endif  // SERVICES_EDM_INCLUDE_UTILS_PASSWORD_POLICY_UTILS_H