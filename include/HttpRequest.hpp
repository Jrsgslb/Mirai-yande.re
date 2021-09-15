#pragma once
#include <iostream>
#include <cpr/cpr.h>

using namespace cpr;
//网络请求
class HttpRequest
{
public:
	static std::string url;
	static bool proxy;
	static std::string https;
	static std::string body;
	static std::string file;
	static std::string proxy_rule;
	static std::string proxy_add;
	//Get请求
	std::string Http_Get(std::string url, bool proxy, std::string proxy_rule, std::string proxy_add)
	{
		Response r;
		try
		{
			if (proxy)
			{
				r = Get(Url{ url }, Proxies{ {proxy_rule, proxy_add} }, Timeout{ 10000 }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			else
			{
				r = Get(Url{ url }, Timeout{ 10000 }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			if (r.status_code != 200)
			{
				printf("%s \n", r.error.message.c_str());
				return std::string("");
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			printf("%s \n %s \n", err.what(), r.error.message.c_str());
			return std::string("");
		}
	};//Get请求
	std::string Http_Get(std::string url)
	{
		Response r;
		try
		{
			r = Get(Url{ url }, Timeout{ 10000 }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });

			if (r.status_code != 200)
			{
				printf("%s \n", r.error.message.c_str());
				return std::string("");
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			printf("%s \n %s \n", err.what(), r.error.message.c_str());
			return std::string("");
		}
	};
	//Get请求（Bilibili）
	std::string Http_Get_Bili(std::string url)
	{
		Response r;
		try
		{
			r = Get(Url{ url }, Timeout{ 10000 }, Cookies{ {"uid", "123"} }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			
			if (r.status_code != 200)
			{
				printf("%s \n", r.error.message.c_str());
				return std::string("");
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			printf("%s \n %s \n", err.what(), r.error.message.c_str());
			return std::string("");
		}
	};
	//Get-pixiv砖用
	std::string Http_Get_Pixiv(std::string url, bool proxy, std::string proxy_rule, std::string proxy_add)
	{
		Response r;
		try
		{
			if (proxy)
			{
				r = Get(Url{ url }, Proxies{ {proxy_rule, proxy_add} }, Timeout{ 10000 }, Header{ {"Referer", "https://www.pixiv.net/"}, {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			else
			{
				r = Get(Url{ url }, Timeout{ 10000 }, Header{ {"Referer", "https://www.pixiv.net/"}, { "user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56" } });
			}
			if (r.status_code != 200)
			{
				printf("%s \n", r.error.message.c_str());
				return std::string("");
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			printf("%s \n %s \n", err.what(), r.error.message.c_str());
			return std::string("");
		}
	};
	//Post请求
	std::string Http_Post(std::string url, bool proxy, std::string proxy_rule, std::string proxy_add, std::string body)
	{
		Response r;
		try
		{
			if (proxy)
			{
				r = Post(Url{ url }, Body{ body }, Proxies{ {proxy_rule, proxy_add} }, Timeout{ 10000 }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			else
			{
				r = Post(Url{ url }, Body{ body }, Timeout{ 10000 }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			if (r.status_code != 200)
			{
				printf("%s \n", r.error.message.c_str());
				return std::string("");
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			printf("%s \n %s \n", err.what(), r.error.message.c_str());
			return std::string("");
		}
	};
	//下载图片到本地
	bool DownloadImg(std::string url, std::string file, bool proxy, std::string proxy_rule, std::string proxy_add)
	{
		// 代理设置
		if (proxy)
		{
			string curl, proxyy;
			curl = " -S -s -o " + file + " \"" + url + "\"";
			if (proxy_rule == "https")
			{
				proxyy = "curl -x http://" + proxy_add + curl;
			}
			else
			{
				proxyy = "curl -x " + proxy_rule + "://" + proxy_add + curl;
			}
			system(proxyy.c_str());
		}
		else
		{
			string curl;
			curl = "curl -S -s -o " + file + " \"" + url + "\"";
			system(curl.c_str());
		}
		return true;
	};

};