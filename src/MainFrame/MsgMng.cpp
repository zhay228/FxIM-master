// IMSocket.cpp: implementation of the CMsgMng class.
//
//////////////////////////////////////////////////////////////////////

#include <time.h>
#include "stdafx.h"
#include "Utils/Config.h"
#include "Utils/BaseDef.h"
#include "MainFrame/MsgMng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMsgMng::CMsgMng()
{
	m_packetNo	= (ULONG)time(NULL);
	m_sockTcp	= INVALID_SOCKET;
	m_sockUdp	= INVALID_SOCKET;
	m_hAsyncWnd = 0;
	
	Config* pCfg = Singleton<Config>::getInstance();
	//////////////////////////////////////////////////////////////////////////
	//��ʼ��
	memset(&m_hostSubLocal, 0, sizeof(HostSub));
	m_hostSubLocal.addr = pCfg->nAddr;
	// Converts a u_short from host to TCP/IP network byte order 
	m_hostSubLocal.portNo = htons(pCfg->nPort);

	DWORD size = sizeof(m_hostSubLocal.hostName);
	::GetComputerName(m_hostSubLocal.hostName, &size);
	
	size = sizeof(m_hostSubLocal.userName);
	::GetUserName(m_hostSubLocal.userName, &size);

	WSADATA wsa;
	WSAStartup(0x0202, &wsa);	
	if (LOBYTE(wsa.wVersion) != 2 || HIBYTE(wsa.wVersion) != 2)
	{
		ShowErrorInfo(WSAGetLastError(), _T("winsock�汾��"));
	}

	// Need sock init.
	if (pCfg->nAddr == INADDR_ANY)       //#define ADDR_ANY INADDR_ANY
	{
		char hostName[MAX_NAMEBUF] = {0};
		if (::gethostname(hostName, sizeof(hostName)) == 0)
		{
			//lstrcpy(m_hostSubLocal.hostName, hostName);
			if (hostent *ent = ::gethostbyname(hostName))
			{
				m_hostSubLocal.addr = *(ULONG*)ent->h_addr_list[0];
			}
		}
	}
}


CMsgMng::~CMsgMng()
{
	WSockTerm();
}

BOOL CMsgMng::WSockInit()
{
	//��ʼ��m_sockUdp , m_sockTcp
	if ((m_sockUdp = ::socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET
		|| (m_sockTcp = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("Please Setup TCP/IP")), FALSE;
	}

	//�󶨶˿�
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = m_hostSubLocal.addr;			//INADDR_ANY
	addr.sin_port = m_hostSubLocal.portNo;				

	if (::bind(m_sockUdp, (SOCKADDR*)&addr, sizeof(addr)) != 0
		|| ::bind(m_sockTcp, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("bind()")), FALSE;//
	}

	// for recvfrom error WSAECONNRESET(10054)
// 	BOOL bNewBehavior = FALSE;
// 	DWORD dwBytesReturned = 0;
// 	WSAIoctl(m_sockUdp, _WSAIOW(IOC_VENDOR,12), &bNewBehavior, sizeof bNewBehavior, NULL, //SIO_UDP_CONNRESET = _WSAIOW(IOC_VENDOR,12)
// 		0, &dwBytesReturned, NULL, NULL);

	//��m_sockTcp / m_sockUdp������Ϊ������ģʽ  ioctlsocket
	BOOL flg = TRUE;
	if (::ioctlsocket(m_sockUdp, FIONBIO, (unsigned long*)&flg) != 0
		||::ioctlsocket(m_sockTcp, FIONBIO, (unsigned long*)&flg) != 0)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("ioctlsocket() FIONBIO")), FALSE;
	}

	//����m_sockUdp���͹㲥 /setsockopt�����׽���ѡ���
	flg = TRUE;
	if (::setsockopt(m_sockUdp, SOL_SOCKET, SO_BROADCAST, (char*)&flg, sizeof(flg)) != 0)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("UDP setsockopt() SO_BROADCAST")), FALSE;
	}
	
	// UDP���ͽ��ջ����С����
	int	bufSize;
	for (bufSize = MAX_SOCKBUF; bufSize > 1; bufSize /= 2)
	{
		//ֱ�����óɹ����ж�ѭ��
		if (::setsockopt(m_sockUdp, SOL_SOCKET, SO_SNDBUF, (char *)&bufSize, sizeof(int)) == 0
			&&::setsockopt(m_sockUdp, SOL_SOCKET, SO_RCVBUF, (char *)&bufSize, sizeof(int)) == 0)
		{
			break;
		}
	}
	
	//���õ�ַ����
	flg = TRUE;
	if (::setsockopt(m_sockTcp, SOL_SOCKET, SO_REUSEADDR, (char*)&flg, sizeof(flg)) != 0)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("TCP setsockopt() SO_REUSEADDR")), FALSE;
	}

	//m_sockTcp��������
	if (IsAvailableTcp() && ::listen(m_sockTcp, 5) != 0)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("TCP listen()")), FALSE;
	}

	return TRUE;
}

