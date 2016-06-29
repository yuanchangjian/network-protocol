#ifndef __PROTOCOL_TOOLS_H__
#define __PROTOCOL_TOOLS_H__

#include <pthread.h>
#include <signal.h>
#include "lua.hpp"

//-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==

#define FD_MAX 1024

//#define DEBUG

#ifdef DEBUG
#define DEBUG(format, ...) 		fprintf(stdout, ""format"\n", ##__VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif

#define MERROR(format, ...)		fprintf(stdout, ""format"\n", ##__VA_ARGS__)

namespace ProtocolTools	{

	//-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==

	int 		CreateServerSocket(int port);
	int 		SocketAccept(int st);
	void 		SetNonblocking(int st);
	int 		CarryCallbackfunc(lua_State *L, const char *jsonBuf, int jsonSize);
	int 		MySignal(int signo, void (*func)(int));
	void 		CatchSignal(int Sign);
	int 		GetKeyword(char *buf, char *key);
}

#endif

