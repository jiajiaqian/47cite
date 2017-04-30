/*! 
* Copyright (c) 2013, ������ѧ���Ƽ����������Զ��������ص�ʵ����(RAL)
* All rights reserved
* ��Ϣ��ʼ��
* FILE Infoinit.cpp
* ˵�� ����  
*  
* ��ϸ˵��
*  
* �汾 1.0 ���� Hanbing ���� 8/6/2013
* 
*/
#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <minmax.h>
#include <math.h>
#include <minmax.h>
#include "../PreProcess.h"
#include "PostProcess.h"
#include "public.H"
#include "Infoinit.h"

int proc_id;
Infoinit::Infoinit(void)
{
}


Infoinit::~Infoinit(void)
{ 
}

int Infoinit::CalculatePostModel(TRACKDATARAL *trackPreRal)
{
	int retval = -1;
	int found = 0;
	// Call of postcalc for �ٶ� profile and flow settings
	if(strcmp(trackPreRal->RalPdi.plateID,"")==NULL)
		return 0;

	retval = trackPreRal->RalPdi.retval;

	if(retval == 0){
		proc_id = POSTPROCESS;

		retval = gPostprocess.PostProcessing(
			&trackPreRal->RalPdi,
			&trackPreRal->RalCoeff,
			&trackPreRal->RalMes,
			&trackPreRal->RalOutPut,
			proc_id,
			NULL, 
			NULL);
	}

	return retval;
}


/// @����˵�� ���� PreCalc ģ��
/// @���� trackPreRal
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int Infoinit::CalculatePreCalcModel(TRACKDATARAL *trackPreRal)
{
	int retval = -1;
	int found = 0;
	int i=0;
	if(strcmp(trackPreRal->RalPdi.plateID,"")==NULL)
	{
		logger::LogDebug("system","st_err.txt  �ļ������ڣ�return");
		return 1;
	}
		
	// Set proc ID
	proc_id = PREPROCESS;
	// Calculate model
	retval = gPreprocess.PreProcess(&trackPreRal->RalPdi,
		&trackPreRal->RalCoeff,
		&trackPreRal->RalOutPut,
		&proc_id,
		NULL, 
		NULL);

	//if (-1==retval)   // лǫ ���� �����������
	//	return -1;// �˴�Ӧ�ڼ�¼�ļ������ [8/9/2013 лǫ]
	if (0!=retval)   // лǫ ���� �����������
		return -1;// �˴�Ӧ�ڼ�¼�ļ������ [8/9/2013 лǫ]


	if(retval==0){
		proc_id = ONLINE_FIRST;

		for(i=0;i<NUZZLE_NUMBER;i++){
			trackPreRal->RalMes.SetPtFlowTop[i]	= trackPreRal->RalOutPut.PreResult.upNuzzleNumber[i] ;
			trackPreRal->RalMes.SetPtFlowBtm[i]	= trackPreRal->RalOutPut.PreResult.downNuzzleNumber[i] ;         //лǫ 2012-6-5 
		}
	}
	trackPreRal->RalPdi.retval = retval;
	return retval;
}


/// @����˵�� ���� PreCalc ģ�� ר���ڵ���������ȴʱ��Ĺ���
/// @���� trackPreRal
/// ����value ����˵��
/// ���� xq
/// ���� 2013-9-12
int Infoinit::CalculatePreCalcModelElite(TRACKDATARAL *trackPreRal)
{
	int retval = -1;
	int found = 0;
	int i=0;
	if(strcmp(trackPreRal->RalPdi.plateID,"")==NULL)
	{

		return 1;
	}


	// Set proc ID
	proc_id = PREPROCESS;
	// Calculate model
	retval = gPreprocess.PreProcessElite(&trackPreRal->RalPdi,
		&trackPreRal->RalCoeff,
		&trackPreRal->RalOutPut,
		&proc_id,
		NULL, 
		NULL);

	trackPreRal->RalPdi.retval = retval;
	return retval;
}

//-

/// @����˵�� Ϊ����PreCalc ��ȡ�������� 
/// @���� trackPreRal
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int Infoinit::GetLayoutData(TRACKDATARAL *trackPreRal)
{
	char dat_lay[256];
	FILE *st_err;
	RALCOEFFDATA lay;

	/* Read layout data only once  */
	/* Do not open FILE every time */
	strcpy_s(dat_lay,DAT_ACC);
	if((st_err = fopen(ST_ERR_FILE,"a")) == NULL){
		logger::LogDebug("system","st_err.txt  �ļ������ڣ�return");
		return(-1);
	}

	// Read layout data from FILE 
	lay =  gPreprocess.LayoutSim(st_err, dat_lay);

	if (trackPreRal->RalPdi.thick>0.05)
	{
		lay.PosP4=165;  // add [3/20/2014 лǫ]
	}
	

	trackPreRal->RalCoeff =  lay;

	fclose(st_err);

	/* Set water and air Temperature to layout data */
	/* from measurement telegram                    */
	return 0;
}

