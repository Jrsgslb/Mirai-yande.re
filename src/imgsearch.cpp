#include "../include/imgsearch.h"
#include "../include/HttpRequest.hpp"

#include <iostream>
#include <regex>

#include <rapidjson/pointer.h>
//解析json，使所有值都以字符串的方式返回
string json_parse(Value& json, string start, string end)
{
	string point = start + "/" + end;
	//不存在的值
	if (!Pointer(start.c_str()).Get(json)->HasMember(end.c_str()))
	{
		return string("");
	}
	//字符串
	if (Pointer(point.c_str()).Get(json)->IsString())
	{
		return Pointer(point.c_str()).Get(json)->GetString();
	}
	//整形
	if (Pointer(point.c_str()).Get(json)->IsInt())
	{
		return to_string(Pointer(point.c_str()).Get(json)->GetInt());
	}
	//int64
	if (Pointer(point.c_str()).Get(json)->IsInt64())
	{
		return to_string(Pointer(point.c_str()).Get(json)->GetInt64());
	}
	//布尔
	if (Pointer(point.c_str()).Get(json)->IsBool())
	{
		if (Pointer(point.c_str()).Get(json)->IsTrue())
		{
			return string("true");
		}
		else
		{
			return string("false");
		}
	}
	//数组
	if (Pointer(point.c_str()).Get(json)->IsArray())
	{
		point = point + "/0";
		return Pointer(point.c_str()).Get(json)->GetString();
	}
	//浮点型
	if (Pointer(point.c_str()).Get(json)->IsDouble())
	{
		return to_string(Pointer(point.c_str()).Get(json)->GetDouble());
	}
	//NULL
	if (Pointer(point.c_str()).Get(json)->IsNull())
	{
		return string("");
	}
}

