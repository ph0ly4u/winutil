#pragma once
#include <string>
#include <windows.h>
#include <memory>
#include "../pub/cmn.h"
NAMESPACE_BEGIN
#define MAXWAITFORPROCESSTIME 1000 * 5  // 最大超时
#define MAXOUTPUTSIZE		  1024 * 5  // 
#define MAX_PIPE_INPUT_COUNT  1024 * 20 // 最大20K管道输入数据量，防止超过最大管道数据量导致，WriteFile阻塞无法返回

//
// 基础类 -> Process 进程类
// Copyright(c) ph0ly 2013.03 - 2013.07. All rights reserved.
//

class PH0LY Process
{
public:

	enum ProcessReturns
	{
		EP_SUCCESS = 0,
		EP_WAITTIMEOUT,
		EP_CREATEFAILED,
		EP_CREATEPIPEFAILED,
		EP_SETINPUTERROR
	};

	Process(void);
	~Process(void);

	// -> 创建进程相关操作

	/// 创建阻塞模式进程，并立启用
	ProcessReturns Create( const char* lpszCmdlines, bool bShowWnd = false );
	/// 创建调试进程，与WaitDbg配套使用
	ProcessReturns CreateDbg( const char* lpszCmdlines );
	/// 创建挂起进程
	ProcessReturns CreateSuspend( const char* lpszCmdlines, bool bDebug = true );
	/// 创建命令行进程
	ProcessReturns CreateCommand( bool bShowWnd = false );
	/// 创建非阻塞模式进程，并立即启用
	/// 参数pOnFinished为回调函数，指示进程创建完毕后应该做什么
	/// 请注意：函数尚未实现
	ProcessReturns CreateAsync( const char* lpszCmdlines, void (*pOnFinished)(void * pParm) );

	// -> 数据写入的相关操作

	/// 写入数据, 返回写的数据数量
	int  Write( const char* lpszInput );
	/// 写入一行数据, 返回写的数据数量
	int  WriteLine( const char* lpszInput );

	/// 读取数据, 返回读的数据数量
	int  Read( char* lpszOutput, unsigned int nCapacity = 0 );
	/// 读取所有数据，返回读取的数量
	/// 这个函数用来解决输出数据量很大的情况
	int  ReadAll( std::string& strOutput );
	/// 读一行, 返回读的数据数量
	int  ReadLine( char* lpszOutput, unsigned int nCapacity = 0 );

	// -> 下面安全与非安全读取，是为了运行exe程序时，能正确捕获输出的未知大小数据量
	// -> 于是产生了所谓的安全和非安全

	/// 安全读取数据，不用手动释放
	/// shared_ptr即智能指针，内存会被自动回收，不用程序员手动释放
	std::tr1::shared_ptr<char> SafeReadAll( int* lpBytesRead );
	/// 不安全读取数据，也就是内部使用堆进行内存分配，需要手动释放
	/// 和UnSafeFreeMemory配套使用
	int  UnSafeReadAll( char** ptr );
	/// 释放不安全内存
	bool UnSafeFreeMemory( char** ptr );

	/// 只查询数据量，不读取，返回数据量
	int  Peek();

	/// 等待进程
	DWORD Wait( DWORD dwMillisenconds = INFINITE );
	/// 等待调试进程退出
	/// dwMillisenconds : 最大等待时间
	/// nErrCount : 异常次数，超过这个数将导致退出调试
	DWORD WaitDbg( DWORD dwMillisenconds, DWORD nErrCount );

	/// 销毁进程
	bool  Destroy();
	/// 获取进程句柄
	HANDLE GetProcessHandle() const;
	/// 获取线程句柄 
	HANDLE GetThreadHandle() const;
	/// 获取退出码
	DWORD  GetExitCode() const;
	/// 获取进程号
	DWORD GetProcessId() const;
	/// 获取主线程号
	DWORD GetThreadId() const;

private:
	HANDLE m_hProcess;
	HANDLE m_hThread;
	HANDLE m_hInputReadPipe;
	HANDLE m_hInputWritePipe;
	HANDLE m_hOutputReadPipe;
	HANDLE m_hOutputWritePipe;

	DWORD m_dwProcessId;
	DWORD m_dwThreadId;

	bool m_bDbg;

	DWORD m_dwExitCode;
};
NAMESPACE_END