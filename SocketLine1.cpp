/****************************************************************************/

#include "stdafx.h"
#include "PCFrameWork.h"
#include "Track.h"

//  boost::atomic<bool> gbThreadSwitch= true;
// 
//  boost::pool<> gPoolSocketRecv(sizeof(GfcEXTL2PdiData)*10);
// 
//  boost::lockfree::queue<PGfcEXTL2PdiData> gSocketLineQueue(10);
// EventNotify gSocketRecvEvent;
// 
// boost::thread_group gThreads;
// 
// void RecvThread();
// void SocketMessageResponse(PGfcEXTL2PdiData pData,SocketMessageDefine *socket1);

//SocketManage sm;


int nBuffSize= 0;
boost::atomic<bool> gbThreadSwitch= true;

boost::pool<> gPoolSocketRecv101(sizeof(GfcEXTL2PdiData)*10);
boost::pool<> gPoolSocketRecv102(sizeof(GfcEXTL2RealPdiData)*10);

boost::lockfree::queue<PGfcEXTL2PdiData> gSocketLineQueue101(10);
boost::lockfree::queue<PGfcEXTL2RealPdiData> gSocketLineQueue102(10);
EventNotify gSocketRecvEvent101;
EventNotify gSocketRecvEvent102;
boost::thread_group gThreads;
void RecvThread101();
void RecvThread102();

void SocketRecvProcess(int nType, const char* pRecv, int nSize);

