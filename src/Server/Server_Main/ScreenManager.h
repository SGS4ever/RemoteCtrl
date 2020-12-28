#pragma once
#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

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

class ScreenManager
{
public:
	bool Shot();
	void MakeZip();
};

#endif