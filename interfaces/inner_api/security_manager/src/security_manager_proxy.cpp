/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "security_manager_proxy.h"

#include "edm_constants.h"
#include "edm_log.h"
#include "func_code.h"

namespace OHOS {
namespace EDM {
std::shared_ptr<SecurityManagerProxy> SecurityManagerProxy::instance_ = nullptr;
std::once_flag SecurityManagerProxy::flag_;
const std::u16string DESCRIPTOR = u"ohos.edm.IEnterpriseDeviceMgr";

std::shared_ptr<SecurityManagerProxy> SecurityManagerProxy::GetSecurityManagerProxy()
{
    std::call_once(flag_, []() {
        if (instance_ == nullptr) {
            instance_ = std::make_shared<SecurityManagerProxy>();
        }
    });
    return instance_;
}

int32_t SecurityManagerProxy::GetSecurityPatchTag(const AppExecFwk::ElementName &admin, std::string &securityPatchTag)
{
    EDMLOGD("SecurityManagerProxy::GetSecurityPatchTag");
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteInt32(HAS_ADMIN);
    data.WriteParcelable(&admin);
    EnterpriseDeviceMgrProxy::GetInstance()->GetPolicy(EdmInterfaceCode::GET_SECURITY_PATCH_TAG, data, reply);
    int32_t ret = ERR_INVALID_VALUE;
    bool blRes = reply.ReadInt32(ret) && (ret == ERR_OK);
    if (!blRes) {
        EDMLOGE("EnterpriseDeviceMgrProxy:GetPolicy fail. %{public}d", ret);
        return ret;
    }
    reply.ReadString(securityPatchTag);
    return ret;
}

int32_t SecurityManagerProxy::GetDeviceEncryptionStatus(const AppExecFwk::ElementName &admin,
    DeviceEncryptionStatus &deviceEncryptionStatus)
{
    EDMLOGD("SecurityManagerProxy::GetDeviceEncryptionStatus");
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteInt32(HAS_ADMIN);
    data.WriteParcelable(&admin);
    EnterpriseDeviceMgrProxy::GetInstance()->GetPolicy(EdmInterfaceCode::GET_DEVICE_ENCRYPTION_STATUS, data, reply);
    int32_t ret = ERR_INVALID_VALUE;
    bool blRes = reply.ReadInt32(ret) && (ret == ERR_OK);
    if (!blRes) {
        EDMLOGW("EnterpriseDeviceMgrProxy:GetPolicy fail. %{public}d", ret);
        return ret;
    }
    reply.ReadBool(deviceEncryptionStatus.isEncrypted);
    return ERR_OK;
}

int32_t SecurityManagerProxy::SetPasswordPolicy(const AppExecFwk::ElementName &admin, const PasswordPolicy &policy)
{
    EDMLOGD("SecurityManagerProxy::SetPasswordPolicy");
    MessageParcel data;
    std::uint32_t funcCode =
        POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::PASSWORD_POLICY);
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteParcelable(&admin);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteString(policy.complexityReg);
    data.WriteInt64(policy.validityPeriod);
    data.WriteString(policy.additionalDescription);
    return EnterpriseDeviceMgrProxy::GetInstance()->HandleDevicePolicy(funcCode, data);
}

int32_t SecurityManagerProxy::GetPasswordPolicy(PasswordPolicy &policy)
{
    EDMLOGD("SecurityManagerProxy::GetPasswordPolicy innerapi");
    return GetPasswordPolicy(nullptr, policy);
}

int32_t SecurityManagerProxy::GetPasswordPolicy(const AppExecFwk::ElementName &admin, PasswordPolicy &policy)
{
    EDMLOGD("SecurityManagerProxy::GetPasswordPolicy");
    return GetPasswordPolicy(&admin, policy);
}

int32_t SecurityManagerProxy::GetPasswordPolicy(const AppExecFwk::ElementName *admin, PasswordPolicy &policy)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    if (admin == nullptr) {
        data.WriteString(EdmConstants::PERMISSION_TAG_SYSTEM_API);
        data.WriteInt32(WITHOUT_ADMIN);
    } else {
        data.WriteString(WITHOUT_PERMISSION_TAG);
        data.WriteInt32(HAS_ADMIN);
        data.WriteParcelable(admin);
    }
    EnterpriseDeviceMgrProxy::GetInstance()->GetPolicy(EdmInterfaceCode::PASSWORD_POLICY, data, reply);
    int32_t ret = ERR_INVALID_VALUE;
    reply.ReadInt32(ret);
    if (ret != ERR_OK) {
        EDMLOGW("EnterpriseDeviceMgrProxy:GetPolicy fail. %{public}d", ret);
        return ret;
    }
    policy.complexityReg = reply.ReadString();
    reply.ReadInt64(policy.validityPeriod);
    policy.additionalDescription = reply.ReadString();
    return ERR_OK;
}

