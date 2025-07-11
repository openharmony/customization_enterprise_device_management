/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "enterprise_device_mgr_stub_mock.h"

#include <fcntl.h>
#include "domain_filter_rule.h"
#include "edm_bundle_info.h"
#include "firewall_rule.h"
#include "http_proxy.h"
#include "iptables_utils.h"
#include "os_account_info.h"
#include "update_policy_utils.h"
#include "usb_device_id.h"
#include "usb_interface_type.h"
#include "wifi_id.h"

namespace OHOS {
namespace EDM {
const uint32_t APPID_MAX_SIZE = 200;
const int32_t BASE_CLASS = 3;
const int32_t SUB_CLASS = 1;
const int32_t PROTOCOL = 2;
const std::string TEST_TARGET_PATH = "/data/service/el1/public/edm/test.txt";


int EnterpriseDeviceMgrStubMock::InvokeGetEnterpriseInfo(const AppExecFwk::ElementName &admin, EntInfo &entInfo)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeGetEnterpriseInfo";
    entInfo.enterpriseName = "test";
    entInfo.description = "this is test";
    return ERR_OK;
}

int EnterpriseDeviceMgrStubMock::InvokeGetEnterpriseInfoFail(const AppExecFwk::ElementName &admin, EntInfo &entInfo)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeGetEnterpriseInfoFail";
    entInfo.enterpriseName = "";
    entInfo.description = "";
    return ERR_OK;
}

int EnterpriseDeviceMgrStubMock::InvokeIsAdminEnabledFail(
    const AppExecFwk::ElementName &admin, int32_t userId, bool &isEnabled)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeIsAdminEnabled";
    isEnabled = false;
    return ERR_OK;
}

int EnterpriseDeviceMgrStubMock::InvokeIsSuperAdminFail(const std::string &bundleName, bool &isSuper)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeIsSuperAdminFail";
    isSuper = false;
    return ERR_OK;
}

int EnterpriseDeviceMgrStubMock::InvokeIsByodAdminFail(const AppExecFwk::ElementName &admin, bool &isByod)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeIsByodAdminFail";
    isByod = false;
    return ERR_OK;
}

int EnterpriseDeviceMgrStubMock::InvokeGetSuperAdmin(std::string &bundleName, std::string &abilityName)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeGetSuperAdmin";
    bundleName = RETURN_STRING;
    abilityName = RETURN_STRING;
    return ERR_OK;
}

int EnterpriseDeviceMgrStubMock::InvokeGetAdmins(std::vector<std::shared_ptr<AAFwk::Want>> &wants)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeGetAdmins";
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    std::string bundleName = "com.edm.test.demo";
    std::string abilityName = "test.ability";
    want->SetParam("bundleName", bundleName);
    want->SetParam("abilityName", abilityName);
    want->SetParam("adminType", static_cast<int32_t>(AdminType::BYOD));
    wants.push_back(want);
    return ERR_OK;
}

