
#define DLL_EXPORTS
#include "CriticalSection.h"

NAMESPACE_PH0LY_BEGIN(thread)
	
CriticalSection::CriticalSection()
{
	InitializeCriticalSection(&m_cs);
}

CriticalSection::CriticalSection(const CriticalSection& rhs)
{
	InitializeCriticalSection(&m_cs);
}

CriticalSection& CriticalSection::operator = (const CriticalSection& rhs)
{
	DeleteCriticalSection(&m_cs);
	InitializeCriticalSection(&m_cs);

	return *this;
}

CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(&m_cs);
}

void CriticalSection::Enter()
{
	EnterCriticalSection(&m_cs);
}

void CriticalSection::Leave()
{
	LeaveCriticalSection(&m_cs);
}

NAMESPACE_PH0LY_END