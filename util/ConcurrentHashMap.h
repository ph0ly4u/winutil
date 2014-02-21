
#pragma once
#include "../pub/cmn.h"
#include "../util/Delegate.h"
#include "../thread/Mutex.h"
#include <hash_map>

NAMESPACE_BEGIN
/** 
 * \brief Concurrent Hash Map
 */ 
template <class _Key, class _Value> 
class ConcurrentHashMap
{
public:
	typedef typename std::hash_map<_Key, _Value>::iterator HashMapIterator;
	typedef typename std::hash_map<_Key, _Value>::const_iterator HashMapConstIterator;
	ConcurrentHashMap();
	ConcurrentHashMap(const ConcurrentHashMap& rhs);
	~ConcurrentHashMap();

	void Add(const _Key& key, const _Value& val);
	bool Remove(const _Key& key);
	_Value& Get(const _Key& key) throw (std::exception);
	const _Value& Get(const _Key& key) const throw (std::exception);
	void Clear();
	uint32 Size() const;
	_Value& operator [] (const _Key& key);
	const _Value& operator [] (const _Key& key) const;
	HashMapIterator Begin();
	HashMapConstIterator Begin() const;
	HashMapIterator End();
	HashMapConstIterator End() const;
	bool Contains(const _Key& key) const;

private:
	std::hash_map<_Key, _Value> m_hm;
	Mutex m_mutex;
};

template <class _Key, class _Value> 
ConcurrentHashMap<_Key, _Value>::ConcurrentHashMap() 
{}

template <class _Key, class _Value> 
ConcurrentHashMap<_Key, _Value>::ConcurrentHashMap(const ConcurrentHashMap& rhs) 
{}

template <class _Key, class _Value> 
ConcurrentHashMap<_Key, _Value>::~ConcurrentHashMap()
{}

template <class _Key, class _Value> 
void ConcurrentHashMap<_Key, _Value>::Add(const _Key& key, const _Value& val)
{
	m_mutex.Lock();
	m_hm[key] = val;
	m_mutex.UnLock();
}

template <class _Key, class _Value> 
bool ConcurrentHashMap<_Key, _Value>::Remove(const _Key& key)
{
	HashMapIterator iter = m_hm.find(key);
	if ( m_hm.end() == iter )
		return false;
	m_mutex.Lock();
	m_hm.erase(iter);
	m_mutex.UnLock();
	return true;
}

template <class _Key, class _Value> 
inline _Value& ConcurrentHashMap<_Key, _Value>::Get(const _Key& key) throw (std::exception)
{
	HashMapConstIterator iter = m_hm.find(key);
	if ( m_hm.end() == iter )
		throw std::exception("no such key!");
	return m_hm[key];
}

template <class _Key, class _Value> 
const _Value& ConcurrentHashMap<_Key, _Value>::Get(const _Key& key) const  throw (std::exception)
{
	return const_cast<ConcurrentHashMap&>(*this).Get(key);
}

template <class _Key, class _Value> 
void ConcurrentHashMap<_Key, _Value>::Clear()
{
	m_mutex.Lock();
	m_hm.clear();
	m_mutex.UnLock();
}

template <class _Key, class _Value>
bool ConcurrentHashMap<_Key, _Value>::Contains(const _Key& key) const
{
	return m_hm.find(key) != m_hm.end();
}

template <class _Key, class _Value> 
inline uint32 ConcurrentHashMap<_Key, _Value>::Size() const
{
	return m_hm.size();
}

template <class _Key, class _Value> 
inline _Value& ConcurrentHashMap<_Key, _Value>::operator [] (const _Key& key)
{
	m_mutex.Lock();
	_Value& val = m_hm[key];
	m_mutex.UnLock();
	return val;
}

template <class _Key, class _Value> 
inline const _Value& ConcurrentHashMap<_Key, _Value>::operator [] (const _Key& key) const
{
	return const_cast<ConcurrentHashMap&>(*this).operator[](key);
}

template <class _Key, class _Value> 
inline typename ConcurrentHashMap<_Key, _Value>::HashMapIterator ConcurrentHashMap<_Key, _Value>::Begin() 
{
	return m_hm.begin();
}

template <class _Key, class _Value> 
inline typename ConcurrentHashMap<_Key, _Value>::HashMapIterator ConcurrentHashMap<_Key, _Value>::End() 
{
	return m_hm.end();
}

template <class _Key, class _Value> 
inline typename ConcurrentHashMap<_Key, _Value>::HashMapConstIterator ConcurrentHashMap<_Key, _Value>::Begin() const
{
	return m_hm.end();
}

NAMESPACE_END