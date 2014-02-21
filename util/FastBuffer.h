
#pragma once
#include "../pub/cmn.h"

NAMESPACE_BEGIN
/**
 * \brief Fast Buffer, based on object
 * Copyright (c) ph0ly 2014. All rights reserved.
 */
template <class _Myt>
class FastBuffer 
{
public:
	FastBuffer(void);
	~FastBuffer(void);
	FastBuffer(const _Myt * pBuf, uint32 length);
	FastBuffer(const FastBuffer& rhs);
	FastBuffer& operator = (const FastBuffer& rhs);
	/**
	 * \brief Assign new value to this buffer
	 * this will delete previous buffer, and copy new to this buffer
	 */
	void Assign(const _Myt * pBuf, uint32 length);
	/**
	 * \brief free this buffer
	 */
	void Release();
	/**
	 * \brief Add a new block to this buffer
	 * you may need Shrink when necessary
	 */
	void Add(const _Myt * pBuf, uint32 length);
	/**
	 * \brief Remove part of the object from this buffer
	 * \param start is the subsript of the first elem
	 * \param end is the subscript+1 of the last elem
	 */
	bool Remove(uint32 start);
	bool Remove(uint32 start, uint32 end);
	/**
	 * \brief Shrink this buffer, 
	 * consider of the efficiency, this buffer will not auto shrink.
	 * invoke Shrink to shrink the buffer when necessary, or
	 * invoke SetAutoShrink to auto shrink itself
	 */
	bool Shrink();
	/**
	 * \brief set auto Shrink this buffer
	 */
	void SetAutoShrink(bool bAutoShrink) {
		m_bAutoShrink = bAutoShrink;
	}
	/**
	 * \brief Set the buffer to zero in bytes
	 */
	void ZeroBuffer();
	/**
	 * \brief Get the start address of the buffer
	 */
	_Myt * Get() {
		return m_pBuf;
	}
	const _Myt * Get() const {
		return m_pBuf;
	}
	uint32 Size() const {
		return m_uLength;
	}
	operator const _Myt* () const {
		return m_pBuf;
	}

private:
	void _Copy(const _Myt* pBuf, uint32 length, uint32 capacity);

	_Myt * m_pBuf;
	uint32 m_uCapacity;
	uint32 m_uLength;
	bool m_bAutoShrink;

};

template <class _Myt>
FastBuffer<_Myt>::FastBuffer() : m_pBuf(NULL), m_uLength(0), m_uCapacity(0), m_bAutoShrink(false) {}

template <class _Myt>
FastBuffer<_Myt>::FastBuffer(const _Myt * pBuf, uint32 length) : m_pBuf(pBuf), m_uLength(length), m_uCapacity(length), m_bAutoShrink(false)
{
	if ( pBuf == m_pBuf )
		return;
	_Copy(pBuf, length, length);
}

template <class _Myt>
FastBuffer<_Myt>::FastBuffer(const FastBuffer& rhs) : m_bAutoShrink(rhs.m_bAutoShrink)
{
	if ( rhs.m_pBuf == m_pBuf )
		return;
	_Copy(rhs.m_pBuf, rhs.m_uLength, rhs.m_uCapacity);
}

template <class _Myt>
FastBuffer<_Myt>::~FastBuffer() 
{
	Release();
}

template <class _Myt>
FastBuffer<_Myt>& FastBuffer<_Myt>::operator = (const FastBuffer& rhs) 
{
	if ( rhs.m_pBuf == m_pBuf )
		return *this;
	m_bAutoShrink = rhs.m_bAutoShrink;
	_Copy(rhs.m_pBuf, rhs.m_uLength, rhs.m_uCapacity);
	return *this;
}

template <class _Myt>
inline void FastBuffer<_Myt>::Assign(const _Myt * pBuf, uint32 length)
{
	if ( rhs.m_pBuf == m_pBuf )
		return;
	Release();
	_Copy(pBuf, length, length);
}

template <class _Myt>
void FastBuffer<_Myt>::Add(const _Myt * pBuf, uint32 length)
{
	if ( m_uLength < m_uLength+length ) {
		_Myt * pMyBuf = new _Myt[m_uLength+length];
		memcpy_s((char*)pMyBuf, (m_uLength+length) * sizeof(_Myt), (char*)m_pBuf, m_uLength * sizeof(_Myt));
		memcpy_s(((char*)pMyBuf)+m_uLength, length * sizeof(_Myt), (char*)pBuf, length * sizeof(_Myt));
		delete [] m_pBuf;
		m_pBuf = pMyBuf;
		m_uLength = m_uLength+length;
		m_uCapacity = m_uLength+length;
	} else {
		memcpy_s(((char*)m_pBuf)+m_uLength, length * sizeof(_Myt), (char*)pBuf, length * sizeof(_Myt));
		m_uLength = m_uLength+length;
	}
	
}

template <class _Myt>
bool FastBuffer<_Myt>::Remove(uint32 start)
{
	return Remove(start, m_uLength);
}

template <class _Myt>
inline bool FastBuffer<_Myt>::Remove(uint32 start, uint32 end)
{
	--end;
	if ( NULL == m_pBuf || start >= end || start >= m_uLength || end >= m_uLength )
		return false;
	memcpy_s((char*)m_pBuf+start, (m_uLength-start) * sizeof(_Myt), (char*)m_pBuf+end, (m_uLength-end) * sizeof(_Myt));
	m_uLength = start + m_uLength - end;
	if ( m_bAutoShrink )
		Shrink();
	return true;
}

template <class _Myt>
bool FastBuffer<_Myt>::Shrink()
{
	if ( m_uLength == m_uCapacity )
		return false;
	_Myt * pMyBuf = new _Myt[m_uLength];
	memcpy_s((char*)pMyBuf, m_uLength * sizeof(_Myt), (char*)m_pBuf, m_uLength * sizeof(_Myt));
	delete [] m_pBuf;
	m_pBuf = pMyBuf;
	m_uCapacity = m_uLength;
	return true;
}

template <class _Myt>
void FastBuffer<_Myt>::ZeroBuffer()
{
	if ( NULL == m_pBuf )
		return;
	memset((char*)m_pBuf, 0, m_uCapacity * sizeof(_Myt));
}

template <class _Myt>
void FastBuffer<_Myt>::Release()
{
	if ( m_pBuf )
		delete [] m_pBuf;
	m_pBuf = NULL;
	m_uLength = 0;
	m_uCapacity = 0;
}

template <class _Myt>
inline void FastBuffer<_Myt>::_Copy(const _Myt* pBuf, uint32 length, uint32 capacity)
{
	m_uLength = length;
	m_uCapacity = capacity;
	m_pBuf = new _Myt[m_uCapacity];
	memcpy_s((char*)m_pBuf, capacity * sizeof(_Myt), (char*)pBuf, m_uLength * sizeof(_Myt));
}

NAMESPACE_END