///// @����˵�� ����PDI�籨���ݵ�PreCalc�ṹ  ????????
///// @���� trackPreRal
///// @���� sock1
///// ����value ����˵��
///// ���� Hanb
///// ���� 7/25/2013
//void Infoinit::InsertPDIToPreCalcStruct(TRACKDATARAL *trackPreRal,SocketLine1Define *sock1)
//{
//	const	double	dEpsCr = 0.001;
//	int ThrowDir = 0;
//
//	int max = PATH_MAX - 1;
//
//	char str_test[100] = "TEST_";
//	SYSTEMTIME systime;	
//
//	char buff[32];
//	int         max_in = PATH_MAX - 1;
//
//	///////////////////////////////   ������¼�ļ�����  //////////////////////////////
//	GetLocalTime(&systime);//20100819_���ͱ�����־_xiaoxiaoshine
//	sprintf(buff,"%d%d%d%d%d%d",systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);   ////����·����LogFILE��  xiaoxiaoshine  20101227
//
//	trackPreRal->RalPdi.isOnlineCalc=0;   //add by xie 8-27
//
//	trackPreRal->RalPdi.eval_pyro = 11111.;
//	strcpy_s(trackPreRal->RalPdi.plateID,sock1->r1[1].GfcPieceId);
//	strcpy_s(trackPreRal->RalPdi.s_slab_code,sock1->r1[1].GfcPltMatId);   //лǫ �˴����ĸ���Ϊ���� 2012-8-6
//	strcpy_s(trackPreRal->RalPdi.mat_id,sock1->r1[1].GfcPltMatCode);   //лǫ ���� 2012-7-10
//
//	trackPreRal->RalPdi.acc_request = sock1->r1[1].GfcPltAccMode;
//	trackPreRal->RalPdi.op_mode = 2;
//	trackPreRal->RalPdi.acc_mode = 1;
//
//	//calculate preferred ��ȴ�ٶ�for cooling 
//	//if ACC mode is required but no ��ȴ�ٶ�is given
//	trackPreRal->RalPdi.cr = sock1->r1[1].GfcPltCrAcc;    //����
//
//	// PDI-Data : PDI data set from furnance
//	trackPreRal->RalPdi.priority  = 1.; /* 1: Temp, 2: CR 3: �ٶ� 0: �ٶ� */
//
//	trackPreRal->RalPdi.ResActId = 1;
//	// PDI-Data : ��� material name e.g. API-5L X70	
//	// PDI-Data : ��� length.
//	trackPreRal->RalPdi.plt_len_tmp = sock1->r1[1].GfcPltLen/1000.f;
//	trackPreRal->RalPdi.length  = sock1->r1[1].GfcPltLen / 1000.f;
//	// PDI-Data : ��� width
//	trackPreRal->RalPdi.plt_wid_tmp = (sock1->r1[1].GfcPltWid + 100.0f ) / 1000.f;
//	trackPreRal->RalPdi.width   = (sock1->r1[1].GfcPltWid + 100.0f ) / 1000.f;
//	// PDI-Data : ��ĺ��
//	trackPreRal->RalPdi.plt_thk_tmp = sock1->r1[1].GfcPltTck / 1000.f;
//	trackPreRal->RalPdi.thick   = sock1->r1[1].GfcPltTck / 1000.f;
//	//	  trackPreRal->RalPdi.thick   = 0.04;
//	// PDI-Data : finish rolling Temperature
//
//	trackPreRal->RalPdi.t_finish_target = sock1->r1[1].GfcAveTmpRolFin;  //add by xie 12-3
//	trackPreRal->RalPdi.finishRollTemp = sock1->r1[1].GfcAveTmpRolFin;
//	// trackPreRal->RalPdi.t_acc_finish_tmp = pData->m_SOCKET1ToL2ID1->medUpTemp;
//
//	trackPreRal->RalPdi.targetFinishCoolTemp = sock1->r1[1].GfcPltTmpAccStp;
//	//	  trackPreRal->RalPdi.targetFinishCoolTemp = 480;
//
//	if(trackPreRal->RalPdi.cr<1.0)
//	{
//		//trackPreRal->RalPdi.cr = GetCr(trackPreRal->RalPdi.thick * 1000);
//		trackPreRal->RalPdi.cr=10.0f;
//	}     
//
//	// лǫ���� 2012-8-6 ��ʱ����Temperature����
//	//trackPreRal->RalPdi.finishRollTemp += GetOffsetMillFinishTmp(trackPreRal->RalPdi.thick * 1000.f,ThrowDir);
//	trackPreRal->RalPdi.alloy_als = sock1->r1[1].GfcPltMatCompAlSol;
//	trackPreRal->RalPdi.alloy_c = sock1->r1[1].GfcPltMatCompC;
//	trackPreRal->RalPdi.alloy_cr = sock1->r1[1].GfcPltMatCompCr;
//	trackPreRal->RalPdi.alloy_cu = sock1->r1[1].GfcPltMatCompCu;
//	trackPreRal->RalPdi.alloy_mn = sock1->r1[1].GfcPltMatCompMn;
//	trackPreRal->RalPdi.alloy_mo = sock1->r1[1].GfcPltMatCompMo;
//	trackPreRal->RalPdi.alloy_nb = sock1->r1[1].GfcPltMatCompNb;
//	trackPreRal->RalPdi.alloy_ni = sock1->r1[1].GfcPltMatCompNi;
//	trackPreRal->RalPdi.alloy_si = sock1->r1[1].GfcPltMatCompSi;
//	trackPreRal->RalPdi.alloy_ti = sock1->r1[1].GfcPltMatCompTi;
//	trackPreRal->RalPdi.alloy_v = sock1->r1[1].GfcPltMatCompV;
//
//	trackPreRal->RalPdi.isHeadFlated=0;  //add by xie 10-17
//	trackPreRal->RalPdi.t_acc_stop_tmp = 600;
//	trackPreRal->RalPdi.targetFinishCoolTemp = 600;
//
//	trackPreRal->RalPdi.t_finish_target = 800;  //add by xie 12-3
//	trackPreRal->RalPdi.finishRollTemp = 800;
//}

