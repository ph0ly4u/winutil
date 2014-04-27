#define DLL_EXPORTS
#include "Mutex.h"

NAMESPACE_PH0LY_BEGIN(thread)

Mutex::Mutex() : m_hMutex(NULL), m_bSignal(false)
{
	 m_hMutex = CreateMutexW(NULL, FALSE, NULL);
}

Mutex::Mutex(bool bSignal) : m_hMutex(NULL), m_bSignal(bSignal)
{
	m_hMutex = CreateMutexW(NULL, bSignal, NULL);
}

Mutex::Mutex(const Mutex& rhs) : m_bSignal(rhs.m_bSignal)
{
	m_hMutex = CreateMutexW(NULL, m_bSignal, NULL);
}

Mutex& Mutex::operator = (const Mutex& rhs) 
{
	if ( m_hMutex )
		CloseHandle(m_hMutex);
	m_bSignal = rhs.m_bSignal;
	m_hMutex = CreateMutexW(NULL, m_bSignal, NULL);
	return *this;
}

Mutex::~Mutex()
{
	CloseHandle(m_hMutex);
	m_hMutex = NULL;
}

void Mutex::SetSignal(bool bSignal /* = true */)
{
	if (bSignal)
		UnLock();
	else
		Lock();
}

void Mutex::Lock()
{
	WaitForSingleObject(m_hMutex, INFINITE);
}

void Mutex::UnLock()
{
	ReleaseMutex(m_hMutex);
}

NAMESPACE_PH0LY_END