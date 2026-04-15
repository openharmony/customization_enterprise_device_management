# enterprise_device_management 技术文档

## 第一部分：项目全景图（一级子目录摘要）

#### 📁 `/customization_enterprise_device_management/common`
- **目录职责**：公共基础模块，提供通用配置、外部系统接口适配和原生工具函数。
- **包含的核心类/文件**：EdmUtils, EdmLog, EdmBundleManagerImpl, EdmAppManagerImpl
- **与其他目录的关联**：被 services/edm 和 services/edm_plugin 依赖使用

#### 📁 `/customization_enterprise_device_management/etc`
- **目录职责**：系统配置和初始化脚本目录，包含系统服务启动配置。
- **包含的核心类/文件**：init/, param/
- **与其他目录的关联**：无直接代码依赖，用于系统启动配置

#### 📁 `/customization_enterprise_device_management/figure`
- **目录职责**：文档图片资源目录，存储架构图和流程图。
- **包含的核心类/文件**：enterprise_device_management.png, en_enterprise_device_management.png
- **与其他目录的关联**：无代码依赖，仅用于文档

#### 📁 `/customization_enterprise_device_management/framework`
- **目录职责**：框架扩展模块，提供企业设备管理的框架层扩展能力。
- **包含的核心类/文件**：extension/
- **与其他目录的关联**：依赖 services/edm 的核心能力

#### 📁 `/customization_enterprise_device_management/interfaces`
- **目录职责**：对外接口层，提供ArkTS/JS的NAPI接口和内部API定义。
- **包含的核心类/文件**：AdminManagerAddon, BundleManagerAddon, IPlugin, IPolicySerializer
- **与其他目录的关联**：依赖 services/edm 的实现，对外暴露API

#### 📁 `/customization_enterprise_device_management/sa_profile`
- **目录职责**：系统能力（System Ability）配置文件，定义EDM服务的SA配置。
- **包含的核心类/文件**：1601.json, BUILD.gn
- **与其他目录的关联**：配置 services/edm 的系统服务注册

#### 📁 `/customization_enterprise_device_management/services`
- **目录职责**：核心服务实现，包含EDM主服务、策略插件和IDL定义。
- **包含的核心类/文件**：EnterpriseDeviceMgrAbility, AdminManager, PolicyManager, PluginManager
- **与其他目录的关联**：依赖 common 和 interfaces，被其他模块调用

#### 📁 `/customization_enterprise_device_management/test`
- **目录职责**：测试代码目录，包含单元测试和模糊测试。
- **包含的核心类/文件**：unittestunittest/
- **与其他目录的关联**：测试 services/edm 和 services/edm_plugin 的功能

#### 📁 `/customization_enterprise_device_management/tools`
- **目录职责**：开发工具和辅助脚本，用于代码生成和构建辅助。
- **包含的核心类/文件**：edm/
- **与其他目录的关联**：辅助整个项目的开发和构建

---

## 第二部分：模块内部细节（类与重要函数统计）

### 📁 `/customization_enterprise_device_management/services/edm`

#### 🧩 `AdminManager`
- **类职责**：管理所有管理员实例，提供管理员生命周期管理和权限验证。
- **重要函数统计**：
    - `GetInstance()`: 获取AdminManager单例实例。 ⭐核心
    - `GetAdminByUserId()`: 根据用户ID获取管理员列表。
    - `GetAdminByPkgName()`: 根据包名获取指定管理员。
    - `DeleteAdmin()`: 删除指定管理员。
    - `IsSuperAdminExist()`: 判断是否存在超级管理员。
    - `EnableAdmin()`: 启用管理员。
    - `DisableAdmin()`: 禁用管理员。
    - `SetAdminValue()`: 设置管理员信息。
    - `SaveSubscribeEvents()`: 保存订阅事件。
    - `GetAdmins()`: 获取所有管理员。
- **关键协作对象**：Admin, AdminPoliciesStorageRdb, IAdminObserver

#### 🧩 `PolicyManager`
- **类职责**：管理所有设备策略，提供策略的增删改查和跨用户策略管理。
- **重要函数统计**：
    - `GetInstance()`: 获取PolicyManager单例实例。 ⭐核心
    - `GetPolicy()`: 获取指定策略值。
    - `SetPolicy()`: 设置策略值。
    - `GetAllPolicyByAdmin()`: 获取管理员的所有策略。
    - `GetAdminByPolicyName()`: 根据策略名获取管理员列表。
    - `Init()`: 初始化策略管理器。
    - `ReplaceAllPolicy()`: 替换所有策略。
