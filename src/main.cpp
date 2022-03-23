// 注意: 本项目的所有源文件都必须是 UTF-8 编码

#include <iostream>
#include <regex>
#include <map>
#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <future>

#include <mirai.h>

#include "../include/Timing.h"
#include "../include/message.h"
#include "../include/imgsearch.h"
#include "../include/yande.h"
#include "../include/Bilibili.h"
#include "../include/Pixiv.h"


//创建config全局变量
/*      变量对应关系
proxy --- /代理/开关
debug --- Debug
qute_search --- /搜图/配置/引用回复搜索
save_img ---  /自定义发图/配置/缓存图片
original_img ---  /自定义发图/配置/发送原图
recall ---  /自定义发图/配置/撤回
send_imgid ---  /自定义发图/配置/发送图片ID
send_msg ---  /自定义发图/配置/发送提示语
qute_send ---  /自定义发图/配置/提示语引用回复
MemberCtrl --- /自定义发图/配置/分群控制
*/
bool proxy, debug, qute_search, save_img, original_img, recall, send_imgid, send_msg, qute_send, MemberCtrl;
/*
master --- 主人
*/
int64_t master;
/*
time_wait --- /搜图/配置/等待时间
match --- /搜图/配置/匹配度
time_recall ---  /自定义发图/配置/撤回延时
time_updata --- 更新时间
*/
int time_wait, match, time_recall, time_updata;
/*
proxy_add --- /代理/地址
command_search_in --- /搜图/指令/开启
command_search_out --- /搜图/指令/关闭
search_api_key --- /搜图/配置/api_key
command_cos_hot --- /cos/组图
command_cos_one --- /cos/单图
command_yande_hot --- /自定义发图/指令/日排行榜
*/
string proxy_add, command_search_in, command_search_out, search_api_key, command_cos_hot, command_cos_one, command_yande_hot;
/*
reply_limit --- 频率限制回复语
command_r18_on --- /自定义发图/指令/开启R18
command_r18_off --- /自定义发图/指令/关闭R18
command_yande_id --- /自定义发图/指令/id图片
command_pixiv_id --- /pixiv/指令/id图片
command_set_essence --- 精华消息
reply_yande_msg ---  /自定义发图/配置/提示语
*/
string reply_limit, command_r18_on, command_r18_off, command_yande_id, command_pixiv_id, command_set_essence, reply_yande_msg;
/*
command_reload --- /刷新配置文件
proxy_rule --- /代理/协议
*/
string command_reload, proxy_rule;
//读取配置文件
void ReloadConfig()
{
	//读取config
	Document s;
	string json = ReloadFile("./config.json");
	if (json.empty())
	{
		throw std::runtime_error("读取 config.json 文件错误\n");
	}
	s.Parse(json.c_str());
	//代理
	proxy = Pointer("/代理/开关").Get(s)->GetBool();
	proxy_add = Pointer("/代理/地址").Get(s)->GetString();
	proxy_rule = Pointer("/代理/协议").Get(s)->GetString();
	//未归类
	master = Pointer("/主人").Get(s)->GetInt64();
	command_set_essence = Pointer("/精华消息").Get(s)->GetString();
	debug = Pointer("/Debug").Get(s)->GetBool();
	time_updata = Pointer("/更新时间").Get(s)->GetInt();
	reply_limit = Pointer("/频率限制回复语").Get(s)->GetString();
	command_reload = Pointer("/刷新配置文件").Get(s)->GetString();
	//pixiv
	command_pixiv_id = Pointer("/pixiv/指令/id图片").Get(s)->GetString();
	//自定义发图
	command_r18_on = Pointer("/自定义发图/指令/开启R18").Get(s)->GetString();
	command_r18_off = Pointer("/自定义发图/指令/关闭R18").Get(s)->GetString();
	command_yande_id = Pointer("/自定义发图/指令/id图片").Get(s)->GetString();
	command_yande_hot = Pointer("/自定义发图/指令/日排行榜").Get(s)->GetString();
	reply_yande_msg = Pointer("/自定义发图/配置/提示语").Get(s)->GetString();
	recall = Pointer("/自定义发图/配置/撤回").Get(s)->GetBool();
	save_img = Pointer("/自定义发图/配置/缓存图片").Get(s)->GetBool();
	send_imgid = Pointer("/自定义发图/配置/发送图片ID").Get(s)->GetBool();
	original_img = Pointer("/自定义发图/配置/发送原图").Get(s)->GetBool();
	MemberCtrl = Pointer("/自定义发图/配置/分群控制").Get(s)->GetBool();
	send_msg = Pointer("/自定义发图/配置/发送提示语").Get(s)->GetBool();
	qute_send = Pointer("/自定义发图/配置/提示语引用回复").Get(s)->GetBool();
	time_recall = Pointer("/自定义发图/配置/撤回延时").Get(s)->GetInt();
	//搜图
	command_search_in = Pointer("/搜图/指令/开启").Get(s)->GetString();
	command_search_out = Pointer("/搜图/指令/关闭").Get(s)->GetString();
	search_api_key = Pointer("/搜图/配置/api_key").Get(s)->GetString();
	qute_search = Pointer("/搜图/配置/引用回复搜索").Get(s)->GetBool();
	time_wait = Pointer("/搜图/配置/等待时间").Get(s)->GetInt();
	match = Pointer("/搜图/配置/匹配度").Get(s)->GetInt();
	//cos
	command_cos_one = Pointer("/cos/单图").Get(s)->GetString();
	command_cos_hot = Pointer("/cos/组图").Get(s)->GetString();

	return;
}

