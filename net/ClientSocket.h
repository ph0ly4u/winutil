#pragma once
#include "Socket.h"

NAMESPACE_BEGIN
class PH0LY ClientSocket : private Socket
{
public:
	ClientSocket(void);
// 	ClientSocket(const std::string& ip, size_t port);
// 	ClientSocket(const Endpoint& endpoint);
	virtual ~ClientSocket(void);
	//void Connect(const Endpoint& endpoint);
	//using Socket::Create;
	bool Create(SockType type = TCP);
	using Socket::Connect; //(const Endpoint& endpoint);
	bool Connect(const std::string& ip, size_t port);
	using Socket::Send;
	using Socket::Recv;

protected:


};
NAMESPACE_END