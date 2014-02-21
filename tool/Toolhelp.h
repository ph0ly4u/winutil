#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include "../pub/cmn.h"

NAMESPACE_BEGIN
//
// 基础类 -> Toolhelp 系统快照类
// Copyright (c) ph0ly 2013.05 - 2013.07 . All rights reserved.
//
class PH0LY Toolhelp
{
public:
	Toolhelp(void);
	Toolhelp(DWORD dwFlags, DWORD dwProcessId = NULL);
	~Toolhelp(void);
	// 创建快照
	bool CreateSnapshot(DWORD dwFlags, DWORD dwProcessId);

	// 枚举快照，每次枚举将会导致枚举计数增加1
	// 如果不想往下继续枚举，需要调用Reset重置快照
	// 特别注意：
	// 枚举类型和创建类型不同，将导致Next返回false
	// 请确保创建快照类型和枚举类型一致
	BOOL Next(THREADENTRY32& te);
	BOOL Next(PROCESSENTRY32& pe);
	BOOL Next(MODULEENTRY32& me);
	BOOL Next(HEAPENTRY32& he, DWORD dwProcessID, UINT_PTR dwHeapID);

	// 查找快照
	BOOL Find(DWORD dwThreadId, THREADENTRY32& te) const;
	BOOL Find(DWORD dwProcessId, PROCESSENTRY32& pe) const;
	BOOL Find(LPCSTR pszExeName, PROCESSENTRY32& pe) const;
	BOOL Find(LPCSTR pszModulePath, MODULEENTRY32& me) const;

	// 重置快照
	BOOL Reset();

public:
	static BOOL EnablePrivilege(PCTSTR szPrivilege, BOOL fEnable = TRUE);

private:
	DWORD m_dwFlags;
	HANDLE m_hSnapshot;
	DWORD m_dwRefer;

};

NAMESPACE_END

