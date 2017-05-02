#include "WinHttp.h"


CWinHttp::CWinHttp(LPCWSTR pwszVerb) {
    hSession = NULL;
    hConnect = NULL;
    hRequest = NULL;
    m_lenofcookie = 0;
    m_lenofallheaders = 0;
    m_strAllHeaders = NULL;
    lstrcpyW(this->m_strVerb, pwszVerb);
    m_cookies = NULL;
}

CWinHttp::~CWinHttp() {
    // Close any open handles.
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);


    if (m_cookies) {
        delete[] m_cookies;
        m_cookies = 0;
    }
    if (m_strAllHeaders) {
        delete[] m_strAllHeaders;
        m_strAllHeaders = 0;
    }
}

bool CWinHttp::AddRequestHeader(_In_  LPCWSTR pwszHeaders,
                                _In_  DWORD   dwHeadersLength,
                                _In_   DWORD   dwModifiers
                               ) {
    bool bRet = false;
    int n;
    if (!WinHttpAddRequestHeaders(hRequest, pwszHeaders, dwHeadersLength, dwModifiers)) {
        n = GetLastError();
        goto exit0;
    }
    bRet = true;
exit0:
    return bRet;
}
bool CWinHttp::SendRequest(PVOID pBuffer, int LenOfData) {
    bool bRet = false,
         bResults = false;


    //int n=GetLastError();
    if (!hRequest)
        goto exit0;
    // Send a request.
    if (lstrcmpW(m_strVerb, L"GET") == 0) {
        bResults = WinHttpSendRequest(
                       hRequest,
                       WINHTTP_NO_ADDITIONAL_HEADERS,
                       0,
                       WINHTTP_NO_REQUEST_DATA,
                       0,
                       0,
                       0);
    } else {
        if (LenOfData >= 1024)
            goto exit0;
        bResults = WinHttpSendRequest(
                       hRequest,
                       WINHTTP_NO_ADDITIONAL_HEADERS,
                       0,
                       WINHTTP_NO_REQUEST_DATA,
                       0,
                       LenOfData,
                       0);
        PostDataToServer(pBuffer, LenOfData);

    }
    if (!bResults)
        goto exit0;

    // End the request.
    bResults = WinHttpReceiveResponse(hRequest, NULL);
    if (!bResults)
        goto exit0;

    bRet = true;
exit0:
    assert(bRet != false);
    return bRet;
}
bool CWinHttp::Open(LPCWSTR url) {
    bool  bRet = false;
    bool  bResults = false;

    if (!CrackURL(url))
        goto exit0;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(
                   L"BbLiFe",
                   WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                   WINHTTP_NO_PROXY_NAME,
                   WINHTTP_NO_PROXY_BYPASS,
                   0
               );
    if (!hSession)
        goto exit0;


    // Specify an HTTP server.
    hConnect = WinHttpConnect(
                   hSession,
                   m_strHostName,
                   m_Port,
                   0
               );
    if (!hConnect)
        goto exit0;


    // Create an HTTP request handle.
    hRequest = WinHttpOpenRequest(
                   hConnect,
                   m_strVerb,
                   m_stUrlPath,
                   NULL, WINHTTP_NO_REFERER,
                   WINHTTP_DEFAULT_ACCEPT_TYPES,
                   WINHTTP_FLAG_ESCAPE_DISABLE
               );

    bRet = true;
exit0:
    assert(bRet != false);
    return  bRet;
}



