#pragma once

#define MIRAICPP_STATICLIB
#include "mirai.h"

using namespace Cyan;

//Ë¢ĞÂÖ±²¥¶©ÔÄuid
void Reload_live_uid();
//Ö±²¥¶©ÔÄ
void Bilibili_live(MiraiBot& bot);
//ÈÈÃÅcos
bool Bilibili_cos(MiraiBot& bot, GID_t gid);
//Ë¢ĞÂ¶¯Ì¬¶©ÔÄuid
void Reload_dynamic_uid();
//½âÎöbÕ¾¶¯Ì¬º¯Êı
MessageChain Bilibili_parse_dynamic(int type, string json);
//¶¯Ì¬¶©ÔÄ
void Bilibili_dynamic(MiraiBot& bot);
//bÕ¾¶©ÔÄÈºÁÄÆ¥Åä
void Bilibili_match(MiraiBot& bot, string type, string mode, string uid, int64_t gid_64, int64_t qq_64, int64_t master, int64_t msid);
//bÕ¾¶©ÔÄË½ÁÄÆ¥Åä
void Bilibili_match(MiraiBot& bot, string type, string mode, string uid, int64_t qq_64, int64_t master);