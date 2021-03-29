# Mirai-yande.re
## 简介
> 本项目是基于[Mirai-Api-Http](https://github.com/project-mirai/mirai-api-http)和[Mirai-cpp](https://github.com/cyanray/mirai-cpp)的[yande.re](https://yande.re)(国内需要使用代理访问)自定义发图项目
 ~~代码太烂大佬们看看就好~~

>问题反馈或者不会用可以加群：[418227569](https://qm.qq.com/cgi-bin/qm/qr?k=FwFTGePLZ7PljrBzwrl4Boo6Q_2JqTuS&jump_from=webapi)
## 使用说明
### **使用之前**
在**插件运行目录**下创建`config.json`写入配置信息
```json
{
    "主人": 主人QQ号,
    "host": "Mirai-Api-Http的ip地址",
    "port": "Mirai-Api-Http的端口",
    "qq": "机器人的QQ号",
    "key": "Mirai-Api-Http的Auth密码",

    "是否使用代理": true,
    "proxy": "代理地址",
    "搜图": "萌萌搜图",

    "是否缓存图片": false,
    "发送原图":false,
    "是否撤回": false,
    "撤回延时": 60,

    "R18开启": ".R18on",
    "R18关闭": ".R18off",

    "发送提示语": "为爱发电中..."
}
```
> 主人qq号直接写qq号就行

> R18开启和关闭为分群开关R18限制指令的自定义，需要群管理员及以上的权限才能使用（主人无须群管理即可开启）

> 开启R18后，该群将无视评级限制

`config.json`必须严格遵循json的书写规范，不然会导致报错
### 指令说明
首次运行本插件后，会在运行目录下创建`config`文件夹

在`config`文件夹下创建并编辑`rule.ini`文件

示例:
```ini
[随机萝莉]
tag=loli
rating=q
mode1=1
mode2=0
time=30
num=28108
member=123|123
send=3
```
需要手动填写的地方：

**`[随机萝莉]`** 为自定义的指令名称

**`tag`** [yande.re](https://yande.re)内的tag名称

**`rating`** 评级限制，可以是：安全的（safe），可疑的（18+）（questionable），明确的（explicit）
> 分别对应填写`s`,`q`,`e`

>注：评级包含关系为`e`包含`q`和`s`,`q`包含`s`

**`send`** 连发数量，可填可不填，不填默认为1

为了您Bot的账号安全，建议填写`q`

**`mode1`,`mode2`,`time`,`member`** 详见[频率限制说明](#频率限制说明)

填写完成后，发送 **`更新tag`** 指令更新`num`

#### 更新tag
更新`rule.ini`配置文件内所有指令的`num`

刷新`command.txt`文件
#### 清理缓存
清理`temp`文件夹内所有文件

#### help/菜单
发送现在bot可以使用的所有自定义指令

#### 引用回复搜图
引用一条图片消息，AtBot或者发送`搜图`自定义的指令

会使用[Ascii2d](https://ascii2d.net)进行图片检索（现在该站被墙，国内需要代理访问）

或者直接私聊bot一张图片

由于`Mirai-Api-Http`限制，引用的图片必须为bot成功连接之后发送的
## 编译说明
本项目使用c++11标准和vcpkg进行包管理

编译环境仅测试过`Windows`

使用到的库：
- Mirai-cpp
- RapidJSON
- Boost
- cpr

[图文说明档](https://jrsgslb.cn)~~持续咕咕咕~~

### 频率限制说明
因为太长，写在最后

#### mode1 :

0代表群为单位

1代表个人为单位,指定对象为人（qq号）

2代表个人为单位，指定对象为群（群号）

#### mode2 :

0代表全部生效

1代表指定生效，其余放行

2代表指定生效，其余屏蔽

3代表指定放行，其余生效

4代表指定屏蔽，其余生效

5代表管理放行，群员生效，指定qq号码/群屏蔽

6代表管理生效，群员屏蔽，指定qq号码/群屏蔽

#### member
填写qq号或者群号，以 **任意非数字字符分割**

#### 白名单说明
在`./config/`文件夹下创建`白名单.txt`和`群白名单.txt`文件

一行一个qq号或者群号