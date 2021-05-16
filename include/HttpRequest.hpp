#pragma once
#include <iostream>
#include <cpr/cpr.h>

#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib,"urlmon.lib")

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
	//Get请求
	std::string Http_Get(std::string url, bool proxy, std::string https)
	{
		Response r;
		try
		{
			if (proxy)
			{
				r = Get(Url{ url }, Proxies{ {"https", https} }, Timeout{ 10000 }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			else
			{
				r = Get(Url{ url }, Timeout{ 10000 }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			if (r.status_code != 200)
			{
				printf("%s \n", r.error.message.c_str());
				return "";
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			printf("%s \n %s \n", err.what(), r.error.message.c_str());
			return "";
		}
	};
	//Get请求（Bilibili）
	std::string Http_Get_Bili(std::string url, bool proxy, std::string https)
	{
		Response r;
		try
		{
			if (proxy)
			{
				r = Get(Url{ url }, Proxies{ {"https", https} }, Timeout{ 10000 }, Cookies{ {"uid", "123"} }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			else
			{
				r = Get(Url{ url }, Timeout{ 10000 }, Cookies{ {"uid", "123"} }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			
			if (r.status_code != 200)
			{
				printf("%s \n", r.error.message.c_str());
				return "";
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			printf("%s \n %s \n", err.what(), r.error.message.c_str());
			return "";
		}
	};
	//Get-pixiv砖用
	std::string Http_Get_Pixiv(std::string url, bool proxy, std::string https)
	{
		Response r;
		try
		{
			if (proxy)
			{
				r = Get(Url{ url }, Proxies{ {"https", https} }, Timeout{ 10000 }, Header{ {"Referer", "https://www.pixiv.net/"}, {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			else
			{
				r = Get(Url{ url }, Timeout{ 10000 }, Header{ {"Referer", "https://www.pixiv.net/"}, { "user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56" } });
			}
			if (r.status_code != 200)
			{
				printf("%s \n", r.error.message.c_str());
				return "";
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			printf("%s \n %s \n", err.what(), r.error.message.c_str());
			return "";
		}
	};
	//Post请求
	std::string Http_Post(std::string url, bool proxy, std::string https, std::string body)
	{
		Response r;
		try
		{
			if (proxy)
			{
				r = Post(Url{ url }, Body{ body }, Proxies{ {"https", https} }, Timeout{ 10000 }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			else
			{
				r = Post(Url{ url }, Body{ body }, Timeout{ 10000 }, Header{ {"user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Safari/537.36 Edg/90.0.818.56"} });
			}
			if (r.status_code != 200)
			{
				printf("%s \n", r.error.message.c_str());
				return "";
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			printf("%s \n %s \n", err.what(), r.error.message.c_str());
			return "";
		}
	};
	//下载图片到本地
	bool DownloadImg(std::string url, std::string file, bool proxy, std::string https)
	{
		//代理设置
		if (proxy)
		{
			std::string http;
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
	};

};