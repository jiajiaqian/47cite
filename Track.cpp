#include "stdafx.h"
#include "PCFramework.h"
#include "MainFrm.h"
#include "PCFrameWorkView.h"
#include "S7Line1.h"
#include "SAPI_DB.h"

//////////////////////////////////////////////////////////
bool SelfLearnChoose;
bool SendDataToHMI;
bool offLineSelfLearn;
int autozip_flag;
int     mode_tag;    //model return
//////////////////////////////////////////////////////////
unsigned __stdcall Track(void* pv)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	SetEvent(pApp->L2Sys->thread[6].ThreadEvent[0]);
	Sleep(200);
	WaitForSingleObject(pApp->L2Sys->thread[6].ThreadEvent[0], INFINITE);
	unsigned int req;
	int nIndex;
	while (1) 
	{
		nIndex=WaitForMultipleObjects(2,pApp->L2Sys->thread[6].ThreadEvent, FALSE,INFINITE);
		if(nIndex==WAIT_OBJECT_0+1)
		{
			SetEvent(pApp->L2Sys->thread[6].ThreadEvent[1]);
			break;
		}
		else if(nIndex==WAIT_OBJECT_0){
			try{
				lockedmemcpy(&req,&(pApp->L2Sys->thread[6].RequestWord),4);  
				for(int i = 0; i < 32; i++){
					if(testRW(&req, i)){
						switch(i)
						{
						case 0://ģ��Ԥ����
							Auto_ModelPreCal();
							break;
						case 1://
							Auto_ModelFrtModiCal();
							break;
						case 2://
							Auto_TrackPltEnterCZone();
							break;
						case 3://
							Auto_TrackEndTreat();
							break;
						case 4://
							ProcessReset();
							break;
						case 5://
							DataRealRecord();
							break;
						case 6://
							Auto_TrackPltOutCZone();
							break;
						case 7://time request

							switch(pApp->sock1->r1[0].GfcPltIcFlg[0])
							{
							case 0:
								pApp->ufcs->CurPltIcFlag=0;
								AutoCalcTimeInFurnance();
								break;
							case 1:
								pApp->ufcs->CurPltIcFlag=0;
								AutoCalcTimeInFurnance();
								pApp->ufcs->CurPltIcFlag=1;
								AutoCalcTimeInFurnance();
								break;
							case 2:
								pApp->ufcs->CurPltIcFlag=0;
								AutoCalcTimeInFurnance();
								pApp->ufcs->CurPltIcFlag=2;
								AutoCalcTimeInFurnance();
								break;
							case 3:
								pApp->ufcs->CurPltIcFlag=0;
								AutoCalcTimeInFurnance();
								pApp->ufcs->CurPltIcFlag=1;
								AutoCalcTimeInFurnance();
								pApp->ufcs->CurPltIcFlag=2;
								AutoCalcTimeInFurnance();
								break;
							}
							break;
						case 8:
							AccidentDispose();
							break;
						case 10:
							ManualChangeAcce();  // add [3/24/2014 лǫ]
							break;
						case 31://��ʾ�̶߳Ի�������еĻ�
							break;
						default:
							break;
						}
						resetTRB(5,i);
					}
				}
				CMainFrame* pMainFrm = (CMainFrame*)pApp->GetMainWnd();
				CPCFrameWorkView *PView = (CPCFrameWorkView *)pMainFrm->GetActiveView();
				PView->Invalidate(TRUE);
			}
			catch(...){
			}
		}
	}
	return 0;
}

//������Ϣ��ӡ 070807 by lc
void AlarmPrint(int error_flg,char *message, double par1,double par2)
{
	FILE *fp;
	char file_name[80];

	if(error_flg!=0){
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		sprintf(file_name,"%s_%d-%d-%d.txt",ERR_FIL,systime.wYear,systime.wMonth,systime.wDay);
		fp=fopen(file_name,"a+");
		if(fp==NULL)
			return;
		fprintf(fp,"%d/%d/%d,%d:%d:%d\n",systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);
		fprintf(fp,"%s, %d, %f, %f\n",message,error_flg,par1,par2);
		int numberFlush=0;
		numberFlush=_flushall();
		fprintf(fp,"numberFlush  %d  \n\n",numberFlush);
		fclose(fp);
	}
}


void AutoCalcTimeInFurnance()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;
	SocketLine1Define  *sock1=theApp->sock1;
	

	int  i=0;
	int  err_flg = 1;
	int  track_flg = 0;
	memset(&ufcs->furTrackData,0,sizeof(TRACKDATARAL));
	AlarmPrint(err_flg,"����AutoCalcTimeInFurnance",0.0,0.0);	

	//logger::LogDebug("system","��������¯����ȴʱ��");

	Infoinit::InsertPDIToPreCalcStruct(&ufcs->furTrackData, &theApp->sock1->r1ontime,ufcs->CurPltIcFlag);
	
	/////////////////test by Zed 2013.12.5/////////////////////////
	//strcpy(ufcs->RefreshTrackData.RalPdi.plateID,"123");
	
	/////////////////test by Zed 2013.12.5/////////////////////////
	ufcs->trk.learnFlg = 1;   //2013-6-6 ʵ����ʱ���
	ufcs->trk.Cool_status =ufcs->Cool_status;

	if(strcmp(ufcs->furTrackData.RalPdi.plateID,"")==0)
		return;

	// лǫ ���� 2012-7-4  �����жϸ���׼ȷ��
	//ufcs->RefreshTrackData = ufcs->FINISH_ROLL;

	ufcs->furTrackData.RalPdi.op_mode = ufcs->trk.oper_mode;
	ufcs->furTrackData.RalPdi.AdaptFlg = ufcs->trk.learnFlg;
	ufcs->furTrackData.RalPdi.CoolStatus = ufcs->trk.Cool_status;
	int question_1=0;  //лǫ ���� 2012-7-10
	question_1= Infoinit::GetLayoutData(&ufcs->furTrackData);
	if (-1==question_1)
		return;

	ufcs->furTrackData.RalCoeff.AlphaCoe = 1.0;
	ufcs->furTrackData.RalCoeff.AlphaCoeIndex = 1.0;
	ufcs->furTrackData.RalCoeff.AlphaCoeAccWatTemp = 1.0;

	if((ufcs->furTrackData.RalCoeff.AlphaCoe < 0.5)||(ufcs->furTrackData.RalCoeff.AlphaCoe>1.5)) 
		ufcs->furTrackData.RalCoeff.AlphaCoe = 1.0;

	ufcs->furTrackData.RalCoeff.WaterTemp = s71->r1ontime.water_temp;

	if(ufcs->furTrackData.RalCoeff.WaterTemp > 50 || ufcs->furTrackData.RalCoeff.WaterTemp < 1.0)
		ufcs->furTrackData.RalCoeff.WaterTemp = 23.0; 

	ufcs->furTrackData.RalCoeff.WaterPress = s71->r1ontime.water_press;
	if(ufcs->furTrackData.RalCoeff.WaterPress >0.5|| ufcs->furTrackData.RalCoeff.WaterPress <0.2)
		ufcs->furTrackData.RalCoeff.WaterPress = 0.2f;

	ufcs->furTrackData.RalCoeff.AirTemp = 8.0f;

	ufcs->furTrackData.RalCoeff.AlphaCoeAccWatTemp = GetAlphaCoeWatTemp(ufcs->furTrackData.RalCoeff.WaterTemp);

	ufcs->furTrackData.RalCoeff.AlphaCoe = ufcs->furTrackData.RalCoeff.AlphaCoe * ufcs->furTrackData.RalCoeff.AlphaCoeAccWatTemp;

	int question=0; //лǫ ���� 2012-7-10

	try
	{
		question =Infoinit::CalculatePreCalcModelElite(&ufcs->furTrackData);
	}
	catch(...)
	{
		throw;
	}

	if (-1==question)
		return;
	////////////////////��ӦTMEIC 101 add by Zed 2013.12.5//////////////////
switch(ufcs->CurPltIcFlag)
{
case 0:
	strcpy_s(sock1->send2011.GfcPieceId,ufcs->furTrackData.RalPdi.plateID);
	strcpy_s(sock1->send2011.GfcPieceIdSpare,"spare");
	//strcpy_s(sock1->send2011.GfcPltMatId,ufcs->furTrackData.RalPdi.mat_id);
	strcpy_s(sock1->send2011.GfcPltMatIdSpare,"spare");
	//strcpy_s(sock1->send2011.GfcPltMatCode,ufcs->furTrackData.RalPdi.mat_id);
	sock1->send2011.GfcResPresetSpd = ufcs->furTrackData.RalCoeff.initSpeed;
	sock1->send2011.GfcPltAccType = ufcs->furTrackData.RalPdi.pass_mode;

	if((3==sock1->send2011.GfcPltAccType)||(2==sock1->send2011.GfcPltAccType))
	{
		sock1->send2011.GfcPltCoolMode1Pass=3;
	}
	else
	{
		sock1->send2011.GfcPltCoolMode1Pass=1;
	}
	
	SendGfcL2PresetAccResult_LP(&sock1->send2011);
	break;
case 1:
	break;
	
}
////////////////////��ӦTMEIC 101 add by Zed 2013.12.5//////////////////	
	
	DisplayData();	
	
	AlarmPrint(err_flg,"��ֹAuto_ModelPreCal",0.0,0.0);	
}



//�Զ����ù�̡�PDIԤ������
void Auto_ModelPreCal() 
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;
	SocketLine1Define  *sock1=theApp->sock1;

	int  i=0;
	int  err_flg = 1;
	int  track_flg = 0;
	///////////////////////������ƶ�����Ϣ��ȡ add by Zed 2013.12.10/////////////////////
	if (0==strcmp(ufcs->FINISH_ROLL[0].RalPdi.plateID,""))
	{
		logger::LogDebug("system","FINISH_ROLL[0] ID is null");  //  [10/29/2013 xq]
		return;
	}
		
	memset(&ufcs->RefreshTrackData,0,sizeof(TRACKDATARAL));

	ufcs->RefreshTrackData = ufcs->FINISH_ROLL[0];
	memset(&ufcs->FINISH_ROLL[0],0,sizeof(TRACKDATARAL ));

	AlarmPrint(err_flg,"����Auto_ModelPreCal",0.0,0.0);	
	logger::LogDebug("system","Auto_ModelPreCal-----------����Ԥ���� plate_id = %s",ufcs->RefreshTrackData.RalPdi.plateID);  //  [10/29/2013 xq]

//	char file_name[80];
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	//sprintf(file_name,"databasetrack%d-%d-%d.txt",systime.wYear,systime.wMonth,systime.wDay);
	//LogWrite(5,1,file_name,30);

	ufcs->trk.oper_mode=s71->r1ontime.OperateMode;  // ���� [11/13/2013 лǫ]

	ufcs->trk.Cool_status =ufcs->Cool_status;

	if(strcmp(ufcs->RefreshTrackData.RalPdi.plateID,"")==0)
		return;

	ufcs->RefreshTrackData.RalPdi.op_mode = ufcs->trk.oper_mode;

	ufcs->RefreshTrackData.RalPdi.AdaptFlg = ufcs->trk.learnFlg;
	ufcs->RefreshTrackData.RalPdi.CoolStatus = ufcs->trk.Cool_status;
	ufcs->RefreshTrackData.RalPdi.manualHC=ufcs->HeatCoefManual;  // add �����޸�hcϵ�� [11/15/2013 лǫ]
	
		int question_1=0;  //лǫ ���� 2012-7-10
	question_1= Infoinit::GetLayoutData(&ufcs->RefreshTrackData);
	if (-1==question_1)
		return;

	ufcs->RefreshTrackData.RalCoeff.AlphaCoe = 1.0;
	ufcs->RefreshTrackData.RalCoeff.AlphaCoeIndex = 1.0;
	ufcs->RefreshTrackData.RalCoeff.AlphaCoeAccWatTemp = 1.0;

	if((ufcs->RefreshTrackData.RalCoeff.AlphaCoe < 0.5)||(ufcs->RefreshTrackData.RalCoeff.AlphaCoe>1.5)) 
		ufcs->RefreshTrackData.RalCoeff.AlphaCoe = 1.0;

	ufcs->RefreshTrackData.RalCoeff.WaterTemp = s71->r1ontime.water_temp;

	if(ufcs->RefreshTrackData.RalCoeff.WaterTemp > 50 || ufcs->RefreshTrackData.RalCoeff.WaterTemp < 1.0)
		ufcs->RefreshTrackData.RalCoeff.WaterTemp = 23.0; 

	ufcs->RefreshTrackData.RalCoeff.WaterPress = s71->r1ontime.water_press;
	if(ufcs->RefreshTrackData.RalCoeff.WaterPress >0.5|| ufcs->RefreshTrackData.RalCoeff.WaterPress <0.2)
		ufcs->RefreshTrackData.RalCoeff.WaterPress = 0.2f;

	ufcs->RefreshTrackData.RalCoeff.AirTemp = 8.0f;

	ufcs->RefreshTrackData.RalCoeff.AlphaCoeAccWatTemp = GetAlphaCoeWatTemp(ufcs->RefreshTrackData.RalCoeff.WaterTemp);

	ufcs->RefreshTrackData.RalCoeff.AlphaCoe = ufcs->RefreshTrackData.RalCoeff.AlphaCoe * ufcs->RefreshTrackData.RalCoeff.AlphaCoeAccWatTemp;

	int question=0; //лǫ ���� 2012-7-10
	ufcs->RefreshTrackData.RalPdi.isCooling=1;

	if (1==ufcs->RefreshTrackData.RalPdi.isCooling)
	{
		try{
			question =Infoinit::CalculatePreCalcModel(&ufcs->RefreshTrackData);
		}
		catch(...){
			throw;
		}
	}
	else if (2==ufcs->RefreshTrackData.RalPdi.isCooling){
		ufcs->RefreshTrackData.RalPdi.acc_mode = EXP; //add by xie 2013-3-18
		ufcs->RefreshTrackData.RalPdi.retval=0;  //add by xie 2013-3-18
	}
	else{
		ufcs->RefreshTrackData.RalPdi.retval=0;  //add by xie 9-12
	}

	if (-1==question)
	{
		logger::LogDebug("system","Auto_ModelPreCal ������Ԥ�������� û�н��м���");
		SendScheduleToPLCDefult();
		SendSpeedToPLCTrigger();
		return;
	}
		
	strcpy_s(sock1->send2011.GfcPieceId,ufcs->RefreshTrackData.RalPdi.plateID);
	strcpy_s(sock1->send2011.GfcPieceIdSpare,"spare");
	//strcpy_s(sock1->send2011.GfcPltMatId,ufcs->RefreshTrackData.RalPdi.mat_id);
	strcpy_s(sock1->send2011.GfcPltMatIdSpare,"spare");
	//strcpy_s(sock1->send2011.GfcPltMatCode,ufcs->RefreshTrackData.RalPdi.mat_id);
	sock1->send2011.GfcResPresetSpd = ufcs->RefreshTrackData.RalCoeff.initSpeed;
	sock1->send2011.GfcPltAccType = ufcs->RefreshTrackData.RalPdi.pass_mode;

	if((3==sock1->send2011.GfcPltAccType)||(2==sock1->send2011.GfcPltAccType))
	{
		sock1->send2011.GfcPltCoolMode1Pass=3;
	}
	else
	{
		sock1->send2011.GfcPltCoolMode1Pass=1;
	}

	
	SendGfcL2PresetAccResult_LP(&sock1->send2011);

	for(i=0;i<AFT_FIN_QUE_NUM;i++){
		if(strcmp(ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID,ufcs->RefreshTrackData.RalPdi.plateID)==0){
			////////////////// �������󾯸棬�øְ��Ѿ����� /////////////////////
			err_flg = ERR_AFT_FINISH_EXIST_FILE;
			AlarmPrint(err_flg,"The ��� has be existed in after mill!",0.0,0.0);
			break;
		}
		else if(strcmp(ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID,"")==0){
			ufcs->AFT_FINISH_ROLL[i] = ufcs->RefreshTrackData;
			break;
		}
		else if((strcmp(ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID,"")!=0)&&(i == AFT_FIN_QUE_NUM-1)){
			////////////////// �������󾯸棬�������� /////////////////////
			err_flg = ERR_AFT_FINISH_FULL_FILE;
			AlarmPrint(err_flg,"AFT_finish queue is full",0.0,0.0);
			for(int i=0;i<AFT_FIN_QUE_NUM;i++)
				memset(&ufcs->AFT_FINISH_ROLL[i],0,sizeof(TRACKDATARAL ));

			ufcs->AFT_FINISH_ROLL[0] = ufcs->RefreshTrackData;
			break;			
		}
	}

	//memset(&ufcs->FINISH_ROLL,0,sizeof(TRACKDATARAL )); ///ǰ���Ѿ����ٲ����FINISH_ROLL���� fixed by Zed 2013.12.10/////////////////

	////////////////// ������ʾ��������ʾ ///////////////////////
	if((ufcs->RefreshTrackData.RalPdi.acc_mode == ACC)||(BACKACC==ufcs->RefreshTrackData.RalPdi.acc_mode)||(UFC==ufcs->RefreshTrackData.RalPdi.acc_mode))
		TRK_LPT_HMI_ACC();
	else if(ufcs->RefreshTrackData.RalPdi.acc_mode == NOACC)	//------added by xiaoxiaohsine ���Ϳ����� 20110322 ------//
		TRK_HMI_NOACC();
	else if(ufcs->RefreshTrackData.RalPdi.acc_mode == EXP)	//------added by xiaoxiaohsine ���ø��̶ֹ���� 20110610 ------//
		TRK_LPT_HMI_EXP();

	DisplayData();	
	 
	SendScheduleToPLC(ufcs->RefreshTrackData.RalOutPut.PreResult.readSideFlowFactor);
	SendSpeedToPLC();

	SendScheduleToPLCTrigger();	
	Sleep(50);
	SendSpeedToPLCTrigger();
	AlarmPrint(err_flg,"��ֹAuto_ModelPreCal",0.0,0.0);	
}


