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
	bool initializeError;		// 检查初始化状态，当构造函数执行失败时将该变量设置为1
	bool WaitConnect();					// 与客户端取得连接（被动）
	bool Connect2Client();				// 与客户端取得连接（主动）
	bool ReceiveData();					// 接受数据，返回实际接受的字节数
	bool SendData(char* data, int len);	// 发送数据，返回是否成功
	bool SendData(const char* data, int len);
	bool SendFile(char* filePath);     //发送文件
	bool ReceiveFile(char* filePath); //接受文件
	bool Clear();
	NetService();							// 缺省的构造函数
	NetService(const char* ip, int port);	// 构造函数，传入本机（或对方）的ip与开放的端口
private:
	SOCKET servSock;
	SOCKET clientSock;
	SOCKET fileSock;			// FTP运行模式，仅当传输文件时打开
	sockaddr_in servAddr;
	sockaddr_in clientAddr;
	sockaddr_in fileAddr;
};

#endif