unsigned __stdcall SocketLine1(void* pv)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);
	Sleep(200);
	WaitForSingleObject(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN], INFINITE);

	//启动
	SocketLine  sock;
	sock.SocketCreateLine(1);
	Sleep(1000);
	sock.SocketSetRecieveBufferFlag(1,(char *)pApp->sock1->RecieveBufferFlag);
	sock.SocketSetRecieveProcessFlag(1,(char *)pApp->sock1->ProcessRecieveBufferFlag);
	sock.SocketSetRecieveMessageMemory(1,1,(char *)pApp->sock1->r1);
	sock.SocketSetRecieveMessageMemory(1,2,(char *)pApp->sock1->r2);

	sock.SocketSetSendBufferFlag(1,(char *)pApp->sock1->SendBufferFlag);
	sock.SocketSetSendProcessFlag(1,(char *)pApp->sock1->ProcessSendBufferFlag);
	sock.SocketSetSendMessageMemory(1, 1, (char *)pApp->sock1->s1);
	sock.SocketSetSendMessageMemory(1, 2, (char *)pApp->sock1->s2);
	sock.SocketSetSendMessageMemory(1, 3, (char *)pApp->sock1->s3);
	sock.SocketSetSendMessageMemory(1, 4, (char *)pApp->sock1->s4);
	sock.SocketSetSendMessageMemory(1, 5, (char *)pApp->sock1->s5);
	sock.SocketSetSendMessageMemory(1, 6, (char *)pApp->sock1->s6);
	sock.SocketSetSendMessageMemory(1, 7, (char *)pApp->sock1->s7);
	sock.SocketSetSendMessageMemory(1, 8, (char *)pApp->sock1->s8);
	sock.SocketSetSendMessageMemory(1, 9, (char *)pApp->sock1->s9);
	sock.SocketSetSendMessageMemory(1, 10, (char *)pApp->sock1->s10);

	sock.SocketSetRecieveMessageTriggerEvent(1,1,"UFCSignalProcess");
	sock.SocketSetRecieveMessageTriggerEvent(1,2,"UFCSignalProcess");

	sock.SocketSetRecieveCallbk(SocketRecvProcess);
	gThreads.create_thread(RecvThread101);
	gThreads.create_thread(RecvThread102);

	sock.SocketStart(1);

	unsigned int req;

	int nIndex;
	while (1) 
	{
		nIndex=WaitForMultipleObjects(2,pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent, FALSE,INFINITE);
		if(nIndex==WAIT_OBJECT_0+1)	//结束状态
		{
			sock.SocketStop(1);
			sock.SocketDeleteLine(1);
			SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_STOP]);

			gbThreadSwitch = false;
			gSocketRecvEvent101.Notify();
			gSocketRecvEvent102.Notify();
			gThreads.join_all();
			break;
		}
		else if(nIndex==WAIT_OBJECT_0)
		{
			try
			{
				lockedmemcpy(&req,&(pApp->L2Sys->thread[TYPE_SOCKETLINE].RequestWord),4);
				for(int i = 0; i < 32; i++)
				{
					if(testRW(&req, i))
					{
						switch(i)
						{
						case 0:			//GfcL2PresetAccResult_LP 2011
							if(pApp->sock1->SendBufferFlag[0]>=31)
								pApp->sock1->SendBufferFlag[0]=1;				
							else
								pApp->sock1->SendBufferFlag[0]++;
							memcpy(&pApp->sock1->s1[pApp->sock1->SendBufferFlag[0]],&pApp->sock1->s1[0],sizeof(GfcL2PresetAccResult_LP));
 							sock.SocketSendMessage(1,1);
							break;
						case 1:			//GfcL2PresetAccResult_tcr1 2012
							if(pApp->sock1->SendBufferFlag[1]>=31)
								pApp->sock1->SendBufferFlag[1]=1;				
							else
								pApp->sock1->SendBufferFlag[1]++;
							memcpy(&pApp->sock1->s2[pApp->sock1->SendBufferFlag[1]],&pApp->sock1->s2[0],sizeof(GfcL2PresetAccResult_tcr1));
							sock.SocketSendMessage(1,2);
							break;
						case 2:			//GfcL2PresetAccResult_tcr2 2013
							if(pApp->sock1->SendBufferFlag[2]>=31)
								pApp->sock1->SendBufferFlag[2]=1;				
							else
								pApp->sock1->SendBufferFlag[2]++;
							memcpy(&pApp->sock1->s3[pApp->sock1->SendBufferFlag[2]],&pApp->sock1->s3[0],sizeof(GfcL2PresetAccResult_tcr2));
							sock.SocketSendMessage(1,3);
							break;
						case 3:			//GfcL2ExtAccResult_LP 2021
							if(pApp->sock1->SendBufferFlag[3]>=31)
								pApp->sock1->SendBufferFlag[3]=1;				
							else
								pApp->sock1->SendBufferFlag[3]++;
							memcpy(&pApp->sock1->s4[pApp->sock1->SendBufferFlag[3]],&pApp->sock1->s4[0],sizeof(GfcL2ExtAccResult_LP));
							sock.SocketSendMessage(1,4);
							break;
						case 4:			//GfcL2ExtAccResult_tcr1 2022
							if(pApp->sock1->SendBufferFlag[4]>=31)
								pApp->sock1->SendBufferFlag[4]=1;				
							else
								pApp->sock1->SendBufferFlag[4]++;
							memcpy(&pApp->sock1->s5[pApp->sock1->SendBufferFlag[4]],&pApp->sock1->s5[0],sizeof(GfcL2ExtAccResult_tcr1));
							sock.SocketSendMessage(1,5);
							break;
						case 5:			//GfcL2ExtAccResult_tcr2 2023
							if(pApp->sock1->SendBufferFlag[5]>=31)
								pApp->sock1->SendBufferFlag[5]=1;				
							else
								pApp->sock1->SendBufferFlag[5]++;
							memcpy(&pApp->sock1->s6[pApp->sock1->SendBufferFlag[5]],&pApp->sock1->s6[0],sizeof(GfcL2ExtAccResult_tcr2));
							sock.SocketSendMessage(1,6);
							break;
						case 6:			//GfcL2ExtColTimeRes_LP 2031
							if(pApp->sock1->SendBufferFlag[6]>=31)
								pApp->sock1->SendBufferFlag[6]=1;				
							else
								pApp->sock1->SendBufferFlag[6]++;
							memcpy(&pApp->sock1->s7[pApp->sock1->SendBufferFlag[6]],&pApp->sock1->s7[0],sizeof(GfcL2ExtColTimeRes_LP));
							sock.SocketSendMessage(1,7);
							break;
						case 7:			//GfcL2ExtColTimeRes_tcr1 2032
							if(pApp->sock1->SendBufferFlag[7]>=31)
								pApp->sock1->SendBufferFlag[7]=1;				
							else
								pApp->sock1->SendBufferFlag[7]++;
							memcpy(&pApp->sock1->s8[pApp->sock1->SendBufferFlag[7]],&pApp->sock1->s8[0],sizeof(GfcL2ExtColTimeRes_tcr1));
							sock.SocketSendMessage(1,8);
							break;
						case 8:			//GfcL2ExtColTimeRes_tcr2 2033
							if(pApp->sock1->SendBufferFlag[8]>=31)
								pApp->sock1->SendBufferFlag[8]=1;				
							else
								pApp->sock1->SendBufferFlag[8]++;
							memcpy(&pApp->sock1->s9[pApp->sock1->SendBufferFlag[8]],&pApp->sock1->s9[0],sizeof(GfcL2ExtColTimeRes_tcr2));
							sock.SocketSendMessage(1,9);
							break;
						case 9:			//GfcL2ExtReq 205
							if(pApp->sock1->SendBufferFlag[9]>=31)
								pApp->sock1->SendBufferFlag[9]=1;				
							else
								pApp->sock1->SendBufferFlag[9]++;
							memcpy(&pApp->sock1->s9[pApp->sock1->SendBufferFlag[8]],&pApp->sock1->s10[0],sizeof(GfcL2ExtReq));
							sock.SocketSendMessage(1,9);
							break;
						case 31:
							sock.SocketShowConfig(1);
							break;
						default:
							break;
						}
						resetTRB(4,i);
					}
				}
			}
			catch(...){
			}
		}
	}
	return 0;
}
//处理接收到的Socket消息
void ProcessPackage(LPVOID lpInfo){

// 	if(lpInfo == NULL)
// 		return;
// 	PGfcEXTL2PdiData pData = (PGfcEXTL2PdiData)gPoolSocketRecv.malloc();
// 	memcpy(pData, lpInfo, sizeof(GfcEXTL2PdiData));
// 
//  	gSocketLineQueue.push(pData);
//  	gSocketRecvEvent.Notify();
}

