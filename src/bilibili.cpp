#include <iostream>

#include <rapidjson/pointer.h>
#include <rapidjson/document.h>

#include "../include/Bilibili.h"
#include "../include/message.h"
#include "../include/HttpRequest.hpp"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace rapidjson;
using namespace std;

vector<string> live_uid;
vector<string> dynamic_uid;
map<string, bool> live_status;
map <string, unsigned long long> dynamic_status;

void Reload_live_uid()
{
	string bili_live_json_txt = ReloadFile("./config/bili/live.json"), temp, uid;
	Document bili_live_json;
	if (bili_live_json_txt.empty())
	{
		return;
	}
	//清空数组
	live_uid.clear();
	//解析json
	bili_live_json.Parse(bili_live_json_txt.c_str());

	for (int i = 0; i < Pointer("").Get(bili_live_json)->Size(); i++)
	{
		temp = "/" + to_string(i) + "/uid";
		uid = Pointer(temp.c_str()).Get(bili_live_json)->GetString();
		live_uid.push_back(uid);
		auto j = live_status.find(uid.c_str());
		if (j == live_status.end())
		{
			live_status[uid] = false;
			printf("已开启对Uid: %s 的直播订阅\n", uid.c_str());
		}
	}
	return;
}

void Bilibili_live(MiraiBot& bot)
{
	//为空直接返回
	if (live_uid.empty())
	{
		return;
	}
	HttpRequest r;
	string txt, url, uid, temp;
	Document bili_live;
	for (int i = 0; i < live_uid.size(); i++)
	{
		string i_str = to_string(i);
		uid = live_uid[i];
		url = "https://api.live.bilibili.com/room/v1/Room/getRoomInfoOld?mid=" + uid;
		txt = r.Http_Get_Bili(url);
		bili_live.Parse(txt.c_str());
		if (txt.empty() || Pointer("/code").Get(bili_live)->GetInt() != 0)
		{
			printf("连接发生错误，url： %s \n", url.c_str());
			continue;
		}
		int status = Pointer("/data/liveStatus").Get(bili_live)->GetInt();
		if (status == 1 && !live_status[uid])
		{
			txt = ReloadFile("./config/bili/live.json");
			Document uid_group;
			uid_group.Parse(txt.c_str());
			temp = "/" + i_str + "/send";
			int size = Pointer(temp.c_str()).Get(uid_group)->Size();
			for (int j = 0; j < size; j++)
			{
				string j_str = to_string(j);
				temp = "/" + i_str + "/send/" + j_str + "/status";
				if (Pointer(temp.c_str()).Get(uid_group)->GetInt() == 1)
				{
					temp = "/" + i_str + "/send/" + j_str + "/id";
					GroupImage img;
					img.Url = Pointer("/data/cover").Get(bili_live)->GetString();
					txt = "订阅的Up正在直播中\n";
					txt = txt + Pointer("/data/title").Get(bili_live)->GetString() + "\n直播间地址:" + Pointer("/data/url").Get(bili_live)->GetString();
					bot.SendMessage(GID_t(Pointer(temp.c_str()).Get(uid_group)->GetInt64()), MessageChain().Image(img).Plain(txt));
				}
			}
			live_status[uid] = true;
		}
		if (status == 0 && live_status[uid])
		{
			Document uid_group, uid_info;
			url = "https://api.live.bilibili.com/live_user/v1/Master/info?uid=" + uid;
			txt = r.Http_Get_Bili(url);
			uid_info.Parse(txt.c_str());
			txt = ReloadFile("./config/bili/live.json");

			uid_group.Parse(txt.c_str());
			temp = "/" + i_str + "/send";
			int size = Pointer(temp.c_str()).Get(uid_group)->Size();
			for (int j = 0; j < size; j++)
			{
				string  j_str = to_string(j);
				temp = "/" + i_str + "/send/" + j_str + "/status";
				if (Pointer(temp.c_str()).Get(uid_group)->GetInt() == 1)
				{
					GroupImage img;
					img.Url = Pointer("/data/info/face").Get(uid_info)->GetString();
					temp = "/" + i_str + "/send/" + j_str + "/id";
					txt = "订阅的Up: ";
					txt = txt + Pointer("/data/info/uname").Get(uid_info)->GetString() + "下播啦\n直播间地址:" + Pointer("/data/url").Get(bili_live)->GetString();
					bot.SendMessage(GID_t(Pointer(temp.c_str()).Get(uid_group)->GetInt64()), MessageChain().Image(img).Plain(txt));
				}
			}
			live_status[uid] = false;
		}
	}

}

