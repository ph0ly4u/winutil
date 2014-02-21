
#Win32 Utility Library

winutility is a open source win32 utility library, it includes many common class.
such as io, conf, net, thread, process etc. so you can easily use this lib.

#How to use ?

you just compile this in visual studio, suggest vs2010.

in xx.cpp
->#include "/path/winutil.h"
->#pragma comment(lib, "/path/winutil")

The following is a simple C/S demo via wintuil:

client.cpp 
	ClientSocket cltSocket;
	// create this socket
	cltSocket.Create(); 
	// connect to server
	cltSocket.Connect("127.0.0.1", 8888); 
	std::string data = "winutil is so cool!";
	// send data
	cltSocket.Send(data.c_str(), data.size()); 
	char buf[1024] = {0};
	// recv data
	cltSocket.Recv(buf, 1024); 
	cout << buf << endl;
	getchar();

server.cpp
	static uint32 OnRead(void * pSender, void * pObject) {
		AsyncServer * _this = static_cast<AsyncServer*>(pSender);
		SocketBase * pSockBase = static_cast<SocketBase*>(pObject);
		std::string data = "server response!";
		char buf[1024] = {0};
		pSocketBase->Recv(buf, 1024);
		pSockBase->Send(data.data(), data.size());
		return 0;
	}
	// create an async server
	AsyncServer aSvr("127.0.0.1", 8888);
	// register an event handler
	aSvr.RegisterHandler(AsyncServer::OnRead, OnRead);
	// start this server
	aSvr.Start();
	getchar();

#License

Licensed under the MIT 