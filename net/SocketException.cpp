#define DLL_EXPORTS
#include "SocketException.h"

NAMESPACE_BEGIN
SocketException::SocketException(void)
{
}

SocketException::SocketException(const char * const & pMsg) : exception(pMsg)
{

}

SocketException::~SocketException(void)
{
}
NAMESPACE_END