//ע���¼�
BOOL CMsgMng::AsyncSelect(HWND hWnd)
{
	if (hWnd)
	{
		m_hAsyncWnd = hWnd;
	}

	if (::WSAAsyncSelect(m_sockUdp, hWnd, WM_UDPEVENT, FD_READ) != 0)
		return FALSE;

	if (::WSAAsyncSelect(m_sockTcp, hWnd, WM_TCPEVENT, FD_ACCEPT| FD_CLOSE) != 0)
		return FALSE;

	return TRUE;
}

BOOL CMsgMng::Accept(HWND hWnd, ConnectInfo *connectInfo)
{
	ASSERT(connectInfo);

	sockaddr_in addr;
	int  size = sizeof(addr);
	if ((connectInfo->sd = ::accept(m_sockTcp, (sockaddr*)&addr, &size)) == INVALID_SOCKET)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("accept")), FALSE;
	}

	BOOL flg = TRUE;
	if (::setsockopt(connectInfo->sd, SOL_SOCKET, TCP_NODELAY, (char*)&flg, sizeof(flg)) != 0)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("setsockopt")), FALSE;
	}

	connectInfo->addr = addr.sin_addr.S_un.S_addr;
	connectInfo->port = addr.sin_port;
	connectInfo->complete = connectInfo->server = TRUE;

	int bufSize = MAX_SOCKBUF;
	for(; bufSize > 1; bufSize /= 2)
	{
		if (::setsockopt(connectInfo->sd, SOL_SOCKET, SO_SNDBUF, (char*)&bufSize, sizeof(bufSize)) == 0)
			break;
	}

	if (AsyncSelectConnect(hWnd, connectInfo))
	{
		connectInfo->startTick = connectInfo->lastTick = ::GetTickCount();
		return	TRUE;
	}

	::closesocket(connectInfo->sd);
	
	return FALSE;
}

BOOL CMsgMng::AsyncSelectConnect(HWND hWnd, ConnectInfo *connectInfo)
{
	if (::WSAAsyncSelect(connectInfo->sd, hWnd, WM_TCPEVENT, 
		(connectInfo->server ? FD_READ : FD_CONNECT)|FD_CLOSE) == SOCKET_ERROR)
	{
		return	FALSE;
	}
	return	TRUE;
}

BOOL CMsgMng::Connect(HWND hWnd, ConnectInfo *connectInfo)
{
	connectInfo->server = FALSE;
	if ((connectInfo->sd = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("socket")), FALSE;
	}

	BOOL flg = TRUE;   //non block
	if (::ioctlsocket(connectInfo->sd, FIONBIO, (u_long*)&flg) != 0)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("ioctlsocket")), FALSE;
	}

	int bufSize = MAX_SOCKBUF;    //tcp RcvBuf
	for (; bufSize > 1; bufSize /= 2)
	{
		if (::setsockopt(connectInfo->sd, SOL_SOCKET, SO_RCVBUF, (char*)&bufSize, 
			sizeof(bufSize)) == 0)
		{
			break;
		}
	}

	if (AsyncSelectConnect(hWnd, connectInfo))
	{
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = connectInfo->addr;
		addr.sin_port = connectInfo->port;
		if ((connectInfo->complete = ::connect(connectInfo->sd, (sockaddr*)&addr, sizeof(addr))) == 0
			|| ::WSAGetLastError() == WSAEWOULDBLOCK)   //WSAEWOULDBLOCK �Է��ѹر�
		{
			connectInfo->lastTick = connectInfo->startTick = ::GetTickCount();
			return TRUE;
		}
	}
	::closesocket(connectInfo->sd);
	
	return FALSE;
}

