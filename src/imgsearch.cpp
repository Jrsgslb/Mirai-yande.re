#include "../include/imgsearch.h"

Document a2d_search(bool proxy, string https, string url)
{
	//��ȡɫ����������Ϣ
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
		//����ƥ��ͼƬhash��ɫ���������
		regex search_hash("class='hash'>(.*?)<");
		regex search_res("<a target=\"_blank\" rel=\"noopener\" href=\"(.* ?)\">(.*?)<\/a>");
		smatch hash, res, pic;
		string::const_iterator iterStart = r.text.begin(), iterEnd = r.text.end();
		regex_search(r.text, hash, search_hash);
		//��������
		host = "https://ascii2d.net/search/bovw/" + hash.str(1);
		if (proxy)
		{
			s = Get(Url{ host }, Proxies{ {"https", https} }, Timeout{ 5000 });
		}
		else
		{
			s = Get(Url{ host }, Timeout{ 5000 });
		}
		//�ж�״̬��
		if (r.status_code == 200 && s.status_code == 200)
		{
			Pointer("/code").Set(p, 0);
		}
		else
		{
			Pointer("/code").Set(p, 1);
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
		regex_search(r.text, pic, search_pic);
		file = pic.str(1);
		file = "./temp/" + file.substr(20,50);
		Pointer("/color/name").Set(p, file.c_str());
		host = "https://ascii2d.net" + pic.str(1);
		Pointer("/color/url").Set(p, host.c_str());
		//����pic
		regex_search(s.text, pic, search_pic);
		file = pic.str(1);
		file = "./temp/" + file.substr(20, 50);
		Pointer("/bovw/name").Set(p, file.c_str());
		host = "https://ascii2d.net" + pic.str(1);
		Pointer("/bovw/url").Set(p, host.c_str());
		//ȡ���������
		iterStart = s.text.begin(), iterEnd = s.text.end();
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
		
		return p;
	}
	catch (const std::exception& err)
	{
		Document p;
		Pointer("/code").Set(p, 1);
		Pointer("/info").Set(p, err.what());
		cout << err.what() << endl;
		return p;
	}
}