#pragma once

#define MIRAICPP_STATICLIB
#include <string>
#include <rapidjson/document.h>
#include <mirai.h>

using namespace std;
using namespace rapidjson;
using namespace Cyan;

//pixiv��ȡָ��idͼƬ
bool Pixiv_id(bool proxy, string& proxy_rule, string& proxy_add, string pid, MiraiBot& bot, int64_t group_id, int msid);
//pixiv���а�
bool Pixiv_rank(bool proxy, string& proxy_rule, string& proxy_add, int count, MiraiBot& bot, int64_t group_id);