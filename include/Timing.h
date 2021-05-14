#pragma once
#include <mirai.h>

using namespace Cyan;

//死循环查询时间
void TimeLoop(int ReoladTime, bool proxy, string& https, MiraiBot& bot, int64_t master);