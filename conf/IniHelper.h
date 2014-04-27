#pragma once
#include <vector>
#include <string>
#include <map>
#include "../pub/cmn.h"

NAMESPACE_PH0LY_BEGIN(conf)
// 
// 基础类 -> IniHelper INI文件操作类
// Copyright (c) ph0ly 2013.07. All rights reserved.
//
class PH0LY IniHelper
{
public:
	IniHelper(void);
	~IniHelper(void);

	static std::vector<std::string> GetChildren(std::string strPath, std::string strNode);
	// 读取ini文件节点下的子节点
	// 注意：该函数存在缺陷，解析容量有限制，在数据量小时可以选择
	static std::vector<std::string> GetProfileSectionNodes(std::string strPath, std::string strNode);

};
NAMESPACE_PH0LY_END

