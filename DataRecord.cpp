#include "stdafx.h"
#include "FL.h"
#include "PCFramework.h"

void UFCPLCToUFCL2ID1SignalProcess();
void TMEICToUFCL2ID1SignalProcess();
void TMEICToUFCL2ID2SignalProcess();
unsigned __stdcall UFCSignalProcess(void* pv)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	int message_index;
	int message_length;
	MSG msg;
	DWORD thread_id;
	thread_id=GetCurrentThreadId();
	SetEvent(pApp->L2Sys->thread[4].ThreadEvent);
	
	WaitForSingleObject(pApp->L2Sys->thread[4].ThreadEvent, INFINITE);
	unsigned int req;
	while (1) 
	{
		if(WaitForSingleObject(pApp->L2Sys->thread[4].ThreadEvent, 100)==WAIT_OBJECT_0)
		{
			try
			{
				lockedmemcpy(&req,&(pApp->L2Sys->thread),4);
				for(int i = 0; i < 32; i++)
				{
					if(testRW(&req, i))
					{
						switch(i)
						{
						case 0://PLCTOL2ID1
							UFCPLCToUFCL2ID1SignalProcess();
							break;
						case 1://SOCKETTOL2ID1					
							TMEICToUFCL2ID1SignalProcess();
							break;
						case 2://SOCKETTOL2ID2						
							TMEICToUFCL2ID2SignalProcess();
							break;
						case 31://
							break;
						default:
							break;
						}
						resetTRB(5,i);
					}
				}

			}
			catch(...)
			{
				/*alarm('d', 111, "S7Line1处理消息失败");*/
			}
		}
	/*	else
		{
			if(PeekMessage(&msg, NULL, 0,0, PM_REMOVE))
			{
				if(msg.message==WM_SOCKET_EVENT)
				{
					s7.S7Recieve(1,&message_index,(char *)(&(UFCPLCCOM->m_PLCToL2ID1)),&message_length);
					s7.S7RecieveConfirm(1);
					setTRB(3,0);
					SetEvent(weak_ufc_track_event);
				}
			}			
		}*/
	}
	return 0;
}
void UFCPLCToUFCL2ID1SignalProcess()
{
	CString mystr;
	static short old_sch_cal_trigger = 0;
	static short Old_Cool_Mode=0;
	static char old_sch_socket_pieceid[32]="empty";  //add by xieqian 2012-7-2
	static short oldManualWaterfactorTrigger = 0;
	static short oldManualBackRedTrigger = 0;

//	int iz;
	static int CurIndexFlg = 0;
	static int SumIndex1=0;
	static int SumIndex2=0;
	static int FRT_Num = 0;
	static float FRT_UP_Max = -1,FRT_UP_Ave = -1;
	static bool FRT_Run = false;
	static bool FRT_Process = false;
	float FRT_CRI = 600;
	static int T_FRT_head_Trigger=0;
	static long T_FRT_head_TriggerTime = 0;

	
	static int SCT_Num = 0;
	static float SCT_UP_Max = -1,SCT_DN_Max = -1,SCT_UP_Ave = -1,SCT_DN_Ave = -1;
	static bool SCT_Run = false;
	static bool SCT_Process = false;
	float SCT_CRI = 600;
	static int Tu_SCT_head_Trigger=0;
	static long Tu_SCT_head_TriggerTime=0;
		
	static int FCT_Num = 0;
	static float FCT_UP_Max = -1,FCT_DN_Max = -1,FCT_UP_Ave = -1,FCT_DN_Ave = -1;
	static bool FCT_Run = false;
	static bool FCT_Process = false;
	float FCT_CRI = 300;
	static int Tu_FCT_head_Trigger=0;
	static int Tu_FCT_tail_Trigger=0;
	static long Tu_FCT_head_TriggerTime=0;
	static long Tu_FCT_tail_TriggerTime=0;
	static long WriteRecord_TriggerTime=0;

	static int RBT_Num = 0;
	static float RBT_UP_Max = -1,RBT_UP_Ave = -1;
	static bool RBT_Run = false;
	static bool RBT_Process = false;
	float RBT_CRI = 311;
	static int RBT_head_Trigger=0;
	static int RBT_tail_Trigger=0;
	static long RBT_head_TriggerTime=0;
	static long RBT_tail_TriggerTime=0;

	static int LRBT_Num = 0;
	static float LRBT_UP_Max = -1,LRBT_UP_Ave = -1;
	static bool LRBT_Run = false;
	static bool LRBT_Process = false;
	float LRBT_CRI = 60;
	static int LRBT_head_Trigger=0;
	static int LRBT_tail_Trigger=0;
	static long LRBT_head_TriggerTime=0;
	static long LRBT_tail_TriggerTime=0;

	static long LP_triggertime=0;
	static int  LP_trigger=0;
    static int  online_trigger = 0;
	static long start_onl_triggertime=0;
	static int  start_onl_trigger=0;
	static long enter_czone_triggertime=0;
	static int  enter_czone_trigger=0;
	static long out_czone_triggertime=0;
	static int  out_czone_trigger=0;


	static int  old_GfcPltCoolZoneChange_1;
	static int  old_GfcPltCoolZoneChange_2;
	static int  old_GfcPltCoolZoneChange_3;

	static short old_TempPcy;
//	FILE *fp;
	
///////////////////////////////////////温度显示///////////////////////////////////////
	if(pData->m_PLCToL2ID1->TempP1_1>100)        pData->m_coolStruct.MeaTemp.TempP1 = pData->m_PLCToL2ID1->TempP1_1;
	else if(pData->m_PLCToL2ID1->TempP1_2>100)   pData->m_coolStruct.MeaTemp.TempP1 = pData->m_PLCToL2ID1->TempP1_2;
	else if(pData->m_PLCToL2ID1->TempP1_3>100)   pData->m_coolStruct.MeaTemp.TempP1 = pData->m_PLCToL2ID1->TempP1_3;
	else 
		pData->m_coolStruct.MeaTemp.TempP1=0;    //谢谦 加入 2012-5-30


	if(pData->m_PLCToL2ID1->TempP2_1>100)        pData->m_coolStruct.MeaTemp.TempP2 = pData->m_PLCToL2ID1->TempP2_1;
	else if(pData->m_PLCToL2ID1->TempP2_2>100)   pData->m_coolStruct.MeaTemp.TempP2 = pData->m_PLCToL2ID1->TempP2_2;
	else if(pData->m_PLCToL2ID1->TempP2_3>100)   pData->m_coolStruct.MeaTemp.TempP2 = pData->m_PLCToL2ID1->TempP2_3;
	else 
		pData->m_coolStruct.MeaTemp.TempP2=0;    //谢谦 加入 2012-5-30
	
	if(pData->m_PLCToL2ID1->TempP3_1>100)        pData->m_coolStruct.MeaTemp.TempP3 = pData->m_PLCToL2ID1->TempP3_1;
	else if(pData->m_PLCToL2ID1->TempP3_2>100)   pData->m_coolStruct.MeaTemp.TempP3 = pData->m_PLCToL2ID1->TempP3_2;
	else if(pData->m_PLCToL2ID1->TempP3_3>100)   pData->m_coolStruct.MeaTemp.TempP3 = pData->m_PLCToL2ID1->TempP3_3;
	else 
		pData->m_coolStruct.MeaTemp.TempP3=0;    //谢谦 加入 2012-5-30

	if(pData->m_PLCToL2ID1->TempP4_1>100)        pData->m_coolStruct.MeaTemp.TempP4 = pData->m_PLCToL2ID1->TempP4_1;
	else if(pData->m_PLCToL2ID1->TempP4_2>100)   pData->m_coolStruct.MeaTemp.TempP4 = pData->m_PLCToL2ID1->TempP4_2;
	else if(pData->m_PLCToL2ID1->TempP4_3>100)   pData->m_coolStruct.MeaTemp.TempP4 = pData->m_PLCToL2ID1->TempP4_3;
	else 
		pData->m_coolStruct.MeaTemp.TempP4=0;    //谢谦 加入 2012-5-30

	if(pData->m_PLCToL2ID1->TempP5_1>100)        pData->m_coolStruct.MeaTemp.TempP5 = pData->m_PLCToL2ID1->TempP5_1;
	else if(pData->m_PLCToL2ID1->TempP5_2>100)   pData->m_coolStruct.MeaTemp.TempP5 = pData->m_PLCToL2ID1->TempP5_2;
	else if(pData->m_PLCToL2ID1->TempP5_3>100)   pData->m_coolStruct.MeaTemp.TempP5 = pData->m_PLCToL2ID1->TempP5_3;
	else 
		pData->m_coolStruct.MeaTemp.TempP5=0;    //谢谦 加入 2012-5-30

	if(pData->m_PLCToL2ID1->TempP6_1>100)        pData->m_coolStruct.MeaTemp.TempP6 = pData->m_PLCToL2ID1->TempP6_1;
	else if(pData->m_PLCToL2ID1->TempP6_2>100)   pData->m_coolStruct.MeaTemp.TempP6 = pData->m_PLCToL2ID1->TempP6_2;
	else if(pData->m_PLCToL2ID1->TempP6_3>100)   pData->m_coolStruct.MeaTemp.TempP6 = pData->m_PLCToL2ID1->TempP6_3;
	else 
		pData->m_coolStruct.MeaTemp.TempP6=0;    //谢谦 加入 2012-5-30

	if(pData->m_PLCToL2ID1->TempP8_1>100)        pData->m_coolStruct.MeaTemp.TempP8 = pData->m_PLCToL2ID1->TempP8_1;
	else if(pData->m_PLCToL2ID1->TempP8_2>100)   pData->m_coolStruct.MeaTemp.TempP8 = pData->m_PLCToL2ID1->TempP8_2;
	else if(pData->m_PLCToL2ID1->TempP8_3>100)   pData->m_coolStruct.MeaTemp.TempP8 = pData->m_PLCToL2ID1->TempP8_3;
	else 
		pData->m_coolStruct.MeaTemp.TempP8=0;    //谢谦 加入 2012-5-30

	if(pData->m_PLCToL2ID1->TempP9_1>100)        pData->m_coolStruct.MeaTemp.TempP9 = pData->m_PLCToL2ID1->TempP9_1;
	else if(pData->m_PLCToL2ID1->TempP9_2>100)   pData->m_coolStruct.MeaTemp.TempP9 = pData->m_PLCToL2ID1->TempP9_2;
	else if(pData->m_PLCToL2ID1->TempP9_3>100)   pData->m_coolStruct.MeaTemp.TempP9 = pData->m_PLCToL2ID1->TempP9_3;
	else 
		pData->m_coolStruct.MeaTemp.TempP9=0;    //谢谦 加入 2012-5-30

	if(pData->m_PLCToL2ID1->TempPcy_1>100)        pData->m_coolStruct.MeaTemp.TempPcy = pData->m_PLCToL2ID1->TempPcy_1;
	else if(pData->m_PLCToL2ID1->TempPcy_2>100)   pData->m_coolStruct.MeaTemp.TempPcy = pData->m_PLCToL2ID1->TempPcy_2;
	else if(pData->m_PLCToL2ID1->TempPcy_3>100)   pData->m_coolStruct.MeaTemp.TempPcy = pData->m_PLCToL2ID1->TempPcy_3;
	else pData->m_coolStruct.MeaTemp.TempPcy = 0;

	if(pData->m_PLCToL2ID1->TempPpl1_1>100)        pData->m_coolStruct.MeaTemp.TempPpl1 = pData->m_PLCToL2ID1->TempPpl1_1;
	else if(pData->m_PLCToL2ID1->TempPpl1_2>100)   pData->m_coolStruct.MeaTemp.TempPpl1 = pData->m_PLCToL2ID1->TempPpl1_2;
	else if(pData->m_PLCToL2ID1->TempPpl1_3>100)   pData->m_coolStruct.MeaTemp.TempPpl1 = pData->m_PLCToL2ID1->TempPpl1_3;
	else pData->m_coolStruct.MeaTemp.TempPpl1 = 0;
	


	////////////////////////////////////////////////////////////////////////


//////////////////////////////////////Process Reset ////////////////////////////////////////////////////////
	pData->Cool_status = pData->m_PLCToL2ID1->cool_used;
	if(Old_Cool_Mode!=pData->m_PLCToL2ID1->cool_used)
	{
		mystr.Format("Cool Used Changed Process Reset!");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,7);
		Old_Cool_Mode=pData->m_PLCToL2ID1->cool_used;
	}
	::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,6);

