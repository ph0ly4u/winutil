
#pragma once

#include "../pub/cmn.h"
#include "Semaphore.h"

NAMESPACE_BEGIN
class PH0LY CountDownLatch
{
public:
	CountDownLatch();
	CountDownLatch(uint32 count);
	~CountDownLatch();
	void CountDown();
	void Await();


private:
	Semaphore * m_pSem;

};

NAMESPACE_END