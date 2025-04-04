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

#ifndef FRAMEWORK_EXTENSION_INCLUDE_ENTERPRISE_ADMIN_EXTENSION_CONTEXT_H
#define FRAMEWORK_EXTENSION_INCLUDE_ENTERPRISE_ADMIN_EXTENSION_CONTEXT_H

#include "extension_context.h"

namespace OHOS {
namespace EDM {
class EnterpriseAdminExtensionContext : public AbilityRuntime::ExtensionContext {
public:
    EnterpriseAdminExtensionContext();

    virtual ~EnterpriseAdminExtensionContext();
};
}  // namespace EDM
}  // namespace OHOS
#endif  // FRAMEWORK_EXTENSION_INCLUDE_ENTERPRISE_ADMIN_EXTENSION_CONTEXT_H