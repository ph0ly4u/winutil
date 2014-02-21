
#pragma once

/*
 * Win32 Utility Library
 * Copyright (c) ph0ly 2014. All rights reserved.
 */

#if defined(PH0LY)
#undef PH0LY
#endif
#define PH0LY __declspec(dllimport)

// util
#include "util/Delegate.h"
#include "util/ConcurrentHashMap.h"
#include "util/FastBuffer.h"

// thread
#include "thread/CriticalSection.h"
#include "thread/Mutex.h"
#include "thread/Semaphore.h"
#include "thread/Thread.h"

// net
#include "net/AsyncServer.h"
#include "net/ClientSocket.h"
#include "net/ServerSocket.h"
#include "net/Socket.h"
#include "net/SocketException.h"
