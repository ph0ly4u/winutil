#pragma once
#include <iostream>
#include "IList.h"
#include <vector>

template<class _Myt>
class ArrayList implements IList<_Myt>
{
public:
	ArrayList();
	virtual ~ArrayList();
public:
	virtual bool add(const _Myt& obj);
	virtual void clear();
	virtual bool remove(int index);
	const _Myt& get(int index) const;
	_Myt& get(int index);
	bool isEmpty();
	virtual size_t size();
	virtual bool setAt(int index, _Myt element);

	virtual const _Myt& operator[] (int index) const;
	virtual _Myt& operator[] (int index);

private:
	std::vector<_Myt> * m_pList;
};

template<class _Myt>
ArrayList<_Myt>::ArrayList(void)
{
	m_pList = new std::vector<_Myt>();
	m_pList->clear();
}

template<class _Myt>
ArrayList<_Myt>::~ArrayList(void)
{
	delete[] m_pList;
	m_pList = NULL;
}

template<class _Myt>
bool ArrayList<_Myt>::isEmpty()
{
	return m_pList->empty();
}

template<class _Myt>
size_t ArrayList<_Myt>::size()
{
	return m_pList->size();
}

template<class _Myt>
_Myt& ArrayList<_Myt>::get(int index)
{
	return m_pList->at(index);
}

template<class _Myt>
const _Myt& ArrayList<_Myt>::get(int index) const
{
	if (index < 0 || index >= m_pList->size())
	{
		return NULL;
	}
	return m_pList->at(index);
}

template<class _Myt>
bool ArrayList<_Myt>::add(const _Myt& obj)
{
	m_pList->push_back(obj);
	return true;
}

template<class _Myt>
bool ArrayList<_Myt>::remove(int index)
{
	if (index < 0 || index > m_pList->size())
	{
		return false;
	}

	if (m_pList->size() - index - 1 > 0)
	{
		memcpy(&m_pList->at(index), &m_pList->at(index + 1), (m_pList->size() - index - 1) * sizeof(_Myt));
	}

	return true;
}

template<class _Myt>
void ArrayList<_Myt>::clear()
{
	m_pList->clear();
}


template<class _Myt>
bool ArrayList<_Myt>::setAt(int index, _Myt element)
{
	if (index < 0 || index > m_pList->size())
	{
		return false;
	}

	m_pList->at(index) = element;

	return true;
}

template<class _Myt>
const _Myt& ArrayList<_Myt>::operator[] (int index) const
{
	return m_pList->at(index);
}

template<class _Myt>
_Myt& ArrayList<_Myt>::operator[] (int index)
{
	return m_pList->at(index);
}