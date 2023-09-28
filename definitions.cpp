#include <windows.h>
#include <WinInet.h>
#include <iostream>
#include <regex>
using namespace std;
string fetch(string url){
    bool secure = url[4] == 's';
    string result;
    HINTERNET internet = InternetOpen("WinInet Test", INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);
    if(internet){
        int pos = url.find('/') + 2, pos2 = url.find('/', pos);
        if(pos2 == -1) pos2 = url.length();
        string::iterator it = url.begin() + pos2;
        HINTERNET connect = InternetConnect(internet, string(url.begin() + pos, it).c_str(), secure ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0, 0, INTERNET_SERVICE_HTTP, 0, 1);
        if(connect){
            HINTERNET request = HttpOpenRequest(connect, "GET", string(it, url.end()).c_str(), 0, 0, 0, INTERNET_FLAG_KEEP_CONNECTION | (secure * INTERNET_FLAG_SECURE), 1);
            if(request){
                bool s = HttpSendRequest(request, 0, 0, 0, 0);
                char data[10000];
                while(s){
                    DWORD bytes;
                    bool read = InternetReadFile(request, data, sizeof(data) - 1, &bytes);
                    if (!read || !bytes) break;
                    data[bytes] = 0;
                    result += data;
                }
                InternetCloseHandle(request);
            }
            InternetCloseHandle(connect);
        }
       InternetCloseHandle(internet);
    }
    return result;
}
int main(int argc, char *argv[]){
    string url = "https://vocabulary.com/dictionary/", html, def;
    regex r("<div class=\"definition\">");
    for(int i = 1; i < argc; ++i){
        html = fetch(url + argv[i]);
        cout << argv[i] << ":\n";
        for(sregex_iterator j(html.begin(), html.end(), r); j != sregex_iterator(); ++j){
            int i = smatch(*j).position() + 24;
            int index = html.find("v>", i) + 1;
            if(string(html.begin() + i, html.begin() + index).find("<d") != -1){
                def = string(html.begin() + index + 1, html.begin() + html.find('<', index));
                def.erase(0, def.find_first_not_of(" \n\r\t\f\v"));
                cout << def << '\n';
                continue;
            }
            cout << string(html.begin() + i, html.begin() + index - 5) << '\n';
        }
        cout << '\n';
    }
    return 0;
}