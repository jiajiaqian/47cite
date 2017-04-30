#include "stdafx.h"
#include "PCFramework.h"
#include "MainFrm.h"

void UFCPLCToUFCL2ID1SignalProcess();
void TMEICToUFCL2ID1SignalProcess();
void TMEICToUFCL2ID2SignalProcess();

unsigned __stdcall UFCSignalProcess(void* pv)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

//	SetEvent(pApp->L2Sys->thread[TYPE_UFCSIGNALPROCESS].ThreadEvent[STATE_RUN]);
//	Sleep(200);
//	WaitForSingleObject(pApp->L2Sys->thread[TYPE_UFCSIGNALPROCESS].ThreadEvent[STATE_RUN], INFINITE);
	unsigned int req;
	int nIndex;
	while (1){
		nIndex=WaitForMultipleObjects(2,pApp->L2Sys->thread[TYPE_UFCSIGNALPROCESS].ThreadEvent, FALSE,INFINITE);
		if(nIndex==WAIT_OBJECT_0+1)	
		{
			SetEvent(pApp->L2Sys->thread[TYPE_UFCSIGNALPROCESS].ThreadEvent[STATE_STOP]);
			break;
		}else if(nIndex==WAIT_OBJECT_0){
			try{
				lockedmemcpy(&req,&(pApp->L2Sys->thread[TYPE_UFCSIGNALPROCESS].RequestWord),4);
				for(int i = 0; i < 32; i++){
					if(testRW(&req, i))
					{
						switch(i){
						case 0://PLCTOL2ID1
							UFCPLCToUFCL2ID1SignalProcess();
							break;
						case 1://SOCKETTOL2ID1
							logger::LogDebug("socket","收到 socket 101 message");  //  [10/29/2013 xq]
							TMEICToUFCL2ID1SignalProcess();
							break;
						case 2://SOCKETTOL2ID2
							logger::LogDebug("socket","收到 socket 102 message");  //  [10/29/2013 xq]
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
			catch(...){
			}
		}
	}
	return 0;
}

void UFCPLCToUFCL2ID1SignalProcess()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	int nPos =  pApp->s71->RecieveProcessFlag[0];
	int nObj =  pApp->s71->RecieveBufferFlag[0];

	for(int i = nPos; i != nObj; i++)
	{
		if(i == 31)  i = -1;
		memcpy(&pApp->s71->r1ontime,&pApp->s71->r1[i+1],sizeof(S7Line1R1Define));
		s7Track();
	}
	pApp->s71->RecieveProcessFlag[0] = nObj;
}

void TMEICToUFCL2ID1SignalProcess()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	int nPos =  pApp->sock1->ProcessRecieveBufferFlag[0];
	int nObj =  pApp->sock1->RecieveBufferFlag[0];

	int nSize = sizeof(GfcEXTL2PdiData);

	CString strInfo;

	//memcpy(&pApp->sock1->r1ontime,&pApp->sock1->r1[1], sizeof(GfcEXTL2PdiData));
	//socketTrack();

	for(int i = nPos; i != nObj; i++)
	{
		if(i == 31)  i = -1;
		memcpy(&pApp->sock1->r1ontime,&pApp->sock1->r1[i+1], sizeof(GfcEXTL2PdiData));

// 		strInfo.Format("%d   %s\r\n", i, pApp->sock1->r1ontime.GfcPieceId);
// 		OutputDebugString(strInfo);
		socketTrack();
	}

	pApp->sock1->ProcessRecieveBufferFlag[0] = nObj;
}

void TMEICToUFCL2ID2SignalProcess()
{
	
}


void s7Track()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	CMainFrame *myMainFrame=(CMainFrame *)pApp->GetMainWnd();

	CString mystr;
	static short old_sch_cal_trigger = 0;
	static short Old_Cool_Mode=0;
	static char old_sch_socket_pieceid[32]="empty";  //add by xieqian 2012-7-2
	static short oldManualWaterfactorTrigger = 0;
	static short oldManualBackRedTrigger = 0;
	static short old_RollSim_Trigger = 0; //add by Zed 2013.12.14
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
	static short old_Frt_trigger = 0;


	setTRB(7,6); //进行数据记录 datarealrecord
	SetEvent(pApp->L2Sys->thread[6].ThreadEvent[0]);

	////////////////////模拟轧制结束重置队列 add by Zed //////////////////////
	if((0==pApp->s71->r1ontime.RollSim)&&(0!=old_RollSim_Trigger))
	{
		setTRB(7,5);
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[0]);

	}
	////////////////////模拟轧制结束重置队列 add by Zed //////////////////////