int32_t SecurityManagerProxy::GetRootCheckStatus(const AppExecFwk::ElementName &admin, std::string &info)
{
    EDMLOGD("SecurityManagerProxy::GetRootCheckStatus");
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteInt32(HAS_ADMIN);
    data.WriteParcelable(&admin);
    std::uint32_t funcCode =
        POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::GET, EdmInterfaceCode::POLICY_CODE_END + 8);
    EnterpriseDeviceMgrProxy::GetInstance()->GetPolicy(funcCode, data, reply);
    int32_t ret = ERR_INVALID_VALUE;
    bool blRes = reply.ReadInt32(ret) && (ret == ERR_OK);
    if (!blRes) {
        EDMLOGW("EnterpriseDeviceMgrProxy:GetPolicy fail. %{public}d", ret);
        return ret;
    }
    reply.ReadString(info);
    return ERR_OK;
}

int32_t SecurityManagerProxy::SetAppClipboardPolicy(const AppExecFwk::ElementName &admin, const int32_t tokenId,
    const int32_t policy)
{
    EDMLOGD("SecurityManagerProxy::SetAppClipboardPolicy");
    MessageParcel data;
    std::uint32_t funcCode =
        POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::CLIPBOARD_POLICY);
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteParcelable(&admin);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteInt32(tokenId);
    data.WriteInt32(policy);
    return EnterpriseDeviceMgrProxy::GetInstance()->HandleDevicePolicy(funcCode, data);
}

int32_t SecurityManagerProxy::GetAppClipboardPolicy(const AppExecFwk::ElementName &admin, const int32_t tokenId,
    std::string &policy)
{
    EDMLOGD("SecurityManagerProxy::GetAppClipboardPolicy");
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteInt32(HAS_ADMIN);
    data.WriteParcelable(&admin);
    data.WriteInt32(tokenId);
    EnterpriseDeviceMgrProxy::GetInstance()->GetPolicy(EdmInterfaceCode::CLIPBOARD_POLICY, data, reply);
    int32_t ret = ERR_INVALID_VALUE;
    reply.ReadInt32(ret);
    if (ret != ERR_OK) {
        EDMLOGW("EnterpriseDeviceMgrProxy:GetPolicy fail. %{public}d", ret);
        return ret;
    }
    policy = reply.ReadString();
    return ERR_OK;
}

int32_t SecurityManagerProxy::SetWatermarkImage(const AppExecFwk::ElementName &admin, const std::string &bundleName,
    const std::shared_ptr<Media::PixelMap> pixelMap, const int32_t accountId)
{
    EDMLOGD("SecurityManagerProxy::SetWatermarkImage");
    MessageParcel data;
    std::uint32_t funcCode =
        POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::SET, EdmInterfaceCode::WATERMARK_IMAGE);
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteParcelable(&admin);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteString(EdmConstants::SecurityManager::SET_SINGLE_WATERMARK_TYPE);
    data.WriteString(bundleName);
    data.WriteInt32(accountId);
    if (!WritePixelMap(pixelMap, data)) {
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    return EnterpriseDeviceMgrProxy::GetInstance()->HandleDevicePolicy(funcCode, data);
}

bool SecurityManagerProxy::WritePixelMap(const std::shared_ptr<Media::PixelMap> pixelMap, MessageParcel &data)
{
    int32_t size = pixelMap->GetByteCount();
    if (size <= 0) {
        EDMLOGE("WritePixelMap size %{public}d", size);
        return false;
    }
    void* pixels = malloc(size);
    if (pixels == nullptr) {
        EDMLOGE("WritePixelMap malloc fail");
        return false;
    }
    uint32_t ret = pixelMap->ReadPixels(size, reinterpret_cast<uint8_t*>(pixels));
    if (ret != ERR_OK) {
        EDMLOGE("WritePixelMap ReadPixels fail!");
        free(pixels);
        return false;
    }
    data.WriteInt32(pixelMap->GetWidth());
    data.WriteInt32(pixelMap->GetHeight());
    data.WriteInt32(size);
    data.WriteRawData(reinterpret_cast<const void*>(pixels), size);
    free(pixels);
    return true;
}

int32_t SecurityManagerProxy::CancelWatermarkImage(const AppExecFwk::ElementName &admin, const std::string &bundleName,
    const int32_t accountId)
{
    EDMLOGD("SecurityManagerProxy::CancelWatermarkImage");
    MessageParcel data;
    std::uint32_t funcCode =
        POLICY_FUNC_CODE((std::uint32_t)FuncOperateType::REMOVE, EdmInterfaceCode::WATERMARK_IMAGE);
    data.WriteInterfaceToken(DESCRIPTOR);
    data.WriteInt32(WITHOUT_USERID);
    data.WriteParcelable(&admin);
    data.WriteString(WITHOUT_PERMISSION_TAG);
    data.WriteString(bundleName);
    data.WriteInt32(accountId);
    return EnterpriseDeviceMgrProxy::GetInstance()->HandleDevicePolicy(funcCode, data);
}
} // namespace EDM
} // namespace OHOS
