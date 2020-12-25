#include "Config.h"
#include "Global.h"
#include "NetService.h"

#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

using namespace std;

NetService::NetService()
{
	WSADATA wsaData;
	memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
	memset(&clientAddr, 0, sizeof(clientAddr));
	initializeError = false;

	int ires = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ires)
	{
		initializeError = true;
	}
	if (WORKMODE == 1)	// 被动连接，初始化servSock，等待clientSock
	{
		servSock = socket(AF_INET, SOCK_STREAM, 0);
		if (servSock == -1)
		{
			initializeError = true;
			WSACleanup();
		}
		// 以下配置socket属性（IP、端口、地址簇
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = inet_addr("0");
		servAddr.sin_port = htons(2333);
		// 以下绑定socket
		int tmp = bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr));
		if (tmp != 0)
		{
			initializeError = true;
			WSACleanup();
		}
	}
	else if (WORKMODE == 2)		// 主动连接，初始化clientSock，等待servSock
	{
		// 只需要配置clientSock，之后调用Connect2Client函数进行连接
		clientSock = socket(AF_INET, SOCK_STREAM, 0);
		if (clientSock == -1)
		{
			initializeError = true;
			WSACleanup();
		}
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_addr.s_addr = inet_addr(TARGET_IP);
		clientAddr.sin_port = htons(3332);
	}
}

bool NetService::WaitConnect()	// 等待连接，成功返回true
{
	if (!listen(servSock, 10))		// listen 返回0，监听到连接请求
	{
		int tmp = sizeof(SOCKADDR);
		clientSock = accept(servSock, (SOCKADDR*)&clientAddr, &tmp);
		return true;
	}
	return false;
}

bool NetService::Connect2Client()
{
	if (connect(clientSock, (SOCKADDR*)&clientAddr, sizeof(clientAddr))) // connect返回0，成功
	{
		return false;
	}
	cout << "Using clientSock: " << clientSock << endl;
	return true;
}

bool NetService::SendData(char* data, int len = 0)	// 发送数据，成功返回true
{
	if (!len)	// len = 0
	{
		len = strlen(data);
	}
	if (send(clientSock, data, len, 0) != SOCKET_ERROR)
	{
		cout << "Successfully send." << endl;
		return true;
	}
	else
	{
		cout << "Failed to send data." << endl;
		return false;
	}
}

bool NetService::SendData(const char* data, int len = 0)
{
	if (!len)	// len = 0
	{
		len = strlen(data);
	}
	if (send(clientSock, data, len, 0) != SOCKET_ERROR)
	{
		cout << "Successfully send." << endl;
		return true;
	}
	else
	{
		cout << "Failed to send data." << endl;
		return false;
	}
}

bool NetService::ReceiveData()		// 接受数据，成功返回true，数据存在全局变量recvBuffer中
{
	int tmp;

	memset(recvBuffer, 0, sizeof(recvBuffer));

	cout << clientSock << endl;
	tmp = recv(clientSock, recvBuffer, BUFFER_SIZE, NULL);
	if (!tmp)	// receive 0
	{
		cout << "Peer closed socket." << endl;
		return false;
	}
	else if (tmp < 0)
	{
		cout << "Failed to receive.." << endl;
		cout << "Error Code: " << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "Received from :" << inet_ntoa(clientAddr.sin_addr) << endl;
		cout << recvBuffer << endl;
		return true;
	}

}

bool NetService::Clear()
{
	closesocket(servSock);
	closesocket(clientSock);
	memset(&servAddr, 0, sizeof(servAddr));
	memset(&clientAddr, 0, sizeof(clientAddr));
	WSACleanup();
	return true;
}

bool NetService::SendFile(char* filePath)
{
	char sendBuffer[11000];
	ifstream targetFile;
	int readLen;
	fileSock = socket(AF_INET, SOCK_STREAM, 0);
	if (fileSock == -1)
	{
		return false;
	}
	fileAddr.sin_family = AF_INET;
	fileAddr.sin_addr.s_addr = inet_addr(TARGET_IP);
	fileAddr.sin_port = htons(2333);
	if (connect(fileSock, (SOCKADDR*)&fileAddr, sizeof(fileAddr))) // connect返回0，成功,非0，失败
	{
		return false;
	}
	targetFile.open(filePath, ios::binary);
	if (!targetFile)	// 打开文件失败，返回false
	{
		return false;
	}
	while (!targetFile.eof())
	{
		targetFile.read(sendBuffer, BUFFER_SIZE);	// 每一次发送和接受数据都以BUFFER_BUFFER_SIZE为单位，故读取文件也以BUFFER_BUFFER_SIZE为单位
		readLen = targetFile.gcount();		// 获取每次读取的长度

		if (send(fileSock, sendBuffer, readLen, 0) != SOCKET_ERROR)
		{
			cout << "Successfully send." << endl;
			// debug += readLen;
		}
		else
		{
			targetFile.close();
			closesocket(fileSock);
			return false;
		}
	}
	// cout << debug / 1024 << endl;
	closesocket(fileSock);
	targetFile.close();		// 成功发送，关闭文件，返回true
	return true;
}

bool NetService::ReceiveFile(char* filePath)
{
	ofstream targetFile;
	int recvLen = 0;

	fileSock = socket(AF_INET, SOCK_STREAM, 0);
	if (fileSock == -1)
	{
		return false;
	}
	fileAddr.sin_family = AF_INET;
	fileAddr.sin_addr.s_addr = inet_addr(TARGET_IP);
	fileAddr.sin_port = htons(2333);
	if (connect(fileSock, (SOCKADDR*)&fileAddr, sizeof(fileAddr))) // connect返回0，成功,非0，失败
	{
		return false;
	}
	targetFile.open(filePath, ios::binary);
	if (!targetFile)
	{
		return false;
	}
	while (true)
	{
		recvLen = recv(fileSock, recvBuffer, BUFFER_SIZE, 0);	// 返回实际接受的字节数，等于0时表示对端关闭连接，小于0时接受出错
		if (recvLen > 0)
		{
			targetFile.write(recvBuffer, recvLen);
		}
		else if (recvLen == 0)	// 对端关闭连接
		{
			break;
		}
		else
		{
			// 出现异常，recv返回负数
			closesocket(fileSock);
			targetFile.close();
			return false;
		}
	}
	// 正常退出
	closesocket(fileSock);
	targetFile.close();
	return true;
}

