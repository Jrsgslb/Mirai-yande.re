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

    handle = _findfirst("./temp/*.*", &findData);    // ����Ŀ¼�еĵ�һ���ļ�
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
    } while (_findnext(handle, &findData) == 0);    // ����Ŀ¼�е���һ���ļ�
    _findclose(handle);    // �ر��������
    return true;
}