bool a2d_search(bool proxy, string proxy_rule, string proxy_add, string url, MiraiBot& bot, MessageType type, int64_t id, int msid)
{
	//获取色调与特征信息
	try
	{
		Document p;
		string host, file, color, bovw, repl_color, repl_bovw;
		host = "https://ascii2d.net/search/url/" + url;
		bool msty = true;
		//辨认消息
		if (type == MessageType::GroupMessage)
		{
			msty = true;
		}
		else if (type == MessageType::FriendMessage)
		{
			msty = false;
		}
		//GET
		HttpRequest r;
		color = r.Http_Get_a2d(host, proxy, proxy_rule, proxy_add);
		//正则匹配图片hash和色调搜索结果
		regex search_hash("class='hash'>(.*?)<");
		regex search_res("rel=\"noopener\" href=\"(.*?)\">(.*?)<");
		smatch hash, res, pic;
		string::const_iterator iterStart = color.begin(), iterEnd = color.end();
		if (!regex_search(color, hash, search_hash))
		{
			printf("正则匹配失败，可能为网络错误\n");
			return false;
		}
		//特征搜索
		host = "https://ascii2d.net/search/bovw/" + hash.str(1);
		bovw = r.Http_Get_a2d(host, proxy, proxy_rule, proxy_add);
		//判断状态码
		if (bovw.empty())
		{
			return false;
		}
		//取出色调结果
		vector<string> res_url, res_author;
		while (regex_search(iterStart, iterEnd, res, search_res))
		{
			res_url.push_back(res.str(1));
			res_author.push_back(res.str(2));
			iterStart = res[0].second; //更新搜索起始位置，搜索剩下的字符串
		}
		//构建消息
		repl_color = res_author[0] + "\n原图地址:" + res_url[0] + "\nAuthor:" + res_url[1];

		//正则匹配色调与特征图片url
		regex search_pic("<img loading=\"lazy\" src=\"(.*?)\"");
		//色调pic
		regex_search(color, pic, search_pic);
		file = pic.str(1);
		file = "./temp/" + file.substr(20, 50);
		host = "https://ascii2d.net" + pic.str(1);
		//下载图片
		//发送消息
		if (r.DownloadImg(host, file, proxy, proxy_rule, proxy_add))
		{
			if (msty)
			{
				GID_t gid = GID_t(id);
				GroupImage img = bot.UploadGroupImage(file);
				bot.SendMessage(gid, MessageChain().Plain("色调搜索结果：\n").Image(img).Plain(repl_color), msid);
			}
			else
			{
				QQ_t gid = QQ_t(id);
				FriendImage img = bot.UploadFriendImage(file);
				bot.SendMessage(gid, MessageChain().Plain("色调搜索结果：\n").Image(img).Plain(repl_color));
			}
		}
		else
		{
			if (msty)
			{
				GID_t gid = GID_t(id);
				GroupImage img = bot.UploadGroupImage(file);
				bot.SendMessage(gid, MessageChain().Plain("网络错误，结果不包含图片"), msid);
				bot.SendMessage(gid, MessageChain().Plain("色调搜索结果：\n").Plain(repl_color), msid);
			}
			else
			{
				QQ_t gid = QQ_t(id);
				FriendImage img = bot.UploadFriendImage(file);
				bot.SendMessage(gid, MessageChain().Plain("网络错误，结果不包含图片"));
				bot.SendMessage(gid, MessageChain().Plain("色调搜索结果：\n").Plain(repl_color));
			}
		}
		//特征pic
		regex_search(bovw, pic, search_pic);
		file = pic.str(1);
		file = "./temp/" + file.substr(20, 50);
		host = "https://ascii2d.net" + pic.str(1);
		//取出特征结果
		iterStart = bovw.begin(), iterEnd = bovw.end();
		res_url.clear();
		res_author.clear();
		while (regex_search(iterStart, iterEnd, res, search_res))
		{
			res_url.push_back(res.str(1));
			res_author.push_back(res.str(2));
			iterStart = res[0].second; //更新搜索起始位置，搜索剩下的字符串
		}
		//构建消息
		repl_bovw = res_author[0] + "\n原图地址:" + res_url[0] + "\nAuthor:" + res_url[1];
		//发送消息
		if (r.DownloadImg(host, file, proxy, proxy_rule, proxy_add))
		{
			if (msty)
			{
				GID_t gid = GID_t(id);
				GroupImage img = bot.UploadGroupImage(file);
				bot.SendMessage(gid, MessageChain().Plain("特征搜索结果：\n").Image(img).Plain(repl_bovw), msid);
			}
			else
			{
				QQ_t gid = QQ_t(id);
				FriendImage img = bot.UploadFriendImage(file);
				bot.SendMessage(gid, MessageChain().Plain("特征搜索结果：\n").Image(img).Plain(repl_bovw));
			}
		}
		else
		{
			if (msty)
			{
				GID_t gid = GID_t(id);
				GroupImage img = bot.UploadGroupImage(file);
				bot.SendMessage(gid, MessageChain().Plain("网络错误，结果不包含图片"), msid);
				bot.SendMessage(gid, MessageChain().Plain("特征搜索结果：\n").Plain(repl_bovw), msid);
			}
			else
			{
				QQ_t gid = QQ_t(id);
				FriendImage img = bot.UploadFriendImage(file);
				bot.SendMessage(gid, MessageChain().Plain("网络错误，结果不包含图片"));
				bot.SendMessage(gid, MessageChain().Plain("特征搜索结果：\n").Plain(repl_bovw));
			}
		}
		return true;
	}
	catch (const std::exception& err)
	{
		printf("%s \n", err.what());
		return false;
	}
}