////////////////////////////L1toL2模拟/////////////////////////
	//if((3 == pApp->s71->r1ontime.sch_cal_trigger)&&(3 != old_sch_cal_trigger)&&(1==TriggerLock[0]))///////////add by Zed 避免重复触发
	if((1 == pApp->s71->r1ontime.sch_cal_trigger)&&(1 != old_sch_cal_trigger))   
	{
		//SocketLine1Define *sock1=pApp->sock1;
		pApp->ufcs->CurPltIcFlag=0;
		//Auto_SendPdiToPLCWithParam(&sock1->r1ontime);
		setTRB(7,1);//咬钢信号,触发预计算
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[0]);
	}

	//if((2 == pApp->s71->r1ontime.sch_cal_trigger)&&(2 != old_sch_cal_trigger))  // 屏蔽  [3/26/2014 谢谦]
	//{
	//	//SocketLine1Define *sock1=pApp->sock1;
	//	pApp->ufcs->CurPltIcFlag=0;
	//	//Auto_SendPdiToPLCWithParam(&sock1->r1ontime);
	//	setTRB(7,1);//咬钢信号,触发预计算
	//	SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[0]);
	//}
	

/////////////////////////////////////倒数第三道次抛钢，预计算//////////////////////////////////////////////	
	//if((3 == pApp->s71->r1ontime.sch_cal_trigger)&&(3 != old_sch_cal_trigger))
	//	mystr.Format("sch_cal_trigger = 3");

	
////////////////////////////////////T1高温计检测到信号后，在线计算////////////////////////////////////////
	//if((1 == pApp->s71->r1ontime.sch_cal_trigger)&&(1 != old_sch_cal_trigger)&&(1==TriggerLock[1]))///////////add by Zed 避免重复触发
	////////////////////暂不用重复处罚预计算 modified by Zed /////////////////
	//if((1 == pApp->s71->r1ontime.sch_cal_trigger)&&(1 != old_sch_cal_trigger))
	//{
	//	LP_triggertime = GetTickCount();
	//	LP_trigger = 1;
	//	setTRB(7,1);//末3道次抛钢信号,触发预计算
	//	SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[0]);
	//	TriggerLock[1] = 0;
	//	TriggerLock[2] = 1;
	//}		
	////////////////////暂不用重复处罚预计算 modified by Zed /////////////////
	
	if(	
		//((pApp->s71->r1ontime.CurIndex_1>29)&&(pApp->s71->r1ontime.CurIndex_1<33))||   //add by xie 在线计算触发一段time 8-27
		//((pApp->s71->r1ontime.CurIndex_2>29)&&(pApp->s71->r1ontime.CurIndex_2<33))||
		//((pApp->s71->r1ontime.CurIndex_3>29)&&(pApp->s71->r1ontime.CurIndex_3<33))
		//(1==pApp->s71->r1ontime.frtTrigger)  &&( 1!=old_Frt_trigger)&&(1==TriggerLock[2]))///////////add by Zed 避免重复触发
		(1==pApp->s71->r1ontime.frtTrigger)  &&( 1!=old_Frt_trigger))//谢谦加入判定条件 一级有信息 二级 无跟踪信息时不进行 在线计算
	{
			setTRB(7,2);
			SetEvent(pApp->L2Sys->thread[6].ThreadEvent[0]);
			
	}

	old_Frt_trigger =pApp->s71->r1ontime.frtTrigger;  // add [11/19/2013 谢谦]
			


//////////////////////////////////由冷前区域返回轧制区域///////////////////////
	if(//(LP_trigger == 1)&&
	   (((pApp->s71->r1ontime.GfcPltCoolZoneChange_1 == 2)&&(old_GfcPltCoolZoneChange_1 != 2))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_2 == 2)&&(old_GfcPltCoolZoneChange_2 != 2))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_3 == 2)&&(old_GfcPltCoolZoneChange_3 != 2))))
	{
		LP_trigger = 0;
    }
	
