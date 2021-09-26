# Mirai-yande.re
## 简介
> 本项目是基于[Mirai-Api-Http](https://github.com/project-mirai/mirai-api-http)和[Mirai-cpp](https://github.com/cyanray/mirai-cpp)的[yande.re](https://yande.re)(国内需要使用代理访问)自定义发图项目
 ~~代码太烂大佬们看看就好~~

>问题反馈或者不会用可以加群：[418227569](https://qm.qq.com/cgi-bin/qm/qr?k=FwFTGePLZ7PljrBzwrl4Boo6Q_2JqTuS&jump_from=webapi)
## 使用环境
Mirai-Api-Http 版本 2.0.2+
## 使用说明
### **使用之前**
在**插件运行目录**下创建`config.json`写入配置信息

<font size=4>**`config.json`必须严格遵循json的书写规范且文件编码为`UTF-8`，不然会导致报错**</font>

[示例文档](doc/config.json)

请确保`Mirai Api Http`配置文件填写无误并对应填写到`./mah.json`文件中(该文件会自动生成)

需要开启`ws`和`http`

```yaml
## 配置文件中的值，全为默认值

## 启用的 adapter, 内置有 http, ws, reverse-ws, webhook
adapters:
  - http
  - ws

## 是否开启认证流程, 若为 true 则建立连接时需要验证 verifyKey
## 建议公网连接时开启
enableVerify: true
verifyKey: 123123

## 开启一些调式信息
debug: true

## 是否开启单 session 模式, 若为 true，则自动创建 session 绑定 console 中登录的 bot
## 开启后，接口中任何 sessionKey 不需要传递参数
## 若 console 中有多个 bot 登录，则行为未定义
## 确保 console 中只有一个 bot 登陆时启用
singleMode: false

## 历史消息的缓存大小
## 同时，也是 http adapter 的消息队列容量
cacheSize: 4096

## adapter 的单独配置，键名与 adapters 项配置相同
adapterSettings:
  ## 详情看 http adapter 使用说明 配置
  http:
    host: 127.0.0.1
    port: 4233
    cors: [*]
  
  ## 详情看 websocket adapter 使用说明 配置
  ws:
    host: 127.0.0.1
    port: 4233
    reservedSyncId: -1

```

## [指令说明](doc/指令说明.md)

## 编译说明
本项目使用c++17标准和vcpkg进行包管理

编译环境仅测试过`Windows x64`

使用到的库：
- RapidJSON
- Boost
- cpr

~~[图文说明档](https://jrsgslb.cn)持续咕咕咕~~