#pragma once
#include <string>
#include <rapidjson/document.h>
#include <mirai.h>
using namespace std;
using namespace rapidjson;
using namespace Cyan;

//pixiv��ȡָ��idͼƬ
bool Pixiv_id(bool proxy, string& https, string pid, MiraiBot& bot, int64_t group_id, int msid);
//pixiv���а�
bool Pixiv_rank(bool proxy, string& https, int count, MiraiBot& bot, int64_t group_id);