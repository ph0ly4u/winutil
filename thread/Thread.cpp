#define DLL_EXPORTS
#include "Thread.h"
#include <process.h>

using namespace std;
NAMESPACE_BEGIN

ConcurrentHashMap<HANDLE, Thread*> Thread::m_hmThreads;

Thread::Thread() : m_bShutdown(false), m_bRun(false), m_hThread(NULL), m_pRun(NULL), m_pSignal(new CountDownLatch(1)) {}
Thread::Thread(IRunnable * ptr) : m_bShutdown(false), m_bRun(false) , m_pRun(ptr), 
	m_hThread(NULL), m_pSignal(new CountDownLatch(1)) {}

Thread::~Thread()
{
	if ( m_hThread )
		Stop();
}

void Thread::Start()
{
	Start(NULL);
}

void Thread::Start(void * pv)
{
	if ( m_pRun == NULL )
		throw std::exception("must set IRunnable!");
	Parameter * pParam = new Parameter;
	pParam->pThread = this;
	pParam->pv = pv;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, pParam, NULL, NULL);
	if ( m_hThread > 0 ) {
		m_hmThreads[m_hThread] = this;
	}
	m_pSignal->CountDown();
	
}

void Thread::Stop()
{
	m_bShutdown = true;
	m_bRun = false;
	CloseHandle(m_hThread);
	m_hThread = NULL;
	if ( m_pSignal )
		delete m_pSignal;
	m_pSignal = NULL;
}

void Thread::Suspend() const
{
	SuspendThread(m_hThread);
}

void Thread::Resume() const
{
	ResumeThread(m_hThread);
}

void Thread::Sleep(uint32 uMilliseconds) const
{
	::Sleep(uMilliseconds);
}

bool Thread::operator == (const Thread& t) const
{
	if ( NULL == m_hThread && NULL == t.m_hThread ) return false;
	return m_hThread == t.m_hThread;
}

/**
 * Dispatch ThreadProc
 */
uint32 STDCALL Thread::ThreadProc(void * pv)
{
	Parameter * pParam = static_cast<Parameter*>(pv);
	pParam->pThread->m_pSignal->Await();
	Thread * _this = pParam->pThread;
	HANDLE hThread = _this->m_hThread;
	if ( !m_hmThreads.Contains(hThread) ) {
		return 0;
	}
	uint32 uRet = (*_this->m_pRun)(pParam->pv);
	_this->Stop();
	delete pParam;
	return uRet;
}

NAMESPACE_END