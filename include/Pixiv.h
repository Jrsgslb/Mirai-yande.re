#pragma once

#define MIRAICPP_STATICLIB
#include <string>
#include <rapidjson/document.h>
#include <mirai.h>

using namespace std;
using namespace rapidjson;
using namespace Cyan;

//pixiv获取指定id图片
bool Pixiv_id(bool proxy, string& proxy_rule, string& proxy_add, string pid, MiraiBot& bot, int64_t group_id, int msid);
//pixiv排行榜
bool Pixiv_rank(bool proxy, string& proxy_rule, string& proxy_add, int count, MiraiBot& bot, int64_t group_id);