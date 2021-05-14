#include "../include/imgsearch.h"
#include "../include/HttpRequest.hpp"

#include <iostream>
#include <regex>

#include <rapidjson/pointer.h>


bool a2d_search(bool proxy, string& https, string url, MiraiBot& bot, MessageType type, int64_t id, int msid)
{
	//获取色调与特征信息
	try
	{
		Document p;
		string host, file, color, bovw, repl_color, repl_bovw;
		host = "https://ascii2d.net/search/url/" + url;
		bool D_img = true, msty = true;
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
		color = r.Http_Get(host, proxy, https);
		//正则匹配图片hash和色调搜索结果
		regex search_hash("class='hash'>(.*?)<");
		regex search_res("rel=\"noopener\" href=\"(.*?)\">(.*?)<");
		smatch hash, res, pic;
		string::const_iterator iterStart = color.begin(), iterEnd = color.end();
		regex_search(color, hash, search_hash);
		//特征搜索
		host = "https://ascii2d.net/search/bovw/" + hash.str(1);
		bovw = r.Http_Get(host, proxy, https);
		//判断状态码
		if (color.size() < 100 && bovw.size() < 100)
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
		if (!r.DownloadImg(host, file, proxy, https))
		{
			D_img = false;
		}
		//发送消息
		if (D_img)
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
		//下载图片
		if (!r.DownloadImg(host, file, proxy, https))
		{
			D_img = false;
		}
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
		if (D_img)
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
		cout << err.what() << endl;
		return false;
	}
}

bool snao_search(bool proxy, string& https, string url, MiraiBot& bot, MessageType type, int64_t id, int msid, int min_match)
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
	rurl = "url=" + url;
	txt = r.Http_Post("https://saucenao.com/search.php", proxy, https, rurl);

	if (txt.size() < 200)
	{
		return false;
	}
	//解析搜索结果
	try
	{
		regex match_regex("<div class=\"resultsimilarityinfo\">(.*?)%</div>");
		regex search_regex("<table class=\"resulttable\">(.*?)</table>");
		regex img_regex("src=\"([http|https](.*?))\"");
		regex info_regex("<div class=\"resultcontentcolumn\">(.*?)</div>");
		regex title_regex("<strong>(.*?)</strong>");
		regex id_regex("<a href=\"(.*?)\" class=\"linkify\">(.*?)</a>");
		regex res_regex("<div class=\"resultmiscinfo\"><a href=\"(.*?)\">(.*?)<\/div>");
		smatch match_res, id_res, img_res, title_res, search_res, info_res, res_res;
		string res, info, name;
		regex_search(txt, search_res, search_regex);//匹配第一个搜索结果
		res = search_res.str(1);
		regex_search(res, match_res, match_regex);//搜索匹配度
		regex_search(res, img_res, img_regex);//搜索img url
		regex_search(res, info_res, info_regex);//搜索画师和id信息
		regex_search(res, title_res, title_regex);//搜索标题信息
		info = info_res[0];
		string::const_iterator strStart = info.begin(), strEnd = info.end();

		snao_res = title_res.str(1) + "\n相似度：" + match_res.str(1) + "%\n图片地址：";
		//分别搜索画师id信息
		if (!regex_search(res, res_res, res_regex))
		{
			if (regex_search(strStart, strEnd, id_res, id_regex))
			{
				snao_res = snao_res + id_res.str(1) + "\n画师：";
				strStart = id_res[0].second;
			}
			if (regex_search(strStart, strEnd, id_res, id_regex))
			{
				snao_res = snao_res + id_res.str(1);
			}
		}
		else
		{
			snao_res = snao_res + res_res.str(1);
		}
		
		srand((unsigned)time(NULL));
		name = "./temp/" + to_string(rand());
		name = name + ".jpg";
		//下载图片
		if (!r.DownloadImg(img_res.str(1), name, proxy, https))
		{
			D_img = false;
		}
		//发送消息
		if (D_img)
		{
			if (msty)
			{
				GID_t gid = GID_t(id);
				GroupImage img = bot.UploadGroupImage(name);
				bot.SendMessage(gid, MessageChain().Image(img).Plain(snao_res), msid);
				if (stod(match_res.str(1)) < min_match)
				{
					bot.SendMessage(gid, MessageChain().Plain("相似度过低，将使用ascii2d搜索"), msid);
					a2d_search(proxy, https, url, bot, type, id, msid);
				}
			}
			else
			{
				QQ_t gid = QQ_t(id);
				FriendImage img = bot.UploadFriendImage(name);
				bot.SendMessage(gid, MessageChain().Image(img).Plain(snao_res));
				if (stod(match_res.str(1)) < min_match)
				{
					bot.SendMessage(gid, MessageChain().Plain("相似度过低，将使用ascii2d搜索"));
					if (!a2d_search(proxy, https, url, bot, type, id, msid))
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
				if (stod(match_res.str(1)) < min_match)
				{
					bot.SendMessage(gid, MessageChain().Plain("相似度过低，将使用ascii2d搜索"), msid);
					a2d_search(proxy, https, url, bot, type, id, msid);
				}
			}
			else
			{
				QQ_t gid = QQ_t(id);
				FriendImage img = bot.UploadFriendImage(name);
				bot.SendMessage(gid, MessageChain().Plain("网络错误，结果不包含图片"));
				bot.SendMessage(gid, MessageChain().Plain(snao_res));
				if (stod(match_res.str(1)) < min_match)
				{
					bot.SendMessage(gid, MessageChain().Plain("相似度过低，将使用ascii2d搜索"));
					a2d_search(proxy, https, url, bot, type, id, msid);
				}
			}
		}
		return true;
	}
	catch (const std::exception& err)
	{
		cout << err.what() << endl;
		return false;
	}
}