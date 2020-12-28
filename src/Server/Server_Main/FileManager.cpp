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
		// ���ϲ��ִ���ʹ�á�==�������жϣ�Ȼ����Щ�ļ��еĸ�����ֵ��Ϊ16��
		// ������16~32֮�䣬��ʹ�ð�λ�������Ϊ��ȷ
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
	// ������·����Ŀ¼
	if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	
	{
		// ����ɾ��ģʽ + �ɹ�ɨ���·��
		if (CASCADE_DELETE && FileScan(filePath))	
		{
			for (vector<string>::iterator file_iter = fileScanResult.begin(); 
				file_iter != fileScanResult.end(); 
				++file_iter)
			{
				// ��ɨ�����е������ļ�ɾ��
				DeleteFile((*file_iter).c_str());	
			}
			return true;
		}
		else
		{
			return false; // ���Ǽ���ɾ��ģʽ���߸�����·����Ч
		}
	}
	else // ������·������Ŀ¼���������ļ�
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