//	if(pData->m_PLCToL2ID1->TempP4 < 350)
//		pData->m_PLCToL2ID1->TempP4 = pData->m_PLCToL2ID1->TempP5;
//	fp = fopen("TempP4.txt","a");
//	fprintf(fp,"\n%f",pData->m_PLCToL2ID1->TempP4);
//	fclose(fp);

/////////////////////////////////////倒数第三道次抛钢，预计算//////////////////////////////////////////////	
	if((3 == pData->m_PLCToL2ID1->sch_cal_trigger)&&(3 != old_sch_cal_trigger))
	{
//		LP_trigger = 1;
		mystr.Format("sch_cal_trigger = 3");
		//mystr.Format("sch_cal_trigger = 1");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 

		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,14);   // xie 8-17 移动至抛刚信号  //modify by xie add 14
	}
	
////////////////////////////////////T1高温计检测到信号后，在线计算////////////////////////////////////////
	if((1 == pData->m_PLCToL2ID1->sch_cal_trigger)&&(1 != old_sch_cal_trigger))
	{
		LP_triggertime = GetTickCount();
		LP_trigger = 1;
		mystr.Format("sch_cal_trigger = 1");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
				
		/*
		HWND pHWnd=::FindWindow(NULL,pData->strServerName);    //谢谦 加入 2012-7-4  pre level通信
		if(pHWnd)
			::SendMessage(pHWnd,WM_SEVER_CLIENT_SOCKET_MSG,0,0);   //由于预计算改至末道次 与预矫直机通信移至预计算尾部
		*/
		mystr.Format("Send PDImessage to pre level");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,0);   // 谢谦 加入 2012-8-17

		//PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing","test spare int 2",pData->m_PLCToL2ID1->spare_int_1); 
		//online_trigger = 1;
	}		

	if(	
		((pData->CurIndex1>29)&&(pData->CurIndex1<33))||   //add by xie 在线计算触发一段时间 8-27
		((pData->CurIndex2>29)&&(pData->CurIndex2<33))||
		((pData->CurIndex3>29)&&(pData->CurIndex3<33))
	     //modify by xie 9-17
		/*
		(online_trigger == 1) && 
		(pData->m_coolStruct.MeaTemp.TempP1 > 600 ) &&  
		(	(0!=strcmp(pData->m_coolStruct.TrkMes[0].s_plate_number,"")  ) || 
		(0!=strcmp(pData->m_coolStruct.TrkMes[1].s_plate_number,"")  ) || 
		(0!=strcmp(pData->m_coolStruct.TrkMes[2].s_plate_number,"")  ) || 
		(0!=strcmp(pData->m_coolStruct.TrkMes[3].s_plate_number,"")  ) || 
		(0!=strcmp(pData->m_coolStruct.TrkMes[4].s_plate_number,"")  ) )
         */   //谢谦 屏蔽 8-23	
		)//谢谦加入判定条件 一级有信息 二级 无跟踪信息时不进行 在线计算
	{
			//mystr.Format("online calculate trigger");
			//PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0);   //暂不输出此语句
			::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,1);
	}
	/*
   if(
	   (online_trigger == 1)
	   &&(pData->m_coolStruct.MeaTemp.TempP1 <= 600)
	   &&(old_TempPcy > 600)
	   )
		online_trigger = 0;
   */

	//////////////////////////////////压头在线计算  xie 10-17/////////////////////// 

	if(	
		(1== pData->m_PLCToL2ID1->spare_int_1)   //add by xie spare_int_2 为 压头标志位
		)
	{
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,13);
	}
	
