#pragma once
#include "../pub/cmn.h"
#include <string>
#include "SocketException.h"

NAMESPACE_PH0LY_BEGIN(net)
typedef struct PH0LY tagEndpoint
{
	std::string ip;
	size_t port;

	tagEndpoint() : port(0) {}
	tagEndpoint(const std::string& _ip, size_t _port) : 
		ip(_ip), port(_port) {}

} Endpoint;
/**
 * \brief class SocketBase
 * Provide some base operations of Network.
 * SocketBase is based on 'Reference Count', 
 * so you don't worry copy temp obj cause the Socket close.
 */
class PH0LY SocketBase 
{
public:
	SocketBase(void);
	SocketBase(const std::string& ip, size_t port);
	SocketBase(const Endpoint& endpoint);
	SocketBase(const SocketBase& h);
	const SocketBase& operator = (const SocketBase& h);
	virtual ~SocketBase(void);

	void Assign(int sockid) { m_nSockId = sockid; }

	size_t Send(const char * buf, size_t len) const;
	size_t Recv(char * buf, size_t bufLen) const;

	void SetIPAddress(const char * ip) { m_endpoint.ip = ip; }
	void SetPort(size_t port) { m_endpoint.port = port; }
	void SetEndpoint(const Endpoint& endpoint) { m_endpoint = endpoint; }

	void Close() const;

	const Endpoint GetEndpoint() const { return m_endpoint; }
	const char* GetIPAddress() const { return m_endpoint.ip.data(); }
	size_t GetPort() const { return m_endpoint.port; }

	int GetHandle() const { return m_nSockId; }

protected:
	void _Inc() const;
	void _Def() const;
	int m_nSockId;
	Endpoint m_endpoint;
	mutable uint32 m_uRef;

};
/**
 * \brief class Socket
 * implements some common operations of Network.
 */
class PH0LY Socket : public SocketBase
{
public:
	Socket(void);
	Socket(const std::string& ip, size_t port);
	Socket(const Endpoint& endpoint);
	virtual ~Socket(void);
	enum { DEFAULT_BACKLOG = 10 };
	enum SockType { TCP, UDP };

	static void InitializeComponent();
	static void CleanupComponent();

	bool Create(const std::string& ip, size_t port, SockType type = TCP);
	bool Create(const Endpoint& endpoint, SockType type = TCP);
	bool Create(SockType type = TCP) throw (SocketException);
	virtual bool Bind() const throw (SocketException);
	bool Listen(int backlog = DEFAULT_BACKLOG) const;
	SocketBase Accept() const throw (SocketException);

	bool Connect(const Endpoint& endpoint) const throw (SocketException);

	void SetNonblocking(bool bNonblocking = true);
	bool IsValid() const { return m_nSockId > 0; }

	bool IsTCP() { return m_type == TCP; }

protected:
	
	SockType m_type;
private:
	static bool m_bFirstTime;

	static void Init();
};

NAMESPACE_PH0LY_END