bool Bilibili_cos(MiraiBot& bot, GID_t gid)
{
	try
	{
		Document d;
		HttpRequest r;
		string txt, url, count, num, file;
		int page_num;
		GroupImage img;
		MessageChain reply;

		srand((unsigned)time(NULL));
		page_num = (rand() % 499) + 0;

		url = "https://api.vc.bilibili.com/link_draw/v2/Photo/list?category=cos&type=hot&page_num=" + to_string(page_num) + "&page_size=1";
		txt = r.Http_Get(url);
		d.Parse(txt.c_str());
		if (txt.empty() || Pointer("/code").Get(d)->GetInt() == 1)
		{
			printf("连接发生错误，url： %s \n", url.c_str());
			return false;
		}
		int size, max;
		max = Pointer("/data/total_count").Get(d)->GetInt();
		if (max < page_num)
		{
			Bilibili_cos(bot, gid);
			return true;
		}
		size = Pointer("/data/items/0/item/pictures").Get(d)->Size();
		for (int i = 0; i < size; i++)
		{
			count = to_string(i);
			count = "/data/items/0/item/pictures/" + count + "/img_src";
			url = Pointer(count.c_str()).Get(d)->GetString();
			img.Url = url;
			reply.Image(img);
		}
		bot.SendMessage(gid, reply);
		return true;
	}
	catch (const std::exception& err)
	{
		printf("%s \n", err.what());
		if (err.what() != "网络错误.")
		{
			return false;
		}
	}
}

