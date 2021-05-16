#include "../include/Pixiv.h"
#include "../include/HttpRequest.hpp"

#include <rapidjson/pointer.h>


//p站id获取图片
bool Pixiv_id(bool proxy, string& https, string pid, MiraiBot& bot, int64_t group_id, int msid)
{
	try
	{
		GID_t gid = GID_t(group_id);
		Document pixiv;
		string txt, url, name;

		HttpRequest r;
		url = "https://www.pixiv.net/ajax/illust/" + pid;
		txt = r.Http_Get_Pixiv(url, proxy, https);

		if (txt.empty())
		{
			return false;
		}
		pixiv.Parse(txt.c_str());

		if (Pointer("/error").Get(pixiv)->GetBool())
		{
			bot.SendMessage(gid, MessageChain().Plain(Pointer("/message").Get(pixiv)->GetString()), msid);
			return true;
		}
		if (Pointer("/body/pageCount").Get(pixiv)->GetInt() == 1)
		{
			name = "./temp/" + pid + ".jpg";
			url = "https://pixiv.cat/" + pid + ".jpg";
			if (r.DownloadImg(url, name, false, https))
			{
				GroupImage img = bot.UploadGroupImage(name);
				bot.SendMessage(gid, MessageChain().Image(img));
				return true;
			}
			else
			{
				bot.SendMessage(gid, MessageChain().Plain("网络错误,图片发送失败"), msid);
				return true;
			}
		}
		else
		{
			bot.SendMessage(gid, MessageChain().Plain("这是一个多图id，共").Plain(Pointer("/body/pageCount").Get(pixiv)->GetInt()).Plain("张"), msid);
			for (int i = 1; i <= Pointer("/body/pageCount").Get(pixiv)->GetInt(); i++)
			{
				name = "./temp/" + pid + "-" + to_string(i) + ".jpg";
				url = "https://pixiv.cat/" + pid + "-" + to_string(i) + ".jpg";
				if (r.DownloadImg(url, name, false, https))
				{
					GroupImage img = bot.UploadGroupImage(name);
					bot.SendMessage(gid, MessageChain().Image(img));
				}
				else
				{
					bot.SendMessage(gid, MessageChain().Plain("网络错误,图片下载失败"), msid);
				}
			}
			bot.SendMessage(gid, MessageChain().Plain("发送完成，共").Plain(Pointer("/body/pageCount").Get(pixiv)->GetInt()).Plain("张"), msid);
			return true;
		}

		return true;
	}
	catch (const std::exception& err)
	{
		printf("%s \n", err.what());
		return false;
	}
}
//p站排行榜获取
bool Pixiv_rank(bool proxy, string& https, int count, MiraiBot& bot, int64_t group_id)
{
	try
	{
		switch (count)
		{
		default:
			break;
		}
		HttpRequest r;
		string txt;
		//txt = r.Http_Get_Pixiv();
		return true;
	}
	catch (const std::exception& err)
	{
		printf("%s \n", err.what());
		return false;
	}
}