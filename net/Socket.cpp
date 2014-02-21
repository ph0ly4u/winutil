#include <WinSock2.h>
#define DLL_EXPORTS
#include "Socket.h"

#pragma comment(lib, "ws2_32")
NAMESPACE_BEGIN

bool Socket::m_bFirstTime = true;

Socket::Socket(void) : m_type(TCP)
{
	Init();
}

Socket::Socket(const std::string& ip, size_t port) : SocketBase(ip, port), m_type(TCP)
{
	Init();
}

Socket::Socket(const Endpoint& endpoint) : SocketBase(endpoint), m_type(TCP)
{
	Init();
}

Socket::~Socket(void)
{
	
}

void Socket::Init()
{
	if ( m_bFirstTime ) {
		InitializeComponent();
		m_bFirstTime = false;
	}
}

void Socket::InitializeComponent()
{
	WSADATA wsaData = {0};
	wsaData.wVersion = MAKEWORD(1, 1);
	if (0 != WSAStartup(MAKEWORD(1, 1), &wsaData))
		return;
	if ( LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1 ) {
		CleanupComponent();
		return;
	}
}

void Socket::CleanupComponent()
{
	WSACleanup();
}

bool Socket::Create(const std::string& ip, size_t port, SockType type)
{
	m_type = type;
	m_endpoint.ip = ip;
	m_endpoint.port = port;
	m_nSockId = ::socket(AF_INET, type == TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
	return m_nSockId > 0;
}

bool Socket::Create(const Endpoint& endpoint, SockType type)
{
	m_type = type;
	m_endpoint = endpoint;
	return Create(endpoint.ip, endpoint.port, type);
}

bool Socket::Create(SockType type) throw (SocketException)
{
	if ( m_endpoint.ip.size() == 0 || m_endpoint.port == 0 )
		throw SocketException("not specify Endpoint!");
	return Create(m_endpoint.ip, m_endpoint.port, type);
}

bool Socket::Bind() const throw (SocketException) 
{
	if ( m_endpoint.ip.size() == 0 || m_endpoint.port == 0 )
		throw SocketException("not specify Endpoint!");
	sockaddr_in inaddr = {0};
	inaddr.sin_addr.S_un.S_addr = inet_addr(m_endpoint.ip.data());
	inaddr.sin_family = AF_INET;
	inaddr.sin_port = htons(m_endpoint.port);
	return ::bind(m_nSockId, (sockaddr*)&inaddr, sizeof(sockaddr)) >= 0;
}

bool Socket::Listen(int backlog /* = DEFAULT_BACKLOG */) const
{
	return ::listen(m_nSockId, backlog) > 0;
}

bool Socket::Connect(const Endpoint& endpoint) const
{
	sockaddr_in inaddr = {0};
	inaddr.sin_addr.S_un.S_addr = inet_addr(endpoint.ip.data());
	inaddr.sin_family = AF_INET;
	inaddr.sin_port = htons(endpoint.port);
	return ::connect(m_nSockId, (sockaddr*)&inaddr, sizeof(sockaddr)) == 0;
}

SocketBase Socket::Accept() const throw (SocketException)
{
	sockaddr_in addrin = {0};
	int addrlen = sizeof(sockaddr);
	int newId = ::accept(m_nSockId, (sockaddr*)&addrin, &addrlen);
	SocketBase base(inet_ntoa(addrin.sin_addr), ntohs(addrin.sin_port));
	base.Assign(newId);
	return base;
}

SocketBase::SocketBase(void) : m_nSockId(0), m_uRef(1) {}

SocketBase::SocketBase(const std::string& ip, size_t port) : m_endpoint(ip, port), m_nSockId(0), m_uRef(1) {}

SocketBase::SocketBase(const Endpoint& endpoint) : m_endpoint(endpoint), m_nSockId(0), m_uRef(1) {}

SocketBase::SocketBase(const SocketBase& h) : m_uRef(h.m_uRef) 
{ 
	h._Inc();
	m_endpoint = h.m_endpoint;
	m_nSockId = h.m_nSockId;
}

SocketBase::~SocketBase()
{
	_Def();
}

const SocketBase& SocketBase::operator = (const SocketBase& h)
{
	h._Inc();
	m_endpoint = h.m_endpoint;
	m_nSockId = h.m_nSockId;
	return *this;
}

void SocketBase::_Inc() const
{
	m_uRef++;
}

void SocketBase::_Def() const
{
	m_uRef--;
	if ( m_uRef <= 0 )
		Close();
}

void SocketBase::Close() const
{
	::shutdown(m_nSockId, SD_BOTH);
	::closesocket(m_nSockId);
}

size_t SocketBase::Send(const char * buf, size_t len) const
{
	return ::send(m_nSockId, buf, len, 0);
}

size_t SocketBase::Recv(char * buf, size_t bufLen) const
{
	return ::recv(m_nSockId, buf, bufLen, 0);
}


NAMESPACE_END