void Auto_TrackPltEnterCZone()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;

	int i;
	int  err_flg = 1;
		

	if (0==strcmp(ufcs->AFT_FINISH_ROLL[0].RalPdi.plateID,""))   //add by xie 12-3 ��ֹ���ź�
		return;


	logger::LogDebug("system","Auto_TrackPltEnterCZone-----------�ְ������ȴ�� ");  //  [10/29/2013 xq]

	TRACKDATARAL plt_data_frt;
	memset(&plt_data_frt,0,sizeof(TRACKDATARAL ));
	AlarmPrint(err_flg,"����Auto_TrackPltEnterCZone",0.0,0.0);	

	//лǫ���� ��¼����time 2012-7-25
	SYSTEMTIME tempTime;
	char beforeCoolTime[16];
	GetLocalTime(&tempTime);
	sprintf(beforeCoolTime,"%04d%02d%02d%02d%02d%02d",tempTime.wYear,tempTime.wMonth,tempTime.wDay,tempTime.wHour,tempTime.wMinute,tempTime.wSecond);   ////����·����LogFILE��  xiaoxiaoshine  20101227
	strcpy_s(ufcs->AFT_FINISH_ROLL[0].RalOutPut.PreResult.FrtTime,beforeCoolTime);


	int oldTimeHour=theApp->timeHour;
	theApp->timeHour=tempTime.wHour;

	if (
		((8!=oldTimeHour)&&(8==theApp->timeHour))||
		((16!=oldTimeHour)&&(16==theApp->timeHour))||
		((0!=oldTimeHour)&&(0==theApp->timeHour))
		)
	{
		theApp->turning++;
		if (theApp->turning>4)
		{
			theApp->turning=1;
		}
	}

	plt_data_frt = ufcs->AFT_FINISH_ROLL[0];	
	
	int myi=0;

	// �洢ʵ������ [11/16/2013 лǫ]
	for (int i=0;i<NUZZLE_NUMBER+GAP_NUZZLE_NUMBER;i++)
	{
		plt_data_frt.RalOutPut.PreResult.realFlowUp[i]=*(&s71->r1ontime.flux_set_C_1+i)+*(&s71->r1ontime.flux_set_L_1+i);
		plt_data_frt.RalOutPut.PreResult.realWaterFactor[i]=*(&s71->r1ontime.water_ratio_C_1+i);
		
		if (*(&s71->r1ontime.flux_set_C_1+i)>80 && myi==0)
		{
			plt_data_frt.RalOutPut.PreResult.flowFactor=*(&s71->r1ontime.water_ratio_C_1+i);   // ��¼ˮ�� ��ѧϰ //  [9/18/2014 qian]
			myi=1;
		}

	}

	plt_data_frt.RalOutPut.PreResult.accSpeed=s71->r1ontime.spare_float_10;
	plt_data_frt.RalOutPut.PreResult.waterPressure2=s71->r1ontime.spare_float_9;
	plt_data_frt.RalOutPut.PreResult.frameHeight1=s71->r1ontime.FrameHeight_1;
	plt_data_frt.RalOutPut.PreResult.frameHeight2=s71->r1ontime.FrameHeight_2;
	plt_data_frt.RalOutPut.PreResult.shieldPos1=s71->r1ontime.ShieldPosition_1;
	plt_data_frt.RalOutPut.PreResult.shieldPos2=s71->r1ontime.ShieldPosition_2;
	plt_data_frt.RalOutPut.PreResult.shieldPos3=s71->r1ontime.ShieldPosition_3;

	plt_data_frt.RalOutPut.PreResult.realTotalFlow=s71->r1ontime.TotalFlowReal;  //  [12/31/2013 xie]


	plt_data_frt.RalPdi.op_mode = s71->r1ontime.OperateMode;  // add [6/25/2014 лǫ]

	plt_data_frt.RalOutPut.PreResult.turning=theApp->turning;  //  [1/14/2015 лǫ]


	// �洢����״̬ [11/30/2013 лǫ]
	CString cstemp;
	CString csresult="'";
	for (int i=0;i<12;i++)
	{
		cstemp.Format("%1d",*(&s71->r1ontime.side_set_1+i));
		csresult+=cstemp;
	}
	csresult+="',";
	sprintf(plt_data_frt.RalOutPut.PreResult.cSideSpray,"%s",csresult);

		
	for(i=0;i<AFT_FIN_QUE_NUM-1;i++)
		ufcs->AFT_FINISH_ROLL[i]=ufcs->AFT_FINISH_ROLL[i+1];

	memset(&ufcs->AFT_FINISH_ROLL[AFT_FIN_QUE_NUM-1],0,sizeof(TRACKDATARAL ));

	for(i=0;i<UND_COOL_QUE_NUM;i++){
		if(strcmp(ufcs->UND_LAMINAR_COOL[i].RalPdi.plateID,plt_data_frt.RalPdi.plateID)==0){
			err_flg = ERR_UND_LAMINAR_EXIST_FILE;
			AlarmPrint(err_flg,"The ��� has be existed in UND_cooling!",0.0,0.0);
			break;
		}
		else if(strcmp(ufcs->UND_LAMINAR_COOL[i].RalPdi.plateID,"")==0){
			ufcs->UND_LAMINAR_COOL[i] = plt_data_frt;
			ufcs->rbt_tar = plt_data_frt.RalPdi.targetFinishCoolTemp;
			break;
		}
		else if((strcmp(ufcs->UND_LAMINAR_COOL[i].RalPdi.plateID,"")!=0)&&(i == UND_COOL_QUE_NUM-1)){
			////////////////// �������󾯸棬�������� /////////////////////
			err_flg = ERR_UND_LAMINAR_FULL_FILE;
			AlarmPrint(err_flg,"UDN_LAMINAR queue is full",0.0,0.0);
			for(int i=0;i<UND_COOL_QUE_NUM;i++){	
				memset(&ufcs->UND_LAMINAR_COOL[i],0,sizeof(TRACKDATARAL ));
			}
			ufcs->AFT_FINISH_ROLL[0] = plt_data_frt;
			break;		
		}
	}
	AlarmPrint(err_flg,"��ֹAuto_TrackPltEnterCZone",0.0,0.0);	
	DisplayData();	
}

void Auto_TrackEndTreat()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 

	int i=0,j=0;
	int err_flg = 1;
		
	if (0==strcmp(ufcs->AFT_LAMINAR_COOL[0].RalPdi.plateID,""))   //add by xie 12-3 ��ֹ���ź�
		return;
	
	//Sleep(2000);

	AlarmPrint(err_flg,"����Auto_TrackEndTreat",0.0,0.0);

	logger::LogDebug("system","Auto_TrackEndTreat--------�ְ忪ʼ����� ");  //  [10/29/2013 xq]

	if(strcmp(ufcs->AFT_LAMINAR_COOL[0].RalPdi.plateID,"")!=0)
	{

		TRACKDATARAL plt_data_rbt;
		memset(&plt_data_rbt,0,sizeof(TRACKDATARAL )); 

		///////////////////// ��ȡ���ݿ��е�AfterCooling�б��е�PDI���ݣ�ͬ�ڴ��е����ݽ��бȽ� ///////////////

		///////////////////////////////////////  ������������ ///////////////////////////////////	
		plt_data_rbt = ufcs->AFT_LAMINAR_COOL[0];

		for(i=0;i<AFT_COOL_QUE_NUM-1;i++)
		{
			ufcs->AFT_LAMINAR_COOL[i]=ufcs->AFT_LAMINAR_COOL[i+1];
		}

		memset(&ufcs->AFT_LAMINAR_COOL[AFT_COOL_QUE_NUM-1],0,sizeof(TRACKDATARAL ));

		//ufcs->trk.learnFlg = SelfLearnChoose;   // ���� [11/16/2013 лǫ]
		ufcs->trk.Cool_status = ufcs->Cool_status;

		//plt_data_rbt.RalPdi.AdaptFlg = ufcs->trk.learnFlg;   // ����Ԥ���㴦��ֵ [11/18/2013 лǫ]

		plt_data_rbt.RalPdi.offLineAdaptFlg = offLineSelfLearn;   //��ȡ�����ѡ��

		plt_data_rbt.RalPdi.CoolStatus = ufcs->trk.Cool_status;

		/////////////////////////////////  ��FILE�ж������߲������� ///////////////////////////////////	
		ReadMeaDataFile(&plt_data_rbt);             //�˴���Ҫ�о� �ڴ�������δ洢

		float tVar=0.0,Tmin=0.0,Tmax=0.0;
		int Tistart=0,Tiend=0;  //add by xie ���õı��� 10-11


		AnalyseArraySp(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempSp5,330,&plt_data_rbt.RalOutPut.PreResult.aveSpCenter,&tVar,&Tmin,&Tmax,&Tistart,&Tiend);
		AnalyseArraySp(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempSp8,330,&plt_data_rbt.RalOutPut.PreResult.aveSpEdge,&tVar,&Tmin,&Tmax,&Tistart,&Tiend);
		AnalyseArraySp(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempSp1,330,&plt_data_rbt.RalOutPut.PreResult.aveSpEdge1,&tVar,&Tmin,&Tmax,&Tistart,&Tiend);  // add [3/31/2015 лǫ]

		
		try{
			///////////// change by wbxang 20120519 //////////////////////////////
			//add by xie ����ʱ�����к���� ���� �����Temperature
			if(plt_data_rbt.RalPdi.isCooling == 1)
				Infoinit::CalculatePostModel(&plt_data_rbt);    //xieqian 2012-7-5
		}
		catch(...){
			throw;
		}

		AnalyseArray(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempP1,600,&plt_data_rbt.RalOutPut.PreResult.aveTmP1,&tVar,&Tmin,&Tmax,&Tistart,&Tiend);
		AnalyseArray(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempP3U,300,&plt_data_rbt.RalOutPut.PreResult.aveTmP3u,&tVar,&Tmin,&Tmax,&Tistart,&Tiend);
		AnalyseArray(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempP3L,300,&plt_data_rbt.RalOutPut.PreResult.aveTmP3l,&tVar,&Tmin,&Tmax,&Tistart,&Tiend);
		//AnalyseArray(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempP4,plt_data_rbt.RalOutPut.PreResult.finishCoolTargetTemperature-150,&plt_data_rbt.RalOutPut.PreResult.aveTmP4,&tVar,&Tmin,&Tmax,&Tistart,&Tiend);
		AnalyseArraySp(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempP4,plt_data_rbt.RalOutPut.PreResult.finishCoolTargetTemperature-150,&plt_data_rbt.RalOutPut.PreResult.aveTmP4,&tVar,&Tmin,&Tmax,&Tistart,&Tiend);
		
		// ���Ӽ���ƽ������ [1/2/2014 xie]
		plt_data_rbt.RalOutPut.PreResult.aveCr=(plt_data_rbt.RalOutPut.PreResult.aveTmP2u-plt_data_rbt.RalOutPut.PreResult.aveTmP4)/(plt_data_rbt.RalOutPut.PreResult.coolTime*0.85);

		AnalyseOrthostate(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempP2U,500,20,plt_data_rbt.RalPdi.startCoolTemp,&plt_data_rbt.RalOutPut.PreResult.aveEftRateT1,&plt_data_rbt.RalOutPut.PreResult.tarEftRateT1);
		AnalyseOrthostate(MAX_MEASURE_VALUE,plt_data_rbt.RalMes.TempP4,500,20,plt_data_rbt.RalPdi.targetFinishCoolTemp,&plt_data_rbt.RalOutPut.PreResult.aveEftRateT4,&plt_data_rbt.RalOutPut.PreResult.tarEftRateT4);

		WaterRatioStudy(plt_data_rbt.RalOutPut.PreResult);  // add [4/16/2014 лǫ]
		
		// add ��HMI���ͼ��㷵���¶� [3/10/2015 лǫ]
		theApp->s71->s1[0].cool_tab_accspd_set=plt_data_rbt.RalOutPut.PreResult.aveTmP4;
		SendScheduleToPLCTrigger();	
		
		//////////////������Ϣ�����ݿ�ṹ�� add by Zed 2013.11///////////////////

		TransMESDateBase(&plt_data_rbt.RalOutPut.PreResult);
		TransMESDateBaseTable2(&plt_data_rbt.RalOutPut.PreResult);
		
		///////////////////////////////////////////
		if(strcmp(plt_data_rbt.RalPdi.plateID,"")==0)
		{
			err_flg = ERR_AFT_RBT_ID_EMPTY_FILE;
			AlarmPrint(err_flg,"Err:PDI ID(RBT) is empty!",0.0,0.0);
			////////////////// ������ʾ��������ʾ ///////////////////////
			DisplayData();	
			return;
		}
		////////////////// ������ʾ��������ʾ ///////////////////////
		DisplayData();	
		AlarmPrint(err_flg,"��ֹAuto_TrackEndTreat",0.0,0.0);
	}
} 