///////////////////////////////////进入冷却区域//////////////////////////////////
	if(//(LP_trigger == 1)&&
	   (//((GetTickCount() - LP_triggertime)>100000 )||
	   (((pApp->s71->r1ontime.GfcPltCoolZoneChange_1 == 3)&&(old_GfcPltCoolZoneChange_1 != 3))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_2 == 3)&&(old_GfcPltCoolZoneChange_2 != 3))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_3 == 3)&&(old_GfcPltCoolZoneChange_3 != 3)))))
	{
		enter_czone_triggertime =  GetTickCount();
		enter_czone_trigger = 1;
	
		setTRB(7,3);//自动触发钢板进入冷却区域的程序
		SetEvent(pApp->L2Sys->thread[6].ThreadEvent[0]);

		LP_trigger = 0;
		start_onl_trigger = 0;
	}
/////////////////////////////////由在冷区域返回冷前区域///////////////////////
		if(//(enter_czone_trigger == 1)&&
	   (((pApp->s71->r1ontime.GfcPltCoolZoneChange_1 == 4)&&(old_GfcPltCoolZoneChange_1 != 4))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_2 == 4)&&(old_GfcPltCoolZoneChange_2 != 4))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_3 == 4)&&(old_GfcPltCoolZoneChange_3 != 4))))
	{
		LP_trigger = 1 ;
		setTRB(7,7);//冷前区域到轧制区域
		enter_czone_trigger = 0;

	}

//////////////////////////////////离开冷却区域/////////////////////////////////
	if(//(enter_czone_trigger == 1)&&
	   (//((GetTickCount() - enter_czone_triggertime)>100000 )||
	   (((pApp->s71->r1ontime.GfcPltCoolZoneChange_1 == 5)&&(old_GfcPltCoolZoneChange_1 != 5))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_2 == 5)&&(old_GfcPltCoolZoneChange_2 != 5))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_3 == 5)&&(old_GfcPltCoolZoneChange_3 != 5)))))
	{
		out_czone_triggertime =  GetTickCount();
		out_czone_trigger = 1;
	
		setTRB(7,7);//自动触发钢板出冷却区的跟踪程序(tail_Tu_FCT)
		SetEvent(pApp->L2Sys->thread[6].ThreadEvent[0]);

		enter_czone_trigger = 0;

	}
	
	////////////////////////////////由冷后区域返回在冷区域//////////////////////////////
	if(//(out_czone_trigger == 1)&&
	   (((pApp->s71->r1ontime.GfcPltCoolZoneChange_1 == 6)&&(old_GfcPltCoolZoneChange_1 != 6))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_2 == 6)&&(old_GfcPltCoolZoneChange_2 != 6))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_3 == 6)&&(old_GfcPltCoolZoneChange_3 != 6))))
	{
		enter_czone_trigger = 1;
		setTRB(7,8);////在冷区域到冷前区域
		out_czone_trigger = 0;
	}

////////////////////////////////离开返红高温计，结束宏跟踪//////////////////////////////
	if(//(out_czone_trigger == 1)&&
	   (//((GetTickCount() - out_czone_triggertime)>100000 )||
	   (((pApp->s71->r1ontime.GfcPltCoolZoneChange_1 == 7)&&(old_GfcPltCoolZoneChange_1 != 7))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_2 == 7)&&(old_GfcPltCoolZoneChange_2 != 7))||
		((pApp->s71->r1ontime.GfcPltCoolZoneChange_3 == 7)&&(old_GfcPltCoolZoneChange_3 != 7)))))
	{
		//Sleep(50);  // 谢谦 加入 试验 2012-7-7
		setTRB(7,4);//自动触发跟踪结束处理程序(after_T_RBT)
		out_czone_trigger = 0;
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[0]);   // add [3/22/2014 xie]
	}
	
	if( (1==pApp->s71->r1ontime.spare_int_2 )&&(0==oldManualWaterfactorTrigger))	{

		setTRB(7,11);
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[0]);  // add [3/22/2014 xie]
	
	}

		

	oldManualWaterfactorTrigger=pApp->s71->r1ontime.spare_int_2;

	
	old_GfcPltCoolZoneChange_1 = pApp->s71->r1ontime.GfcPltCoolZoneChange_1;
	old_GfcPltCoolZoneChange_2 = pApp->s71->r1ontime.GfcPltCoolZoneChange_2;
	old_GfcPltCoolZoneChange_3 = pApp->s71->r1ontime.GfcPltCoolZoneChange_3;
	old_sch_cal_trigger	= pApp->s71->r1ontime.sch_cal_trigger;
	old_RollSim_Trigger = pApp->s71->r1ontime.RollSim;

	strcpy_s(old_sch_socket_pieceid, pApp->sock1->r1[0].GfcPieceId);
}

