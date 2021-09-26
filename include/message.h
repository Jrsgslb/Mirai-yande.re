#pragma once
#include <string>
#include <rapidjson/document.h>
using namespace std;
using namespace rapidjson;

//启动检查
void StartCheck();
//y站指令刷新减少文件读取次数
void CommandReload();
//y站自定义指令检查
bool MessageCheck(string plain);
//频率限制
bool MessageLimit(string plain, int64_t qq_num, int64_t group_num, bool admin);
//r18设定
bool MessageR18(int64_t qq_num, int64_t group_num, bool R18);
//读取文件
string ReloadFile(const char* file);
//写入文件
bool WriteFile(const char* file, string txt);