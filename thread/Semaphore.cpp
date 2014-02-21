#define DLL_EXPORTS
#include "Semaphore.h"

NAMESPACE_BEGIN

Semaphore::Semaphore() : m_nCount(1)
{
	m_hSemaphore = CreateSemaphoreW(NULL, m_nCount, m_nCount, NULL);
}

Semaphore::Semaphore(int initsize) : m_nCount(initsize)
{
	m_hSemaphore = CreateSemaphoreW(NULL, initsize, initsize, NULL);
}

Semaphore::Semaphore(int initsize, int maxsize) : m_nCount(initsize), m_nMaxCount(maxsize)
{
	m_hSemaphore = CreateSemaphoreW(NULL, initsize, maxsize, NULL);
}

Semaphore::Semaphore(const Semaphore& rhs)
{
	m_nCount = rhs.m_nCount;
	m_nMaxCount = rhs.m_nMaxCount;
	m_hSemaphore = CreateSemaphoreW(NULL, m_nCount, m_nMaxCount, NULL);
}

Semaphore& Semaphore::operator = (const Semaphore& rhs)
{
	if ( m_hSemaphore )
		CloseHandle(m_hSemaphore);
	m_nCount = rhs.m_nCount;
	m_nMaxCount = rhs.m_nMaxCount;
	m_hSemaphore = CreateSemaphoreW(NULL, m_nCount, m_nMaxCount, NULL);
	return *this;
}

Semaphore::~Semaphore()
{
	CloseHandle(m_hSemaphore);
	m_nCount = 0;
	m_hSemaphore = NULL;
}

void Semaphore::Acquire()
{
	--m_nCount;
	WaitForSingleObject(m_hSemaphore, INFINITE);
}

bool Semaphore::TryAcquire()
{
	if ( m_nCount - 1 >= 0 ) {
		Acquire();
		return true;
	}
	return false;
}

void Semaphore::Release()
{
	LONG lPrevCount;
	ReleaseSemaphore(m_hSemaphore, 1, &lPrevCount);
	++m_nCount;
}

NAMESPACE_END