/// @����˵�� ����PDI�籨���ݵ�PreCalc�ṹ  ????????
/// @���� trackPreRal
/// @���� sock1
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013




void Infoinit::InsertPDIToPreCalcStruct(TRACKDATARAL *trackPreRal, PGfcEXTL2PdiData sock1,int PltIcFlgTemp)
{
	trackPreRal->RalPdi.PltIcFlg[0]=sock1->GfcPltIcFlg[0];// add [9/23/2013 лǫ]
	trackPreRal->RalPdi.PltIcFlgTemp=PltIcFlgTemp;

	trackPreRal->RalPdi.targetFinishCoolTemp = sock1->GfcPltTmpAccStp;
	trackPreRal->RalPdi.t_finish_target = 820;  
	trackPreRal->RalPdi.finishRollTemp = 820;
	trackPreRal->RalPdi.length  = sock1->GfcPltLen / 1000.f;
	trackPreRal->RalPdi.width   = (sock1->GfcPltWid + 100.0f ) / 1000.f;
	trackPreRal->RalPdi.thick   = sock1->GfcPltTck / 1000.f;
	strcpy_s(trackPreRal->RalPdi.plateID, sock1->GfcPieceId);
	trackPreRal->RalPdi.startCoolTemp=sock1->GfcPltTmpAccSta;// acc�����¶� [2/20/2014 лǫ]

	trackPreRal->RalPdi.acc_mode = sock1->GfcPltAccMode;

	//calculate preferred ��ȴ�ٶ�for cooling 
	//if ACC mode is required but no ��ȴ�ٶ�is given
	trackPreRal->RalPdi.cr = sock1->GfcPltCrAcc;    //����

	switch(PltIcFlgTemp)
	{
	case 0:
		//trackPreRal->RalPdi.t_finish_target = sock1->GfcCorTmpRolFin;//////////////fixed by Zed 2013.12.10
		//trackPreRal->RalPdi.finishRollTemp = sock1->GfcAveTmpRolFin;
		//trackPreRal->RalPdi.targetFinishCoolTemp = sock1->GfcPltTmpAccStp;
		break;
	case 1:
		trackPreRal->RalPdi.t_finish_target = sock1->GfcSlbTcr1TmpAccSta; 
		trackPreRal->RalPdi.finishRollTemp = sock1->GfcSlbTcr1TmpAccSta;
		trackPreRal->RalPdi.targetFinishCoolTemp = sock1->GfcSlbTcr1TmpAccStp+(sock1->GfcSlbTcr1TmpAccSta-sock1->GfcSlbTcr1TmpAccStp)/2.5;

		trackPreRal->RalPdi.length  = sock1->GfcInterSlabTcr1Len / 1000.f;
		trackPreRal->RalPdi.width   = (sock1->GfcInterSlabTcr1Wid + 100.0f ) / 1000.f;
		trackPreRal->RalPdi.thick   = sock1->GfcInterSlabTcr1Tck / 1000.f;
		strcat_s(trackPreRal->RalPdi.plateID,"_tcr1");

		trackPreRal->RalPdi.acc_mode = ACC;  // add [7/11/2014 лǫ]
		trackPreRal->RalPdi.cr = 3;    // add [7/10/2014 лǫ]
		trackPreRal->RalPdi.ICtype=1;   // add [7/11/2014 лǫ]

		break;
	case 2:
		trackPreRal->RalPdi.t_finish_target = sock1->GfcSlbTcr2TmpAccSta; 
		trackPreRal->RalPdi.finishRollTemp = sock1->GfcSlbTcr2TmpAccSta;
		trackPreRal->RalPdi.targetFinishCoolTemp = sock1->GfcSlbTcr2TmpAccStp+30;

		trackPreRal->RalPdi.length  = sock1->GfcInterSlabTcr2Len / 1000.f;
		trackPreRal->RalPdi.width   = (sock1->GfcInterSlabTcr2Wid + 100.0f ) / 1000.f;
		trackPreRal->RalPdi.thick   = sock1->GfcInterSlabTcr2Tck / 1000.f;

		strcat_s(trackPreRal->RalPdi.plateID,"_tcr2");
		break;
	}

	const	double	dEpsCr = 0.001;
	trackPreRal->RalPdi.isOnlineCalc=0;   //add by xie 8-27

	trackPreRal->RalPdi.eval_pyro = 11111.;

	strcpy_s(trackPreRal->RalPdi.s_slab_code,sock1->GfcPltMatId);   //лǫ �˴����ĸ���Ϊ���� 2012-8-6
	strcpy_s(trackPreRal->RalPdi.mat_id,sock1->GfcPltMatId);   //лǫ ���� 2012-7-10
	trackPreRal->RalPdi.acc_request = sock1->GfcPltAccMode;
	trackPreRal->RalPdi.op_mode = 2;
	
	// PDI-Data : PDI data set from furnance
	trackPreRal->RalPdi.priority  = 1.; /* 1: Temp, 2: CR 3: �ٶ� 0: �ٶ� */

	trackPreRal->RalPdi.ResActId = 1;
	// PDI-Data : ��� material name e.g. API-5L X70	
	// PDI-Data : ��� length.
	
	if(trackPreRal->RalPdi.cr<1.0){
		//trackPreRal->RalPdi.cr = GetCr(trackPreRal->RalPdi.thick * 1000);
		trackPreRal->RalPdi.cr=10.0f;
	}     

	// лǫ���� 2012-8-6 ��ʱ����Temperature����
	//trackPreRal->RalPdi.finishRollTemp += GetOffsetMillFinishTmp(trackPreRal->RalPdi.thick * 1000.f,ThrowDir);
	trackPreRal->RalPdi.alloy_als = sock1->GfcPltMatCompAlSol;
	trackPreRal->RalPdi.alloy_c = sock1->GfcPltMatCompC;
	trackPreRal->RalPdi.alloy_cr = sock1->GfcPltMatCompCr;
	trackPreRal->RalPdi.alloy_cu = sock1->GfcPltMatCompCu;
	trackPreRal->RalPdi.alloy_mn = sock1->GfcPltMatCompMn;
	trackPreRal->RalPdi.alloy_mo = sock1->GfcPltMatCompMo;
	trackPreRal->RalPdi.alloy_nb = sock1->GfcPltMatCompNb;
	trackPreRal->RalPdi.alloy_ni = sock1->GfcPltMatCompNi;
	trackPreRal->RalPdi.alloy_si = sock1->GfcPltMatCompSi;
	trackPreRal->RalPdi.alloy_ti = sock1->GfcPltMatCompTi;
	trackPreRal->RalPdi.alloy_v = sock1->GfcPltMatCompV;

	trackPreRal->RalPdi.isHeadFlated=0; 
	//trackPreRal->RalPdi.t_acc_stop_tmp = 720;
}