#include "../include/include.h"

Document yande(string plain, bool proxy, string https, int64_t group_num, bool first);
bool DownloadImg(string url, string file, bool proxy, string https);
static __int64 GetUnixTime();
bool ClearTemp();
string MessageReload(bool proxy, string https);