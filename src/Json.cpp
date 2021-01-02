#include <iostream>
#include <string>
#include <map>
#include <cstdio>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>

#include "Json.h"

using namespace std;
using namespace rapidjson;

string JsonReaderString(char* Path, char* point)
{
#ifdef _WIN32
    FILE* Filep = fopen(Path, "rb");
    if (!Filep)
    {
        Filep = fopen(Path, "wb");
    }
#else
    FILE* Filep = fopen(Path, "r");
    if (!Filep)
    {
        Filep = fopen(Path, "w");
    }
#endif // _WIN32

    char readBuffer[65536];
    FileReadStream read(Filep, readBuffer, sizeof(readBuffer));

    Document d;
    d.ParseStream(read);

    Value* v = Pointer(point).Get(d);
    if (v == nullptr)
    {
        return "empty";
    }
    else
    {
        return v->GetString();
    }
    fclose(Filep);
}

int JsonReaderInt(char* Path, char* point)
{
#ifdef _WIN32
    FILE* Filep = fopen(Path, "rb");
    if (!Filep)
    {
        Filep = fopen(Path, "wb");
    }
#else
    FILE* Filep = fopen(Path, "r");
    if (!Filep)
    {
        Filep = fopen(Path, "w");
    }
#endif // _WIN32

    char readBuffer[65536];
    FileReadStream read(Filep, readBuffer, sizeof(readBuffer));

    Document d;
    d.ParseStream(read);

    Value* v = Pointer(point).Get(d);
    if (v == nullptr)
    {
        return -1;
    }
    else
    {
        return v->GetInt();
    }
    fclose(Filep);
}

// json数据处理，传出string
string JsonGetString(const char* Json, char* Path)
{
    StringStream JsonStream(Json); // 创建输入流，给RapidJson传递json原始数据
    Document JsonDOM; // 创建RapidJson DOM
    JsonDOM.ParseStream(JsonStream); // 分析json原始数据并传入DOM
    Value* JsonValue = Pointer(Path).Get(JsonDOM); // 获取对应路径的值，并返回
    if (JsonValue == nullptr)
    {
        return "empty";
    }
    else
    {
        return JsonValue->GetString();
    }

}

// json数据处理，传出int
int JsonGetInt(const char* Json, char* Path)
{
    StringStream JsonStream(Json); // 创建输入流，给RapidJson传递json原始数据
    Document JsonDOM; // 创建RapidJson DOM
    JsonDOM.ParseStream(JsonStream); // 分析json原始数据并传入DOM
    Value* JsonValue = Pointer(Path).Get(JsonDOM); // 获取对应路径的值，并返回
    if (JsonValue == nullptr)
    {
        return -1;
    }
    else
    {
        return JsonValue->GetInt();
    }

}
/*int port, qq;
string key;
port = JsonReaderInt("./config.json", "/port");
qq = JsonReaderInt("./config.json", "/qq");
key = JsonReaderString("./config.json", "/key");
QQ_t qqNum_qq = QQ_t(qq);
cout << port << endl << qq << endl << key << endl;
*/
/*#ifdef _WIN32
    FILE* Filep = fopen(Path, "rb");
    if (!Filep)
    {
        Filep = fopen(Path, "wb");
    }
#else
    FILE* Filep = fopen(Path, "r");
    if (!Filep)
    {
        Filep = fopen(Path, "w");
    }
#endif // _WIN32

    fclose(Filep);*/



    /*
                        Response r = Get(Url{ "https://yande.re/post.json?page=2&tags=loli%20rating:q&limit=1" }, Proxies{ {"https", "127.0.0.1:1080"} });
                        stringstream ss(r.text);
                        ptree pt;
                        // 读取JSON数据
                        read_json(ss, pt);
                        write_json("./tmp.json", pt);

                        Json::Reader reader;// 解析json用Json::Reader
                        Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array

                        ifstream is;
                        is.open("./tmp.json", ios::binary);
                        if (!reader.parse(is, root)) {
                        }
                        else {
                            std::cout << root[""]["file_url"].asString() << endl;
                        }
                        is.close();
                        */