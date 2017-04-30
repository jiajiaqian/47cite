#include "StdAfx.h"
#include "SocketManage.h"
#include "SocketLine1.h"
#include "IniManage/IniManage.h"
#include "PCFrameWork.h"

static SOCKET m_TcpClientSock;
static SOCKET m_TcpServerSock;

boost::pool<> gPoolSenderSocket(sizeof(SENDPACKAGE)*10);
boost::lockfree::queue<PSENDPACKAGE> gSenderSocketQueue(10);
EventNotify gSocketSenderEvent;

static struct event_base* m_EventBase;

static struct event signal_event;
static struct event m_listen_ev;

static SocketProcess gSocketProcess;

static COMMUCONFIG m_CommuConfig;

boost::atomic<bool> gbHeartThreadSwitch= true;
boost::atomic<bool> gbSenderThreadSwitch= true;


SocketManage::SocketManage(void)
{
	m_nSeq = 0;
}


SocketManage::~SocketManage(void)
{
}
bool SocketManage::InitConfig()
{
	logger::LogDebug("socket","��ʼ�������ļ�");
	CIniManage IniCtrl;
	//��ʼ��ͨѶ����
	if(!IniCtrl.Open(IniCtrl.GetBasePath()+"Socket_Line1\\Setting.ini")){
		logger::LogInfo("socket","�����ļ���ʼ��ʧ��");
		return false;
	}
	m_CommuConfig.strlocalIP = IniCtrl.GetValue("config","Local IP");
	m_CommuConfig.strlocalPort = IniCtrl.GetValue("config","Local port");
	m_CommuConfig.strRemoteIP = IniCtrl.GetValue("config","Partner IP");
	m_CommuConfig.strRemotePort = IniCtrl.GetValue("config", "Partner port");
	m_CommuConfig.strBindLocalIP = IniCtrl.GetValue("config", "Bind local IP");
	m_CommuConfig.strBindLocalPort = IniCtrl.GetValue("config", "Bind local port");

	//���ͨ�ŷ�ʽ
	m_CommuConfig.strProtocol = IniCtrl.GetValue("config", "Protocol");
	m_CommuConfig.strDoubleLine = IniCtrl.GetValue("config", "Double line");
	m_CommuConfig.strSide = IniCtrl.GetValue("config", "Side");
	m_CommuConfig.strDConnect = 	IniCtrl.GetValue("config","Dynamic connect");
	m_CommuConfig.strPreformanceMode = IniCtrl.GetValue("config","Performance mode");
	m_CommuConfig.strAutoReconnect = IniCtrl.GetValue("config","Auto reconnect");
	m_CommuConfig.strServerReconnectInterVal = IniCtrl.GetValue("config","Server reconnect interval");
	m_CommuConfig.strClientReconnectInterVal =IniCtrl.GetValue("config","Client reconnect interval");
	m_CommuConfig.strAckMessage = IniCtrl.GetValue("config","Ack message");
	m_CommuConfig.strSameID = IniCtrl.GetValue("config","Ack with same ID");
	m_CommuConfig.strAckHeadID = IniCtrl.GetValue("config","Ack with head ID");
	m_CommuConfig.strHeartBeat = IniCtrl.GetValue("config","Heartbeat");
	m_CommuConfig.strHeartBeatAck = IniCtrl.GetValue("config","Heartbeat ack");
	m_CommuConfig.strAutoRecord = IniCtrl.GetValue("config","Auto record");
	m_CommuConfig.strHeartbeatDataRecord = IniCtrl.GetValue("config","Heartbeat data record");
	m_CommuConfig.strNotifyThread = IniCtrl.GetValue("config","Notify thread");
	m_CommuConfig.strHeartBeatSendSide = IniCtrl.GetValue("config","Heartbeat send side");
	m_CommuConfig.strheartBeatInterval = IniCtrl.GetValue("config","Heartbeat interval");
	m_CommuConfig.strHeartbeatTolerance = IniCtrl.GetValue("config","Heartbeat tolerance");
	m_CommuConfig.strAckIDInfoOffset = IniCtrl.GetValue("config","Ack ID info offset");
	m_CommuConfig.strAckLine = IniCtrl.GetValue("config","Ack line");
	m_CommuConfig.strAckMessageTolerance = IniCtrl.GetValue("config","Ack message tolerance");
	m_CommuConfig.strrAutoResend =  IniCtrl.GetValue("config","Auto resend");
	m_CommuConfig.strIDType = IniCtrl.GetValue("config","ID type");
	m_CommuConfig.strIDAddress = IniCtrl.GetValue("config","ID address");
	m_CommuConfig.strIDOffset = IniCtrl.GetValue("config","ID offset");
	m_CommuConfig.strReceiveMessageCount = IniCtrl.GetValue("config","Recieve message count");
	m_CommuConfig.strSendMessageCount = IniCtrl.GetValue("config","Send message count");

	return true;
}

