#define DLL_EXPORTS
#include "File.h"
#include <iostream>
#include <windows.h>
#include <fstream>
NAMESPACE_BEGIN
using namespace std;

File::File(void)
{
}

File::~File(void)
{
}

// void FileEx::GetFileExtension(char* name, char* out)
// {
// 	std::string strTmp = name;
// 	int nPos = strTmp.find_last_of('.');
// 	if (strTmp.length() > nPos)
// 	{
// 		std::string tmp = strTmp.substr(nPos+1, strTmp.length() - nPos);
// 		strcpy(out, tmp.c_str());
// 		out[tmp.length()] = 0;
// 	}
// 	strcpy(out, "");
// }

bool File::GetFileExtension(const char* lpszName, char* lpszOut, int nMaxBuffer)
{
	if (lpszName == NULL || lpszOut == NULL)
	{
		return false;
	}
	std::string strTmp = lpszName;
	unsigned int nPos = strTmp.find_last_of('.');
	if (strTmp.length() > nPos && nPos != -1)
	{
		std::string tmp = strTmp.substr(nPos+1, strTmp.length() - nPos);
		//strcpy_s(lpszOut, tmp.size() + 1, tmp.c_str());
		memcpy_s(lpszOut, nMaxBuffer, tmp.c_str(), tmp.size()+1);
		lpszOut[tmp.length()] = 0;
		return true;
	}
	lpszOut[0] = 0;
	return true;
}

bool File::GetFilePrefix(const char* lpszName, char* lpszOut, int nMaxBuffer)
{
	if (lpszName == NULL || lpszOut == NULL)
	{
		return false;
	}
	std::string strTmp = lpszName;
	int nPos = strTmp.find_last_of('.');
	if (nPos != -1)
	{
		strTmp = strTmp.substr(0, nPos);
		//strcpy_s(lpszOut, strTmp.size() + 1, strTmp.c_str());
		memcpy_s(lpszOut, nMaxBuffer, strTmp.c_str(), strTmp.size()+1);
		lpszOut[nPos] = 0;
		return true;
	}
	lpszOut[0] = 0;
	return true;
}

bool File::GetFileName(const char* lpszName, char* lpszOut, int nMaxBuffer)
{
	if (lpszName == NULL || lpszOut == NULL)
	{
		return false;
	}
	std::string strTmp = lpszName;
	int nStartPos = strTmp.find_last_of('\\') + 1;
	int nEndPos = strTmp.find_last_of('.');
	if (nStartPos != -1 && nEndPos != -1 && nStartPos < nEndPos)
	{
		strTmp = strTmp.substr(nStartPos, nEndPos - nStartPos);
		//strcpy_s(lpszOut, strTmp.size() + 1, strTmp.c_str());
		memcpy_s(lpszOut, nMaxBuffer, strTmp.c_str(), strTmp.size()+1);
		lpszOut[nEndPos - nStartPos] = 0;
		return true;
	}
	lpszOut[0] = 0;
	return true;
}

bool File::GetFileNameWithExtension(const char* lpszName, char* lpszOut, int nMaxBuffer)
{
	if (lpszName == NULL || lpszOut == NULL)
	{
		return false;
	}
	std::string strTmp = lpszName;
	int nPos = strTmp.find_last_of('\\') + 1;
	strTmp = strTmp.substr(nPos, strTmp.size()-nPos);
	memcpy_s(lpszOut, nMaxBuffer, strTmp.c_str(), strTmp.size()+1);

	return true;
}

bool File::GetDirectory(const char* lpszPath, char * lpszOut, int nMaxBuffer)
{
	if (lpszPath == NULL || lpszOut == NULL)
	{
		return false;
	}
	std::string strTmp = lpszPath;
	int nPos = strTmp.find_last_of('\\');
	if (nPos != -1)
	{
		//strcpy(lpszOut, strTmp.substr(0, nPos).c_str());
		memcpy_s(lpszOut, nMaxBuffer, strTmp.c_str(), strTmp.size()+1);
		lpszOut[nPos] = 0;
		return true;
	}
	lpszOut[0] = 0;
	return true;
}

