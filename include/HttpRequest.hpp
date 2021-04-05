#pragma once
#include <iostream>
#include <cpr/cpr.h>

#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib,"urlmon.lib")

using namespace cpr;

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
				r = Get(Url{ url }, Proxies{ {"https", https} }, Timeout{ 10000 });
			}
			else
			{
				r = Get(Url{ url }, Timeout{ 10000 });
			}
			if (r.status_code != 200)
			{
				std::cout << r.error.message << std::endl;
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			std::cout << err.what() << std::endl << r.error.message << std::endl;
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
				r = Post(Url{ url }, Body{ body }, Proxies{ {"https", https} }, Timeout{ 10000 });
			}
			else
			{
				r = Post(Url{ url }, Body{ body }, Timeout{ 10000 });
			}
			if (r.status_code != 200)
			{
				std::cout << r.error.message << std::endl;
			}
			return r.text;
		}
		catch (const std::exception& err)
		{
			std::cout << err.what() << std::endl << r.error.message << std::endl;
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