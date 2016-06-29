#include "ClientConTextIntf.h"
#include "ProtocolTools.h"
#include "os_ports.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


/*
#include "LuaBridge.h"
#include "RefCountedObject.h"
#include "RefCountedPtr.h"
using namespace luabridge;
*/

using namespace MOONLIB;
using namespace ProtocolTools;
using namespace NetworkProtocol;

//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
MOONLIB::CriticalLock		gLock;
static lua_State *gL_netpro = NULL;

//---------------------------------------------------------------------

int Start( lua_State* L )
{
	int port = luaL_checkinteger(L, 1);
	if (port < 0)
	{
		MERROR("func ServerStart : port < 0!");
		lua_pushnil(L);
		return 1;
	}

	DEBUG("start=============>");
	
	int st = CreateServerSocket(port);//init socket
	if (st == -1)
	{
		MERROR("func ServerStart : createServerSocket failed!");
		lua_pushnil(L);
		return 1;
	}

	lua_pushinteger(L, st);
	
	return 1;
}

//---------------------------------------------------------------------

void *LuaControl(void *arg)
{
	DEBUG("new pthread start===>");

	int st = (int)arg;

	SetNonblocking(st);//set non-blocking socket
	//MySignal(SIGPIPE, CatchSignal);//catch SIGPIPE signal

	char recvBuf[MAX_PACKET_SIZE];
	memset(recvBuf, 0, MAX_PACKET_SIZE);
	fd_set fds;
	int maxfd = -1;//max fd in fds
	int sockid;//temp socket variable

	while (1)
	{
		//init select pool and get maxfd
		FD_ZERO(&fds);
		FD_SET(st, &fds);
		maxfd = st;

		ClientsList::iterator ptr = g_clients.begin();		//map maintain select pool
		while ( ptr != g_clients.end() )
		{
			FD_SET(ptr->first, &fds);
	
			if (ptr->first > maxfd)
				maxfd = ptr->first;

			ptr++;
		}

		int rc = select(maxfd + 1, &fds, NULL, NULL, NULL);
		
		//--------------select deal with client connection--------------
		if ( FD_ISSET(st, &fds) )
		{
			DEBUG("connect a client");
			sockid = SocketAccept(st);
			if (sockid >= 0)
			{
				if ( (g_clients.size() + 1) == FD_MAX )
				{
					close(sockid);
					MERROR("Warning : select pool no more than 1024!");
				}
				else
				{
					SetNonblocking(sockid);
					FD_SET(sockid, &fds);
				
					ClientConText *ctx = new ClientConText(sockid);
					if (ctx == NULL)
					{
						MERROR("func RegisterControlCallbacks : new ClientConText failed, maybe not has memory");
						return NULL;
					}

					g_clients[sockid] = ctx;
				}
				
				DEBUG("g_clients size is %ld", g_clients.size());
			}
			
			rc--;
			if (rc == 0)
				continue;
		}
		
		//--------------select deal with recv client data--------------
		ptr = g_clients.begin();
		while ( ptr != g_clients.end() )
		{
			if ( FD_ISSET( ptr->first, &fds ) )
			{
				int recvSize = recv(ptr->first, recvBuf, MAX_PACKET_SIZE, 0);
				if (recvSize > 0)
				{
					DEBUG("start push stream");
					int ret = ptr->second->PushData(recvBuf, recvSize);
					if (ret == -2)	//format error and recvSize > jsonSize
					{
						rc--;
						if (rc == 0)
							break;
		
						continue;
					}

					if ( ptr->second->IsCompleted() )				
					{
						const char *jsonBuf = ptr->second->GetData() + ptr->second->GetProtoSize();
						gLock.Lock();
						//pthread_mutex_lock(&mutex);
						if (gL_netpro != NULL)				
							CarryCallbackfunc( gL_netpro, jsonBuf, ptr->second->GetJsonSize() );
						//pthread_mutex_unlock(&mutex);
						gLock.Unlock();
						ptr->second->Reset();
					}
				}
				else
				{
					if (recvSize == 0)
						printf("client disconnect\n");
					else
						MERROR("func RegisterControlCallbacks : recv failed : %s\n", strerror(errno));
					 
						close(ptr->first);
						if (ptr->second != NULL)
							delete ptr->second;
						ClientsList::iterator nptr = ++ptr;
						g_clients.erase(--ptr);
						
						rc--;
						if (rc == 0)
							break;

						ptr = nptr;
						continue;
				}
				rc--;
			}
			ptr++;
		}
	}

	return NULL;
}


int RegisterControlCallbacks( lua_State *L )
{
	DEBUG("prepare create pthread===>");
	int st = luaL_checkinteger(L, 1);
	DEBUG("st = %d\n", st);

	//lua_remove(L, 1);
	gLock.Lock();
	//pthread_mutex_lock(&mutex);
	if ( !gL_netpro && NULL != L )
	{
		gL_netpro = lua_newthread(L);
		//lua_xmove(L, gL_netpro, 1);
		lua_setglobal(gL_netpro, "_Network_protocol_Thread_");

		pthread_t thrd;

		int ret = pthread_create(&thrd, NULL, LuaControl, (void *)st);
		if (ret != 0)
		{
			MERROR("func RegisterControlCallbacks : create pthread error!");
			lua_pushnil(L);
			return 1;
		}
	
		pthread_detach(thrd);
	}
	//pthread_mutex_unlock(&mutex);
	gLock.Unlock();

	lua_pushstring(L, "OK");
	return 1;
}


//---------------------------------------------------------------------


#ifdef __cplusplus
extern "C" {
#endif

static const struct luaL_Reg networkProtocol[] = {
	{"Start", Start},
	{"RegisterControlCallbacks", RegisterControlCallbacks},
	{NULL, NULL}
};


extern "C" int luaopen_NetworkProtocol( lua_State* L )
{
	luaL_newlib(L, networkProtocol);
	return 1;
}


#ifdef __cplusplus
}
#endif

/*
#ifdef __cplusplus
extern "C" {
#endif

int luaopen_NetworkProtocol(lua_State *L)
{
	//std::cout << "*** Network Protocol module started ***" << std::endl;

	getGlobalNamespace(L)
	  .beginNamespace( "Netpro")
		.addCFunction( "ServerStart", &ServerStart )
		.addCFunction( "Recv", &RegisterControlCallbacks )
	  .endNamespace();
	
	return 0;
		
}

#ifdef __cplusplus
}
#endif
*/