//ACC L2 TO PM L2 2011
void SendGfcL2PresetAccResult_LP(PGfcL2PresetAccResult_LP pMsg)
{
	//strcpy_s(pMsg->GfcPieceId,"test2011");
	//strcpy_s(pMsg->GfcPieceIdSpare,"test2011_spare");
	//strcpy_s(pMsg->GfcPltMatId,"Q235");
	//strcpy_s(pMsg->GfcPltMatIdSpare,"Q235_spare");
	//strcpy_s(pMsg->GfcPltMatCode,"Q235_code");
	//pMsg->GfcResPresetSpd=1.7;
	//pMsg->GfcPltAccType=1;
	//pMsg->GfcPltCoolMode1Pass=1;
	//pMsg->Spare1=1.1;
	//pMsg->Spare2=1.2;
	//pMsg->Spare3=1.3;
	//pMsg->Spare4=1.4;
	//pMsg->Spare5=1.5;
	//pMsg->Spare6=1.6;
	//pMsg->Spare7=1.7;
	//pMsg->Spare8=1.8;
	//pMsg->Spare9=1.9;
	//pMsg->Spare10=1.10;

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	memcpy(&pApp->sock1->s1[0], pMsg, sizeof(GfcL2PresetAccResult_LP));
	setTRB(4, 1);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

//	sm.SendToTMeic(2011, pMsg, sizeof(GfcL2PresetAccResult_LP));
}

//ACC L2 TO PM L2 2012
void SendGfcL2PresetAccResult_tcr1(PGfcL2PresetAccResult_tcr1 pMsg)
{
	/*strcpy_s(pMsg->GfcPieceId,"test2011");
	strcpy_s(pMsg->GfcPieceIdSpare,"test2011_spare");
	strcpy_s(pMsg->GfcPltMatId,"Q235");
	strcpy_s(pMsg->GfcPltMatIdSpare,"Q235_spare");
	strcpy_s(pMsg->GfcPltMatCode,"Q235_code");
	pMsg->GfcResSlbTcr1PresetSpd=1.7;
	pMsg->GfcSlbTcr1AccType=1;
	pMsg->GfcSlbTcr1CoolMode1Pass=1;
	pMsg->Spare1=1.1;
	pMsg->Spare2=1.2;
	pMsg->Spare3=1.3;
	pMsg->Spare4=1.4;
	pMsg->Spare5=1.5;
	pMsg->Spare6=1.6;
	pMsg->Spare7=1.7;
	pMsg->Spare8=1.8;
	pMsg->Spare9=1.9;
	pMsg->Spare10=1.10;*/

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	memcpy(&pApp->sock1->s2[0], pMsg, sizeof(PGfcL2PresetAccResult_tcr1));
	setTRB(4, 2);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

//	sm.SendToTMeic(2012, pMsg, sizeof(GfcL2PresetAccResult_tcr1));
}

