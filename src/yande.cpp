#include <iostream>
#include <random>
#include <fstream>
#include <ctime>
#include <stdio.h>

#include <cpr/cpr.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ini_parser.hpp> 

#include< urlmon.h >

#include "json/json.h"

#include "yande.h"

#define BOOST_SPIRIT_THREADSAFE

#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib,"urlmon.lib")
using namespace std;

using namespace cpr;
using namespace boost::property_tree;
using namespace Json;

string yande(string plain, bool proxy, string http, string https, bool file_url)
{
	ptree p;
	ini_parser::read_ini("./config/rule.ini", p);
	basic_ptree<string, string> tag = p.get_child(plain);
	string tags = tag.get<string>("tag");

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
	if (r.status_code != 200)return "error";
	stringstream ss(r.text);
	ptree pt;
	// 读取JSON数据
	read_json(ss, pt);
	string tem = "./temp/" + page + "temp.json";
	write_json(tem.c_str(), pt);

	Json::Reader reader;
	Json::Value root;
	ifstream is;
	is.open(tem.c_str(), ios::binary);
	if (!reader.parse(is, root))
	{
		remove(tem.c_str());
		return "error";
	}
	is.close();
	remove(tem.c_str());
	//级别判断
	if (root[""]["rating"].asString() < tag.get<string>("rating")) return "r18";
	string url, file;
	if (file_url)
	{
		url = root[""]["file_url"].asString();
		file = "./temp/" + root[""]["id"].asString() + "." + root[""]["file_ext"].asString();
	}
	else
	{
		url = root[""]["sample_url"].asString();
		file = "./temp/" + root[""]["id"].asString() + ".jpg";
	}

	if (proxy)
	{
		http = "set http_proxy = http://" + http;
		https = "set https_proxy = https://" + https;
		system(http.c_str());
		system(https.c_str());
	}
	
	HRESULT hr = URLDownloadToFile(NULL, url.c_str(), file.c_str(), 0, NULL);
	if (hr != S_OK)
		return "error";
	return file;
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