void SocketManage::CreateTCPServer()
{
	logger::LogDebug("socket","����TCP����");
	struct sockaddr_in my_addr;
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0); 
	int yes = 1;

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (LPCSTR)&yes, sizeof(int));
	UINT port = atoi(m_CommuConfig.strlocalPort.GetBuffer(0));
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = INADDR_ANY;
 	::bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
	listen(sock, BACKLOG);
	logger::LogInfo("socket","��������");

	evthread_use_windows_threads();
	m_EventBase = event_base_new();
	event_set(&m_listen_ev, sock, EV_READ|EV_PERSIST, TcpAccept, NULL);
	event_base_set(m_EventBase, &m_listen_ev);
	event_add(&m_listen_ev, NULL);
	event_base_dispatch(m_EventBase);
}


void SocketManage::CreateServer()
{
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
	int sock = NULL;

	//��������
	if(m_CommuConfig.strProtocol == "TCP/IP"){
		CreateTCPServer();
	}else if(m_CommuConfig.strProtocol == "UDP/IP"){
	}
}

bool SocketManage::CreateClient()
{
	logger::LogDebug("socket","����TCP�ͻ���");

	//������Ϣ�����ﴦ��
	if(!_CreateTcpSocket(m_TcpClientSock)){
		logger::LogInfo("socket","����TCP�ͻ���ʧ��");
		return false;
	}

	if(!_ConnectRemote(m_TcpClientSock, m_CommuConfig.strRemoteIP, atoi(m_CommuConfig.strRemotePort))){
		logger::LogInfo("socket","����Զ�̷�������ʧ��");
		return false;
	}					
	logger::LogDebug("socket","����TCP�ͻ������");
	return true;
}