//ACC L2 TO PM L2 2013
void SendGfcL2PresetAccResult_tcr2(PGfcL2PresetAccResult_tcr2 pMsg)
{
	/*strcpy_s(pMsg->GfcPieceId,"test2011");
	strcpy_s(pMsg->GfcPieceIdSpare,"test2011_spare");
	strcpy_s(pMsg->GfcPltMatId,"Q235");
	strcpy_s(pMsg->GfcPltMatIdSpare,"Q235_spare");
	strcpy_s(pMsg->GfcPltMatCode,"Q235_code");
	pMsg->GfcResSlbTcr2PresetSpd=1.7;
	pMsg->GfcSlbTcr2AccType=1;
	pMsg->GfcSlbTcr2CoolMode1Pass=1;
	pMsg->Spare1=1.1;
	pMsg->Spare2=1.2;
	pMsg->Spare3=1.3;
	pMsg->Spare4=1.4;
	pMsg->Spare5=1.5;
	pMsg->Spare6=1.6;
	pMsg->Spare7=1.7;
	pMsg->Spare8=1.8;
	pMsg->Spare9=1.9;
	pMsg->Spare10=1.10;*/

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	memcpy(&pApp->sock1->s3[0], pMsg, sizeof(PGfcL2PresetAccResult_tcr2));
	setTRB(4, 3);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

//	sm.SendToTMeic(2013, pMsg, sizeof(GfcL2PresetAccResult_tcr2));
}

//ACC L2 TO PM L2 2021
void SendGfcL2ExtAccResult_LP(PGfcL2ExtAccResult_LP pMsg)
{
	/*strcpy_s(pMsg->GfcPieceId,"test2011");
	strcpy_s(pMsg->GfcPieceIdSpare,"test2011_spare");
	strcpy_s(pMsg->GfcPltMatId,"Q235");
	strcpy_s(pMsg->GfcPltMatIdSpare,"Q235_spare");
	strcpy_s(pMsg->GfcPltMatCode,"Q235_code");
	pMsg->GfcResAccReq=1;
	pMsg->GfcResPltTmpColStaSurHead=800.0;
	pMsg->GfcResPltTmpColStaCalHead=799.0;
	pMsg->GfcResPltTmpColStaCorHead=798.0;
	pMsg->GfcResPltTmpColStaSurBody=797.0;
	pMsg->GfcResPltTmpColStaCalBody=796.0;
	pMsg->GfcResPltTmpColStaCorBody=795.0;
	pMsg->GfcResPltTmpColStaSurTail=794.0;
	pMsg->GfcResPltTmpColStaCalTail=793.0;
	pMsg->GfcResPltTmpColStaCorTail=792.0;
	pMsg->GfcResPltTmpColStpSurHead=791;
	pMsg->GfcResPltTmpColStpCalHead=790;
	pMsg->GfcResPltTmpColStpCorHead=789;
	pMsg->GfcResPltTmpColStpSurBody=788;
	pMsg->GfcResPltTmpColStpCalBody=787;
	pMsg->GfcResPltTmpColStpCorBody=786;
	pMsg->GfcResPltTmpColStpSurTail=785;
	pMsg->GfcResPltTmpColStpCalTail=784;
	pMsg->GfcResPltTmpColStpCorTail=783;
	pMsg->GfcResColRatAvgCorDQ=20.1;
	pMsg->GfcResColRatAvgCalDQ=20.2;
	pMsg->GfcResColRatAvgSurDQ=20.3;
	pMsg->GfcResColRatAvgCorACC=20.4;
	pMsg->GfcResColRatAvgCalACC=20.5;
	pMsg->GfcResColRatAvgSurACC=20.6;
	pMsg->GfcResColTmpStaMilY=699;
	pMsg->GfcResColTmpStaMilX=698;
	pMsg->GfcResColTmpStaTckMea=30.3;
	pMsg->GfcResColTmpStpAccX=697;
	pMsg->GfcResColTmpStpHplY=696;
	pMsg->GfcResTmpWat=25.5;
	pMsg->GfcResPrsWat=0.5;
	strcpy_s(pMsg->GfcResColStaDate,"201309041406");
	strcpy_s(pMsg->GfcResColStpDate,"201309041500");
	for (int i=0;i<24;i++)
	{
	pMsg->GfcResWatFlwAvgThd[i]=300;
	}
	for (int i=0;i<24;i++)
	{
	pMsg->GfcResWatFlwAvgBhd[i]=300;
	}
	pMsg->GfcRefWatFlwRefTop[0]=301;
	pMsg->GfcRefWatFlwRefTop[1]=302;
	pMsg->GfcResWatFlwRefBot[0]=303;
	pMsg->GfcResWatFlwRefBot[1]=304;
	pMsg->GfcResWatFlwAvgTop[0]=305;
	pMsg->GfcResWatFlwAvgTop[1]=306;
	pMsg->GfcResWatFlwAvgBot[0]=307;
	pMsg->GfcResWatFlwAvgBot[1]=308;
	pMsg->GfcResFrmPos[0]=309;
	pMsg->GfcResFrmPos[1]=310;
	pMsg->GfcResColOpMode=1;
	pMsg->GfcResColErrSta=0;
	pMsg->GfcResHedMskTopFac=0.4;
	pMsg->GfcResHedMskTopLen=1;
	pMsg->GfcResHedMskBotFac=0.5;
	pMsg->GfcResHedMskBotLen=1.1;
	pMsg->GfcResTalMskTopFac=0.4;
	pMsg->GfcResTalMskTopLen=1;
	pMsg->GfcResTalMskBotFac=0.5;
	pMsg->GfcResTalMskBotLen=1;
	pMsg->GfcResHedRunTimRef=10;
	pMsg->GfcResHedRunTimAct=12;
	pMsg->GfcResHedRunTimAct2=13;
	pMsg->GfcResTalRunTimRef=14;
	pMsg->GfcResTalRunTimAct=15;
	pMsg->GfcResTalRunTimAct2=16;

	for (int i=0;i<10;i++)
	{
	pMsg->Spare[i]=0;
	}*/

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	memcpy(&pApp->sock1->s4[0], pMsg, sizeof(PGfcL2ExtAccResult_LP));
	setTRB(4, 4);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

//	sm.SendToTMeic(2021, pMsg, sizeof(GfcL2ExtAccResult_LP));
}

