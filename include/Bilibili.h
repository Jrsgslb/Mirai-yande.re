#pragma once

#define MIRAICPP_STATICLIB
#include "mirai.h"

using namespace Cyan;

//ˢ��ֱ������uid
void Reload_live_uid();
//ֱ������
void Bilibili_live(MiraiBot& bot);
//����cos
bool Bilibili_cos(MiraiBot& bot, GID_t gid);
//ˢ�¶�̬����uid
void Reload_dynamic_uid();
//����bվ��̬����
MessageChain Bilibili_parse_dynamic(int type, string json);
//��̬����
void Bilibili_dynamic(MiraiBot& bot);
//bվ����Ⱥ��ƥ��
void Bilibili_match(MiraiBot& bot, string type, string mode, string uid, int64_t gid_64, int64_t qq_64, int64_t master, int64_t msid);
//bվ����˽��ƥ��
void Bilibili_match(MiraiBot& bot, string type, string mode, string uid, int64_t qq_64, int64_t master);