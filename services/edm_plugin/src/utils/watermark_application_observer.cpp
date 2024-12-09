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

#include "watermark_application_observer.h"

#include "managed_event.h"
#ifdef OS_ACCOUNT_EDM_ENABLE
#include "os_account_manager.h"
#endif

namespace OHOS {
namespace EDM {
void WatermarkApplicationObserver::OnProcessCreated(const AppExecFwk::ProcessData &processData)
{
#ifdef OS_ACCOUNT_EDM_ENABLE
    int32_t accountId = -1;
    ErrCode ret = AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(processData.uid, accountId);
    if (ret != ERR_OK) {
        EDMLOGE("OnProcessCreated GetOsAccountLocalIdFromUid error");
        return;
    }
    listener_.SetProcessWatermarkOnAppStart(processData.bundleName, accountId, processData.pid, true);
#endif
}
} // namespace EDM
} // namespace OHOS