#pragma once
#include <string>
#include <rapidjson/document.h>
using namespace std;
using namespace rapidjson;

//y站自定义指令发图
Document yande(string plain, bool proxy, string proxy_rule, string proxy_add, int64_t group_num, bool first, bool original);
//y站获取指定id图片
Document yid(string id, bool proxy, string proxy_rule, string proxy_add, int64_t group_num);
//获取13位时间戳
long long GetUnixTime();
//清理temp文件夹
bool ClearTemp();
//更新y站tag
string MessageReload(bool proxy, string proxy_rule, string proxy_add);
//热门排行榜图片
Document Hot_Img(bool proxy, string proxy_rule, string proxy_add, int64_t group, bool orginal);