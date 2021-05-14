# Mirai-yande.re
## 简介
> 本项目是基于[Mirai-Api-Http](https://github.com/project-mirai/mirai-api-http)和[Mirai-cpp](https://github.com/cyanray/mirai-cpp)的[yande.re](https://yande.re)(国内需要使用代理访问)自定义发图项目
 ~~代码太烂大佬们看看就好~~

>问题反馈或者不会用可以加群：[418227569](https://qm.qq.com/cgi-bin/qm/qr?k=FwFTGePLZ7PljrBzwrl4Boo6Q_2JqTuS&jump_from=webapi)
## 使用环境
Mirai-Api-Http 版本 1.11+
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
    "搜图": "进入搜图模式的指令",
    "退出搜图": "退出搜图模式的指令，目前必须手动退出，自动退出待完善",
    "匹配度": 80,
    
    "单图cos": "单图cos图片指令",
    "组图cos": "组图cos图片指令",

    "是否缓存图片": false,
    "发送原图":false,
    "是否撤回": false,
    "撤回延时": 60,
    "发送图片ID": true,

    "R18开启": "开启分群R18的指令",
    "R18关闭": "关闭分区R18的指令",
    "yid": "获取y站id的指令前缀,例：yid123  会发生y站id为123的图片",
    "pid": "同上，获取站点为p站",
    "精华消息": "引用回复设置精华消息的指令",
    "更新时间": 30,

    "发送提示语": "自定义发送提示语"
}
```
> 主人qq号直接写qq号就行

> R18开启和关闭为分群开关R18限制指令的自定义，需要群管理员及以上的权限才能使用（主人无须群管理即可开启）

> 开启R18后，该群将无视评级限制

`config.json`必须严格遵循json的书写规范且文件编码为`UTF-8`，不然会导致报错
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

**`mode1`,`mode2`,`time`,`member`** 详见[频率限制说明](doc/频率限制说明.md)

填写完成后，发送 **`更新tag`** 指令更新`num`

#### 更新tag
更新`rule.ini`配置文件内所有指令的`num`

刷新`command.txt`文件
#### 清理缓存
清理`temp`文件夹内所有文件

#### help/菜单
发送现在bot可以使用的所有自定义指令

#### 搜图
引用一条图片消息并`@Bot`或者发送`搜图`自定义的指令

或直接发送`搜图`指令

先使用[Saucenao](https://saucenao.com/)进行图片检索，如果相似度低于`config.json`内的`相似度`

再使用[Ascii2d](https://ascii2d.net)进行图片检索（两个站国内需要代理访问）

或者直接私聊bot一张图片

由于`Mirai-Api-Http`限制，引用的图片必须为bot成功连接之后发送的否则会报错:`mirai-api-http 错误: 指定对象不存在
`

#### B站直播订阅
订阅指定Uid的直播开关推送
##### 指令:
`增加|关闭|删除直播订阅Uid|uid|UID:123`

指令很明确了，不用解释了吧QAQ

刷新频率为`config.json`内的`更新时间`单位为秒

> 注意:每次启动本程序都会重置直播状态，如果该主播在直播中，则会推送一次

#### 设置精华消息
引用某消息并发送指令，会将该消息设置为群精华消息
> 注意:某些群因为莫名其妙的原因会导致报错，报错内容为:`mirai-api-http 错误: 无操作权限` 应该为Mirai-api-http的某些bug
### cos相关
#### 单图cos
[Api](htts://api.jrsgslb.cn/cos)
#### 组图cos
从b站的api内返回一组图片
> 注意：如果随机到的值b站为空的话，则会导致闪退报错
## 编译说明
本项目使用c++11标准和vcpkg进行包管理

编译环境仅测试过`Windows`

使用到的库：
- RapidJSON
- Boost
- cpr

[图文说明档](https://jrsgslb.cn)~~持续咕咕咕~~