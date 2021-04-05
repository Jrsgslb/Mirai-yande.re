#pragma once
#include <string>
#include <rapidjson/document.h>
using namespace std;
using namespace rapidjson;

bool StartCheck();
bool MessageCheck(string plain);
bool MessageLimit(string plain, int64_t qq_num, int64_t group_num, bool admin);
bool MessageR18(int64_t qq_num, int64_t group_num, bool R18);
Document ReloadConfig();