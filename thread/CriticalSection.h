#pragma once

#include "../pub/cmn.h"

NAMESPACE_PH0LY_BEGIN(thread)
class PH0LY CriticalSection
{
public:
	CriticalSection();
	~CriticalSection();
	CriticalSection(const CriticalSection&);
	CriticalSection& operator = (const CriticalSection& rhs);

	void Enter();
	void Leave();

private:
	CRITICAL_SECTION m_cs;
	
};

NAMESPACE_PH0LY_END