void Auto_TrackPltOutCZone()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 

	int i=0,j=0,k=0;
	int err_flg = 1;

	if (0==strcmp(ufcs->UND_LAMINAR_COOL[0].RalPdi.plateID,""))   //add by xie 12-3 ��ֹ���ź�
		return;
	
	TRACKDATARAL plt_data_tail_fct;
	memset(&plt_data_tail_fct,0,sizeof(TRACKDATARAL ));
	///////////////////// ��ȡ���ݿ��е�Cooling�б��е�PDI���ݣ�ͬ�ڴ��е����ݽ��бȽ� ///////////////
	AlarmPrint(err_flg,"����Auto_TrackPltOutCZone",0.0,0.0);		

	logger::LogDebug("system","Auto_TrackPltOutCZone-----------�ְ����ȴ�� ");  //  [10/29/2013 xq]

	//��¼����time 2012-7-25
	SYSTEMTIME tempTime;
	char afterCoolTime[16];
	GetLocalTime(&tempTime);
	sprintf(afterCoolTime,"%04d%02d%02d%02d%02d%02d",tempTime.wYear,tempTime.wMonth,tempTime.wDay,tempTime.wHour,tempTime.wMinute,tempTime.wSecond);   ////����·����LogFILE��  xiaoxiaoshine  20101227
	strcpy_s(ufcs->UND_LAMINAR_COOL[0].RalOutPut.PreResult.FctTime,afterCoolTime);	

	plt_data_tail_fct = ufcs->UND_LAMINAR_COOL[0];
		
	for(i=0;i<UND_COOL_QUE_NUM-1;i++)
	{
		ufcs->UND_LAMINAR_COOL[i]=ufcs->UND_LAMINAR_COOL[i+1];
	}

	memset(&ufcs->UND_LAMINAR_COOL[UND_COOL_QUE_NUM-1],0,sizeof(TRACKDATARAL ));

	if(strcmp(plt_data_tail_fct.RalPdi.plateID,"")==0){
		err_flg = ERR_TAIL_FCT_ID_EMPTY_FILE;
		AlarmPrint(err_flg,"PDI ID(Auto_TrackPltOutCZone) is empty!",0.0,0.0);
		DisplayData();	
		return;
	}

	//ֻ�ں������һ�����λ ��ֹ ���ٳ��� 
	if(strcmp(ufcs->AFT_LAMINAR_COOL[0].RalPdi.plateID,plt_data_tail_fct.RalPdi.plateID)==0){
		err_flg = ERR_AFT_LAMINAR_EXIST_FILE;
		AlarmPrint(err_flg,"Err:The ��� has be existed in AFT_LAMINAR!",0.0,0.0);
	}
	else if(strcmp(ufcs->AFT_LAMINAR_COOL[0].RalPdi.plateID,"")==0)
		ufcs->AFT_LAMINAR_COOL[0] = plt_data_tail_fct;

	AlarmPrint(err_flg,"��ֹAuto_TrackPltOutCZone",0.0,0.0);
	DisplayData();	
}

void PDI_Prepare() 
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	SocketLine1Define *sock1=theApp->sock1;
	S7Line1Define *s71=theApp->s71;

	int  i=0;
	int  err_flg =1;

	AlarmPrint(err_flg,"����PDI_Prepare",0.0,0.0);	

	memset(&ufcs->RefreshTrackData,0,sizeof(TRACKDATARAL)); 

	////////////////////����PDI����////////////////////
	Infoinit::InsertPDIToPreCalcStruct(&ufcs->RefreshTrackData,&sock1->r1ontime,ufcs->CurPltIcFlag);
	int realIndex=0;

	///////////////////// ���µĸְ���ص�PDI���ݴ������ݿ��е�PDIDATA�б��� ///////////////
	////////////////////���������ݴ浽�����ṹ����������////////////////////		
	ufcs->FINISH_ROLL[0] = ufcs->RefreshTrackData;
	AlarmPrint(err_flg,"��ֹPDI_Prepare",0.0,0.0);
	DisplayData();	
}

void PDI_Prepare(PGfcEXTL2PdiData pInfo) 
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	AlarmPrint(1,"����PDI_Prepare",0.0,0.0);	
	memset(&ufcs->pdiPrepareTrackData,0,sizeof(TRACKDATARAL)); 

	////////////////////����PDI����////////////////////
	Infoinit::InsertPDIToPreCalcStruct(&ufcs->pdiPrepareTrackData, pInfo,ufcs->CurPltIcFlag);
	
	if(strcmp(ufcs->FINISH_ROLL[0].RalPdi.plateID,ufcs->pdiPrepareTrackData.RalPdi.plateID)==0)
	{
		//////////////////��Ҫÿ���θ���PDI���Է���ID��ͬӦ�ø���PDI��Ϣ add by Zed 2013.12.13 /////////////////////
		ufcs->FINISH_ROLL[0] = ufcs->pdiPrepareTrackData;
		logger::LogDebug("system","The plate has be existed in after mill!����ԭPDI���ƶ���");
		//////////////////��Ҫÿ���θ���PDI���Է���ID��ͬӦ�ø���PDI��Ϣ add by Zed 2013.12.13 /////////////////////
	}
	//else if(strcmp(ufcs->FINISH_ROLL[0].RalPdi.plateID,"")==0)
	else
	{
		// ������ֵ�� �������ٶ���
		ufcs->FINISH_ROLL[0] = ufcs->pdiPrepareTrackData;

		logger::LogDebug("system","���������ƶ���  %s",ufcs->FINISH_ROLL[0].RalPdi.plateID);  // add [12/20/2013 xie]
	}
	
	////////////////////���������ݴ浽�����ṹ����������////////////////////		
	//ufcs->FINISH_ROLL = ufcs->RefreshTrackData;

	AlarmPrint(1, "��ֹPDI_Prepare",0.0,0.0);
	DisplayData();	
}

void SendScheduleToPLC(float sideWater) 
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;

	int i,err_flg = 1;
	AlarmPrint(err_flg,"����SendScheduleToHMI",0.0,0.0);

	s71->s1[0].water_mode=ufcs->scr.setpoints.acc_mode;

	// ���鼯��ʱ ��ˮ���Ǹ���������+50 [11/2/2013 лǫ]
	for (i=2;i<NUZZLE_NUMBER;i=i+3)
	{
		if (3==(ufcs->scr.setpoints.up_valve_pat[i]+ufcs->scr.setpoints.up_valve_pat[i+1]+ufcs->scr.setpoints.up_valve_pat[i+2]))
		{
			ufcs->scr.setpoints.qtop[i+2]=ufcs->scr.setpoints.qtop[i+2]+60;
		}
	}
	
	for(i=0;i<NUZZLE_NUMBER+2;i++)/////////Modified by Zed
	{
		
		*(&s71->s3[0].valve_set_1+i)=ufcs->scr.setpoints.up_valve_pat[i];     //�˴�����̶��� ����û�к÷�����value
		//*(&s71->s3[0].flux_set_C_1+i)=ufcs->scr.setpoints.qtop[i]*0.75;
		//*(&s71->s3[0].water_ratio_C_1+i)=ufcs->scr.setpoints.w_ratio[i];
		//*(&s71->s3[0].flux_set_L_1+i)=ufcs->scr.setpoints.qtop[i]*0.25;
		//*(&s71->s3[0].water_ratio_L_1+i)=ufcs->scr.setpoints.w_ratio[i];

		*(&s71->s3[0].flux_set_C_1+i)=ufcs->scr.setpoints.qtop[i];
		*(&s71->s3[0].water_ratio_C_1+i)=ufcs->scr.setpoints.w_ratio[i];
		*(&s71->s3[0].flux_set_L_1+i)=ufcs->scr.setpoints.qtop[i]*(sideWater);
		*(&s71->s3[0].water_ratio_L_1+i)=ufcs->scr.setpoints.w_ratio[i];

		//if(*(&ufcs->AFT_FINISH_ROLL[0].RalPdi.width)<2.701)
		if(ufcs->RefreshTrackData.RalPdi.width<2.801)
		{
			*(&s71->s3[0].flux_set_L_1+i)=0;
		}
		
	}
	//s71->s3[0].valve_set_5 =0;
	//s71->s3[0].valve_set_6 =0;
	//s71->s3[0].valve_set_7 =0;
	//s71->s3[0].valve_set_8 =0;
	//s71->s3[0].valve_set_9 =0;
	//s71->s3[0].valve_set_10 =0;
	//s71->s3[0].valve_set_11 =0;

	//s71->s3[0].flux_set_C_12 =200;
	//s71->s3[0].flux_set_L_12 =150;
	//s71->s3[0].flux_set_C_13 =200;
	//s71->s3[0].flux_set_C_14 =250;
	//s71->s3[0].flux_set_L_13 =150;
	//s71->s3[0].flux_set_L_14 =150;
	
//	/*s71->s3[0].valve_set_13 = 1;
//	s71->s3[0].valve_set_14 = 1;
//	s71->s3[0].flux_set_C_13 =100;
//	s71->s3[0].flux_set_C_14 =100;
//	s71->s3[0].flux_set_L_13 =100;
//	s71->s3[0].flux_set_L_14 =100;
//	s71->s3[0].water_ratio_C_13 = 1.2;
//	s71->s3[0].water_ratio_C_14 = 1.2;
//	s71->s3[0].water_ratio_L_13 = 1.2;
//	s71->s3[0].water_ratio_L_14 = 1.2;
//	s71->s3[0].side_set_1 = 1;
//	s71->s3[0].side_set_2 = 2;
//	s71->s3[0].side_set_3 = 3;
//	s71->s3[0].side_set_4 = 4;
//	s71->s3[0].side_set_5 = 5;
//	s71->s3[0].side_set_6 = 6;
//*/
//
//	FILE *Ztxt;
//	float q;
//	q = ufcs->scr.setpoints.v;
//	Ztxt = fopen("Ztxt.txt","a");
//	fprintf(Ztxt,"%s\n",ufcs->AFT_FINISH_ROLL->RalPdi.plateID);
//	fprintf(Ztxt,"%f\n",ufcs->AFT_FINISH_ROLL->RalPdi.thick);
//	fprintf(Ztxt,"%f\n",q);
//
//	fclose(Ztxt);
	
	s71->s3[0].cool_tab_spd_set=ufcs->scr.setpoints.v;
	//s71->s3[0].cool_tab_spd_set=1.3f;
	s71->s3[0].cool_tab_accspd_set=ufcs->scr.setpoints.a;

	s71->s1[0].valve_set_1=ufcs->scr.setpoints.isWellSelfLearned;

	s71->s1[0].cool_model = ufcs->RefreshTrackData.RalPdi.pass_mode;

	if (3==ufcs->RefreshTrackData.RalPdi.pass_mode)
	{
		s71->s1[0].cool_pass = 3;
	}
	else if(1==ufcs->RefreshTrackData.RalPdi.pass_mode)
	{
		s71->s1[0].cool_pass = 1;
	}

	s71->s3[0].side_set_1=ufcs->scr.setpoints.widShield;  // add [11/29/2013 xie]
	s71->s3[0].side_set_2=ufcs->scr.setpoints.widDetaShield;  // add [11/29/2013 xie]

	AlarmPrint(err_flg,"��ֹSendScheduleToHMI",0.0,0.0);
}


void SendScheduleToPLCDefult() 
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;

	int i;
		
	for(i=0;i<NUZZLE_NUMBER+2;i++)/////////Modified by Zed
	{
		*(&s71->s3[0].valve_set_1+i)=0;    //�˴�����̶��� ����û�к÷�����value
		*(&s71->s3[0].flux_set_C_1+i)=0;
		*(&s71->s3[0].water_ratio_C_1+i)=0;
		*(&s71->s3[0].flux_set_L_1+i)=0;
		*(&s71->s3[0].water_ratio_L_1+i)=0;
	}
		
	s71->s3[0].cool_tab_spd_set=1.5f;
// add [12/25/2014 лǫ]
	if (ufcs->RefreshTrackData.RalPdi.thick>0.1)
	{
		s71->s3[0].cool_tab_spd_set=0.9f;
	}
	else if (ufcs->RefreshTrackData.RalPdi.thick>0.05)
	{
		s71->s3[0].cool_tab_spd_set=1.0f;
	}
	else if (ufcs->RefreshTrackData.RalPdi.thick>0.04)
	{
		s71->s3[0].cool_tab_spd_set=1.1f;
	}
	else if (ufcs->RefreshTrackData.RalPdi.thick>0.03)
	{
		s71->s3[0].cool_tab_spd_set=1.2f;
	}
	else if (ufcs->RefreshTrackData.RalPdi.thick>0.02)
	{
		s71->s3[0].cool_tab_spd_set=1.4f;
	}
	else if (ufcs->RefreshTrackData.RalPdi.thick>0.01)
	{
		s71->s3[0].cool_tab_spd_set=1.5f;
	}
	else if (ufcs->RefreshTrackData.RalPdi.thick>0.005)
	{
		s71->s3[0].cool_tab_spd_set=1.6f;
	}

	s71->s3[0].cool_tab_accspd_set=0.005;
	//s71->s1[0].cool_pass = ufcs->
	logger::LogDebug("system","send defult schedule");

}