- **关键协作对象**：UserPolicyManager, DevicePoliciesStorageRdb

#### 🧩 `PluginManager`
- **类职责**：管理所有策略插件，提供插件加载、卸载和策略执行功能。
- **重要函数统计**：
    - `GetInstance()`: 获取PluginManager单例实例。 ⭐核心
    - `GetPluginByFuncCode()`: 根据功能码获取插件。
    - `UpdateDevicePolicy()`: 更新设备策略。
    - `GetPolicy()`: 获取策略值。
    - `LoadPlugin()`: 加载插件动态库。
    - `UnloadPlugin()`: 卸载插件动态库。
    - `AddPlugin()`: 添加插件。
    - `AddExtensionPlugin()`: 添加扩展插件kt()`: 获取策略权限。
- **关键协作对象**：IPlugin, IPluginExecuteStrategy

#### 🧩 `EnterpriseDeviceMgrAbility`
- **类职责**：企业设备管理主Ability，实现SystemAbility接口，提供跨进程IPC服务。
- **重要函数统计**：
    - `GetInstance()`: 获取Ability单例实例。 ⭐核心
    - `OnStart()`: 服务启动回调。
    - `OnStop()`: 服务停止回调。
    - `EnableAdmin()`: 启用管理员。
    - `DisableAdmin()`: 禁用管理员。
    - `HandleDevicePolicy()`: 处理设备策略设置。
    - `GetDeviceDevicePolicy()`: 获取设备策略。
    - `SubscribeManagedEvent()`: 订阅管理事件。
    - `AuthorizeAdmin()`: 授权管理员。
    - `ReplaceSuperAdmin()`: 替换超级管理员。
- **关键协作对象**：AdminManager, PolicyManager, PluginManager, EnterpriseDeviceMgrStub

---

### 📁 `/customization_enterprise_device_management/services/edm_plugin`

#### 🧩 `AllowedAppBundlesPlugin`
- **类职责**：管理允许安装的应用包列表策略。
- **重要函数统计**：
    - `OnSetPolicy()`: 设置允许安装包列表。
    - `OnGetPolicy()`: 获取允许安装包列表。
    - `OnRemovePolicy()`: 移除策略。

#### 🧩 `DisableBluetoothPlugin`
- **类职责**：管理禁用蓝牙策略。
- **重要函数统计**：
    - `OnSetPolicy()`: 设置禁用蓝牙策略。
    - `OnGetPolicy()`: 获取蓝牙禁用状态。

#### 🧩 `DisableCameraPlugin`
- **类职责**：管理禁用摄像头策略。
- **重要函数统计**：
    - `OnSetPolicy()`: 设置禁用摄像头策略。
    - `OnGetPolicy()`: 获取摄像头禁用状态。

---

### 📁 `/customization_enterprise_device_management/interfaces/inner_api/plugin_kits`

#### 🧩 `IPlugin`
- **类职责**：策略插件的核心接口，定义了插件处理策略的标准生命周期方法。
- **重要函数统计**：
    - `OnHandlePolicy()`: 处理策略设置请求，核心业务逻辑入口。 ⭐核心
    - `OnHandlePolicyDone()`: 策略处理完成后的回调。
    - `OnAdminRemove()`: 管理员移除时的策略清理。
    - `OnAdminRemoveDone()`: 管理员移除完成后的回调。
    - `OnGetPolicy()`: 获取当前策略值。
    - `OnOtherServiceStart()`: 其他系统服务启动时的回调。
    - `GetOthersMergePolicyData()`: 获取其他管理员的合并策略数据。
    - `GetCode()`: 获取策略功能码。
    - `GetPolicyName()`: 获取策略名称。
    - `GetPermission()`: 获取策略所需权限。
- **关键协作对象**：IPluginExecuteStrategy, IExternalModuleAdapter, HandlePolicyData

#### 🧩 `IPluginTemplate<CT, DT>`
- **类职责**：策略处理模板类，实现IPlugin接口并提供策略处理的事件注册机制。
    - CT: 策略处理逻辑类，是需要实现的代码
    - DT: 策略数据类型，例如：string, vector<string>, map<string,string>...
- **重要函数统计**：
    - `OnHandlePolicy()`: 处理策略设置请求，根据操作类型调用对应的监听器。 ⭐核心
    - `OnHandlePolicyDone()`: 策略处理完成后的回调，调用HandlePolicyDone监听器。
    - `OnAdminRemove()`: 管理员移除时的策略清理，调用AdminRemove监听器。
    - `OnAdminRemoveDone()`: 管理员移除完成后的回调，调用AdminRemoveDone监听器。
    - `OnGetPolicy()`: 获取当前策略值。
    - `OnOtherServiceStart()`: 其他系统服务启动时的回调，调用OtherServiceStart监听器。
    - `GetOthersMergePolicyData()`: 获取其他管理员的合并策略数据。
    - `SetOnHandlePolicyListener()`: 注册策略处理事件监听器，支持多种函数类型（Supplier, Function, BiFunction, ReplyFunction）。
    - `SetOnHandlePolicyDoneListener()`: 注册策略处理完成事件监听器，支持BoolConsumer和BiBoolConsumer。
    - `SetOnAdminRemoveListener()`: 注册管理员移除事件监听器，支持Supplier和BiAdminFunction。
    - `SetOnAdminRemoveDoneListener()`: 注册管理员移除完成事件监听器，支持Runner和BiAdminConsumer。
    - `SetOtherServiceStartListener()`: 注册其他服务启动事件监听器，支持IntConsumer。
    - `SetInstance()`: 设置策略处理对象实例。
    - `SetSerializer()`: 设置策略数据序列化器。
    - `InitAttribute()`: 初始化策略属性（策略码、策略名称、权限配置等）。
    - `InitPermission()`: 初始化策略权限配置。
    - `GetMergePolicyData()`: 获取合并后的策略数据。
- **关键协作对象**：IPlugin, IPolicySerializer<DT>, IPolicyManager, HandlePolicyData

#### 🧩 `IPluginManager`
- **类职责**：插件管理器接口，负责插件的注册、查找和管理。
- **重要函数统计**：
    - `GetInstance()`: 获取插件管理器单例。 ⭐核心
    - `AddPlugin()`: 注册基础插件。
    - `AddExtensionPlugin()`: 注册扩展插件，支持不同的执行策略。

#### 🧩 `IPluginExecuteStrategy`
- **类职责**：插件执行策略接口，定义策略执行的不同模式。
- **重要函数统计**：
    - `OnGetExecute()`: 执行获取策略操作。
    - `OnSetExecute()`: 执行设置策略操作。
    - `OnInitExecute()`: 执行初始化操作。
    - `OnAdminRemoveExecute()`: 执行管理员移除操作。

#### 🧩 `IPolicySerializer<DT>`
- **类职责**：策略数据序列化模板接口，负责数据在不同格式间的转换。
- **重要函数统计**：
    - `Deserialize()`: 将JSON字符串反序列化为数据对象。
    - `Serialize()`: 将数据对象序列化为JSON字符串。
    - `GetPolicy()`: 从MessageParcel读取策略数据。
    - `WritePolicy()`: 将策略数据写入MessageParcel。
    - `MergePolicy()`: 合并多个管理员的策略数据。 ⭐核心

#### 🧩 `ArraySerializer<DT, T_ARRAY>`
- **类职责**：数组类型策略的序列化器，继承自IPolicySerializer。
- **重要函数统计**：
    - `Deserialize()`: 反序列化数组数据。
    - `Serialize()`: 序列化数组数据。
    - `GetPolicy()`: 读取数组策略。
    - `WritePolicy()`: 写入数组策略。
    - `MergePolicy()`: 合并数组策略，自动去重。
    - `Deduplication()`: 数组去重处理。

#### 🧩 `StringSerializer`
- **类职责**：字符串类型策略的序列化器。
- **重要函数统计**：
    - `Deserialize()`: 反序列化字符串。
    - `Serialize()`: 序列化字符串。
    - `GetPolicy()`: 读取字符串策略。
    - `WritePolicy()`: 写入字符串策略。
    - `MergePolicy()`: 合并字符串策略。

#### 🧩 `IExternalModuleAdapter`
- **类职责**：外部模块适配器接口，用于与外部系统模块交互。
- **重要函数统计**：
    - `NotifyUpgradePackages()`: 通知升级包信息。
    - `SetUpdatePolicy()`: 设置更新策略。
    - `GetCallingBundleName()`: 获取调用方包名。

#### 🧩 `HandlePolicyData`
- **类职责**：策略处理数据结构，用于传递策略处理过程中的数据。
- **重要成员**：
    - `policyData`: 当前策略数据。
    - `mergePolicyData`: 合并后的策略数据。
    - `isChanged`: 策略是否发生变化。

---

### 📁 `/customization_enterprise_device_management/interfaces/kits`

#### 🧩 `AdminManagerAddon`
- **类职责**：NAPI接口适配层，将C++接口暴露给JavaScript/ArkTS。
- **重要函数统计**：
    - `EnableAdmin()`: 启用管理员的NAPI接口。
    - `DisableAdmin()`: 禁用管理员的NAPI接口。
    - `GetEnabledAdmin()`: 获取已启用管理员的NAPI接口。
    - `SubscribeManagedEvent()`: 订阅事件的NAPI接口。

#### 🧩 `BundleManagerAddon`
-- **类职责**：应用包管理相关的NAPI接口。
- **重要函数统计**：
    - `SetBundleInstallPolicies()`: 设置应用安装策略。
    - `GetBundleInstallPolicies()`: 获取应用安装策略。

---

### 📁 `/customization_enterprise_device_management/common/native`

#### 🧩 `EdmUtils`
- **类职责**：提供通用的工具函数和辅助方法。
- **重要函数统计**：
    - `GetBundleNameByUid()`: 根据UID获取包名。
    - `GetUserId()`: 获取当前用户ID。
    - `CheckPermission()`: 检查权限。

#### 🧩 `EdmLog`
- **类职责**：日志管理工具类。
- **重要函数统计**：
    - `EdmDebug()`: 输出调试日志。
    - `EdmInfo()`: 输出信息日志。
    - `EdmError()`: 输出错误日志。

---

### 📁 `/customization_enterprise_device_management/common/external`

#### 🧩 `EdmBundleManagerImpl`
- **类职责**：BundleManager接口的适配实现。
- **重要函数统计**：
    - `GetBundleInfo()`: 获取应用包信息。
    - `GetBundleInfos()`: 获取应用包列表。
    - `QueryAbilityInfo()`: 查询Ability信息。

#### 🧩 `EdmAppManagerImpl`
- **类职责**：AppManager接口的适配实现。
- **重要函数统计**：
    - `IsRunning()`: 检查应用是否运行。
    - `KillProcess()`: 终止应用进程。

---

## 第三部分：策略存储方式和格式（新策略开发参考）

### 策略存储方式

#### 持久化存储
- **存储介质**：关系型数据库（RDB）
- **操作接口**：`NativeRdb::ValuesBucket`
- **管理类**：
    - `DevicePoliciesStorageRdb`：策略策略数据操作
    - `AdminPoliciesStorageRdb`：管理员信息操作
    - `UserPolicyManager`：用户策略内存管理

#### 数据库表结构
1. **device_admin_policies表**
   - 字段：userId, adminName, policyName, policyValue

2. **device_combined_policies表**
   - 字段：userId, policyName, policyValue

3. **admin_policies表**
   - 字段：userId, packageName, entInfo, managedEvents

### 策略数据格式

#### 策略值格式
- **序列化格式**：JSON字符串
- **序列化库**：cJSON

#### 内存数据结构
```cpp
// 策略名称 -> 策略值（JSON字符串）
PolicyItemsMap = std::unordered_map<std::string, std::string>