void Reload_dynamic_uid()
{
	string bili_dynamic_json_txt = ReloadFile("./config/bili/dynamic.json"), temp, uid;
	Document bili_dynamic_json;
	if (bili_dynamic_json_txt.empty())
	{
		return;
	}
	//清空数组
	dynamic_uid.clear();
	//解析json
	bili_dynamic_json.Parse(bili_dynamic_json_txt.c_str());
	//循环写入数据
	for (int i = 0; i < Pointer("").Get(bili_dynamic_json)->Size(); i++)
	{
		temp = "/" + to_string(i) + "/uid";
		uid = Pointer(temp.c_str()).Get(bili_dynamic_json)->GetString();
		auto j = dynamic_status.find(uid.c_str());
		dynamic_uid.push_back(uid);
		if (j == dynamic_status.end())
		{
			HttpRequest r;
			Document bili_dynamic_status;
			string url;
			url = "https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/space_history?host_uid=" + uid;
			bili_dynamic_status.Parse(r.Http_Get_Bili(url).c_str());
			//断网或者网络错误时判断
			if (bili_dynamic_status.HasParseError())
			{
				continue;
			}
			//新用户无动态情况
			if (!Pointer("/data").Get(bili_dynamic_status)->HasMember("cards"))
			{
				printf("UID: %s 发生错误，该ID下暂无动态消息或网络发生错误\n", uid.c_str());
				dynamic_status[uid] = 0;
				continue;
			}
			dynamic_status[uid] = Pointer("/data/cards/0/desc/dynamic_id").Get(bili_dynamic_status)->GetUint64();
			printf("已开启对Uid: %s 的动态订阅    消息ID为 %s \n", uid.c_str(), Pointer("/data/cards/0/desc/dynamic_id_str").Get(bili_dynamic_status)->GetString());
		}
	}
	return;
}
//解析b站动态函数
MessageChain Bilibili_parse_dynamic(int type, string json)
{
	MessageChain reply;

	try
	{
		Document bili_card_json;
		bili_card_json.Parse(json.c_str());
		
		switch (type)
		{
		//不支持的动态类型
		default:
		{
			reply.Plain(Pointer("/user/uname").Get(bili_card_json)->GetString()).Plain(":\n该消息暂不支持解析,请将订阅ID反馈给开发者或自己开发").Face(317);
			return reply;
		}
		//转发的动态类型，并解析原始动态
		case(1):
		{
			string reply_str;
			reply_str = Pointer("/user/uname").Get(bili_card_json)->GetString();
			reply_str = reply_str + "转发了一条动态:\n" + Pointer("/item/content").Get(bili_card_json)->GetString() + "\n";
			reply = reply.Plain(reply_str) + Bilibili_parse_dynamic(Pointer("/item/orig_type").Get(bili_card_json)->GetInt(), Pointer("/origin").Get(bili_card_json)->GetString());
			return reply;
		}
		//图片动态类型
		case(2):
		{
			string reply_str;
			reply_str = Pointer("/user/name").Get(bili_card_json)->GetString();
			reply_str = reply_str + ":\n" + Pointer("/item/description").Get(bili_card_json)->GetString();
			reply.Plain(reply_str);
			for (int i = 0; i < Pointer("/item/pictures").Get(bili_card_json)->Size(); i++)
			{
				GroupImage img;
				string temp;
				temp = "/item/pictures/" + to_string(i) + "/img_src";
				img.Url = Pointer(temp.c_str()).Get(bili_card_json)->GetString();
				reply.Image(img);
			}
			return reply;
		}
		//文字动态类型
		case(4):
		{
			string reply_str;

			reply_str = Pointer("/user/uname").Get(bili_card_json)->GetString();
			reply_str = reply_str + ":\n" + Pointer("/item/content").Get(bili_card_json)->GetString();

			reply.Plain(reply_str);
			return reply;
		}
		//视频动态类型
		case(8):
		{
			string reply_str, url;
			GroupImage img;

			url = Pointer("/pic").Get(bili_card_json)->GetString();
			url.replace(0, 7, "http:/");
			img.Url = url;
			reply_str = Pointer("/owner/name").Get(bili_card_json)->GetString();
			reply_str = reply_str + ":\n" + Pointer("/desc").Get(bili_card_json)->GetString() + "\n";
			reply.Plain(reply_str).Image(img);
			reply_str = Pointer("/short_link").Get(bili_card_json)->GetString();
			reply.Plain(reply_str);
			return reply;
		}
		//专栏动态类型
		case(64):
		{
			string reply_str, url;
			GroupImage img;
			url = Pointer("/origin_image_urls/0").Get(bili_card_json)->GetString();
			url.replace(0, 7, "http:/");
			img.Url = url;
			reply_str = Pointer("/author/name").Get(bili_card_json)->GetString();
			reply_str = reply_str + ":  专栏动态\n" + Pointer("/summary").Get(bili_card_json)->GetString() + "\n" + Pointer("/title").Get(bili_card_json)->GetString();
			reply.Plain(reply_str).Image(img);
			return reply;
		}
		//音频动态类型
		case(256):
		{
			string reply_str, url;
			GroupImage img;
			url = Pointer("/cover").Get(bili_card_json)->GetString();
			url.replace(0, 7, "http:/");
			img.Url = url;
			reply_str = Pointer("/upper").Get(bili_card_json)->GetString();
			reply_str = reply_str + ":\n" + Pointer("/title").Get(bili_card_json)->GetString() + "\n" + Pointer("/intro").Get(bili_card_json)->GetString();
			reply.Plain(reply_str).Image(img);
			return reply;
		}
		//直播动态类型 type  4200
		case(4200):
		{
			string reply_str;
			GroupImage img;
			img.Url = Pointer("/cover").Get(bili_card_json)->GetString();
			reply_str = Pointer("/uname").Get(bili_card_json)->GetString();
			reply_str = reply_str + ":\n" + Pointer("/title").Get(bili_card_json)->GetString() + "\n直播地址: " + Pointer("/slide_link").Get(bili_card_json)->GetString();
			reply.Plain(reply_str).Image(img);

			return reply;
		}
		//直播动态类型  type 4308
		case(4308):
		{
			GroupImage img;
			Document uid_info;
			HttpRequest r;
			string url, uid, reply_str;
			//获取直播间信息
			uid = to_string(Pointer("/live_play_info/uid").Get(bili_card_json)->GetInt());
			url = "https://api.live.bilibili.com/live_user/v1/Master/info?uid=" + uid;
			uid_info.Parse(r.Http_Get_Bili(url).c_str());
			img.Url = Pointer("/live_play_info/cover").Get(bili_card_json)->GetString();
			reply_str = Pointer("/data/info/uname").Get(uid_info)->GetString();
			reply_str = reply_str + ":\n" + Pointer("/live_play_info/title").Get(bili_card_json)->GetString() + "\n直播间链接: " + Pointer("/live_play_info/link").Get(bili_card_json)->GetString();
			
			return reply.Plain(reply_str).Image(img);
		}
		}
	}
	catch (const std::exception& err)
	{
		printf("%s \n", err.what());
		return reply;
	}

	return reply;
}

