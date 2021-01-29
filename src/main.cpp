// 注意: 本项目的所有源文件都必须是 UTF-8 编码
#include <iostream>
#include <mirai.h>
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include<sstream>
#include <direct.h>
#include <io.h>

#include "yande.h"
#include "message.h"
#include "ClearTemp.h"

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>


int main()
{
	using namespace std;
	using namespace Cyan;
	using namespace rapidjson;

#if defined(WIN32) || defined(_WIN32)
	// 切换代码页，让 CMD 可以显示 UTF-8 字符
	system("chcp 65001");
	SetConsoleTitle("Mirai-yande.re");
#endif

	//检测并创建文件夹
	if (_access("./temp", 0) == -1)_mkdir("./temp");
	if (_access("./config", 0) == -1)_mkdir("./config");
	if (_access("./config/data", 0) == -1)_mkdir("./config/data");


	//读取config

	FILE* fp = fopen("./config.json", "rb");

	if (fp == NULL)
	{
		cout << "Error reading configuration file, please check whether the configuration file exists.\n";
		system("pause");
		return 0;
	}

	char readBuffer[10000];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;
	d.ParseStream(is);

	fclose(fp);
	int64_t port, QQ_num;

	port = atoi(d["port"].GetString());
	stringstream stream;
	stream << d["qq"].GetString();
	stream >> QQ_num;
	stream.clear();
	QQ_t qq = QQ_t(QQ_num);

	

	//自动登录
	MiraiBot bot(d["host"].GetString(),port );
	while (true)
	{
		try
		{
			bot.Auth(d["key"].GetString(), qq);
			break;
		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
		}
		MiraiBot::SleepSeconds(1);
	}
	cout << "Bot Working..." << endl << "\nAll config files of this program adopt UTF-8 character encoding.\n\nReload the config file, please restart the program...\n\n";

	bot.On<GroupMessage>(
		[&](GroupMessage m)
		{
			try
			{
				string plain = m.MessageChain.GetPlainText(), adminer = GroupPermissionStr(m.Sender.Permission);
				if (MessageCheck(plain))
				{
					bool admin;
					if (adminer == "ADMINISTRATOR" || adminer == "OWNER") admin = true;
					else admin = false;
					if (MessageLimit(plain, m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64(), admin))
					{
						m.QuoteReply(MessageChain().Plain(d["发送提示语"].GetString()));
						string yand;
						assert(d["是否使用代理"].IsBool());
						if (d["是否使用代理"].GetBool())
							yand = yande(plain, true, d["http"].GetString(), d["https"].GetString(), d["发送原图"].GetBool());
						else
							yand = yande(plain, false, "123", "123", d["发送原图"].GetBool());

						if (yand == "error") m.Reply(MessageChain().Plain("网络错误或tag填写错误"));
						else if (yand == "r18") m.Reply(MessageChain().Plain("这张图不适合在本群观看"));
						else
						{
							//发送图片并处理发送完成事宜
							//处理优先级：撤回>清除缓存
							GroupImage img = bot.UploadGroupImage(yand);
							int MsId = bot.SendMessage(m.Sender.Group.GID, MessageChain().Image(img));

							if (d["是否撤回"].GetBool())
							{
								_sleep(d["撤回延时"].GetInt()*1000);
								bot.Recall(MsId);
							}

							if (!d["是否缓存图片"].GetBool())
							{
								_sleep(5*1000);
								remove(yand.c_str());
							}
						}
					}
					return;
				}

				if (plain == "更新tag" && m.Sender.QQ.ToInt64() == d["主人"].GetInt64()) {
					string tag;
					m.QuoteReply(MessageChain().Plain("更新中..."));
					if (d["是否使用代理"].GetBool())
						tag = MessageReload(true, d["http"].GetString(), d["https"].GetString());
					else
						tag = MessageReload(false, d["http"].GetString(), d["https"].GetString());
					if (tag == "ok")m.QuoteReply(MessageChain().Plain("更新完成"));
					else
					{
						tag = "更新出错，出错的tag有:\n" + tag;
						m.Reply(MessageChain().Plain(tag));
					}
					return;
				}
				
				if (plain == "清理缓存" && m.Sender.QQ.ToInt64() == d["主人"].GetInt64())
				{
					if (ClearTemp()) m.QuoteReply(MessageChain().Plain("清理成功"));
					else
						m.QuoteReply(MessageChain().Plain("清理失败"));
				}
			}
			catch (const std::exception& ex)
			{
				cout << ex.what() << endl;
			}
		});

	bot.EventLoop();

	return 0;
}