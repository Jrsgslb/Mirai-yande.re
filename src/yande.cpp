#include <iostream>
#include <stdio.h>
#include <fstream>
#include <ctime>

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

	srand((unsigned)time(NULL));
	int rands = rand() % tag.get<int>("num") + 1;
	string page = to_string(rands);

	Response r;
	if (proxy)
		r = Get(Url{ "https://yande.re/post.json" }, Parameters{ {"page", page.c_str()}, {"tags", tags.c_str()}, {"limit","1"} }, Proxies{ {"https", https} });
	else
		r = Get(Url{ "https://yande.re/post.json" }, Parameters{ {"page", page.c_str()}, {"tags", tags.c_str()}, {"limit","1"} });
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
	
	HRESULT hr = URLDownloadToFile(NULL, url.c_str(), _T(file.c_str()), 0, NULL);
	if (hr != S_OK)
		return "error";
	return file;
}