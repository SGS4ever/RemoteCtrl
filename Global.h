#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include <string>
#include "Config.h"

using namespace std;

extern char recvBuffer[BUFFER_SIZE];		// socket���ջ�����

extern vector<string> fileScanResult;		// �ļ�����ʱ�����������ļ�

extern vector<string> directoryScanResult;	// �ļ�����ʱ�����������ļ���

#endif