#pragma once

#define MIRAICPP_STATICLIB
#include <mirai.h>
using namespace Cyan;

//��ѭ����ѯʱ��
void TimeLoop(int ReoladTime, int Search_time, bool proxy, string proxy_rule, string proxy_add, MiraiBot& bot, int64_t master);
//��ͼ���غ���
bool Search_switch(bool status, int64_t member, int64_t group);
//��ͼ״̬���
void Search_status(int Search_time, int64_t new_time, MiraiBot& bot);
//��ͼ״̬��ѯ
bool Search_check(int64_t member, int64_t group);
//�汾���
void CheckVersion(string mahVersion, string mcppVersion, string myVersion);