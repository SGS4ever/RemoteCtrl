#pragma once
#ifndef CONTROLLER_H
#define CONTROLLER_H

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
#include "FileManager.h"
#include "ScreenManager.h"
#include "NetService.h"
#include "Shell.h"
#include "Global.h"

class Controller
{
public:
	bool working;
	bool initializeError;
	Controller();
	bool RecvCmd();
	bool ExecCmd();		// 从recvBuffer中获取指令
	// void Restart();
	void Close();
protected:
	NetService ns;
	FileManager fm;
	ScreenManager sc;
	Shell sh;
};

#endif
