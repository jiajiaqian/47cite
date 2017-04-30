/****************************************************************************/

#include "stdafx.h"
#include "PCFrameWork.h"
#include "Track.h"



 boost::atomic<bool> gbThreadSwitch= true;

 boost::pool<> gPoolSocketRecv(sizeof(GfcEXTL2PdiData)*10);

 boost::lockfree::queue<PGfcEXTL2PdiData> gSocketLineQueue(10);
EventNotify gSocketRecvEvent;

boost::thread_group gThreads;

void RecvThread();

void ProcessPackage(LPVOID lpInfo);

SocketManage sm;
unsigned __stdcall SocketLine1(void* pv)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

	WaitForSingleObject(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN], INFINITE);
	//启动

	//打开连接
	sm.OpenCommunication();
	logger::LogInfo("socket","初始化socket连接");
	//设置回调函数
	sm.SetCalbkProcess(ProcessPackage);
	gThreads.create_thread(RecvThread);
	logger::LogInfo("socket","启动接收线程");
	int nIndex;
	while (1) 
	{
		nIndex=WaitForMultipleObjects(2,pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent, FALSE,INFINITE);
		if(nIndex==WAIT_OBJECT_0+1)	{//结束状态
			gbThreadSwitch = false;
			gSocketRecvEvent.Notify();
			gThreads.join_all();
			sm.CloseCommunication();
			SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_STOP]);
			break;
		}else if(nIndex==WAIT_OBJECT_0){
		}
	}
	return 0;
}
//处理接收到的Socket消息
void ProcessPackage(LPVOID lpInfo){

	if(lpInfo == NULL)
		return;
	PGfcEXTL2PdiData pData = (PGfcEXTL2PdiData)gPoolSocketRecv.malloc();
	memcpy(pData, lpInfo, sizeof(GfcEXTL2PdiData));

 	gSocketLineQueue.push(pData);
 	gSocketRecvEvent.Notify();
}

//ACC L2 TO PM L2 2011
void SendGfcL2PresetAccResult_LP(PGfcL2PresetAccResult_LP pMsg)
{
	sm.SendToTMeic(enum_GfcL2PresetAccResult_LP, pMsg, sizeof(GfcL2PresetAccResult_LP));
}

//ACC L2 TO PM L2 2012
void SendGfcL2PresetAccResult_tcr1(PGfcL2PresetAccResult_tcr1 pMsg)
{
	sm.SendToTMeic(enum_GfcL2PresetAccResult_tcr1, pMsg, sizeof(GfcL2PresetAccResult_tcr1));
}

//ACC L2 TO PM L2 2013
void SendGfcL2PresetAccResult_tcr2(PGfcL2PresetAccResult_tcr2 pMsg)
{
	sm.SendToTMeic(enum_GfcL2PresetAccResult_tcr2, pMsg, sizeof(GfcL2PresetAccResult_tcr2));
}

//ACC L2 TO PM L2 2021
void SendGfcL2ExtAccResult_LP(PGfcL2ExtAccResult_LP pMsg)
{
	sm.SendToTMeic(enum_GfcL2ExtAccResult_LP, pMsg, sizeof(GfcL2ExtAccResult_LP));
}

//ACC L2 TO PM L2 2022
void SendGfcL2ExtAccResult_tcr1(PGfcL2ExtAccResult_tcr1 pMsg)
{
	sm.SendToTMeic(enum_GfcL2ExtAccResult_tcr1, pMsg, sizeof(GfcL2ExtAccResult_tcr1));
}

//ACC L2 TO PM L2 2023
void SendGfcL2ExtAccResult_tcr2(PGfcL2ExtAccResult_tcr2 pMsg)
{
	sm.SendToTMeic(enum_GfcL2ExtAccResult_tcr1, pMsg, sizeof(GfcL2ExtAccResult_tcr2));
}

//ACC L2 TO PM L2 2032
void SendGfcL2ExtColTimeRes_tcr1(PGfcL2ExtColTimeRes_tcr1 pMsg)
{
	sm.SendToTMeic(enum_GfcL2ExtColTimeRes_tcr1, pMsg, sizeof(GfcL2ExtColTimeRes_tcr1));
}

//ACC L2 TO PM L2 2033
void SendGfcL2ExtColTimeRes_tcr2(PGfcL2ExtColTimeRes_tcr2 pMsg)
{
	sm.SendToTMeic(enum_GfcL2ExtColTimeRes_tcr2, pMsg, sizeof(GfcL2ExtColTimeRes_tcr2));
}

void RecvThread()
{
	while(gbThreadSwitch){

		gSocketRecvEvent.Wait();

		PGfcEXTL2PdiData pData;
		while(gSocketLineQueue.pop(pData)){
			if (1==pData->GfcRollPassNumNow){
				Auto_SendPdiToPLCWithParam(pData);
			}
			gPoolSocketRecv.free(pData);
		}
	}
}
