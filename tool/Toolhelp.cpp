#define DLL_EXPORTS
#include "Toolhelp.h"
#include <stdexcept>

using namespace std;
NAMESPACE_PH0LY_BEGIN(tool)
Toolhelp::Toolhelp(void) : m_hSnapshot(NULL), m_dwRefer(0)
{
}

Toolhelp::Toolhelp(DWORD dwFlags, DWORD dwProcessID)
{
	m_hSnapshot = NULL;
	CreateSnapshot(dwFlags, dwProcessID);
}

Toolhelp::~Toolhelp(void)
{
	m_dwRefer = 0;
	CloseHandle(m_hSnapshot);
}

bool Toolhelp::CreateSnapshot(DWORD dwFlags, DWORD dwProcessID)
{
	if (m_hSnapshot != NULL)
	{
		CloseHandle(m_hSnapshot);
		m_dwRefer = 0;
	}
	m_hSnapshot = CreateToolhelp32Snapshot(dwFlags, dwProcessID);
	m_dwFlags = dwFlags;
	return m_hSnapshot != INVALID_HANDLE_VALUE;
}

BOOL Toolhelp::Reset()
{
	m_dwRefer = 0;
	return true;
}

BOOL Toolhelp::Next(THREADENTRY32& te)
{
	if (m_hSnapshot == NULL)
	{
		return false;
	}
	//不是线程类型
	if (m_dwFlags != TH32CS_SNAPTHREAD)
	{
		return false;
	}
	BOOL bOk = (m_dwRefer++ == 0 ? Thread32First(m_hSnapshot, &te) : Thread32Next(m_hSnapshot, &te));

	return bOk;
}

BOOL Toolhelp::Next(PROCESSENTRY32& pe)
{
	if (m_hSnapshot == NULL)
	{
		return false;
	}
	//不是进程类型
	if (m_dwFlags != TH32CS_SNAPPROCESS)
	{
		return false;
	}
	BOOL bOk = (m_dwRefer++ == 0 ? Process32First(m_hSnapshot, &pe) : Process32Next(m_hSnapshot, &pe));

	return bOk;
}

BOOL Toolhelp::Next(MODULEENTRY32& me)
{
	if (m_hSnapshot == NULL)
	{
		return false;
	}
	//不是模块类型
	if (m_dwFlags != TH32CS_SNAPMODULE)
	{
		return false;
	}
	BOOL bOk = (m_dwRefer++ == 0 ? Module32First(m_hSnapshot, &me) : Module32Next(m_hSnapshot, &me));

	return bOk;
}

BOOL Toolhelp::Next(HEAPENTRY32& he, DWORD dwProcessID, UINT_PTR dwHeapID)
{
	if (m_hSnapshot == NULL)
	{
		return false;
	}
	//不是堆类型
	if (m_dwFlags != TH32CS_SNAPHEAPLIST)
	{
		return false;
	}
	BOOL bOk = (m_dwRefer++ == 0 ? Heap32First(&he, dwProcessID, dwHeapID) : Heap32Next(&he));

	return bOk;
}

BOOL Toolhelp::Find(DWORD dwThreadId, THREADENTRY32& te) const
{
	if (m_hSnapshot == NULL)
	{
		return false;
	}

	for (BOOL bOk = Thread32First(m_hSnapshot, &te); bOk; bOk = Thread32Next(m_hSnapshot, &te))
	{
		if (te.th32ThreadID == dwThreadId)
		{
			return true;
		}
	}
	return false;
}

BOOL Toolhelp::Find(DWORD dwProcessId, PROCESSENTRY32& pe) const
{
	if (m_hSnapshot == NULL)
	{
		return false;
	}

	for (BOOL bOk = Process32First(m_hSnapshot, &pe); bOk; bOk = Process32Next(m_hSnapshot, &pe))
	{
		if (pe.th32ProcessID == dwProcessId)
		{
			return true;
		}
	}
	return false;
}

BOOL Toolhelp::Find(LPCSTR pszExeName, PROCESSENTRY32& pe) const
{
	if (m_hSnapshot == NULL)
	{
		return false;
	}

	for (BOOL bOk = Process32First(m_hSnapshot, &pe); bOk; bOk = Process32Next(m_hSnapshot, &pe))
	{
		if (_strcmpi(pe.szExeFile, pszExeName) == 0)
		{
			return true;
		}
	}
	return false;
}

BOOL Toolhelp::Find(LPCSTR pszModulePath, MODULEENTRY32& me) const
{
	if (m_hSnapshot == NULL)
	{
		return false;
	}

	for (BOOL bOk = Module32First(m_hSnapshot, &me); bOk; bOk = Module32Next(m_hSnapshot, &me))
	{
		if (_strcmpi(me.szModule, pszModulePath) == 0 ||
			_strcmpi(me.szExePath, pszModulePath) == 0)
		{
			return true;
		}
	}
	return false;
}

BOOL Toolhelp::EnablePrivilege(PCTSTR szPrivilege, BOOL fEnable) {

	// Enabling the debug privilege allows the application to see
	// information about service applications
	BOOL fOk = FALSE;    // Assume function fails
	HANDLE hToken;

	// Try to open this process's access token
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, 
		&hToken)) {

			// Attempt to modify the given privilege
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			LookupPrivilegeValue(NULL, szPrivilege, &tp.Privileges[0].Luid);
			tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
			fOk = (GetLastError() == ERROR_SUCCESS);

			// Don't forget to close the token handle
			CloseHandle(hToken);
	}
	return(fOk);
}

NAMESPACE_PH0LY_END