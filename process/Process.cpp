#define DLL_EXPORTS
#include "Process.h"
NAMESPACE_BEGIN

//#define __DEBUG__

#ifdef __DEBUG__
#include <iostream>
#endif

Process::Process(void)
{
	m_hProcess = NULL;
	m_hThread = NULL;
	m_hInputWritePipe = NULL;
	m_hInputReadPipe = NULL;
	m_hOutputReadPipe = NULL;
	m_hOutputWritePipe = NULL;
	m_dwProcessId = 0;
	m_dwThreadId = 0;
	m_bDbg = false;
}

Process::~Process(void)
{
	if (m_hProcess != NULL)
	{
		Destroy();
	}
}

Process::ProcessReturns 
Process::Create( const char* lpszCmdlines, bool bShowWnd )
{
	if (m_hProcess != NULL)
	{
		Destroy();
	}

	SECURITY_ATTRIBUTES attr;
	attr.bInheritHandle = TRUE;
	attr.lpSecurityDescriptor = NULL;
	attr.nLength = sizeof(SECURITY_ATTRIBUTES);

	if (!CreatePipe(&m_hInputReadPipe, &m_hInputWritePipe, &attr, MAX_PIPE_INPUT_COUNT))
	{
#ifdef __DEBUG__
		printf("create pipe failed\n");
#endif
		return EP_CREATEPIPEFAILED;
	}
	if (!CreatePipe(&m_hOutputReadPipe, &m_hOutputWritePipe, &attr, MAX_PIPE_INPUT_COUNT))
	{
#ifdef __DEBUG__
		printf("create pipe failed\n");
#endif
		CloseHandle(m_hInputReadPipe);
		CloseHandle(m_hInputWritePipe);
		return EP_CREATEPIPEFAILED;
	}

	STARTUPINFO si;
	GetStartupInfoA(&si);
	si.cb = sizeof(STARTUPINFO);
	si.cbReserved2 = 0;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdError = si.hStdOutput = m_hOutputWritePipe;
	si.hStdInput = m_hInputReadPipe;
	si.wShowWindow = bShowWnd;
	PROCESS_INFORMATION pi;
	TCHAR szBufTmp[2048] = {0};

	memcpy_s(szBufTmp, 2048, lpszCmdlines, strlen(lpszCmdlines)+1);
	if (!CreateProcessA(NULL, szBufTmp, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
#ifdef __DEBUG__
		printf("create process failed...\n");
#endif
		CloseHandle(m_hInputReadPipe);
		CloseHandle(m_hInputWritePipe);
		CloseHandle(m_hOutputReadPipe);
		CloseHandle(m_hOutputWritePipe);
		return EP_CREATEFAILED;
	}
	m_hProcess = pi.hProcess;
	m_hThread = pi.hThread;
	m_dwProcessId = pi.dwProcessId;
	m_dwThreadId = pi.dwThreadId;

	return EP_SUCCESS;
}

Process::ProcessReturns 
Process::CreateDbg(const char* lpszCmdlines)
{
	if (m_hProcess != NULL)
	{
		Destroy();
	}

	SECURITY_ATTRIBUTES attr;
	attr.bInheritHandle = TRUE;
	attr.lpSecurityDescriptor = NULL;
	attr.nLength = sizeof(SECURITY_ATTRIBUTES);

	if (!CreatePipe(&m_hInputReadPipe, &m_hInputWritePipe, &attr, MAX_PIPE_INPUT_COUNT))
	{
#ifdef __DEBUG__
		printf("create pipe failed\n");
#endif
		return EP_CREATEPIPEFAILED;
	}
	if (!CreatePipe(&m_hOutputReadPipe, &m_hOutputWritePipe, &attr, MAX_PIPE_INPUT_COUNT))
	{
#ifdef __DEBUG__
		printf("create pipe failed\n");
#endif
		return EP_CREATEPIPEFAILED;
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount= 1;
	HANDLE hToken;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	DWORD dwRetLen= 0;
	LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);// 查找当前权限
	tp.Privileges[0].Attributes= SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges( hToken, FALSE, &tp, sizeof(tp), NULL, &dwRetLen);//提升至SetDebugPrivilege权限

	STARTUPINFO si;
	GetStartupInfoA(&si);
	si.cb = sizeof(STARTUPINFO);
	si.cbReserved2 = 0;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdError = si.hStdOutput = m_hOutputWritePipe;
	si.hStdInput = m_hInputReadPipe;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	TCHAR szBufTmp[MAX_PATH] = {0};

	memcpy_s(szBufTmp, MAX_PATH, lpszCmdlines, strlen(lpszCmdlines)+1);
	if (!CreateProcessA(NULL, szBufTmp, NULL, NULL, TRUE, DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi))
	{
#ifdef __DEBUG__
		printf("create process failed...\n");
#endif
		CloseHandle(m_hInputReadPipe);
		CloseHandle(m_hInputWritePipe);
		CloseHandle(m_hOutputReadPipe);
		CloseHandle(m_hOutputWritePipe);
		CloseHandle(hToken);
		return EP_CREATEFAILED;
	}
	m_hProcess = pi.hProcess;
	m_hThread = pi.hThread;
	m_dwProcessId = pi.dwProcessId;
	m_dwThreadId = pi.dwThreadId;
	m_bDbg = true;
	CloseHandle(hToken);
	return EP_SUCCESS;
}

Process::ProcessReturns 
Process::CreateSuspend(const char* lpszCmdlines, bool bDebug)
{
	if (m_hProcess != NULL)
	{
		Destroy();
	}

	SECURITY_ATTRIBUTES attr;
	attr.bInheritHandle = TRUE;
	attr.lpSecurityDescriptor = NULL;
	attr.nLength = sizeof(SECURITY_ATTRIBUTES);

	if (!CreatePipe(&m_hInputReadPipe, &m_hInputWritePipe, &attr, MAX_PIPE_INPUT_COUNT))
	{
#ifdef __DEBUG__
		printf("create pipe failed\n");
#endif
		return EP_CREATEPIPEFAILED;
	}
	if (!CreatePipe(&m_hOutputReadPipe, &m_hOutputWritePipe, &attr, MAX_PIPE_INPUT_COUNT))
	{
#ifdef __DEBUG__
		printf("create pipe failed\n");
#endif
		CloseHandle(m_hInputReadPipe);
		CloseHandle(m_hInputWritePipe);
		return EP_CREATEPIPEFAILED;
	}

	STARTUPINFO si;
	GetStartupInfoA(&si);
	si.cb = sizeof(STARTUPINFO);
	si.cbReserved2 = 0;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdError = si.hStdOutput = m_hOutputWritePipe;
	si.hStdInput = m_hInputReadPipe;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	TCHAR szBufTmp[MAX_PATH] = {0};

	memcpy_s(szBufTmp, MAX_PATH, lpszCmdlines, strlen(lpszCmdlines)+1);
	DWORD dwCreateFlag = CREATE_SUSPENDED;
	if (bDebug) {
		dwCreateFlag |= DEBUG_ONLY_THIS_PROCESS;
	}
	m_bDbg = bDebug;
	if (!CreateProcessA(NULL, szBufTmp, NULL, NULL, TRUE, dwCreateFlag, NULL, NULL, &si, &pi))
	{
#ifdef __DEBUG__
		printf("create process failed...\n");
#endif
		CloseHandle(m_hInputReadPipe);
		CloseHandle(m_hInputWritePipe);
		CloseHandle(m_hOutputReadPipe);
		CloseHandle(m_hOutputWritePipe);
		return EP_CREATEFAILED;
	}
	m_hProcess = pi.hProcess;
	m_hThread = pi.hThread;
	m_dwProcessId = pi.dwProcessId;
	m_dwThreadId = pi.dwThreadId;

	return EP_SUCCESS;
}

Process::ProcessReturns 
Process::CreateCommand( bool bShowWnd/* =false */ )
{
	char szCmdPath[MAX_PATH];
	char szBuffer[MAX_PATH];
	GetSystemDirectoryA(szBuffer, MAX_PATH);
	sprintf_s(szCmdPath, MAX_PATH, "%s\\cmd.exe", szBuffer);
	// 	FILE *fp = fopen("c:\\before_create cmd", "wb");
	// 	fputs("aaa", fp);
	// 	fclose(fp);
	return Create(szCmdPath, bShowWnd);
}

Process::ProcessReturns  
Process::CreateAsync( const char* lpszCmdlines, void (*pOnFinished)(void * pParm) )
{
	return EP_SUCCESS;
}

int 
Process::Write( const char* lpszInput )
{
	DWORD dwBytesRead = 0, dwBytesWritten;
	int nLength = strlen(lpszInput);
	char *pszTemp = NULL;
	if (nLength > MAX_PIPE_INPUT_COUNT)
	{
		pszTemp = new char[MAX_PIPE_INPUT_COUNT];
		memcpy_s(pszTemp, MAX_PIPE_INPUT_COUNT, lpszInput, MAX_PIPE_INPUT_COUNT-1);
		pszTemp[MAX_PIPE_INPUT_COUNT-1] = 0;
		nLength = MAX_PIPE_INPUT_COUNT-1;
	}
	WriteFile(m_hInputWritePipe, pszTemp == NULL ? lpszInput : pszTemp, nLength, &dwBytesWritten, NULL);
	if (pszTemp != NULL)
	{
		delete [] pszTemp;
		pszTemp = NULL;
	}
	if (dwBytesWritten == 0)
	{
		return 0;
	}
	return dwBytesWritten;
}

int 
Process::WriteLine( const char* lpszInput )
{
	int bytesWritten = Write(lpszInput);
	bytesWritten += Write("\n");
	return bytesWritten;
}

int 
Process::Read( char* lpszOutput, unsigned int nCapacity /* = 0 */ )
{
	DWORD dwBytesRead;
	PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	int nOffset = 0;
	int MaxBufferSize = 1024;
	TCHAR szTmp[1024] = {0};
	//	std::string strOutput = "";
	while (dwBytesRead > 0)
	{
		ReadFile(m_hOutputReadPipe, szTmp, MaxBufferSize, &dwBytesRead, NULL);
		//偏移量大于缓冲区，那么返回0
		if (nOffset+dwBytesRead > nCapacity) {
			return 0;
		}
		memcpy_s(lpszOutput+nOffset, dwBytesRead, szTmp, dwBytesRead);
		//strOutput += szTmp;
		//memset(szTmp, 0, MaxBufferSize);
		nOffset += dwBytesRead;
		Sleep(10); //睡10毫秒，以让缓冲区就绪
		PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	}
	lpszOutput[nOffset] = 0;
	return nOffset;
	// 	int sizeToCopy = strOutput.size() > nCapacity ? nCapacity : strOutput.size() + 1;
	// 	memcpy(lpszOutput, strOutput.c_str(), sizeToCopy);
	// 	lpszOutput[sizeToCopy] = 0;
	//	return sizeToCopy-1;
}

int 
Process::ReadAll( std::string& strOutput )
{
	DWORD dwBytesRead;
	PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	int MaxBufferSize = 1024;
	TCHAR szTmp[1024+1] = {0};
	while (dwBytesRead > 0)
	{
		ReadFile(m_hOutputReadPipe, szTmp, MaxBufferSize, &dwBytesRead, NULL);
		szTmp[dwBytesRead] = 0;
		strOutput += szTmp;
		memset(szTmp, 0, MaxBufferSize);
		Sleep(10); //睡10毫秒，以让缓冲区就绪
		PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	}
	return strOutput.size();
}

std::tr1::shared_ptr<char>
Process::SafeReadAll(int* lpBytesRead)
{
	using namespace std;
	DWORD dwBytesRead = 0;
	PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	int MaxBufferSize = 1024;
	TCHAR szTmp[1024+1] = {0};
	std::string strBuffer;
	while (dwBytesRead > 0)
	{
		ReadFile(m_hOutputReadPipe, szTmp, MaxBufferSize, &dwBytesRead, NULL);
		szTmp[dwBytesRead] = 0;
#ifdef __DEBUG__
		//std::cout << szTmp << std::endl;
#endif
		//strTemp += szTmp;
		strBuffer += szTmp;
		memset(szTmp, 0, MaxBufferSize);
		if (dwBytesRead < (DWORD)MaxBufferSize)
		{
			break;
		}
		// 		Sleep(50); //睡10毫秒，以让缓冲区就绪
		// 		PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	}
	int count = strBuffer.size();
	char *psz = new char[count+1];
	memcpy_s(psz, count+1, strBuffer.data(), count);
	psz[count] = 0;
	*lpBytesRead = count;
	std::tr1::shared_ptr<char> sptr(psz);
#ifdef __DEBUG__
	//std::cout << strBuffer.data() << std::endl;
#endif

	return sptr;
}

int 
Process::UnSafeReadAll( char** ptr )
{
	using namespace std;

	DWORD dwBytesRead;
	PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	int MaxBufferSize = 1024;
	TCHAR szTmp[1024+1] = {0};
	string strOutput;
	while (dwBytesRead > 0)
	{
		ReadFile(m_hOutputReadPipe, szTmp, MaxBufferSize, &dwBytesRead, NULL);
		szTmp[dwBytesRead] = 0;
		strOutput += szTmp;
		Sleep(10); //睡10毫秒，以让缓冲区就绪
		PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	}
	int count = strOutput.size();
	char * psz = new char[count+1];
	memcpy_s(psz, count+1, strOutput.data(), count);
	psz[count] = 0;
	*ptr = psz;
	return strOutput.size();
}

bool 
Process::UnSafeFreeMemory( char** ptr )
{
	if (ptr != NULL)
		delete [] *ptr;
	return true;
}

int  
Process::ReadLine( char* lpszOutput, unsigned int nCapacity /* = 0 */ )
{
	DWORD dwBytesRead;
	PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	int nOffset = 0;
	int MaxBufferSize = 1024*10;
	TCHAR szTmp[1024*10] = {0};
	while (dwBytesRead > 0)
	{
		ReadFile(m_hOutputReadPipe, szTmp, MaxBufferSize, &dwBytesRead, NULL);
		//偏移量大于缓冲区，那么返回0
		if (nOffset+dwBytesRead > nCapacity) {
			return 0;
		}
		memcpy_s(lpszOutput+nOffset, dwBytesRead, szTmp, dwBytesRead);
		for (size_t i=nOffset; i<nOffset+dwBytesRead; i++)
		{
			if (lpszOutput[i] == '\n')
			{
				nOffset += i;
				goto end;
			}
		}
		nOffset += dwBytesRead;
		Sleep(10); //睡10毫秒，以让缓冲区就绪
		PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	}
end:
	lpszOutput[nOffset] = 0;
	return nOffset;
}

int
Process::Peek()
{
	DWORD dwBytesRead;
	PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	// 	int MaxBufferSize = 1024*10;
	// 	TCHAR szTmp[1024*10] = {0};
	// 	while (dwBytesRead > 0)
	// 	{
	// 		ReadFile(m_hOutputReadPipe, szTmp, MaxBufferSize, &dwBytesRead, NULL);
	// 		strOutput += szTmp;
	// 		Sleep(10); //睡10毫秒，以让缓冲区就绪
	// 		PeekNamedPipe(m_hOutputReadPipe, NULL, NULL, NULL, &dwBytesRead, NULL);
	// 	}
	return dwBytesRead;
}

DWORD 
Process::Wait( DWORD dwMilliseconds /* = INFINITE */ )
{
	return m_bDbg ? WaitDbg(dwMilliseconds, 10) : WaitForSingleObject(m_hProcess, dwMilliseconds);
}

DWORD 
Process::WaitDbg( DWORD dwMillisenconds, DWORD nErrCount )
{
	if (!m_bDbg)
	{
		return Wait(dwMillisenconds);
	}
	DEBUG_EVENT dbe;
	DWORD count = 0;
	while (WaitForDebugEvent(&dbe, dwMillisenconds))
	{   
		if(dbe.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)   
		{
			break;
		}
		else if (dbe.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
		{
			count++;
			if (count > nErrCount)
			{
				break;
			}
		}
		ContinueDebugEvent(dbe.dwProcessId, dbe.dwThreadId, DBG_CONTINUE);   
	}
	//卸载调试器，以释放资源，防止进程杀不死
	DebugActiveProcessStop(m_dwProcessId);
	return WAIT_OBJECT_0;
}

HANDLE 
Process::GetProcessHandle() const
{
	return m_hProcess;
}

HANDLE
Process::GetThreadHandle() const
{
	return m_hThread;
}

DWORD 
Process::GetExitCode() const
{
	return m_dwExitCode;
}

DWORD
Process::GetProcessId() const
{
	return m_dwProcessId;
}

DWORD
Process::GetThreadId() const
{
	return m_dwThreadId;
}

bool 
Process::Destroy()
{
	TerminateProcess(m_hProcess, 0);
	CloseHandle(m_hProcess);
	CloseHandle(m_hThread);
	CloseHandle(m_hInputReadPipe);
	CloseHandle(m_hInputWritePipe);
	CloseHandle(m_hOutputReadPipe);
	CloseHandle(m_hOutputWritePipe);
	m_hProcess         = NULL;
	m_hInputReadPipe   = NULL;
	m_hInputWritePipe  = NULL;
	m_hOutputReadPipe  = NULL;
	m_hOutputWritePipe = NULL;
	return true;
}

NAMESPACE_END