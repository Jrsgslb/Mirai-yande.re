#pragma once
#include "../include/include.h"

#include <direct.h>

bool StartCheck();
bool MessageCheck(string plain);
string MessageReload(bool proxy, string https);
bool MessageLimit(string plain, int64_t qq_num, int64_t group_num, bool admin);
bool MessageR18(int64_t qq_num, int64_t group_num, bool R18);