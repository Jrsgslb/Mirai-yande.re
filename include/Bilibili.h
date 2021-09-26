#pragma once

#include "mirai.h"

using namespace Cyan;

//刷新直播订阅uid
void Reload_live_uid();
//直播订阅
void Bilibili_live(MiraiBot& bot);
//热门cos
bool Bilibili_cos(MiraiBot& bot, GID_t gid);
//刷新动态订阅uid
void Reload_dynamic_uid();
//解析b站动态函数
MessageChain Bilibili_parse_dynamic(int type, string json);
//动态订阅
void Bilibili_dynamic(MiraiBot& bot);
//b站订阅群聊匹配
void Bilibili_match(MiraiBot& bot, string type, string mode, string uid, int64_t gid_64, int64_t qq_64, int64_t master, int64_t msid);
//b站订阅私聊匹配
void Bilibili_match(MiraiBot& bot, string type, string mode, string uid, int64_t qq_64, int64_t master);