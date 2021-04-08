#pragma once
#include <string>
#include <rapidjson/document.h>
#include <mirai.h>
using namespace std;
using namespace rapidjson;
using namespace Cyan;

bool a2d_search(bool proxy, string& https, string url, MiraiBot& bot, MessageType type, int64_t id, int msid);
bool snao_search(bool proxy, string& https, string url, MiraiBot& bot, MessageType type, int64_t id, int msid);