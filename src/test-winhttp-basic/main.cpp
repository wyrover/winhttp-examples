#include <iostream>
#include "WinHttp.h"

using namespace std;

int main()
{


	// Ĭ�� Get ��ʽ
	CWinHttp WinHttp;

	WCHAR *url = L"http://www.csdn.com/";

	// ������
	if (!WinHttp.Open(url))
		return 0;
	// �������ͷѡ��
	LPWSTR host = L"Host:www.csdn.com";
	if (!WinHttp.AddRequestHeader(host, lstrlenW(host)))
		return 0;
	// ��������
	if (!WinHttp.SendRequest("hello", 5))
		return 0;
	// �õ���������
	if (!WinHttp.GetResponseConext())
		return 0;
	// ת��
	ConvertUtf8ToGBK(WinHttp.m_strResponseContent);
	cout << WinHttp.m_strResponseContent << endl;

	// �õ�����ͷ
	WinHttp.GetAllHeaders();
	wcout << WinHttp.m_strAllHeaders << endl;
	getchar();
}