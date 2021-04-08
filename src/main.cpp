// 注意: 本项目的所有源文件都必须是 UTF-8 编码
#include <iostream>
#include <map>

#include <rapidjson/pointer.h>
#include <future>

#include <mirai.h>

#include "../include/Timing.h"
#include "../include/message.h"
#include "../include/imgsearch.h"
#include "../include/yande.h"
#include "../include/Bilibili.h"

int main()
{
	using namespace std;
	using namespace Cyan;

	if (!StartCheck())
	{
		cout << "DLL加载错误";
		return 0;
	}


#if defined(WIN32) || defined(_WIN32)
	// 切换代码页，让 CMD 可以显示 UTF-8 字符
	system("chcp 65001");
	SetConsoleTitle("Mirai-yande.re");
#endif

	//读取config
	Document d = ReloadConfig();

	
	int64_t port, QQ_num, master;

	port = atoi(d["port"].GetString());
	stringstream stream;
	stream << d["qq"].GetString();
	stream >> QQ_num;
	stream.clear();
	QQ_t qq = QQ_t(QQ_num);

	bool proxy = d["是否使用代理"].GetBool();
	string proxy_http;
	proxy_http = d["proxy"].GetString();
	master = d["主人"].GetInt64();

	//自动登录
	MiraiBot bot(d["host"].GetString(), port);
	while (true)
	{
		try
		{
			bot.Auth(d["key"].GetString(), qq);
			bot.SendMessage(QQ_t(master), MessageChain().Plain("Bot已上线\n刷新配置文件请重启Mirai-yande"));
			break;
		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
		}
		MiraiBot::SleepSeconds(1);
	}
	cout << "Bot Working..." << endl << "\nAll config files of this program adopt UTF-8 character encoding.\n\nReload the config file, please restart the program...\n\n";

	auto a = async(TimeLoop, d["更新时间"].GetInt(), proxy, proxy_http);
	cout << "已抛出时钟线程\n";

	map<int64_t, bool> search_map;

	bot.On<GroupMessage>(
		[&](GroupMessage m)
		{
			try
			{
				auto mc = m.MessageChain;
				auto qms = mc.GetAll<QuoteMessage>();
				string plain = mc.GetPlainText(), adminer = GroupPermissionStr(m.Sender.Permission);
				vector<ImageMessage> imgs = mc.GetAll<ImageMessage>();
				bool admin;
				//发送者权限判断
				if (adminer == "ADMINISTRATOR" || adminer == "OWNER" || m.Sender.QQ.ToInt64() == master)
				{
					admin = true;
				}
				else 
				{
					admin = false;
				}
				//图片消息
				if (imgs.size() != 0)
				{
					if (m.AtMe() || search_map[m.Sender.QQ.ToInt64()])
					{
						if (!snao_search(proxy, proxy_http, imgs[0].ToMiraiImage().Url, m.GetMiraiBot(), MessageType::GroupMessage, m.Sender.Group.GID.ToInt64(), m.MessageId()))
						{
							m.QuoteReply(MessageChain().Plain("发生错误，详见控制台"));
							return;
						}
						return;
					}

					return;
				}
				//随机cos
				if (plain == "随机cos" && MessageLimit("随机cos",m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64(), admin))
				{
					GroupImage cos;
					cos.Url = "https://api.jrsgslb.cn/cos/url.php?return=img";
					m.Reply(MessageChain().Image(cos));
					return;
				}
				//更新tag
				if (plain == "更新tag" && m.Sender.QQ.ToInt64() == master)
				{
					string tag;
					m.QuoteReply(MessageChain().Plain("更新中..."));
					if (proxy)
						tag = MessageReload(true, proxy_http);
					else
						tag = MessageReload(false, proxy_http);
					if (tag == "ok")m.QuoteReply(MessageChain().Plain("更新完成"));
					else
					{
						tag = "更新出错，出错的tag有:\n" + tag;
						m.Reply(MessageChain().Plain(tag));
					}
					return;
				}
				//缓存清理
				if (plain == "清理缓存" && m.Sender.QQ.ToInt64() == master)
				{
					if (ClearTemp()) m.QuoteReply(MessageChain().Plain("清理成功"));
					else
						m.QuoteReply(MessageChain().Plain("清理失败"));
					return;
				}
				//R18开启
				if (plain == d["R18开启"].GetString() && admin)
				{
					if (MessageR18(m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64(), true))
						m.QuoteReply(MessageChain().Plain("开启成功"));
					else
						m.QuoteReply(MessageChain().Plain("请勿重复开启哦"));
				}
				//R18关闭
				if (plain == d["R18关闭"].GetString() && admin)
				{
					if (MessageR18(m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64(), false))
						m.QuoteReply(MessageChain().Plain("关闭成功"));
					else
						m.QuoteReply(MessageChain().Plain("请勿重复关闭哦"));
				}
				//菜单指令
				if (plain == "菜单" || plain == "help")
				{
					ifstream in("./config/command.txt");
					string i, menu;
					while (getline(in, i))
					{
						menu = "\n" + i + menu;
					}
					m.Reply(MessageChain().At(m.Sender.QQ).Plain(menu.c_str()));

				}
				//yid
				if (strstr(plain.c_str(), d["yid"].GetString()))
				{
					regex id_regex("([0-9]{1,12})");
					cmatch id_res;
					if (regex_search(plain.c_str(),id_res , id_regex))
					{
						Document yid_info;
						m.Reply(MessageChain().Plain(d["发送提示语"].GetString()));
						yid_info = yid(id_res.str(1), proxy, proxy_http, m.Sender.Group.GID.ToInt64());
						if (Pointer("/code").Get(yid_info)->GetInt() == 0)
						{
							m.QuoteReply(MessageChain().Plain(Pointer("/info").Get(yid_info)->GetString()));
							return;
						}
						if (Pointer("/dimg").Get(yid_info)->GetInt() == 1)
						{
							GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(yid_info)->GetString());
							m.Reply(MessageChain().Image(img));
							if (!d["是否缓存图片"].GetBool())
							{
								_sleep(3 * 1000);
								remove(Pointer("/name").Get(yid_info)->GetString());
							}
							return;
						}
						else
						{
							m.QuoteReply(MessageChain().Plain("网络错误"));
							return;
						}
					}
				}
				//搜图
				if (plain == d["搜图"].GetString() && !search_map[m.Sender.QQ.ToInt64()])
				{
					search_map[m.Sender.QQ.ToInt64()] = true;
					m.QuoteReply(MessageChain().Plain("搜图模式开启成功,请发送图片吧\n退出搜图请发送:").Plain(d["退出搜图"].GetString()));
					return;
				}
				if (plain == d["退出搜图"].GetString() && search_map[m.Sender.QQ.ToInt64()])
				{
					search_map[m.Sender.QQ.ToInt64()] = false;
					m.QuoteReply(MessageChain().Plain("嗷呜~"));
					return;
				}
				//引用回复消息
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
							if (!snao_search(proxy, proxy_http, Pointer("/messageChain/0/url").Get(ms)->GetString(), m.GetMiraiBot(), MessageType::GroupMessage, m.Sender.Group.GID.ToInt64(), id))
							{
								m.QuoteReply(MessageChain().Plain("发生错误，详见控制台"));
								return;
							}
							return;
						}
					}
				}
				//自定义发图
				if (MessageCheck(plain))
				{
					if (MessageLimit(plain, m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64(), admin))
					{
						m.QuoteReply(MessageChain().Plain(d["发送提示语"].GetString()));
						Document yand;
						yand = yande(plain, proxy, proxy_http, m.Sender.Group.GID.ToInt64(), true, d["发送原图"].GetBool());
						int max_send = Pointer("/count").Get(yand)->GetInt(), MsId[256];
						for (int i = 1; i <= max_send; i++)
						{
							if (Pointer("/code").Get(yand)->GetInt() == 0) m.Reply(MessageChain().Plain(Pointer("/info").Get(yand)->GetString()));
							else
							{
								//发送图片并处理发送完成事宜
								//处理优先级：撤回<清除缓存
								string id = Pointer("/id").Get(yand)->GetString();
								if (Pointer("/dimg").Get(yand)->GetInt() == 1)
								{
									GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(yand)->GetString());
									MsId[i] = bot.SendMessage(m.Sender.Group.GID, MessageChain().Image(img));
									if (d["发送图片ID"].GetBool())
									{
										id = "Y站图片ID：" + id;
										bot.SendMessage(m.Sender.Group.GID, MessageChain().Plain(id), MsId[i]);
									}
									if (!d["是否缓存图片"].GetBool())
									{
										_sleep(1 * 1000);
										remove(Pointer("/name").Get(yand)->GetString());
									}
								}
								else
								{
									m.QuoteReply(MessageChain().Plain("网络错误"));
									return;
								}
								if (i != max_send)
								{
									yand = yande(plain, proxy, proxy_http, m.Sender.Group.GID.ToInt64(), true, d["发送原图"].GetBool());
								}
							}
						}
						if (d["是否撤回"].GetBool())
						{
							_sleep(d["撤回延时"].GetInt() * 1000);
							for (int i = 1; i <= max_send; i++)
							{
								bot.Recall(MsId[i]);
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
				bot.SendMessage(QQ_t(master), MessageChain().Plain(ex.what()));
			}
		});

	bot.On<FriendMessage>(
		[&](FriendMessage m)
		{
			try
			{
				auto plain = m.MessageChain;
				//tag更新
				if (plain.GetPlainText() == "更新tag" && m.Sender.QQ.ToInt64() == master)
				{
					string tag;
					m.QuoteReply(MessageChain().Plain("更新中..."));
					if (proxy)
						tag = MessageReload(true, proxy_http);
					else
						tag = MessageReload(false, proxy_http);
					if (tag == "ok")m.QuoteReply(MessageChain().Plain("更新完成"));
					else
					{
						tag = "更新出错，出错的tag有:\n" + tag;
						m.Reply(MessageChain().Plain(tag));
					}
					return;
				}
				//搜图
				Document img;
				img.Parse(plain.ToString().c_str());
				if (strstr(Pointer("/0/type").Get(img)->GetString(), "Image"))
				{
					if (!snao_search(proxy, proxy_http, Pointer("/0/url").Get(img)->GetString(), m.GetMiraiBot(), MessageType::FriendMessage, m.Sender.QQ.ToInt64(), m.MessageId()))
					{
						m.QuoteReply(MessageChain().Plain("发生错误，详见控制台"));
						return;
					}
					return;
				}
			}
			catch (const std::exception& err)
			{
				cout << err.what() << endl;
				bot.SendMessage(QQ_t(master), MessageChain().Plain(err.what()));
			}
		});

	bot.EventLoop();

	return 0;
}