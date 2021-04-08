#include "../include/Bilibili.h"
#include "../include/HttpRequest.hpp"

#include <iostream>

#include <rapidjson/pointer.h>

using namespace std;

Document Bilibili_live(MiraiBot bot)
{
	return 0;
}

Document Bilibili_cos()
{
	Document d;
	HttpRequest r;
	string txt, url;
	int page_num, page_size;

	srand((unsigned)time(NULL));
	page_num = (rand() % (15 - 1 + 1)) + 1;
	page_size = (rand() % (20 - 1 + 1)) + 1;
	//cout << page_num << endl << endl << page_size << endl << endl;
	
	url = "https://api.vc.bilibili.com/link_draw/v2/Photo/list?category=cos&type=hot&page_num=" + to_string(page_num) + "&page_size=20";
	//cout << url << endl;
	txt = r.Http_Get(url, false, "1");
	return 0;
}