// 管理员名称 -> 策略值（JSON字符串）
AdminValueItemsMap = std::unordered_map<std::string, std::string>

// 合并后的策略
PolicyItemsMap combinedPolicies_

// 管理员 -> 策略集合
std::unordered_map<std::string, PolicyItemsMap> adminPolicies_

// 策略 -> 管理员集合
std::unordered_map<std::string, AdminValueItemsMap> policyAdmins_
```

### 策略序列化器实现

#### 标准序列化器类型
- `IPolicySerializer<DT>`：基础序列化器接口
- `ArraySerializer<DT, T_ARRAY>`：数组类型策略
- `StringSerializer`：字符串类型策略

#### 自定义序列化器实现步骤
1. 继承 `IPolicySerializer<DT>` 和 `DelayedSingleton<YourSerializer>`
2. 实现以下方法：
   - `Deserialize()`: JSON字符串 -> 数据对象
   - `Serialize()`: 数据对象 -> JSON字符串
   - `GetPolicy()`: MessageParcel -> 数据对象
   - `WritePolicy()`: 数据对象 -> MessageParcel
    - `MergePolicy()`: 合并多个管理员策略

#### 示例：WatermarkImageSerializer
```cpp
class WatermarkImageSerializer :
    public IPolicySerializer<std::map<std::pair<std::string, int32_t>, WatermarkImageType>>,
    public DelayedSingleton<WatermarkImageSerializer> {
public:
    bool Deserialize(const std::string &policy, std::map<std::pair<std::string, int32_t>, WatermarkImageType> &dataObj) override;
    bool Serialize(const std::map<std::pair<std::string, int32_t>, WatermarkImageType> &dataObj, std::string &policy) override;
    bool GetPolicy(MessageParcel &data, std::map<std::pair<std::string, int32_t>, WatermarkImageType> &result) override;
    bool WritePolicy(MessageParcel &reply, std::map<std::pair<std::string, int32_t>, WatermarkImageType> &result) override;
    bool MergePolicy(std::vector<std::map<std::pair<std::string, int32_t>, WatermarkImageType>> &data,
        std::map<std::pair<std::string, int32_t>, WatermarkImageType> &result) override;
};
```

### 策略JSON格式示例

#### 水印图片策略（POLICY_WATERMARK_IMAGE_POLICY）
```json
[
  {
    "bundleName": "com.example.app1",
    "accountId": 100,
    "fileName": "edm_1234567890",
    "width": 1920,
    "height": 1080
  }
]
```

#### 字段说明
- `bundleName`: string - 应用包名
- `accountId`: number - 账户ID
- `fileName`: string - 水印图片文件名
- `width`: number - 图片宽度
- `height`: number - 图片高度

### 新策略开发流程

1. **定义策略常量**：在 `edm_constants.h` 中添加策略名称常量
2. **定义数据结构**：创建策略对应的数据结构（如 `WatermarkImageType`）
3. **实现序列化器**：继承 `IPolicySerializer<DT>` 实现序列化/反序列化
4. **实现插件类**：继承 `IPlugin` 或使用 `IPluginTemplate<CT, DT>` 模板
5. **注册插件**：在插件构造函数中通过 `IPluginManager::GetInstance()->AddPlugin()` 注册
6. **定义接口码**：在 `edm_ipc_interface_code.h` 中添加接口码
7. **添加权限配置**：在插件中配置所需权限

---

### 📁 `/customization_enterprise_device_management/services/edm/src/query_policy`

#### 🧩 `DisallowedP2PQuery`
- **类职责**：查询禁止P2P策略的状态。
- **重要函数统计**：
    - `GetPolicyName()`: 返回策略名称常量 `POLICY_DISALLOWED_P2P`。
    - `GetPermission()`: 根据权限类型返回相应的权限配置。
    - `QueryPolicy()`: 调用 `GetBoolPolicy` 获取布尔策略值。

---

## 第四部分：BUILD.gn构建配置规则

### services/edm/BUILD.gn
- **配置位置**：在 `sources` 列表中添加query源文件
- **添加规则**：
  ```gn
  sources += [ "./src/query_policy/disallowed_p2p_query.cpp" ]
  ```

### services/edm_plugin/BUILD.gn
- **配置位置**：在 `sources` 列表中添加plugin源文件
- **添加规则**：
  ```gn
  sources += [ "./src/disallowed_p2p_plugin.cpp" ]
  ```

### test/unittest/services/edm/BUILD.gn
- **配置位置**：在 `sources` 列表中添加query源文件（相对路径）
- **添加规则**：
  ```gn
  sources += [ "../../../../services/edm/src/query_policy/disallowed_p2p_query.cpp" ]
  ```

### test/unittest/services/edm_plugin/BUILD.gn
- **配置位置**：在 `sources` 列表中添加plugin源文件（相对路径）
- **添加规则**：
  ```gn
  sources += [ "../../../../services/edm_plugin/src/disallowed_p2p_plugin.cpp" ]
  ```

### test/fuzztest/enterprisedevicemgrstubmock_fuzzer/BUILD.gn
- **配置位置**：在 `sources` 列表中添加query源文件（相对路径）
- **添加规则**：
  ```gn
  sources += [ "../../../services/edm/src/query_policy/disallowed_p2p_query.cpp" ]
  ```

---

## 第五部分：完整新增策略开发示例

### 示例：新增禁止P2P策略

#### 1. 定义策略常量（edm_constants.h）
```cpp
namespace EdmConstants::Restrictions {
    const char* const LABEL_DISALLOWED_POLICY_P2P = "p2p";
}