//ACC L2 TO PM L2 2022
void SendGfcL2ExtAccResult_tcr1(PGfcL2ExtAccResult_tcr1 pMsg)
{
	/*strcpy_s(pMsg->GfcPieceId,"test2022");
	strcpy_s(pMsg->GfcPieceIdSpare,"test2022_spare");
	strcpy_s(pMsg->GfcPltMatId,"Q235");
	strcpy_s(pMsg->GfcPltMatIdSpare,"Q235_spare");
	strcpy_s(pMsg->GfcPltMatCode,"Q235_code");
	pMsg->GfcResSlbAccReq=1;
	pMsg->GfcResSlbTcr1TmpColStpSurBody=800;
	pMsg->GfcResSlbTcr1TmpColStpCalBody=800;
	pMsg->GfcResSlbTcr1TmpColStpCorBody=800;
	pMsg->GfcResSlbTcr1TmpColStpSurTail=799;
	pMsg->GfcResSlbTcr1TmpColStpCalTail=799;
	pMsg->GfcResSlbTcr1TmpColStpCorTail=799;
	pMsg->GfcResSlbTcr1ColRatAvgCor=30;
	pMsg->GfcResSlbTcr1ColRatAvgCal=30;
	pMsg->GfcResSlbTcr1ColRatAvgSur=30;
	pMsg->GfcResSlbColTmpStaMilY=600;
	pMsg->GfcResSlbColTmpStaMilX=600;
	pMsg->GfcResSlbTmpWat=25;
	pMsg->GfcResSlbPrsWat=0.5;
	strcpy_s(pMsg->GfcResSlbColStaDate,"201309041406");
	strcpy_s(pMsg->GfcResSlbColStpDate,"201309041500");
	for (int i=0;i<24;i++)
	{
	pMsg->GfcResSlbWatFlwAvgThd[i]=300;
	}
	for (int i=0;i<24;i++)
	{
	pMsg->GfcResSlbWatFlwAvgBhd[i]=300;
	}
	pMsg->GfcRefSlbWatFlwRefTop[0]=299;
	pMsg->GfcRefSlbWatFlwRefTop[1]=299;
	pMsg->GfcResSlbWatFlwRefBot[0]=298;
	pMsg->GfcResSlbWatFlwRefBot[1]=298;
	pMsg->GfcResSlbWatFlwAvgTop[0]=297;
	pMsg->GfcResSlbWatFlwAvgTop[1]=297;
	pMsg->GfcResSlbWatFlwAvgBot[0]=296;
	pMsg->GfcResSlbWatFlwAvgBot[1]=296;

	pMsg->GfcResSlbFrmPos[0]=309;
	pMsg->GfcResSlbFrmPos[1]=310;
	pMsg->GfcResSlbColOpMode=1;
	pMsg->GfcResSlbColErrSta=0;
	pMsg->GfcResSlbHedMskTopFac=0.4;
	pMsg->GfcResSlbHedMskTopLen=1;
	pMsg->GfcResSlbHedMskBotFac=0.5;
	pMsg->GfcResSlbHedMskBotLen=1.1;
	pMsg->GfcResSlbTalMskTopFac=0.4;
	pMsg->GfcResSlbTalMskTopLen=1;
	pMsg->GfcResSlbTalMskBotFac=0.5;
	pMsg->GfcResSlbTalMskBotLen=1;
	pMsg->GfcResSlbHedRunTimRef=10;
	pMsg->GfcResSlbHedRunTimAct=12;
	pMsg->GfcResSlbHedRunTimAct2=13;
	pMsg->GfcResSlbTalRunTimRef=14;
	pMsg->GfcResSlbTalRunTimAct=15;
	pMsg->GfcResSlbTalRunTimAct2=16;

	for (int i=0;i<10;i++)
	{
	pMsg->Spare[i]=0;
	}*/		

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	memcpy(&pApp->sock1->s5[0], pMsg, sizeof(PGfcL2ExtAccResult_tcr1));
	setTRB(4, 5);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

//	sm.SendToTMeic(2022, pMsg, sizeof(GfcL2ExtAccResult_tcr1));
}

