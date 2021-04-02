﻿// 注意: 本项目的所有源文件都必须是 UTF-8 编码
#include <mirai.h>

#include "../include/yande.h"
#include "../include/message.h"
#include "../include/imgsearch.h"
#include <rapidjson\writer.h>
#include <future>


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

	bot.On<GroupMessage>(
		[&](GroupMessage m)
		{
			try
			{
				auto mc = m.MessageChain;
				auto qms = mc.GetAll<QuoteMessage>();
				string plain = mc.GetPlainText(), adminer = GroupPermissionStr(m.Sender.Permission);
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
				//随机cos
				if (plain == "随机cos" && MessageLimit("随机cos",m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64(), admin))
				{
					GroupImage cos;
					cos.Url = "https://api.jrsgslb.cn/cos/url.php?return=img";
					m.Reply(MessageChain().Image(cos));
					return;
				}
				//更新tag
				if (plain == "更新tag" && m.Sender.QQ.ToInt64() == master) {
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
						if (DownloadImg(Pointer("/url").Get(yid_info)->GetString(), Pointer("/name").Get(yid_info)->GetString(), proxy, proxy_http))
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
							Document a2d_json, snao_json;
							snao_json = snao_search(proxy, proxy_http, Pointer("/messageChain/0/url").Get(ms)->GetString());
							if (Pointer("/code").Get(snao_json)->GetInt() != 1)
							{
								if (d["Debug"].GetBool())
								{
									m.Reply(MessageChain().Plain(Pointer("/info").Get(snao_json)->GetString()));
									return;
								}
								else
								{
									m.Reply(MessageChain().Plain("saucenao搜索出错了，请稍后重试"));
									return;
								}
							}
							if (stold(Pointer("/match").Get(snao_json)->GetString()) <= 80)
							{
								m.QuoteReply(MessageChain().Plain("相似度过低将使用ascii2d搜索"));
								a2d_json = a2d_search(proxy, proxy_http, Pointer("/messageChain/0/url").Get(ms)->GetString());
								//先回复snao消息
								string snao_res;
								if (DownloadImg(Pointer("/url").Get(snao_json)->GetString(), Pointer("/name").Get(snao_json)->GetString(), proxy, proxy_http))
								{
									snao_res = Pointer("/title").Get(snao_json)->GetString();
									snao_res = snao_res + "\n相似度：" + Pointer("/match").Get(snao_json)->GetString() + "%\n图片地址：" + Pointer("/id").Get(snao_json)->GetString() + "\n画师：" + Pointer("/member").Get(snao_json)->GetString();
									GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(snao_json)->GetString());
									m.QuoteReply(MessageChain().Image(img).Plain(snao_res));

									if (!d["是否缓存图片"].GetBool())
									{
										_sleep(5 * 1000);
										remove(Pointer("/name").Get(snao_json)->GetString());
									}
								}
								else
								{
									m.QuoteReply(MessageChain().Plain("网络错误，搜索结果可能不包含图片"));
									snao_res = Pointer("/title").Get(snao_json)->GetString();
									snao_res = snao_res + "\n相似度：" + Pointer("/match").Get(snao_json)->GetString() + "%\n图片地址：" + Pointer("/id").Get(snao_json)->GetString() + "\n画师：" + Pointer("/member").Get(snao_json)->GetString();
									m.QuoteReply(MessageChain().Plain(snao_res));
								}
								//a2d
								if (Pointer("/code").Get(a2d_json)->GetInt() != 1)
								{
									if (d["Debug"].GetBool())
									{
										m.Reply(MessageChain().Plain(Pointer("/info").Get(a2d_json)->GetString()));
										return;
									}
									else
									{
										m.Reply(MessageChain().Plain("a2d搜索出错了，请稍后重试"));
										return;
									}
								}
								if (DownloadImg(Pointer("/color/url").Get(a2d_json)->GetString(), Pointer("/color/name").Get(a2d_json)->GetString(), proxy, proxy_http) && DownloadImg(Pointer("/bovw/url").Get(a2d_json)->GetString(), Pointer("/bovw/name").Get(a2d_json)->GetString(), proxy, proxy_http))
								{
									string repl_color, repl_bovw;
									GroupImage img_clolr = bot.UploadGroupImage(Pointer("/color/name").Get(a2d_json)->GetString());
									GroupImage img_bovw = bot.UploadGroupImage(Pointer("/bovw/name").Get(a2d_json)->GetString());
									repl_color = Pointer("/color/pic/name").Get(a2d_json)->GetString();
									repl_color = repl_color + "\n原图地址:" + Pointer("/color/pic/url").Get(a2d_json)->GetString() + "\nAuthor:" + Pointer("/color/user/url").Get(a2d_json)->GetString();
									repl_bovw = Pointer("/bovw/pic/name").Get(a2d_json)->GetString();
									repl_bovw = repl_bovw + "\n原图地址:" + Pointer("/bovw/pic/url").Get(a2d_json)->GetString() + "\nAuthor:" + Pointer("/bovw/user/url").Get(a2d_json)->GetString();
									m.QuoteReply(MessageChain().Plain("色调搜索结果:").Image(img_clolr).Plain(repl_color));
									m.QuoteReply(MessageChain().Plain("特征搜索结果:").Image(img_bovw).Plain(repl_bovw));

									if (!d["是否缓存图片"].GetBool())
									{
										_sleep(5 * 1000);
										remove(Pointer("/color/name").Get(a2d_json)->GetString());
										remove(Pointer("/bovw/name").Get(a2d_json)->GetString());
									}
								}
								else
								{
									m.QuoteReply(MessageChain().Plain("网络错误，搜索结果可能不包含图片"));
									string repl_color, repl_bovw;
									repl_color = Pointer("/color/pic/name").Get(a2d_json)->GetString();
									repl_color = repl_color + "\n原图地址:" + Pointer("/color/pic/url").Get(a2d_json)->GetString() + "\nAuthor:" + Pointer("/color/user/url").Get(a2d_json)->GetString();
									repl_bovw = Pointer("/bovw/pic/name").Get(a2d_json)->GetString();
									repl_bovw = repl_bovw + "\n原图地址:" + Pointer("/bovw/pic/url").Get(a2d_json)->GetString() + "\nAuthor:" + Pointer("/bovw/user/url").Get(a2d_json)->GetString();
									m.QuoteReply(MessageChain().Plain("色调搜索结果:").Plain(repl_color));
									m.QuoteReply(MessageChain().Plain("特征搜索结果:").Plain(repl_bovw));
								}
								return;
							}
							else
							{
								string snao_res;
								if (DownloadImg(Pointer("/url").Get(snao_json)->GetString(), Pointer("/name").Get(snao_json)->GetString(), proxy, proxy_http))
								{
									snao_res = Pointer("/title").Get(snao_json)->GetString();
									snao_res = snao_res + "\n相似度：" + Pointer("/match").Get(snao_json)->GetString() + "\n图片地址：" + Pointer("/id").Get(snao_json)->GetString() + "\n画师：" + Pointer("/member").Get(snao_json)->GetString();
									GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(snao_json)->GetString());
									m.QuoteReply(MessageChain().Image(img).Plain(snao_res));
									return;
								}
								else
								{
									m.QuoteReply(MessageChain().Plain("网络错误，搜索结果可能不包含图片"));
									snao_res = Pointer("/title").Get(snao_json)->GetString();
									snao_res = snao_res + "\n相似度：" + Pointer("/match").Get(snao_json)->GetString() + "\n图片地址：" + Pointer("/id").Get(snao_json)->GetString() + "\n画师：" + Pointer("/member").Get(snao_json)->GetString();
									m.QuoteReply(MessageChain().Plain(snao_res));
									return;
								}
							}
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
						yand = yande(plain, proxy, proxy_http, m.Sender.Group.GID.ToInt64(), true);
						int max_send = Pointer("/count").Get(yand)->GetInt(), MsId[256];
						for (int i = 1; i <= max_send; i++)
						{
							if (Pointer("/code").Get(yand)->GetInt() == 0) m.Reply(MessageChain().Plain(Pointer("/info").Get(yand)->GetString()));
							else
							{
								//发送图片并处理发送完成事宜
								//处理优先级：撤回<清除缓存
								string name, id, url;
								id = to_string(Pointer("/id").Get(yand)->GetInt());
								if (d["发送原图"].GetBool())
								{
									name = "./temp/" + id + "." + Pointer("/file/ext").Get(yand)->GetString();
									url = Pointer("/file/url").Get(yand)->GetString();
								}
								else
								{
									name = "./temp/" + id + ".jpg";
									url = Pointer("/sample/url").Get(yand)->GetString();
								}
								if (DownloadImg(url, name, proxy, proxy_http))
								{
									GroupImage img = bot.UploadGroupImage(name);
									MsId[i] = bot.SendMessage(m.Sender.Group.GID, MessageChain().Image(img));
									if (d["发送图片ID"].GetBool())
									{
										id = "Y站图片ID：" + id;
										bot.SendMessage(m.Sender.Group.GID, MessageChain().Plain(id), MsId[i]);
									}
									if (!d["是否缓存图片"].GetBool())
									{
										_sleep(1 * 1000);
										remove(name.c_str());
									}
								}
								else
								{
									m.QuoteReply(MessageChain().Plain("网络错误"));
									return;
								}
								if (i != max_send)
								{
									yand = yande(plain, proxy, proxy_http, m.Sender.Group.GID.ToInt64(), true);
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
				system("");
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
					Document a2d_json, snao_json;
					snao_json = snao_search(proxy, proxy_http, Pointer("/0/url").Get(img)->GetString());
					if (Pointer("/code").Get(snao_json)->GetInt() != 1)
					{
						if (d["Debug"].GetBool())
						{
							m.Reply(MessageChain().Plain(Pointer("/info").Get(snao_json)->GetString()));
							return;
						}
						else
						{
							m.Reply(MessageChain().Plain("saucenao搜索出错了，请稍后重试"));
							return;
						}
					}

					if (stold(Pointer("/match").Get(snao_json)->GetString()) <= 80)
					{
						m.Reply(MessageChain().Plain("相似度过低将使用ascii2d搜索"));
						a2d_json = a2d_search(proxy, proxy_http, Pointer("/0/url").Get(img)->GetString());
						//先回复snao消息
						string snao_res;
						if (DownloadImg(Pointer("/url").Get(snao_json)->GetString(), Pointer("/name").Get(snao_json)->GetString(), proxy, proxy_http))
						{
							snao_res = Pointer("/title").Get(snao_json)->GetString();
							snao_res = snao_res + "\n相似度：" + Pointer("/match").Get(snao_json)->GetString() + "%\n图片地址：" + Pointer("/id").Get(snao_json)->GetString() + "\n画师：" + Pointer("/member").Get(snao_json)->GetString();
							GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(snao_json)->GetString());
							m.Reply(MessageChain().Image(img).Plain(snao_res));

							if (!d["是否缓存图片"].GetBool())
							{
								_sleep(5 * 1000);
								remove(Pointer("/name").Get(snao_json)->GetString());
							}
						}
						else
						{
							m.Reply(MessageChain().Plain("网络错误，搜索结果可能不包含图片"));
							snao_res = Pointer("/title").Get(snao_json)->GetString();
							snao_res = snao_res + "\n相似度：" + Pointer("/match").Get(snao_json)->GetString() + "%\n图片地址：" + Pointer("/id").Get(snao_json)->GetString() + "\n画师：" + Pointer("/member").Get(snao_json)->GetString();
							m.Reply(MessageChain().Plain(snao_res));
						}
						//a2d结果回复
						if (Pointer("/code").Get(a2d_json)->GetInt() != 1)
						{
							if (d["Debug"].GetBool())
							{
								m.Reply(MessageChain().Plain(Pointer("/info").Get(a2d_json)->GetString()));
								return;
							}
							else
							{
								m.Reply(MessageChain().Plain("a2d搜索出错了，请稍后重试"));
								return;
							}
						}
						if (DownloadImg(Pointer("/color/url").Get(a2d_json)->GetString(), Pointer("/color/name").Get(a2d_json)->GetString(), proxy, proxy_http) && DownloadImg(Pointer("/bovw/url").Get(a2d_json)->GetString(), Pointer("/bovw/name").Get(a2d_json)->GetString(), proxy, proxy_http))
						{
							string repl_color, repl_bovw;
							FriendImage img_clolr = bot.UploadFriendImage(Pointer("/color/name").Get(a2d_json)->GetString());
							FriendImage img_bovw = bot.UploadFriendImage(Pointer("/bovw/name").Get(a2d_json)->GetString());
							repl_color = Pointer("/color/pic/name").Get(a2d_json)->GetString();
							repl_color = repl_color + "\n原图地址:" + Pointer("/color/pic/url").Get(a2d_json)->GetString() + "\nAuthor:" + Pointer("/color/user/url").Get(a2d_json)->GetString();
							repl_bovw = Pointer("/bovw/pic/name").Get(a2d_json)->GetString();
							repl_bovw = repl_bovw + "\n原图地址:" + Pointer("/bovw/pic/url").Get(a2d_json)->GetString() + "\nAuthor:" + Pointer("/bovw/user/url").Get(a2d_json)->GetString();
							m.Reply(MessageChain().Plain("色调搜索结果:").Image(img_clolr).Plain(repl_color));
							m.Reply(MessageChain().Plain("特征搜索结果:").Image(img_bovw).Plain(repl_bovw));

							if (!d["是否缓存图片"].GetBool())
							{
								_sleep(5 * 1000);
								remove(Pointer("/color/name").Get(a2d_json)->GetString());
								remove(Pointer("/bovw/name").Get(a2d_json)->GetString());
							}
						}
						else
						{
							m.Reply(MessageChain().Plain("网络错误，搜索结果可能不包含图片"));
							string repl_color, repl_bovw;
							repl_color = Pointer("/color/pic/name").Get(a2d_json)->GetString();
							repl_color = repl_color + "\n原图地址:" + Pointer("/color/pic/url").Get(a2d_json)->GetString() + "\nAuthor:" + Pointer("/color/user/url").Get(a2d_json)->GetString();
							repl_bovw = Pointer("/bovw/pic/name").Get(a2d_json)->GetString();
							repl_bovw = repl_bovw + "\n原图地址:" + Pointer("/bovw/pic/url").Get(a2d_json)->GetString() + "\nAuthor:" + Pointer("/bovw/user/url").Get(a2d_json)->GetString();
							m.Reply(MessageChain().Plain("色调搜索结果:").Plain(repl_color));
							m.Reply(MessageChain().Plain("特征搜索结果:").Plain(repl_bovw));
						}
					}
					else
					{
						string snao_res;
						if (DownloadImg(Pointer("/url").Get(snao_json)->GetString(), Pointer("/name").Get(snao_json)->GetString(), proxy, proxy_http))
						{
							snao_res = Pointer("/title").Get(snao_json)->GetString();
							snao_res = snao_res + "\n相似度：" + Pointer("/match").Get(snao_json)->GetString() + "\n图片地址：" + Pointer("/id").Get(snao_json)->GetString() + "\n画师：" + Pointer("/member").Get(snao_json)->GetString();
							GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(snao_json)->GetString());
							m.Reply(MessageChain().Image(img).Plain(snao_res));
							return;
						}
						else
						{
							m.Reply(MessageChain().Plain("网络错误，搜索结果可能不包含图片"));
							snao_res = Pointer("/title").Get(snao_json)->GetString();
							snao_res = snao_res + "\n相似度：" + Pointer("/match").Get(snao_json)->GetString() + "\n图片地址：" + Pointer("/id").Get(snao_json)->GetString() + "\n画师：" + Pointer("/member").Get(snao_json)->GetString();
							m.Reply(MessageChain().Plain(snao_res));
							return;
						}
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