void CMsgMng::ConnectDone(HWND hWnd, ConnectInfo *connectInfo)
{
	::WSAAsyncSelect(connectInfo->sd, hWnd, 0, 0);
	
	BOOL flg = FALSE;
	::ioctlsocket(connectInfo->sd, FIONBIO, (u_long*)&flg);
}

void CMsgMng::WSockTerm(void)
{
	if (m_sockTcp != INVALID_SOCKET)
	{
		closesocket(m_sockTcp);
	}
	if (m_sockUdp != INVALID_SOCKET)
	{
		closesocket(m_sockUdp);		
	}

	m_sockTcp = m_sockUdp = INVALID_SOCKET;
	WSACleanup();
}

//������Ϣ�ܳ���
int  CMsgMng::MakeMsg(char *dest, int packetNo, ULONG command, 
						const char *msg /*= NULL*/, const char *exMsg /*= NULL*/)
{
	ASSERT(dest);
	*dest = 0;

	int		len = 0;
	int		ex_len = 0;
	int		max_len = MAX_UDPBUF;
	
	len = wsprintf(dest, "%d:%ld:%s:%s:%ld:", IM_VERSION, packetNo,	//Ĭ���ǰ�����Լ�
		m_hostSubLocal.userName, m_hostSubLocal.hostName, command);

	max_len -= ex_len;

	//��msg�е�'\r'ȥ���������ȥ������ɸ�ͨ��ʱ���ɸ�Ὣ'\n\r'���'\n\r\r'
	if (msg != NULL) {
		len += LocalNewLineToUnix(dest + len, msg, max_len - len);			
	}

	len++;  //for '\0'

	if (exMsg != NULL)
	{
		ex_len = strlen(exMsg) + 1;
		if (ex_len + len + 1 >= MAX_UDPBUF)   // +1 for '\0' of exMsg
		{
			len = MAX_UDPBUF - ex_len - 1;  //��Դͷ�Ͻ�������������Ϣ�ռ䲻������ռ����Ϣ�Ŀռ䣬�����������һ�㲻�ᷢ��
			dest[len - 1] = 0;            //��Ϣ�븽����Ϣ֮������ӽ�����'\0'
		}
		memcpy(dest + len, exMsg, ex_len);
		len += ex_len;
	}
	return len;
}


int CMsgMng::MakeMsg(char *dest, ULONG command, const char *msg/* = NULL*/, const char *exMsg/* = NULL*/)
{
	return MakeMsg(dest, MakePacketNo(), command, msg, exMsg);
}

BOOL CMsgMng::UdpSend(ULONG addr, USHORT portNo, const char *udpMsg, int len)
{
	ASSERT(udpMsg);
	sockaddr_in addr_in;

	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.S_un.S_addr = addr;
	addr_in.sin_port = portNo;

	//���и�����Ϣʱ����Ϣ�븽����Ϣ֮�����ַ�0(������)���ʴ�ʱlen != strlen(udpMsg) + 1
	if (::sendto(m_sockUdp, udpMsg, len, 0, (sockaddr*)&addr_in, 
		sizeof(addr_in)) == SOCKET_ERROR)
	{
		return ShowErrorInfo(WSAGetLastError(), _T("UDP send")), FALSE;
	}
	return TRUE;
}

BOOL CMsgMng::UdpSend(HostSub *hostSub, const char* udpMsg, int len)
{
	return UdpSend(hostSub->addr, hostSub->portNo, udpMsg, len);
}

BOOL CMsgMng::Send(ULONG addr, USHORT portNo, ULONG command, const char *msg /* = NULL */, 
					 const char *exMsg /* = NULL */)
{
	int	 len;
	char szSendBuf[MAX_UDPBUF];

	len = MakeMsg(szSendBuf, command, msg, exMsg);
	return CMsgMng::UdpSend(addr, portNo, szSendBuf, len);
}

BOOL CMsgMng::Send(HostSub *hostSub, ULONG command, const char *msg /* = NULL */,
					 const char *exMsg /* = NULL */)
{
	ASSERT(hostSub);

	return CMsgMng::Send(hostSub->addr, hostSub->portNo, command, msg, exMsg);
}

BOOL CMsgMng::Send(HostSub *hostSub, ULONG command, int iVal)
{
	ASSERT(hostSub);

	char buf[MAX_NAMEBUF] = {0};
	wsprintf(buf, _T("%d"), iVal);

	return CMsgMng::Send(hostSub, command, buf);
}

