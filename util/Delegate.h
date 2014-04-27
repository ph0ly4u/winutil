#pragma once

#include "../pub/cmn.h"
#include <hash_set>
#include <algorithm>

NAMESPACE_PH0LY_BEGIN(util)
template <class Fn>
class Delegate
{
public:
	Delegate(void) : m_pFn(NULL) {}
	Delegate(Fn pFn) : m_pFn(pFn) {}
	virtual ~Delegate() {}
	virtual void operator += (Fn pFn);

protected:
	Fn m_pFn;

};

template <class Fn>
void Delegate<Fn>::operator += (Fn pFn) 
{
	m_pFn = pFn;
}

typedef unsigned int (*FnType)(void * pSender, void * pObject);
/**
 * \brief event delegate
 * Copyright (c) ph0ly 2014. All rights reserved.
 */
class PH0LY EventSource : public Delegate<FnType>
{
public:
	EventSource(void) {}
	EventSource(FnType pFn);
	virtual ~EventSource(void) {}
	/**
	 * \brief Add new fn to this event
	 */ 
	void operator += (FnType pFn);
	/**
	 * \brief Remove the fn from this event
	 */ 
	void operator -= (FnType pFn);

	unsigned int operator() (void * pSender, void * pObject);

	void Clean();

private:
	std::hash_set<FnType> m_hsCallbacks;
};

NAMESPACE_PH0LY_END