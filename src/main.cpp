// 注意: 本项目的所有源文件都必须是 UTF-8 编码
#include <mirai.h>

#include "../include/yande.h"
#include "../include/message.h"
#include "../include/imgsearch.h"
#include <rapidjson\writer.h>


int main()
{
	if (!StartCheck())
	{
		cout << "DLL加载错误";
		return 0;
	}

	using namespace Cyan;


#if defined(WIN32) || defined(_WIN32)
	// 切换代码页，让 CMD 可以显示 UTF-8 字符
	system("chcp 65001");
	SetConsoleTitle("Mirai-yande.re");
#endif

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
			bot.SendMessage(QQ_t(d["主人"].GetInt64()),MessageChain().Plain("Bot已上线\n刷新配置文件请重启Mirai-yande"));
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
				auto mc = m.MessageChain;
				auto qms = mc.GetAll<QuoteMessage>();
				string plain = mc.GetPlainText(), adminer = GroupPermissionStr(m.Sender.Permission);
				bool admin;
				if (!qms.empty())
				{
					auto id = qms[0].MessageId();
					if (strstr(plain.c_str(), d["搜图"].GetString()) != NULL || m.AtMe())
					{
						Document ms;
						ms.Parse(bot.GetGroupMessageFromId(id).ToString().c_str());
						string type = Pointer("/messageChain/0/type").Get(ms)->GetString();
						if (type == "Image")
						{
							Document p;
							p = a2d_search(d["是否使用代理"].GetBool(), d["proxy"].GetString(), Pointer("/messageChain/0/url").Get(ms)->GetString());
							if (Pointer("/code").Get(p)->GetInt() != 0)
							{
								m.QuoteReply(MessageChain().Plain("搜索出错了，请稍后重试"));
								return;
							}
							if (DownloadImg(Pointer("/color/url").Get(p)->GetString(), Pointer("/color/name").Get(p)->GetString(), d["是否使用代理"].GetBool(), d["proxy"].GetString()) && DownloadImg(Pointer("/bovw/url").Get(p)->GetString(), Pointer("/bovw/name").Get(p)->GetString(), d["是否使用代理"].GetBool(), d["proxy"].GetString()))
							{
								string repl_color, repl_bovw;
								GroupImage img_clolr = bot.UploadGroupImage(Pointer("/color/name").Get(p)->GetString());
								GroupImage img_bovw = bot.UploadGroupImage(Pointer("/bovw/name").Get(p)->GetString());
								repl_color = Pointer("/color/pic/name").Get(p)->GetString();
								repl_color = repl_color + "\n原图地址:" + Pointer("/color/pic/url").Get(p)->GetString() + "\nAuthor:" + Pointer("/color/user/url").Get(p)->GetString();
								repl_bovw = Pointer("/bovw/pic/name").Get(p)->GetString();
								repl_bovw = repl_bovw + "\n原图地址:" + Pointer("/bovw/pic/url").Get(p)->GetString() + "\nAuthor:" + Pointer("/bovw/user/url").Get(p)->GetString();
								m.QuoteReply(MessageChain().Plain("色调搜索结果:").Image(img_clolr).Plain(repl_color));
								m.QuoteReply(MessageChain().Plain("特征搜索结果:").Image(img_bovw).Plain(repl_bovw));

								if (!d["是否缓存图片"].GetBool())
								{
									_sleep(5 * 1000);
									remove(Pointer("/color/name").Get(p)->GetString());
									remove(Pointer("/bovw/name").Get(p)->GetString());
								}
							}
							else
							{
								m.QuoteReply(MessageChain().Plain("网络错误，搜索结果可能不包含图片"));
								string repl_color, repl_bovw;
								repl_color = Pointer("/color/pic/name").Get(p)->GetString();
								repl_color = repl_color + "\n原图地址:" + Pointer("/color/pic/url").Get(p)->GetString() + "\nAuthor:" + Pointer("/color/user/url").Get(p)->GetString();
								repl_bovw = Pointer("/bovw/pic/name").Get(p)->GetString();
								repl_bovw = repl_bovw + "\n原图地址:" + Pointer("/bovw/pic/url").Get(p)->GetString() + "\nAuthor:" + Pointer("/bovw/user/url").Get(p)->GetString();
								m.QuoteReply(MessageChain().Plain("色调搜索结果:").Plain(repl_color));
								m.QuoteReply(MessageChain().Plain("特征搜索结果:").Plain(repl_bovw));
							}
							return;
						}
					}
					return;
				}
				
				if (adminer == "ADMINISTRATOR" || adminer == "OWNER" || m.Sender.QQ.ToInt64() == d["主人"].GetInt64()) admin = true;
				else admin = false;

				if (plain == "随机cos")
				{
					GroupImage cos;
					cos.Url = "https://api.jrsgslb.cn/cos/url.php?return=img";
					m.Reply(MessageChain().Image(cos));
					return;
				}

				if (plain == "更新tag" && m.Sender.QQ.ToInt64() == d["主人"].GetInt64()) {
					string tag;
					m.QuoteReply(MessageChain().Plain("更新中..."));
					if (d["是否使用代理"].GetBool())
						tag = MessageReload(true, d["proxy"].GetString());
					else
						tag = MessageReload(false, d["proxy"].GetString());
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

				if (plain == "菜单" || plain == "help")
				{
					ifstream in("./config/command.txt");
					string i,menu;
					while (getline(in, i))
					{
						menu = "\n" + i + menu;
					}
					m.Reply(MessageChain().At(m.Sender.QQ).Plain(menu.c_str()));

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
							yand = yande(plain, d["是否使用代理"].GetBool(), d["proxy"].GetString(), d["发送原图"].GetBool(), m.Sender.Group.GID.ToInt64());

							if (yand.size() == 1) m.Reply(MessageChain().Plain(yand[0]));
							else
							{
								//发送图片并处理发送完成事宜
								//处理优先级：撤回>清除缓存
								if (DownloadImg(yand[1],yand[2], d["是否使用代理"].GetBool(), d["proxy"].GetString()))
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
					else
					{
						m.QuoteReply(MessageChain().Plain("要懂得节制哦 QAQ"));
					}
					return;
				}
			}
			catch (const std::exception& ex)
			{
				cout << ex.what() << endl;
				bot.SendMessage(QQ_t(d["主人"].GetInt64()), MessageChain().Plain(ex.what()));
			}
		});

	bot.On<FriendMessage>(
		[&](FriendMessage m)
		{
			try
			{
				auto plain = m.MessageChain;
				if (plain.GetPlainText() == "更新tag" && m.Sender.QQ.ToInt64() == d["主人"].GetInt64()) 
				{
					string tag;
					m.QuoteReply(MessageChain().Plain("更新中..."));
					if (d["是否使用代理"].GetBool())
						tag = MessageReload(true, d["proxy"].GetString());
					else
						tag = MessageReload(false, d["proxy"].GetString());
					if (tag == "ok")m.QuoteReply(MessageChain().Plain("更新完成"));
					else
					{
						tag = "更新出错，出错的tag有:\n" + tag;
						m.Reply(MessageChain().Plain(tag));
					}
					return;
				}

			
				Document img;
				img.Parse(plain.ToString().c_str());
				if (strstr(Pointer("/0/type").Get(img)->GetString(), "Image"))
				{
					Document p;
					p = a2d_search(d["是否使用代理"].GetBool(), d["proxy"].GetString(), Pointer("/0/url").Get(img)->GetString());
					if (Pointer("/code").Get(p)->GetInt() != 0)
					{
						m.Reply(MessageChain().Plain("搜索出错了，请稍后重试"));
						return;
					}
					if (DownloadImg(Pointer("/color/url").Get(p)->GetString(), Pointer("/color/name").Get(p)->GetString(), d["是否使用代理"].GetBool(), d["proxy"].GetString()) && DownloadImg(Pointer("/bovw/url").Get(p)->GetString(), Pointer("/bovw/name").Get(p)->GetString(), d["是否使用代理"].GetBool(), d["proxy"].GetString()))
					{
						string repl_color, repl_bovw;
						FriendImage img_clolr = bot.UploadFriendImage(Pointer("/color/name").Get(p)->GetString());
						FriendImage img_bovw = bot.UploadFriendImage(Pointer("/bovw/name").Get(p)->GetString());
						repl_color = Pointer("/color/pic/name").Get(p)->GetString();
						repl_color = repl_color + "\n原图地址:" + Pointer("/color/pic/url").Get(p)->GetString() + "\nAuthor:" + Pointer("/color/user/url").Get(p)->GetString();
						repl_bovw = Pointer("/bovw/pic/name").Get(p)->GetString();
						repl_bovw = repl_bovw + "\n原图地址:" + Pointer("/bovw/pic/url").Get(p)->GetString() + "\nAuthor:" + Pointer("/bovw/user/url").Get(p)->GetString();
						m.Reply(MessageChain().Plain("色调搜索结果:").Image(img_clolr).Plain(repl_color));
						m.Reply(MessageChain().Plain("特征搜索结果:").Image(img_bovw).Plain(repl_bovw));

						if (!d["是否缓存图片"].GetBool())
						{
							_sleep(5 * 1000);
							remove(Pointer("/color/name").Get(p)->GetString());
							remove(Pointer("/bovw/name").Get(p)->GetString());
						}
					}
					else
					{
						m.Reply(MessageChain().Plain("网络错误，搜索结果可能不包含图片"));
						string repl_color, repl_bovw;
						repl_color = Pointer("/color/pic/name").Get(p)->GetString();
						repl_color = repl_color + "\n原图地址:" + Pointer("/color/pic/url").Get(p)->GetString() + "\nAuthor:" + Pointer("/color/user/url").Get(p)->GetString();
						repl_bovw = Pointer("/bovw/pic/name").Get(p)->GetString();
						repl_bovw = repl_bovw + "\n原图地址:" + Pointer("/bovw/pic/url").Get(p)->GetString() + "\nAuthor:" + Pointer("/bovw/user/url").Get(p)->GetString();
						m.Reply(MessageChain().Plain("色调搜索结果:").Plain(repl_color));
						m.Reply(MessageChain().Plain("特征搜索结果:").Plain(repl_bovw));
					}
					return;
				}
			}
			catch (const std::exception& err)
			{
				cout << err.what();
				bot.SendMessage(QQ_t(d["主人"].GetInt64()), MessageChain().Plain(err.what()));
			}
		});

	bot.EventLoop();

	return 0;
}