#include <iostream>
#include <time.h>

#include "../include/Timing.h"
#include "../include/yande.h"

using namespace std;

//循环查询时间
void TimeLoop(int ReoladTime, bool proxy, string https)
{
	bool Reload = false;
	while (true)
	{
		time_t now = time(0);

		tm* ltm = localtime(&now);
		if (ltm->tm_hour == 2)
		{
			if (!Reload)
			{
				cout << "AUTO ： 开始更新Tag";
				MessageReload(proxy, https);
				Reload = true;
			}
		}
		else
		{
			Reload = false;
		}
		
		_sleep(ReoladTime * 1000);
	}
}
/*
cout << "时间: " << ltm->tm_hour << ":";
		cout << ltm->tm_min << ":";
		cout << ltm->tm_sec << endl;
*/