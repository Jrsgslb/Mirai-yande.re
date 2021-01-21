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
    ifstream in("./config/ָ��.txt");
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
	//����rule.ini
	for (auto i = tag.begin(); i != tag.end(); i++)
	{
		Response r;
		smatch res;
		regex reg("<posts count=\"([0-9]*)\" offset=\"([0-9]*)\"");
		basic_ptree<string, string> temp = pt.get_child((*i).first.data());
		string tags = temp.get<string>("tag"), txt, num;
		num = (*i).first.data();
		//��ȡ��ҳ������״̬
		if (proxy)
			r = Get(Url{ "https://yande.re/post.xml" }, Parameters{ {"tags", tags.c_str()} }, Proxies{ {"https", https} });
		else
			r = Get(Url{ "https://yande.re/post.xml" }, Parameters{ {"tags", tags.c_str()} });
		if (r.status_code != 200)err = tags + "\n" + err;
		else
		{
			//����ȡ������
			auto pos = r.text.cbegin();
			auto end = r.text.cend();
			for (; regex_search(pos, end, res, reg); pos = res.suffix().first) txt = res.str(1);
			//д���ļ�
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
		FILE* tem = fopen("./config/ָ��.txt", "w");
		fprintf(tem, "%s", comm.c_str());
		fclose(tem);
		return "ok";
	}
}

bool MessageLimit(string plain, int qq_num, int group_num, bool admin)
{
	SetConsoleOutputCP(65001);
	ifstream in1("./config/Ⱥ������.txt"), in2("./config/������.txt");
	string line1, line2;
	//Ⱥ��������֤
	if (in1) while (getline(in1, line1)) if (!line1.empty() && stoi(line1) == group_num) return true;
	//���˰�������֤
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
	��������
	member�ָ�����
	*/
	//Ƶ�����ƿ�ʼ
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
				//��ȺΪ��λ��ȫ��Ⱥ��Ч
			case(0):
				if (time_new - time_old > time_set)
				{
					group_pt.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/group.ini", group_pt);
					return true;
				}
				else
					return false;
				//��ȺΪ��λ��ָ��Ⱥ��Ч������Ⱥ����Ч
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
				//��ȺΪ��λ��ָ��Ⱥ��Ч������Ⱥ����
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
				//��ȺΪ��λ��ָ��Ⱥ����Ч������Ⱥ��Ч
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
				//��ȺΪ��λ��ָ��Ⱥ���Σ�����Ⱥ��Ч
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
				//��ȺΪ��λ��������У�ָ��Ⱥ����
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
				//��ȺΪ��λ��������Ч��ָ��Ⱥ��ȺԱ����
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
				//�Ը���Ϊ��λ��ȫ������Ч
			case(0):
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//�Ը���Ϊ��λ��ָ��������Ч
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
				//�Ը���Ϊ��λ��ָ��������Ч������������
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
				//�Ը���Ϊ��λ��ָ�����˷��У����������Ч
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
				//�Ը���Ϊ��λ��ָ���������Σ����������Ч
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
				//�Ը���Ϊ��λ��Ⱥ����Ա���У�ָ���������Σ���������Ч
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
				//�Ը���Ϊ��λ��Ⱥ����Ա��Ч��ָ�����˺������������
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
				//�Ը���Ϊ��λ��ȫ������Ч
			case(0):
				if (time_new - time_old > time_set)
				{
					member.put<int>(test.c_str(), time_new);
					ini_parser::write_ini("./config/data/member.ini", member);
					return true;
				}
				else
					return false;
				//�Ը���Ϊ��λ��ָ����Ⱥ��Ч
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
				//�Ը���Ϊ��λ��ָ����Ⱥ��Ч������������
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
				//�Ը���Ϊ��λ��ָ����Ⱥ���У����������Ч
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
				//�Ը���Ϊ��λ��ָ����Ⱥ���Σ����������Ч
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
				//�Ը���Ϊ��λ��Ⱥ����Ա���У�ָ����Ⱥ���Σ���������Ч
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
				//�Ը���Ϊ��λ��Ⱥ����Ա��Ч��ָ����Ⱥ�������������
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
	//Ƶ�����ƽ���
}