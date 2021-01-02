// 注意: 本项目的所有源文件都必须是 UTF-8 编码

#include <iostream>
#include <map>
#include <mirai.h>
#include <windows.h>
#include "myheader.h"
#include <stdio.h>
#include <fstream>
#include <ctime>

#include "yande.h"
#include "massgage.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>

#include "Json.h"
#include "json/json.h"
#include "../mirai-cpp/src/mirai_bot.cpp"

#define BOOST_SPIRIT_THREADSAFE

int main()
{
	using namespace std;
	using namespace Cyan;

#if defined(WIN32) || defined(_WIN32)
	// 切换代码页，让 CMD 可以显示 UTF-8 字符
	system("chcp 65001");
#endif

	MiraiBot bot("127.0.0.1", 4233);
	while (true)
	{
		try
		{
			bot.Auth("INITKEYXayIRiSH", 2901025495_qq);
			break;
		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
		}
		MiraiBot::SleepSeconds(1);
	}
	cout << "Bot Working..." << endl;

	map<GID_t, bool> groups;

	bot.On<GroupMessage>(
		[&](GroupMessage m)
		{
			try
			{
				string plain = m.MessageChain.GetPlainText();
				massage(plain);
				if (plain == "test")
				{
					m.Reply(MessageChain().Plain("..."));
					/*
					int yand = yande(m.Sender.Group.GID);
					if(yand == 2) m.Reply(MessageChain().Plain("网络错误"));
					else
					{
						GroupImage img = bot.UploadGroupImage("./sky1.jpg");
						bot.SendMessage(m.Sender.Group.GID, MessageChain().FlashImage(img));
						remove("./skt.jpg");
					}
					*/
					return;
				}
			}
			catch (const std::exception& ex)
			{
				cout << ex.what() << endl;
			}
		});

	//消息撤回事件
	bot.On<BotInvitedJoinGroupRequestEvent>(
		[&](BotInvitedJoinGroupRequestEvent e)
		{
			cout << "邀请你入群：" << e.GroupName << ", " << e.Message << endl;
			e.Reject();
		});


	bot.EventLoop();

	return 0;
}