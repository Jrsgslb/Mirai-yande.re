#pragma once

#include <string>
#include <rapidjson/document.h>
#include <mirai.h>

using namespace std;
using namespace rapidjson;
using namespace Cyan;

bool a2d_search(bool proxy, string proxy_rule, string proxy_add, string url, MiraiBot& bot, MessageType type, int64_t id, int msid);
bool snao_search(bool proxy, string proxy_rule, string proxy_add, string url, MiraiBot& bot, MessageType type, int64_t id, int msid, int min_match, string api_key);