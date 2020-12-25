#pragma once
#ifndef NET_SERVICE_H
#define NET_SERVICE_H

#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <iomanip>
#include <WinUser.h>

class NetService
{
public:
	// bool isNetWorkAvailable = false;
	bool initializeError;		// ����ʼ��״̬�������캯��ִ��ʧ��ʱ���ñ�������Ϊ1
	bool WaitConnect();					// ��ͻ���ȡ�����ӣ�������
	bool Connect2Client();				// ��ͻ���ȡ�����ӣ�������
	bool ReceiveData();					// �������ݣ�����ʵ�ʽ��ܵ��ֽ���
	bool SendData(char* data, int len);	// �������ݣ������Ƿ�ɹ�
	bool SendData(const char* data, int len);
	bool SendFile(char* filePath);     //�����ļ�
	bool ReceiveFile(char* filePath); //�����ļ�
	bool Clear();
	NetService();							// ȱʡ�Ĺ��캯��
	NetService(const char* ip, int port);	// ���캯�������뱾������Է�����ip�뿪�ŵĶ˿�
private:
	SOCKET servSock;
	SOCKET clientSock;
	SOCKET fileSock;			// FTP����ģʽ�����������ļ�ʱ��
	sockaddr_in servAddr;
	sockaddr_in clientAddr;
	sockaddr_in fileAddr;
};

#endif