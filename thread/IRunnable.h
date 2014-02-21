
#pragma once
#include "../pub/cmn.h"

NAMESPACE_BEGIN
class IRunnable
{
public:
	virtual uint32 Run(void * pv) = 0;

	virtual uint32 operator () (void * pv) {
		return Run(pv);
	}

};
NAMESPACE_END