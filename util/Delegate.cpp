#define DLL_EXPORTS
#include "Delegate.h"

NAMESPACE_BEGIN

EventSource::EventSource(FnType pFn) : Delegate(pFn) { m_hsCallbacks.insert(pFn); }

void EventSource::operator += (FnType pFn) 
{
	m_hsCallbacks.insert(pFn);
}

void EventSource::operator -= (FnType pFn) 
{
	using namespace std;
	hash_set<FnType>::iterator iter = find(m_hsCallbacks.begin(), m_hsCallbacks.end(), pFn);
	if ( iter != m_hsCallbacks.end() )
		m_hsCallbacks.erase(iter);
}

unsigned int EventSource::operator() (void * pSender, void * pObject)
{
	using namespace std;
	hash_set<FnType>::iterator beg = m_hsCallbacks.begin();
	hash_set<FnType>::iterator end = m_hsCallbacks.end();
	unsigned int uRet = 0;
	for (; beg != end; ++beg)
	{
		uRet = (*beg)(pSender, pObject);
	}
	return uRet;
}

void EventSource::Clean()
{
	m_hsCallbacks.clear();
}

NAMESPACE_END