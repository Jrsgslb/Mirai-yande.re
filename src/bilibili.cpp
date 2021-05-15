#include <iostream>

#include <rapidjson/pointer.h>
#include <rapidjson/document.h>

#include "../include/Bilibili.h"
#include "../include/message.h"
#include "../include/HttpRequest.hpp"

using namespace rapidjson;
using namespace std;

vector<string> live_uid;
map<string, bool> live_status;
bool first;

bool Reload_live_uid()
{
	string bili_live_json_txt = ReloadFile("./config/bili/live.json"), temp, uid;
	Document bili_live_json;
	if (bili_live_json_txt.empty())
	{
		return true;
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
	return true;
}

void Bilibili_live(MiraiBot& bot)
{
	HttpRequest r;
	string txt, url, uid, temp;
	Document bili_live;
	for (int i = 0; i < live_uid.size(); i++)
	{
		uid = live_uid[i];
		url = "https://api.live.bilibili.com/room/v1/Room/getRoomInfoOld?mid=" + uid;
		txt = r.Http_Get_Bili(url, false, "1");
		bili_live.Parse(txt.c_str());
		int status = Pointer("/data/liveStatus").Get(bili_live)->GetInt();
		if (status == 1 && !live_status[uid])
		{
			txt = ReloadFile("./config/bili/live.json");
			Document uid_group;
			uid_group.Parse(txt.c_str());
			temp = "/" + to_string(i) + "/send";
			int size = Pointer(temp.c_str()).Get(uid_group)->Size();
			for (int j = 0; j < size; j++)
			{
				temp = "/" + to_string(i) + "/send/" + to_string(j) + "/status";
				if (Pointer(temp.c_str()).Get(uid_group)->GetInt() == 1)
				{
					temp = "/" + to_string(i) + "/send/" + to_string(j) + "/id";
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
			txt = r.Http_Get_Bili(url, false, "1");
			uid_info.Parse(txt.c_str());
			txt = ReloadFile("./config/bili/live.json");

			uid_group.Parse(txt.c_str());
			temp = "/" + to_string(i) + "/send";
			int size = Pointer(temp.c_str()).Get(uid_group)->Size();
			for (int j = 0; j < size; j++)
			{
				temp = "/" + to_string(i) + "/send/" + to_string(j) + "/status";
				if (Pointer(temp.c_str()).Get(uid_group)->GetInt() == 1)
				{
					GroupImage img;
					img.Url = Pointer("/data/info/face").Get(uid_info)->GetString();
					temp = "/" + to_string(i) + "/send/" + to_string(j) + "/id";
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

		srand((unsigned)time(NULL));
		page_num = (rand() % 499) + 0;

		url = "https://api.vc.bilibili.com/link_draw/v2/Photo/list?category=cos&type=hot&page_num=" + to_string(page_num) + "&page_size=1";
		txt = r.Http_Get(url, false, "1");
		d.Parse(txt.c_str());
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
			bot.SendMessage(gid, MessageChain().Image(img));
		}

		return true;
	}
	catch (const std::exception& err)
	{
		cout << err.what() << endl;
	}
}