void TRK_LPT_HMI_ACC()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	UFCDefine *ufcs=theApp->ufcs; 

	int i;
	bool read_flg = 0;

	/////////////// �Ӳ�����������ǰ�����ְ�,������һ��ְ�Ĺ�̴���HMI ////////////////
	TRACKDATARAL plt_data_ref;//����ְ�(���߼�����ȴ�ְ�)�Ĺ��
	memset(&plt_data_ref,0,sizeof(TRACKDATARAL )); 

	if(strcmp(ufcs->AFT_FINISH_ROLL[0].RalPdi.plateID,"")!=0){
		plt_data_ref = ufcs->AFT_FINISH_ROLL[0];
		read_flg = 1;
	}

	if(	read_flg == 0)
		return;

	////////////////////////////////// ��HMI�������� ///////////////////////////////////
	memset(&ufcs->scr,0,sizeof(SCRDATA));

	////////////////  change by wbxang 20100621 ////////////////////////////////

	ufcs->scr.setpoints.isWellSelfLearned=plt_data_ref.RalOutPut.PreResult.isWellSelfLearned;  //add by xie 10-22

	for(i=0;i<NUZZLE_NUMBER+1;i++)
	{
		if(	ufcs->scr.setpoints.up_valve_pat[i]  == 0)
		{
			ufcs->scr.setpoints.qtop[i] = 100;
		}
	}

	//if (plt_data_ref.RalPdi.thick>=0.03)
	//{
	//	ufcs->scr.setpoints.a = 0.005;
	//}
	//else if (plt_data_ref.RalPdi.thick>=0.018)
	//{
	//	ufcs->scr.setpoints.a = 0.008;
	//}
	//else if (plt_data_ref.RalPdi.thick>=0.013)
	//{
	//	ufcs->scr.setpoints.a = 0.01;
	//}
	//else if (plt_data_ref.RalPdi.thick>=0.008)
	//{
	//	ufcs->scr.setpoints.a = 0.012;
	//}
	//else if (plt_data_ref.RalPdi.thick>=0.005)
	//{
	//	ufcs->scr.setpoints.a = 0.015;
	//}
	ufcs->scr.setpoints.a=plt_data_ref.RalOutPut.PreResult.readAceSpeed;  // add [11/23/2013 лǫ]
	ufcs->scr.setpoints.v = plt_data_ref.RalCoeff.initSpeed;

	ufcs->scr.setpoints.widShield=plt_data_ref.RalOutPut.PreResult.readWidShield;  // add [11/29/2013 лǫ]
	ufcs->scr.setpoints.widDetaShield=plt_data_ref.RalOutPut.PreResult.readDetaWidShield;  // add [11/29/2013 лǫ]


	if ( 
		(plt_data_ref.RalPdi.thick<=0.015)    //add by xie 11-14
		&&(plt_data_ref.RalCoeff.initSpeed<1.6)
		)
		plt_data_ref.RalCoeff.initSpeed=1.6f;

	////////////////////Area modified for NG4700 by Zed//////////////////
	ufcs->scr.setpoints.qtop[0]      = plt_data_ref.RalMes.SetPtFlowTop[0]*4.7*1.6*60.0/1000.0/2.0; //��һ���϶ �ֿ�
	ufcs->scr.setpoints.qbtm[0]      = plt_data_ref.RalMes.SetPtFlowBtm[0]*4.7*1.6*60.0/1000.0/2.0; 
	ufcs->scr.setpoints.qtop[1]      = plt_data_ref.RalMes.SetPtFlowTop[0]*4.7*1.6*60.0/1000.0/2.0; 
	ufcs->scr.setpoints.qbtm[1]      = plt_data_ref.RalMes.SetPtFlowBtm[0]*4.7*1.6*60.0/1000.0/2.0; 
	ufcs->scr.setpoints.qtop[2]      = plt_data_ref.RalMes.SetPtFlowTop[1]*4.7*1.6*60.0/1000.0/2.0; 
	ufcs->scr.setpoints.qbtm[2]      = plt_data_ref.RalMes.SetPtFlowBtm[1]*4.7*1.6*60.0/1000.0/2.0; 
	ufcs->scr.setpoints.qtop[3]      = plt_data_ref.RalMes.SetPtFlowTop[1]*4.7*1.6*60.0/1000.0/2.0; 
	ufcs->scr.setpoints.qbtm[3]      = plt_data_ref.RalMes.SetPtFlowBtm[1]*4.7*1.6*60.0/1000.0/2.0; 


	if ((plt_data_ref.RalOutPut.headerWorkingStatus[0] == 1)||(plt_data_ref.RalOutPut.headerWorkingStatus[0] == 2))
	{
		ufcs->scr.setpoints.up_valve_pat[0] = 1;  //лǫ���� ������ 2012-5-18
		ufcs->scr.setpoints.down_valve_pat[0] = 1;
		ufcs->scr.setpoints.up_valve_pat[1] = 1;  //лǫ���� ������ 2012-5-18
		ufcs->scr.setpoints.down_valve_pat[1] = 1;
	}

	if ((plt_data_ref.RalOutPut.headerWorkingStatus[1] == 1)||(plt_data_ref.RalOutPut.headerWorkingStatus[1] == 2))
	{
		ufcs->scr.setpoints.up_valve_pat[2] = 1;  //лǫ���� ������ 2012-5-18
		ufcs->scr.setpoints.down_valve_pat[2] = 1;
		ufcs->scr.setpoints.up_valve_pat[3] = 1;  //лǫ���� ������ 2012-5-18
		ufcs->scr.setpoints.down_valve_pat[3] = 1;
	}

	////////////////////Area modified for NG4700 by Zed//////////////////
	for(i=4;i<NUZZLE_NUMBER + 2;i++)
	{
		ufcs->scr.setpoints.qtop[i]      = plt_data_ref.RalMes.SetPtFlowTop[i-2]*4.7*1.6*60.0/1000.0; // ���һ����� 2012-6-5
		ufcs->scr.setpoints.qbtm[i]      = plt_data_ref.RalMes.SetPtFlowBtm[i-2]*4.7*1.6*60.0/1000.0; // ���һ����� 2012-6-5
		ufcs->scr.setpoints.up_valve_pat[i]   = plt_data_ref.RalOutPut.headerWorkingStatus[i-2];  //лǫ���� ������ 2012-5-18
		ufcs->scr.setpoints.down_valve_pat[i] = plt_data_ref.RalOutPut.headerWorkingStatus[i-2];

	}
	////////////////////Area modified for NG4700 by Zed//////////////////
	for(i = 0;i<NUZZLE_NUMBER+2;i++){
		ufcs->scr.setpoints.w_ratio[i] = 1.2;	
		
		//if(ufcs->scr.setpoints.qbtm[i] > 30)
		//{
		//	if (plt_data_ref.RalOutPut.PreResult.plateThick<50)
		//	{
		//		ufcs->scr.setpoints.w_ratio[i] =plt_data_ref.RalOutPut.PreResult.readWaterFactor+(ufcs->scr.setpoints.v-plt_data_ref.RalOutPut.PreResult.readSpeed)*0.8;
		//	}
		//	else
		//		ufcs->scr.setpoints.w_ratio[i] =plt_data_ref.RalOutPut.PreResult.readWaterFactor+ufcs->scr.setpoints.v-plt_data_ref.RalOutPut.PreResult.readSpeed;

		//}

		if(ufcs->scr.setpoints.qbtm[i] > 30)
			ufcs->scr.setpoints.w_ratio[i] = plt_data_ref.RalOutPut.PreResult.readWaterFactor;  // add [1/14/2015 qian]
		
		//if(plt_data_ref.RalOutPut.PreResult.readWaterFactorSl > 0.6 && plt_data_ref.RalOutPut.PreResult.readWaterFactorSl<2.5 )   //  [9/18/2014 qian]
		//	ufcs->scr.setpoints.w_ratio[i] = plt_data_ref.RalOutPut.PreResult.readWaterFactorSl;
		//else
		//	ufcs->scr.setpoints.w_ratio[i] = plt_data_ref.RalOutPut.PreResult.readWaterFactor;		

	}

	ufcs->scr.setpoints.head_btm_length = 0;
	ufcs->scr.setpoints.tail_btm_length = 0;   //add by xie 9-5 �������߼��㹦��

	ufcs->scr.setpoints.head_btm_coe = 0.5;
	ufcs->scr.setpoints.tail_btm_coe = 0.0;    //�˺�������ʱռ�� ������ȫ�Զ�ʱ��ѹͷ�������� 9-16 add by xie

	//add by xie 9-16
	//float thickLevel[5]={0.003f,0.010f,0.02f,0.040f,0.060f};    // ��ʱ���ü��� 10-19
	float thickLevel[5]={0.003f,0.003f,0.003f,0.003f,0.003f};        //10-19��΢���� 
	float HeadAccSpeedLevel[5]={0.15f,0.1f,0.05f,0.02f,0.01f};  // modify by xie 9-17 
	int nthick=sizeof(HeadAccSpeedLevel)/sizeof(HeadAccSpeedLevel[0]);
	ufcs->scr.setpoints.tail_btm_coe=GetLinearInterpolationFromAbscissa( thickLevel, HeadAccSpeedLevel, nthick,plt_data_ref.RalPdi.thick);

	for(i=0;i<EDGEMASKING_NUMBER;i++)
	{
		ufcs->scr.setpoints.mask_cal[i]=plt_data_ref.RalOutPut.edgePosition[i];
	}

	ufcs->scr.setpoints.side_jet[3]=1;
	ufcs->scr.setpoints.side_jet[4]=1;
	ufcs->scr.setpoints.side_jet[5]=1;
	ufcs->scr.setpoints.side_jet[6]=0;
	ufcs->scr.setpoints.side_jet[7]=0;

	ufcs->scr.setpoints.acc_mode = plt_data_ref.RalPdi.acc_mode;
	ufcs->scr.setpoints.acc_dir = plt_data_ref.RalPdi.acc_dir;
	ufcs->scr.setpoints.inter_acc_req = plt_data_ref.RalPdi.tcr_flg;

	for(i=0;i<POSITION_NUMBER;i++)
	{
		ufcs->scr.setpoints.plateHeadPosition[i] = plt_data_ref.RalOutPut.speedPosition[i];
		ufcs->scr.setpoints.plateHeadSpeed[i] = plt_data_ref.RalOutPut.speed[i];
	}
	for(i=1;i<POSITION_NUMBER;i++){
		if((ufcs->scr.setpoints.plateHeadPosition[i]<60.0)||(ufcs->scr.setpoints.plateHeadPosition[i]>200.0)){
			ufcs->scr.setpoints.plateHeadPosition[0] = 67.0;
			ufcs->scr.setpoints.plateHeadSpeed[0] = 1.5;   //лǫ �޸� 2 to 1.5

			ufcs->scr.setpoints.plateHeadPosition[i] = ufcs->scr.setpoints.plateHeadPosition[0] + i*2.0; 
			ufcs->scr.setpoints.plateHeadSpeed[i] = 1.5;   //лǫ �޸� 2 to 1.5
		}
	}

	if(ufcs->scr.setpoints.acc_mode == 3){

		ufcs->scr.setpoints.side_jet[0]=0;
		ufcs->scr.setpoints.side_jet[1]=1;
		ufcs->scr.setpoints.side_jet[2]=1;
		ufcs->scr.setpoints.Blow[0] = 1;
		ufcs->scr.setpoints.Blow[1] = 1;
		ufcs->scr.setpoints.activeFlg = 1;
	}else{

		ufcs->scr.setpoints.side_jet[0]=0;
		ufcs->scr.setpoints.side_jet[1]=1;
		ufcs->scr.setpoints.side_jet[2]=1;

		ufcs->scr.setpoints.Blow[0] = 0;
		ufcs->scr.setpoints.Blow[1] = 0;
		ufcs->scr.setpoints.activeFlg = 0;
	}
	ufcs->scr.setpoints.Blow[2] = 1;

	if(ufcs->scr.setpoints.acc_dir == 2){
		//////// add by wbxang 20100809 for �Ƚ�ֱ������ȴ/////////////////////
		ufcs->scr.setpoints.Blow[0] = 1; //the first blow
		ufcs->scr.setpoints.side_jet[2] = 1;     //the ufc last side spray
		ufcs->scr.setpoints.side_jet[3] = 1;     //the acc first side spray
	}

	if((ufcs->trk.oper_mode==AUTO_MODE)&&(ufcs->trk.Cool_status == 1))  
		ufcs->scr.send_flg = true;
	else  
		ufcs->scr.send_flg = false;
}

//-------------------------------- added by xiaoxiaoshine �������趨 20110322 ------------------------------------//
void TRK_HMI_NOACC()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 


	int i;
	bool read_flg = 0;

	/////////////// �Ӳ�����������ǰ�����ְ�,������һ��ְ�Ĺ�̴���HMI ////////////////
	TRACKDATARAL plt_data_ref;//����ְ�(���߼�����ȴ�ְ�)�Ĺ��
	memset(&plt_data_ref,0,sizeof(TRACKDATARAL )); 

	for(i=AFT_FIN_QUE_NUM-1;i>-1;i--){
		if(strcmp(ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID,"")!=0){
			plt_data_ref = ufcs->AFT_FINISH_ROLL[i];
			read_flg = 1;
			break;
		}
	}

	if(	read_flg == 0)
	{
		// ��������������Ϣ
		return;
	}


	memset(&ufcs->scr,0,sizeof(SCRDATA));

	ufcs->scr.setpoints.acc_mode = plt_data_ref.RalPdi.acc_mode;
	ufcs->scr.setpoints.acc_dir = plt_data_ref.RalPdi.acc_dir;
	ufcs->scr.setpoints.inter_acc_req = plt_data_ref.RalPdi.tcr_flg;

	//------ ��̳�ʼ���趨 ------//

	//------ ���伯�ܳ�ʼ���趨 ------//
	for (i=0;i<NUZZLE_NUMBER;i++)
	{
		ufcs->scr.setpoints.up_valve_pat[i] = 0;
	}

	//------ ���伯��������ʼ���趨 ------//
	for (i=0;i<NUZZLE_NUMBER;i++)
	{
		ufcs->scr.setpoints.qtop[i] = 200.0;
	}

	//------ �����ʼ���趨 ------//
	for (i=0;i<6;i++)
	{
		ufcs->scr.setpoints.side_jet[i] = 0;
	}
	/*
	//------ ������ˮ�ȳ�ʼ���趨 ------//
	for (i=0;i<10;i++)
	{
	scr.setpoints.u_ratio[i] = 2.0;
	}
	*/
	//------ ����ˮ�ȳ�ʼ���趨 ------//
	for (i=0;i<NUZZLE_NUMBER+2;i++) ///////////////////////modified by Zed
	{
		ufcs->scr.setpoints.w_ratio[i] = 1.5;
	}


	//------ ͷβ�ڱγ�ʼ���趨 ------//
	ufcs->scr.setpoints.head_btm_length = 0.0;
	ufcs->scr.setpoints.tail_btm_length = 0.0;
	ufcs->scr.setpoints.head_btm_coe = 0.0;
	ufcs->scr.setpoints.tail_btm_coe = 0.0;

	//------ �����ڱγ�ʼ���趨 ------//
	for (i=0;i<3;i++)
	{
		ufcs->scr.setpoints.mask_cal[i] = 0.0;
	}

	//------ �紵ɨ��ʼ���趨 ------//
	for (i=0;i<3;i++)
	{
		ufcs->scr.setpoints.Blow[i] = 0;
	}

	//------ �ٶȳ�ʼ���趨 ------//

	ufcs->scr.setpoints.a = 0.003;

	if (plt_data_ref.RalPdi.thick>=0.05)
	{
		ufcs->scr.setpoints.v = 1.2f;
	}
	else if (plt_data_ref.RalPdi.thick>=0.035)
	{
		ufcs->scr.setpoints.v = 1.3f;
	}
	else if (plt_data_ref.RalPdi.thick>=0.02)
	{
		ufcs->scr.setpoints.v = 1.4f;
	}
	else if (plt_data_ref.RalPdi.thick>=0.016)
	{
		ufcs->scr.setpoints.v = 1.6f;
	}
	else if (plt_data_ref.RalPdi.thick>=0.012)
	{
		ufcs->scr.setpoints.v = 1.7f;
	}
	else if (plt_data_ref.RalPdi.thick>=0.008)
	{
		ufcs->scr.setpoints.v = 1.8f;
	}
	else
	{
		ufcs->scr.setpoints.v = 2.0f;
	}


	for(i=0;i<POSITION_NUMBER;i++)
	{
		ufcs->scr.setpoints.plateHeadPosition[i] = plt_data_ref.RalOutPut.speedPosition[i];
		ufcs->scr.setpoints.plateHeadSpeed[i] = 2.0;
	}
	for(i=1;i<POSITION_NUMBER;i++)
	{
		if((ufcs->scr.setpoints.plateHeadPosition[i]<30.0)||(ufcs->scr.setpoints.plateHeadPosition[i]>200.0))
		{
			ufcs->scr.setpoints.plateHeadPosition[0] = 34.4;
			ufcs->scr.setpoints.plateHeadSpeed[0] = 2.0;

			ufcs->scr.setpoints.plateHeadPosition[i] = ufcs->scr.setpoints.plateHeadPosition[0] + i*3.5;
			ufcs->scr.setpoints.plateHeadSpeed[i] = 2.0;
		}
	}

	///////////////// ����Temperature��������PLC�����˻����� //////////////////////////////// 
	if((ufcs->trk.oper_mode==AUTO_MODE)&&(ufcs->trk.Cool_status == 1))  
		ufcs->scr.send_flg = true;
	else  
		ufcs->scr.send_flg = false;

}

