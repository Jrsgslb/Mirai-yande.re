#pragma once
#include "message.h"

#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/ini_parser.hpp> 

#include <cpr/cpr.h>

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <regex>
#include <time.h>

using namespace std;
using namespace cpr;
using namespace boost::property_tree;


bool MessageCheck(string plain)
{
    SetConsoleOutputCP(65001);
    ifstream in("./config/指令.txt");
    string line;
	while (getline(in, line))
	{
		if (line == plain)
		{
			return 1;
		}
	}
	return 0;
}

string MessageReload(bool proxy, string http, string https)
{
	ptree pt;
	ini_parser::read_ini("./config/rule.ini", pt);

	basic_ptree<string, string> tag = pt.get_child("");

	string err, comm;
	//遍历rule.ini
	for (auto i = tag.begin(); i != tag.end(); i++)
	{
		Response r;
		smatch res;
		regex reg("<posts count=\"([0-9]*)\" offset=\"([0-9]*)\"");
		basic_ptree<string, string> temp = pt.get_child((*i).first.data());
		string tags = temp.get<string>("tag"), txt, num;
		num = (*i).first.data();
		//获取网页并检验状态
		if (proxy)
			r = Get(Url{ "https://yande.re/post.xml" }, Parameters{ {"tags", tags.c_str()} }, Proxies{ {"https", https} });
		else
			r = Get(Url{ "https://yande.re/post.xml" }, Parameters{ {"tags", tags.c_str()} });
		if (r.status_code != 200)err = tags + "\n" + err;
		else
		{
			//正则取出次数
			auto pos = r.text.cbegin();
			auto end = r.text.cend();
			for (; regex_search(pos, end, res, reg); pos = res.suffix().first) txt = res.str(1);
			//写入文件
			num = num + ".num";
			pt.put<string>(num.c_str(), txt.c_str());
			ini_parser::write_ini("./config/rule.ini", pt);
			if (i == tag.begin()) comm = (*i).first.data();
			else
			comm = comm + "\n" + (*i).first.data();
			_sleep(1 * 1000);
		}
	}

	if (!err.empty())return err;
	else
	{
		FILE* tem = fopen("./config/指令.txt", "w");
		fprintf(tem, "%s", comm.c_str());
		fclose(tem);
		return "ok";
	}
}

bool MessageLimit(string plain, int qq_num, int group_num, bool admin)
{
	SetConsoleOutputCP(65001);
	ifstream in1("./config/群白名单.txt"), in2("./config/白名单.txt");
	string line1, line2;
	//群白名单验证
	if (in1) while (getline(in1, line1)) if (!line1.empty() && stoi(line1) == group_num) return true;
	//个人白名单验证
	if (in2) while (getline(in2, line2))if (!line2.empty() && stoi(line2) == qq_num) return true;

	ptree p;
	ini_parser::read_ini("./config/rule.ini", p);
	auto ini = p.get_child(plain);
	int mod1, mod2, time_set;
	mod1 = ini.get<int>("mode1");
	mod2 = ini.get<int>("mode2");
	time_set = ini.get<int>("time");
	fstream fs1, fs2;
	fs1.open("./config/data/member.ini", ios::in);
	fs2.open("./config/data/group.ini", ios::in);
	if (!fs1)
	{
		ofstream fout("./config/data/member.ini");
		if (fout)fout.close();
	}
	if (!fs2)
	{
		ofstream fout("./config/data/group.ini");
		if (fout)fout.close();
	}
	/*
	残留问题
	member分割问题
	*/
	//频率限制开始
	switch (mod1)
	{
		default: return true;
		case(0):
		{
			ptree group_pt;
			ini_parser::read_ini("./config/data/group.ini", group_pt);
			string test = to_string(group_num) + ".test";
			group_pt.put<string>(test.c_str(), "0");
			ini_parser::write_ini("./config/data/member.ini", group_pt);
			test = to_string(group_num) + ".time";
			auto group_time = group_pt.get_child(to_string(group_num));
			int time_old = group_time.get<int>(test.c_str(), 0);
			time_t time_new = time(NULL);
			int num = ini.get<int>("member", 0);
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
				if (num == group_num)
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
				if (num == group_num)
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
				if (num == group_num) return true;
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
				if (num == group_num) return false;
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
				if (num == group_num) return false;
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
				if (num == group_num || !admin) return false;
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
			ptree member;
			ini_parser::read_ini("./config/data/member.ini", member);
			string test = to_string(qq_num) + ".test";
			member.put<string>(test.c_str(), "0");
			ini_parser::write_ini("./config/data/member.ini", member);
			test = to_string(qq_num) + ".time";
			auto n_time = member.get_child(to_string(qq_num));
			int time_old = n_time.get<int>(test.c_str(), 0);
			time_t time_new = time(NULL);
			int num = ini.get<int>("member", 0);
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
				if (qq_num == num)
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
				if (qq_num == num)
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
				if (qq_num == num) return true;
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
				if (qq_num == num) return false;
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
				if (qq_num == num) return false;
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
				if (qq_num == num || !admin) return false;
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
			ptree member;
			ini_parser::read_ini("./config/data/member.ini", member);
			string test = to_string(qq_num) + ".test";
			member.put<string>(test.c_str(), "0");
			ini_parser::write_ini("./config/data/member.ini", member);
			test = to_string(qq_num) + ".time";
			auto n_time = member.get_child(to_string(qq_num));
			int time_old = n_time.get<int>(test.c_str(), 0);
			time_t time_new = time(NULL);
			int num = ini.get<int>("member", 0);
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
				if (group_num == num)
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
				if (group_num == num)
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
				if (group_num == num) return true;
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
				if (group_num == num) return false;
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
				if (group_num == num) return false;
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
				if (group_num == num || !admin) return false;
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