void SocketManage::SenderThread()
{
	fd_set fdSend;
	timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 500;

	PSENDPACKAGE pMsg;
	int ret = 0;
// 	while(gbSenderThreadSwitch){
// 		if(SocketManage::CreateClient())
// 			break;
// 		Sleep(1000);
// 	}
	FD_ZERO(&fdSend);
	FD_SET(m_TcpClientSock, &fdSend);
	while(gbSenderThreadSwitch){
		gSocketSenderEvent.Wait();
		while(gSenderSocketQueue.pop(pMsg)){
			ret = select(m_TcpClientSock+1, NULL, &fdSend, NULL, &TimeOut);
			if (ret > 0)	{
				switch(pMsg->nType)
				{
				case enum_GfcL2ExtAccResult_LP:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->earlp)+sizeof(HEADER), 0);
					break;
				case enum_GfcL2ExtAccResult_tcr1:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->eartcr1)+sizeof(HEADER), 0);
					break;
				case enum_GfcL2ExtAccResult_tcr2:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->eartcr2)+sizeof(HEADER), 0);
					break;
				case enum_GfcL2PresetAccResult_LP:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->parlp)+sizeof(HEADER), 0);
					break;
				case enum_GfcL2PresetAccResult_tcr1:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->partcr1)+sizeof(HEADER), 0);
					break;
				case enum_GfcL2PresetAccResult_tcr2:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->partcr2)+sizeof(HEADER), 0);
					break;
				case enum_GfcL2ExtColTimeRes_LP:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->ectlp)+sizeof(HEADER), 0);
					break;
				case enum_GfcL2ExtColTimeRes_tcr1:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->ecttcr1)+sizeof(HEADER), 0);
					break;
				case enum_GfcL2ExtColTimeRes_tcr2:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->ecttcr2)+sizeof(HEADER), 0);
					break;
				case enum_GfcL2ExtReq:
					ret = send(m_TcpClientSock, (const char*)(pMsg), sizeof(pMsg->er)+sizeof(HEADER), 0);
					break;
				default:
					break;
				}
				gPoolSenderSocket.free(pMsg);
				logger::LogDebug("socket","������Ϣ�ɹ��� ��Ϣ��Ϊ %d, �ɹ����Ͱ�����Ϊ%d", pMsg->nType, ret);
				if (ret == 0 || ret == SOCKET_ERROR){ //�Է������ж�
					Sleep(1000);
					closesocket(m_TcpClientSock);
					if(SocketManage::CreateClient()){
						FD_ZERO(&fdSend);
						FD_SET(m_TcpClientSock, &fdSend);
						gSenderSocketQueue.push(pMsg);	//����ʧ�ܺ����·���
					}				
				}
			}
			else if(SOCKET_ERROR==ret)	{
				Sleep(3000);
				closesocket(m_TcpClientSock);
				SocketManage::CreateClient();				
				FD_ZERO(&fdSend);
				FD_SET(m_TcpClientSock, &fdSend);
			}else{
			}
		}
	}
}

void FillHeartBeat(HEARTBEAT *pheartBeat)
{
	static int nSeq = 1;
	pheartBeat->MESSAGE_ID = 204;
	pheartBeat->MESSAGE_LENGTH = 40;
	pheartBeat->sequence = ++nSeq;
	pheartBeat->spare = 0;

	SYSTEMTIME  systime;
	GetLocalTime(&systime);
	pheartBeat->GfcAlvMsgHor = systime.wHour;
	pheartBeat->GfcAlvMsgMin =systime.wMinute;
	pheartBeat->GfcAlvMsgSec = systime.wSecond;
	pheartBeat->GfcAlvMsgDay = systime.wDay;
	pheartBeat->GfcAlvMsgMon = systime.wMonth;
	pheartBeat->GfcAlvMsgYer = systime.wYear;

	if(nSeq >= 20000)
		nSeq = 1;
}

UINT HeartBeatThread(LPVOID lpInfo)
{
	logger::LogDebug("socket","���������߳�");
	SocketManage *pSocket = (SocketManage *)lpInfo;
	
	HEARTBEAT heartBeat;
	fd_set fdWrite;
	timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 3000;
	
	int nInv = (atoi(m_CommuConfig.strheartBeatInterval));

	FD_ZERO(&fdWrite);
	FD_SET(m_TcpClientSock, &fdWrite);

	int nSend = 0;
	while(gbHeartThreadSwitch){
		int ret = select(m_TcpClientSock+1, NULL, &fdWrite, NULL, &TimeOut);
		if(ret > 0){
			Sleep(nInv);
			FillHeartBeat(&heartBeat);
			nSend = send(m_TcpClientSock, (char *)&heartBeat, sizeof(heartBeat), 0);

		}else if(ret == SOCKET_ERROR || ret == 0){
			if(ret == SOCKET_ERROR)
				logger::LogInfo("socket","����ʧ�ܣ�ʧȥ���ӣ������: %d",  WSAGetLastError());
			else
				logger::LogInfo("socket","�������ӳ�ʱ");

			closesocket(m_TcpClientSock);
//			if(m_CommuConfig.strAutoReconnect == "Yes"){
			if(!SocketManage::CreateClient()){
				logger::LogInfo("socket","�����߳��д����ͻ���ʧ��");
				Sleep(1000);
			}else{
				FD_ZERO(&fdWrite);
				FD_SET(m_TcpClientSock, &fdWrite);

			}
//				}
//			}else{
//				break;
//			}
		}
 	}
	return 0;
}