void TRK_LPT_HMI_EXP()
{

	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 

	int i;
	bool read_flg = 0;

	TRACKDATARAL plt_data_ref;
	memset(&plt_data_ref,0,sizeof(TRACKDATARAL )); 

	for(i=AFT_FIN_QUE_NUM-1;i>-1;i--)
	{
		if(strcmp(ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID,"")!=0)
		{
			plt_data_ref = ufcs->AFT_FINISH_ROLL[i];
			read_flg = 1;
			break;
		}
	}

	if(	read_flg == 0)
	{
		return;
	}


	memset(&ufcs->scr,0,sizeof(SCRDATA));

	ufcs->scr.setpoints.acc_mode = plt_data_ref.RalPdi.acc_mode;
	ufcs->scr.setpoints.acc_dir = plt_data_ref.RalPdi.acc_dir;
	ufcs->scr.setpoints.inter_acc_req = plt_data_ref.RalPdi.tcr_flg;

	//------ ���伯�ܳ�ʼ���趨 ------//
	for (i=0;i<NUZZLE_NUMBER+2;i++)
		ufcs->scr.setpoints.up_valve_pat[i] = 0;

	//------ ���伯��������ʼ���趨 ------//
	for (i=0;i<NUZZLE_NUMBER+2;i++)
		ufcs->scr.setpoints.qtop[i] = 200.0;

	//------ �����ʼ���趨 ------//
	for (i=0;i<8;i++)
		ufcs->scr.setpoints.side_jet[i] = 0;

	//------ ������ˮ�ȳ�ʼ���趨 ------//
	for (i=0;i<NUZZLE_NUMBER+2;i++)
		ufcs->scr.setpoints.u_ratio[i] = 2.0;

	//------ ����ˮ�ȳ�ʼ���趨 ------//
	ufcs->scr.setpoints.w_ratio[0] = 2.5;

	//------ ͷβ�ڱγ�ʼ���趨 ------//
	ufcs->scr.setpoints.head_btm_length = 0.0;
	ufcs->scr.setpoints.tail_btm_length = 0.0;
	ufcs->scr.setpoints.head_btm_coe = 0.0;
	ufcs->scr.setpoints.tail_btm_coe = 0.0;

	//------ �����ڱγ�ʼ���趨 ------//
	for (i=0;i<3;i++)
		ufcs->scr.setpoints.mask_cal[i] = 0.0;

	//------ �紵ɨ��ʼ���趨 ------//
	for (i=0;i<3;i++)
		ufcs->scr.setpoints.Blow[i] = 0;

	//------ �ٶȳ�ʼ���趨 ------//

	ufcs->scr.setpoints.a = 0.0;
	ufcs->scr.setpoints.v = 2.0;


	//------ ������� ------//

	///////////////// ����Temperature��������PLC�����˻����� //////////////////////////////// 

	if((ufcs->trk.oper_mode==AUTO_MODE)&&(ufcs->trk.Cool_status == 1))  
		ufcs->scr.send_flg = true;
	else  
		ufcs->scr.send_flg = false;
}

///////////////////////////////////////////////////////
//������������Ϊ���溯�����õ��ĸ�������
///////////////////////////////////////////////////////

void SendSpeedToPLC() 
{
}

void SendScheduleToPLCTrigger()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	int err_flg = 1;
	AlarmPrint(err_flg,"����SendScheduleToPLCTrigger",0.0,0.0);

	setTRB(5,1);
	SetEvent(theApp->L2Sys->thread[4].ThreadEvent[0]);

	logger::LogDebug("system","send schedule to PLC S1");
	
	AlarmPrint(err_flg,"��ֹSendScheduleToPLCTrigger",0.0,0.0);
}

void SendSpeedToPLCTrigger()
{

	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	
	
	AlarmPrint(1,"����SendSpeedToPLCTrigger",0.0,0.0);
	//setTRB(5,2);
	setTRB(5,3);
	SetEvent(theApp->L2Sys->thread[4].ThreadEvent[0]);

    logger::LogDebug("system","send schedule to PLC S3");

	AlarmPrint(1,"��ֹSendSpeedToPLCTrigger",0.0,0.0);
}

void SendPDIToPLTrigger()
{
	AlarmPrint(1,"����SendPDIToPLTrigger",0.0,0.0);
	AlarmPrint(1,"��ֹSendPDIToPLTrigger",0.0,0.0);
}

//////////////////////// add by wbxang 20101205 ////////////////////
float GetAlphaCoeWatTemp(float fWatTempInput)
{
	float fAlphaCoeWatTempSet = 1.f;
	char            line[256];
	FILE *fp;
	if((fp = fopen("Logs\\preprocess\\AlphaCoeWatTemp.txt", "r")) == NULL)
		return fAlphaCoeWatTempSet;

	fgets(line,sizeof(line),fp); /* 1. line -> ignore */
	fgets(line,sizeof(line),fp); /* 2. line -> ignore */
	fgets(line,sizeof(line),fp); /* 3. line -> ignore */

	fgets(line,sizeof(line),fp); /* 4. line -> read thickness */
	float fWatTemp[23];
	memset(fWatTemp, 0, sizeof(fWatTemp));
	sscanf(line,"%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f",	&fWatTemp[0], &fWatTemp[1], &fWatTemp[2], &fWatTemp[3], &fWatTemp[4], &fWatTemp[5], 
		&fWatTemp[6], &fWatTemp[7], &fWatTemp[8], &fWatTemp[9], &fWatTemp[10], &fWatTemp[11], 
		&fWatTemp[12], &fWatTemp[13], &fWatTemp[14], &fWatTemp[15], &fWatTemp[16], &fWatTemp[17], 
		&fWatTemp[18], &fWatTemp[19], &fWatTemp[20], &fWatTemp[21], &fWatTemp[22]);

	float fAlphaCoe[23];
	memset(fAlphaCoe, 0, sizeof(fAlphaCoe));
	fgets(line,sizeof(line),fp); /* 5. line -> read offset value */
	sscanf(line,"%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f",	&fAlphaCoe[0], &fAlphaCoe[1], &fAlphaCoe[2], &fAlphaCoe[3], &fAlphaCoe[4], &fAlphaCoe[5], 
		&fAlphaCoe[6], &fAlphaCoe[7], &fAlphaCoe[8], &fAlphaCoe[9], &fAlphaCoe[10], &fAlphaCoe[11], 
		&fAlphaCoe[12], &fAlphaCoe[13], &fAlphaCoe[14], &fAlphaCoe[15], &fAlphaCoe[16], &fAlphaCoe[17], 
		&fAlphaCoe[18], &fAlphaCoe[19], &fAlphaCoe[20], &fAlphaCoe[21], &fAlphaCoe[22]);


	//read value of switch
	int iSwitch = 0; //1:use offset, 0: no use of offset
	fgets(line,sizeof(line),fp); /* 6. line -> read switch value */
	sscanf(line,"%i",	&iSwitch);
	fclose(fp);

	if(iSwitch == 1) //use offset value for ����ion
		fAlphaCoeWatTempSet = GetLinearInterpolationFromAbscissa(fWatTemp, fAlphaCoe, 23, fWatTempInput);

	//limit offset to +/- 100��
	if(fabs(fAlphaCoeWatTempSet) > 2.f)
	{
		if(fAlphaCoeWatTempSet < 0.f)
			fAlphaCoeWatTempSet = 1.f;
		else
			fAlphaCoeWatTempSet = 1.f;
	}//if

	return fAlphaCoeWatTempSet;
}//GetOffsetMillFinishTmp

void ProcessReset()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 

	int i,err_flg = 1;
	//AlarmPrint(err_flg,"����ProcessReset",0.0,0.0);
	logger::LogDebug("system","����ProcessReset");
	for(i=0;i<FIN_QUE_NUM;i++)
	    memset(&ufcs->FINISH_ROLL[i],0,sizeof(TRACKDATARAL ));
	
	for(i=0;i<AFT_FIN_QUE_NUM;i++)
		memset(&ufcs->AFT_FINISH_ROLL[i],0,sizeof(TRACKDATARAL ));

	for(i=0;i<UND_COOL_QUE_NUM;i++)
		memset(&ufcs->UND_LAMINAR_COOL[i],0,sizeof(TRACKDATARAL ));
	for(i=0;i<AFT_COOL_QUE_NUM;i++)
		memset(&ufcs->AFT_LAMINAR_COOL[i],0,sizeof(TRACKDATARAL ));
	DisplayData();	
	//AlarmPrint(err_flg,"��ֹProcessReset",0.0,0.0);
	logger::LogDebug("system","��ֹProcessReset");
}

void DisplayData() 
{
	CString mystr;
	float T_FRT = 0.0;
}

void ReadMeaDataFile(TRACKDATARAL *trackPreRal)
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	UFCDefine *ufcs=theApp->ufcs; 

	/////////////////////// ����FILE��ȡ·�� //////////////////////
	/////////////////////// ��ȡFILE�е����� //////////////////////
	///////////////////////////////   ������¼�ļ�����  //////////////////////////////
	for (int i=0;i<5;i++)
	{
		if (0==strcmp(trackPreRal->RalPdi.plateID,ufcs->m_coolStruct.TrkMes[i].plateID))
		{
			for (int j=0;j<MAX_MEASURE_VALUE;j++)
			{
				trackPreRal->RalMes.CurIndex=ufcs->m_coolStruct.TrkMes[i].number;
				trackPreRal->RalMes.TimeSinceP1[j]=ufcs->m_coolStruct.TrkMes[i].TimeSinceP1[j];
				trackPreRal->RalMes.Position[j]=ufcs->m_coolStruct.TrkMes[i].Position[j];
				trackPreRal->RalMes.Speed[j]=ufcs->m_coolStruct.TrkMes[i].Speed[j];
				trackPreRal->RalMes.TempP1[j]=ufcs->m_coolStruct.TrkMes[i].TempT0[j]; 
				trackPreRal->RalMes.TempTH1[j]=ufcs->m_coolStruct.TrkMes[i].TempTH1[j];
				trackPreRal->RalMes.TempTH2[j]=ufcs->m_coolStruct.TrkMes[i].TempTH2[j];
				trackPreRal->RalMes.TempTH3[j]=ufcs->m_coolStruct.TrkMes[i].TempTH3[j];
				trackPreRal->RalMes.TempP2U[j]=ufcs->m_coolStruct.TrkMes[i].TempT1[j];
				trackPreRal->RalMes.TempP3U[j]=ufcs->m_coolStruct.TrkMes[i].TempT2[j];
				trackPreRal->RalMes.TempP3L[j]=ufcs->m_coolStruct.TrkMes[i].TempT3[j];
				
				if (trackPreRal->RalPdi.thick>0.05)
				{
					trackPreRal->RalMes.TempP4[j]=ufcs->m_coolStruct.TrkMes[i].TempTH1[j];  // add [3/12/2014 лǫ]
				}
				else
				{
					trackPreRal->RalMes.TempP4[j]=ufcs->m_coolStruct.TrkMes[i].TempT4[j];
				}

				trackPreRal->RalMes.TempSp4[j]=ufcs->m_coolStruct.TrkMes[i].TempSp4[j];  // add [1/2/2014 xie]
				trackPreRal->RalMes.TempSp5[j]=ufcs->m_coolStruct.TrkMes[i].TempSp5[j];
				trackPreRal->RalMes.TempSp7[j]=ufcs->m_coolStruct.TrkMes[i].TempSp7[j];
				trackPreRal->RalMes.TempSp8[j]=ufcs->m_coolStruct.TrkMes[i].TempSp8[j];

				trackPreRal->RalMes.TempSp1[j]=ufcs->m_coolStruct.TrkMes[i].TempSp1[j];  // add [21/2/2014 xie]
				trackPreRal->RalMes.TempSp2[j]=ufcs->m_coolStruct.TrkMes[i].TempSp2[j];
				trackPreRal->RalMes.TempSp3[j]=ufcs->m_coolStruct.TrkMes[i].TempSp3[j];
				trackPreRal->RalMes.TempSp6[j]=ufcs->m_coolStruct.TrkMes[i].TempSp6[j];

			}
			return;
		}
	}

	return;

}


