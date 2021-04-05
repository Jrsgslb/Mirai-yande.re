#pragma once
#include <string>
#include <rapidjson/document.h>
using namespace std;
using namespace rapidjson;

Document yande(string plain, bool proxy, string https, int64_t group_num, bool first, bool original);
Document yid(string id, bool proxy, string https, int64_t group_num);
static __int64 GetUnixTime();
bool ClearTemp();
string MessageReload(bool proxy, string https);