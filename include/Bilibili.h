#pragma once
#include "mirai/mirai.h"

using namespace Cyan;

//刷新直播订阅uid
bool Reload_live_uid();
//直播订阅
void Bilibili_live(MiraiBot& bot);
//热门cos
bool Bilibili_cos(MiraiBot& bot, GID_t gid);