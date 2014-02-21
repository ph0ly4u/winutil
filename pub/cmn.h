
#pragma once
#include <windows.h>
#pragma warning(disable: 4251)
#pragma warning(disable: 4290)
/**
 * base data type
 */
#define byte unsigned char
#define int16 short
#define int32 int
#define int64 long long
#define uint16 unsigned short
#define uint32 unsigned int
#define uint64 unsigned long long


/**
 * namespace ph0ly
 */
#define NAMESPACE_BEGIN namespace ph0ly {
#define NAMESPACE_END	} 

#ifndef DLL_EXPORTS
#define PH0LY __declspec(dllimport)
#else
#define PH0LY __declspec(dllexport)
#endif

/**
 * common macro
 */ 
#define STDCALL	__stdcall

/**
 * 
 */ 
#define implements : public 