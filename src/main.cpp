// 注意: 本项目的所有源文件都必须是 UTF-8 编码
#include <iostream>
#include <mirai.h>
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include<sstream>
#include <direct.h>
#include <vector>
#include <io.h>

#include "yande.h"
#include "message.h"
#include "ClearTemp.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp> 


int main()
{
	using namespace std;
	using namespace Cyan;
	using namespace rapidjson;
	using namespace boost::property_tree;

#if defined(WIN32) || defined(_WIN32)
	// 切换代码页，让 CMD 可以显示 UTF-8 字符
	system("chcp 65001");
	SetConsoleTitle("Mirai-yande.re");
#endif

	//检测并创建文件夹
	if (_access("./temp", 0) == -1)_mkdir("./temp");
	if (_access("./config", 0) == -1)_mkdir("./config");
	if (_access("./config/data", 0) == -1)_mkdir("./config/data");
	//文件不存在自动创建
	fstream fs1, fs2;
	fs1.open("./config/data/member.ini", ios::in);
	fs2.open("./config/data/group.ini", ios::in);
	if (!fs1)
	{
		ofstream fout("./config/data/member.ini");
		if (fout) fout.close();
	}
	else
		fs1.close();
	if (!fs2)
	{
		ofstream fout("./config/data/group.ini");
		if (fout) fout.close();
	}
	else
		fs2.close();


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
				bool admin;
				if (adminer == "ADMINISTRATOR" || adminer == "OWNER" || m.Sender.QQ.ToInt64() == d["主人"].GetInt64()) admin = true;
				else admin = false;

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
					return;
				}

				if (plain == d["R18开启"].GetString() && admin)
				{
					if (MessageR18(m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64(), true))
						m.QuoteReply(MessageChain().Plain("开启成功"));
					else
						m.QuoteReply(MessageChain().Plain("请勿重复开启哦"));
				}

				if (plain == d["R18关闭"].GetString() && admin)
				{
					if (MessageR18(m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64(), false))
						m.QuoteReply(MessageChain().Plain("关闭成功"));
					else
						m.QuoteReply(MessageChain().Plain("请勿重复关闭哦"));
				}

				if (MessageCheck(plain))
				{
					if (MessageLimit(plain, m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64(), admin))
					{
						m.QuoteReply(MessageChain().Plain(d["发送提示语"].GetString()));
						//获取tag信息
						ptree p;
						ini_parser::read_ini("./config/rule.ini", p);
						basic_ptree<string, string> tag = p.get_child(plain);
						int max_send = tag.get<int>("send",1);
						for (int i = 1; i <= max_send; i++)
						{
							vector<string> yand;
							yand = yande(plain, d["是否使用代理"].GetBool(), d["http"].GetString(), d["https"].GetString(), d["发送原图"].GetBool(), m.Sender.Group.GID.ToInt64());

							if (yand.size() == 1) m.Reply(MessageChain().Plain(yand[0]));
							else
							{
								//发送图片并处理发送完成事宜
								//处理优先级：撤回>清除缓存
								if (DownloadImg(yand[1],yand[2], d["是否使用代理"].GetBool(), d["http"].GetString(), d["https"].GetString()))
								{
									GroupImage img = bot.UploadGroupImage(yand[2]);
									int MsId = bot.SendMessage(m.Sender.Group.GID, MessageChain().Image(img));

									if (d["是否撤回"].GetBool())
									{
										_sleep(d["撤回延时"].GetInt() * 1000);
										bot.Recall(MsId);
									}

									if (!d["是否缓存图片"].GetBool())
									{
										_sleep(5 * 1000);
										remove(yand[2].c_str());
									}
									yand.clear();
								}
								else
								{
									m.QuoteReply(MessageChain().Plain("网络错误"));
									return;
								}
							}
						}
					}
					return;
				}
			}
			catch (const std::exception& ex)
			{
				cout << ex.what() << endl;
			}
		});

	bot.On<FriendMessage>(
		[&](FriendMessage m)
		{
			string plain = m.MessageChain.GetPlainText();
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
		});

	bot.EventLoop();

	return 0;
}