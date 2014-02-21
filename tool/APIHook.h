#pragma once
#include <windows.h>
#include <string>
#include "../pub/cmn.h"

NAMESPACE_BEGIN
//
// 基础类 -> APIHook API拦截类
// Copyright (c) ph0ly 2013.06-2013.07 . All rights reserved.
//
class PH0LY APIHook
{
public:

	// @param lpszModule 模块名称
	// @param lpszCurrentModule 当前模块的名称
	// @param lpszFuncName 要替换的函数名称
	// @param pfnHook 新函数
	// 示例：
	// APIHook g_HookMessageBoxA("user32.dll", "currentdll.dll", "MessageBoxA", (PROC)Hook_MessageBoxA);
	APIHook(LPCSTR lpszModule, LPCSTR lpszCurrentModule, LPCSTR lpszFuncName, PROC pfnHook);

	// 该构造函数会自动寻找 @param lpszFuncName 所在的模块，例如user32.dll
	APIHook(LPCSTR lpszCurrentModule, LPCSTR lpszFuncName, PROC pfnHook);
	~APIHook(void);

	// 替换导入表
	void ReplaceIAT();
	// 恢复导入表
	void RestoreIAT();

private:

	void ReplaceIATInOneMod(PROC pfnOrig, PROC pfnNew);

	PROC m_pfnOrig;
	PROC m_pfnCurrent;

	std::string m_strCurrentModule;

	std::string m_strModule;

};
NAMESPACE_END

