#pragma once

#include "../pub/cmn.h"
#include "../util/Delegate.h"
#include "CriticalSection.h"
NAMESPACE_PH0LY_BEGIN(thread)
/**
 * \brief Mutex impl
 */
class PH0LY Mutex
{
public:
	Mutex();
	Mutex(bool bSignal);
	~Mutex();
	Mutex& operator = (const Mutex& rhs);
	Mutex(const Mutex& );
	void SetSignal(bool bSignal = true);
	/**
	 * \brief lock the mutex
	 */
	void Lock();
	/**
	 * \brief unlock the mutex
	 */
	void UnLock();

private:
	HANDLE m_hMutex;
	bool m_bSignal;
};

NAMESPACE_PH0LY_END