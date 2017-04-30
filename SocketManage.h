#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/event-config.h>
#include <event2/thread.h>
#include <evhttp.h>

#define BACKLOG 5
#define MEM_SIZE 4096

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "libevent.lib")
#pragma comment(lib, "libevent_core.lib")
#pragma comment(lib, "libevent_extras.lib")

typedef struct tagMessageElement{
	char szElementName[100];
	char szElementType[10];
	char szElementDefValue[100];
	int nCount;
}MESSAGEELEMENT;

typedef struct tagHeartBeat{
	int      MESSAGE_LENGTH;
	int	  MESSAGE_ID;
	int      sequence;
	int      spare;
	int    GfcAlvMsgHor;
	int    GfcAlvMsgMin;
	int    GfcAlvMsgSec;
	int    GfcAlvMsgDay;
	int    GfcAlvMsgMon;
	int    GfcAlvMsgYer;
}HEARTBEAT, *PHEARTBEAT;

typedef struct tagCommunication{
	CString strlocalIP;
	CString strlocalPort;
	CString strRemoteIP;
	CString strRemotePort;
	CString strBindLocalIP;
	CString strBindLocalPort;

	//获得通信方式
	CString strProtocol ; 
	CString strDoubleLine;
	CString strSide;
	CString strDConnect;
	CString strPreformanceMode ;
	CString strAutoReconnect;
	CString strServerReconnectInterVal;
	CString strClientReconnectInterVal;
	CString strAckMessage;
	CString strSameID;
	CString strAckHeadID;
	CString strHeartBeat;
	CString strHeartBeatAck;
	CString strAutoRecord;
	CString strHeartbeatDataRecord;
	CString strNotifyThread;
	CString strHeartBeatSendSide;
	CString strheartBeatInterval;
	CString strHeartbeatTolerance;
	CString strAckIDInfoOffset;
	CString strAckLine;
	CString strAckMessageTolerance;
	CString strrAutoResend;
	CString strIDType;
	CString strIDAddress;
	CString strIDOffset;
	CString strReceiveMessageCount;
	CString strSendMessageCount;
}COMMUCONFIG;


typedef struct tagRecvBuffer{
	byte byBuffer[1024*100];
	int nLength;
	int nJump;

	tagRecvBuffer(){
		nLength = 0;
		nJump = 0;
	}

	void Reset(){
		if(nJump < nLength)	{	//还有余留数据
			for(int i = 0; i < (nLength-nJump); i++){
				byBuffer[i] = byBuffer[nJump+i];
			}
			nLength = nLength-nJump;
		}
		else
			nLength = 0;
		nJump = 0;
	}

	int GetSize(){
		return nLength - nJump;
	}

	byte* GetBuffer(){
		return byBuffer + nJump;
	}

	void Reset(int nPos){
		nJump = nPos;
	}

}RECVBUFFER;

class SocketManage
{
public:
	SocketManage(void);
	~SocketManage(void);

	bool OpenCommunication();
	void CloseCommunication();

	bool InitConfig();
	bool InitCommu();

	void SetCalbkProcess(SocketProcess socketProcess);

	static void TcpAccept(int sock, short event, void* arg); 
	static void TcpRead(int sock, short event, void* arg);

public:
	static void CreateServer();
	static bool CreateClient();
	static void CreateTCPServer();
	static BOOL _CreateTcpSocket(SOCKET &sock);
	static BOOL	_ConnectRemote(SOCKET socket, LPCSTR lpszHostAddress, UINT nHostPort);

	void SendToTMeic(int nType, LPVOID lpInfo, int nSize);

	boost::thread_group m_Threads;

	vector<MESSAGEELEMENT> m_vecRecvHeartBeat;
	vector<MESSAGEELEMENT> m_vecSendHeartBeat;

	static void FillElement(std::vector<MESSAGEELEMENT> *pvecRecvHeartBeat,  CString strElement);
 
	static void SenderThread();

	int m_nSeq;
};

