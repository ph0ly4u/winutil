#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include "../pub/cmn.h"

NAMESPACE_PH0LY_BEGIN(tool)
//
// 基础类 -> PE PE文件操作类
// Copyright(c) ph0ly 2013.07 . All rights reserved.
//
class PH0LY PE
{
public:

	enum PEType
	{
		DOS_HEADER,
		NT_HEADER,
		SECTIONS_HEADER,
		IMPORT_TABLE,
		EXPORT_TABLE
	};

	PE(void);
	PE(const char * pszFile);
	~PE(void);

	// 从物理磁盘加载PE文件，加载后映射
	bool Load( const char * pszFile );
	// 从内存加载PE文件映像
	bool Load( DWORD dwProcessId );
	// 从物理磁盘懒加载PE文件，加载后不映射
	void LazyLoad( const char * pszFile );

	// 映射PE文件到内存
	bool Map();
		
	// 判断是否为PE文件
	bool IsPE();

	// 只获取导入函数
	bool GetImportFunctions( std::vector<std::string>& );

	// 获取包含导入库下的导入函数
	// 即同时获得库名称和函数名称
	bool GetImportFunctions( std::map<std::string, std::vector<std::string>>& );
	// 获取导出函数
	bool GetExportFunctions( std::vector<std::string>& );
	// 获取节名称
	bool GetSections( std::vector<std::string>& );
	// 获取节数量
	int  GetSections();
	// 获取PE文件入口点
	PVOID GetEntryPoint();

	// RVA 转换为 Raw Address
	PVOID RVAToRA( PVOID pvBase, PVOID pvRVA );

	// 定位PE文件位置
	PVOID Locate( PEType );
	// 定位PE文件位置, pvBase为基地址
	PVOID Locate( PVOID pvBase, PEType );

	// 将内存中的PE文件提取出来
	bool Dump( const char * pszFilePath );

	// 取消PE文件在内存的映射
	bool UnMap();

	operator PVOID();

private:
	virtual PVOID MapViewOfFile( std::string strFile, DWORD dwMapAccess = FILE_MAP_READ );
	virtual bool  UnMapViewOfFile( PVOID pv );
	// 获取相同属性的区域大小
	virtual DWORD GetVMRgn( HANDLE hProcess, PVOID pvBase );
	// RVA 转换为 Raw Address
	PVOID RVAToRA( PVOID pvRVA );
	DWORD RVAToRA( DWORD pvRVA );

private:
	//std::string m_strFile;
	char m_strFile[MAX_PATH];
	HANDLE m_hProcess;
	PVOID m_pvBase;

	DWORD m_dwVMRgn;

	HANDLE m_hFileMapping;
	HANDLE m_hFile;

	bool m_bAsImage;

};

NAMESPACE_PH0LY_END