//////////////////////////////////由冷前区域返回轧制区域///////////////////////
	if(//(LP_trigger == 1)&&
	   (((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_1 == 2)&&(old_GfcPltCoolZoneChange_1 != 2))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_2 == 2)&&(old_GfcPltCoolZoneChange_2 != 2))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_3 == 2)&&(old_GfcPltCoolZoneChange_3 != 2))))
	{
		mystr.Format("SpData->m_PLCToL2ID1->GfcPltCoolZoneChange = 2!");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,8);
		LP_trigger = 0;
   }
	
///////////////////////////////////进入冷却区域//////////////////////////////////
	if(//(LP_trigger == 1)&&
	   (//((GetTickCount() - LP_triggertime)>100000 )||
	   (((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_1 == 3)&&(old_GfcPltCoolZoneChange_1 != 3))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_2 == 3)&&(old_GfcPltCoolZoneChange_2 != 3))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_3 == 3)&&(old_GfcPltCoolZoneChange_3 != 3)))))
	{
		enter_czone_triggertime =  GetTickCount();
		enter_czone_trigger = 1;
		mystr.Format("SpData->m_PLCToL2ID1->GfcPltCoolZoneChange = 3!");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,2);
		LP_trigger = 0;
		pData->Lpt_flg = 0;
		start_onl_trigger = 0;
				/*
		FILE *testLevel;
		testLevel=fopen("testLevel.txt","a+");		
		fprintf(testLevel,"\n\t plateID= %s,entry gap=%5.1f",pData->m_SOCKET2ToL2ID2->piece_id,pData->m_SOCKET2ToL2ID2->entry_gap);
		fclose(testLevel);
		*/
	}