bool SocketManage::InitCommu()
{
	WORD    wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 0 ); 
	WSAStartup( wVersionRequested, &wsaData );

	//������������
	if(m_CommuConfig.strDoubleLine == "Yes" || m_CommuConfig.strSide == "Server")
		m_Threads.create_thread(CreateServer);
	
	if(m_CommuConfig.strDoubleLine == "Yes" || m_CommuConfig.strSide == "Client"){
		if(m_CommuConfig.strProtocol == "TCP/IP"){
			m_Threads.create_thread(SenderThread);
		}
		//������Ϣ�����߳�
		
	}

	//��������
	if(m_CommuConfig.strHeartBeat == "Yes"){
		gbHeartThreadSwitch = true;
		m_Threads.create_thread(boost::bind(&HeartBeatThread, this));
	}
	return true;
}

void SocketManage::SetCalbkProcess(SocketProcess socketProcess)
{
	gSocketProcess = socketProcess;
}

bool SocketManage::OpenCommunication()
{
	//��ʼ�������ļ�
	if(!InitConfig())
		return false;
	if(!InitCommu())
		return false;
	return true;
}

void SocketManage::CloseCommunication()
{
	event_del(&m_listen_ev);
	closesocket(m_TcpServerSock);
	closesocket(m_TcpClientSock);
	event_base_loopbreak(m_EventBase);
	if(m_Threads.size() > 0){
		gbHeartThreadSwitch = false;
		gbSenderThreadSwitch = false;
		gSocketSenderEvent.Notify();
		m_Threads.join_all();
	}
	event_base_free(m_EventBase);
	WSACleanup();
}

void SocketManage::TcpAccept(int sock, short event, void* arg) 
{
	struct sockaddr_in cli_addr;
	int sin_size;
	struct event* read_ev = (struct event*)malloc(sizeof(struct event));;
	sin_size = sizeof(struct sockaddr_in);
	m_TcpServerSock = accept(sock, (struct sockaddr*)&cli_addr, &sin_size);

	event_set(read_ev, m_TcpServerSock, EV_READ|EV_PERSIST, TcpRead, read_ev);
	event_base_set(m_EventBase, read_ev);
	event_add(read_ev, NULL);
}


void SocketManage::TcpRead(int sock, short event, void* arg)
{
	static char recvLeavebuffer[MEM_SIZE];
	static int nLeaveSize = 0;
	static char recvbuffer[MEM_SIZE*10];
	ZeroMemory(recvbuffer, MEM_SIZE*10);
	if(nLeaveSize > 0)	//�ϴ�������
		memcpy(recvbuffer,recvLeavebuffer, nLeaveSize);
	int size = 0;
	size = recv(sock, recvbuffer+nLeaveSize, MEM_SIZE*10, 0);
	nLeaveSize = 0;

	logger::LogInfo("socket","���յ���Ϣ ��Ϣ����: %d", size);

	if (size == 0 || size == -1) {
		event_del((struct event*)arg);
		free((struct event*)arg);
		closesocket(sock);
		return;
	}
	//��Ϣ
	int nJump = 0;		//���ջ�������λ��
	int nLen = 0;
	static int nTestCount = 0;
	while(nJump < size){

		memcpy(&nLen, recvbuffer+nJump, 4);
		int nSize = sizeof(GfcEXTL2PdiData);
		nSize = sizeof(HEADER);
		if(nLen == (sizeof(GfcEXTL2PdiData) + sizeof(HEADER))){
			if((size - nJump) < nLen){
				memcpy(recvLeavebuffer,recvbuffer+nJump, size-nJump);
				nLeaveSize = size-nJump;
				break;
			}
			gSocketProcess((LPVOID)(recvbuffer+nJump + 16));
			nJump += nLen;

		}else if(nLen ==  (sizeof(GfcEXTL2RealPdiData) + sizeof(HEADER))){
			CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
//			logger::LogInfo("socket","���յ�GfcEXTL2RealPdiData��Ϣ %d");
			nJump += nLen;
		}
		else if(nLen == (sizeof(GfcEXTL2AlvStatusMsg))){
			logger::LogInfo("socket","���յ�������Ϣ", nTestCount++);
			nJump += nLen;
		}else{
			break;
		}
	}
}


