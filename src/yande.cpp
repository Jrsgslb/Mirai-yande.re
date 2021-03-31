﻿#include "../include/yande.h"

#include <cpr/cpr.h>

#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib,"urlmon.lib")

using namespace cpr;

Document yande(string plain, bool proxy, string https, int64_t group_num)
{
	int num;
	Document id_info;
	//读取tag信息
	ptree p, num_time;
	ini_parser::read_ini("./config/rule.ini", p);
	basic_ptree<string, string> tag = p.get_child(plain);
	string tags = tag.get<string>("tag");

	time_t timep;
	struct tm* t;
	time(&timep);
	t = localtime(&timep);
	//写入调用次数
	ini_parser::read_ini("./temp/num.ini", num_time);
	plain = plain + "." + to_string(t->tm_year + 1900) + "/" + to_string(t->tm_mon + 1) + "/" + to_string(t->tm_mday);
	int num_num = num_time.get<int>(plain.c_str(), 0) + 1;
	num_time.put<int>(plain.c_str(), num_num);
	ini_parser::write_ini("./temp/num.ini", num_time);
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
	string page = to_string(u(e));

	Response r;
	//5 秒超时
	if (proxy)
		r = Get(Url{ "https://yande.re/post.json" }, Parameters{ {"page", page.c_str()}, {"tags", tags.c_str()}, {"limit","1"} }, Proxies{ {"https", https} }, Timeout{ 5000 });
	else
		r = Get(Url{ "https://yande.re/post.json" }, Parameters{ {"page", page.c_str()}, {"tags", tags.c_str()}, {"limit","1"} }, Timeout{ 5000 });
	if (r.status_code != 200)
	{
		Pointer("/code").Set(id_info, 0);
		Pointer("/info").Set(id_info, r.error.message.c_str());
		return id_info;
	}
	//读取json
	Document y;
	y.Parse(r.text.c_str());
	//R18限制
	string  rating, r18_temp;
	rating = Pointer("/0/rating").Get(y)->GetString();
	ptree r18;
	ini_parser::read_ini("./config/data/group.ini", r18);
	r18_temp = to_string(group_num) + ".R18";
	if (rating < tag.get<string>("rating") && !r18.get<bool>(r18_temp.c_str(), false))
	{
		Pointer("/code").Set(id_info, 0);
		Pointer("/info").Set(id_info, "这张图片不适合在本群观看哦");
		return id_info;
	}
	//push数据
	Pointer("/code").Set(id_info, 1);
	Pointer("/file/url").Set(id_info, Pointer("/0/file_url").Get(y)->GetString());
	Pointer("/file/ext").Set(id_info, Pointer("/0/file_ext").Get(y)->GetString());
	Pointer("/sample/url").Set(id_info, Pointer("/0/sample_url").Get(y)->GetString());
	Pointer("/id").Set(id_info, Pointer("/0/id").Get(y)->GetInt());
	return id_info;
}

//获取13位时间戳
static __int64 GetUnixTime()
{
	string nowTimeUnix;
	string cs_uninxtime;
	string cs_milliseconds;
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	time_t unixTime;
	time(&unixTime);
	char buf[30], buf1[30];
	sprintf_s(buf, sizeof(buf), "%I64d", (INT64)unixTime);
	sprintf_s(buf1, sizeof(buf1), "%03I64d", (INT64)sysTime.wMilliseconds);
	nowTimeUnix = string(buf) + string(buf1);
	return _atoi64(nowTimeUnix.c_str());
}

bool DownloadImg(string url, string file, bool proxy, string https)
{
	//代理设置
	if (proxy)
	{
		string http;
		http = "set http_proxy = http://" + https;
		https = "set https_proxy = https://" + https;
		system(http.c_str());
		system(https.c_str());
	}

	HRESULT hr = URLDownloadToFile(NULL, url.c_str(), file.c_str(), 0, NULL);
	if (hr != S_OK)
	{
		return false;
	}
	else
		return true;
}

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
				cout << "Del:" << findData.name << endl;
				remove(file.c_str());
			}
		}
	} while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件
	_findclose(handle);    // 关闭搜索句柄
	return true;
}

string MessageReload(bool proxy, string https)
{
	SetConsoleOutputCP(65001);
	ptree pt;
	ini_parser::read_ini("./config/rule.ini", pt);

	basic_ptree<string, string> tag = pt.get_child("");

	string err, comm;
	//遍历rule.ini
	for (auto i = tag.begin(); i != tag.end(); i++)
	{
		Response r;
		smatch res;
		regex reg("<posts count=\"([0-9]*)\" offset=\"([0-9]*)\"");
		basic_ptree<string, string> temp = pt.get_child((*i).first.data());
		string tags = temp.get<string>("tag"), txt, num;
		num = (*i).first.data();

		if (i == tag.begin()) comm = (*i).first.data();
		else
			comm = comm + "\n" + (*i).first.data();

		//获取网页并检验状态
		if (proxy)
			r = Get(Url{ "https://yande.re/post.xml" }, Parameters{ {"tags", tags.c_str()} }, Proxies{ {"https", https} }, Timeout{ 10000 });
		else
			r = Get(Url{ "https://yande.re/post.xml" }, Parameters{ {"tags", tags.c_str()} }, Timeout{ 10000 });
		if (r.status_code != 200)err = tags + "\n" + err;
		else
		{
			//正则取出次数
			auto pos = r.text.cbegin();
			auto end = r.text.cend();
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

	if (!err.empty())return err;
	else return "ok";
}