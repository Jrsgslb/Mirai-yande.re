#include <iostream>
#include <map>
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <ctime>

#include <cpr/cpr.h>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include< urlmon.h >

#include "Json.h"
#include "json/json.h"

#define BOOST_SPIRIT_THREADSAFE

#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib,"urlmon.lib")
using namespace std;

using namespace cpr;
using namespace rapidjson;
using namespace boost::property_tree;
using namespace Json;

int yande(int groupid) {

	srand((unsigned)time(NULL));
	int rands = rand() % 100 + 1;
	string page = to_string(rands);
	cout << page.c_str() << endl;
	Response r = Get(Url{ "https://yande.re/post.json" }, Parameters{ {"page", page.c_str()}, {"tags", "loli"}, {"limit","1"} }, Proxies{ {"https", "127.0.0.1:1080"} });
	stringstream ss(r.text);
	ptree pt;
	// 读取JSON数据
	read_json(ss, pt);
	write_json("./tmp.json", pt);

	Json::Reader reader;// 解析json用Json::Reader
	Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array
	ifstream is;
	is.open("./tmp.json", ios::binary);
	if (!reader.parse(is, root)) {
	}
	else {
		std::cout << root[""]["file_url"].asString() << endl;
	}
	is.close();


	string url = root[""]["file_url"].asString();
	system("set http_proxy=http://127.0.0.1:1080");
	system("set https_proxy = https://127.0.0.1:1080");
	HRESULT hr = URLDownloadToFile(NULL, url.c_str(), _T("./sky1.jpg"), 0, NULL);
	if (hr == S_OK)
	{
		cout << "ok" << endl;
	}
	else
		return 2;
	return 1;
}