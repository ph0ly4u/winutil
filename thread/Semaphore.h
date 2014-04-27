#pragma once

#include "../pub/cmn.h"

NAMESPACE_PH0LY_BEGIN(thread)
class PH0LY Semaphore 
{
public:
	Semaphore();
	Semaphore(int initsize);
	Semaphore(int initsize, int maxsize);
	Semaphore(const Semaphore& rhs);
	Semaphore& operator = (const Semaphore& rhs);
	~Semaphore();

// 	Semaphore& operator ++();
// 	const Semaphore operator ++(int);
// 
// 	Semaphore& operator --();
// 	const Semaphore operator --(int);

// 	Semaphore& Increment();
// 	const Semaphore Decrement();

	void Acquire();
	bool TryAcquire();
	void Release();

private:
	int m_nCount;
	int m_nMaxCount;
	HANDLE m_hSemaphore;

};

NAMESPACE_PH0LY_END