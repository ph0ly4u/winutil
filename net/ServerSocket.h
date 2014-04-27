#pragma once
#include <hash_map>
#include <hash_set>
#include <string>
#include "Socket.h"

NAMESPACE_PH0LY_BEGIN(net)
/**
 * Copyright (c) ph0ly 2014. All rights reserved.
 */ 
class PH0LY ServerSocket : protected Socket
{
public:
	ServerSocket(void);
	ServerSocket(const std::string& ip, size_t port);
	ServerSocket(const Endpoint& endpoint);
	virtual ~ServerSocket(void);

	using Socket::Create;
	using Socket::Bind;
	using Socket::Listen;
	using Socket::Accept;
	/**
	 * QuickStart includes Create, Bind, Listen.
	 */
	bool QuickStart();
	bool QuickStart(const std::string& ip, size_t port);
	bool QuickStart(const Endpoint& endpoint);
	using Socket::Send;
	using Socket::Recv;

protected:


};
NAMESPACE_PH0LY_END