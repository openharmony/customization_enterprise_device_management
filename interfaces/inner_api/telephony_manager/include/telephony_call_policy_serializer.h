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

#ifndef INTERFACES_INNER_API_TELEPHONY_CALL_POLICY_SERIALIZER_H
#define INTERFACES_INNER_API_TELEPHONY_CALL_POLICY_SERIALIZER_H

#include <map>
#include "ipolicy_serializer.h"

namespace OHOS {
    namespace EDM {
    
    struct TelephonyCallPolicyType {
        std::vector<std::string> numberList;
        int32_t policyFlag = 0;
    };
    
    /*
     * Policy data serializer of type std::map<std::string, TelephonyCallPolicyType>.
     */
    class TelephonyCallPolicySerializer : public IPolicySerializer<std::map<std::string, TelephonyCallPolicyType>>,
        public DelayedSingleton<TelephonyCallPolicySerializer> {
    public:
        cJSON* CreateArray(const std::vector<std::string> &numberList);

        bool Deserialize(const std::string &policy, std::map<std::string, TelephonyCallPolicyType> &dataObj) override;
    
        bool Serialize(const std::map<std::string, TelephonyCallPolicyType> &dataObj, std::string &policy) override;
    
        bool GetPolicy(MessageParcel &data, std::map<std::string, TelephonyCallPolicyType> &result) override;
    
        bool WritePolicy(MessageParcel &reply, std::map<std::string, TelephonyCallPolicyType> &result) override;
    
        bool MergePolicy(std::vector<std::map<std::string, TelephonyCallPolicyType>> &data,
            std::map<std::string, TelephonyCallPolicyType> &result) override;
    };
    } // namespace EDM
    } // namespace OHOS

#endif // INTERFACES_INNER_API_TELEPHONY_CALL_POLICY_SERIALIZER_H