int main()
{
	using namespace std;
	using namespace Cyan;

#if defined(WIN32) || defined(_WIN32)
	// 切换代码页，让 CMD 可以显示 UTF-8 字符
	system("chcp 65001");
	//SetConsoleTitle("Mirai-yande.re");
	system("cls");
#endif
	printf("初次使用请在 config.json 和 mah.json 文件内写入配置\n配置文件示例见 https://github.com/Jrsgslb/Mirai-yande.re/blob/main/README.md \n");
	
	//自动登录
	MiraiBot bot;
	SessionOptions opts;
	try
	{
		StartCheck();
		ReloadConfig();
		opts = SessionOptions::FromJsonFile("./mah.json");
		bot.Connect(opts);
	}
	catch (const std::exception& err)
	{
		printf("%s \n", err.what());
		cin.get(); 
		return 0;
	}
	//检测版本
	CheckVersion(bot.GetMiraiApiHttpVersion(), bot.GetMiraiCppVersion(), "1.2.0");
	//抛出循环线程
	auto a = async(TimeLoop, time_updata, time_wait, ref(proxy), ref(proxy_rule), ref(proxy_add), ref(bot), ref(master));
	//上线提醒
	bot.SendMessage(QQ_t(master), MessageChain().Plain("Bot已上线"));
	//订阅消息正则
	regex bili_live_regex("^(增加|开启|删除|关闭)(动态|直播)订阅(?:Uid|UID|uid):?(\\d+)$");
	cmatch bili_live_res;

	bot.On<GroupMessage>(
		[&](GroupMessage m)
		{
			try
			{
				auto &mc = m.MessageChain;
				auto qms = mc.GetAll<QuoteMessage>();
				string plain = mc.GetPlainText(), admin_str = GroupPermissionStr(m.Sender.Permission);
				vector<ImageMessage> imgs = mc.GetAll<ImageMessage>();
				bool admin;
				int64_t gid_64, qq_64;//大概会优化一下?
				gid_64 = m.Sender.Group.GID.ToInt64();
				qq_64 = m.Sender.QQ.ToInt64();
				//发送者权限判断
				if (admin_str == "ADMINISTRATOR" || admin_str == "OWNER" || qq_64 == master)
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
					if (m.AtMe() || Search_check(m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64()))
					{
						Search_switch(true, m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64());
						if (!snao_search(proxy, proxy_rule, proxy_add, imgs[0].ToMiraiImage().Url, m.GetMiraiBot(), MessageType::GroupMessage, gid_64, m.MessageId(), match, search_api_key))
						{
							m.QuoteReply(MessageChain().Plain("发生错误，详见控制台"));
						}
					}
					return;
				}
				//配置文件热重载
				if (plain == command_reload && qq_64 == master)
				{
					ReloadConfig();
					m.QuoteReply(MessageChain().Plain("配置文件重载成功"));
					return;
				}
				//更新tag
				if (plain == "更新tag" && qq_64 == master)
				{
					string tag;
					m.QuoteReply(MessageChain().Plain("更新中..."));
					tag = MessageReload(proxy, proxy_rule, proxy_add);
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
				//戳全群（做着玩）
				/*
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
				if (admin && plain == command_r18_on)
				{
					if (MessageR18(qq_64, gid_64, true))
						m.QuoteReply(MessageChain().Plain("开启成功"));
					else
						m.QuoteReply(MessageChain().Plain("请勿重复开启哦"));
					return;
				}
				//R18关闭
				if (admin && plain == command_r18_off)
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
					//待重写
					//m.Reply(MessageChain().At(m.Sender.QQ).Plain(ReloadFile("./config/command.txt")));
					return;
				}
				//订阅相关
				if (admin && regex_search(plain.c_str(), bili_live_res, bili_live_regex))
				{
					Bilibili_match(m.GetMiraiBot(), bili_live_res.str(2), bili_live_res.str(1), bili_live_res.str(3), gid_64, qq_64, master, m.MessageId());
					return;
				}
				//引用回复消息
				if (!qms.empty())
				{
					auto id = qms[0].MessageId();
					//设置精华消息
					if (plain == command_set_essence)
					{
						bot.SetEssence(id);
						return;
					}
					//搜图
					if (qute_search && strstr(plain.c_str(), command_search_in.c_str()) != NULL || m.AtMe())
					{
						Document ms;
						ms.Parse(bot.GetGroupMessageFromId(id).ToString().c_str());
						string type = Pointer("/messageChain/0/type").Get(ms)->GetString();
						if (type == "Image")
						{
							if (!snao_search(proxy, proxy_rule, proxy_add, Pointer("/messageChain/0/url").Get(ms)->GetString(), m.GetMiraiBot(), MessageType::GroupMessage, gid_64, id, match, search_api_key))
							{
								m.QuoteReply(MessageChain().Plain("发生错误，详见控制台"));
								return;
							}
							return;
						}
					}
				}
				//搜图
				if (plain == command_search_in)
				{
					if (Search_switch(true, m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64()))
					{
						m.QuoteReply(MessageChain().Plain("搜图模式开启成功,请发送图片吧\n退出搜图请发送:").Plain(command_search_out));
					}
					else
					{
						m.QuoteReply(MessageChain().Plain("您仍处于搜图模式下\n退出搜图请发送:").Plain(command_search_out));
					}
					return;
				}
				//退出搜图
				if (plain == command_search_out)
				{
					if (Search_switch(false, m.Sender.QQ.ToInt64(), m.Sender.Group.GID.ToInt64()))
					{
						m.QuoteReply(MessageChain().Plain("嗷呜~"));
					}
					return;
				}
				//频率限制
				if (MessageLimit(plain, qq_64, gid_64, admin))
				{
					//随机cos
					if (plain == command_cos_one)
					{
						GroupImage cos;
						cos.Url = "https://api.jrsgslb.cn/cos/url.php?return=img";
						m.Reply(MessageChain().Image(cos));
						return;
					}
					//热门cos
					if (plain == command_cos_hot)
					{
						if (send_msg)
						{
							if (qute_send)
							{
								m.QuoteReply(MessageChain().Plain(reply_yande_msg));
							}
							else
							{
								m.Reply(MessageChain().Plain(reply_yande_msg));
							}
						}
						if (!Bilibili_cos(m.GetMiraiBot(), m.Sender.Group.GID))
						{
							m.QuoteReply(MessageChain().Plain("发送错误，详见控制台"));
							return;
						}
						m.QuoteReply(MessageChain().Plain("发电完成"));
						return;
					}
					//yid
					if (plain.find(command_yande_id) == 0)
					{
						regex id_regex("([0-9]{1,12})");
						cmatch id_res;
						if (regex_search(plain.c_str(), id_res, id_regex))
						{
							Document yid_info;
							if (send_msg)
							{
								if (qute_send)
								{
									m.QuoteReply(MessageChain().Plain(reply_yande_msg));
								}
								else
								{
									m.Reply(MessageChain().Plain(reply_yande_msg));
								}
							}
							yid_info = yid(id_res.str(1), proxy, proxy_rule, proxy_add, gid_64);
							if (Pointer("/code").Get(yid_info)->GetInt() == 0)
							{
								m.QuoteReply(MessageChain().Plain(Pointer("/info").Get(yid_info)->GetString()));
								return;
							}
							if (Pointer("/dimg").Get(yid_info)->GetInt() == 1)
							{
								GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(yid_info)->GetString());
								int msid = m.Reply(MessageChain().Image(img));
								if (send_imgid)
								{
									bot.SendMessage(m.Sender.Group.GID, MessageChain().Plain("Y站图片ID：").Plain(id_res.str(1)), msid);
								}
								if (!save_img)
								{
									_sleep(3 * 1000);
									remove(Pointer("/name").Get(yid_info)->GetString());
								}
								if (recall)
								{
									_sleep(time_recall* 1000);
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
					if (plain.find(command_pixiv_id) == 0)
					{
						regex id_regex("([0-9]{1,12})");
						cmatch id_res;
						if (regex_search(plain.c_str(), id_res, id_regex))
						{
							if (send_msg)
							{
								if (qute_send)
								{
									m.QuoteReply(MessageChain().Plain(reply_yande_msg));
								}
								else
								{
									m.Reply(MessageChain().Plain(reply_yande_msg));
								}
							}
							if (!Pixiv_id(proxy, proxy_rule, proxy_add, id_res.str(1), m.GetMiraiBot(), gid_64, m.MessageId()))
							{
								m.QuoteReply(MessageChain().Plain("发生错误，详见控制台"));
								return;
							}
						}
					}
					//y站热门榜
					if (plain == command_yande_hot)
					{
						if (send_msg)
						{
							if (qute_send)
							{
								m.QuoteReply(MessageChain().Plain(reply_yande_msg));
							}
							else
							{
								m.Reply(MessageChain().Plain(reply_yande_msg));
							}
						}
						Document Hot_img_json;
						Hot_img_json = Hot_Img(proxy, proxy_rule, proxy_add, gid_64, original_img);
						if (Pointer("/code").Get(Hot_img_json)->GetInt() == 0)
						{
							m.Reply(MessageChain().Plain(Pointer("/info").Get(Hot_img_json)->GetString()));
							return;
						}
						int msid;
						GroupImage img = bot.UploadGroupImage(Pointer("/name").Get(Hot_img_json)->GetString());
						msid = m.Reply(MessageChain().Image(img));
						if (send_imgid)
						{
							bot.SendMessage(m.Sender.Group.GID, MessageChain().Plain("Y站图片ID：").Plain(Pointer("/id").Get(Hot_img_json)->GetString()), msid);
						}
						if (!save_img)
						{
							_sleep(1 * 1000);
							remove(Pointer("/name").Get(Hot_img_json)->GetString());
						}
						if (recall)
						{
							_sleep(time_recall * 1000);
							bot.Recall(msid);
						}
						return;
					}
					//自定义发图
					if (MessageCheck(plain))
					{
						if (send_msg)
						{
							if (qute_send)
							{
								m.QuoteReply(MessageChain().Plain(reply_yande_msg));
							}
							else
							{
								m.Reply(MessageChain().Plain(reply_yande_msg));
							}
						}
						Document yand;
						yand = yande(plain, proxy, proxy_rule, proxy_add, gid_64, true, original_img);
						int max_send = Pointer("/count").Get(yand)->GetInt(), MsId[256]{};
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
									if (send_imgid)
									{
										id = "Y站图片ID：" + id;
										bot.SendMessage(m.Sender.Group.GID, MessageChain().Plain(id), MsId[i]);
									}
									if (!save_img)
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
									yand = yande(plain, proxy, proxy_rule, proxy_add, gid_64, true, original_img);
								}
							}
						}
						if (recall)
						{
							_sleep(time_recall * 1000);
							for (int i = 1; i <= max_send; i++)
							{
								bot.Recall(MsId[i]);
							}
						}
						return;
					}
				}
				else
				{
				    m.QuoteReply(MessageChain().Plain(reply_limit));
					return;
				}
			}
			catch (const std::exception& err)
			{
				printf("%s \n", err.what());
				if (debug)
				{
					bot.SendMessage(QQ_t(master), MessageChain().Plain(err.what()));
				}
			}
		});

	bot.On<FriendMessage>(
		[&](FriendMessage m)
		{
			try
			{
				int64_t qq_64;
				auto& mc = m.MessageChain;
				qq_64 = m.Sender.QQ.ToInt64();
				string plain = mc.GetPlainText();
				//tag更新
				if (plain == "更新tag" && qq_64 == master)
				{
					string tag;
					m.QuoteReply(MessageChain().Plain("更新中..."));
					tag = MessageReload(proxy, proxy_rule, proxy_add);
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
				img.Parse(mc.ToString().c_str());
				if (strstr(Pointer("/0/type").Get(img)->GetString(), "Image"))
				{
					if (!snao_search(proxy, proxy_rule, proxy_add, Pointer("/0/url").Get(img)->GetString(), m.GetMiraiBot(), MessageType::FriendMessage, qq_64, m.MessageId(), match, search_api_key))
					{
						m.Reply(MessageChain().Plain("发生错误，详见控制台"));
						return;
					}
					return;
				}
				//动态和直播订阅
				if (regex_search(plain.c_str(), bili_live_res, bili_live_regex))
				{
					Bilibili_match(m.GetMiraiBot(), bili_live_res.str(2), bili_live_res.str(1), bili_live_res.str(3), qq_64, master);
					return;
				}
			}
			catch (const std::exception& err)
			{
				printf("%s \n", err.what());
				if (debug)
				{
					bot.SendMessage(QQ_t(master), MessageChain().Plain(err.what()));
				}
			}
		});

	bot.On<BotLeaveEventKick>
		([&](BotLeaveEventKick m)
	{
				
	});

	bot.On<LostConnection>
		([&](LostConnection e)
		{
			cout << e.ErrorMessage << " (" << e.Code << ")" << endl;
			while (true)
			{
				try
				{
					printf("尝试与 mirai-api-http 重新建立连接...\n");
					bot.Reconnect();
					break;
				}
				catch (const std::exception& ex)
				{
					printf("%s \n", ex.what());
				}
				MiraiBot::SleepSeconds(1);
			}
			printf("成功与 mirai-api-http 重新建立连接!\n");
		});
	char ch;
	while ((ch = getchar()) != 'q');
	bot.Disconnect();

	return 0;
}