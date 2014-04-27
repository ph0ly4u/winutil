#define DLL_EXPORTS
#include "SocketException.h"

NAMESPACE_PH0LY_BEGIN(net)
SocketException::SocketException(void)
{
}

SocketException::SocketException(const char * const & pMsg) : exception(pMsg)
{

}

SocketException::~SocketException(void)
{
}
NAMESPACE_PH0LY_END