//ACC L2 TO PM L2 2023
void SendGfcL2ExtAccResult_tcr2(PGfcL2ExtAccResult_tcr2 pMsg)
{
	/*strcpy_s(pMsg->GfcPieceId,"test2023");
	strcpy_s(pMsg->GfcPieceIdSpare,"test2023_spare");
	strcpy_s(pMsg->GfcPltMatId,"Q235");
	strcpy_s(pMsg->GfcPltMatIdSpare,"Q235_spare");
	strcpy_s(pMsg->GfcPltMatCode,"Q235_code");
	pMsg->GfcResSlbAccReq=1;
	pMsg->GfcResSlbTcr2TmpColStpSurBody=800;
	pMsg->GfcResSlbTcr2TmpColStpCalBody=800;
	pMsg->GfcResSlbTcr2TmpColStpCorBody=800;
	pMsg->GfcResSlbTcr2TmpColStpSurTail=799;
	pMsg->GfcResSlbTcr2TmpColStpCalTail=799;
	pMsg->GfcResSlbTcr2TmpColStpCorTail=799;
	pMsg->GfcResSlbTcr2ColRatAvgCor=30;
	pMsg->GfcResSlbTcr2ColRatAvgCal=30;
	pMsg->GfcResSlbTcr2ColRatAvgSur=30;
	pMsg->GfcResSlbColTmpStaMilY=600;
	pMsg->GfcResSlbColTmpStaMilX=600;
	pMsg->GfcResSlbTmpWat=25;
	pMsg->GfcResSlbPrsWat=0.5;
	strcpy_s(pMsg->GfcResSlbColStaDate,"201309041406");
	strcpy_s(pMsg->GfcResSlbColStpDate,"201309041500");
	for (int i=0;i<24;i++)
	{
		pMsg->GfcResSlbWatFlwAvgThd[i]=300;
	}
	for (int i=0;i<24;i++)
	{
		pMsg->GfcResSlbWatFlwAvgBhd[i]=300;
	}
	pMsg->GfcRefSlbWatFlwRefTop[0]=299;
	pMsg->GfcRefSlbWatFlwRefTop[1]=299;
	pMsg->GfcResSlbWatFlwRefBot[0]=298;
	pMsg->GfcResSlbWatFlwRefBot[1]=298;
	pMsg->GfcResSlbWatFlwAvgTop[0]=297;
	pMsg->GfcResSlbWatFlwAvgTop[1]=297;
	pMsg->GfcResSlbWatFlwAvgBot[0]=296;
	pMsg->GfcResSlbWatFlwAvgBot[1]=296;

	pMsg->GfcResSlbFrmPos[0]=309;
	pMsg->GfcResSlbFrmPos[1]=310;
	pMsg->GfcResSlbColOpMode=1;
	pMsg->GfcResSlbColErrSta=0;
	pMsg->GfcResSlbHedMskTopFac=0.4;
	pMsg->GfcResSlbHedMskTopLen=1;
	pMsg->GfcResSlbHedMskBotFac=0.5;
	pMsg->GfcResSlbHedMskBotLen=1.1;
	pMsg->GfcResSlbTalMskTopFac=0.4;
	pMsg->GfcResSlbTalMskTopLen=1;
	pMsg->GfcResSlbTalMskBotFac=0.5;
	pMsg->GfcResSlbTalMskBotLen=1;
	pMsg->GfcResSlbHedRunTimRef=10;
	pMsg->GfcResSlbHedRunTimAct=12;
	pMsg->GfcResSlbHedRunTimAct2=13;
	pMsg->GfcResSlbTalRunTimRef=14;
	pMsg->GfcResSlbTalRunTimAct=15;
	pMsg->GfcResSlbTalRunTimAct2=16;

	for (int i=0;i<10;i++)
	{
		pMsg->Spare[i]=0;
	}*/

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	memcpy(&pApp->sock1->s6[0], pMsg, sizeof(PGfcL2ExtAccResult_tcr2));
	setTRB(4, 6);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

//	sm.SendToTMeic(2023, pMsg, sizeof(GfcL2ExtAccResult_tcr2));
}


