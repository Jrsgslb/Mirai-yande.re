#pragma once
#include "mirai/mirai.h"
#include <rapidjson/document.h>

using namespace rapidjson;
using namespace Cyan;

Document Bilibili_live(MiraiBot bot);
Document Bilibili_cos();