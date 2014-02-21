#define DLL_EXPORTS
#include "CountDownLatch.h"

NAMESPACE_BEGIN
CountDownLatch::CountDownLatch() : m_pSem(new Semaphore(0, 1))
{
	m_pSem->Acquire();
}

CountDownLatch::CountDownLatch(uint32 count) : m_pSem(new Semaphore(-1 * count, 1))
{
	 
}

CountDownLatch::~CountDownLatch()
{
	if ( m_pSem )
		delete m_pSem;
}

void CountDownLatch::CountDown()
{
	m_pSem->Release();
}

void CountDownLatch::Await()
{
	m_pSem->Acquire();
}

NAMESPACE_END