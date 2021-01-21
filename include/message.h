#pragma once
#include <iostream>
using namespace std;

bool MessageCheck(string plain);
string MessageReload(bool proxy, string http, string https);
bool MessageLimit(string plain, int qq_num, int group_num, bool admin);