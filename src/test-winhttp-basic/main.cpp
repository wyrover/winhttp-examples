#include <iostream>
#include "WinHttp.h"

using namespace std;

int main()
{


	// 默认 Get 方式
	CWinHttp WinHttp;

	WCHAR *url = L"http://www.csdn.com/";

	// 打开连接
	if (!WinHttp.Open(url))
		return 0;
	// 添加请求头选项
	LPWSTR host = L"Host:www.csdn.com";
	if (!WinHttp.AddRequestHeader(host, lstrlenW(host)))
		return 0;
	// 发送请求
	if (!WinHttp.SendRequest("hello", 5))
		return 0;
	// 得到返回内容
	if (!WinHttp.GetResponseConext())
		return 0;
	// 转码
	ConvertUtf8ToGBK(WinHttp.m_strResponseContent);
	cout << WinHttp.m_strResponseContent << endl;

	// 得到请求头
	WinHttp.GetAllHeaders();
	wcout << WinHttp.m_strAllHeaders << endl;
	getchar();
}