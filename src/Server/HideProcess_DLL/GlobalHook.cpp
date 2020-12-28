// GlobalHook.cpp��
/*
��������������ʾ����DLL��DLLMain�н�ȫ�־��g_hHook����ΪDLL����ľ����
������SetWindowsHookEx��ʹ����g_hModule���ҽ����һ����������Ϊ0��ȫ��Hook����
��˱����ǵ���Ϣ���ӹ�ȡ�����н��̶���������ǵ�DLL��
*/
#include "pch.h"


extern HMODULE g_hModule;

#pragma data_seg("mydata")
HHOOK g_hHook = NULL;
#pragma data_seg()
#pragma comment(linker, "/SECTION:mydata,RWS")

extern "C" __declspec(dllexport) HHOOK SetHook();
extern "C" __declspec(dllexport) BOOL UnsetHook(HHOOK hHook);
// ��Ϣȫ�ֹ��ӻص�����
LRESULT CALLBACK GetMsgProc(
	int code,       // hook code
	WPARAM wParam,  // removal option  
	LPARAM lParam   // message
)
{
	// �������κβ���������ȫ�ֹ��ӵ�Ŀ�ľ��ǽ���DLLע����ѣ���Ҫ������ڽ��е�API�ҹ�
	return ::CallNextHookEx(g_hHook, code, wParam, lParam);
}

// ����ȫ�ֹ���
HHOOK SetHook()
{
	// ����ȫ�ֹ���
	HHOOK hHook = ::SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, g_hModule, 0);
	if (NULL == hHook)
	{
		return NULL;
	}
	g_hHook = hHook;
	return hHook;
}


// ж��ȫ�ֹ���
BOOL UnsetHook(HHOOK hHook)
{
	if (FALSE == ::UnhookWindowsHookEx(hHook))
	{
		return FALSE;
	}
	return TRUE;
}