/////////////////////////////////由在冷区域返回冷前区域///////////////////////
		if(//(enter_czone_trigger == 1)&&
	   (((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_1 == 4)&&(old_GfcPltCoolZoneChange_1 != 4))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_2 == 4)&&(old_GfcPltCoolZoneChange_2 != 4))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_3 == 4)&&(old_GfcPltCoolZoneChange_3 != 4))))
	{
		LP_trigger = 1 ;
		mystr.Format("SpData->m_PLCToL2ID1->GfcPltCoolZoneChange = 4!");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,9);
		enter_czone_trigger = 0;

	}

//////////////////////////////////离开冷却区域/////////////////////////////////
	if(//(enter_czone_trigger == 1)&&
	   (//((GetTickCount() - enter_czone_triggertime)>100000 )||
	   (((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_1 == 5)&&(old_GfcPltCoolZoneChange_1 != 5))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_2 == 5)&&(old_GfcPltCoolZoneChange_2 != 5))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_3 == 5)&&(old_GfcPltCoolZoneChange_3 != 5)))))
	{
		out_czone_triggertime =  GetTickCount();
		out_czone_trigger = 1;
		mystr.Format("SpData->m_PLCToL2ID1->GfcPltCoolZoneChange = 5!");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,3);
		enter_czone_trigger = 0;

	}


////////////////////////////////由冷后区域返回在冷区域//////////////////////////////
	if(//(out_czone_trigger == 1)&&
	   (((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_1 == 6)&&(old_GfcPltCoolZoneChange_1 != 6))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_2 == 6)&&(old_GfcPltCoolZoneChange_2 != 6))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_3 == 6)&&(old_GfcPltCoolZoneChange_3 != 6))))
	{
		enter_czone_trigger = 1;
		mystr.Format("SpData->m_PLCToL2ID1->GfcPltCoolZoneChange = 6!");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,10);
		out_czone_trigger = 0;
	}

