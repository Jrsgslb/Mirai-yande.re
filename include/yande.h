#pragma once
#include <string>
#include <rapidjson/document.h>
using namespace std;
using namespace rapidjson;

//yվ�Զ���ָ�ͼ
Document yande(string plain, bool proxy, string proxy_rule, string proxy_add, int64_t group_num, bool first, bool original);
//yվ��ȡָ��idͼƬ
Document yid(string id, bool proxy, string proxy_rule, string proxy_add, int64_t group_num);
//��ȡ13λʱ���
long long GetUnixTime();
//����temp�ļ���
bool ClearTemp();
//����yվtag
string MessageReload(bool proxy, string proxy_rule, string proxy_add);
//�������а�ͼƬ
Document Hot_Img(bool proxy, string proxy_rule, string proxy_add, int64_t group, bool orginal);