#define DLL_EXPORTS
#include "ClientSocket.h"

NAMESPACE_BEGIN
ClientSocket::ClientSocket(void)
{
}

// ClientSocket::ClientSocket(const std::string& ip, size_t port) : Socket(ip, port) {}
// 
// ClientSocket::ClientSocket(const Endpoint& endpoint) : Socket(endpoint) {}

ClientSocket::~ClientSocket(void)
{
}

bool ClientSocket::Connect(const std::string& ip, size_t port)
{
	return Socket::Connect(Endpoint(ip, port));
}

bool ClientSocket::Create(SockType type)
{
	return Socket::Create(m_endpoint.ip, m_endpoint.port, type);
}

NAMESPACE_END