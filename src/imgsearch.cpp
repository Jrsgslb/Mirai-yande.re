#include "../include/imgsearch.h"
#include "../include/HttpRequest.hpp"

#include <iostream>
#include <regex>

#include <rapidjson/pointer.h>


Document a2d_search(bool proxy, string https, string url)
{
	//��ȡɫ����������Ϣ
	try
	{
		Document p;
		string host, file, color, bovw;
		host = "https://ascii2d.net/search/url/" + url;
		
		HttpRequest r;
		color = r.Http_Get(host, proxy, https);
		//����ƥ��ͼƬhash��ɫ���������
		regex search_hash("class='hash'>(.*?)<");
		regex search_res("<a target=\"_blank\" rel=\"noopener\" href=\"(.* ?)\">(.*?)</a>");
		smatch hash, res, pic;
		string::const_iterator iterStart = color.begin(), iterEnd = color.end();
		regex_search(color, hash, search_hash);
		//��������
		host = "https://ascii2d.net/search/bovw/" + hash.str(1);
		bovw = r.Http_Get(host, proxy, https);
		//�ж�״̬��
		if (!color.empty() && !bovw.empty())
		{
			Pointer("/code").Set(p, 1);
		}
		else
		{
			Pointer("/code").Set(p, 0);
			Pointer("/info").Set(p, "���������������̨");
			return p;
		}
		//ȡ��ɫ�����
		vector<string> res_url, res_author;
		while (regex_search(iterStart, iterEnd, res, search_res))
		{
			res_url.push_back(res.str(1));
			res_author.push_back(res.str(2));
			iterStart = res[0].second; //����������ʼλ�ã�����ʣ�µ��ַ���
		}
		//д����json
		Pointer("/color/pic/url").Set(p, res_url[0].c_str());
		Pointer("/color/pic/name").Set(p, res_author[0].c_str());
		Pointer("/color/user/url").Set(p, res_url[1].c_str());
		Pointer("/color/user/name").Set(p, res_author[1].c_str());
		//����ƥ��ɫ��������ͼƬurl
		regex search_pic("<img loading=\"lazy\" src=\"(.*?)\"");
		//ɫ��pic
		regex_search(color, pic, search_pic);
		file = pic.str(1);
		file = "./temp/" + file.substr(20, 50);
		Pointer("/color/name").Set(p, file.c_str());
		host = "https://ascii2d.net" + pic.str(1);
		Pointer("/color/url").Set(p, host.c_str());
		//����pic
		regex_search(bovw, pic, search_pic);
		file = pic.str(1);
		file = "./temp/" + file.substr(20, 50);
		Pointer("/bovw/name").Set(p, file.c_str());
		host = "https://ascii2d.net" + pic.str(1);
		Pointer("/bovw/url").Set(p, host.c_str());
		//ȡ���������
		iterStart = bovw.begin(), iterEnd = bovw.end();
		res_url.clear();
		res_author.clear();
		while (regex_search(iterStart, iterEnd, res, search_res))
		{
			res_url.push_back(res.str(1));
			res_author.push_back(res.str(2));
			iterStart = res[0].second; //����������ʼλ�ã�����ʣ�µ��ַ���
		}
		//д����json
		Pointer("/bovw/pic/url").Set(p, res_url[0].c_str());
		Pointer("/bovw/pic/name").Set(p, res_author[0].c_str());
		Pointer("/bovw/user/url").Set(p, res_url[1].c_str());
		Pointer("/bovw/user/name").Set(p, res_author[1].c_str());
		//����ͼƬ
		if (r.DownloadImg(Pointer("/color/url").Get(p)->GetString(), Pointer("/color/name").Get(p)->GetString(), proxy, https) && r.DownloadImg(Pointer("/bovw/url").Get(p)->GetString(), Pointer("/bovw/name").Get(p)->GetString(), proxy, https))
		{
			Pointer("/dimg").Set(p, 1);
		}
		else
		{
			Pointer("/dimg").Set(p, 0);
		}
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
	string txt;
	HttpRequest r;
	//����ͼƬ
	url = "url=" + url;
	txt = r.Http_Post("https://saucenao.com/search.php", proxy, https, url);

	if (txt.empty())
	{
		Pointer("/code").Set(d, 0);
		Pointer("/info").Set(d, "���������������̨");
		return d;
	}
	else
	{
		Pointer("/code").Set(d, 1);
	}
	//�����������
	try
	{
		regex match_regex("<div class=\"resultsimilarityinfo\">(.*?)%</div>");
		regex search_regex("<table class=\"resulttable\">(.*?)</table>");
		regex img_regex("src=\"([http|https](.*?))\"");
		regex info_regex("<div class=\"resultcontentcolumn\">(.*?)</div>");
		regex title_regex("<strong>(.*?)</strong>");
		regex id_regex("<a href=\"(.*?)\" class=\"linkify\">(.*?)</a>");
		smatch match_res, id_res, img_res, title_res, search_res, info_res;
		string res, info, name;
		regex_search(txt, search_res, search_regex);//ƥ���һ���������
		res = search_res.str(1);
		regex_search(res, match_res, match_regex);//����ƥ���
		regex_search(res, img_res, img_regex);//����img url
		regex_search(res, info_res, info_regex);//������ʦ��id��Ϣ
		regex_search(res, title_res, title_regex);//����������Ϣ
		info = info_res[0];
		string::const_iterator strStart = info.begin(), strEnd = info.end();
		//�ֱ�������ʦid��Ϣ
		if (regex_search(strStart, strEnd, id_res, id_regex))
		{
			Pointer("/id").Set(d, id_res.str(1).c_str());
			strStart = id_res[0].second;
		}
		else
		{
			Pointer("/id").Set(d, "NULL");
		}
		if (regex_search(strStart, strEnd, id_res, id_regex))
		{
			Pointer("/member").Set(d, id_res.str(1).c_str());
		}
		else
		{
			Pointer("/member").Set(d, "NULL");
		}
		
		//��Ϣд��json
		srand((unsigned)time(NULL));
		name = "./temp/" + to_string(rand());
		name = name + ".jpg";
		Pointer("/name").Set(d, name.c_str());
		Pointer("/match").Set(d, match_res.str(1).c_str());
		Pointer("/url").Set(d, img_res.str(1).c_str());
		Pointer("/title").Set(d, title_res.str(1).c_str());
		//����ͼƬ
		if (r.DownloadImg(img_res.str(1), name, proxy, https))
		{
			Pointer("/dimg").Set(d, 1);
		}
		else
		{
			Pointer("/dimg").Set(d, 0);
		}
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