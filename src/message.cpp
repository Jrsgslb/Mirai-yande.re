#include "../include/message.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <io.h>

#include <direct.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <../include/HttpRequest.hpp>

using namespace boost::property_tree;

vector<string> command;
//创建文件函数
bool CreateFile(string file)
{
	fstream fs;
	fs.open(file.c_str(), ios::in);
	if (!fs)
	{
		ofstream fout(file.c_str());
		if (fout) fout.close();
		return true;
	}
	else
	{
		fs.close();
		return false;
	}
}
//启动文件及文件夹检查
void StartCheck()
{
	//检测并创建文件夹
	if (_access("./temp", 0) == -1)_mkdir("./temp");
	if (_access("./config", 0) == -1)_mkdir("./config");
	if (_access("./config/bili", 0) == -1)_mkdir("./config/bili");
	if (_access("./config/data", 0) == -1)_mkdir("./config/data");
	//文件不存在自动创建
	CreateFile("./config/data/member.ini"); //个人数据记录文件
	CreateFile("./config/data/group.ini"); //群组数据记录文件
	CreateFile("./temp/num.ini"); //调用统计次数文件
	CreateFile("./config/Group_whitelist.txt"); //群白名单文件
	CreateFile("./config/whitelist.txt"); //个人白名单文件
	CreateFile("./config/bili/live.json"); //直播订阅文件
	CreateFile("./config/rule.ini"); //自定义发图配置文件
	CreateFile("./config/bili/dynamic.json");//动态订阅文件
	if (CreateFile("./mah.json")) //mirai api http 连接文件
	{
		WriteFile("./mah.json", "{\n		\"hostname\": \"127.0.0.1\",\n		\"port\" : 8081,\n		\"botQQ\" : 100001,\n		\"verifyKey\" : \"Hello\",\n		\"enableVerify\" : true,\n		\"singleMode\" : false,\n		\"reservedSyncId\" : \"-1\",\n		\"threadPoolSize\" : 4\n}");
	}
	if (CreateFile("./config.json")) //程序配置文件
	{
		HttpRequest r;
		WriteFile("./config.json", r.Http_Get("https://raw.githubusercontent.com/Jrsgslb/Mirai-yande.re/main/doc/config,json"));
	}
	//检查指令文件有无空行
	ifstream in("./config/command.txt");
	string line, str;
	bool first = true;
	while (getline(in, line))
	{
		if (line == "")
		{
			continue;
		}
		if (first)
		{
			first = false;
			str = str + line;
			continue;
		}
		str = str + "\n" + line;
	}

	WriteFile("./config/command.txt", str);
	in.close();
	// 指令读取
	CommandReload();
}

void CommandReload()
{
	ifstream in("./config/command.txt");
	string line;
	while (getline(in, line))
	{
		command.push_back(line);
	}
	in.close();
	return;
}

bool MessageCheck(string plain)
{
	for (int i = 0; i < command.size(); i++)
	{
		if (plain == command[i])
		{
			return true;
		}
	}
	return false;
}

