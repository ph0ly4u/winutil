#include <WinSock2.h>
#define DLL_EXPORTS 
#include "AsyncServer.h"
#include "../util/Delegate.h"

NAMESPACE_BEGIN
AsyncServer::AsyncServer(void) : m_bShutdown(false)
{
	
}

AsyncServer::AsyncServer(const std::string& ip, size_t port) : m_bShutdown(false), ServerSocket(ip, port) {}
AsyncServer::AsyncServer(const Endpoint& endpoint) : m_bShutdown(false), ServerSocket(endpoint) {}

AsyncServer::~AsyncServer(void)
{
	Stop();
}

void AsyncServer::RegisterHandler(AsyncEvent event, const EventSource& handler)
{
	m_hmEventHandlers[event] = handler;
}

void AsyncServer::Start()
{
	ServerSocket::QuickStart();
	if ( m_nSockId == 0 )
		throw std::exception("must create socket");
	WSAEVENT wsaEvent = ::WSACreateEvent();
	WSAEventSelect(m_nSockId, wsaEvent, FD_ACCEPT);
	std::vector<WSAEVENT> * events = new std::vector<WSAEVENT>;
	std::vector<SOCKET> * sockets = new std::vector<SOCKET>;
	events->push_back(wsaEvent);
	sockets->push_back(m_nSockId);
	m_hPtrs.push_back(events);
	m_sPtrs.push_back(sockets);
	m_pThread = new Thread(&m_processor);
	m_pThread->Start(this);
}

void AsyncServer::Stop()
{
	HandleIterator hBeg = m_hPtrs.begin();
	HandleIterator hEnd = m_hPtrs.end();
	SocketIterator sBeg = m_sPtrs.begin();
	for (; hBeg != hEnd; )
	{
		std::vector<WSAEVENT> * events = *hBeg;
		std::vector<SOCKET> * sockets = *sBeg;
		hBeg = m_hPtrs.erase(hBeg);
		sBeg = m_sPtrs.erase(sBeg);
		delete events;
		delete sockets;
	}
	if ( m_pThread ) {
		m_pThread->Stop();
		delete m_pThread;
	}
	m_bShutdown = true;
}

uint32 AsyncServer::DispatchEvent(void * pv)
{
	while (!m_bShutdown)
	{
		HandleIterator hBeg = m_hPtrs.begin();
		HandleIterator hEnd = m_hPtrs.end();
		SocketIterator sBeg = m_sPtrs.begin();
		for (; hBeg != hEnd; )
		{
			std::vector<WSAEVENT> * pEvents = *hBeg;
			std::vector<SOCKET> * pSockets = *sBeg;
			//  WSAWaitForMultipleEvents Max Wait 64 Elements
			DWORD dwStart = ::WSAWaitForMultipleEvents(pEvents->size(), pEvents->data(), FALSE, WSA_INFINITE, FALSE);
			size_t count = pEvents->size();
			for (size_t i=dwStart-WSA_WAIT_EVENT_0; i<count; ++i)
			{
				int ret;
				ret = ::WSAWaitForMultipleEvents(1, pEvents->data()+i, TRUE, 100, FALSE); 
				if( ret == WSA_WAIT_FAILED || ret == WSA_WAIT_TIMEOUT )
				{
					continue;
				}
				WSANETWORKEVENTS ev;
				::WSAEnumNetworkEvents(pSockets->at(i), pEvents->at(i), &ev);
				if( ev.lNetworkEvents & FD_ACCEPT )
				{
					if( ev.iErrorCode[FD_ACCEPT_BIT] == 0 )
					{
						m_mutex.Lock();
						if( pEvents->size() >= WSA_MAXIMUM_WAIT_EVENTS )
						{
							// Add New Block
							std::vector<WSAEVENT> * events = new std::vector<WSAEVENT>;
							std::vector<SOCKET> * sockets = new std::vector<SOCKET>;
							m_hPtrs.push_back(events);
							m_sPtrs.push_back(sockets);
						}
						m_mutex.UnLock();
						sockaddr_in inaddr;
						int addrlen = sizeof(sockaddr);
						SOCKET sNew = ::accept(pSockets->at(i), (sockaddr*)&inaddr, &addrlen);
						WSAEVENT event = ::WSACreateEvent();
						::WSAEventSelect(sNew, event, FD_READ | FD_CLOSE | FD_WRITE); 
						// 添加到表中
						m_mutex.Lock();
						m_hPtrs.back()->push_back(event);
						m_sPtrs.back()->push_back(sNew);
						m_mutex.UnLock();
						SocketBase base(inet_ntoa(inaddr.sin_addr), ntohs(inaddr.sin_port));
						base.Assign(sNew);
						m_hmSocketBases[sNew] = base;
						HashMapIterator iter = m_hmEventHandlers.find(OnAccept);
						if ( iter != m_hmEventHandlers.end())
						{
							iter->second((void*)this, (void*)&base);
						}
					}
				}
				else if(ev.lNetworkEvents & FD_READ)         // 处理FD_READ通知消息  
				{  
					if(ev.iErrorCode[FD_READ_BIT] == 0)  
					{  
						HashMapIterator iter = m_hmEventHandlers.find(OnRead);
						if ( iter != m_hmEventHandlers.end())
						{
							iter->second((void*)this, (void*)&m_hmSocketBases[pSockets->at(i)]);
						}
					}
				}  
				else if(ev.lNetworkEvents & FD_CLOSE)        // 处理FD_CLOSE通知消息
				{  
					if( ev.iErrorCode[FD_CLOSE_BIT ] == 0)
					{  
						::shutdown(pSockets->at(i), SD_BOTH);
						::closesocket(pSockets->at(i));
						HashMapIterator iter = m_hmEventHandlers.find(OnClose);
						if ( iter != m_hmEventHandlers.end())
						{
							iter->second((void*)this, (void*)&m_hmSocketBases[pSockets->at(i)]);
						}
						pSockets->erase(find(pSockets->begin(), pSockets->end(), pSockets->at(i)));
						pEvents->erase(find(pEvents->begin(), pEvents->end(), pEvents->at(i)));
						if ( pSockets->size() == 0 )
						{
							m_mutex.Lock();
							std::vector<WSAEVENT> * events = *hBeg;
							std::vector<SOCKET> * sockets = *sBeg;
							hBeg = m_hPtrs.erase(hBeg);
							sBeg = m_sPtrs.erase(sBeg);
							delete events;
							delete sockets;
							m_mutex.UnLock();
							continue;
						}
					}  
				}  
			}
			++hBeg, ++sBeg;
		}
		
	}
	return 0;
}

uint32 AsyncServer::Processor::Run(void * pv) 
{
	return static_cast<AsyncServer*>(pv)->DispatchEvent(pv);
}

// uint32 STDCALL AsyncServer::ThreadProc(void * pv)
// {
// 	
// }

NAMESPACE_END
