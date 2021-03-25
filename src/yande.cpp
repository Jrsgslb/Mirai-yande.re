#include "../include/yande.h"

vector<string> yande(string plain, bool proxy, string https, bool file_url, int64_t group_num)
{
	int num;
	vector<string> id_info;
	//读取tag信息
	ptree p, num_time;
	ini_parser::read_ini("./config/rule.ini", p);
	basic_ptree<string, string> tag = p.get_child(plain);
	string tags = tag.get<string>("tag");
	//老图片过滤机制
	time_t timep;
	struct tm* t;
	time(&timep);
	t = localtime(&timep);
	ini_parser::read_ini("./temp/num.ini", num_time);
	plain = plain + "." + to_string(t->tm_year + 1900) + "/" + to_string(t->tm_mon +1) + "/" + to_string(t->tm_mday);
	int num_num = num_time.get<int>(plain.c_str(), 0) + 1;
	num_time.put<int>(plain.c_str(), num_num);
	ini_parser::write_ini("./temp/num.ini", num_time);

	if (tag.get<int>("num") >= 1000)
	{
		if (num_num > 500 && tag.get<int>("num") > 2000)
		{
			num = 1000;
		}
		else
		{
			num = 500;
		}
	}
	else
		num = tag.get<int>("num");
	//取随机数
	default_random_engine e;
	uniform_int_distribution<unsigned> u(1, num);
	e.seed(GetUnixTime());
	string page = to_string(u(e));

	Response r;
	//5 秒超时
	if (proxy)
		r = Get(Url{ "https://yande.re/post.json" }, Parameters{ {"page", page.c_str()}, {"tags", tags.c_str()}, {"limit","1"} }, Proxies{ {"https", https} }, Timeout{5000});
	else
		r = Get(Url{ "https://yande.re/post.json" }, Parameters{ {"page", page.c_str()}, {"tags", tags.c_str()}, {"limit","1"} }, Timeout{5000});
	if (r.status_code != 200)
	{
		id_info.push_back("网络错误或tag填写错误");
		return id_info;
	}
	//读取json
	Document y;
	y.Parse(r.text.c_str());
	string  url, file,rating,r18_temp;
	int id;
	id = Pointer("/0/id").Get(y)->GetInt();
	if (file_url)
	{
		url = Pointer("/0/file_url").Get(y)->GetString();
		file = "./temp/" + to_string(id) + "." + Pointer("/0/file_ext").Get(y)->GetString();
	}
	else
	{
		url = Pointer("/0/sample_url").Get(y)->GetString();
		file = "./temp/" + to_string(id) + ".jpg";
	}
	rating = Pointer("/0/rating").Get(y)->GetString();
	//R18限制
	ptree r18;
	ini_parser::read_ini("./config/data/group.ini", r18);
	r18_temp = to_string(group_num) + ".R18";
	if (rating < tag.get<string>("rating") && !r18.get<bool>(r18_temp.c_str(), false))
	{
		id_info.push_back("这张图片不适合在本群观看哦");
		return id_info;
	}
	//push数据
	id_info.push_back(to_string(id));
	id_info.push_back(url);
	id_info.push_back(file);
	id_info.push_back(rating);
	return id_info;
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

bool DownloadImg(string url, string file, bool proxy, string https)
{
	//代理设置
	if (proxy)
	{
		string http;
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
}

bool ClearTemp()
{
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst("./temp/*.*", &findData);    // 查找目录中的第一个文件
	if (handle == -1) return false;
	string file;
	do
	{
		if (strcmp(findData.name, ".") && strcmp(findData.name, ".."))
		{
			file = findData.name;
			file = "./temp/" + file;
			cout << "Del:" << findData.name << endl;
			remove(file.c_str());
		}
	} while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件
	_findclose(handle);    // 关闭搜索句柄
	return true;
}