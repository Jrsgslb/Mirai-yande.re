#include "../include/message.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <io.h>

#include <direct.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp> 

#include <rapidjson/pointer.h>
#include <rapidjson/filereadstream.h>

using namespace boost::property_tree;

bool StartCheck()
{
	//��Ⲣ�����ļ���
	if (_access("./temp", 0) == -1)_mkdir("./temp");
	if (_access("./config", 0) == -1)_mkdir("./config");
	if (_access("./config/data", 0) == -1)_mkdir("./config/data");
	//���dll
	if (access("./libcurl.dll", 0) == -1)
	{
		if (system("curl http://101.37.245.179:8000/down/k3b5es2COdMe --output libcurl.dll")!=0)
		{
			return false;
		}
	}
	if (access("./zlib1.dll", 0) == -1)
	{
		if (system("curl http://101.37.245.179:8000/down/LI5W5ABlEfKi --output zlib1.dll") != 0)
		{
			return false;
		}
	}
	//�ļ��������Զ�����
	fstream fs1, fs2, fs3;
	fs1.open("./config/data/member.ini", ios::in);
	fs2.open("./config/data/group.ini", ios::in);
	fs3.open("./temp/num.ini", ios::in);
	if (!fs1)
	{
		ofstream fout("./config/data/member.ini");
		if (fout) fout.close();
	}
	else
		fs1.close();
	if (!fs2)
	{
		ofstream fout("./config/data/group.ini");
		if (fout) fout.close();
	}
	else
		fs2.close();
	if (!fs3)
	{
		ofstream fout("./temp/num.ini");
		if (fout) fout.close();
	}
	else
		fs3.close();
	return true;
}

bool MessageCheck(string plain)
{
    ifstream in("./config/command.txt");
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

bool MessageLimit(string plain, int64_t qq_num, int64_t group_num, bool admin)
{
	ifstream in1("./config/Ⱥ������.txt"), in2("./config/������.txt");
	string line1, line2;
	//Ⱥ��������֤
	if (in1) while (getline(in1, line1)) if (!line1.empty() && stoi(line1) == group_num) return true;
	//���˰�������֤
	if (in2) while (getline(in2, line2))if (!line2.empty() && stoi(line2) == qq_num) return true;

	ptree p, ini;
	ini_parser::read_ini("./config/rule.ini", p);
	ini = p.get_child(plain);
	int mod1, mod2, time_set;
	mod1 = ini.get<int>("mode1", 10);
	mod2 = ini.get<int>("mode2", 10);
	time_set = ini.get<int>("time", 10);
	string str = ini.get<string>("member", "0"), temp;

	//����ָ�member
	regex reg("[1-9][0-9]{4,14}");
	smatch res;
	//����������
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

	/*
	�������⣺

	������
	����ͼƬָ���Ƶ������ʵ��
	������

	*/

	//Ƶ�����ƿ�ʼ
	switch (mod1)
	{
		default: return true;
		case(0):
		{
			ptree group_pt, group_time;
			string test = to_string(group_num) + ".time";
			ini_parser::read_ini("./config/data/group.ini", group_pt);
			//�ռ�������
			try
			{
				group_time = group_pt.get_child(to_string(group_num));
			}
			catch (const std::exception&)
			{
				group_pt.put<string>(test.c_str(), "0");
				ini_parser::write_ini("./config/data/group.ini", group_pt);
				group_time = group_pt.get_child(to_string(group_num));
			}
			int time_old = group_time.get<int>("time");
			time_t time_new = time(NULL);
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
				//��ȺΪ��λ��ָ��Ⱥ��Ч������Ⱥ����
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
				//��ȺΪ��λ��ָ��Ⱥ����Ч������Ⱥ��Ч
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
				//��ȺΪ��λ��ָ��Ⱥ���Σ�����Ⱥ��Ч
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
				//��ȺΪ��λ��������У�ָ��Ⱥ����
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
				//��ȺΪ��λ��������Ч��ָ��Ⱥ��ȺԱ����
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
			//�ռ�������
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
				//�Ը���Ϊ��λ��ָ��������Ч������������
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
				//�Ը���Ϊ��λ��ָ�����˷��У����������Ч
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
				//�Ը���Ϊ��λ��ָ���������Σ����������Ч
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
				//�Ը���Ϊ��λ��Ⱥ����Ա���У�ָ���������Σ���������Ч
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
				//�Ը���Ϊ��λ��Ⱥ����Ա��Ч��ָ�����˺������������
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
			//�ռ�������
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
				//�Ը���Ϊ��λ��ָ����Ⱥ��Ч������������
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
				//�Ը���Ϊ��λ��ָ����Ⱥ���У����������Ч
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
				//�Ը���Ϊ��λ��ָ����Ⱥ���Σ����������Ч
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
				//�Ը���Ϊ��λ��Ⱥ����Ա���У�ָ����Ⱥ���Σ���������Ч
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
				//�Ը���Ϊ��λ��Ⱥ����Ա��Ч��ָ����Ⱥ�������������
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
	//Ƶ�����ƽ���
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

Document ReloadConfig()
{
	FILE* fp = fopen("./config.json", "rb");

	if (fp == NULL)
	{
		cout << "Error reading configuration file, please check whether the configuration file exists.\n";
		system("pause");
		return 0;
	}

	char readBuffer[10000];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;
	d.ParseStream(is);

	fclose(fp);
	return d;
}