bool File::IsFile( const char* lpszPath )
{
	WIN32_FIND_DATAA wfd;
	HANDLE hFile = FindFirstFileA(lpszPath, &wfd);
	FindClose(hFile);
	if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
	{
		return false;
	}
	return true;
}

bool File::IsDirectory( const char* lpszPath )
{
	return !IsFile(lpszPath);
}

bool File::FindFiles(const char* lpszDir, const char* lpszEnds, std::vector<std::string>& vOutFiles)
{
	//遍历查找目录内lpszEnds类型的文件
	WIN32_FIND_DATAA wfd;
	std::string path = lpszDir;
	path += "\\*.";
	path += lpszEnds;
	HANDLE hFile = FindFirstFileA(path.c_str(), &wfd);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string tmp = lpszDir;
			tmp += "\\";
			tmp += wfd.cFileName;
			vOutFiles.push_back(tmp);
		}
		while (FindNextFileA(hFile, &wfd));
	}
	FindClose(hFile);
	return true;
}

bool File::Exists(const char* lpszFileName)
{
	if (lpszFileName == NULL)
	{
		return false;
	}
	WIN32_FIND_DATAA wfa;
	HANDLE hFile = FindFirstFileA(lpszFileName, &wfa);
	FindClose(hFile);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	return true;
}

bool File::Create( const char* lpszPath )
{
	ofstream ofs(lpszPath);
	return ofs.good();
}

bool File::Delete( const char* lpszFileName )
{
	return DeleteFileA(lpszFileName) == 1;
}

// bool File::ReadAllText(const char* lpszPath, std::string& strOutput)
// {
// 	using namespace std;
// 	ifstream inf(lpszPath, ios::binary | ios::_Nocreate);
// 	if (!inf)
// 	{
// 		return false;
// 	}
// 	char szBuf[MAX_PATH] = {0};
// 	while (inf.good())
// 	{
// 		inf.getline(szBuf, MAX_PATH);
// 		strOutput += szBuf;
// 	}
// 
// 	return true;
// }

bool File::ReadAllText(const char* lpszPath, std::string& strOutput)//, Mode mode)
{
	using namespace std;
//	DWORD dwMode = 0;
// 	switch (mode)
// 	{
// 	case FM_TEXT:
// 		dwMode = ios::_Nocreate;
// 		break;
// 	case FM_BINARY:
// 		dwMode = ios::binary | ios::_Nocreate;
// 		break;
// 	case FM_CREATE:
// 	case FM_TEXT | FM_CREATE:
// 		break;
// 	case FM_BINARY | FM_CREATE:
// 		dwMode = ios::binary;
// 		break;
// 	default:
// 		return false;
// 	}
	ifstream inf(lpszPath, ios::_Nocreate);
	if (!inf)
	{
		return false;
	}
	char szBuf[MAX_PATH] = {0};
	while (inf.good())
	{
		inf.getline(szBuf, MAX_PATH);
		strOutput += szBuf;
	}

	return true;
}

bool File::SaveAllText(const char* lpszPath, const std::string& strInput)//, Mode mode)
{
	ofstream ofs(lpszPath);
	if (!ofs.good())
	{
		return false;
	}
	ofs.write(strInput.data(), strInput.size());
	return true;
}
NAMESPACE_END
//bool File::ReadAll( const char* lpszPath, char** lpszOutput, size_t& uBuffer )
//{
//	if (lpszPath == NULL || lpszOutput == NULL )
//	{
//		return false;
//	}
//	ifstream ifs(lpszPath, ios::binary | ios::_Nocreate);
//	if (!ifs.good())
//	{
//		return false;
//	}
//	int nBuffer = 1024;
//	int nSum = 0;
//	*lpszOutput = new char[nBuffer];
//	char szBuf[1024];
//	while (ifs.good())
//	{
//		int count = ifs.read(szBuf, nBuffer).gcount();
//		nSum += count;
//		char* p = new char[nSum];
//		memcpy_s(p, nSum, szBuf, nBuffer);
//		delete [] *lpszOutput;
//	}
//}