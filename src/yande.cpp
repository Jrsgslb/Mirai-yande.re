﻿#include "../include/yande.h"
#include "../include/HttpRequest.hpp"

#include <iostream>
#include <random>
#include <regex>
#include <time.h>
#include <cstdio>
#include <io.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp> 

#include <rapidjson/pointer.h>

using namespace boost::property_tree;


//y站指定tag图片获取
Document yande(string plain, bool proxy, string proxy_rule, string proxy_add, int64_t group_num, bool first, bool original)
{
	try
	{
		int num, num_num;
		Document id_info;
		//读取tag信息
		ptree p, num_time;
		ini_parser::read_ini("./config/rule.ini", p);
		basic_ptree<string, string> tag = p.get_child(plain);
		string tags = tag.get<string>("tag"), plain_str;

		time_t timep;
		struct tm* t;
		time(&timep);
		t = localtime(&timep);
		//写入调用次数
		ini_parser::read_ini("./temp/num.ini", num_time);
		num_num = num_time.get<int>(plain_str.c_str(), 0);
		if (first)
		{
			plain_str = plain + "." + to_string(t->tm_year + 1900) + "/" + to_string(t->tm_mon + 1) + "/" + to_string(t->tm_mday);
			num_num = num_time.get<int>(plain_str.c_str(), 0) + 1;
			num_time.put<int>(plain_str.c_str(), num_num);
			ini_parser::write_ini("./temp/num.ini", num_time);
		}
		if (tag.get<int>("send", 1) > 1)
		{
			Pointer("/count").Set(id_info, tag.get<int>("send", 1));
		}
		else
		{
			Pointer("/count").Set(id_info, 1);
		}
		//老图片过滤机制
		if (tag.get<int>("num") >= 1000)
		{
			if (num_num > 500 && tag.get<int>("num") > 2000)
			{
				num = 1000;
			}
			else
			{
				num = 500;
			}
		}
		else
			num = tag.get<int>("num");
		//取随机数
		default_random_engine e;
		uniform_int_distribution<unsigned> u(1, num);
		e.seed(GetUnixTime());
		string page = to_string(u(e)), url, txt;
		HttpRequest r;

		url = "https://yande.re/post.json?page=" + page + "&tags=" + tags + "&limit=1";
		txt = r.Http_Get(url, proxy, proxy_rule, proxy_add);

		if (txt.empty())
		{
			Pointer("/code").Set(id_info, 0);
			Pointer("/info").Set(id_info, "发生错误，详见控制台");
			return id_info;
		}

		//读取json
		Document y;
		y.Parse(txt.c_str());
		//R18限制
		string  rating, r18_temp;
		rating = Pointer("/0/rating").Get(y)->GetString();
		ptree r18;
		ini_parser::read_ini("./config/data/group.ini", r18);
		r18_temp = to_string(group_num) + ".R18";
		if (rating < tag.get<string>("rating") && !r18.get<bool>(r18_temp.c_str(), false))
		{
			//递归大法好
			id_info = yande(plain, proxy, proxy_rule, proxy_add, group_num, false, original);
			return id_info;
		}
		//push数据
		Pointer("/code").Set(id_info, 1);
		Pointer("/file/url").Set(id_info, Pointer("/0/file_url").Get(y)->GetString());
		Pointer("/file/ext").Set(id_info, Pointer("/0/file_ext").Get(y)->GetString());
		Pointer("/sample/url").Set(id_info, Pointer("/0/sample_url").Get(y)->GetString());
		string name, durl, id;
		id = to_string(Pointer("/0/id").Get(y)->GetInt());
		if (original)
		{
			name = "./temp/" + id + "." + Pointer("/0/file_ext").Get(y)->GetString();
			durl = Pointer("/0/file_url").Get(y)->GetString();
		}
		else
		{
			name = "./temp/" + id + ".jpg";
			durl = Pointer("/0/sample_url").Get(y)->GetString();
		}
		if (r.DownloadImg(durl, name, proxy, proxy_rule, proxy_add))
		{
			Pointer("/dimg").Set(id_info, 1);
		}
		else
		{
			Pointer("/dimg").Set(id_info, 0);
		}
		Pointer("/id").Set(id_info, id.c_str());
		Pointer("/name").Set(id_info, name.c_str());
		Pointer("/durl").Set(id_info, durl.c_str());
		return id_info;
	}
	catch (const std::exception& err)
	{
		Document info;
		printf("%s \n", err.what());
		Pointer("/code").Set(info, 0);
		Pointer("/info").Set(info, "发生错误，详见控制台");
		return info;
	}
}
//y站指定id图片获取
Document yid(string id, bool proxy, string proxy_rule, string proxy_add, int64_t group_num)
{
	try
	{
		Document info;
		string url, rating, r18_temp, name, txt;
		url = "https://yande.re/post/show/" + id;
		//访问id页面
		HttpRequest r;
		txt = r.Http_Get(url, proxy, proxy_rule, proxy_add);
		if (txt.empty())
		{
			Pointer("/code").Set(info, 0);
			Pointer("/info").Set(info, "发生错误，详见控制台");
			return info;
		}
		else
		{
			Pointer("/code").Set(info, 1);
		}

		//正则查找
		regex rating_regex("Rating: (.*?) <span class=\"vote-desc\">");
		regex png_img_regex("<a class=\"original-file-unchanged\" id=\"([a-z]{3,4})\" href=\"(.*?)\">");
		regex file_img_regex("<a class=\"original-file-changed\" id=\"highres\" href=\"(.*?)\">");
		cmatch rating_res, png_img_res, file_img_res;

		regex_search(txt.c_str(), rating_res, rating_regex);
		rating = rating_res[1];
		if (rating == "Safe")
		{
			rating = "s";
		}
		else if (rating == "Questionable")
		{
			rating = "q";
		}
		else if (rating == "Explicit")
		{
			rating = "e";
		}
		else
		{
			Pointer("/code").Set(info, 0);
			Pointer("/info").Set(info, "鬼知道出了什么错？？？");
			return info;
		}
		ptree r18;
		ini_parser::read_ini("./config/data/group.ini", r18);
		r18_temp = to_string(group_num) + ".R18";
		if (rating == "e" && !r18.get<bool>(r18_temp.c_str(), false))
		{
			Pointer("/code").Set(info, 0);
			Pointer("/info").Set(info, "这张图片为限制图片，不能看哦~");
			return info;
		}
		Pointer("/rating").Set(info, rating.c_str());
		if (!regex_search(txt.c_str(), png_img_res, png_img_regex))
		{
			regex_search(txt.c_str(), file_img_res, file_img_regex);
			name = "./temp/" + id + ".jpg";
			Pointer("/name").Set(info, name.c_str());
			Pointer("/url").Set(info, file_img_res.str(1).c_str());
			if (r.DownloadImg(file_img_res.str(1), name, proxy, proxy_rule, proxy_add))
			{
				Pointer("/dimg").Set(info, 1);
			}
			else
			{
				Pointer("/dimg").Set(info, 0);
			}
			return info;
		}
		name = "./temp/" + id + "." + png_img_res.str(1);
		Pointer("/name").Set(info, name.c_str());
		Pointer("/url").Set(info, png_img_res.str(2).c_str());
		if (r.DownloadImg(png_img_res.str(2), name, proxy, proxy_rule, proxy_add))
		{
			Pointer("/dimg").Set(info, 1);
		}
		else
		{
			Pointer("/dimg").Set(info, 0);
		}
		return info;
	}
	catch (const std::exception& err)
	{
		Document info;
		printf("%s \n", err.what());
		Pointer("/code").Set(info, 0);
		Pointer("/info").Set(info, "发生错误，详见控制台");
		return info;
	}
}
//获取13位时间戳
long long GetUnixTime()
{
	int64_t times = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return times;
}
//清理./temp文件夹下的文件
bool ClearTemp()
{
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst("./temp/*.*", &findData);    // 查找目录中的第一个文件
	if (handle == -1) return false;
	string file;
	do
	{
		if (strcmp(findData.name, ".") && strcmp(findData.name, ".."))
		{
			file = findData.name;
			if (file != "num.ini")
			{
				file = "./temp/" + file;
				printf("Del: %s \n", findData.name);
				remove(file.c_str());
			}
		}
	} while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件
	_findclose(handle);    // 关闭搜索句柄
	return true;
}
//更新tag
string MessageReload(bool proxy, string proxy_rule, string proxy_add)
{
	ptree pt, limit;
	ini_parser::read_ini("./config/rule.ini", pt);

	basic_ptree<string, string> tag = pt.get_child("");

	string err, comm;
	//遍历rule.ini
	for (auto i = tag.begin(); i != tag.end(); i++)
	{
		smatch res;
		regex reg("<posts count=\"([0-9]*)\" offset=\"([0-9]*)\"");
		basic_ptree<string, string> temp = pt.get_child((*i).first.data());
		string tags, txt, num, url, text;

		tags = temp.get<string>("tag", "");
		if (tags.empty())
		{
			continue;
		}

		num = (*i).first.data();

		if (i == tag.begin()) comm = (*i).first.data();
		else
			comm = comm + "\n" + (*i).first.data();
		
		//获取网页并检验状态
		HttpRequest r;
		url = "https://yande.re/post.xml?tags=" + tags;
		text = r.Http_Get(url, proxy, proxy_rule, proxy_add);

		if (text.empty())err = tags + "\n" + err;
		else
		{
			//正则取出次数
			auto pos = text.cbegin();
			auto end = text.cend();
			for (; regex_search(pos, end, res, reg); pos = res.suffix().first) txt = res.str(1);
			//写入文件
			num = num + ".num";
			pt.put<string>(num.c_str(), txt.c_str());
			ini_parser::write_ini("./config/rule.ini", pt);
		}
		
	}

	FILE* tem = fopen("./config/command.txt", "w");
	fprintf(tem, "%s", comm.c_str());
	fclose(tem);

	//检查指令文件有无空行
	ifstream in("./config/command.txt");
	string line, str;
	bool first = true;
	while (getline(in, line))
	{
		if (line == "")
		{
			continue;
		}
		if (first)
		{
			first = false;
			str = str + line;
			continue;
		}
		str = str + "\n" + line;
	}

	tem = fopen("./config/command.txt", "w");
	fprintf(tem, "%s", str.c_str());
	fclose(tem);
	in.close();

	if (!err.empty())return err;
	else return "ok";
}
//热门排行榜图片
Document Hot_Img(bool proxy, string proxy_rule, string proxy_add, int64_t group, bool orginal)
{
	HttpRequest r;
	//取随机数
	default_random_engine e;
	uniform_int_distribution<unsigned> u(0, 39);
	e.seed(GetUnixTime());
	string page = to_string(u(e)), txt, temp, url, rating, file, id;
	//解析json
	Document Hot_img_json, res;
	txt = r.Http_Get("https://yande.re/post/popular_recent.json", proxy, proxy_rule, proxy_add);
	if (txt.size() < 100)
	{
		Pointer("/code").Set(res, 0);
		Pointer("/info").Set(res, "发送错误，详见控制台");
		return res;
	}
	Hot_img_json.Parse(txt.c_str());
	//R18
	ptree r18;
	temp = "/" + page + "/rating";
	rating = Pointer(temp.c_str()).Get(Hot_img_json)->GetString();
	ini_parser::read_ini("./config/data/group.ini", r18);
	temp = to_string(group) + ".R18";
	if (rating == "e" && !r18.get<bool>(temp.c_str(), false))
	{
		Pointer("/code").Set(res, 0);
		Pointer("/info").Set(res, "这张图片为限制图片，不能看哦~");
		return res;
	}
	//图片信息处理
	temp = "/" + page + "/id";
	id = to_string(Pointer(temp.c_str()).Get(Hot_img_json)->GetInt());
	Pointer("/id").Set(res, id.c_str());
	if (orginal)
	{
		temp = "/" + page + "/file_url";
		url = Pointer(temp.c_str()).Get(Hot_img_json)->GetString();
		temp = "/" + page + "/file_ext";
		file = "./temp/" + id + "." + Pointer(temp.c_str()).Get(Hot_img_json)->GetString();
		if (!r.DownloadImg(url, file, proxy, proxy_rule, proxy_add))
		{
			Pointer("/code").Set(res, 0);
			Pointer("/info").Set(res, "发送错误，详见控制台");
			return res;
		}
		Pointer("/code").Set(res, 1);
		Pointer("/name").Set(res, file.c_str());
	}
	else
	{
		temp = "/" + page + "/sample_url";
		url = Pointer(temp.c_str()).Get(Hot_img_json)->GetString();
		file = "./temp/" + id + ".jpg";
		if (!r.DownloadImg(url, file, proxy, proxy_rule, proxy_add))
		{
			Pointer("/code").Set(res, 0);
			Pointer("/info").Set(res, "发送错误，详见控制台");
			return res;
		}
		Pointer("/code").Set(res, 1);
		Pointer("/name").Set(res, file.c_str());
	}
	return res;
}