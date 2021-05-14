#include <iostream>
#include <time.h>
#include <future>

#include "../include/Timing.h"
#include "../include/yande.h"
#include "../include/Bilibili.h"

using namespace std;

//循环查询时间
void TimeLoop(int ReoladTime, bool proxy, string& https, MiraiBot& bot, int64_t master)
{
	bool Reload = false, first = true;
	int64_t old_time, new_time;
	while (true)
	{
		time_t now = time(0);
		new_time = now;
		tm* ltm = localtime(&now);

		if (first)
		{
			first = false;
			old_time = new_time;
			if (!Reload_live_uid())
			{
				QQ_t qq = QQ_t(master);
				bot.SendMessage(qq, MessageChain().Plain("发生影响运行错误，将终止时间线程，详见控制台"));
				return;
			}
		}

		if (ltm->tm_hour == 2 && !Reload)
		{
			printf("AUTO ： 开始更新Tag\n");
			Reload = true;
			auto a  = async(MessageReload, ref(proxy), ref(https));
		}
		else if(ltm->tm_hour != 2)
		{
			Reload = false;
		}
		if (new_time - old_time >= ReoladTime)
		{
			auto a = async(Bilibili_live, ref(bot));
			old_time = new_time;
		}

		_sleep(1 * 1000);
	}
}
/*
cout << "时间: " << ltm->tm_hour << ":";
		cout << ltm->tm_min << ":";
		cout << ltm->tm_sec << endl;
*/