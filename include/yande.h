#pragma once
#include <string>
#include <rapidjson/document.h>
using namespace std;
using namespace rapidjson;

//yվ�Զ���ָ�ͼ
Document yande(string plain, bool proxy, string https, int64_t group_num, bool first, bool original);
//yվ��ȡָ��idͼƬ
Document yid(string id, bool proxy, string https, int64_t group_num);
//��ȡ13λʱ���
static __int64 GetUnixTime();
//����temp�ļ���
bool ClearTemp();
//����yվtag
string MessageReload(bool proxy, string https);
//�������а�ͼƬ
Document Hot_Img(bool proxy, string https, int64_t group, bool orginal);