namespace PolicyName {
    const char* const POLICY_DISALLOWED_P2P = "disallowed_p2p";
}
```

#### 2. 定义接口码（edm_ipc_interface_code.h）
```cpp
enum EdmInterfaceCode : uint32_t {
    // ... 其他接口码
    DISALLOWED_P2P = 1148,
    POLICY_CODE_END = 3000,
};
```

#### 3. 实现插件类（disallowed_p2p_plugin.h/cpp）
```cpp
// disallowed_p2p_plugin.h
class DisallowedP2PPlugin : public PluginSingleton<DisallowedP2PPlugin, bool>,
    public BasicBoolPlugin {
public:
    void InitPlugin(std::shared_ptr<IPluginTemplate<DisallowedP2PPlugin, bool>> ptr) override;
private:
    ErrCode SetOtherModulePolicy(bool data, int32_t userId) override;
    ErrCode RemoveOtherModulePolicy(int32_t userId) override;
};

// disallowed_p2p_plugin.cpp
void DisallowedP2PPlugin::InitPlugin(...) {
    std::map<IPlugin::PermissionType, std::string> typePermissions;
    typePermissions.emplace(IPlugin::PermissionType::SUPER_DEVICE_ADMIN,
        EdmPermission::PERMISSION_ENTERPRISE_MANAGE_RESTRICTIONS);
    typePermissions.emplace(IPlugin::PermissionType::BYOD_DEVICE_ADMIN,
        EdmPermission::PERMISSION_PERSONAL_MANAGE_RESTRICTIONS);
    IPlugin::PolicyPermissionConfig config = IPlugin::PolicyPermissionConfig(typePermissions, IPlugin::ApiType::PUBLIC);
    ptr->InitAttribute(EdmInterfaceCode::DISALLOWED_P2P, PolicyName::POLICY_DISALLOWED_P2P, config, true);
    ptr->SetSerializer(BoolSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisallowedP2PPlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnAdminRemoveListener(&DisallowedP2PPlugin::OnAdminRemove);
    persistParam_ = "persist.edm.p2p_disallowed";
}
```

#### 4. 实现查询类（（disallowed_p2p_query.h/cpp）
```cpp
// disallowed_p2p_query.h
class DisallowedP2PQuery : public IPolicyQuery {
public:
    std::string GetPolicyName() override;
    std::string GetPermission(IPlugin::PermissionType, const std::string &permissionTag) override;
    ErrCode QueryPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply, int32_t userId) override;
};