////////////////////////////////离开返红高温计，结束宏跟踪//////////////////////////////
	if(//(out_czone_trigger == 1)&&
	   (//((GetTickCount() - out_czone_triggertime)>100000 )||
	   (((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_1 == 7)&&(old_GfcPltCoolZoneChange_1 != 7))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_2 == 7)&&(old_GfcPltCoolZoneChange_2 != 7))||
		((pData->m_PLCToL2ID1->GfcPltCoolZoneChange_3 == 7)&&(old_GfcPltCoolZoneChange_3 != 7)))))
	{
		mystr.Format("SpData->m_PLCToL2ID1->GfcPltCoolZoneChange = 7!");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,4);
		Sleep(50);  // 谢谦 加入 试验 2012-7-7
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,5);
		out_czone_trigger = 0;
	}


	if(     //add by xie 11-27
		(1==pData->m_PLCToL2ID1->spare_int_3 )
		&&(0==oldManualWaterfactorTrigger)
		)
	{
		mystr.Format("手动修正全自动水比 系数");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,15);
	}
	
	if(     //add by xie 11-27
		(1==pData->m_PLCToL2ID1->spare_int_6 )
		&&(0==oldManualBackRedTrigger)
		)
	{
		mystr.Format("手动修正返红温度层别");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,16);
	}
	
	old_GfcPltCoolZoneChange_1 = pData->m_PLCToL2ID1->GfcPltCoolZoneChange_1;
	old_GfcPltCoolZoneChange_2 = pData->m_PLCToL2ID1->GfcPltCoolZoneChange_2;
	old_GfcPltCoolZoneChange_3 = pData->m_PLCToL2ID1->GfcPltCoolZoneChange_3;
	old_sch_cal_trigger	= pData->m_PLCToL2ID1->sch_cal_trigger;
	old_TempPcy = pData->m_coolStruct.MeaTemp.TempP1;

	oldManualWaterfactorTrigger=pData->m_PLCToL2ID1->spare_int_3;   //add by xie 11-27
	oldManualBackRedTrigger=pData->m_PLCToL2ID1->spare_int_6;   //add by xie 12-1

 //谢谦 加入 2012-7-2
	strcpy(old_sch_socket_pieceid,pData->m_SOCKET1ToL2ID1->pieceid);  

