#pragma once
#ifndef SHELL_H
#define SHELL_H

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

class Shell
{
public:
	std::string eval(char* cmd);
};

#endif