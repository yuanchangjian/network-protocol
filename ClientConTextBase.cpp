#include "ClientConTextIntf.h"
#include "ProtocolTools.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace NetworkProtocol;

//---------------------------------------------------------------------

/*	
*  procotol : CONTROL
*  version : 1.0
*  format :
*		    CONTROL/1.0
*			Content-length:150
*
*			...(json)
*  purpose : In order to ensure recevive the complete data
*/

ClientConText::ClientConText(int sockid) :
		m_sockid(sockid),
		m_receivedSize(0),
		m_jsonSize(0),
		m_protoSize(0),	
		m_complete(false)
{
	memset(m_buffer, 0, MAX_PACKET_SIZE);
}


ClientConText::~ClientConText() 
{
}


int ClientConText::PushData( char *buf, int bufSize )
{
	int remainedSize = MAX_PACKET_SIZE - m_receivedSize;
	if (bufSize > remainedSize)
	{
		MERROR("func ClientConText::PushData : Cammand'format error, more than max_size");
		Reset();
		return -2;
	}	

	memcpy(m_buffer + m_receivedSize, buf, bufSize);
	m_receivedSize += bufSize;
	
	DEBUG("func ClientConText::PushData: push data success!");
	DEBUG("buffer:%s", m_buffer);	

	if (SetJsonSize() == -1 || SetProtoSize() == -1)
	{
		MERROR("Warning : func ClientConText::PushData : recv prohead incomplete");
		return -1;//warning,but ignore
	}
	
	if (m_receivedSize - m_protoSize >= m_jsonSize)//judge data integrity
	{	
		if (m_receivedSize - m_protoSize == m_jsonSize)
			m_complete = true;
		else
		{
			MERROR("func ClientConText::PushData : Cammand'format error, maybe contain spacing or line break");
			Reset();
			return -2;
		}
	}

	return 0;
}

void ClientConText::Reset()
{
	m_receivedSize = 0;
	m_jsonSize = 0;
	m_protoSize = 0;
	m_complete = false;
	memset(m_buffer, 0, MAX_PACKET_SIZE);
}

int ClientConText::SetJsonSize()
{
	char temp[1024];
	memset(temp, 0, sizeof(temp));
	char *p1 = m_buffer;
	char *p2 = NULL;
	p1 = strstr(p1, "KRC/1.0");
	if (p1 == NULL)
		return -1;

	p1 = strstr(p1, "\r\n");
	if (p1 == NULL)	
		return -1;

	p1 = strstr(p1, "Content-length");
	if (p1 == NULL)	
		return -1;

	p1 = strchr(p1, ':');
	if (p1 == NULL)	
		return -1;

	p2 = ++p1;
	while (*p1 == ' ')
		p2 = ++p1;

	p2 = strstr(p2, "\r\n");
	if (p2 == NULL)
		return -1;

	while (*(p2 - 1) == ' ')
		p2--;

	memcpy(temp, p1, p2 - p1);

	m_jsonSize = atoi(temp);//json length
	
	DEBUG("jsonsize : %d", m_jsonSize);
	return 0;
}

int ClientConText::SetProtoSize()
{
	char *p1 = m_buffer;
	char *p2 = NULL;

	p2 = strstr(p1, "\r\n");
	if (p2 == NULL)
		return -1;

	p2 = strstr(p1, "\r\n\r\n");
	if (p2 == NULL)
		return -1;

	p2 = p2 + strlen("\r\n\r\n");//point to behind of "\r\n\r\n"

	m_protoSize = p2 - p1;

	DEBUG("protoSize : %d", m_protoSize);//protocol head length
	return 0;
}