/*
	if(1 == pData->Lpt_flg)
	{
		if(0==CurIndexFlg)
		{
			pData->m_coolStruct.TrkFrtMes.CurIndex = 0;
			CurIndexFlg = 1;
		}

		if(pData->m_PLCToL2ID1->TempP1_1 > 650 )
		{

			removeSpaces(pData->m_PLCToL2ID1->TrkID_1);
			strcmp(pData->m_coolStruct.TrkFrtMes.s_plate_number,pData->m_PLCToL2ID1->TrkID_1);


			if(pData->m_PLCToL2ID1->TimeSinceP1_1 < 0)
			{
				pData->m_PLCToL2ID1->TimeSinceP1_1 = 0.0;
			}
			else if(pData->m_PLCToL2ID1->TimeSinceP1_1 > 1000000)
			{
				pData->m_PLCToL2ID1->TimeSinceP1_1 = 0.0;
			}

			if(pData->m_PLCToL2ID1->Speed_1 < 0)
			{
				pData->m_PLCToL2ID1->Speed_1 = 0.0;
			}
			else if(pData->m_PLCToL2ID1->Speed_1 > 10)
			{
				pData->m_PLCToL2ID1->Speed_1 = 0.0;
			}


			if(pData->m_PLCToL2ID1->Position_1 < 0)
			{
				pData->m_PLCToL2ID1->Position_1 = 0.0;
			}
			else if(pData->m_PLCToL2ID1->Position_1 > 200000)
			{
				pData->m_PLCToL2ID1->Position_1 = 0.0;
			}


			if(pData->m_PLCToL2ID1->TempP1_1 < 601)
			{
				pData->m_PLCToL2ID1->TempP1_1 = 0.0;
			}
			else if(pData->m_PLCToL2ID1->TempP1_1 > 1800)
			{
				pData->m_PLCToL2ID1->TempP1_1 = 0.0;
			}


			if(pData->m_PLCToL2ID1->TempP2_1 < 601)
			{
				pData->m_PLCToL2ID1->TempP2_1 = 0.0;
			}
			else if(pData->m_PLCToL2ID1->TempP2_1 > 1800)
			{
				pData->m_PLCToL2ID1->TempP2_1 = 0.0;
			}



			if(pData->m_PLCToL2ID1->TempP3_1 < 401)
			{
				pData->m_PLCToL2ID1->TempP3_1 = 0.0;
			}
			else if(pData->m_PLCToL2ID1->TempP3_1 > 1800)
			{
				pData->m_PLCToL2ID1->TempP3_1 = 0.0;
			}

			if(pData->m_PLCToL2ID1->TempP4_1 < 401)
			{
				pData->m_PLCToL2ID1->TempP4_1 = 0.0;
			}
			else if(pData->m_PLCToL2ID1->TempP4_1 > 1800)
			{
				pData->m_PLCToL2ID1->TempP4_1 = 0.0;
			}

			if(pData->m_PLCToL2ID1->TempP5_1 < 401)
			{
				pData->m_PLCToL2ID1->TempP5_1 = 0.0;
			}
			else if(pData->m_PLCToL2ID1->TempP5_1 > 1800)
			{
				pData->m_PLCToL2ID1->TempP5_1 = 0.0;
			}

			pData->m_coolStruct.TrkFrtMes.TimeSinceP1[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_PLCToL2ID1->TimeSinceP1_1;
			pData->m_coolStruct.TrkFrtMes.Speed[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_PLCToL2ID1->Speed_1;

			pData->m_coolStruct.TrkFrtMes.Position[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_PLCToL2ID1->Position_1/1000.0;
			pData->m_coolStruct.TrkFrtMes.TempP1[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_PLCToL2ID1->TempP1_1; 

			if( pData->m_coolStruct.TrkFrtMes.CurIndex == 0)
			{
				pData->m_coolStruct.TrkFrtMes.TimeSinceP1[pData->m_coolStruct.TrkFrtMes.CurIndex] = 0.0;
				pData->m_coolStruct.TrkFrtMes.Position[pData->m_coolStruct.TrkFrtMes.CurIndex] = 13.4;
				pData->m_coolStruct.TrkFrtMes.TempP1[pData->m_coolStruct.TrkFrtMes.CurIndex] = 900;
				pData->m_coolStruct.TrkFrtMes.Speed[pData->m_coolStruct.TrkFrtMes.CurIndex] = 2.0;
			}
			else
			{
				pData->m_coolStruct.TrkFrtMes.TimeSinceP1[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_coolStruct.TrkFrtMes.CurIndex *0.1;
				pData->m_coolStruct.TrkFrtMes.Speed[pData->m_coolStruct.TrkFrtMes.CurIndex] = 3.0;
				pData->m_coolStruct.TrkFrtMes.Position[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_coolStruct.TrkFrtMes.Position[pData->m_coolStruct.TrkFrtMes.CurIndex-1]
						                                                                       + pData->m_coolStruct.TrkFrtMes.Speed[pData->m_coolStruct.TrkFrtMes.CurIndex] * 0.1;
				
//					pData->m_coolStruct.TrkFrtMes.TempP1[pData->m_coolStruct.TrkFrtMes.CurIndex] = 900 - 0.2*pData->m_coolStruct.TrkFrtMes.CurIndex;
			}

			
			for(iz = 0;iz < 8; iz++)
			{
//				pData->m_coolStruct.TrkFrtMes.TempP1[iz]  = pData->m_coolStruct.TrkFrtMes.TempP1[8];
			}



			pData->m_coolStruct.TrkFrtMes.TempP2[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_PLCToL2ID1->TempP2_1;
			pData->m_coolStruct.TrkFrtMes.TempP2[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_PLCToL2ID1->TempP2_1;


			pData->m_coolStruct.TrkFrtMes.TempP3[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_PLCToL2ID1->TempP3_1;
			pData->m_coolStruct.TrkFrtMes.TempP3[pData->m_coolStruct.TrkFrtMes.CurIndex] = pData->m_PLCToL2ID1->TempP3_1;
			pData->m_coolStruct.TrkFrtMes.TempP4[pData->m_coolStruct.TrkFrtMes.CurIndex]  = pData->m_PLCToL2ID1->TempP4_1;
			pData->m_coolStruct.TrkFrtMes.TempP5[pData->m_coolStruct.TrkFrtMes.CurIndex]  = pData->m_PLCToL2ID1->TempP5_1;





			pData->m_coolStruct.TrkFrtMes.CurIndex ++;

			if(	pData->m_coolStruct.TrkFrtMes.CurIndex>798)
			{
				pData->m_coolStruct.TrkFrtMes.CurIndex = 0;
			}

			FRT_UP_Ave = FRT_UP_Ave + pData->m_PLCToL2ID1->TempP1_1; 

			SumIndex1 ++;
			if(SumIndex1 > 10)
			{
				FRT_Run = true;
			}

			//////////////////////// 如果钢板经过P1位置，则进行在线修正计算  //////////////////////////////
			if((SumIndex1>20)&&((GetTickCount()-T_FRT_head_TriggerTime)>20000))
			{

				FRT_UP_Ave = FRT_UP_Ave/(SumIndex1+1);
				pData->m_coolStruct.MeaTemp.TFrtAve = FRT_UP_Ave;
				mystr.Format("SumIndex1 = %d FRT_UP_Ave = %f T_FRT signal trigger!",SumIndex1,FRT_UP_Ave);
				PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
				::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,1);

				SumIndex1 = 0;
				FRT_Run = false;
				T_FRT_head_TriggerTime = GetTickCount();
				pData->Lpt_flg = 0;
			}
		}
		else if ((1 == FRT_Run)&&((GetTickCount()-T_FRT_head_TriggerTime)>20000))
		{
			FRT_UP_Ave = FRT_UP_Ave/(SumIndex1+1);
			pData->m_coolStruct.MeaTemp.TFrtAve = FRT_UP_Ave;
			mystr.Format("SumIndex1 = %d FRT_UP_Ave = %f T_FRT signal trigger!",SumIndex1,FRT_UP_Ave);
			PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
			::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,1);

			SumIndex1 = 0;
			FRT_Run = false;

			T_FRT_head_TriggerTime = GetTickCount();
			pData->Lpt_flg = 0;
		}
	}

*/
/*
	if(!SCT_Process)
	{
		if(pData->m_PLCToL2ID1->TempP2_1 > SCT_CRI)
		{
			SCT_Num++;
			if(SCT_Num>10)
			{

				SCT_UP_Ave += pData->m_PLCToL2ID1->TempP2_1;
				SCT_DN_Ave += pData->m_PLCToL2ID1->TempP2_1;
				SCT_Run = true;
			}
			if(SCT_Num==20)
			{
				SCT_UP_Ave = SCT_UP_Ave / 10.0 ;
				pData->m_coolStruct.MeaTemp.TuSctAve = SCT_UP_Ave;
				mystr.Format("Tu_SCT_head=%.4f    SCT_Num=%d signal trigger!",SCT_UP_Ave,SCT_Num);
				PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 

				Tu_SCT_head_Trigger=1;
				Tu_SCT_head_TriggerTime=GetTickCount();
				
				SCT_Num = 0;
				SCT_Run = false;
				SCT_Process = true;
			}
	
		}
		else if(1==SCT_Run)
		{
			SCT_UP_Ave = SCT_UP_Ave / (SCT_Num-10.0) ;
			pData->m_coolStruct.MeaTemp.TuSctAve = SCT_UP_Ave;
			mystr.Format("Tu_SCT_head=%.4f    SCT_Num=%d signal trigger!",SCT_UP_Max,SCT_Num);
			PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
			
			Tu_SCT_head_Trigger=1;
			Tu_SCT_head_TriggerTime=GetTickCount();
			
			SCT_Num = 0;
			SCT_Run = false;
			SCT_Process = true;
		}
		
	}

	if((GetTickCount()-Tu_SCT_head_TriggerTime)>10000)
	{
//		SCT_Process = false;


	}




	if(!FCT_Process)
	{
		if(pData->m_PLCToL2ID1->TempP3_1 > FCT_CRI)
		{
			FCT_Num++;
			if(FCT_Num>10)
			{

				FCT_UP_Ave += pData->m_PLCToL2ID1->TempP3_1;
				FCT_DN_Ave += pData->m_PLCToL2ID1->TempP3_1;
				FCT_Run = true;
			}
			if(FCT_Num==20)
			{
				FCT_UP_Ave = FCT_UP_Ave / 10.0 ;
				pData->m_coolStruct.MeaTemp.TuFctAve = FCT_UP_Ave;
				mystr.Format("Tu_FCT_head=%.4f    FCT_Num=%d signal trigger!",FCT_UP_Ave,FCT_Num);
				PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 

				Tu_FCT_head_Trigger=1;
				Tu_FCT_head_TriggerTime=GetTickCount();
				
				FCT_Num = 0;
				FCT_Run = false;
				FCT_Process = true;


				SCT_Process = false;
			}
	
		}
		else if(1==FCT_Run)
		{
			mystr.Format("Tu_FCT_head=%.4f    FCT_Num=%d signal trigger!",FCT_UP_Max,FCT_Num);
			PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
			
			Tu_FCT_head_Trigger=1;
			Tu_FCT_head_TriggerTime=GetTickCount();
			
			FCT_Num = 0;
			FCT_Run = false;
			FCT_Process = true;


			SCT_Process = false;
		}
		
	}
*/
/*	if((GetTickCount()-Tu_FCT_head_TriggerTime)>15000)
	{
		FCT_Process = false;
	}
*/
	/*	if((pData->pPLCMemory->m_PLCToL2ID1FltPro.TempP3U_tail)&&(Tu_FCT_tail_Trigger==0))
	{
	mystr.Format("FCT_UP_Ave=%.4f signal trigger!",FCT_UP_Ave);
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,4);
		Tu_FCT_tail_TriggerTime=GetTickCount();
		CurIndexFlg = 0;
		FCT_UP_Max = -1;
		FCT_DN_Max = -1;
		FCT_UP_Ave = -1;
		FCT_Num = 0;
		FCT_Process = false;



		SCT_UP_Max = -1;
		SCT_DN_Max = -1;
		SCT_UP_Ave = -1;
		SCT_Num = 0;
		SCT_Process = false;
		Tu_FCT_tail_Trigger = 1;

		FCT_Process = false;

	}
		*/	
