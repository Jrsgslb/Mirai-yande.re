#include "../include/include.h"

#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib,"urlmon.lib")

vector<string> yande(string plain, bool proxy, string https, bool file_url, int64_t group_num);
bool DownloadImg(string url, string file, bool proxy, string https);
static __int64 GetUnixTime();
bool ClearTemp();