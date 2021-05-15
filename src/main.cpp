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
#include "../include/Pixiv.h"
#include <rapidjson\stringbuffer.h>
#include <rapidjson\writer.h>

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
	system("cls");
#endif

	//读取config
	Document d;
	string rjson = ReloadFile("./config.json");
	if (rjson.empty())
	{
		return 0;
	}
	d.Parse(rjson.c_str());
	
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

	auto a = async(TimeLoop, d["更新时间"].GetInt(), ref(proxy), ref(proxy_http), ref(bot), ref(master));
	CommandReload();
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
				int64_t gid_64, qq_64;//大概会优化一下？
				gid_64 = m.Sender.Group.GID.ToInt64();
				qq_64 = m.Sender.QQ.ToInt64();
				//订阅消息正则
				regex bili_live_regex("^(增加|删除|关闭)直播订阅(?:Uid|UID|uid):?(\\d+)$");
				//regex bili_;
				cmatch bili_live_res;
				//发送者权限判断
				if (adminer == "ADMINISTRATOR" || adminer == "OWNER" || qq_64 == master)
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
					if (m.AtMe() || search_map[qq_64])
					{
						if (!snao_search(proxy, proxy_http, imgs[0].ToMiraiImage().Url, m.GetMiraiBot(), MessageType::GroupMessage, gid_64, m.MessageId(), d["匹配度"].GetInt()))
						{
							m.QuoteReply(MessageChain().Plain("发生错误，详见控制台"));
						}
					}
					return;
				}
				//随机cos
				if (plain == d["单图cos"].GetString() && MessageLimit(d["单图cos"].GetString(),qq_64, gid_64, admin))
				{
					GroupImage cos;
					cos.Url = "https://api.jrsgslb.cn/cos/url.php?return=img";
					m.Reply(MessageChain().Image(cos));
					return;
				}
				//热门cos
				if (plain == d["组图cos"].GetString())
				{
					m.QuoteReply(MessageChain().Plain(d["发送提示语"].GetString()));
					if (!Bilibili_cos(m.GetMiraiBot(), m.Sender.Group.GID))
					{
						m.QuoteReply(MessageChain().Plain("发送错误，详见控制台"));
						return;
					}
					m.QuoteReply(MessageChain().Plain("发电完成"));
					return;
				}
				//更新tag
				if (plain == "更新tag" && qq_64 == master)
				{
					string tag;
					m.QuoteReply(MessageChain().Plain("更新中..."));
					tag = MessageReload(proxy, proxy_http);
					if (tag == "ok")m.QuoteReply(MessageChain().Plain("更新完成"));
					else
					{
						tag = "更新出错，出错的tag有:\n" + tag;
						m.Reply(MessageChain().Plain(tag));
					}
					CommandReload();
					return;
				}
				//缓存清理
				if (plain == "清理缓存" && qq_64 == master)
				{
					if (ClearTemp()) m.QuoteReply(MessageChain().Plain("清理成功"));
					else
						m.QuoteReply(MessageChain().Plain("清理失败"));
					return;
				}
				/*
				//戳全群（做着玩）
				if (plain == "戳亿下" && qq_64 == master)
				{
					auto gMembers = bot.GetGroupMembers(m.Sender.Group.GID);
					if (gMembers.size() > 50)
					{
						m.QuoteReply(MessageChain().Plain("人太多了，会冻结的"));
						return;
					}
					for (const auto& gMem : gMembers)
					{
						bot.SendNudge(gMem.QQ, m.Sender.Group.GID);
					}
					m.QuoteReply(MessageChain().Plain("戳完咯，7777"));
					return;
				}
				*/
				//R18开启
				if (plain == d["R18开启"].GetString() && admin)
				{
					if (MessageR18(qq_64, gid_64, true))
						m.QuoteReply(MessageChain().Plain("开启成功"));
					else
						m.QuoteReply(MessageChain().Plain("请勿重复开启哦"));
					return;
				}
				//R18关闭
				if (plain == d["R18关闭"].GetString() && admin)
				{
					if (MessageR18(qq_64, gid_64, false))
						m.QuoteReply(MessageChain().Plain("关闭成功"));
					else
						m.QuoteReply(MessageChain().Plain("请勿重复关闭哦"));
					return;
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
					return;
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
						yid_info = yid(id_res.str(1), proxy, proxy_http, gid_64);
						if (Pointer("/code").Get(yid_info)->GetInt() == 0)
						{
							m.QuoteReply(MessageChain().Plain(Pointer("/info").Get(yid_info)->GetString()));
							return;
						}
						if (Pointer("/dimg").Get(yid_info)->GetInt() == 1)
						{
							GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(yid_info)->GetString());
							int msid = m.Reply(MessageChain().Image(img));
							if (!d["是否缓存图片"].GetBool())
							{
								_sleep(3 * 1000);
								remove(Pointer("/name").Get(yid_info)->GetString());
							}
							if (d["是否撤回"].GetBool())
							{
								_sleep(d["撤回延时"].GetInt() * 1000);
									bot.Recall(msid);
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
				//pid
				if (strstr(plain.c_str(), d["pid"].GetString()))
				{
					regex id_regex("([0-9]{1,12})");
					cmatch id_res;
					if (regex_search(plain.c_str(), id_res, id_regex))
					{
						m.Reply(MessageChain().Plain(d["发送提示语"].GetString()));
						if (!Pixiv_id(proxy, proxy_http, id_res.str(1),m.GetMiraiBot(), gid_64, m.MessageId()))
						{
							m.QuoteReply(MessageChain().Plain("发生错误，详见控制台"));
							return;
						}
					}
				}
				//订阅相关
				if (regex_search(plain.c_str(), bili_live_res, bili_live_regex) && admin)
				{
					string bili_live_json_txt = ReloadFile("./config/bili/live.json"), temp, newUid, newId;
					Document bili_live_json;
					if (bili_live_json_txt.empty())
					{
						if (bili_live_res.str(1) == "增加")
						{
							Pointer("/0/uid").Set(bili_live_json, bili_live_res.str(2).c_str());
							temp = "/0/send/0/id";
							Pointer(temp.c_str()).Set(bili_live_json, gid_64);
							temp = "/0/send/0/type";
							Pointer(temp.c_str()).Set(bili_live_json, 0);
							temp = "/0/send/0/status";
							Pointer(temp.c_str()).Set(bili_live_json, 1);

							//写入json文件
							StringBuffer buffer;
							Writer<StringBuffer> writer(buffer);
							bili_live_json.Accept(writer);

							const char* output = buffer.GetString();
							WriteFile("./config/bili/live.json", output);
							//刷新uid
							Reload_live_uid();
							//消息回复
							m.QuoteReply(MessageChain().Plain(bili_live_res.str(1)).Plain("成功"));
							return;
						}
						else
						{
							m.QuoteReply(MessageChain().Plain("请先增加哦！"));
							return;
						}
					}
					//备份json文件
					WriteFile("./config/bili/live.backup", bili_live_json_txt);
					//解析json
					bili_live_json.Parse(bili_live_json_txt.c_str());
					//遍历json，是否为新增uid
					bool isNew_uid = true, isNew_group = true;
					for (int i = 0; i < Pointer("").Get(bili_live_json)->Size(); i++)
					{
						temp = "/" + to_string(i) + "/uid";
						if (Pointer(temp.c_str()).Get(bili_live_json)->GetString() == bili_live_res.str(2))
						{
							isNew_uid = false;
							newUid = to_string(i);
							break;
						}
					}
					//如果不是新uid，查询群号是否为新群
					if (!isNew_uid)
					{
						string id_search;
						id_search = "/" + newUid + "/send";
						for (int i = 0; i < Pointer(id_search.c_str()).Get(bili_live_json)->Size(); i++)
						{
							temp = "/" + newUid + "/send/" + to_string(i) + "/id";
							if (Pointer(temp.c_str()).Get(bili_live_json)->GetInt64() == gid_64)
							{
								isNew_group = false;
								newId = to_string(i);
								break;
							}
						}
					}
					//增加订阅
					if (bili_live_res.str(1) == "增加")
					{
						//新增uid
						if (isNew_uid)
						{
							int num;

							Pointer("/-/uid").Set(bili_live_json, bili_live_res.str(2).c_str());
							num = Pointer("").Get(bili_live_json)->Size() - 1;
							temp = "/" + to_string(num) + "/send/0/id";
							Pointer(temp.c_str()).Set(bili_live_json, gid_64);
							temp = "/" + to_string(num) + "/send/0/type";
							Pointer(temp.c_str()).Set(bili_live_json, 0);
							temp = "/" + to_string(num) + "/send/0/status";
							Pointer(temp.c_str()).Set(bili_live_json, 1);
						}
						else    //增加群组
						{
							if (isNew_group)
							{
								int num;
								temp = "/" + newUid + "/send/-/id";
								Pointer(temp.c_str()).Set(bili_live_json, gid_64);
								temp = "/" + newUid + "/send";
								num = Pointer(temp.c_str()).Get(bili_live_json)->Size() - 1;
								temp = "/" + newUid + "/send/" + to_string(num) + "/type";
								Pointer(temp.c_str()).Set(bili_live_json, 0);
								temp = "/" + newUid + "/send/" + to_string(num) + "/status";
								Pointer(temp.c_str()).Set(bili_live_json, 1);
							}
							else
							{
								temp = "/" + newUid + "/send/" + newId + "/status";
								if (Pointer(temp.c_str()).Get(bili_live_json)->GetInt() != 1)
								{
									Pointer(temp.c_str()).Set(bili_live_json, 1);
								}
								else
								{
									m.QuoteReply(MessageChain().Plain("该订阅已存在"));
									return;
								}
							}
						}
					}
					//删除订阅
					if (bili_live_res.str(1) == "删除")
					{
						if (qq_64 != master)
						{
							m.QuoteReply(MessageChain().Plain("您无权进行操作"));
							return;
						}
						if (isNew_uid)
						{
							m.QuoteReply(MessageChain().Plain("该订阅不存在"));
							return;
						}
						temp = "/" + newUid;
						Pointer(temp.c_str()).Erase(bili_live_json);
					}
					//关闭订阅
					if (bili_live_res.str(1) == "关闭")
					{
						if (isNew_uid)
						{
							m.QuoteReply(MessageChain().Plain("该订阅不存在"));
							return;
						}
						else if (isNew_group)
						{
							m.QuoteReply(MessageChain().Plain("本群并未开启此订阅"));
							return;
						}
						else
						{
							//对于一个uid无群开启的处理
							//简单的判断size
							temp = "/" + newUid + "/send";
							int temp_size = Pointer(temp.c_str()).Get(bili_live_json)->Size();
							if (temp_size == 1)
							{
								temp = "/" + newUid;
								Pointer(temp.c_str()).Erase(bili_live_json);
							}
							else
							{
								temp = "/" + newUid + "/send/" + newId + "/status";
								if (Pointer(temp.c_str()).Get(bili_live_json)->GetInt() != 0)
								{
									Pointer(temp.c_str()).Set(bili_live_json, 0);
								}
								else
								{
									m.QuoteReply(MessageChain().Plain("本群并未开启此订阅"));
									return;
								}
								//遍历所有群的状态，查看是否都为关闭
								temp = "/" + newUid + "/send";
								bool other = false;
								for (int i = 0; i < temp_size; i++)
								{
									temp = "/" + newUid + "/send/" + to_string(i) + "/status";
									if (Pointer(temp.c_str()).Get(bili_live_json)->GetInt() == 1)
									{
										other = true;
										break;
									}
								}
								if (!other)
								{
									temp = "/" + newUid;
									Pointer(temp.c_str()).Erase(bili_live_json);
								}
							}
						}
					}
					//写入json文件
					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					bili_live_json.Accept(writer);

					const char* output = buffer.GetString();
					WriteFile("./config/bili/live.json", output);
					//刷新uid
					Reload_live_uid();
					//消息回复
					m.QuoteReply(MessageChain().Plain(bili_live_res.str(1)).Plain("成功"));
					return;
				}
				//引用回复消息
				if (!qms.empty())
				{
					auto id = qms[0].MessageId();
					//设置精华消息
					if (plain == d["精华消息"].GetString())
					{
						bot.SetEssence(id);
						return;
					}
					//搜图
					if (strstr(plain.c_str(), d["搜图"].GetString()) != NULL || m.AtMe())
					{
						Document ms;
						ms.Parse(bot.GetGroupMessageFromId(id).ToString().c_str());
						string type = Pointer("/messageChain/0/type").Get(ms)->GetString();
						if (type == "Image")
						{
							if (!snao_search(proxy, proxy_http, Pointer("/messageChain/0/url").Get(ms)->GetString(), m.GetMiraiBot(), MessageType::GroupMessage, gid_64, id, d["匹配度"].GetInt()))
							{
								m.QuoteReply(MessageChain().Plain("发生错误，详见控制台"));
								return;
							}
							return;
						}
					}
				}
				//搜图
				if (plain == d["搜图"].GetString() && !search_map[qq_64])
				{
					search_map[qq_64] = true;
					m.QuoteReply(MessageChain().Plain("搜图模式开启成功,请发送图片吧\n退出搜图请发送:").Plain(d["退出搜图"].GetString()));
					/*_sleep(180 * 1000);
					//无奈的超时解决方案
					if (search_map[qq_64])
					{
						search_map[qq_64] = false;
						m.QuoteReply(MessageChain().Plain("超时已退出搜图模式"));
					}*/
					return;
				}
				//退出搜图
				if (plain == d["退出搜图"].GetString() && search_map[qq_64])
				{
					search_map[qq_64] = false;
					m.QuoteReply(MessageChain().Plain("嗷呜~"));
					return;
				}
				//y站热门榜
				if (plain == d["y站热榜"].GetString() && MessageLimit(d["y站热榜"].GetString(), qq_64, gid_64, admin))
				{
					m.QuoteReply(MessageChain().Plain(d["发送提示语"].GetString()));
					Document Hot_img_json;
					Hot_img_json = Hot_Img(proxy, proxy_http, gid_64, d["发送原图"].GetBool());
					if (Pointer("/code").Get(Hot_img_json)->GetInt() == 0)
					{
						m.Reply(MessageChain().Plain(Pointer("/info").Get(Hot_img_json)->GetString()));
						return;
					}
					int msid;
					GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(Hot_img_json)->GetString());
					msid = m.Reply(MessageChain().Image(img));
					if (d["发送图片ID"].GetBool())
					{
						bot.SendMessage(m.Sender.Group.GID, MessageChain().Plain("Y站图片ID：").Plain(Pointer("/id").Get(Hot_img_json)->GetString()), msid);
					}
					if (!d["是否缓存图片"].GetBool())
					{
						_sleep(1 * 1000);
						remove(Pointer("/name").Get(Hot_img_json)->GetString());
					}
					if (d["是否撤回"].GetBool())
					{
						_sleep(d["撤回延时"].GetInt() * 1000);
						bot.Recall(msid);
					}
					return;
				}
				//自定义发图
				if (MessageCheck(plain))
				{
					if (MessageLimit(plain, qq_64, gid_64, admin))
					{
						m.QuoteReply(MessageChain().Plain(d["发送提示语"].GetString()));
						Document yand;
						yand = yande(plain, proxy, proxy_http, gid_64, true, d["发送原图"].GetBool());
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
									yand = yande(plain, proxy, proxy_http, gid_64, true, d["发送原图"].GetBool());
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
						m.QuoteReply(MessageChain().Plain(d["频率限制回复语"].GetString()));
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
				int64_t qq_64;
				qq_64 = m.Sender.QQ.ToInt64();
				auto plain = m.MessageChain;
				//tag更新
				if (plain.GetPlainText() == "更新tag" && qq_64 == master)
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
					CommandReload();
					return;
				}
				//搜图
				Document img;
				img.Parse(plain.ToString().c_str());
				if (strstr(Pointer("/0/type").Get(img)->GetString(), "Image"))
				{
					if (!snao_search(proxy, proxy_http, Pointer("/0/url").Get(img)->GetString(), m.GetMiraiBot(), MessageType::FriendMessage, qq_64, m.MessageId(), d["匹配度"].GetInt()))
					{
						m.Reply(MessageChain().Plain("发生错误，详见控制台"));
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