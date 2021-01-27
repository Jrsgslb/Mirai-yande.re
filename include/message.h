#pragma once
#include <iostream>
using namespace std;

bool MessageCheck(string plain);
string MessageReload(bool proxy, string http, string https);
bool MessageLimit(string plain, int64_t qq_num, int64_t group_num, bool admin);