bool MessageLimit(string plain, int64_t qq_num, int64_t group_num, bool admin)
{
	ifstream in1("./config/Group_whitelist.txt"), in2("./config/whitelist.txt");
	string line1, line2;
	//群白名单验证
	if (in1) while (getline(in1, line1)) if (!line1.empty() && stoi(line1) == group_num) return true;
	//个人白名单验证
	if (in2) while (getline(in2, line2))if (!line2.empty() && stoi(line2) == qq_num) return true;

	ptree p, ini;
	bool on = false;
	ini_parser::read_ini("./config/rule.ini", p);
	basic_ptree<string, string> tag = p.get_child("");
	//没写入文件的频率限制调用检查
	for (auto i = tag.begin(); i != tag.end(); i++)
	{
		if ((*i).first.data() == plain)
		{
			on = true;
			break;
		}
	}
	if (!on)
	{
		return true;
	}

	ini = p.get_child(plain);

	int mod1, mod2, time_set;
	mod1 = ini.get<int>("mode1", 10);
	mod2 = ini.get<int>("mode2", 10);
	time_set = ini.get<int>("time", 10);
	string str = ini.get<string>("member", "0"), temp;

	//正则分割member
	regex reg("[1-9][0-9]{4,14}");
	smatch res;
	//迭代器声明
	string::const_iterator ben = str.begin(); 
	string::const_iterator end = str.end();
	bool member_ini = false;
	while (regex_search(ben, end, res, reg))
	{
		temp = res[0];
		if (_atoi64(temp.c_str()) == qq_num)
		{
			member_ini = true;
			break;
		}
		member_ini = false;
		ben = res[0].second;
	}

	//频率限制开始
	switch (mod1)
	{
		default: return true;
		case(0):
		{
			int time_old;
			ptree group_pt, group_time;
			string test = to_string(group_num) + ".time";
			ini_parser::read_ini("./config/data/group.ini", group_pt);
			//空键名处理
			try
			{
				group_time = group_pt.get_child(to_string(group_num));
				time_old = group_time.get<int>("time");
			}
			catch (const std::exception& err)
			{
				group_pt.put<string>(test.c_str(), "0");
				ini_parser::write_ini("./config/data/group.ini", group_pt);
				group_time = group_pt.get_child(to_string(group_num));
				time_old = group_time.get<int>("time");
			}
			time_t time_new = time(NULL);
			switch (mod2)
			{
			default: return true;
				//以群为单位，全部群生效
			case(0):
				if (time_new - time_old > time_set)
				{
					group_pt.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/group.ini", group_pt);
					return true;
				}
				else
					return false;
				//以群为单位，指定群生效，其余群不生效
			case(1):
				if (member_ini)
				{
					if (time_new - time_old > time_set)
					{
						group_pt.put<int>(test.c_str(), time_new);
						ini_parser::write_ini("./config/data/group.ini", group_pt);
						return true;
					}
					else
						return false;
				}
				else
					return true;
				//以群为单位，指定群生效，其余群屏蔽
			case(2):
				if (member_ini)
				{
					if (time_new - time_old > time_set)
					{
						group_pt.put<int>(test.c_str(), time_new);
						ini_parser::write_ini("./config/data/group.ini", group_pt);
						return true;
					}
					else
						return false;
				}
				else
					return false;
				//以群为单位，指定群不生效，其余群生效
			case(3):
				if (member_ini) return true;
				if (time_new - time_old > time_set)
				{
					group_pt.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/group.ini", group_pt);
					return true;
				}
				else
					return false;
				//以群为单位，指定群屏蔽，其余群生效
			case(4):
				if (member_ini) return false;
				if (time_new - time_old > time_set)
				{
					group_pt.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/group.ini", group_pt);
					return true;
				}
				else
					return false;
				//以群为单位，管理放行，指定群屏蔽
			case(5):
				if (member_ini) return false;
				if (admin) return true;
				if (time_new - time_old > time_set)
				{
					group_pt.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/group.ini", group_pt);
					return true;
				}
				else
					return false;
				//以群为单位，管理生效，指定群和群员屏蔽
			case(6):
				if (member_ini || !admin) return false;
				if (time_new - time_old > time_set)
				{
					group_pt.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/group.ini", group_pt);
					return true;
				}
				else
					return false;
			}
		}
		case(1):
		{
			ptree member, n_time;
			string test = to_string(qq_num) + ".time";
			ini_parser::read_ini("./config/data/member.ini", member);
			//空键名处理
			try
			{
				n_time = member.get_child(to_string(qq_num));
			}
			catch (const std::exception&)
			{
				member.put<string>(test.c_str(), "0");
				ini_parser::write_ini("./config/data/member.ini", member);
				n_time = member.get_child(to_string(qq_num));
			}
			int time_old = n_time.get<int>("time");
			time_t time_new = time(NULL);
			switch (mod2)
			{
			default: return true;
				//以个人为单位，全部人生效
			case(0):
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//以个人为单位，指定的人生效
			case(1):
				if (member_ini)
				{
					if (time_new - time_old > time_set)
					{
						member.put<int>(test.c_str(), time_new);
						ini_parser::write_ini("./config/data/member.ini", member);
						return true;
					}
					else
						return false;
				}
				else
					return true;
				//以个人为单位，指定的人生效，其余人屏蔽
			case(2):
				if (member_ini)
				{
					if (time_new - time_old > time_set)
					{
						member.put<int>(test.c_str(), time_new);
						ini_parser::write_ini("./config/data/member.ini", member);
						return true;
					}
					else
						return false;
				}
				else
					return false;
				//以个人为单位，指定的人放行，其余的人生效
			case(3):
				if (member_ini) return true;
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//以个人为单位，指定的人屏蔽，其余的人生效
			case(4):
				if (member_ini) return false;
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//以个人为单位，群管理员放行，指定的人屏蔽，其余人生效
			case(5):
				if (member_ini) return false;
				if (admin) return true;
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//以个人为单位，群管理员生效，指定的人和其余的人屏蔽
			case(6):
				if (member_ini || !admin) return false;
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
			}
		}
		case(2):
		{
			ptree member, n_time;
			string test = to_string(qq_num) + ".time";
			ini_parser::read_ini("./config/data/member.ini", member);
			//空键名处理
			try
			{
				n_time = member.get_child(to_string(qq_num));
			}
			catch (const std::exception&)
			{
				member.put<string>(test.c_str(), "0");
				ini_parser::write_ini("./config/data/member.ini", member);
				n_time = member.get_child(to_string(qq_num));
			}
			int time_old = n_time.get<int>("time");
			time_t time_new = time(NULL);
			switch (mod2)
			{
			default: return true;
				//以个人为单位，全部人生效
			case(0):
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//以个人为单位，指定的群生效
			case(1):
				if (member_ini)
				{
					if (time_new - time_old > time_set)
					{
						member.put<int>(test.c_str(), time_new);
						ini_parser::write_ini("./config/data/member.ini", member);
						return true;
					}
					else
						return false;
				}
				else
					return true;
				//以个人为单位，指定的群生效，其余人屏蔽
			case(2):
				if (member_ini)
				{
					if (time_new - time_old > time_set)
					{
						member.put<int>(test.c_str(), time_new);
						ini_parser::write_ini("./config/data/member.ini", member);
						return true;
					}
					else
						return false;
				}
				else
					return false;
				//以个人为单位，指定的群放行，其余的人生效
			case(3):
				if (member_ini) return true;
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//以个人为单位，指定的群屏蔽，其余的人生效
			case(4):
				if (member_ini) return false;
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//以个人为单位，群管理员放行，指定的群屏蔽，其余人生效
			case(5):
				if (member_ini) return false;
				if (admin) return true;
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//以个人为单位，群管理员生效，指定的群和其余的人屏蔽
			case(6):
				if (member_ini || !admin) return false;
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
			}
		}
	}
	//频率限制结束
}

bool MessageR18(int64_t qq_num, int64_t group_num, bool R18)
{
	string temp;
	ptree r18;

	ini_parser::read_ini("./config/data/group.ini", r18);

	if (R18)
	{
		temp = to_string(group_num) + ".R18";
		if (r18.get<bool>(temp.c_str(), false))
		{
			return false;
		}
		r18.put<bool>(temp.c_str(), true);
		ini_parser::write_ini("./config/data/group.ini", r18);
		return true;
	}
	else
	{
		temp = to_string(group_num) + ".R18";
		if (!r18.get<bool>(temp.c_str(), false))
		{
			return false;
		}
		r18.put<bool>(temp.c_str(), false);
		ini_parser::write_ini("./config/data/group.ini", r18);
		return true;
	}
}

string ReloadFile(const char* file)
{
	ifstream in(file);
	string line, res;
	if (!in)
	{
		printf("打开文件失败，请检查文件是否存在\n");
		return "";
	}
	while (getline(in, line))
	{
		res = res + line;
	}
	in.close();
	return res;
}

bool WriteFile(const char* file, string txt)
{
	ofstream out(file);
	if (out.is_open())
	{
		out << txt;
		out.close();
		return true;
	}
	else
	{
		return false;
	}
}