#include "../include/imgsearch.h"

Document a2d_search(bool proxy, string https, string url)
{
	//获取色调与特征信息
	try
	{
		Document p;
		string host, file;
		host = "https://ascii2d.net/search/url/" + url;
		Response r, s;
		if (proxy)
		{
			r = Get(Url{ host }, Proxies{ {"https", https} }, Timeout{ 5000 });
		}
		else
		{
			r = Get(Url{ host }, Timeout{ 5000 });
		}
		//正则匹配图片hash和色调搜索结果
		regex search_hash("class='hash'>(.*?)<");
		regex search_res("<a target=\"_blank\" rel=\"noopener\" href=\"(.* ?)\">(.*?)<\/a>");
		smatch hash, res, pic;
		string::const_iterator iterStart = r.text.begin(), iterEnd = r.text.end();
		regex_search(r.text, hash, search_hash);
		//特征搜索
		host = "https://ascii2d.net/search/bovw/" + hash.str(1);
		if (proxy)
		{
			s = Get(Url{ host }, Proxies{ {"https", https} }, Timeout{ 10000 });
		}
		else
		{
			s = Get(Url{ host }, Timeout{ 10000 });
		}
		//判断状态码
		if (r.status_code == 200 && s.status_code == 200)
		{
			Pointer("/code").Set(p, 1);
		}
		else
		{
			string err_info;
			err_info = s.error.message+ "\n" + r.error.message;
			Pointer("/code").Set(p, 0);
			Pointer("/info").Set(p, err_info.c_str());
			cout << s.status_code << endl << r.status_code << endl;
			cout << s.error.message << r.error.message;
			return p;
		}
		//取出色调结果
		vector<string> res_url, res_author;
		while (regex_search(iterStart, iterEnd, res, search_res))
		{
			res_url.push_back(res.str(1));
			res_author.push_back(res.str(2));
			iterStart = res[0].second; //更新搜索起始位置，搜索剩下的字符串
		}
		//写入结果json
		Pointer("/color/pic/url").Set(p, res_url[0].c_str());
		Pointer("/color/pic/name").Set(p, res_author[0].c_str());
		Pointer("/color/user/url").Set(p, res_url[1].c_str());
		Pointer("/color/user/name").Set(p, res_author[1].c_str());
		//正则匹配色调与特征图片url
		regex search_pic("<img loading=\"lazy\" src=\"(.*?)\"");
		//色调pic
		regex_search(r.text, pic, search_pic);
		file = pic.str(1);
		file = "./temp/" + file.substr(20,50);
		Pointer("/color/name").Set(p, file.c_str());
		host = "https://ascii2d.net" + pic.str(1);
		Pointer("/color/url").Set(p, host.c_str());
		//特征pic
		regex_search(s.text, pic, search_pic);
		file = pic.str(1);
		file = "./temp/" + file.substr(20, 50);
		Pointer("/bovw/name").Set(p, file.c_str());
		host = "https://ascii2d.net" + pic.str(1);
		Pointer("/bovw/url").Set(p, host.c_str());
		//取出特征结果
		iterStart = s.text.begin(), iterEnd = s.text.end();
		res_url.clear();
		res_author.clear();
		while (regex_search(iterStart, iterEnd, res, search_res))
		{
			res_url.push_back(res.str(1));
			res_author.push_back(res.str(2));
			iterStart = res[0].second; //更新搜索起始位置，搜索剩下的字符串
		}
		//写入结果json
		Pointer("/bovw/pic/url").Set(p, res_url[0].c_str());
		Pointer("/bovw/pic/name").Set(p, res_author[0].c_str());
		Pointer("/bovw/user/url").Set(p, res_url[1].c_str());
		Pointer("/bovw/user/name").Set(p, res_author[1].c_str());
		
		return p;
	}
	catch (const std::exception& err)
	{
		Document p;
		Pointer("/code").Set(p, 0);
		Pointer("/info").Set(p, err.what());
		cout << err.what() << endl;
		return p;
	}
}

Document snao_search(bool proxy, string https, string url)
{
	Document d;
	Response s;
	//搜索图片
	url = "url=" + url;
	if (proxy)
	{
		s = Post(Url{ "https://saucenao.com/search.php" }, Body{ url }, Proxies{ {"https", https} }, Timeout{ 5000 });
	}
	else
	{
		s = Post(Url{ "https://saucenao.com/search.php" }, Body{ url }, Timeout{ 5000 });
	}
	if (s.status_code != 200)
	{
		cout << s.error.message << endl;
		Pointer("/code").Set(d, 0);
		Pointer("/info").Set(d, s.error.message.c_str());
		return d;
	}
	else
	{
		Pointer("/code").Set(d, 1);
	}
	//解析搜索结果
	try
	{
		regex match_regex("<div class=\"resultsimilarityinfo\">(.*?)%<\/div>");
		regex search_regex("<table class=\"resulttable\">(.*?)<\/table>");
		regex img_regex("src=\"(.*?)\"");
		regex info_regex("<div class=\"resultcontentcolumn\">(.*?)<\/div>");
		regex title_regex("<strong>(.*?)<\/strong>");
		regex id_regex("<a href=\"(.*?)\" class=\"linkify\">(.*?)<\/a>");
		smatch match_res, id_res, img_res, title_res, search_res, info_res;
		string res, info, name;
		regex_search(s.text, search_res, search_regex);//匹配第一个搜索结果
		res = search_res.str(1);
		regex_search(res, match_res, match_regex);//搜索匹配度
		regex_search(res, img_res, img_regex);//搜索img url
		regex_search(res, info_res, info_regex);//搜索画师和id信息
		regex_search(res, title_res, title_regex);//搜索标题信息
		info = info_res[0];
		string::const_iterator strStart = info.begin(), strEnd = info.end();
		regex_search(strStart, strEnd, id_res, id_regex);//分别搜索画师id信息
		Pointer("/id").Set(d, id_res.str(1).c_str());
		strStart = id_res[0].second;
		regex_search(strStart, strEnd, id_res, id_regex);
		Pointer("/member").Set(d, id_res.str(1).c_str());
		//信息写入json
		srand((unsigned)time(NULL));
		name = "./temp/" + to_string(rand());
		name = name + ".jpg";
		Pointer("/name").Set(d, name.c_str());
		Pointer("/match").Set(d, match_res.str(1).c_str());
		Pointer("/url").Set(d, img_res.str(1).c_str());
		Pointer("/title").Set(d, title_res.str(1).c_str());
		return d;
	}
	catch (const std::exception& err)
	{
		cout << err.what() << endl;
		Pointer("/code").Set(d, 0);
		Pointer("/info").Set(d, err.what());
		return d;
	}
}