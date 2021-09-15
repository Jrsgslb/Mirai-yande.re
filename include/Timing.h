#pragma once

#define MIRAICPP_STATICLIB
#include <mirai.h>
using namespace Cyan;

//ËÀÑ­»·²éÑ¯Ê±¼ä
void TimeLoop(int ReoladTime, int Search_time, bool proxy, string proxy_rule, string proxy_add, MiraiBot& bot, int64_t master);
//ËÑÍ¼¿ª¹Øº¯Êý
bool Search_switch(bool status, int64_t member, int64_t group);
//ËÑÍ¼×´Ì¬¼ì²â
void Search_status(int Search_time, int64_t new_time, MiraiBot& bot);
//ËÑÍ¼×´Ì¬²éÑ¯
bool Search_check(int64_t member, int64_t group);
//°æ±¾¼ì²â
void CheckVersion(string mahVersion, string mcppVersion, string myVersion);