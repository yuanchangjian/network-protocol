#include "ProtocolTools.h"
#include "ClientConTextIntf.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

using namespace ProtocolTools;

//--------------------------------------------------------------------

//create socket
int ProtocolTools::CreateServerSocket(int port)
{
	int st = socket(AF_INET, SOCK_STREAM, 0);//setup tcp protocal
	if (st == -1)
	{
		MERROR("func createServerSocket : socket failed : %s", strerror(errno));
		return -1;
	}

	socklen_t on = 1;
	if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)  //set address reusable
	{
		MERROR("func createServerSocket : setsockopt failed : %s", strerror(errno));
		return -1;
	}
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(st, (struct sockaddr *)&addr, sizeof(addr)) == -1)  //bind process and socket
	{
		MERROR("func createServerSocket : bind failed : %s", strerror(errno));
		return -1;
	}

	if (listen(st, 100) == -1)	//listen socket
	{
		MERROR("func createServerSocket : listen failed : %s", strerror(errno));
		return -1;
	}
	
	return st;
}

//---------------------------------------------------------------------

void ProtocolTools::SetNonblocking(int st)//set non-blocking socket
{
	int opts = fcntl(st, F_GETFL);
	if (opts < 0)
	{
		MERROR("func setNonblocking : failed, %s", strerror(errno));
	}

	opts = opts | O_NONBLOCK;
	if (fcntl(st, F_SETFL, opts) < 0)
	{
		MERROR("func setNonblocking : failed, %s", strerror(errno));
	}
}


//---------------------------------------------------------------------

int ProtocolTools::SocketAccept(int st)
{
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	memset(&clientaddr, 0, sizeof(clientaddr));
	
	int sockid = accept(st, (struct sockaddr *)&clientaddr, &len);
	if (sockid == -1)
	{
		MERROR("SocketAccept failed : %s", strerror(errno));
		return -1;
	}
	
	printf("Socket===============> %d\n", sockid);

	int keepAlive = 1;
	int keepIdle = 30;
	int keepInterval = 10;
	int keepCount = 3;	

	setsockopt(sockid, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
	setsockopt(sockid, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle));
	setsockopt(sockid, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
	setsockopt(sockid, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
	
	printf("accept by %s\n", inet_ntoa(clientaddr.sin_addr));

	return sockid;
}

//---------------------------------------------------------------------

int ProtocolTools::GetKeyword(char *buf, char *key)	//"Command":"key"  -->  key
{
	if (buf == NULL || key == NULL)
	{
		MERROR("Invaild memory(transfer pointer to NULL)");
		return -1;
	}
	
	char *p1 = NULL;
	char *p2 = NULL;
	p1 = strstr(buf, "Command");
	if (p1 == NULL)
	{
		MERROR("not find Command, maybe command error");
		return -1;	
	}

	p1 = strchr(p1, ':');
	p1 = strchr(p1, '\"');
	if (++p1 != NULL)
		while(*p1 == ' ')
			p1++;

	p2 = strchr(p1, '\"');
	if (p2 != NULL)
		while (*(p2 - 1) == ' ')
			p2--;

	strncpy(key, p1, p2 - p1);//copy key value to key

	DEBUG("key:%s", key);	

	return 0;
}

//---------------------------------------------------------------------


int ProtocolTools::CarryCallbackfunc( lua_State *L, const char *jsonBuf, int jsonSize)
{
	lua_getglobal(L, "callbackfunctable");

	DEBUG("start carry callbackfunc");

	char cammand[MAX_PACKET_SIZE];
	memset(cammand, 0, MAX_PACKET_SIZE);
	char key[64];
	memset(key, 0, sizeof(key));

	memcpy(cammand, jsonBuf, jsonSize);

	if ( GetKeyword(cammand, key) != -1 )
	{
		DEBUG("cammand:%s\n", cammand);
		lua_pushstring(L, key);
		lua_gettable(L, -2);
		if (lua_isfunction(L, -1))
		{
			lua_pushstring(L, cammand);//push first param
			bool err = lua_pcall(L, 1, 0, 0);
			if (err)
			{
				MERROR("CarryCallbackfunc : call backfunc error");
				lua_pop(L, 1);//pop the error message
			}
		}
	}

	return 0;//Keep the client connection
}

//---------------------------------------------------------------------

void ProtocolTools::CatchSignal(int Sign)
{
	switch (Sign)
	{
	case SIGPIPE:
		MERROR("signal SIGPIPE\n");
		break;
	}
}

int ProtocolTools::MySignal(int signo, void (*func)(int))
{
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	return sigaction(signo, &act, &oact);
}

//---------------------------------------------------------------------