//消息免打扰
bool Bilibili_dynamic_Send()
{

	return true;
}

void Bilibili_dynamic(MiraiBot& bot)
{
	//为空直接返回
	if (dynamic_uid.empty())
	{
		return;
	}
	//免打扰判断
	if (Bilibili_dynamic_Send())
	{
		return;
	}
	HttpRequest r;
	Document bili_dynamic_json;
	string txt, url, uid, temp, i_str;
	unsigned long long new_id;
	try
	{
		for (int i = 0; i < dynamic_uid.size(); i++)
		{
			i_str = to_string(i);
			uid = dynamic_uid[i];
			url = "https://api.vc.bilibili.com/dynamic_svr/v1/dynamic_svr/space_history?host_uid=" + uid;
			txt = r.Http_Get_Bili(url);
			bili_dynamic_json.Parse(txt.c_str());
			//错误链接跳过
			if (txt.empty() || Pointer("/code").Get(bili_dynamic_json)->GetInt() != 0)
			{
				printf("连接发生错误，url： %s \n", url.c_str());
				continue;
			}
			//无动态跳过
			if (!Pointer("/data").Get(bili_dynamic_json)->HasMember("cards"))
			{
				continue;
			}
			new_id = Pointer("/data/cards/0/desc/dynamic_id").Get(bili_dynamic_json)->GetUint64();
			//删除动态判断
			if (dynamic_status[uid] > new_id)
			{
				dynamic_status[uid] = new_id;
				continue;
			}
			//动态id判断
			if (new_id != dynamic_status[uid])
			{
				//读取配置json
				Document d;
				d.Parse(ReloadFile("./config/bili/dynamic.json").c_str());
				int size_j = Pointer("/data/cards").Get(bili_dynamic_json)->Size();
				//循环查找动态信息，防止漏掉
				for (int j = 0; j < size_j; j++)
				{
					string num = to_string(j);
					temp = "/data/cards/" + num + "/desc/type";
					int type = Pointer(temp.c_str()).Get(bili_dynamic_json)->GetInt();
					temp = "/data/cards/" + num + "/desc/dynamic_id";
					unsigned long long dynamic_id = Pointer(temp.c_str()).Get(bili_dynamic_json)->GetUint64();
					//当动态ID小于等于时退出
					if (dynamic_id <= dynamic_status[uid])
					{
						break;
					}
					temp = "/" + i_str + "/send";
					int size = Pointer(temp.c_str()).Get(d)->Size();
					string s_str;
					long long id = 0;
					printf("UID: %s 发布了新动态 ID: %llu \n", dynamic_uid[i].c_str(), new_id);
					//发送消息
					for (int s = 0; s < size; s++)
					{
						s_str = to_string(s);
						temp = "/" + i_str + "/send/" + s_str + "/id";
						id = Pointer(temp.c_str()).Get(d)->GetUint64();
						temp = "/" + i_str + "/send/" + s_str + "/type";
						if (Pointer(temp.c_str()).Get(d)->GetInt() == 0)
						{
							temp = "/data/cards/" + num + "/card";
							bot.SendMessage(GID_t(id), Bilibili_parse_dynamic(type, Pointer(temp.c_str()).Get(bili_dynamic_json)->GetString()));
						}
						else
						{
							temp = "/data/cards/" + num + "/card";
							bot.SendMessage(QQ_t(id), Bilibili_parse_dynamic(type, Pointer(temp.c_str()).Get(bili_dynamic_json)->GetString()));
						}
					}
				}
				//刷新动态ID
				dynamic_status[uid] = new_id;
			}
		}
		return;
	}
	catch (const std::exception& err)
	{
		printf("%s \n", err.what());
		if (err.what() != "网络错误")
		{
			if (new_id > 0)
			{
				dynamic_status[uid] = new_id;
			}
			return;
		}
	}
	return;
}

