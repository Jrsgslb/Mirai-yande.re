#pragma once
#include <iostream>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

using namespace std;
using namespace rapidjson;

string JsonGetString(const char* Json, char* Path);
int JsonGetInt(const char* Json, char* Path);
string JsonReaderString(char* Path, char* point);
int JsonReaderInt(char* Path, char* point);