//ACC L2 TO PM L2 2031
void SendGfcL2ExtColTimeRes_LP(PGfcL2ExtColTimeRes_LP pMsg)
{
	//ZeroMemory(pMsg, sizeof(GfcL2ExtColTimeRes_LP));
	//strcpy_s(pMsg->GfcPieceId,"test2031");
	//pMsg->GfcResFnlCr=20;
//	sm.SendToTMeic(2031, pMsg, sizeof(GfcL2ExtColTimeRes_LP));

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	
	memcpy(&pApp->sock1->s7[0], pMsg, sizeof(GfcL2ExtColTimeRes_LP));//by Zed
	setTRB(4, 7);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

}


//ACC L2 TO PM L2 2032
void SendGfcL2ExtColTimeRes_tcr1(PGfcL2ExtColTimeRes_tcr1 pMsg)
{
	//strcpy(pMsg->GfcPieceId, "test2032");
	//pMsg->GfcResInterSlabTcr1ColTimAir=50;
	//pMsg->GfcResInterSlabTcr1ColTimWat=7.5;
	//pMsg->GfcResTcr1ColTimAir=55;
	//pMsg->GfcResTcr1ColTimWat=8.5;
	//pMsg->GfcResTcr1ColTimSta=1;
	//pMsg->GfcResTcr1PresetSpd=1.3;
	//pMsg->GfcPltTcr1AccType=1;
	//pMsg->GfcPltTcr1CoolMode1Pass=0;
	//for (int i=0;i<10;i++)
	//{
	//	pMsg->Spare[i]=i*1.0;
	//}
	//	sm.SendToTMeic(2032, pMsg, sizeof(GfcL2ExtColTimeRes_tcr1));

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	memcpy(&pApp->sock1->s8[0], pMsg, sizeof(PGfcL2ExtColTimeRes_tcr1));
	setTRB(4, 8);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

}

//ACC L2 TO PM L2 2033
void SendGfcL2ExtColTimeRes_tcr2(PGfcL2ExtColTimeRes_tcr2 pMsg)
{
	//strcpy(pMsg->GfcPieceId, "test2033");
	//pMsg->GfcResInterSlabTcr2ColTimAir=50;
	//pMsg->GfcResInterSlabTcr2ColTimWat=7.5;
	//pMsg->GfcResTcr2ColTimAir=55;
	//pMsg->GfcResTcr2ColTimWat=8.5;
	//pMsg->GfcResTcr2ColTimSta=1;
	//pMsg->GfcResTcr2PresetSpd=1.3;
	//pMsg->GfcPltTcr2AccType=1;
	//pMsg->GfcPltTcr2CoolMode1Pass=0;
	//for (int i=0;i<10;i++)
	//{
	//	pMsg->Spare[i]=i*1.0;
	//}
//	sm.SendToTMeic(2033, pMsg, sizeof(GfcL2ExtColTimeRes_tcr2));

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	memcpy(&pApp->sock1->s9[0], pMsg, sizeof(PGfcL2ExtColTimeRes_tcr2));
	setTRB(4, 9);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);

}

