#include "Shell.h"
#include "Global.h"

using namespace std;

string Shell::eval(char* cmd)
{
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE hRead, hWrite;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return "";
	}
	// ���������н���������Ϣ(�����ط�ʽ���������λ�������hWrite
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	// ����������
	PROCESS_INFORMATION pi;
	if (!CreateProcess(NULL, (LPSTR)cmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		return "";
	}
	// �����ر�hWrite
	CloseHandle(hWrite);
	// ��ȡ����ֵ
	string strRet;
	char buff[1024] = { 0 };
	DWORD dwRead = 0;
	strRet = buff;
	while (ReadFile(hRead, buff, 1024, &dwRead, NULL))
	{
		strRet += buff;
		memset(buff, 0, 1024);
	}
	CloseHandle(hRead);

	return strRet;
}
