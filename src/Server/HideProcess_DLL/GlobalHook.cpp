// GlobalHook.cpp：
/*
首先由主程序显示加载DLL，DLLMain中将全局句柄g_hHook设置为DLL本身的句柄。
我们在SetWindowsHookEx中使用了g_hModule，且将最后一个变量设置为0（全局Hook），
因此被我们的消息钩子钩取的所有进程都会加载我们的DLL。
*/
#include "pch.h"


extern HMODULE g_hModule;

#pragma data_seg("mydata")
HHOOK g_hHook = NULL;
#pragma data_seg()
#pragma comment(linker, "/SECTION:mydata,RWS")

extern "C" __declspec(dllexport) HHOOK SetHook();
extern "C" __declspec(dllexport) BOOL UnsetHook(HHOOK hHook);
// 消息全局钩子回调函数
LRESULT CALLBACK GetMsgProc(
	int code,       // hook code
	WPARAM wParam,  // removal option  
	LPARAM lParam   // message
)
{
	// 不进行任何操作，设置全局钩子的目的就是进行DLL注入而已，主要是主入口进行的API挂钩
	return ::CallNextHookEx(g_hHook, code, wParam, lParam);
}

// 设置全局钩子
HHOOK SetHook()
{
	// 设置全局钩子
	HHOOK hHook = ::SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, g_hModule, 0);
	if (NULL == hHook)
	{
		return NULL;
	}
	g_hHook = hHook;
	return hHook;
}


// 卸载全局钩子
BOOL UnsetHook(HHOOK hHook)
{
	if (FALSE == ::UnhookWindowsHookEx(hHook))
	{
		return FALSE;
	}
	return TRUE;
}
