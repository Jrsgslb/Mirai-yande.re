#pragma once
#include <string>
#include <rapidjson/document.h>
using namespace std;
using namespace rapidjson;

//�������
void StartCheck();
//yվָ��ˢ�¼����ļ���ȡ����
void CommandReload();
//yվ�Զ���ָ����
bool MessageCheck(string plain);
//Ƶ������
bool MessageLimit(string plain, int64_t qq_num, int64_t group_num, bool admin);
//r18�趨
bool MessageR18(int64_t qq_num, int64_t group_num, bool R18);
//��ȡ�ļ�
string ReloadFile(const char* file);
//д���ļ�
bool WriteFile(const char* file, string txt);