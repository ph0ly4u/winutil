#define DLL_EXPORTS
#include "IniHelper.h"
#include <windows.h>
NAMESPACE_PH0LY_BEGIN(conf)

IniHelper::IniHelper(void)
{
}

IniHelper::~IniHelper(void)
{
}

std::vector<std::string> IniHelper::GetChildren(std::string strPath, std::string strNode)
{
	std::vector<std::string> tmp;
	return tmp;
}

std::vector<std::string> IniHelper::GetProfileSectionNodes(std::string strPath, std::string strNode)
{
	std::vector<std::string> tmp;
	DWORD dwNumber = 1024*1024;
	char * pBuf = new char[dwNumber];
	DWORD dwRead = GetPrivateProfileSectionA(strNode.data(), pBuf, dwNumber, strPath.data());
	char * p = pBuf;
	for (size_t i=0; i<dwRead; i++)
	{
		if (pBuf[i] == 0)
		{
			tmp.push_back(p);
			p += strlen(p);
			p++;
		}
	}
	delete[] pBuf;
	return tmp;
}

NAMESPACE_PH0LY_END