//205
void SendGfcL2ExtReq(PGfcL2ExtReq pmsg)
{
	//strcpy(pmsg->GfcPieceId, "1220657756");
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	memcpy(&pApp->sock1->s10[0], pmsg, sizeof(PGfcL2ExtColTimeRes_tcr2));

	setTRB(4, 10);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);
//	sm.SendToTMeic(205, pmsg, sizeof(GfcL2ExtReq));
}

void RecvThread()
{
// 	CPCFrameWorkApp * pApp;
// 	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
// 
// 	SocketMessageDefine *sock1=pApp->sockNew;
// 
// 	while(gbThreadSwitch){
// 		gSocketRecvEvent.Wait();
// 		PGfcEXTL2PdiData pData;
// 		while(gSocketLineQueue.pop(pData)){
// 			CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
// 			sock1->recive101=*pData;
// 			SocketMessageResponse(pData,pApp->sockNew);
// 			gPoolSocketRecv.free(pData);
// 		}
// 	}
}

void SocketMessageResponse(PGfcEXTL2PdiData pData,SocketMessageDefine *socket1)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	clock_t     startTimeTemp;
	startTimeTemp   = clock();
	char outPutword[100];

	// 处理Time request [9/23/2013 谢谦]
	if (1==pData->GfcProdState)
	{
		setTRB(7,8);  // 线程号+1 设定的任务号 +1  [9/22/2013 谢谦]
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);
		return;
	}
		
	if (
		(1==pData->GfcRollPassNumNow)&&(pData->GfcPltIcFlg[0] == '0'))
	{
		pApp->ufcs->CurPltIcFlag=0;
		Auto_SendPdiToPLCWithParam(pData);
		setTRB(7,1);  // 线程号+1 设定的任务号 +1  [9/22/2013 谢谦]
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);

		sprintf_s(outPutword,"咬钢触发 time: %d",startTimeTemp);
		LogWrite(7,1,outPutword,100);
		return;
	}

	// 功能待完成 [9/23/2013 谢谦]
	if (
		(pData->GfcTcr1RollPassNumSum==pData->GfcRollPassNumNow)&&(pData->GfcPltIcFlg[0] == '1')
		)
	{
		pApp->ufcs->CurPltIcFlag=1;
		setTRB(7,1);  // 线程号+1 设定的任务号 +1  [9/22/2013 谢谦]
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);
		return;
	}
		// 功能待完成 [9/23/2013 谢谦]
	if (
		(pData->GfcTcr2RollPassNumSum==pData->GfcRollPassNumNow)&&(pData->GfcPltIcFlg[0] == '2')
		)
	{
		pApp->ufcs->CurPltIcFlag=2;
		setTRB(7,1);  // 线程号+1 设定的任务号 +1  [9/22/2013 谢谦]
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);
		return;
	}


}


void SocketRecvProcess(int nType, const char* pRecv, int nSize)
{
	if(nType == 0){							//101
		PGfcEXTL2PdiData pData = (PGfcEXTL2PdiData)gPoolSocketRecv101.malloc();
		memcpy(pData, pRecv, sizeof(GfcEXTL2PdiData));
		gSocketLineQueue101.push(pData);
		nBuffSize++;
		gSocketRecvEvent101.Notify();
	}else if(nType == 1){					//102
		PGfcEXTL2RealPdiData pData = (PGfcEXTL2RealPdiData)gPoolSocketRecv102.malloc();
		memcpy(pData, pRecv, sizeof(PGfcEXTL2RealPdiData));
		gSocketLineQueue102.push(pData);
		gSocketRecvEvent102.Notify();
	}
	CString strDebug;
	strDebug.Format("Recv -----  %d\r\n", nBuffSize);
	OutputDebugString(strDebug);
}


void RecvThread101()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	while(gbThreadSwitch){
		gSocketRecvEvent101.Wait();
		PGfcEXTL2PdiData pData;
		while(gSocketLineQueue101.pop(pData)){
			CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
			memcpy(&pApp->sock1->r1ontime, pData, sizeof(S7Line1R1Define));
			socketTrack();
			gPoolSocketRecv101.free(pData);

			CString strDebug;
			strDebug.Format("Process -----  %d\r\n", --nBuffSize);
			OutputDebugString(strDebug);
		}
	}
}

void RecvThread102()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	while(gbThreadSwitch){
		gSocketRecvEvent102.Wait();
		PGfcEXTL2PdiData pData;
		while(gSocketLineQueue102.pop(pData)){
			CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
			//			socketTrack(pData);
			gPoolSocketRecv102.free(pData);
		}
	}
}