void DataRealRecord()
{

	CPCFrameWorkApp * pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	UFCDefine *ufcs=pApp->ufcs; 
	S7Line1Define *s71=pApp->s71;
	CMainFrame *myMainFrame=(CMainFrame *)pApp->GetMainWnd();

	//	int i,j;
	static int CurIndex_1 = 0;
	static int CurIndex_2 = 0;
	static int CurIndex_3 = 0;
	//    int no;

	int found = 0;
	int FindFlg_1 = 0;
	int FindFlg_2 = 0;
	int FindFlg_3 = 0;
	float fTimeMin = 0.01f;


	int CurIndex1,CurIndex2,CurIndex3;

	static int nCount1=0;
	static int nCount2=0;
	static int nCount3=0;

	static int CurIndexFlg = 0;

	static int realIndex1=0;
	static int realIndex2=0;
	static int realIndex3=0;

	static int oldIndex2=0; 
	static int oldIndex3=0;  
	CurIndex1 = int((s71->r1ontime.CurIndex_1)/2);
	CurIndex2 = int((s71->r1ontime.CurIndex_2)/2);
	CurIndex3 = int((s71->r1ontime.CurIndex_3)/2);

	ufcs->CurIndex1 = CurIndex1;
	ufcs->CurIndex2 = CurIndex2;
	ufcs->CurIndex3 = CurIndex3;

	realIndex1=CurIndex1;
	realIndex2=CurIndex2;
	realIndex3=CurIndex3;

	if (realIndex1>MAX_MEASURE_VALUE-2)
	{
		realIndex1=MAX_MEASURE_VALUE-2;
	}
	if (realIndex1<0)
	{
		realIndex1=0;
	}

	if (realIndex2>MAX_MEASURE_VALUE-2)
	{
		realIndex2=MAX_MEASURE_VALUE-2;
	}

	if (realIndex2<0)
	{
		realIndex2=0;
	}

	if (realIndex3>MAX_MEASURE_VALUE-2) 
	{
		realIndex3=MAX_MEASURE_VALUE-2;
	}

	if (realIndex3<0){
		realIndex3=0;
	}

	// �������������ĸְ�  5����������

	for (int i=0;i<5;i++)
	{
		if((strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,"") != 0) &&
			(strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_1) != 0) &&
			(strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_2) != 0) &&
			(strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_3) != 0) )
		{
				memset(&ufcs->m_coolStruct.TrkMes[i],0,sizeof(MESUREDATA));    
		}
	}


	if((s71->r1ontime.TrkFlg_1 == 1)&&(CurIndex1 > 0)){
		//&&(CurIndex1 < MAX_MEASURE_VALUE))
		for (int i=0;i<5;i++){
			if((strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_1) == 0))
			{
				strcpy(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_1);
				ufcs->m_coolStruct.TrkMes[i].realIndex ++;
				ufcs->m_coolStruct.TrkMes[i].CurIndex[realIndex1]=realIndex1;
				ufcs->m_coolStruct.TrkMes[i].number = realIndex1;
				ufcs->m_coolStruct.TrkMes[i].TimeSinceP1[realIndex1] = s71->r1ontime.TimeSinceP1_1/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaTimP1;
				ufcs->m_coolStruct.TrkMes[i].Speed[realIndex1] = s71->r1ontime.Speed_1;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltSpd;
				ufcs->m_coolStruct.TrkMes[i].Position[realIndex1] = s71->r1ontime.Position_1/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltPos;
				ufcs->m_coolStruct.TrkMes[i].TempT0[realIndex1]  = s71->r1ontime.TempT0_1;//max(Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX1,Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX2); 
				ufcs->m_coolStruct.TrkMes[i].TempTH1[realIndex1]  = s71->r1ontime.TempTH1_1;
				ufcs->m_coolStruct.TrkMes[i].TempTH2[realIndex1]  = s71->r1ontime.TempTH2_1;
				ufcs->m_coolStruct.TrkMes[i].TempTH3[realIndex1]  = s71->r1ontime.TempTH3_1;
				ufcs->m_coolStruct.TrkMes[i].TempTc[realIndex1]  = s71->r1ontime.TempTc_1;
				ufcs->m_coolStruct.TrkMes[i].TempT1[realIndex1]  = s71->r1ontime.TempT1_1;
				ufcs->m_coolStruct.TrkMes[i].TempT2[realIndex1]  = s71->r1ontime.TempT2_1;
				ufcs->m_coolStruct.TrkMes[i].TempT3[realIndex1]  = s71->r1ontime.TempT3_1;
				ufcs->m_coolStruct.TrkMes[i].TempT4[realIndex1]  = s71->r1ontime.TempT4_1;

				ufcs->m_coolStruct.TrkMes[i].TempSp1[realIndex1]  = s71->r1ontime.spare_float_1;
				ufcs->m_coolStruct.TrkMes[i].TempSp2[realIndex1]  = s71->r1ontime.spare_float_2;
				ufcs->m_coolStruct.TrkMes[i].TempSp3[realIndex1]  = s71->r1ontime.spare_float_3;
				ufcs->m_coolStruct.TrkMes[i].TempSp4[realIndex1]  = s71->r1ontime.spare_float_4;
				ufcs->m_coolStruct.TrkMes[i].TempSp5[realIndex1]  = s71->r1ontime.spare_float_5;
				ufcs->m_coolStruct.TrkMes[i].TempSp6[realIndex1]  = s71->r1ontime.spare_float_6;
				ufcs->m_coolStruct.TrkMes[i].TempSp7[realIndex1]  = s71->r1ontime.spare_float_7;
				ufcs->m_coolStruct.TrkMes[i].TempSp8[realIndex1]  = s71->r1ontime.spare_float_8;

				FindFlg_1 = 1;

				nCount1=1;
				realIndex1++;

				break;
			}			
		}
		if(0 == FindFlg_1)
		{
			for (int i=0;i<5;i++)
			{
				if((strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,"") == 0))
				{
					strcpy(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_1);

					ufcs->m_coolStruct.TrkMes[i].realIndex ++;
					ufcs->m_coolStruct.TrkMes[i].CurIndex[realIndex1]=realIndex1;
					ufcs->m_coolStruct.TrkMes[i].number = realIndex1;
					ufcs->m_coolStruct.TrkMes[i].TimeSinceP1[realIndex1] = s71->r1ontime.TimeSinceP1_1/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaTimP1;
					ufcs->m_coolStruct.TrkMes[i].Speed[realIndex1] = s71->r1ontime.Speed_1;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltSpd;
					ufcs->m_coolStruct.TrkMes[i].Position[realIndex1] = s71->r1ontime.Position_1/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltPos;
					ufcs->m_coolStruct.TrkMes[i].TempT0[realIndex1]  = s71->r1ontime.TempT0_1;//max(Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX1,Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX2); 
					ufcs->m_coolStruct.TrkMes[i].TempTH1[realIndex1]  = s71->r1ontime.TempTH1_1;
					ufcs->m_coolStruct.TrkMes[i].TempTH2[realIndex1]  = s71->r1ontime.TempTH2_1;
					ufcs->m_coolStruct.TrkMes[i].TempTH3[realIndex1]  = s71->r1ontime.TempTH3_1;
					ufcs->m_coolStruct.TrkMes[i].TempTc[realIndex1]  = s71->r1ontime.TempTc_1;
					ufcs->m_coolStruct.TrkMes[i].TempT1[realIndex1]  = s71->r1ontime.TempT1_1;
					ufcs->m_coolStruct.TrkMes[i].TempT2[realIndex1]  = s71->r1ontime.TempT2_1;
					ufcs->m_coolStruct.TrkMes[i].TempT3[realIndex1]  = s71->r1ontime.TempT3_1;
					ufcs->m_coolStruct.TrkMes[i].TempT4[realIndex1]  = s71->r1ontime.TempT4_1;

					ufcs->m_coolStruct.TrkMes[i].TempSp1[realIndex1]  = s71->r1ontime.spare_float_1;
					ufcs->m_coolStruct.TrkMes[i].TempSp2[realIndex1]  = s71->r1ontime.spare_float_2;
					ufcs->m_coolStruct.TrkMes[i].TempSp3[realIndex1]  = s71->r1ontime.spare_float_3;
					ufcs->m_coolStruct.TrkMes[i].TempSp4[realIndex1]  = s71->r1ontime.spare_float_4;
					ufcs->m_coolStruct.TrkMes[i].TempSp5[realIndex1]  = s71->r1ontime.spare_float_5;
					ufcs->m_coolStruct.TrkMes[i].TempSp6[realIndex1]  = s71->r1ontime.spare_float_6;
					ufcs->m_coolStruct.TrkMes[i].TempSp7[realIndex1]  = s71->r1ontime.spare_float_7;
					ufcs->m_coolStruct.TrkMes[i].TempSp8[realIndex1]  = s71->r1ontime.spare_float_8;

					FindFlg_1 = 1;
					nCount1=1;
					realIndex1++;

					break;
				}	
			}
		}

	}

	if((s71->r1ontime.TrkFlg_2 == 1)&&(CurIndex2 > 0) )
		//&&(CurIndex2 < MAX_MEASURE_VALUE))
	{
		for (int i=0;i<5;i++)
		{
			if((strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_2) == 0))
			{
				strcpy(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_2);
				ufcs->m_coolStruct.TrkMes[i].realIndex ++;
				ufcs->m_coolStruct.TrkMes[i].CurIndex[realIndex2]=realIndex2;
				ufcs->m_coolStruct.TrkMes[i].number = realIndex2;
				ufcs->m_coolStruct.TrkMes[i].TimeSinceP1[realIndex2] = s71->r1ontime.TimeSinceP1_2/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaTimP1;
				ufcs->m_coolStruct.TrkMes[i].Speed[realIndex2] = s71->r1ontime.Speed_2;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltSpd;
				ufcs->m_coolStruct.TrkMes[i].Position[realIndex2] = s71->r1ontime.Position_2/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltPos;
				ufcs->m_coolStruct.TrkMes[i].TempT0[realIndex1]  = s71->r1ontime.TempT0_2;//max(Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX1,Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX2); 
				ufcs->m_coolStruct.TrkMes[i].TempTH1[realIndex1]  = s71->r1ontime.TempTH1_2;
				ufcs->m_coolStruct.TrkMes[i].TempTH2[realIndex1]  = s71->r1ontime.TempTH2_2;
				ufcs->m_coolStruct.TrkMes[i].TempTH3[realIndex1]  = s71->r1ontime.TempTH3_2;
				ufcs->m_coolStruct.TrkMes[i].TempTc[realIndex1]  = s71->r1ontime.TempTc_2;
				ufcs->m_coolStruct.TrkMes[i].TempT1[realIndex1]  = s71->r1ontime.TempT1_2;
				ufcs->m_coolStruct.TrkMes[i].TempT2[realIndex1]  = s71->r1ontime.TempT2_2;
				ufcs->m_coolStruct.TrkMes[i].TempT3[realIndex1]  = s71->r1ontime.TempT3_2;
				ufcs->m_coolStruct.TrkMes[i].TempT4[realIndex1]  = s71->r1ontime.TempT4_2;

				ufcs->m_coolStruct.TrkMes[i].TempSp1[realIndex1]  = s71->r1ontime.spare_float_1;
				ufcs->m_coolStruct.TrkMes[i].TempSp2[realIndex1]  = s71->r1ontime.spare_float_2;
				ufcs->m_coolStruct.TrkMes[i].TempSp3[realIndex1]  = s71->r1ontime.spare_float_3;
				ufcs->m_coolStruct.TrkMes[i].TempSp4[realIndex1]  = s71->r1ontime.spare_float_4;
				ufcs->m_coolStruct.TrkMes[i].TempSp5[realIndex1]  = s71->r1ontime.spare_float_5;
				ufcs->m_coolStruct.TrkMes[i].TempSp6[realIndex1]  = s71->r1ontime.spare_float_6;
				ufcs->m_coolStruct.TrkMes[i].TempSp7[realIndex1]  = s71->r1ontime.spare_float_7;
				ufcs->m_coolStruct.TrkMes[i].TempSp8[realIndex1]  = s71->r1ontime.spare_float_8;

				FindFlg_2 = 1;

				nCount2=1;

				realIndex2++;

				oldIndex2=CurIndex2;

				break;
			}			
		}
		if(0 == FindFlg_2)
		{
			for (int i=0;i<5;i++)
			{
				if((strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,"") == 0))
				{
					strcpy(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_2);
					ufcs->m_coolStruct.TrkMes[i].realIndex ++;
					ufcs->m_coolStruct.TrkMes[i].CurIndex[realIndex2]=realIndex2;
					ufcs->m_coolStruct.TrkMes[i].number = realIndex2;
					ufcs->m_coolStruct.TrkMes[i].TimeSinceP1[realIndex2] = s71->r1ontime.TimeSinceP1_2/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaTimP1;
					ufcs->m_coolStruct.TrkMes[i].Speed[realIndex2] = s71->r1ontime.Speed_2;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltSpd;
					ufcs->m_coolStruct.TrkMes[i].Position[realIndex2] = s71->r1ontime.Position_2/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltPos;
					ufcs->m_coolStruct.TrkMes[i].TempT0[realIndex1]  = s71->r1ontime.TempT0_2;//max(Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX1,Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX2); 
					ufcs->m_coolStruct.TrkMes[i].TempTH1[realIndex1]  = s71->r1ontime.TempTH1_2;
					ufcs->m_coolStruct.TrkMes[i].TempTH2[realIndex1]  = s71->r1ontime.TempTH2_2;
					ufcs->m_coolStruct.TrkMes[i].TempTH3[realIndex1]  = s71->r1ontime.TempTH3_2;
					ufcs->m_coolStruct.TrkMes[i].TempTc[realIndex1]  = s71->r1ontime.TempTc_2;
					ufcs->m_coolStruct.TrkMes[i].TempT1[realIndex1]  = s71->r1ontime.TempT1_2;
					ufcs->m_coolStruct.TrkMes[i].TempT2[realIndex1]  = s71->r1ontime.TempT2_2;
					ufcs->m_coolStruct.TrkMes[i].TempT3[realIndex1]  = s71->r1ontime.TempT3_2;
					ufcs->m_coolStruct.TrkMes[i].TempT4[realIndex1]  = s71->r1ontime.TempT4_2;


					ufcs->m_coolStruct.TrkMes[i].TempSp1[realIndex1]  = s71->r1ontime.spare_float_1;
					ufcs->m_coolStruct.TrkMes[i].TempSp2[realIndex1]  = s71->r1ontime.spare_float_2;
					ufcs->m_coolStruct.TrkMes[i].TempSp3[realIndex1]  = s71->r1ontime.spare_float_3;
					ufcs->m_coolStruct.TrkMes[i].TempSp4[realIndex1]  = s71->r1ontime.spare_float_4;
					ufcs->m_coolStruct.TrkMes[i].TempSp5[realIndex1]  = s71->r1ontime.spare_float_5;
					ufcs->m_coolStruct.TrkMes[i].TempSp6[realIndex1]  = s71->r1ontime.spare_float_6;
					ufcs->m_coolStruct.TrkMes[i].TempSp7[realIndex1]  = s71->r1ontime.spare_float_7;
					ufcs->m_coolStruct.TrkMes[i].TempSp8[realIndex1]  = s71->r1ontime.spare_float_8;

					FindFlg_2 = 1;

					nCount2=1;

					realIndex2++;

					oldIndex2= CurIndex2;

					break;
				}	
			}
		}

	}

	if((s71->r1ontime.TrkFlg_3 == 1)&&(CurIndex3 > 0) ){
		for (int i=0;i<5;i++)
		{
			if((strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_3) == 0))
			{
				strcpy(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_3);
				ufcs->m_coolStruct.TrkMes[i].realIndex ++;
				ufcs->m_coolStruct.TrkMes[i].CurIndex[realIndex3]=realIndex3;
				ufcs->m_coolStruct.TrkMes[i].number = realIndex3;
				ufcs->m_coolStruct.TrkMes[i].TimeSinceP1[realIndex3] = s71->r1ontime.TimeSinceP1_3/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaTimP1;
				ufcs->m_coolStruct.TrkMes[i].Speed[realIndex3] = s71->r1ontime.Speed_3;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltSpd;
				ufcs->m_coolStruct.TrkMes[i].Position[realIndex3] = s71->r1ontime.Position_3/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltPos;
				ufcs->m_coolStruct.TrkMes[i].TempT0[realIndex1]  = s71->r1ontime.TempT0_3;//max(Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX1,Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX2); 
				ufcs->m_coolStruct.TrkMes[i].TempTH1[realIndex1]  = s71->r1ontime.TempTH1_3;
				ufcs->m_coolStruct.TrkMes[i].TempTH2[realIndex1]  = s71->r1ontime.TempTH2_3;
				ufcs->m_coolStruct.TrkMes[i].TempTH3[realIndex1]  = s71->r1ontime.TempTH3_3;
				ufcs->m_coolStruct.TrkMes[i].TempTc[realIndex1]  = s71->r1ontime.TempTc_3;
				ufcs->m_coolStruct.TrkMes[i].TempT1[realIndex1]  = s71->r1ontime.TempT1_3;
				ufcs->m_coolStruct.TrkMes[i].TempT2[realIndex1]  = s71->r1ontime.TempT2_3;
				ufcs->m_coolStruct.TrkMes[i].TempT3[realIndex1]  = s71->r1ontime.TempT3_3;
				ufcs->m_coolStruct.TrkMes[i].TempT4[realIndex1]  = s71->r1ontime.TempT4_3;

				ufcs->m_coolStruct.TrkMes[i].TempSp1[realIndex1]  = s71->r1ontime.spare_float_1;
				ufcs->m_coolStruct.TrkMes[i].TempSp2[realIndex1]  = s71->r1ontime.spare_float_2;
				ufcs->m_coolStruct.TrkMes[i].TempSp3[realIndex1]  = s71->r1ontime.spare_float_3;
				ufcs->m_coolStruct.TrkMes[i].TempSp4[realIndex1]  = s71->r1ontime.spare_float_4;
				ufcs->m_coolStruct.TrkMes[i].TempSp5[realIndex1]  = s71->r1ontime.spare_float_5;
				ufcs->m_coolStruct.TrkMes[i].TempSp6[realIndex1]  = s71->r1ontime.spare_float_6;
				ufcs->m_coolStruct.TrkMes[i].TempSp7[realIndex1]  = s71->r1ontime.spare_float_7;
				ufcs->m_coolStruct.TrkMes[i].TempSp8[realIndex1]  = s71->r1ontime.spare_float_8;

				FindFlg_3 = 1;

				nCount3=1;

				realIndex3++;

				oldIndex3=CurIndex3;

				break;
			}			
		}
		if(0 == FindFlg_3)
		{
			for (int i=0;i<5;i++)
			{
				if((strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,"") == 0))
				{
					strcpy(ufcs->m_coolStruct.TrkMes[i].plateID,s71->r1ontime.TrkID_3);
					ufcs->m_coolStruct.TrkMes[i].realIndex ++;
					ufcs->m_coolStruct.TrkMes[i].CurIndex[realIndex3]=realIndex3;
					ufcs->m_coolStruct.TrkMes[i].number = realIndex3;
					ufcs->m_coolStruct.TrkMes[i].TimeSinceP1[realIndex3] = s71->r1ontime.TimeSinceP1_3/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaTimP1;
					ufcs->m_coolStruct.TrkMes[i].Speed[realIndex3] = s71->r1ontime.Speed_3;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltSpd;
					ufcs->m_coolStruct.TrkMes[i].Position[realIndex3] = s71->r1ontime.Position_3/1000.0f;//Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltPos;
					ufcs->m_coolStruct.TrkMes[i].TempT0[realIndex1]  = s71->r1ontime.TempT0_3;//max(Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX1,Con_L2PC.Tel_GfcL1MPMeaDatRx.GfcMeaPltTmpFMilX2); 
					ufcs->m_coolStruct.TrkMes[i].TempTH1[realIndex1]  = s71->r1ontime.TempTH1_3;
					ufcs->m_coolStruct.TrkMes[i].TempTH2[realIndex1]  = s71->r1ontime.TempTH2_3;
					ufcs->m_coolStruct.TrkMes[i].TempTH3[realIndex1]  = s71->r1ontime.TempTH3_3;
					ufcs->m_coolStruct.TrkMes[i].TempTc[realIndex1]  = s71->r1ontime.TempTc_3;
					ufcs->m_coolStruct.TrkMes[i].TempT1[realIndex1]  = s71->r1ontime.TempT1_3;
					ufcs->m_coolStruct.TrkMes[i].TempT2[realIndex1]  = s71->r1ontime.TempT2_3;
					ufcs->m_coolStruct.TrkMes[i].TempT3[realIndex1]  = s71->r1ontime.TempT3_3;
					ufcs->m_coolStruct.TrkMes[i].TempT4[realIndex1]  = s71->r1ontime.TempT4_3;

					ufcs->m_coolStruct.TrkMes[i].TempSp1[realIndex1]  = s71->r1ontime.spare_float_1;
					ufcs->m_coolStruct.TrkMes[i].TempSp2[realIndex1]  = s71->r1ontime.spare_float_2;
					ufcs->m_coolStruct.TrkMes[i].TempSp3[realIndex1]  = s71->r1ontime.spare_float_3;
					ufcs->m_coolStruct.TrkMes[i].TempSp4[realIndex1]  = s71->r1ontime.spare_float_4;
					ufcs->m_coolStruct.TrkMes[i].TempSp5[realIndex1]  = s71->r1ontime.spare_float_5;
					ufcs->m_coolStruct.TrkMes[i].TempSp6[realIndex1]  = s71->r1ontime.spare_float_6;
					ufcs->m_coolStruct.TrkMes[i].TempSp7[realIndex1]  = s71->r1ontime.spare_float_7;
					ufcs->m_coolStruct.TrkMes[i].TempSp8[realIndex1]  = s71->r1ontime.spare_float_8;

					FindFlg_3 = 1;

					nCount3=1;

					realIndex3++;

					oldIndex3=CurIndex3;
					break;
				}	
			}
		}
	}

	//��L1����û�иְ�ʱ��ն���������Ϣ
	if (
		(0==s71->r1ontime.Position_1)&& (0==s71->r1ontime.Position_2)&& (0==s71->r1ontime.Position_3)
		)
	{

		if (1==nCount1)
		{
			//add by xie 9-15
			for(int i=0;i<AFT_COOL_QUE_NUM;i++)
			{
				memset(&ufcs->AFT_FINISH_ROLL[i],0,sizeof(TRACKDATARAL ));
			}

			for(int i=0;i<UND_COOL_QUE_NUM;i++)
			{
				memset(&ufcs->UND_LAMINAR_COOL[i],0,sizeof(TRACKDATARAL ));
			}
			for(int i=0;i<AFT_COOL_QUE_NUM;i++)
			{
				memset(&ufcs->AFT_LAMINAR_COOL[i],0,sizeof(TRACKDATARAL ));
			}

			//DisplayData();	
			
			logger::LogDebug("system","UFCDataRecord.cpp---DataRealRecord--------һ�����ٶ���û�и�����Ϣ����ն�������");

			nCount1=0;
			nCount2=0;
			nCount3=0;
		}

	}

}