bool CWinHttp::CrackURL(LPCWCH url) {

    bool bRet = false;

    URL_COMPONENTS urlComp;

    DWORD dwUrlLen = 0;

    // Initialize the URL_COMPONENTS structure.
    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.lpszHostName = m_strHostName;
    urlComp.lpszUrlPath = m_stUrlPath;

    // Set required component lengths to non-zero so that they are cracked.

    urlComp.dwHostNameLength  = 256;
    urlComp.dwUrlPathLength   = 256;


    // Crack the URL.
    if (!WinHttpCrackUrl(url,
                         NULL,
                         ICU_ESCAPE,
                         &urlComp)
       ) {
        //int n=GetLastError();
        goto exit0;
    }

    else {

        m_dwServiceType = urlComp.nScheme;
        m_Port = urlComp.nPort;
    }
    bRet = true;
exit0:
    assert(bRet != false);
    return bRet;
}
void CWinHttp::SetPort(int port) {
    m_Port = port;
}
bool CWinHttp::GetResponseConext() {
    bool bRet = false;
    DWORD dwSize = 0 ;
    LPSTR pszOutBuffer;
    if (hRequest) {
        do {
            // Check for available data.
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                goto exit0;
            if (dwSize == 0)
                break;
            else {
                // Allocate space for the buffer.
                pszOutBuffer = new  char[dwSize + 1];

                // Read the data.
                ZeroMemory(pszOutBuffer, dwSize + 1);

                if (!WinHttpReadData(
                            hRequest,
                            (LPVOID)pszOutBuffer,
                            dwSize,
                            0)
                   )
                    goto exit0;

                else {
                    m_strResponseContent = m_strResponseContent + pszOutBuffer;
                }

                // Free the memory allocated to the buffer.
                delete [] pszOutBuffer;
            }
        } while (dwSize > 0);
    }
    bRet = true;
exit0:
    assert(bRet != false);
    return bRet;
}
bool CWinHttp::GetAllHeadersLength() {
    bool bRet = false;

    WinHttpQueryHeaders(
        hRequest,
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        WINHTTP_NO_OUTPUT_BUFFER,
        &m_lenofallheaders,
        WINHTTP_NO_HEADER_INDEX);

    bRet = true;
exit0:
    return bRet;
}
bool CWinHttp::GetAllHeaders() {
    bool bRet = false;

    GetAllHeadersLength();
    m_strAllHeaders = new WCHAR[m_lenofallheaders + 1];
    if (!WinHttpQueryHeaders(hRequest,
                             WINHTTP_QUERY_RAW_HEADERS_CRLF,
                             WINHTTP_HEADER_NAME_BY_INDEX,
                             m_strAllHeaders,
                             &m_lenofallheaders,
                             WINHTTP_NO_HEADER_INDEX))
        goto exit0;



    bRet = true;
exit0:
    return bRet;
}

bool CWinHttp::GetCookiesLength() {
    bool bRet = false;
    DWORD dwRead = 1024 * 5;
    int n;
    WinHttpQueryHeaders(hRequest,
                        WINHTTP_QUERY_COOKIE ,
                        WINHTTP_HEADER_NAME_BY_INDEX,
                        WINHTTP_NO_OUTPUT_BUFFER,
                        &m_lenofcookie,
                        WINHTTP_NO_HEADER_INDEX);
    bRet = true;
exit0:
    return bRet;
}

bool CWinHttp::GetCookies() {

    bool bRet = false;
    GetCookiesLength();
    m_cookies = new char[m_lenofcookie + 1];
    int n;
    if (!WinHttpQueryHeaders(
                hRequest,
                WINHTTP_QUERY_RAW_HEADERS_CRLF,
                WINHTTP_HEADER_NAME_BY_INDEX,
                m_cookies,
                &m_lenofcookie,
                WINHTTP_NO_HEADER_INDEX)) {

        n = GetLastError();
        goto exit0;
    }


    bRet = true;
exit0:
    return bRet;
}
bool CWinHttp::PostDataToServer(PVOID pBuffer, int len) {
    bool bRet = false;
    DWORD dwBytesWritten = 0;
    int index = 0;
    while (1) {
        if (!WinHttpWriteData(
                    hRequest,
                    pBuffer,
                    index,
                    &dwBytesWritten)
           )
            goto exit0;
        if (index == len)
            break;
        pBuffer = (PVOID)((DWORD)pBuffer + dwBytesWritten);
        index += dwBytesWritten;
    }
    bRet = true;
exit0:
    assert(bRet != false);
    return bRet;
}

void ConvertUtf8ToGBK(string& str) {
    string strtemp;

    int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)str.c_str(), -1, NULL, 0);
    WCHAR* wszGBK = new WCHAR[len];
    memset(wszGBK, 0, len * 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)str.c_str(), -1, wszGBK, len);


    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len];
    memset(szGBK, 0, len);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);

    str = szGBK;
    delete[] szGBK;
    delete[] wszGBK;
}

void ConvertGBKToUtf8(string& strGBK) {
    int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK.c_str(), -1, NULL, 0);
    WCHAR* wszUtf8 = new WCHAR[len];
    memset(wszUtf8, 0, len * 2);
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK.c_str(), -1, wszUtf8, len);

    len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL);
    char* szUtf8 = new char[len];
    memset(szUtf8, 0, len);
    WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL, NULL);
    strGBK = szUtf8;
    delete[] szUtf8;
    delete[] wszUtf8;
}