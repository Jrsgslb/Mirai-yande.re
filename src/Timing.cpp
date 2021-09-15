#include <iostream>
#include <time.h>
#include <future>

#include <rapidjson/pointer.h>

#include "../include/Timing.h"
#include "../include/yande.h"
#include "../include/Bilibili.h"

#include <HttpRequest.hpp>

using namespace std;
using namespace rapidjson;

Document search_json;
vector<string> qq_num;
struct Search_time{
	string time;
	int64_t group;
	int64_t member;
};

//循环查询时间
void TimeLoop(int ReoladTime, int Search_time, bool proxy, string proxy_rule, string proxy_add, MiraiBot& bot, int64_t master)
{
	bool Reload = false, first = true;
	int64_t old_time;
	Pointer("/code").Create(search_json, 0);
	while (true)
	{
		time_t now = time(0);
		tm* ltm = localtime(&now);

		if (first)
		{
			first = false;
			old_time = now;
			Reload_live_uid();
			Reload_dynamic_uid();
		}

		if (ltm->tm_hour == 2 && !Reload)
		{
			printf("AUTO : 开始更新Tag\n");
			Reload = true;
			auto a  = async(MessageReload, ref(proxy), ref(proxy_rule), ref(proxy_add));
		}
		else if(ltm->tm_hour != 2)
		{
			Reload = false;
		}
		if (now - old_time >= ReoladTime)
		{
			auto a = async(Bilibili_live, ref(bot));
			auto b = async(Bilibili_dynamic, ref(bot));
			old_time = now;
		}
		//搜图检测
		Search_status(Search_time, now, bot);
		_sleep(1 * 1000);
	}
}
/*
cout << "时间: " << ltm->tm_hour << ":";
		cout << ltm->tm_min << ":";
		cout << ltm->tm_sec << endl;
*/
//搜图开关函数
bool Search_switch(bool status, int64_t member, int64_t group)
{
	string temp, str_member;
	time_t now = time(0);
	str_member = to_string(member);
	if (status)
	{
		if (!Pointer("").Get(search_json)->HasMember(str_member.c_str()))
		{
			temp = "/" + str_member + "/0/group";
			Pointer(temp.c_str()).Set(search_json, group);
			temp = "/" + str_member + "/0/time";
			Pointer(temp.c_str()).Set(search_json, now);
		}
		else
		{
			string point;
			point = "/" + str_member;
			for (int i = 0; i < Pointer(point.c_str()).Get(search_json)->Size(); i++)
			{
				temp = "/" + str_member + "/" + to_string(i) + "/group";
				if (Pointer(temp.c_str()).Get(search_json)->GetInt64() == group)
				{
					temp = "/" + str_member + "/" + to_string(i) + "/time";
					Pointer(temp.c_str()).Set(search_json, now);
					return false;
				}
			}
			temp = "/" + str_member + "/-/group";
			Pointer(temp.c_str()).Set(search_json, group);
			temp = "/" + str_member + "/" + to_string(Pointer(point.c_str()).Get(search_json)->Size() - 1) + "/time";
			Pointer(temp.c_str()).Set(search_json, now);
		}
		
		vector<string>::iterator it = find(qq_num.begin(), qq_num.end(), str_member);
		if (it == qq_num.end())
		{
			qq_num.push_back(str_member);
		}
	}
	else
	{
		if (!Pointer("").Get(search_json)->HasMember(str_member.c_str()))
		{
			return false;
		}
		temp = "/" + str_member;
		int size = Pointer(temp.c_str()).Get(search_json)->Size();
		if (size == 1)
		{
			Pointer(temp.c_str()).Erase(search_json);
			for (vector<string>::iterator iter = qq_num.begin(); iter != qq_num.end(); iter++)
			{
				if (*iter == str_member)
				{
					qq_num.erase(iter);
					break;
				}
			}
		}
		else
		{
			for (int i = 0; i < size; i++)
			{
				temp = "/" + str_member + "/" + to_string(i) + "/group";
				if (Pointer(temp.c_str()).Get(search_json)->GetInt64() == group)
				{
					temp = "/" + str_member + "/" + to_string(i);
					Pointer(temp.c_str()).Erase(search_json);
				}
			}
		}
	}
	return true;
}
//搜图状态检测
void Search_status(int Search_time, int64_t new_time, MiraiBot& bot)
{
	if (qq_num.empty())
	{
		return;
	}
	string temp;
	for (int i = 0; i < qq_num.size(); i++)
	{
		temp = "/" + qq_num[i];
		int size = Pointer(temp.c_str()).Get(search_json)->Size();
		if (size == 1)
		{
			temp = temp + "/0/time";
			if (new_time - Pointer(temp.c_str()).Get(search_json)->GetInt64() >= Search_time)
			{
				temp = "/" + qq_num[i] + "/0/group";
				bot.SendMessage(GID_t(Pointer(temp.c_str()).Get(search_json)->GetInt64()), MessageChain().At(QQ_t(_atoi64(qq_num[i].c_str()))).Plain(" 超时自动退出搜图模式"));
				temp = "/" + qq_num[i];
				Pointer(temp.c_str()).Erase(search_json);
				for (vector<string>::iterator iter = qq_num.begin(); iter != qq_num.end(); iter++)
				{
					if (*iter == qq_num[i])
					{
						qq_num.erase(iter);
						break;
					}
				}
			}
		}
		else
		{
			for (int j = 0; j < size; j++)
			{
				temp = "/" + qq_num[i] + "/" + to_string(j) + "/time";
				if (new_time - Pointer(temp.c_str()).Get(search_json)->GetInt64() >= Search_time)
				{
					temp = "/" + qq_num[i] + "/" + to_string(j) + "/group";
					bot.SendMessage(GID_t(Pointer(temp.c_str()).Get(search_json)->GetInt64()), MessageChain().At(QQ_t(_atoi64(qq_num[i].c_str()))).Plain(" 超时自动退出搜图模式"));
					temp = "/" + qq_num[i] + "/" + to_string(j);
					Pointer(temp.c_str()).Erase(search_json);
					break;
				}
			}
		}
	}
}
//搜图状态查询
bool Search_check(int64_t member, int64_t group)
{
	string temp, str_member;
	str_member = to_string(member);
	if (Pointer("").Get(search_json)->HasMember(str_member.c_str()))
	{
		int size;
		temp = "/" + str_member;
		size = Pointer(temp.c_str()).Get(search_json)->Size();
		if (size == 1)
		{
			temp = temp + "/0/group";
			if (Pointer(temp.c_str()).Get(search_json)->GetInt64() == group)
			{
				return true;
			}
		}
		else
		{
			for (int j = 0; j < size; j++)
			{
				temp = "/" + str_member + "/" + to_string(j) + "/group";
				if (Pointer(temp.c_str()).Get(search_json)->GetInt64() == group)
				{
					return true;
				}
			}
		}
	}
	return false;
}
//版本检测
void CheckVersion(string mahVersion, string mcppVersion, string myVersion)
{
	//版本检查
	printf("当前Mirai-Api-Http版本为: %s \n", mahVersion.c_str());
	printf("当前Mirai-Cpp版本为: %s \n", mcppVersion.c_str());
	printf("当前Mirai-yande.re编译版本为: %s \n", myVersion.c_str());
	HttpRequest r;
	if (r.Http_Get("https://jrsgslb.cn/bot/Version/") != myVersion)
	{
		printf("\n检测到Mirai-yande.re有新编译版本,请前往 https://github.com/Jrsgslb/Mirai-yande.re/releases 下载 \n\n");
	}
	printf("\nBot已上线\n\n如果刷新 mah.json 文件，请重启程序\n\n");
	return;
}