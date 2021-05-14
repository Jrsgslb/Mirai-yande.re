#pragma once
#include <string>
#include <rapidjson/document.h>
#include <mirai.h>
using namespace std;
using namespace rapidjson;
using namespace Cyan;

//pixiv获取指定id图片
bool Pixiv_id(bool proxy, string& https, string pid, MiraiBot& bot, int64_t group_id, int msid);
//pixiv排行榜
bool Pixiv_rank(bool proxy, string& https, int count, MiraiBot& bot, int64_t group_id);