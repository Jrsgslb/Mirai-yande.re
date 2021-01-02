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

// json���ݴ�������string
string JsonGetString(const char* Json, char* Path)
{
    StringStream JsonStream(Json); // ��������������RapidJson����jsonԭʼ����
    Document JsonDOM; // ����RapidJson DOM
    JsonDOM.ParseStream(JsonStream); // ����jsonԭʼ���ݲ�����DOM
    Value* JsonValue = Pointer(Path).Get(JsonDOM); // ��ȡ��Ӧ·����ֵ��������
    if (JsonValue == nullptr)
    {
        return "empty";
    }
    else
    {
        return JsonValue->GetString();
    }

}

// json���ݴ�������int
int JsonGetInt(const char* Json, char* Path)
{
    StringStream JsonStream(Json); // ��������������RapidJson����jsonԭʼ����
    Document JsonDOM; // ����RapidJson DOM
    JsonDOM.ParseStream(JsonStream); // ����jsonԭʼ���ݲ�����DOM
    Value* JsonValue = Pointer(Path).Get(JsonDOM); // ��ȡ��Ӧ·����ֵ��������
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
                        // ��ȡJSON����
                        read_json(ss, pt);
                        write_json("./tmp.json", pt);

                        Json::Reader reader;// ����json��Json::Reader
                        Json::Value root; // Json::Value��һ�ֺ���Ҫ�����ͣ����Դ����������͡���int, string, object, array

                        ifstream is;
                        is.open("./tmp.json", ios::binary);
                        if (!reader.parse(is, root)) {
                        }
                        else {
                            std::cout << root[""]["file_url"].asString() << endl;
                        }
                        is.close();
                        */