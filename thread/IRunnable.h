
#pragma once
#include "../pub/cmn.h"

NAMESPACE_PH0LY_BEGIN(thread)
class IRunnable
{
public:
	virtual uint32 Run(void * pv) = 0;

	virtual uint32 operator () (void * pv) {
		return Run(pv);
	}

};
NAMESPACE_PH0LY_END