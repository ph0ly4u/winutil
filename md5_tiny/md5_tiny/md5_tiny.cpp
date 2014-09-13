// md5_tiny.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "md5.h"
#include <iostream>
#include <queue>
#include <windows.h>

std::queue<std::string> filequeue;
std::queue<std::string> resultqueue;
DWORD TraverseFolder(LPCSTR path);

CRITICAL_SECTION g_cs;

int _tmain(int argc, _TCHAR* argv[])
{
	//const char *text = "hello world";
	if (argc != 4)
	{
		printf("Usage: source folder, md5 code, dest file name\n");
		return 0;
	}

	TraverseFolder(argv[1]);
	filequeue.pop();

	HANDLE hResultFile = CreateFile(argv[3], 
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 
		NULL,
		CREATE_ALWAYS, 
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	if (hResultFile == INVALID_HANDLE_VALUE)
	{
		printf("create file %s failed!\n", argv[3]);
		getchar();
		return 0;
	}
	filequeue.pop();
	CMD5 iMD5;
	do 
	{
		std::string strFilePath = filequeue.front();
		filequeue.pop();

		HANDLE hFile = CreateFile(strFilePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING, 
			FILE_FLAG_RANDOM_ACCESS,
			NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			printf("open file %s failed!\n", strFilePath.c_str());
			getchar();
			return 0;
		}

		DWORD dwFileSizeHigh;
		__int64 qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

		if (dwFileSizeHigh > 0)
		{			
			printf("this file %s is too large, please choose the advance tool\n", strFilePath.c_str());
			getchar();
			return 0;
		}

		unsigned char *buf = new unsigned char[qwFileSize + 1];
		memset(buf, '\0', qwFileSize + 1);

		DWORD dwBufLen = 0;

		ReadFile(hFile, buf, qwFileSize, &dwBufLen, 0);
		iMD5.GenerateMD5(buf, qwFileSize);
		std::string result = iMD5.ToString();

		if (strcmp(result.c_str(), argv[2]))
		{
			resultqueue.push(strFilePath.append(result));
		}
		delete [] buf;
		CloseHandle(hFile);
	} while (!filequeue.empty());
	
	
	while (!resultqueue.empty())
	{
		DWORD dwBufLen = 0;
		std::string strResult = resultqueue.front();
		int resultLen = strResult.length();
		WriteFile(hResultFile, strResult.c_str(), resultLen, &dwBufLen, 0);
		WriteFile(hResultFile, "\r\n", sizeof("\r\n"), &dwBufLen, 0);
		resultqueue.pop();
	} 

	CloseHandle(hResultFile);
	printf("finish!\n");
	
	return 0;
}

DWORD TraverseFolder(LPCSTR lpPath)
{
	CHAR szFind[MAX_PATH] = {("\0")};
	CHAR szFilePath[MAX_PATH] = {("\0")};
	WIN32_FIND_DATA findFileData;
	BOOL bRet;
	int nFileNum = 0;

	strcpy_s(szFind, MAX_PATH, lpPath);
	strcat_s(szFind, "\\*.*");     //这里一定要指明通配符，不然不会读取所有文件和目录

	HANDLE hFind = ::FindFirstFile(szFind, &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return 0;
	}

	//遍历文件夹
	while (TRUE)
	{
		strcpy_s(szFilePath, MAX_PATH, lpPath);
		strcat_s(szFilePath, findFileData.cFileName);

		filequeue.push(szFilePath);
		//如果是当前路径或者父目录的快捷方式，或者是普通目录，则寻找下一个目录或者文件
		bRet = ::FindNextFile(hFind, &findFileData);
		if (!bRet)
		{//函数调用失败
			//cout << "FindNextFile failed, error code: " 
			//  << GetLastError() << endl;
			break;
		}
		
	}

	::FindClose(hFind);
}
