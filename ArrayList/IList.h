#pragma once
#include <vector>
#define implements : public 

template<class _Myt>
class IList
{
public:
	virtual bool add(const _Myt& obj) = 0;
	virtual void clear() = 0;
	virtual bool remove(int index) = 0;
	virtual const _Myt& get(int index) const = 0;
	virtual _Myt& get(int index) = 0;
	virtual bool isEmpty() = 0;
	virtual size_t size() = 0;
	virtual bool setAt(int index, _Myt element) = 0;

	virtual const _Myt& operator[] (int index) const = 0;
	virtual _Myt& operator[] (int index) = 0;

protected:
	std::vector<_Myt> m_objs;
};

