
#include <Windows.h>
#include <Winhttp.h>
#include <string>
#include <assert.h>
#include <vector>

using std::string;
void ConvertUtf8ToGBK(string& str);

class CWinHttp {
public:
    CWinHttp(LPCWSTR pwszVerb = L"GET");
    ~CWinHttp();
    bool Open(LPCWSTR url);


    bool GetResponseConext();

    bool SendRequest(PVOID pBuffer = 0, int = 0);
    bool PostDataToServer(PVOID pBuffer = 0, int = 0);

    bool AddRequestHeader(_In_  LPCWSTR pwszHeaders,
                          _In_  DWORD dwHeadersLength,
                          _In_  DWORD dwModifiers = WINHTTP_ADDREQ_FLAG_ADD_IF_NEW
                         );

    bool GetAllHeaders();

    bool GetCookies();

    bool GetCookiesLength();

    bool GetAllHeadersLength();

    void SetPort(int port);

    //cookies
    LPSTR m_cookies;
    DWORD  m_lenofcookie;

    LPWSTR m_strAllHeaders;
    DWORD  m_lenofallheaders;

    LPSTR m_strResponseConext;

    //Post or Get
    WCHAR m_strVerb[5];

    //response content
    std::string m_strResponseContent;
private:

    bool CrackURL(LPCWCH);
    HINTERNET  hSession ,
               hConnect ,
               hRequest ;

    DWORD m_dwServiceType;
    WCHAR m_strHostName[256], m_stUrlPath[256];
    INTERNET_PORT m_Port;
};