bool snao_search(bool proxy, string proxy_rule, string proxy_add, string url, MiraiBot& bot, MessageType type, int64_t id, int msid, int min_match, string api_key)
{
	Document d;
	string txt, snao_res, rurl;
	HttpRequest r;
	bool D_img = true,msty = true;
	//辨认消息
	GroupImage img;
	if (type == MessageType::GroupMessage)
	{
		msty = true;
	}
	else if (type == MessageType::FriendMessage)
	{
		msty = false;
	}
	//搜索图片
	rurl = "https://saucenao.com/search.php?db=999&output_type=2&testmode=1&numres=1&url=" + url + "&api_key=" + api_key;
	txt = r.Http_Get(rurl, proxy, proxy_rule, proxy_add);

	if (txt.empty())
	{
		return false;
	}

	//解析搜索结果
	try
	{
		string name, match;
		Document snao_json;
		snao_json.Parse(txt.c_str());

		if (Pointer("/header/status").Get(snao_json)->GetInt() > 0)
		{
			return false;
		}
		match = Pointer("/results/0/header/similarity").Get(snao_json)->GetString();
		//分别本子与图片，防止解析错误
		if (strstr(txt.c_str(), "eng_name"))
		{
			snao_res = "相似度：" + match + "%\n来源：" + json_parse(snao_json, "/results/0/data", "source") + "\n名字:" + json_parse(snao_json, "/results/0/data", "eng_name");
		}
		else
		{
			string member;
			//分别搜索画师id信息
			snao_res = json_parse(snao_json, "/results/0/data", "title");
			snao_res = snao_res + "\n相似度：" + match + "%\n图片地址：" + json_parse(snao_json, "/results/0/data", "ext_urls");
			member = json_parse(snao_json, "/results/0/data", "member_name");
			if (!member.empty())
			{
				snao_res = snao_res +"\n画师：" + member + "\n画师Id:" + json_parse(snao_json, "/results/0/data", "member_id");
			}
		}
		
		srand((unsigned)time(NULL));
		name = "./temp/" + to_string(rand());
		name = name + ".jpg";
		
		//发送消息
		if (r.DownloadImg(Pointer("/results/0/header/thumbnail").Get(snao_json)->GetString(), name, proxy, proxy_rule, proxy_add))
		{
			if (msty)
			{
				GID_t gid = GID_t(id);
				GroupImage img = bot.UploadGroupImage(name);
				bot.SendMessage(gid, MessageChain().Image(img).Plain(snao_res), msid);
				if (stod(match) < min_match)
				{
					bot.SendMessage(gid, MessageChain().Plain("相似度过低，将使用ascii2d搜索"), msid);
					a2d_search(proxy, proxy_rule, proxy_add, url, bot, type, id, msid);
				}
			}
			else
			{
				QQ_t gid = QQ_t(id);
				FriendImage img = bot.UploadFriendImage(name);
				bot.SendMessage(gid, MessageChain().Image(img).Plain(snao_res));
				if (stod(match) < min_match)
				{
					bot.SendMessage(gid, MessageChain().Plain("相似度过低，将使用ascii2d搜索"));
					if (!a2d_search(proxy, proxy_rule, proxy_add, url, bot, type, id, msid))
					{
						return false;
					}
				}
			}			
		}
		else
		{
			if (msty)
			{
				GID_t gid = GID_t(id);
				GroupImage img = bot.UploadGroupImage(name);
				bot.SendMessage(gid, MessageChain().Plain("网络错误，结果不包含图片"), msid);
				bot.SendMessage(gid, MessageChain().Plain(snao_res), msid);
				if (stod(match) < min_match)
				{
					bot.SendMessage(gid, MessageChain().Plain("相似度过低，将使用ascii2d搜索"), msid);
					a2d_search(proxy, proxy_rule, proxy_add, url, bot, type, id, msid);
				}
			}
			else
			{
				QQ_t gid = QQ_t(id);
				FriendImage img = bot.UploadFriendImage(name);
				bot.SendMessage(gid, MessageChain().Plain("网络错误，结果不包含图片"));
				bot.SendMessage(gid, MessageChain().Plain(snao_res));
				if (stod(match) < min_match)
				{
					bot.SendMessage(gid, MessageChain().Plain("相似度过低，将使用ascii2d搜索"));
					a2d_search(proxy, proxy_rule, proxy_add, url, bot, type, id, msid);
				}
			}
		}
		return true;
	}
	catch (const std::exception& err)
	{
		printf("%s \n", err.what());
		return false;
	}
}