//-----------------------------------------------------------------------------
//          ����Socket����
//IN    :	sock: �׽���
//OUT   :	
//RETURN:
//AUTHOR:   8:12:2009   9:53   Created by Hanbing
//NOTE  :	
//-----------------------------------------------------------------------------
BOOL   SocketManage::_CreateTcpSocket(SOCKET &sock)
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		logger::LogInfo("socket","��ʼ��TCP����ʧ��");
		int nErrorCode_Create = ::WSAGetLastError();
		sock = 0;
		return FALSE;
	}
	else
	{
		int timeout = 35000; 
		int bread = 0;
		bread = setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,   sizeof(timeout));   
		timeout = 35000;   
		bread = setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,   sizeof(timeout)); 

		return TRUE;
	}
}


//-----------------------------------------------------------------------------
//          SOCKET����
//IN    :	lpszHostAddress: Ŀ��IP��ַ
//			nHostPort: Ŀ��˿�
//OUT   :	
//RETURN:	ִ�гɹ����� TRUE�� ִ��ʧ�ܷ���FALSE
//AUTHOR:   2009-12-7  8:41   Created by Hanbing
//NOTE  :	
//-----------------------------------------------------------------------------
BOOL	SocketManage::_ConnectRemote(SOCKET socket, LPCSTR lpszHostAddress, UINT nHostPort)
{
	if(socket == NULL)
		return FALSE;
	if(lpszHostAddress == NULL || strlen(lpszHostAddress) == 0)
		return FALSE;
	if(nHostPort <= 0)
		return FALSE;

	struct sockaddr_in  server;
	server.sin_family			= AF_INET;
	server.sin_port				= htons(nHostPort);
	server.sin_addr.S_un.S_addr = inet_addr(lpszHostAddress);

	int nConnect = 0; 

	nConnect = connect(socket, (struct sockaddr FAR*)&server, sizeof(struct sockaddr_in));

	if(nConnect == 0){
		logger::LogInfo("socket","����Զ�̷�����ʧ��");
		return TRUE;
	}
	else if(nConnect == -1){
		//��¼�������Ӵ���
		logger::LogInfo("socket","����Զ�̷�����ʧ��, %d" , WSAGetLastError());
		return FALSE;
	}

	return TRUE;
}

void SocketManage::SendToTMeic(int nType, LPVOID lpInfo, int nSize)
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	static char  SendBuf[1024*20];
	HEADER header;
	header.nMsgID = nType;
	header.nMsgLength = sizeof(HEADER) + nSize;
	header.nFlags = 0;
	header.nSeqCounter = ++m_nSeq;
	if(m_nSeq > 20000)
		m_nSeq = 1;

	memcpy(SendBuf, &header, sizeof(HEADER));
	memcpy(SendBuf+sizeof(HEADER), lpInfo, nSize);

	PSENDPACKAGE pMsg = (PSENDPACKAGE)gPoolSenderSocket.malloc();
	pMsg->nType = nType;
	memcpy(pMsg, (const char*)SendBuf, sizeof(HEADER)+nSize);

	gSenderSocketQueue.push(pMsg);	
	gSocketSenderEvent.Notify();
}
