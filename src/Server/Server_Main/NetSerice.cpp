#include "Config.h"
#include "Global.h"
#include "NetService.h"

#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll

using namespace std;

NetService::NetService()
{
	WSADATA wsaData;
	memset(&servAddr, 0, sizeof(servAddr));  //ÿ���ֽڶ���0���
	memset(&clientAddr, 0, sizeof(clientAddr));
	initializeError = false;

	int ires = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ires)
	{
		initializeError = true;
	}
	if (WORKMODE == 1)	// �������ӣ���ʼ��servSock���ȴ�clientSock
	{
		servSock = socket(AF_INET, SOCK_STREAM, 0);
		if (servSock == -1)
		{
			initializeError = true;
			WSACleanup();
		}
		// ��������socket���ԣ�IP���˿ڡ���ַ��
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = inet_addr("0");
		servAddr.sin_port = htons(2333);
		// ���°�socket
		int tmp = bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr));
		if (tmp != 0)
		{
			initializeError = true;
			WSACleanup();
		}
	}
	else if (WORKMODE == 2)		// �������ӣ���ʼ��clientSock���ȴ�servSock
	{
		// ֻ��Ҫ����clientSock��֮�����Connect2Client������������
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

bool NetService::WaitConnect()	// �ȴ����ӣ��ɹ�����true
{
	if (!listen(servSock, 10))		// listen ����0����������������
	{
		int tmp = sizeof(SOCKADDR);
		clientSock = accept(servSock, (SOCKADDR*)&clientAddr, &tmp);
		return true;
	}
	return false;
}

bool NetService::Connect2Client()
{
	if (connect(clientSock, (SOCKADDR*)&clientAddr, sizeof(clientAddr))) // connect����0���ɹ�
	{
		return false;
	}
	cout << "Using clientSock: " << clientSock << endl;
	return true;
}

bool NetService::SendData(char* data, int len = 0)	// �������ݣ��ɹ�����true
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

bool NetService::ReceiveData()		// �������ݣ��ɹ�����true�����ݴ���ȫ�ֱ���recvBuffer��
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
	if (connect(fileSock, (SOCKADDR*)&fileAddr, sizeof(fileAddr))) // connect����0���ɹ�,��0��ʧ��
	{
		return false;
	}
	targetFile.open(filePath, ios::binary);
	if (!targetFile)	// ���ļ�ʧ�ܣ�����false
	{
		return false;
	}
	while (!targetFile.eof())
	{
		targetFile.read(sendBuffer, BUFFER_SIZE);	// ÿһ�η��ͺͽ������ݶ���BUFFER_BUFFER_SIZEΪ��λ���ʶ�ȡ�ļ�Ҳ��BUFFER_BUFFER_SIZEΪ��λ
		readLen = targetFile.gcount();		// ��ȡÿ�ζ�ȡ�ĳ���

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
	targetFile.close();		// �ɹ����ͣ��ر��ļ�������true
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
	if (connect(fileSock, (SOCKADDR*)&fileAddr, sizeof(fileAddr))) // connect����0���ɹ�,��0��ʧ��
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
		recvLen = recv(fileSock, recvBuffer, BUFFER_SIZE, 0);	// ����ʵ�ʽ��ܵ��ֽ���������0ʱ��ʾ�Զ˹ر����ӣ�С��0ʱ���ܳ���
		if (recvLen > 0)
		{
			targetFile.write(recvBuffer, recvLen);
		}
		else if (recvLen == 0)	// �Զ˹ر�����
		{
			break;
		}
		else
		{
			// �����쳣��recv���ظ���
			closesocket(fileSock);
			targetFile.close();
			return false;
		}
	}
	// �����˳�
	closesocket(fileSock);
	targetFile.close();
	return true;
}

