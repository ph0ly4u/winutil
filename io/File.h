#pragma once
#include <vector>
#include <string>
#include <memory>
#include "../pub/cmn.h"

NAMESPACE_PH0LY_BEGIN(io)
//
// 基础类 -> File 文件类 
// Copyright (c) ph0ly 2013.03.13 . All rights reserved.
// 
class PH0LY File
{
public:
	// FM -> File Mode
	enum Mode
	{
		FM_TEXT = 1,
		FM_BINARY = 2,
		FM_CREATE = 4
	};

	File(void);
	~File(void);
	/**
	 * 文件是否存在
	 * @param lpszFileName 文件路径
	 */
	static bool Exists( const char* lpszFileName );

	/// 删除文件
	static bool Delete( const char* lpszFileName );

	/// 创建文件
	static bool Create( const char* lpszPath );

	/// 获取文件前缀（不包含后缀部分）
	static bool GetFilePrefix( const char* lpszName, char* lpszOut, int nMaxBuffer );

	/// 获取文件名
	static bool GetFileName( const char* lpszName, char* lpszOut, int nMaxBuffer );
	/// 获取文件名
	static bool GetFileNameWithExtension( const char* lpszName, char* lpszOut, int nMaxBuffer );

	/// 是否是文件
	static bool IsFile( const char* lpszPath );
	/// 是否是目录
	static bool IsDirectory( const char* lpszPath );

	/// 获取文件后缀
	static bool GetFileExtension( const char* lpszName, char* lpszOut, int nMaxBuffer );

	/// 获取文件的目录
	static bool GetDirectory( const char* lpszPath, char * lpszOut, int nMaxBuffer );

	/// 查找目录lpszDir中的，以lpszEnds为后缀的文件，并存入vOutFiles中
	static bool FindFiles( const char* lpszDir, const char* lpszEnds, std::vector<std::string>& vOutFiles );

	/// 读取所有文本文件内容 
	static bool ReadAllText( const char* lpszPath, std::string& strOutput );

	/// 保存文本文件
	static bool SaveAllText( const char* lpszPath, const std::string& strInput );
};
NAMESPACE_PH0LY_END

