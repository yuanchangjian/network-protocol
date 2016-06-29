#ifndef __KILOVIEW_NETWORKPROTOCOL_INTERFACE_H__
#define __KILOVIEW_NETWORKPROTOCOL_INTERFACE_H__

#include <string>
#include <map>

namespace NetworkProtocol {

	#define MAX_PACKET_SIZE  	(4*1024)	//4k
	//-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==
	
	class ClientConText {
	public:
	ClientConText(int sockid);
	~ClientConText();

	public:
		int 	PushData( char *buf, int bufSize );

		void 	Reset();

		char* 	GetData() 			{ return m_buffer; }

		int 	GetSocketID()		{ return m_sockid; }

		int		GetProtoSize()		{ return m_protoSize; }

		int		GetJsonSize()		{ return m_jsonSize; }

		bool 	IsCompleted()		{ return m_complete; }

		int		SetProtoSize();

		int		SetJsonSize();

	private:
		int 	m_sockid;
		char 	m_buffer[MAX_PACKET_SIZE];
		int		m_receivedSize;
		int		m_jsonSize;
		int		m_protoSize;
		bool	m_complete;

	};

	//-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==

	typedef std::map<int, ClientConText *>	ClientsList;

	static ClientsList g_clients;

	//-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==-=--=--==
}

#endif