//����һ����������
void Auto_ModelFrtModiCal()
{
	CPCFrameWorkApp * pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	UFCDefine *ufcs=pApp->ufcs; 
	S7Line1Define *s71=pApp->s71;
	CMainFrame *myMainFrame=(CMainFrame *)pApp->GetMainWnd();

	int i=0;
	int j;
	int err_flg = 1;

//	char FrtTime[32];

	//add by xie 8-17
	float P1mean=0.0f;  
	float P1Var=0.0f;    
	float P1max=0.0f;   
	float P1min=0.0f;   
	int P1start=0;        
	int P1end=0;    

	//	FILE *fp_sp;
	//	FILE *fp_mea;

	//AlarmPrint(err_flg,"����Auto_ModelFrtModiCal",0.0,0.0);	
	//SYSTEMTIME systime;
	//GetLocalTime(&systime);
	//sprintf(FrtTime,"%4d%2d%2d%2d%2d%2d",systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);
	//L=strlen(FrtTime);
	//for(j=0;j<L;j++)
	//{
	//	if(strncmp(FrtTime+j," ",1) == 0)
	//		strncpy(FrtTime+j,"0",1);
	//}

	//TRACKDATARAL plt_data_pre; 
	memset(&ufcs->RefreshTrackData,0,sizeof(TRACKDATARAL )); 

	//memset(&plt_data_pre,0,sizeof(TRACKDATARAL )); 

	for(i=0;i<5;i++)
	{
		//if((strcmp(ufcs->m_coolStruct.TrkMes[i].plateID,ufcs->AFT_FINISH_ROLL[0].RalPdi.plateID) == 0)&&
		//	(strcmp("",ufcs->AFT_FINISH_ROLL[0].RalPdi.plateID) != 0))
		if(strcmp("",ufcs->AFT_FINISH_ROLL[0].RalPdi.plateID) != 0)
		{   

				AlarmPrint(err_flg,"����Auto_ModelFrtModiCal1",0.0,0.0);	

				//plt_data_pre = ufcs->AFT_FINISH_ROLL[0];
				ufcs->RefreshTrackData = ufcs->AFT_FINISH_ROLL[0];

				for(j=0;j<ufcs->m_coolStruct.TrkMes[i].number;j++)
				{
					ufcs->RefreshTrackData.RalMes.CurIndex = ufcs->m_coolStruct.TrkMes[i].number;
					ufcs->RefreshTrackData.RalMes.TimeSinceP1[j] = ufcs->m_coolStruct.TrkMes[i].TimeSinceP1[j];
					ufcs->RefreshTrackData.RalMes.Speed[j] = ufcs->m_coolStruct.TrkMes[i].Speed[j];
					ufcs->RefreshTrackData.RalMes.Position[j] = ufcs->m_coolStruct.TrkMes[i].Position[j];
					ufcs->RefreshTrackData.RalMes.TempP1[j] = float(ufcs->m_coolStruct.TrkMes[i].TempT0[j]);// 2012-5-30 �޸�
				}

				AnalyseArrayFilter(j,ufcs->RefreshTrackData.RalMes.TempP1,650,&P1mean,&P1Var,&P1min,&P1max,&P1start,&P1end);
						
				int question=0;

				if((P1mean>650)&&(P1mean<1200)&&(0==ufcs->RefreshTrackData.RalPdi.isOnlineCalc)){
					ufcs->RefreshTrackData.RalPdi.finishRollTemp=P1mean;

					if (1==ufcs->RefreshTrackData.RalPdi.isCooling) {
						try{
							question=Infoinit::CalculatePreCalcModel(&ufcs->RefreshTrackData);

							if (-1==question)
							{
								logger::LogDebug("system","������Ԥ�������� û�н��м��� Auto_ModelFrtModiCal ");
								SendScheduleToPLCDefult();
								SendSpeedToPLCTrigger();
								return;
							}								
							logger::LogDebug("system","UFCSignalProcess.cpp----------��ʼ���߼���");
						}
						catch(...){
							throw;
						}
					} 
					else if (2==ufcs->RefreshTrackData.RalPdi.isCooling){
						ufcs->RefreshTrackData.RalPdi.acc_mode = EXP; //add by xie 2013-3-18
						ufcs->RefreshTrackData.RalPdi.retval=0;  //add by xie 2013-3-18
					}
					else
						ufcs->RefreshTrackData.RalPdi.acc_mode=NOACC;  //add by xie 9-12

					ufcs->AFT_FINISH_ROLL[0]=ufcs->RefreshTrackData;   //add by xieqian 8-23

					if((ufcs->RefreshTrackData.RalPdi.acc_mode == ACC)||(BACKACC==ufcs->RefreshTrackData.RalPdi.acc_mode)||(UFC==ufcs->RefreshTrackData.RalPdi.acc_mode))
						TRK_LPT_HMI_ACC();

					//------added by xiaoxiaohsine ���Ϳ����� 20110322 ------//
					else if(ufcs->RefreshTrackData.RalPdi.acc_mode == NOACC)
						TRK_HMI_NOACC();

					//------added by xiaoxiaohsine ���ø��̶ֹ���� 20110610 ------//
					else if(ufcs->RefreshTrackData.RalPdi.acc_mode == EXP)
						TRK_LPT_HMI_EXP();

					//	TRK_HMI_REFRESH();

					SendScheduleToPLC(ufcs->RefreshTrackData.RalOutPut.PreResult.readSideFlowFactor);
					SendSpeedToPLC();

					//SendScheduleToPLCTrigger();	  // ���� [3/26/2014 лǫ]
					Sleep(50);
					SendSpeedToPLCTrigger();

					ufcs->AFT_FINISH_ROLL[0].RalPdi.isOnlineCalc=1; //add by xie 8-27

					AlarmPrint(err_flg,"��ֹAuto_ModelFrtModiCal1",0.0,0.0);	
					//	memset(&plt_data_pre,0,sizeof(TRACKDATARAL )); 
					return;								
				}
				else
				{
					if (0==ufcs->RefreshTrackData.RalPdi.isOnlineCalc)
					{
					//		memset(&plt_data_pre,0,sizeof(TRACKDATARAL )); 
						return;
					}
				}
		}
	}

}

void Auto_SendPdiToPLCWithParam(PGfcEXTL2PdiData pInfo) 
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;
	SocketLine1Define *sock1=theApp->sock1;

	logger::LogDebug("system","send message to L1 and PDI prepare plate_ID= %s ,����=%s ,����= %6.1f ���= %4.3f,Ŀ�귵�� %4.1f ",pInfo->GfcPieceId,pInfo->GfcPltMatId,pInfo->GfcPltLen,pInfo->GfcPltTck,pInfo->GfcPltTmpAccStp);  //  [10/29/2013 xq]

	strcpy_s(s71->s1[0].Trk_ID,pInfo->GfcPieceId);
	strcpy_s(s71->s1[0].Steel_type, pInfo->GfcPltMatIdSpare);  // ֣����Ҫ���޸� [4/5/2015 лǫ]

	s71->s1[0].Plate_length=pInfo->GfcPltLen;
	s71->s1[0].Plate_width=pInfo->GfcPltWid;
	s71->s1[0].Plate_thick=pInfo->GfcPltTck;
	s71->s1[0].tar_rbt=pInfo->GfcPltTmpAccStp;
	s71->s1[0].tar_sct=pInfo->GfcPltTmpAccSta;

	s71->s1[0].valve_set_3=pInfo->GfcRollPassNumSum-pInfo->GfcRollPassNumNow+1;  // add [3/10/2014 лǫ]

	//if ('1'==sock1->r1ontime.GfcPltIcFlg[0])
    if (1==ufcs->CurPltIcFlag)
	{
		s71->s1[0].Plate_length=pInfo->GfcInterSlabTcr1Len;
		s71->s1[0].Plate_width=pInfo->GfcInterSlabTcr1Wid;
		s71->s1[0].Plate_thick=pInfo->GfcInterSlabTcr1Tck;
		s71->s1[0].tar_rbt=pInfo->GfcSlbTcr1TmpStp;
		s71->s1[0].tar_sct=pInfo->GfcSlbTcr1TmpRolFin;
	}
	

	s71->s1[0].tar_cr_1=pInfo->GfcPltCrAcc;
	s71->s1[0].tar_cr_2=pInfo->GfcCorTmpRolFin;  // ��Ϊ�����¶� [11/1/2013 лǫ]
	
	s71->s1[0].water_mode=pInfo->GfcPltAccMode;  // ��� [11/30/2013 лǫ]
	//s71->s1[0].cool_model = pInfo->GfcPltAccMode;//add by Zed 2013.12.9

	setTRB(5,1);
	SetEvent(theApp->L2Sys->thread[4].ThreadEvent[0]);

	PDI_Prepare(pInfo);
	NotifyS7();
	 //LogWrite(7,1,"ʵ���¼1",9);

	CMainFrame* pMainFrm = (CMainFrame*)theApp->GetMainWnd();
	if(pMainFrm != NULL)
	{
		CPCFrameWorkView *PView = (CPCFrameWorkView *)pMainFrm->GetActiveView();
		PView->Invalidate(TRUE);
	}
}

void Auto_SendPdiToPLC() 
 {
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	SocketLine1Define *sock1=theApp->sock1;
	S7Line1Define *s71=theApp->s71;
	
	strcpy_s(s71->s1[0].Trk_ID,sock1->r1[1].GfcPieceId);
	strcpy_s(s71->s1[0].Steel_type,sock1->r1[1].GfcPltMatCode);

	s71->s1[0].Plate_length=sock1->r1[1].GfcPltLen;
	s71->s1[0].Plate_width=sock1->r1[1].GfcPltWid;
	s71->s1[0].Plate_thick=sock1->r1[1].GfcPltTck;
	s71->s1[0].tar_rbt=sock1->r1[1].GfcPltTmpAccStp;
    s71->s1[0].tar_sct=sock1->r1[1].GfcPltTmpAccSta;

	PDI_Prepare();

	//pData->m_L2ToPLCID1->tar_sct = (short)pData->m_SOCKET1ToL2ID1->std_tgt_temp_before_cooling;
	//pData->m_L2ToPLCID1->Sideblow[0]=0;
	//pData->m_L2ToPLCID1->Plate_Head_Length=pData->ceq;   //add by xie 2013-3-13
	/*
	if (0==pData->m_Goracle.rbOkJudge)
	{
		pData->m_L2ToPLCID1->tar_rbt =0;
	}
	else
	{		
		pData->m_L2ToPLCID1->tar_rbt =(short) pData->m_Goracle.BackRedTemp;
	}
	*/
	//	pData->m_L2ToPLCID1->acc_mode = plt_data_pre.TrackCoolPdi.acc_mode;
	
    setTRB(5,1);
	SetEvent(theApp->L2Sys->thread[4].ThreadEvent[0]);
	
	return;
}