/*	
	if((GetTickCount()-Tu_FCT_head_TriggerTime)>60000)
	{
		Tu_FCT_tail_Trigger = 0;
	}


	if(!RBT_Process)
	{
		if(pData->m_PLCToL2ID1->TempP4_1 > RBT_CRI)
		{
			RBT_Num++;
			if(RBT_Num>10)
			{

				RBT_UP_Ave += pData->m_PLCToL2ID1->TempP4_1;

				RBT_Run = true;
			}
			if(RBT_Num==20)
			{
				RBT_UP_Ave = RBT_UP_Ave / 10.0 ;
				pData->m_coolStruct.MeaTemp.ThRbtAve = RBT_UP_Ave;

				mystr.Format("RBT_head=%.4f    RBT_Num=%d signal trigger!",RBT_UP_Ave,RBT_Num);
				PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 

				RBT_head_Trigger=1;
				RBT_head_TriggerTime=GetTickCount();
				
				RBT_Num = 0;
				RBT_Run = false;
				RBT_Process = true;


			}
	
		}
		else if(1==RBT_Run)
		{
			RBT_UP_Ave = RBT_UP_Ave/(RBT_Num-10);
			pData->m_coolStruct.MeaTemp.ThRbtAve = RBT_UP_Ave;
			mystr.Format("RBT_head=%.4f    RBT_Num=%d signal trigger!",RBT_UP_Max,RBT_Num);
			PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
			
			RBT_head_Trigger=1;
			RBT_head_TriggerTime=GetTickCount();
			
			RBT_Num = 0;
			RBT_Run = false;
			RBT_Process = true;


		}

	}


	if(!LRBT_Process)
	{
		if(pData->m_PLCToL2ID1->TempP5_1 > LRBT_CRI)
		{
			LRBT_Num++;
			if(LRBT_Num>10)
			{

				LRBT_UP_Ave += pData->m_PLCToL2ID1->TempP5_1;

				LRBT_Run = true;
			}
			if(LRBT_Num==20)
			{
				LRBT_UP_Ave = LRBT_UP_Ave / 10.0 ;
				pData->m_coolStruct.MeaTemp.TlRbtAve = LRBT_UP_Ave;
				mystr.Format("LRBT_head=%.4f    LRBT_Num=%d signal trigger!",LRBT_UP_Ave,LRBT_Num);
				PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 

				LRBT_head_Trigger=1;
				LRBT_head_TriggerTime=GetTickCount();
				
				LRBT_Num = 0;
				LRBT_Run = false;
				LRBT_Process = true;


			}
	
		}
		else if(1==LRBT_Run)
		{
			LRBT_UP_Ave = LRBT_UP_Ave/(LRBT_Num-10.0);
			pData->m_coolStruct.MeaTemp.TlRbtAve = LRBT_UP_Ave;
			mystr.Format("LRBT_head=%.4f    LRBT_Num=%d signal trigger!",LRBT_UP_Max,LRBT_Num);
			PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
			
			LRBT_head_Trigger=1;
			LRBT_head_TriggerTime=GetTickCount();
			
			LRBT_Num = 0;
			LRBT_Run = false;
			LRBT_Process = true;


		}

	}
*/