// disallowed_p2p_query.cpp
std::string DisallowedP2PQuery::GetPolicyName() {
    return PolicyName::POLICY_DISALLOWED_P2P;
}

ErrCode DisallowedP2PQuery::QueryPolicy(...) {
    return GetBoolPolicy(policyData, reply);
}
```

#### 5. 注册查询类（plugin_policy_reader.cpp）
```cpp
// 在文件开头添加头文件
#include "disallowed_p2p_query.h"

// 在GetPolicyQueryTenth函数中添加case分支
case EdmInterfaceCode::DISALLOWED_P2P:
    obj = std::make_shared<DisallowedP2PQuery>();
    return ERR_OK;
```

#### 6. 添加restrictions映射（restrictions_addon.cpp）
```cpp
std::unordered_map<std::string, uint32_t> RestrictionsAddon::labelCodeMap = {
    // ... 其他映射
    {EdmConstants::Restrictions::LABEL_DISALLOWED_POLICY_P2P, EdmInterfaceCode::DISALLOWED_P2P},
};
```

#### 7. 更新BUILD.gn文件
```gn
# services/edm/BUILD.gn
sources += [ "./src/query_policy/disallowed_p2p_query.cpp" ]

# services/edm_plugin/BUILD.gn
sources += [ "./src/disallowed_p2p_plugin.cpp" ]

# test/unittest/services/edm/BUILD.gn
sources += [ "../../../../services/edm/src/query_policy/disallowed_p2p_query.cpp" ]

# test/unittest/services/edm_plugin/BUILD.gn
sources += [ "../../../../services/edm_plugin/src/disallowed_p2p_plugin.cpp" ]

# test/fuzztest/enterprisedevicemgrstubmock_fuzzer/BUILD.gn
sources += [ "../../../services/edm/src/query_policy/disallowed_p2p_query.cpp" ]
```