char* removespace(char* text)
{
	char* i;

	if(text == NULL)
		return text;
	i = text;
	while(*i == ' '&& *i !='\0')
		i++;
	strcpy(text,i);
	i= text;
	i+=strlen(i)-1;
	while(*i == ' '&& i>text)
		i--;
	*(i+1)='\0';
	return text;
}

bool TransMESDateBase(PRERESULT *PreResult)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	UFCDefine *ufcs=pApp->ufcs; 
	
	CString str,str_oracle;  //д�����ݿ����

	switch(PreResult->coolTypeMode)
	{
	case 0:
		sprintf_s(PreResult->CcoolType,"A");
		break;
	case 1:
		sprintf_s(PreResult->CcoolType,"W");
		break;
	case 2:
		sprintf_s(PreResult->CcoolType,"U");
		break;
	case 3:
		sprintf_s(PreResult->CcoolType,"E");
		break;
	case 4:
		sprintf_s(PreResult->CcoolType,"D");
		break;
	case 5:
		sprintf_s(PreResult->CcoolType,"Q");
		break;
	default:
		sprintf_s(PreResult->CcoolType,"N");
		break;
	}

	str_oracle.Empty();

	str.Format("ng4700mes.seqtest.nextval,");
	str_oracle = str_oracle + str ;
	str.Format("'%s',",PreResult->plateID);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->plateLength);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->plateWidth);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->plateThick);
	str_oracle = str_oracle + str ;
	str.Format("%d,",PreResult->coolModeFlg);
	str_oracle = str_oracle + str ;
	str.Format("'%14s',",PreResult->FrtTime);
	str_oracle = str_oracle + str ;
	str.Format("'%14s',",PreResult->FctTime);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->tempWater);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->WatPress);
	str_oracle = str_oracle + str ;
	str.Format("1,");   // coe_tail [11/20/2013 лǫ]
	str_oracle = str_oracle + str ;
	str.Format("1,");   // coe_head [11/20/2013 лǫ]
	str_oracle = str_oracle + str ;
	str.Format("%1d,",PreResult->turning);   // ���ռ�ÿ�λ  [1/14/2015 лǫ]
	str_oracle = str_oracle + str ;
	str.Format("0,");   // coe_head [11/20/2013 лǫ]
	str_oracle = str_oracle + str ;
	str.Format("%d,",(int)PreResult->aveTmP2u);
	str_oracle = str_oracle + str ;
	str.Format("0,");   // ����±��¶�  [11/20/2013 лǫ]
	str_oracle = str_oracle + str ;
	str.Format("%d,",(int)PreResult->aveTmP3u);
	str_oracle = str_oracle + str ;
	str.Format("%d,",(int)PreResult->aveTmP3l);
	str_oracle = str_oracle + str ;
	str.Format("%d,",(int)PreResult->aveTmP4);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->coolRate);
	str_oracle = str_oracle + str ;
	str.Format("%s",PreResult->cSideSpray);   // ���� ״̬ [11/20/2013 лǫ]
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[0]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[1]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[2]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[3]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[0]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[1]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[2]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[3]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[4]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[5]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[6]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[7]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[8]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[9]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[10]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[11]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[12]);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->realFlowUp[13]);
	str_oracle = str_oracle + str ;

	str.Format("%3.2f,",PreResult->realWaterFactor[4]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[5]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[6]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[7]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[8]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[9]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[10]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[11]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[12]);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->realWaterFactor[13]);
	str_oracle = str_oracle + str ;
	
	str.Format("%3.2f,",PreResult->aveSpeed);
	str_oracle = str_oracle + str ;
	str.Format("%5.4f,",PreResult->accSpeed);
	str_oracle = str_oracle + str ;
	str.Format("%1d,",PreResult->operateMode);
	str_oracle = str_oracle + str ;
	str.Format("%1d,",PreResult->coolThroughMethod);
	str_oracle = str_oracle + str ;
	str.Format("'%1s',",PreResult->CcoolType);
	str_oracle = str_oracle + str ;
	str.Format("'%1d',",PreResult->coolOpenMode);
	str_oracle = str_oracle + str ;

	str.Format("'%s',",PreResult->slabGrade);
	str_oracle = str_oracle + str ;

	// add  [4/24/2014 лǫ]
	str.Format("%d,",(int)PreResult->aveTmP1);
	str_oracle = str_oracle + str ;
	str.Format("%d,",(int)PreResult->aveSpEdge);
	str_oracle = str_oracle + str ;
	str.Format("%d,",(int)PreResult->aveSpCenter);
	str_oracle = str_oracle + str ;
	str.Format("%d,",(int)PreResult->aveSpEdge1);  // add [3/31/2015 лǫ]
	str_oracle = str_oracle + str ;
	str.Format("%d,",(int)PreResult->finishCoolTargetTemperature);
	str_oracle = str_oracle + str ;

	str.Format("%5.2f,",PreResult->frameHeight1);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->frameHeight2);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->shieldPos1);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->shieldPos2);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->shieldPos3);
	str_oracle = str_oracle + str ;
	str.Format("%3.1f,",PreResult->aveCr);
	str_oracle = str_oracle + str ;
	str.Format("%6.1f,",PreResult->realTotalFlow);
	str_oracle = str_oracle + str ;
	str.Format("%6.1f,",PreResult->calcTotalFlow);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f",0.33-PreResult->scanTSelflearn);
	str_oracle = str_oracle + str ;
	
	sprintf_s(ufcs->CSpdiAdcos,"INSERT INTO NG4700MES.GP_DQACCRSTIFW (SEQ_NUM,slab_id,length,width,gauge,usage,start_cooling,stop_cooling,temp_water,"
		"pressure,coe_head,coe_tail,length_head,length_tail,temp_entry_surf_up,temp_entry_surf_down,temp_exit_surf_up,temp_exit_surf_down,temp_exit_avg,"
		"cooling_rate,side_status,dq_up_flow_1,dq_up_flow_2,dq_up_flow_3,dq_up_flow_4,dq_ratio_1,dq_ratio_2,dq_ratio_3,dq_ratio_4,acc_up_flow_1,acc_up_flow_2,"
		"acc_up_flow_3,acc_up_flow_4,acc_up_flow_5,acc_up_flow_6,acc_up_flow_7,acc_up_flow_8,acc_up_flow_9,acc_up_flow_10,acc_ratio_1,acc_ratio_2,acc_ratio_3,"
		"acc_ratio_4,acc_ratio_5,acc_ratio_6,acc_ratio_7,acc_ratio_8,acc_ratio_9,acc_ratio_10,speed,acceleration,operating_mode,through_mode,cool_method,cool_mode,SLAB_GRADE,"
		"FINISH_ROLL_TEMP,SCAN1_TEMP,SCAN4_TEMP,SCAN8_TEMP,TARGET_T4,FRAMEHEIGHT1,FRAMEHEIGHT2,SHIELDPOSITION1,SHIELDPOSITION2,SHIELDPOSITION3,AVECR,REAL_TOTAL_FLOW,CALC_TOTAL_FLOW,SIDE_RATIO) VALUES ("+str_oracle+")") ;
	
	setTRB(6,2);//�������ݿ���� add by Zed 2013.11
	SetEvent(pApp->L2Sys->thread[TYPE_DBLINE].ThreadEvent[STATE_RUN]); 

	return 1;

}

bool TransMESDateBaseTable2(PRERESULT *PreResult)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	UFCDefine *ufcs=pApp->ufcs; 

	CString str,str_oracle;  //д�����ݿ����

	
	str_oracle.Empty();

	str.Format("'%s',",PreResult->plateID);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->selfLearnAlphaCoe);
	str_oracle = str_oracle + str ;
	str.Format("'%s',",PreResult->MatId);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->WatPress);
	str_oracle = str_oracle + str ;
	str.Format("%3.2f,",PreResult->waterPressure2);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->flowdenC);
	str_oracle = str_oracle + str ;
	str.Format("%6.2f,",PreResult->flowdenL);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->frameHeight1);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->frameHeight2);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->shieldPos1);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->shieldPos2);
	str_oracle = str_oracle + str ;
	str.Format("%5.2f,",PreResult->shieldPos3);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->finishRollTargetTemperature);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->finishCoolTargetTemperature);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->aveTmP2u);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->aveTmP3u);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->aveTmP4);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->aveTmP1);
	str_oracle = str_oracle + str ;

	// ������� [1/2/2014 xie]
	str.Format("%6.1f,",PreResult->realTotalFlow);
	str_oracle = str_oracle + str ;
	str.Format("%3.1f,",PreResult->aveCr);
	str_oracle = str_oracle + str ;
	str.Format("%5.1f,",PreResult->aveHC);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->aveSpCenter);
	str_oracle = str_oracle + str ;
	str.Format("%4.1f,",PreResult->aveSpEdge);
	str_oracle = str_oracle + str ;
	str.Format("%2.1f,",PreResult->scanTSelflearn);
	str_oracle = str_oracle + str ;

	// ������� [2/21/2014 лǫ]
	str.Format("%4.3f,",PreResult->tarEftRateT1);
	str_oracle = str_oracle + str ;
	str.Format("%4.3f,",PreResult->aveEftRateT1);
	str_oracle = str_oracle + str ;
	str.Format("%4.3f,",PreResult->tarEftRateT4);
	str_oracle = str_oracle + str ;
	str.Format("%4.3f",PreResult->aveEftRateT4);
	str_oracle = str_oracle + str ;
			
	sprintf_s(ufcs->CSpdiAdcos,"INSERT INTO NG4700L2.realpdi (PLATE_ID,HCCOEF,PLATEGRADE,WP1,WP2,FLOWDENSITYC,FLOWDENSITYL,FRAMEHEIGHT1,"
		"FRAMEHEIGHT2,SHIELDPOSITION1,SHIELDPOSITION2,SHIELDPOSITION3,TARFINISHROLLT,TARFINISHCOOLT,REALSTARTCOOLT,"
		"REALFINISHCOOLT,REALBACKREDT,REALFINISHROLLT,REALTOTALFLOW,AVECR,AVEHC,AVESPCENTER,AVESPEDGE,STSELFLEARN,TAREFTRATET1,AVEEFTRATET1,TAREFTRATET4,AVEEFTRATET4) VALUES ("+str_oracle+")") ;
	
	setTRB(6,2);//�������ݿ���� add by Zed 2013.11
	SetEvent(pApp->L2Sys->thread[TYPE_DBLINE].ThreadEvent[STATE_RUN]); 

	return 1;

}

void AccidentDispose()
{
	
  CPCFrameWorkApp * theApp;
  theApp =(CPCFrameWorkApp *)AfxGetApp(); 
 
  SocketLine1Define *sock1= theApp->sock1;
  UFCDefine *ufcs=theApp->ufcs;

  char DelID[20];
  int i,j;
  strcpy(DelID,sock1->r1ontime.GfcPieceId);
  for(i=0;i<FIN_QUE_NUM;i++)
  {
	  if(strcmp(ufcs->FINISH_ROLL[i].RalPdi.plateID,DelID)==0)
	  {
		  //////////////////ɾ����ӦPDI��Ϣ add by Zed /////////////////////
		 memset(&ufcs->FINISH_ROLL[i],0,sizeof(TRACKDATARAL));

		 logger::LogDebug("system","ɾ�����ƶ���No%d plate_id = %s",i,DelID);
		  //////////////////ɾ����ӦPDI��Ϣ add by Zed /////////////////////
		 for(j=i;j<FIN_QUE_NUM-1;j++)
		 {
			 ufcs->FINISH_ROLL[j] = ufcs->FINISH_ROLL[j+1];
		 }

		 memset(&ufcs->FINISH_ROLL[j+1],0,sizeof(TRACKDATARAL));  // add [12/20/2013 xie]
		 
		 break;
	  }
  }

for(i=0;i<AFT_FIN_QUE_NUM;i++)
{
	if(strcmp(ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID,DelID)==0)
	{
		//////////////////ɾ����ӦPDI��Ϣ add by Zed /////////////////////
		memset(&ufcs->AFT_FINISH_ROLL[i],0,sizeof(TRACKDATARAL));

		logger::LogDebug("system","ɾ��������� plate_id = %s",DelID);
		//////////////////ɾ����ӦPDI��Ϣ add by Zed /////////////////////
		for(j=i;j<AFT_FIN_QUE_NUM-1;j++)
		{
			ufcs->AFT_FINISH_ROLL[j] = ufcs->AFT_FINISH_ROLL[j+1];
		}

		 memset(&ufcs->AFT_FINISH_ROLL[j+1],0,sizeof(TRACKDATARAL));  // add [12/20/2013 xie]

		break;
    }
	
}
strcpy(DelID,"");
return;
 
}


void ManualChangeAcce()
{
	// TODO: Add your control notification handler code here
	CString filename,CSacce;
	filename="SchLevel4700n.csv";
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;

	float x=(float)(theApp->s71->r1ontime.spare_int_3)/1000;
	CSacce.Format("%4.3f",x);
	theApp->m_dataProc.SetValue(filename,ufcs->rowNum-1,10,CSacce);
	theApp->m_dataProc.SaveObj();

	CString passLocation="d:\\���չ���ļ�";
	theApp->m_dataProc.InitData(passLocation);
	logger::LogDebug("system","����sch�ļ�,�ֶ��������ٶȳɹ�, %d acce= %s",ufcs->rowNum,CSacce);

	//����������ļ�
	return;
}


void WaterRatioStudy(PRERESULT preResult)
{
	if (preResult.aveTmP3u>730||preResult.aveTmP3u<300||preResult.aveTmP3l>730||preResult.aveTmP3l<300)
	{
		logger::LogDebug("system","��ȴ�����¶ȳ��޲�������ѧϰ");
		return;
	}

	CString waterRatio="";
	float fWaterRatio=0.0;
	if (abs(preResult.aveTmP3u-preResult.aveTmP3l)<20)
	{
		return;
	}
	else{
		fWaterRatio=preResult.readWaterFactorMod+((preResult.aveTmP3l-preResult.aveTmP3u)/25)*0.1;
	}

	if (fWaterRatio<=0.01)
	{
		fWaterRatio=0.01;
	}

	if (fWaterRatio>=0.2)
	{
		fWaterRatio=0.2;
	}

	waterRatio.Format("%3.2f",fWaterRatio);

	//CString filename;
	//filename="SchLevel4700n.csv";
	//CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	//UFCDefine *ufcs=theApp->ufcs; 

	//theApp->m_dataProc.SetValue(filename,ufcs->rowNum-1,7,waterRatio);
	//theApp->m_dataProc.SaveObj();

	//CString passLocation="d:\\���չ���ļ�";
	//theApp->m_dataProc.InitData(passLocation);

	//logger::LogDebug("system","����sch�ļ�,ˮ�������� %3.2f��rowNum=%d",fWaterRatio,ufcs->rowNum-1);
	logger::LogDebug("system","����sch�ļ�,ˮ�������� %3.2f��rowNum=%d",fWaterRatio,0);

	//����������ļ�
	return;
}