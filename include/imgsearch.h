#pragma once
#include <string>
#include <rapidjson/document.h>
using namespace std;
using namespace rapidjson;

Document a2d_search(bool proxy, string https, string url);
Document snao_search(bool proxy, string https, string url);