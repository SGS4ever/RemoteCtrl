#include "Config.h"
#include "Global.h"
#include "FileManager.h"

using namespace std;

bool FileManager::FileScan(char* filePath)
{
	char nowPath[MAX_PATH] = { '0' };
	char newPath[MAX_PATH] = { '0' };
	char fileGet[MAX_PATH] = { '0' };
	WIN32_FIND_DATA data;

	strcpy(nowPath, filePath);
	strcat(nowPath, "\\*.*");
	HANDLE hFind = FindFirstFile(nowPath, &data);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		cout << "Invalid: " << nowPath << endl;
		return false;
	}
	do
	{
		if ((!strcmp(".", data.cFileName)) 
			|| (!strcmp("..", data.cFileName)))
		{
			continue;
		}
		// 网上部分代码使用“==”进行判断，然而有些文件夹的该属性值不为16，
		// 而是在16~32之间，故使用按位与运算较为正确
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			sprintf(newPath, "%s\\%s", filePath, data.cFileName);
			directoryScanResult.push_back(newPath);
			cout << newPath << endl;
			// FileScan(newPath);
		}
		else
		{
			sprintf(fileGet, "%s\\%s", filePath, data.cFileName);
			cout << fileGet << endl;
			fileScanResult.push_back(fileGet);
		}
	} while (FindNextFile(hFind, &data));

	FindClose(hFind);
	return true;
}

bool FileManager::FileDelete(char* filePath)
{
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(filePath, &data);
	// 给出的路径是目录
	if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	
	{
		// 级联删除模式 + 成功扫描该路径
		if (CASCADE_DELETE && FileScan(filePath))	
		{
			for (vector<string>::iterator file_iter = fileScanResult.begin(); 
				file_iter != fileScanResult.end(); 
				++file_iter)
			{
				// 将扫描结果中的所有文件删除
				DeleteFile((*file_iter).c_str());	
			}
			return true;
		}
		else
		{
			return false; // 不是级联删除模式或者给出的路径无效
		}
	}
	else // 给出的路径不是目录，可能是文件
	{
		if (DeleteFile(filePath))
		{
			FindClose(hFind);
			return true;
		}
		else
		{
			FindClose(hFind);
			return false;
		}
	}
}