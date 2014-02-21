#pragma once

#include "../pub/cmn.h"
#include <hash_map>
#include "../util/ConcurrentHashMap.h"
#include "IRunnable.h"
#include "CountDownLatch.h"
NAMESPACE_BEGIN
/**
 * \brief class Thread
 * you must specify IRunnable object, 
 * generally you must implements fn -> Run of interface IRunnable.
 */
class PH0LY Thread 
{
public:
	Thread();
	Thread(IRunnable * ptr);
	virtual ~Thread();

	void SetRunnable(IRunnable * ptr) {
		m_pRun = ptr;
	}
	void Start();
	void Start(void * pv);
	void Stop();
	void Suspend() const;
	void Resume() const;
	void Sleep(uint32 uMilliseconds) const;

	bool operator == (const Thread& t) const;

	static uint32 GetCurrentThreadCount() {
		return m_hmThreads.Size();
	}

protected:
	struct Parameter
	{
		Thread * pThread;
		void * pv;
	};

private:
	Thread(const Thread& );
	Thread& operator = (const Thread& rhs);

	static uint32 STDCALL ThreadProc(void * pv);

	HANDLE m_hThread;
	bool m_bRun;
	bool m_bShutdown;
	IRunnable * m_pRun;
	CountDownLatch * m_pSignal;

	static ConcurrentHashMap<HANDLE, Thread*> m_hmThreads;

};


NAMESPACE_END