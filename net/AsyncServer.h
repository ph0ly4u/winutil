#pragma once
#include "ServerSocket.h"
#include "../thread/Thread.h"
#include "../thread/Mutex.h"
#include <vector>
#include <list>

NAMESPACE_BEGIN
class EventSource;
/**
 * \brief class AsyncServer
 * I/O multiplexing, use WSAEventSelect implements AsyncServer
 */
class PH0LY AsyncServer : protected ServerSocket
{
public:
	enum AsyncEvent { OnAccept, OnRead, OnSend, OnClose };

	AsyncServer(void);
	AsyncServer(const std::string& ip, size_t port);
	AsyncServer(const Endpoint& endpoint);
	virtual ~AsyncServer(void);

	using ServerSocket::Create;
	/**
	 * \brief Register Event Handler
	 * tips : EventSource can register some fns to itself
	 */
	void RegisterHandler(AsyncEvent event, const EventSource& handler);
	/**
	 * \brief Start this server
	 */
	void Start();
	/**
	 * \brief Stop this server
	 */
	void Stop();

private:
	uint32 DispatchEvent(void * pv);

	std::hash_map<AsyncEvent, EventSource> m_hmEventHandlers;
	std::hash_map<SOCKET, SocketBase> m_hmSocketBases;
	
	bool m_bShutdown;
	Thread * m_pThread;
	Mutex m_mutex;

	class Processor implements IRunnable
	{
	public:
		virtual uint32 Run(void * pv) ;
	} m_processor;

private:
	typedef std::list<std::vector<HANDLE>*>::iterator HandleIterator;
	typedef std::list<std::vector<SOCKET>*>::iterator SocketIterator;
	typedef std::hash_map<AsyncEvent, EventSource>::iterator HashMapIterator;
	std::list<std::vector<HANDLE>*> m_hPtrs;
	std::list<std::vector<SOCKET>*> m_sPtrs;
	AsyncServer(const AsyncServer& rhs);
	AsyncServer& operator = (const AsyncServer& rhs);

};

NAMESPACE_END