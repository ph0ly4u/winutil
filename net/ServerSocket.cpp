#define DLL_EXPORTS
#include <WinSock2.h>
#include "ServerSocket.h"

NAMESPACE_BEGIN

#define PASSERT(x) if ( !(x) ) return false;
ServerSocket::ServerSocket(void)
{
}

ServerSocket::ServerSocket(const std::string& ip, size_t port) : Socket(ip, port)
{

}

ServerSocket::ServerSocket(const Endpoint& endpoint) : Socket(endpoint)
{

}

ServerSocket::~ServerSocket(void)
{
}

bool ServerSocket::QuickStart()
{
	return QuickStart(m_endpoint);
}

bool ServerSocket::QuickStart(const std::string& ip, size_t port)
{
	PASSERT(Create(ip, port));
	PASSERT(Bind());
	PASSERT(Listen());

	return true;
}

bool ServerSocket::QuickStart(const Endpoint& endpoint)
{
	return QuickStart(endpoint.ip, endpoint.port);
}

// bool ServerSocket::Destroy() const
// {
// 	::shutdown(m_nSockId, SD_BOTH);
// 	::closesocket(m_nSockId);
// 	return true;
// }

NAMESPACE_END