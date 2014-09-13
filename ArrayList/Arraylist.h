#pragma once
#include <iostream>
#include "IList.h"
#include <vector>

template<class _Myt>
class ArrayList implements IList<_Myt>
{
public:
	ArrayList();
	ArrayList(const ArrayList<_Myt>&);
	virtual ~ArrayList();

public:
	virtual bool add(const _Myt& obj);
	virtual void clear();
	virtual bool remove(int index);
	const _Myt& get(int index) const;
	_Myt& get(int index);
	bool isEmpty();
	bool isEmpty() const;
	virtual size_t size();
	virtual size_t size() const;
	virtual bool setAt(int index, _Myt element);

	virtual const _Myt& operator[] (int index) const;
	virtual _Myt& operator[] (int index);

private:
	inline bool IsValidIndex(int index);
	inline bool IsValidIndex(int index) const;
private:
	std::vector<_Myt> * m_pList;
};

template<class _Myt>
ArrayList<_Myt>::ArrayList(void)
{
	m_pList = new std::vector<_Myt>();
}

template<class _Myt>
ArrayList<_Myt>::ArrayList(const ArrayList<_Myt>& sArrayList)
{
	m_pList = new std::vector<_Myt>(*(sArrayList.m_pList));
}

template<class _Myt>
ArrayList<_Myt>::~ArrayList(void)
{
	delete m_pList;
	m_pList = NULL;
}

template<class _Myt>
bool ArrayList<_Myt>::IsValidIndex(int index)
{
	if (index < 0 || index >= m_pList->size())
	{
		return false;
	}
	return true;	
}

template<class _Myt>
bool ArrayList<_Myt>::IsValidIndex(int index) const
{
	if (index < 0 || index >= m_pList->size())
	{
		return false;
	}
	return true;	
}

template<class _Myt>
bool ArrayList<_Myt>::isEmpty()
{
	return m_pList->empty();
}

template<class _Myt>
bool ArrayList<_Myt>::isEmpty() const
{
	return m_pList->empty();
}


template<class _Myt>
size_t ArrayList<_Myt>::size()
{
	return m_pList->size();
}

template<class _Myt>
size_t ArrayList<_Myt>::size() const
{
	return m_pList->size();
}

template<class _Myt>
_Myt& ArrayList<_Myt>::get(int index)
{
	try
	{
		if (!IsValidIndex(index))
		{
			throw std::exception();
		}
		return m_pList->at(index);
	}
	catch (...)
	{
		std::cerr << "ERROR! Invalid parameter" << std::endl;
	}	
}

template<class _Myt>
const _Myt& ArrayList<_Myt>::get(int index) const
{
	try
	{
		if (!IsValidIndex(index))
		{
			throw std::exception();
		}
		return m_pList->at(index);
	}
	catch (...)
	{
		std::cout << "ERROR! Invalid parameter" << std::endl;
	}	
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
	if (!IsValidIndex(index))
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
	if (!IsValidIndex(index))
	{
		return false;
	}

	m_pList->at(index) = element;

	return true;
}

template<class _Myt>
const _Myt& ArrayList<_Myt>::operator[] (int index) const
{
	try
	{
		if (!IsValidIndex(index))
		{
			throw std::exception();
		}
		return m_pList->at(index);
	}
	catch (...)
	{
		std::cerr << "ERROR! Invalid parameter" << std::endl;
	}	
}

template<class _Myt>
_Myt& ArrayList<_Myt>::operator[] (int index)
{
	try
	{
		if (!IsValidIndex(index))
		{
			throw std::exception();
		}
		return m_pList->at(index);
	}
	catch (...)
	{
		std::cerr << "ERROR! Invalid parameter" << std::endl;
	}	
}