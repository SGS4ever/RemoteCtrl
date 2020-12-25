#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include <string>
#include "Config.h"

using namespace std;

extern char recvBuffer[BUFFER_SIZE];		// socket接收缓冲区

extern vector<string> fileScanResult;		// 文件遍历时遇到的所有文件

extern vector<string> directoryScanResult;	// 文件遍历时遇到的所有文件夹

#endif