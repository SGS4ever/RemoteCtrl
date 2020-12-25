#pragma once
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

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

class FileManager
{
public:
	bool FileScan(char* filePath);
	bool FileDelete(char* filePath);
};

#endif