#pragma once
#include <exception>
#include "../pub/cmn.h"

NAMESPACE_BEGIN
class PH0LY SocketException : private std::exception
{
public:
	SocketException(void);
	explicit SocketException(const char * const &);
	virtual ~SocketException(void);

	virtual const char* What() const { return std::exception::what(); }

};

NAMESPACE_END