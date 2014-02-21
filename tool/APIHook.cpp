#define DLL_EXPORTS
#include "APIHook.h"
#include <windows.h>
#include "Toolhelp.h"
#include "PE.h"
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")
//#define __DEBUG__
NAMESPACE_BEGIN

APIHook::APIHook(LPCSTR lpszModule, LPCSTR lpszCurrentModule, LPCSTR lpszFuncName, PROC pfnHook)
{
	m_pfnOrig = GetProcAddress(GetModuleHandleA(lpszModule), lpszFuncName);
	m_pfnCurrent = pfnHook;

	m_strModule = lpszModule;
	m_strCurrentModule = lpszCurrentModule;

#ifdef __DEBUG__
	FILE *fp = fopen("d:\\aa.txt", "ab");
	char szModule[MAX_PATH];
	GetModuleFileNameA(GetModuleHandleA(NULL), szModule, MAX_PATH);
	fputs(szModule, fp);
	fputs("\r\n", fp);
	fclose(fp);
	MessageBoxA(GetActiveWindow(), "hhh", "", 0);
#endif

	ReplaceIAT();
}

APIHook::APIHook(LPCSTR lpszCurrentModule, LPCSTR lpszFuncName, PROC pfnHook)
{
	//没有模块名称，需要遍历找出
	PE pe;
	pe.Load(GetCurrentProcessId());
	std::map<std::string, std::vector<std::string>> dlls;
	bool bValid = pe.GetImportFunctions(dlls);
	std::string strModule = "";
	for (std::map<std::string, std::vector<std::string>>::iterator iter=dlls.begin(); 
		 iter != dlls.end(); iter++)
	{
		std::vector<std::string> funcs = iter->second;
		bool flag = false;
		for (size_t i=0; i<funcs.size(); i++)
		{
			if (_strcmpi(funcs[i].data(), lpszFuncName) == 0)
			{
				flag = true;
				break;
			}
		}
		if (flag) {
			strModule = iter->first;
			break;
		}
	}
#ifdef __DEBUG__
	FILE *fp = fopen("d:\\aa.txt", "ab");
	char szModule[MAX_PATH];
	GetModuleFileNameA(GetModuleHandleA(NULL), szModule, MAX_PATH);
	fputs(szModule, fp);
	fputs("\r\n", fp);
	fclose(fp);
	MessageBoxA(GetActiveWindow(), "hhh", "", 0);
#endif
	m_pfnOrig = GetProcAddress(GetModuleHandleA(strModule.data()), lpszFuncName);
	m_pfnCurrent = pfnHook;

	m_strModule = strModule;
	m_strCurrentModule = lpszCurrentModule;

	ReplaceIAT();
}

APIHook::~APIHook(void)
{
	RestoreIAT();
}

void APIHook::ReplaceIAT()
{
	ReplaceIATInOneMod(m_pfnOrig, m_pfnCurrent);
}

void APIHook::RestoreIAT()
{
	ReplaceIATInOneMod(m_pfnCurrent, m_pfnOrig);
}

void APIHook::ReplaceIATInOneMod(PROC pfnOrig, PROC pfnNew)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	MODULEENTRY32 me32 = {sizeof(MODULEENTRY32)};
	Module32First(hSnapshot, &me32);
	HMODULE hThisMod = GetModuleHandleA(m_strCurrentModule.data());
	do 
	{
		if (me32.hModule == hThisMod)
		{
			continue;
		}
		ULONG ulSize;
		PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
		__try
		{
			pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(me32.hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{

		}
		if (pImportDesc == NULL)
			continue;
		for (; pImportDesc->Name; pImportDesc++)
		{
			PSTR pszName = (PSTR)((PBYTE)me32.hModule + pImportDesc->Name);
			//cout << pszName << endl;
			if (lstrcmpiA(m_strModule.data(), pszName) == 0)
			{
				PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)me32.hModule + pImportDesc->FirstThunk);
				for (; pThunk->u1.Function; pThunk++) {

					// Get the address of the function address
					PROC* ppfn = (PROC*) &pThunk->u1.Function;

					// Is this the function we're looking for?
					BOOL bFound = (*ppfn == pfnOrig);
					if (bFound) {

						//PROC pfnNew = (PROC)Hook_ExitProcess;
						if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(char*), NULL) &&
							ERROR_NOACCESS == GetLastError())
						{
							DWORD dwOldProtect;
							if (VirtualProtect(ppfn, sizeof(char*), PAGE_WRITECOPY, &dwOldProtect))
							{
								WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(char*), NULL);
								VirtualProtect(ppfn, sizeof(char*), dwOldProtect, &dwOldProtect);
							}
						}
						CloseHandle(hSnapshot);
						return ;
					}
				}
			}
		}
	} while (Module32Next(hSnapshot, &me32));
	CloseHandle(hSnapshot);
}

NAMESPACE_END