BOOL CMsgMng::UdpRecv(RecvBuf *recv)
{
	ASSERT(recv);
	
	recv->addrSize = sizeof(recv->addr);
	if ((recv->size = ::recvfrom(m_sockUdp, recv->msgBuf, sizeof(recv->msgBuf) -1, 0, 
		(sockaddr*)&recv->addr, &recv->addrSize)) == SOCKET_ERROR)
	{
		DUI__Trace(_T("recvfrom failed. error %d"), WSAGetLastError());
		return FALSE;
	}
	
	recv->msgBuf[recv->size] = 0;

	return TRUE;
}

BOOL CMsgMng::Recv(MsgBuf *msg)
{
	ASSERT(msg);
	RecvBuf	recvBuf;

	memset(&recvBuf, 0, sizeof(recvBuf));

	if (!UdpRecv(&recvBuf) || recvBuf.size == 0)
	{
		return FALSE;
	}

	return ResolveMsg(&recvBuf, msg);
}

//��recv�е����ݾ�������Ƶ�msg
BOOL CMsgMng::ResolveMsg(RecvBuf *buf, MsgBuf *msg)
{
	//ASSERT(recv && msg);
	char	*exStr = NULL, *tok, *p;
	int		len;

	if (buf->size > (len = strlen(buf->msgBuf)) +1)
		exStr = buf->msgBuf + len +1;

	msg->hostSub.addr	= buf->addr.sin_addr.s_addr;
	msg->hostSub.portNo	= buf->addr.sin_port;

	if ((tok = separate_token(buf->msgBuf, ':', &p)) == NULL)
		return	FALSE;
	if ((msg->version = atoi(tok)) != IM_VERSION)
		return	FALSE;

	if ((tok = separate_token(NULL, ':', &p)) == NULL)
		return	FALSE;
	msg->packetNo = atol(tok);

	if ((tok = separate_token(NULL, ':', &p)) == NULL)
		return	FALSE;
	strncpy(msg->hostSub.userName, tok, sizeof(msg->hostSub.userName));

	if ((tok = separate_token(NULL, ':', &p)) == NULL)
		return	FALSE;
	strncpy(msg->hostSub.hostName, tok, sizeof(msg->hostSub.hostName));

	if ((tok = separate_token(NULL, ':', &p)) == NULL)
		return	FALSE;
	msg->command = atol(tok);

	int		cnt = 0, ex_len;
	*msg->msgBuf = 0;
	if ((tok = separate_token(NULL, 0, &p)) != NULL)
	{
		while (*tok != '\0' && cnt < MAX_UDPBUF -1)
		{
			if ((msg->msgBuf[cnt++] = *tok++) == '\n')
			{
				msg->msgBuf[cnt-1] = '\r';
				if (cnt < MAX_UDPBUF -1)
					msg->msgBuf[cnt++] = '\n';
			}
		}
		msg->msgBuf[cnt] = '\0';
	}
	msg->exOffset = cnt;

	if (exStr && (ex_len = strlen(exStr) + 1) < MAX_UDPBUF -1)
	{
		if (++msg->exOffset + ex_len >= MAX_UDPBUF)
			msg->msgBuf[(msg->exOffset = MAX_UDPBUF - ex_len) -1] = '\0'; // exStr
		memcpy(msg->msgBuf + msg->exOffset, exStr, ex_len);
	}

	return	TRUE;
}


int  CMsgMng::LocalNewLineToUnix(char *dest, const char *src, int maxlen)
{
	int		len = 0;

	maxlen--;	// \0 

	while (*src != '\0' && len < maxlen)
		if ((dest[len] = *src++) != '\r')
			len++;
	dest[len] = 0;

	return	len;
}

int  CMsgMng::UnixNewLineToLocal(char *dest, const char *src, int maxlen)
{
	int		len = 0;
	char	*tmpbuf = NULL;

	if (src == dest)
		tmpbuf = strdup(src), src = tmpbuf;

	maxlen--;	// \0 �i�[��

	while (*src != '\0' && len < maxlen)
	{
		if ((dest[len] = *src++) == '\n')
		{
			dest[len++] = '\r';
			if (len < maxlen)
				dest[len] = '\n';
		}
		len++;
	}
	dest[len] = 0;
	if (tmpbuf)
		free(tmpbuf);

	return	len;
}