void Bilibili_match(MiraiBot& bot, string type, string mode, string uid, int64_t gid_64, int64_t qq_64, int64_t master, int64_t msid)
{
	string bili_live_json_txt, temp, newUid, newId, file, file_backup;
	if (type == "直播")
	{
		file = "./config/bili/live.json";
		file_backup = "./config/bili/live.backup";
	}
	else
	{
		file = "./config/bili/dynamic.json";
		file_backup = "./config/bili/dynamic.backup";
	}
	bili_live_json_txt = ReloadFile(file.c_str());
	Document bili_live_json;
	if (bili_live_json_txt.empty())
	{
		if (mode == "增加" || mode == "开启")
		{
			Pointer("/0/uid").Set(bili_live_json, uid.c_str());
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
			WriteFile(file.c_str(), output);
			//刷新uid
			if (type == "直播")
			{
				Reload_live_uid();
			}
			else
			{
				Reload_dynamic_uid();
			}
			//消息回复
			bot.SendMessage(GID_t(gid_64), MessageChain().Plain(mode).Plain("成功"),msid);
			return;
		}
		else
		{
			bot.SendMessage(GID_t(gid_64), MessageChain().Plain("请先增加哦！"), msid);
			return;
		}
	}
	//备份json文件
	WriteFile(file_backup.c_str(), bili_live_json_txt);
	//解析json
	bili_live_json.Parse(bili_live_json_txt.c_str());
	//遍历json，是否为新增uid
	bool isNew_uid = true, isNew_id = true;
	for (int i = 0; i < Pointer("").Get(bili_live_json)->Size(); i++)
	{
		temp = "/" + to_string(i) + "/uid";
		if (Pointer(temp.c_str()).Get(bili_live_json)->GetString() == uid)
		{
			isNew_uid = false;
			newUid = to_string(i);
			//如果不是新uid，查询群号是否为新群
			string id_search;
			id_search = "/" + newUid + "/send";
			for (int j = 0; j < Pointer(id_search.c_str()).Get(bili_live_json)->Size(); j++)
			{
				temp = "/" + newUid + "/send/" + to_string(j) + "/id";
				if (Pointer(temp.c_str()).Get(bili_live_json)->GetInt64() == gid_64)
				{
					isNew_id = false;
					newId = to_string(j);
					break;
				}
			}
			break;
		}
	}
	//增加订阅
	if (mode == "增加" || mode == "开启")
	{
		//新增uid
		if (isNew_uid)
		{
			int num;

			Pointer("/-/uid").Set(bili_live_json, uid.c_str());
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
			if (isNew_id)
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
					bot.SendMessage(GID_t(gid_64), MessageChain().Plain("该订阅已存在"), msid);
					return;
				}
			}
		}
	}
	//删除订阅
	if (mode == "删除")
	{
		if (qq_64 != master)
		{
			bot.SendMessage(GID_t(gid_64), MessageChain().Plain("您无权进行操作"), msid);
			return;
		}
		if (isNew_uid)
		{
			bot.SendMessage(GID_t(gid_64), MessageChain().Plain("该订阅不存在"), msid);
			return;
		}
		temp = "/" + newUid;
		Pointer(temp.c_str()).Erase(bili_live_json);
	}
	//关闭订阅
	if (mode == "关闭")
	{
		if (isNew_uid)
		{
			bot.SendMessage(GID_t(gid_64), MessageChain().Plain("该订阅不存在"), msid);
			return;
		}
		else if (isNew_id)
		{
			bot.SendMessage(GID_t(gid_64), MessageChain().Plain("本群并未开启此订阅"), msid);
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
					bot.SendMessage(GID_t(gid_64), MessageChain().Plain("本群并未开启此订阅"), msid);
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
	WriteFile(file.c_str(), output);
	//刷新uid
	if (type == "直播")
	{
		Reload_live_uid();
	}
	else
	{
		Reload_dynamic_uid();
	}
	//消息回复
	bot.SendMessage(GID_t(gid_64), MessageChain().Plain(mode).Plain("成功"), msid);
	return;
}

void Bilibili_match(MiraiBot& bot, string type, string mode, string uid, int64_t qq_64, int64_t master)
{
	string bili_live_json_txt, temp, newUid, newId, file, file_backup;
	if (type == "直播")
	{
		file = "./config/bili/live.json";
		file_backup = "./config/bili/live.backup";
	}
	else
	{
		file = "./config/bili/dynamic.json";
		file_backup = "./config/bili/dynamic.backup";
	}
	bili_live_json_txt = ReloadFile(file.c_str());
	Document bili_live_json;
	if (bili_live_json_txt.empty())
	{
		if (mode == "增加" || mode == "开启")
		{
			Pointer("/0/uid").Set(bili_live_json, uid.c_str());
			temp = "/0/send/0/id";
			Pointer(temp.c_str()).Set(bili_live_json, qq_64);
			temp = "/0/send/0/type";
			Pointer(temp.c_str()).Set(bili_live_json, 1);
			temp = "/0/send/0/status";
			Pointer(temp.c_str()).Set(bili_live_json, 1);

			//写入json文件
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			bili_live_json.Accept(writer);

			const char* output = buffer.GetString();
			WriteFile(file.c_str(), output);
			//刷新uid
			if (type == "直播")
			{
				Reload_live_uid();
			}
			else
			{
				Reload_dynamic_uid();
			}
			//消息回复
			bot.SendMessage(QQ_t(qq_64), MessageChain().Plain(mode).Plain("成功"));
			return;
		}
		else
		{
			bot.SendMessage(QQ_t(qq_64), MessageChain().Plain("请先增加哦!"));
			return;
		}
	}
	//备份json文件
	WriteFile(file_backup.c_str(), bili_live_json_txt);
	//解析json
	bili_live_json.Parse(bili_live_json_txt.c_str());
	//遍历json，是否为新增uid
	bool isNew_uid = true, isNew_id = true;
	for (int i = 0; i < Pointer("").Get(bili_live_json)->Size(); i++)
	{
		temp = "/" + to_string(i) + "/uid";
		if (Pointer(temp.c_str()).Get(bili_live_json)->GetString() == uid)
		{
			isNew_uid = false;
			newUid = to_string(i);
			//如果不是新uid，查询群号是否为新群
			string id_search;
			id_search = "/" + newUid + "/send";
			for (int j = 0; j < Pointer(id_search.c_str()).Get(bili_live_json)->Size(); j++)
			{
				temp = "/" + newUid + "/send/" + to_string(j) + "/id";
				if (Pointer(temp.c_str()).Get(bili_live_json)->GetInt64() == qq_64)
				{
					isNew_id = false;
					newId = to_string(j);
					break;
				}
			}
			break;
		}
	}
	//增加订阅
	if (mode == "增加" || mode == "开启")
	{
		//新增uid
		if (isNew_uid)
		{
			int num;

			Pointer("/-/uid").Set(bili_live_json, uid.c_str());
			num = Pointer("").Get(bili_live_json)->Size() - 1;
			temp = "/" + to_string(num) + "/send/0/id";
			Pointer(temp.c_str()).Set(bili_live_json, qq_64);
			temp = "/" + to_string(num) + "/send/0/type";
			Pointer(temp.c_str()).Set(bili_live_json, 1);
			temp = "/" + to_string(num) + "/send/0/status";
			Pointer(temp.c_str()).Set(bili_live_json, 1);
		}
		else    //增加群组
		{
			if (isNew_id)
			{
				int num;
				temp = "/" + newUid + "/send/-/id";
				Pointer(temp.c_str()).Set(bili_live_json, qq_64);
				temp = "/" + newUid + "/send";
				num = Pointer(temp.c_str()).Get(bili_live_json)->Size() - 1;
				temp = "/" + newUid + "/send/" + to_string(num) + "/type";
				Pointer(temp.c_str()).Set(bili_live_json, 1);
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
					bot.SendMessage(QQ_t(qq_64), MessageChain().Plain("该订阅已存在"));
					return;
				}
			}
		}
	}
	//删除订阅
	if (mode == "删除")
	{
		if (qq_64 != master)
		{
			bot.SendMessage(QQ_t(qq_64), MessageChain().Plain("您无权进行操作"));
			return;
		}
		if (isNew_uid)
		{
			bot.SendMessage(QQ_t(qq_64), MessageChain().Plain("该订阅不存在"));
			return;
		}
		temp = "/" + newUid;
		Pointer(temp.c_str()).Erase(bili_live_json);
	}
	//关闭订阅
	if (mode == "关闭")
	{
		if (isNew_uid)
		{
			bot.SendMessage(QQ_t(qq_64), MessageChain().Plain("该订阅不存在"));
			return;
		}
		else if (isNew_id)
		{
			bot.SendMessage(QQ_t(qq_64), MessageChain().Plain("未开启此订阅"));
			return;
		}
		else
		{
			//对于一个uid无人开启的处理
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
					bot.SendMessage(QQ_t(qq_64), MessageChain().Plain("未开启此订阅"));
					return;
				}
				//遍历所有的状态，查看是否都为关闭
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
	WriteFile(file.c_str(), output);
	//刷新uid
	if (type == "直播")
	{
		Reload_live_uid();
	}
	else
	{
		Reload_dynamic_uid();
	}
	//消息回复
	bot.SendMessage(QQ_t(qq_64), MessageChain().Plain(mode).Plain("成功"));
	return;
}