/*
	if(((pData->pPLCMemory->m_PLCToL2ID1FltPro.TempP4_tail)&& (pData->rbt_tar >= 300 )))
	{


		
		WriteRecord_TriggerTime=GetTickCount();
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,8);
		
		
		mystr.Format("T4_RBT=%.4f signal trigger!",RBT_UP_Ave);
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		RBT_tail_Trigger = 1;
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,5);
		RBT_UP_Max = -1;
		RBT_UP_Ave = -1;
		RBT_Num = 0;
		RBT_Process = false;

		LRBT_UP_Max = -1;
		LRBT_UP_Ave = -1;
		LRBT_Num = 0;
		LRBT_Process = false;

	}

*/

	//触发后计算，从文件中读取在线数据
/*	if(((pData->rbt_tar < 300 )&&(pData->pPLCMemory->m_PLCToL2ID1FltPro.TempP5_tail)))
	{


		
		WriteRecord_TriggerTime=GetTickCount();
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,8);
		
		
		mystr.Format("T5_RBT=%.4f signal trigger!",LRBT_UP_Ave);
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		LRBT_tail_Trigger = 1;
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,5);
		LRBT_UP_Max = -1;
		LRBT_UP_Ave = -1;
		LRBT_Num = 0;
		LRBT_Process = false;

		RBT_UP_Max = -1;
		RBT_UP_Ave = -1;
		RBT_Num = 0;
		RBT_Process = false;



	}
*/
/*	if((GetTickCount()-RBT_head_TriggerTime)>60000)
	{
		RBT_tail_Trigger = 0;

		RBT_UP_Max = -1;
		RBT_UP_Ave = -1;
		RBT_Num = 0;
		RBT_Process = false;
	
	
	}

	if((GetTickCount()-LRBT_head_TriggerTime)>60000)
	{
		LRBT_tail_Trigger = 0;

		LRBT_UP_Max = -1;
		LRBT_UP_Ave = -1;
		LRBT_Num = 0;
		LRBT_Process = false;


	}
*/
}
void TMEICToUFCL2ID1SignalProcess()
{
		//谢谦 轧机咬钢时发送就发送数据
	if(	
		(0 != strcmp(pData->m_SOCKET1ToL2ID1->pieceid,"")) 
		&& (0 != strcmp(old_sch_socket_pieceid,pData->m_SOCKET1ToL2ID1->pieceid))  
		)
	{
		//		LP_trigger = 1;
		mystr.Format("接收到新钢板信息");
		PrintMessage("CProcessCtlDlg","ProcessClientPLCEvent","nothing",mystr,0); 
		::PostThreadMessage(pData->p_track_thread->m_nThreadID,WM_TRACK_PLC_MSG,0,12);
	};
}
void TMEICToUFCL2ID2SignalProcess()
{
	;
}