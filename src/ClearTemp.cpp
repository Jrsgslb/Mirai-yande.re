#include "ClearTemp.h"
#include <iostream>
#include <cstdio>
#include <direct.h>
#include <io.h>

using namespace std;

bool ClearTemp()
{
    intptr_t handle;
    _finddata_t findData;

    handle = _findfirst("./temp/*.*", &findData);    // 查找目录中的第一个文件
    if (handle == -1) return false;
    string file;
    do
    {
        if (strcmp(findData.name, ".") && strcmp(findData.name, ".."))
        {
            file = findData.name;
            file = "./temp/" + file;
            cout << "Del:" << findData.name << endl;
            remove(file.c_str());
        }
    } while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件
    _findclose(handle);    // 关闭搜索句柄
    return true;
}