int EnterpriseDeviceMgrStubMock::InvokeGetEnabledAdmin(AdminType type, std::vector<std::string> &enabledAdminList)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeGetEnabledAdmin";
    std::string enabledAdmin = "com.edm.test.demo";
    enabledAdminList.push_back(enabledAdmin);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestEnableAdmin(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestEnableAdmin code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    std::vector<std::string> writeArray{"com.edm.test.demo"};
    reply.WriteInt32(writeArray.size());
    reply.WriteStringVector(writeArray);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestMapEnableAdminTwoSuc(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestMapEnableAdminTwoSuc code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    std::vector<std::string> writeArray{"com.edm.test.demo"};
    reply.WriteStringVector(writeArray);

    std::vector<std::string> writeArray2{"set date time policy"};
    reply.WriteStringVector(writeArray2);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestSetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteString(RETURN_STRING);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPolicyForWriteFileToStream(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPolicyForWriteFileToStream :" << code;
    int32_t fd = open(TEST_TARGET_PATH.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteFileDescriptor(fd);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestSetPolicyInstallFail(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestSetPolicyInstallFail code :" << code;
    code_ = code;
    reply.WriteInt32(EdmReturnErrCode::APPLICATION_INSTALL_FAILED);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeBoolSendRequestGetPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteBool(true);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeBoolSendRequestGetFirewallRule(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeBoolSendRequestGetFirewallRule code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteInt32(1);
    IPTABLES::DomainFilterRule rule{IPTABLES::Action::INVALID, "321", "www.example.com", IPTABLES::Direction::OUTPUT};
    IPTABLES::DomainFilterRuleParcel ruleParcel{rule};
    ruleParcel.Marshalling(reply);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetDomainFilterRules(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetDomainFilterRules code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteInt32(1);
    IPTABLES::FirewallRule rule{IPTABLES::Direction::INVALID, IPTABLES::Action::INVALID, IPTABLES::Protocol::INVALID,
        "", "", "", "", ""};
    IPTABLES::FirewallRuleParcel ruleParcel{rule};
    ruleParcel.Marshalling(reply);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeIntSendRequestGetPolicy(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeIntSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteInt32(0);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeArrayStringSendRequestGetPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteStringVector(std::vector<std::string>{RETURN_STRING});
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetErrPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(EdmReturnErrCode::SYSTEM_ABNORMALLY);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPolicyExceedsMax(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteInt32(APPID_MAX_SIZE + 1);
    reply.WriteStringVector(std::vector<std::string>{RETURN_STRING});
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeArrayElementSendRequestGetPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteStringVector(std::vector<std::string>{ELEMENT_STRING});
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestParamError(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(EdmReturnErrCode::PARAM_ERROR);
    reply.WriteString(RETURN_STRING);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeHttpProxySendRequestGetPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeHttpProxySendRequestGetPolicy code :" << code;
    code_ = code;
    OHOS::NetManagerStandard::HttpProxy httpProxy;
    reply.WriteInt32(ERR_OK);
    httpProxy.Marshalling(reply);
    return ERR_OK;
}

int EnterpriseDeviceMgrStubMock::InvokeBluetoothProxySendRequestGetPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeBluetoothProxySendRequestGetPolicy code :" << code;
    EntInfo entInfo;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteString(RETURN_STRING);
    reply.WriteInt32(1);
    reply.WriteInt32(1);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeAccountProxySendRequestAddOsAccount(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeOsAccountProxySendRequestGetPolicy code :" << code;
    OHOS::AccountSA::OsAccountInfo accountInfo;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    accountInfo.SetLocalName(RETURN_STRING);
    accountInfo.Marshalling(reply);
    reply.WriteString(RETURN_STRING);
    reply.WriteString(RETURN_STRING);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeAllowedUsbDevicesSendRequestGetPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeAllowedUsbDevicesSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    std::vector<UsbDeviceId> usbDeviceIds;
    UsbDeviceId id1;
    int32_t testVid = 1;
    int32_t testPid = 9;
    id1.SetVendorId(testVid);
    id1.SetProductId(testPid);
    usbDeviceIds.push_back(id1);
    reply.WriteInt32(usbDeviceIds.size());
    std::for_each(usbDeviceIds.begin(), usbDeviceIds.end(), [&](const auto usbDeviceId) {
        usbDeviceId.Marshalling(reply);
    });
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeDisallowedUsbDevicesSendRequestGetPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) <<
        "mock EnterpriseDeviceMgrStubMock InvokeDisallowedUsbDevicesSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    std::vector<USB::UsbDeviceType> usbDeviceTypes;
    USB::UsbDeviceType type;
    type.baseClass = BASE_CLASS;
    type.subClass = SUB_CLASS;
    type.protocol = PROTOCOL;
    type.isDeviceType = false;
    usbDeviceTypes.push_back(type);
    reply.WriteInt32(usbDeviceTypes.size());
    std::for_each(usbDeviceTypes.begin(), usbDeviceTypes.end(), [&](const auto usbDeviceType) {
        usbDeviceType.Marshalling(reply);
    });
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeWifiListSendRequestGetPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeWifiListSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    std::vector<WifiId> wifiIds;
    WifiId id1;
    std::string testSsid = "wifi_name";
    std::string testBssid = "68:77:24:77:A6:D6";
    id1.SetSsid(testSsid);
    id1.SetBssid(testBssid);
    wifiIds.push_back(id1);
    reply.WriteInt32(wifiIds.size());
    std::for_each(wifiIds.begin(), wifiIds.end(), [&](const auto wifiId) {
        wifiId.Marshalling(reply);
    });
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeArrayIntSendRequestGetPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock InvokeArrayIntSendRequestGetPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    std::vector<int32_t> inputData;
    int32_t policyOne = 1;
    int32_t policyTwo = 2;
    inputData.push_back(policyOne);
    inputData.push_back(policyTwo);
    reply.WriteInt32Vector(inputData);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetSuperAdmin(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetSuperAdmin code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteString(RETURN_STRING);
    reply.WriteString(RETURN_STRING);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetOTAUpdatePolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetOTAUpdatePolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    UpdatePolicy updatePolicy;
    updatePolicy.type = UpdatePolicyType::PROHIBIT;
    updatePolicy.version = UPGRADE_VERSION;
    UpdatePolicyUtils::WriteUpdatePolicy(reply, updatePolicy);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetUpgradeResult(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetOTAUpdatePolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    UpgradeResult upgradeResult;
    upgradeResult.status = UpgradeStatus::UPGRADE_FAILURE;
    upgradeResult.version = UPGRADE_VERSION;
    upgradeResult.errorCode = UPGRADE_FAILED_CODE;
    upgradeResult.errorMessage = UPGRADE_FAILED_MESSAGE;
    UpdatePolicyUtils::WriteUpgradeResult(reply, upgradeResult);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetUpdateAuthData(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetUpdateAuthData code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteString(AUTH_DATA);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPasswordPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetPasswordPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteString("test_complexityReg");
    reply.WriteInt64(1);
    reply.WriteString("test_additionalDescription");
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetManagedBrowserPolicy(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetManagedBrowserPolicy code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    int32_t size = 200;
    reply.WriteInt32(size);
    void* rawData = malloc(size);
    reply.WriteRawData(rawData, size);
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetAdmins(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetAdmins code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    int32_t size = 1;
    reply.WriteUint32(size);
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    std::string bundleName = "com.edm.test.demo";
    std::string abilityName = "test.ability";
    want->SetParam("bundleName", bundleName);
    want->SetParam("abilityName", abilityName);
    want->SetParam("adminType", static_cast<int32_t>(AdminType::BYOD));
    reply.WriteParcelable(want.get());
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestCheckAndGetAdminProvisionInfo(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetAdmins code :" << code;
    code_ = code;
    reply.WriteInt32(ERR_OK);
    reply.WriteString("com.edm.test.demo");
    return 0;
}

int EnterpriseDeviceMgrStubMock::InvokeSendRequestGetInstalledBundleList(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    GTEST_LOG_(INFO) << "mock EnterpriseDeviceMgrStubMock InvokeSendRequestGetInstalledBundleList code :" << code;
    code_ = code;
    EdmBundleInfo edmBundleInfo;
    edmBundleInfo.name = "com.edm.test.demo";
    std::vector<EdmBundleInfo> edmBundleInfos;
    edmBundleInfos.emplace_back(edmBundleInfo);

    MessageParcel tempParcel;
    (void)tempParcel.SetMaxCapacity(EdmConstants::MAX_PARCEL_CAPACITY);
    tempParcel.WriteInt32(edmBundleInfos.size());
    for (auto &parcelable : edmBundleInfos) {
        tempParcel.WriteParcelable(&parcelable);
    }
    reply.WriteInt32(ERR_OK);
    size_t dataSize = tempParcel.GetDataSize();
    reply.WriteInt32(static_cast<int32_t>(dataSize));
    reply.WriteRawData(reinterpret_cast<uint8_t *>(tempParcel.GetData()), dataSize);
    return 0;
}

} // namespace EDM
} // namespace OHOS
