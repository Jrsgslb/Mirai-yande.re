#include <iostream>
#include <random>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <vector>

#include <cpr/cpr.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp> 

#include "rapidjson/pointer.h"

#include< urlmon.h >

#include "yande.h"

#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib,"urlmon.lib")

using namespace std;
using namespace cpr;
using namespace boost::property_tree;
using namespace rapidjson;

vector<string> yande(string plain, bool proxy, string http, string https, bool file_url, int64_t group_num)
{
	vector<string> id_info;
	//读取tag信息
	ptree p;
	ini_parser::read_ini("./config/rule.ini", p);
	basic_ptree<string, string> tag = p.get_child(plain);
	string tags = tag.get<string>("tag");
	//取随机数
	default_random_engine e;
	uniform_int_distribution<unsigned> u(1, tag.get<int>("num"));
	e.seed(GetUnixTime());
	string page = to_string(u(e));

	Response r;
	//5 秒超时
	if (proxy)
		r = Get(Url{ "https://yande.re/post.json" }, Parameters{ {"page", page.c_str()}, {"tags", tags.c_str()}, {"limit","1"} }, Proxies{ {"https", https} }, Timeout{5000});
	else
		r = Get(Url{ "https://yande.re/post.json" }, Parameters{ {"page", page.c_str()}, {"tags", tags.c_str()}, {"limit","1"} }, Timeout{5000});
	if (r.status_code != 200)
	{
		id_info.push_back("网络错误或tag填写错误");
		return id_info;
	}
	//读取json
	Document y;
	y.Parse(r.text.c_str());
	string  url, file,rating,r18_temp;
	int id;
	id = Pointer("/0/id").Get(y)->GetInt();
	if (file_url)
	{
		url = Pointer("/0/file_url").Get(y)->GetString();
		file = "./temp/" + to_string(id) + "." + Pointer("/0/file_ext").Get(y)->GetString();
	}
	else
	{
		url = Pointer("/0/sample_url").Get(y)->GetString();
		file = "./temp/" + to_string(id) + ".jpg";
	}
	rating = Pointer("/0/rating").Get(y)->GetString();
	//R18限制
	ptree r18;
	ini_parser::read_ini("./config/data/group.ini", r18);
	r18_temp = to_string(group_num) + ".R18";
	if (rating < tag.get<string>("rating") && !r18.get<bool>(r18_temp.c_str(), false))
	{
		id_info.push_back("这张图片不适合在本群观看哦");
		return id_info;
	}
	//push数据
	id_info.push_back(to_string(id));
	id_info.push_back(url);
	id_info.push_back(file);
	id_info.push_back(rating);
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

bool DownloadImg(string url, string file, bool proxy, string https, string http)
{
	//代理设置
	if (proxy)
	{
		http = "set http_proxy = http://" + http;
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