void socketTrack()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	//CMainFrame *myMainFrame=(CMainFrame *)pApp->GetMainWnd();
	SocketLine1Define *sock1=pApp->sock1;
	S7Line1Define *s71=pApp->s71;
	///////////////////PDI队列删除 add by Zed//////////////////////
	if(5==sock1->r1ontime.GfcIdTsk)
	{
		setTRB(7,9);
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);
		logger::LogDebug("system","5==sock1->r1ontime.GfcIdTsk");
		return;
	}
	///////////////////PDI队列删除 add by Zed//////////////////////
		
	///////////////////增加对TMEIC 101 的响应 发送2011 add by Zed 2013.12.5/////////////////
	if((1==sock1->r1ontime.GfcProdState)&&(4==sock1->r1ontime.GfcIdTsk))
	{
		setTRB(7,8);
		SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);
		return;
	}

	///////////////////增加对TMEIC 101 的响应 发送2011 add by Zed 2013.12.5/////////////////
	/*if (
		(1==sock1->r1ontime.GfcRollPassNumNow)&&(sock1->r1ontime.GfcPltIcFlg[0] == '0'))*/

	//if((1==sock1->r1ontime.GfcRollPassNumNow)||(2==sock1->r1ontime.GfcRollPassNumNow))//by Zed  // 临时屏蔽 没有轧制道次 [11/5/2013 谢谦]
	//{
	
	s71->s1->valve_set_2=0;

	if ((2==sock1->r1ontime.GfcProdState)&&(5!=sock1->r1ontime.GfcIdTsk))
	{
		pApp->ufcs->CurPltIcFlag=0;
		
		if (sock1->r1ontime.GfcPltIcFlg[0]=='1')
		{
			if (sock1->r1ontime.GfcRollPassNumNow==sock1->r1ontime.GfcTcr1RollPassNumSum-1)
			{
				pApp->ufcs->CurPltIcFlag=1;
				Auto_SendPdiToPLCWithParam(&sock1->r1ontime);
				s71->s1->valve_set_2=2;
				SendScheduleToPLCTrigger();

				logger::LogDebug("system","中间坯道次 now=%d ,Sum1=%d ",sock1->r1ontime.GfcRollPassNumNow,sock1->r1ontime.GfcTcr1RollPassNumSum);  // add [6/26/2014 谢谦]
				return;
			}

			if (sock1->r1ontime.GfcRollPassNumNow==sock1->r1ontime.GfcTcr1RollPassNumSum)
			{
				s71->s1->valve_set_2=1;
				SendScheduleToPLCTrigger();

				logger::LogDebug("system","中间坯道次 now=%d ,Sum1=%d ",sock1->r1ontime.GfcRollPassNumNow,sock1->r1ontime.GfcTcr1RollPassNumSum);   // add [6/26/2014 谢谦]

				setTRB(7,1);//咬钢信号,触发预计算
				SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[0]);
				// 需在预计算处添加 相应的判断条件 [2/19/2014 xie]
				return;
			}
		}		

		if (sock1->r1ontime.GfcRollPassNumSum-sock1->r1ontime.GfcRollPassNumNow>2) 
			return;  // 最后三道次才生成PDI [2/22/2014 谢谦]

		if (1==sock1->r1ontime.GfcRollPassNumSum-sock1->r1ontime.GfcRollPassNumNow)
		{
			Auto_SendPdiToPLCWithParam(&sock1->r1ontime);
		}
		
		//pMainFrm->RefreshCtrlPanel();
		return;
	}			

	

	//	return;
	//}

	// 功能待完成 [9/23/2013 谢谦]
	//if (
	//(sock1->r1ontime.GfcTcr1RollPassNumSum==sock1->r1ontime.GfcRollPassNumNow)&&(sock1->r1ontime.GfcPltIcFlg[0] == '1')
	//)
	//{
	//	pApp->ufcs->CurPltIcFlag=1;
	//	setTRB(7,1);  // 线程号+1 设定的任务号 +1  [9/22/2013 谢谦]
	//	SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);
	//	return;
	//}
	//// 功能待完成 [9/23/2013 谢谦]
	//if (
	//	(sock1->r1ontime.GfcTcr2RollPassNumSum==sock1->r1ontime.GfcRollPassNumNow)&&(sock1->r1ontime.GfcPltIcFlg[0] == '2')
	//	)
	//{
	//	pApp->ufcs->CurPltIcFlag=2;
	//	setTRB(7,1);  // 线程号+1 设定的任务号 +1  [9/22/2013 谢谦]
	//	SetEvent(pApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);
	//	return;
	//}

	return;
}