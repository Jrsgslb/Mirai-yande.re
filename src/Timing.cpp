#include <iostream>
#include <time.h>

#include "../include/Timing.h"
#include "../include/yande.h"

using namespace std;

//ѭ����ѯʱ��
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
				cout << "AUTO �� ��ʼ����Tag";
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
cout << "ʱ��: " << ltm->tm_hour << ":";
		cout << ltm->tm_min << ":";
		cout << ltm->tm_sec << endl;
*/