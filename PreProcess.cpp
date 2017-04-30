/*! 
* Copyright (c) 2013, ������ѧ���Ƽ����������Զ��������ص�ʵ����(RAL)
* All rights reserved
* PREPROCESS
* FILE PreProcess.cpp
* ˵�� ����  
*  
* ��ϸ˵��
*  
* �汾 1.0 ���� Hanbing ���� 7/25/2013
* 
*/

#include "stdafx.h"
#include "PCFramework.h"
#include "PreProcess.h"
#include "BaseCoolStruct.h"
#include "ufcsource\SelfLearnFunction.h"
#include "ufcsource\TemperatureField.h"


#include <ctime>

CPreProcess::CPreProcess()
{
}
CPreProcess::~CPreProcess()
{

}

//  �ӹ��ձ���ж�ȡ���ٺ�������Ϣ [11/12/2013 лǫ]
void CPreProcess::readFileSch(
	RALINDATA		*ralInPut,
	PRERESULT		*PreResult,
	FILE *erg_f)
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	CString fileName="thicklevel.csv";
	CStringArray  key;
	CStringArray gotValue;
	CString thickInput;
	thickInput.Format("%3.1f",ralInPut->thick*1000);
	key.Add(thickInput);
	theApp->m_dataProc.GetRangeValue(fileName,&key,&gotValue);

	if (0==gotValue.GetSize())
	{
		thickInput="7";
	}
	else
	{
		thickInput=gotValue.GetAt(2);
	}
	gotValue.RemoveAll();

	CString temInPut;
	temInPut.Format("%f",ralInPut->targetFinishCoolTemp);
	fileName="TemLevel.csv";
	key.RemoveAll();
	key.Add(temInPut);
	theApp->m_dataProc.GetRangeValue(fileName,&key,&gotValue);

	if (0==gotValue.GetSize())
	{
		temInPut="3";
	}
	else
	{
		temInPut=gotValue.GetAt(2);
	}
	gotValue.RemoveAll();

	fileName="SchLevel4700n.csv";
	key.RemoveAll();
	key.Add(temInPut);
	key.Add(thickInput);

	fprintf(erg_f,"\t ������ �¶� %s,thick %s ",temInPut,thickInput);

	theApp->m_dataProc.GetValue(fileName,&key,&gotValue,&theApp->ufcs->rowNum);

	if (0==gotValue.GetSize())	{
		PreResult->readWaterFactor=1.5;
		PreResult->readFlowUpLimit=210;
		PreResult->readFlowDnLimit=130;
		PreResult->readFlowUpLimitUFC=400;
		PreResult->readFlowDnLimitUFC=330;
		PreResult->readSpeed=1.2;
		PreResult->readAceSpeed=0.005;
		PreResult->readCoeAlpha=1.0;
		logger::LogDebug("system","��ȡ�ļ����ʧ��");
	}
	else	{
		/////////////////��ӻ���ϵ��������� add by Zed 2013.12.3///////////////
		PreResult->readCoeAlpha=atof(gotValue.GetAt((gotValue.GetSize()-1)));
		PreResult->readAceSpeed=atof(gotValue.GetAt(gotValue.GetSize()-2));
		PreResult->readFlowUpLimitUFC=atof(gotValue.GetAt(gotValue.GetSize()-3));
		PreResult->readFlowDnLimitUFC=atof(gotValue.GetAt(gotValue.GetSize()-4));
		PreResult->readWaterFactorMod=atof(gotValue.GetAt(gotValue.GetSize()-5));
		PreResult->readFlowUpLimit=atof(gotValue.GetAt(gotValue.GetSize()-6));
		PreResult->readFlowDnLimit=atof(gotValue.GetAt(gotValue.GetSize()-7));
		PreResult->readSpeed=atof(gotValue.GetAt(gotValue.GetSize()-8));
		//logger::LogDebug("system","��ȡ�ļ���̳ɹ�");
		/////////////////��ӻ���ϵ��������� add by Zed 2013.12.3///////////////
	}

	PreResult->readWaterFactor=1.33-0.005267*ralInPut->thick*1000;  // add [3/17/2014 лǫ] 3/25 modify by libo

	if (PreResult->readWaterFactor>1.75)
	{
		PreResult->readWaterFactor=1.75;
	}

	if (PreResult->readWaterFactor<1.1)
	{
		PreResult->readWaterFactor=1.1;
	}
		
	if (PreResult->readFlowUpLimit>=500 ||PreResult->readFlowUpLimit<130)
	{
		PreResult->readFlowUpLimit=480;
	}

	if (PreResult->readFlowDnLimit>=440 ||PreResult->readFlowDnLimit<135)
	{
		PreResult->readFlowDnLimit=135;
	}

	if (PreResult->readFlowUpLimitUFC>=700 ||PreResult->readFlowUpLimitUFC<400)
	{
		PreResult->readFlowUpLimitUFC=600;
	}

	if (PreResult->readFlowDnLimitUFC>=650 ||PreResult->readFlowDnLimitUFC<334)
	{
		PreResult->readFlowDnLimitUFC=334;
	}

	if (PreResult->readAceSpeed>=0.04 ||PreResult->readAceSpeed<0.004)
	{
		PreResult->readAceSpeed=0.01;
	}


	if (PreResult->readSpeed>=1.8 ||PreResult->readSpeed<0.5)
	{
		PreResult->readSpeed=1.3;
	}

	if (PreResult->readFlowDnLimit>PreResult->readFlowUpLimit)
	{
		PreResult->readFlowUpLimit=PreResult->readFlowDnLimit+20;
	}

	if (PreResult->readFlowDnLimitUFC>PreResult->readFlowUpLimitUFC)
	{
		PreResult->readFlowUpLimitUFC=PreResult->readFlowDnLimitUFC+20;
	}

	if (PreResult->readCoeAlpha>=1.5||PreResult->readCoeAlpha<0.5)
	{
		PreResult->readCoeAlpha = 1.0;
	}

	fprintf(erg_f,"\n \t ��ȡ�ļ� ����= %4.2f \n\t ��������[%4.1f,%4.1f]  ˮ��=%3.2f",PreResult->readSpeed,PreResult->readFlowDnLimit,PreResult->readFlowUpLimit,PreResult->readWaterFactor);
	fprintf(erg_f,"\n \t UFC��������[%4.1f,%4.1f]  ���ٶ�=%4.3f ",PreResult->readFlowDnLimitUFC,PreResult->readFlowUpLimitUFC,PreResult->readAceSpeed);

	PreResult->readFlowUpLimit=PreResult->readFlowUpLimit*1000/451;
	PreResult->readFlowDnLimit=PreResult->readFlowDnLimit*1000/451;  // ת��Ϊˮ���ܶ� [11/19/2013 лǫ]
	PreResult->readFlowUpLimitUFC=PreResult->readFlowUpLimitUFC*1000/451;
	PreResult->readFlowDnLimitUFC=PreResult->readFlowDnLimitUFC*1000/451;  // ת��Ϊˮ���ܶ� [11/19/2013 лǫ]


	// �����Զ���ȡ�߲��ڱι��� [11/29/2013 xie]
	CString WidInPut;
	WidInPut.Format("%f",ralInPut->width*1000);
	gotValue.RemoveAll();
	fileName="WidShield.csv";
	key.RemoveAll();
	key.Add(WidInPut);
	theApp->m_dataProc.GetRangeValue(fileName,&key,&gotValue);

	if (0==gotValue.GetSize())
	{
		PreResult->readWidShield=0;
		PreResult->readDetaWidShield=0;
		//logger::LogDebug("system","��ȡ����ڱ��ļ����ʧ��");
	}
	else
	{
		PreResult->readDetaWidShield=atoi(gotValue.GetAt(gotValue.GetSize()-1));
		PreResult->readWidShield=atoi(gotValue.GetAt(gotValue.GetSize()-2));
	}
	gotValue.RemoveAll();


	if (PreResult->readDetaWidShield>=100)
	{
		PreResult->readDetaWidShield=100;
	}

	if (PreResult->readWidShield>=500)
	{
		PreResult->readWidShield=500;
	}


	fprintf(erg_f,"\n \t ����ڱ��� %d,�ڱ��ݶ� %d  \n",PreResult->readWidShield,PreResult->readDetaWidShield);


	return;
}

// add [9/22/2014 лǫ]
void CPreProcess::readFileSchAcc(
	RALINDATA		*ralInPut,
	PRERESULT		*PreResult,
	FILE *erg_f)
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	CString fileName="thicklevel.csv";
	CStringArray  key;
	CStringArray gotValue;
	CString thickInput;
	thickInput.Format("%3.1f",ralInPut->thick*1000);
	key.Add(thickInput);
	theApp->m_dataProc.GetRangeValue(fileName,&key,&gotValue);

	if (0==gotValue.GetSize())
	{
		thickInput="7";
	}
	else
	{
		thickInput=gotValue.GetAt(2);
	}
	gotValue.RemoveAll();

	CString LenInPut;
	LenInPut.Format("%f",ralInPut->length);
	fileName="lenLevel.csv";
	key.RemoveAll();
	key.Add(LenInPut);
	theApp->m_dataProc.GetRangeValue(fileName,&key,&gotValue);

	if (0==gotValue.GetSize())
	{
		LenInPut="1";
	}
	else
	{
		LenInPut=gotValue.GetAt(2);
	}
	gotValue.RemoveAll();


	fileName="accLevel.csv";
	key.RemoveAll();

	key.Add(LenInPut);
	key.Add(thickInput);


	fprintf(erg_f,"\t ������ ���� %s,��� %s ",LenInPut,thickInput);

	theApp->m_dataProc.GetValue(fileName,&key,&gotValue,&theApp->ufcs->rowNum);

	if (0==gotValue.GetSize())	{
		PreResult->readAceSpeed=0.01;
		logger::LogDebug("system","��ȡ�ļ����ʧ��");
	}
	else	{
		PreResult->readAceSpeed=atof(gotValue.GetAt((gotValue.GetSize()-1)));
		//logger::LogDebug("system","��ȡ�ļ���̳ɹ�");
	}

	fprintf(erg_f,"\n \t ��ȡ�ļ����ٶ�=%4.3f",PreResult->readAceSpeed);

	return;
}



//  �ӹ��ձ���ж�ȡ���ٺ�������Ϣ //  [12/23/2014 qian]
void CPreProcess::readFileSchZheng(
	RALINDATA		*ralInPut,
	PRERESULT		*PreResult,
	FILE *erg_f)
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	CString fileName="zhengthick.csv";
	CStringArray  key;
	CStringArray gotValue;
	CString thickInput;
	thickInput.Format("%3.1f",ralInPut->thick*1000);
	key.Add(thickInput);
	theApp->m_dataProc.GetRangeValue(fileName,&key,&gotValue);

	if (0==gotValue.GetSize())
	{
		thickInput="7";
	}
	else
	{
		thickInput=gotValue.GetAt(2);
	}
	gotValue.RemoveAll();

	CString widInPut;
	widInPut.Format("%f",ralInPut->width*1000);
	fileName="zhengwidth.csv";
	key.RemoveAll();
	key.Add(widInPut);
	theApp->m_dataProc.GetRangeValue(fileName,&key,&gotValue);

	if (0==gotValue.GetSize())
	{
		widInPut="3";
	}
	else
	{
		widInPut=gotValue.GetAt(2);
	}
	gotValue.RemoveAll();

	fileName="zheng600.csv";
	key.RemoveAll();
	key.Add(thickInput);
	key.Add(widInPut);

	fprintf(erg_f,"\t ������ thick %s , width %s",widInPut,thickInput);

	theApp->m_dataProc.GetValue(fileName,&key,&gotValue,&theApp->ufcs->rowNum);

	if (0==gotValue.GetSize())	{
		PreResult->readSideFlowFactor=0;
		PreResult->readWaterFactor=1.5;
		PreResult->readSpeed=1.2;
		logger::LogDebug("system","��ȡ�ļ����ʧ��");
	}
	else	{
		/////////////////��ӻ���ϵ��������� add by Zed 2013.12.3///////////////
		PreResult->readSideFlowFactor=atof(gotValue.GetAt((gotValue.GetSize()-1)));
		PreResult->readWaterFactor=atof(gotValue.GetAt(gotValue.GetSize()-2));
		PreResult->readSpeed=atof(gotValue.GetAt(gotValue.GetSize()-3));

		//logger::LogDebug("system","��ȡ�ļ���̳ɹ�");
		/////////////////��ӻ���ϵ��������� add by Zed 2013.12.3///////////////
	}


	if (PreResult->readSpeed>=1.8 ||PreResult->readSpeed<0.5)
	{
		PreResult->readSpeed=1.3;
	}

	if (PreResult->readSideFlowFactor>=0.6)
	{
		PreResult->readSideFlowFactor=0.55;
	}

	if (PreResult->readWaterFactor>=2 ||PreResult->readWaterFactor<0.8)
	{
		PreResult->readWaterFactor=1.3;
	}

	
	fprintf(erg_f,"\n \t ��ȡ�ļ�zheng \n ����= %4.2f ˮ��=%3.2f ����ڱ�ϵ��=%3.2f ",
		PreResult->readSpeed,PreResult->readWaterFactor,PreResult->readSideFlowFactor);

	return;
}




/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// 0) main module for PREPROCESS / declaration in calling software */
///    module with function calls for equipment specific purpose  */
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
pwr_tInt32  CPreProcess::PreProcess(
	RALINDATA *ralInPut,
	RALCOEFFDATA *lay,
	RALOUTDATA *pre_out,
	pwr_tInt32                     *proc_id,
	struct T_AccSysPar			   *SysPar,
	struct T_AccPattern			   *AccPattern)
{
	PHYSICAL ver_par1;   /* css : ver_mds */
	NUZZLECONDITION       nuzzleCondtion;
	FILE            *st_err = NULL; /*      end material    */
	FILE            *erg_f = NULL;
	INPUTDATAPDI pdi_mds;
	flow_cr         cr_flow;
	int		i_top  = 0;//  HEAD_1_TOP;	/* offset for FILE in	*/
	int		i_bot  = 0;// HEAD_1_BOT;	/* offset for FILE in	*/
	int		i_check;
	int		ioutpxx	= IOUT;
	int 		iz;
	int		nStroke = 1;	/* No. of strokes */
	clock_t         start,calc_zeit;
	start   = clock();

	if((erg_f = fopen("Logs\\preprocess\\PreCalculateprocess.txt","w")) == NULL)
		return -1; 
	else {
		OutputPDIData(ralInPut,erg_f);
		OutputLayoutData(lay,erg_f);
		fclose(erg_f);
		erg_f = NULL;
	}

	if((st_err  = fopen("Logs\\preerror\\preerror.txt","a")) == NULL)
		return -1;

	ver_par1 = InitPhysicalParam(lay->nHeader + 2, MAT_STEEL,PREPROCESS, st_err);

	InitControlValues(&(pdi_mds.controlAcc));

	// 1) transfer of pdi-��Ϣ
	///////////// in[]��Ϊ�ܹ���ǰ�׶���Ϣ���ݵ���׶εĽṹ�� ///////////////////////////
	pdi_mds.taskID = PREPROCESS;
	iz = -1;

	memset(&pre_out->PreResult, 0, sizeof(pre_out->PreResult));

	FillPDIStruct(&pdi_mds,ralInPut,lay, &pre_out->PreResult, ioutpxx);
	strcpy_s(pre_out->PreResult.slabGrade,ralInPut->s_slab_code);
	strcpy_s(ralInPut->file_nam,pdi_mds.file_nam);

	InitControlValues(&(pdi_mds.controlAcc), &(pre_out->controlAcc));

	i_check = CheckDataValid(&pdi_mds); /* checking data �ж���Ϣ��Ч��*/

	//��UFC��ȴģʽʱ������PDI��UFC�½�С����Сvalue������ȴģʽ��ΪACC
	if(i_check==14){
		ralInPut->acc_mode = ACC;
		i_check=0;
	}

	//UFCģʽʱ UFC�½�����Ҫ�� ACC���½�������Ҫ�� ʱ��ȴģʽ��ΪDQ
	if(i_check==15)
		i_check=0;

	if(i_check > 0){
		logger::LogDebug("system","i check no =%d",i_check);
		fclose(st_err);
		st_err = NULL;
		return(i_check);	/* not plausible input	*/
	}

	// 1.1) output PDI - ��Ϣ on FILE and close 
	if(strcmp(pdi_mds.file_nam,"")==0)
		strcpy_s(pdi_mds.file_nam,"\\Logs\\postprocess\\0000.txt");

	//�˴����FILE�� \Log\post_dat\�ļ��� ��   лǫ 2012-7-20
	errno_t err;
	if((erg_f = fopen(pdi_mds.file_nam,"w")) == NULL)
	{		
		_get_errno( &err );
		fprintf( st_err,"\t errno = %d\n", err );
		OutputErrorLog(st_err, pdi_mds.file_nam,"open FILE erg_f", "pre_org()");
		exit(1);
	}
	
	//��ȡ����ϵ������ ����ver_par1->alpha_coe  лǫ ���� 2012-7-12
	ver_par1.waterTemp=lay->WaterTemp;
	ver_par1.alpha_coe=1.0f;
	ver_par1.waterFactorCoe=0.0f;
	pre_out->PreResult.alphaCoe=ver_par1.alpha_coe;

	pdi_mds.waterTemperature = ver_par1.waterTemp;//[2014/12/8 by Zed]  // add [1/13/2015 qian]

	OutputPreInfo(&pdi_mds,erg_f);

	// 1.2) read strategy data for cooling
	i_check = ReadAccStrategyParam(&pdi_mds, lay, &pre_out->accStrategy);

	if(i_check == FALSE)
	{
		OutputErrorLog(st_err, pdi_mds.file_nam,"���� cooling strategy", "pre_org()");
		fclose(erg_f);
		return -1;
	}

	ModifyAccStrategy(ralInPut, lay, &pre_out->accStrategy);
	ModifyPDIStrategy(erg_f, &pre_out->accStrategy, &pdi_mds, ralInPut, &pre_out->PreResult);

	//////////////////////////////////////////////////////��ȴģʽ�ж�    ///////////////////////////////////////////////////////
	i_check=PreDefineTechPara(ralInPut,&ver_par1);//change by wbxang 20120505
	if (1!=i_check){
		OutputErrorLog(st_err, pdi_mds.plateID, "û����ȷ�涨��ȴ���", "PreDefineTechPara()");
		fclose(erg_f);
		erg_f=NULL;
		return -1;
	}
	
	readFileSch(ralInPut,&pre_out->PreResult,erg_f);  // add [11/19/2013 лǫ]

	readFileSchAcc(ralInPut,&pre_out->PreResult,erg_f);  // add [9/22/2014 лǫ]

	if (ralInPut->targetFinishCoolTemp>=300)
	{
		readFileSchZheng(ralInPut,&pre_out->PreResult,erg_f); // add [12/23/2014 лǫ]
	}
	
	pdi_mds.acc_mode = ralInPut->acc_mode;
	pdi_mds.open_type = ralInPut->open_type;
	pdi_mds.pass_mode = ralInPut->pass_mode;
	pdi_mds.activeSecType = ralInPut->activeSecType;
	pdi_mds.activeFirstJet = ralInPut->activeFirstJet;
	//pdi_mds.manualHC=ralInPut->manualHC;   // add [11/15/2013 лǫ]
	pdi_mds.manualHC = pre_out->PreResult.readCoeAlpha;
	pre_out->PreResult.coolOpenMode=ralInPut->activeSecType-1;  // ���ܿ�����ʽ[11/21/2013 лǫ]
	pre_out->PreResult.coolThroughMethod=ralInPut->pass_mode-1;  // ͨ��ģʽ  [11/21/2013 лǫ]

	// 2) read ������Ϣ
	i_check = InputInfomations( erg_f, st_err, &ver_par1, &pre_out->PreResult);
	cr_flow = GetFlowInfo(st_err, &pdi_mds, &pre_out->accStrategy, &pre_out->PreResult,ver_par1.waterFactorCoe);	/* flow data input */
	SetLowFlowCoolingRate(erg_f, &pdi_mds, cr_flow.iFlwLimit, &pdi_mds.cr);  //�˹�����Ҫ�޸� лǫ 2012-5-10
	pre_out->PreResult.coolRate = pdi_mds.cr;

	if(AccPattern != NULL)
		SetCoolPatternFlowValues(&cr_flow, &pdi_mds, lay, SysPar, AccPattern);


	//  3) calculate Temperature for different flows
	const	int	 nThick = sizeof(lay->ThickUpLimit) / sizeof(lay->ThickUpLimit[0]); //��ȵĲ�𣿣� лǫ

	//lay->initSpeed = AccPreferedSpeed(pdi_mds.thick * 1000.f, nThick, lay->ThickUpLimit,	lay->MaxSpeedCool); // ���ݺ�� ���� �ٶȲ�value
	lay->initSpeed=pre_out->PreResult.readSpeed;
	fprintf(erg_f,"\n\t ��ʼ�ٶ�: %f",lay->initSpeed);

	pdi_mds.initFinishRollingTem = pdi_mds.targetFinishCoolTemp;  // лǫ ���� �洢ԭ�趨Temperature 2012-6-5

	//��ԥ������ˮ��ǰ�½�
	
	switch(pdi_mds.pass_mode)
	{
	case 1:
	    i_check = CalculatePre(st_err, erg_f, SysPar, AccPattern,	&pdi_mds, lay, &pre_out->accStrategy, &pre_out->PreResult, &ver_par1,&cr_flow,&pre_out->adapt,&pre_out->SLP_Post);
	    break;
	case 2:
		break;
    case 3:
		i_check = CalcPreOSC_Out(st_err, erg_f, SysPar, AccPattern,	&pdi_mds, lay, &pre_out->accStrategy, &pre_out->PreResult, &ver_par1,&cr_flow,&pre_out->adapt);
	    break;
	case 4:
		i_check = CalculatePre(st_err, erg_f, SysPar, AccPattern,	&pdi_mds, lay, &pre_out->accStrategy, &pre_out->PreResult, &ver_par1,&cr_flow,&pre_out->adapt,&pre_out->SLP_Post);
		break;
	}

	if (0!=i_check){
		OutputErrorLog(st_err, pdi_mds.plateID, "û����ȷȷ����ȴģʽ", "PreProcess()");
		fclose(erg_f);
		erg_f=NULL;
		return -1; 
	}

	//////////////////////////////// ��n�����������ȴ(���ܿ��伯������ʹ��ģʽ��ˮ���ܶȿ�����ȴ�ٶ�) ///////////////////////////////////////
	/*  3b) modify stop temp for IC Cooling Mode according to IcColTime */

	pre_out->fTimeCalReqWatIC = 0;
	pre_out->fTimeCalReqAirIC = 0;

	//////////////////////////////////////////////////////�������Ʋ����趨 ///////////////////////////////////////////////////////
	//////////////////////////////////////////////////////�߲��ڱ� ///////////////////////////////////////////////////////

	//  4) calculate position edge �ڱ�
	CalculateEdgePosition(&pdi_mds, lay, &cr_flow, &pre_out->PreResult);
		//////////////////////////////////////////////////////ˮ͹��///////////////////////////////////////////////////////
	//////////////////////////////////////////////////////ͷβ�ڱ�///////////////////////////////////////////////////////
	//////////////////////////////////////////////////////���ܼ������趨 ///////////////////////////////////////////////////////
	//w_header       s_he;
	i_check=CoolSchduleDecide(st_err,erg_f,&nuzzleCondtion,lay,&cr_flow,&pdi_mds);

	pre_out->PreResult.calcTotalFlow=nuzzleCondtion.header_no*(1+pre_out->PreResult.calcWaterFactor)*(cr_flow.flow_t*4.7*2.0*60.0f/1000.0f);  //add by xie ��¼����������
	pre_out->PreResult.activeCoolZoneLen=nuzzleCondtion.header_no*lay->CoolBankLen;  // лǫ ���� Ϊ��ѧϰ�ṩ���� 2012-8-15
	pre_out->PreResult.vPred=lay->initSpeed;   // лǫ ���� Ϊ��ѧϰ�ṩ���� 2012-8-15
	pre_out->PreResult.flowdenC=cr_flow.flow_t*0.75;
	pre_out->PreResult.flowdenL=cr_flow.flow_t*0.25;
	pre_out->PreResult.coolTime=cr_flow.time_c;  // add [12/31/2013 xie]
	pre_out->PreResult.aveHC=cr_flow.aveHC;
	
	if (0!=i_check){
		OutputErrorLog(st_err, pdi_mds.plateID, "û����ȷ������ȴ���", "CoolSchduleDecide()");
		fclose(erg_f);
		erg_f=NULL;
		return -1;
	}

	//????????????????????????????????????????

	i_check=CalcTempCurve(st_err,erg_f,&pdi_mds,lay,&cr_flow,&ver_par1,&nuzzleCondtion,&pre_out->PreResult);  // add [11/1/2013 лǫ]

	//////////////////////////////////////////////////////����ˮ���ܶ����� ///////////////////////////////////////////////////////
	if (0!=i_check)
	{
		OutputErrorLog(st_err, pdi_mds.plateID, "û����ȷ�����½�����", "CalcTempCurve()");
		fclose(erg_f);
		erg_f=NULL;
		return -1;     //лǫ ��� 2012-7-22 
	}

	//  5) transfer to header setpoint               
	pre_out->PreResult.coolRateWaterProc	= cr_flow.cr_akt; /* see: crpolat.c	*/
	pre_out->PreResult.writeAdapt = cr_flow.write_adapt;
	pre_out->fTimeAirIC = pre_out->PreResult.fTimeAirIC;
	nStroke = pre_out->PreResult.nStroke;

	fprintf(st_err," \nPlate: %s", pdi_mds.plateID);
	fprintf(st_err," \nnStroke: %d",nStroke);

	if(nStroke > 1){
		if(cr_flow.iadaption == 1){ 
			ver_par1.levelNumber =m_SelfLearnFunction.AdaptionProcess( &pdi_mds, pre_out->PreResult.nStroke, &pre_out->adapt, ver_par1.adaptKey,
				&ver_par1.adaptRatioFound,ver_par1.waterTemp);
			m_SelfLearnFunction.WriteAdaptationInfo(erg_f, &pre_out->adapt);
			pre_out->PreResult.coeCoolRate = ver_par1.adaptRatioFound;  
		} 

		pre_out->fTimeCalReqWatIC = pre_out->PreResult.tcHead;//may be some error by wbxang 20120513
		pre_out->fTimeCalReqAirIC = pre_out->PreResult.fTimeAirIC;	/* recalculated for oscillation */

		//there are some point error by wbxang 20120513 
		if(pdi_mds.acc_mode == IC && pdi_mds.IcColTime > 0) 
			pre_out->fTimeCalReqAirIC = pdi_mds.IcColTime - pre_out->fTimeCalReqWatIC;
		if(pre_out->fTimeCalReqAirIC < 10.0f) 
			pre_out->fTimeCalReqAirIC = 10.0f;
	}

	pre_out->PreResult.headerNo	= nuzzleCondtion.header_no;

	int i = -1;
	while(++i < NUZZLE_NUMBER)	
	{
		pre_out->PreResult.upNuzzleNumber[i] = nuzzleCondtion.flow_header_top[i];
		pre_out->PreResult.downNuzzleNumber[i] = nuzzleCondtion.flow_header_bottom[i];
	}

	fprintf(st_err," \ns_he.header_no: %d",nuzzleCondtion.header_no);
	CalculMaskFlowReduction(st_err, &pdi_mds, &cr_flow, &pre_out->PreResult);

	//  6) output in report FILE                         
	LogPreInfo(erg_f,&pdi_mds,&cr_flow,&pre_out->PreResult,ver_par1.levelNumber,ver_par1.adaptRatioFound);

	//  7) output in report FILE / detailed output (optional)

	//ģ��ģʽ ��ʱû��ʹ��
	if(ralInPut->iSimMode == SIMULATION)
		WirteTelegramPre(ralInPut->plateID, &pre_out->PreResult,erg_f);         

	WritePreSSAB(lay,pre_out,&nuzzleCondtion);    // ���ܵĿ�����Ϣ�� ͨ�� pre_out �ṹ����� ��in �е���Ϣ���� pre_out �� 
	calc_zeit = clock() - start;

	fprintf(erg_f,"\n\tԤ��������time calculation pre_func: %i msec", (int)(calc_zeit*1000)/CLOCKS_PER_SEC);
	fprintf(erg_f,"\n\t -----------------------------------------Ԥ�������------------------------------------------");

	if(erg_f != NULL)
	{
		fclose(erg_f);
		erg_f = NULL;
	}
	if(st_err != NULL){
		fclose(st_err);
		st_err = NULL;
	}
	/* detailed output fine cooling calculation */
	return(i_check);	/* return of value 1 for OSAP-application	*/
}

//  calculate for different water flows
/// @����˵�� ACCˮ�乤�ռ���
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::CalcDifferentWaterFlow(
	FILE          *st_err,		/* �쳣���� */
	FILE          *erg_f,			/* ��Ľ��FILE */
	INPUTDATAPDI *pdiInPut,		/* PDI data           */
	MATERIAL *matal,			/* material data      */
	PHYSICAL *ver_par1,		/* ��ȴ���� */
	COMPUTATIONAL *compx,			/* computational para.*/
	flow_cr       *cr_flow,       /* flow value		  */
	const    float         real_temp,		/* Temperature        */
	const    float         *end_temp_air,  /* air Temperature    */
	const    float         *end_aust_air)  /* austenite decomp.  */     
{
	int iv;
	int it;
	float aveHC=0.0f;

	pdiInPut->working_stage=ACC;

	//CTemperatureField m_TemperatureField;
	for (iv = 0; iv < cr_flow->anz_flow; iv++) 
	{ /* loop over flow rates */
		ver_par1->topFlowRate = cr_flow->flow_rate[iv];
		ver_par1->bottomFlowRate =  cr_flow->flow_rate[iv] *cr_flow->flow_factor[iv] *cr_flow->flow_factor_width[iv];
		ver_par1->actualStopTime = 0.0f;
		ver_par1->actualStartTemp = real_temp;

		compx->computeID = 0;

		it  = -1;

		while(++it < NODES_NUMBER)
		{
			compx->startTempDistribution[it] = end_temp_air[it];
			compx->startAusteniteDistribution[it] = end_aust_air[it]; 
		}

		EstimationComputationalParam(erg_f,compx, pdiInPut, ver_par1);

		m_TemperatureField.SolveBoundary(st_err, ver_par1, matal, pdiInPut);      /* SolveBoundary     */

		if ((pdiInPut->thick>=0.035) &&(compx->calculationStopTemp<=650))
			compx->calculationStopTemp=compx->calculationStopTemp+5;

		m_TemperatureField.CalculationTemperatureDistribution(st_err, erg_f, pdiInPut, compx, ver_par1, matal);
		cr_flow->zeit_stop[iv] = compx->calculationStopTime;     //�����ȴtime


		// ����ƽ������ϵ�� [12/31/2013 xie]
		for (int i=1;i<800;i++)
		{
			aveHC+=matal->alpha.top[i];
		}
		aveHC=aveHC/799;
		cr_flow->aveHeatTC[iv]=aveHC;
		
	}                   /*      loop over flow rates    */
	return TRUE;
}


/// @����˵�� ���ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::CoolRateFlows(	FILE	*erg_f,		/* ���FILE */
	const	INPUTDATAPDI *pdiInPut,	/*PDI���� */
	const	ADAPTATION	*adapt,		/* ����Ӧvalue */
	flow_cr *cr_flow,	/* flow value */
	const	float	qcr_find,// ����ӦvalueCR */
	int tStage)	//����״̬ ������״̬Ϊ2ʱ�������Ϣ
{
	int         iret1 = 0; /* return value for interpolation */
	int         iret2 = 0; /* ����Ӧvalueis in use */
	int			iRet  = 0; /* summarized value */
	float       crCorrect; /* ����ed ��ȴ�ٶ�by ����Ӧvalue*/
	/* value for original configuration (no adaption) */
	float       fFlwTop1;       /* specific top flow */
	float       fCrAct1;        /* ��ȴ�ٶ�*/
	float       fTimeWat1;      /* cooling time in water */
	float       fFlwFactor1;    /* multiplier btm/top flow */
	/* value for modified configuration (adaption) */
	float       fFlwTop2;       /* specific top flow */
	float       fCrAct2;        /* ��ȴ�ٶ�*/
	float       fTimeWat2;      /* cooling time in water */
	float       fFlwFactor2;    /* multiplier btm/top flow */

	float aveHC1;
	float aveHC2;
	float       eps = 1.0e-6f;
	float		  qcr_act = qcr_find;

	if (qcr_act <= eps) 
		qcr_act = 1.0f;  /* Not suitable adapt_value */
	crCorrect = pdiInPut->cr / qcr_act;

	iret1 = CalculateInterpolationFlow(cr_flow->anz_flow,
		pdiInPut->cr,
		cr_flow->flow_rate,
		cr_flow->cr_rate,
		cr_flow->zeit_stop,
		cr_flow->flow_factor,
		cr_flow->flow_factor_width,
		cr_flow->aveHeatTC,
		&fFlwTop1,
		&fCrAct1,
		&fTimeWat1,
		&fFlwFactor1,
		&aveHC1);

	iret2 = CalculateInterpolationFlow(cr_flow->anz_flow,
		crCorrect,
		cr_flow->flow_rate,
		cr_flow->cr_rate,
		cr_flow->zeit_stop,
		cr_flow->flow_factor,
		cr_flow->flow_factor_width,
		cr_flow->aveHeatTC,
		&fFlwTop2,
		&fCrAct2,
		&fTimeWat2,
		&fFlwFactor2,
		&aveHC2);

	if (1==tStage){
		fprintf(erg_f,	"\n\n\t ���������в�ֵ ��ʵ��Ŀ������  CoolRateFlows():"
			"\n\t PDI���� = %6.2f ����ϵ�� = %6.2f �������� = %6.2f  ��ȡĿ������  = %6.2f",	pdiInPut->cr, qcr_act, crCorrect, adapt->tr_foundMean);
		fprintf(erg_f, "\n\t ����ȷ����ʽ nResult= (2: ������; 3: ������; 0: ��ֵ)");
		fprintf(erg_f, "\n\t δ���������ٶ�Ӧ���� = %12.1f [K/s] nResult = %d", 	pdiInPut->cr, iret1);
		OutputFlow(erg_f, fFlwTop1, fCrAct1, fTimeWat1, fFlwTop1 * fFlwFactor1);
		fprintf(erg_f, "\n\t ����������ٶ�Ӧ���� = %12.1f [K/s] nResult = %d", crCorrect, iret2);
		OutputFlow(erg_f, fFlwTop2, fCrAct2, fTimeWat2, fFlwTop2 * fFlwFactor2);
	}

	// �����ܾ���ʱ�趨ΪĬ��ֵ [8/12/2013 лǫ]

	// iret1			= 2;  // ����  [12/31/2013 xie]
	iRet            = iret1;
	cr_flow->flow_t = fFlwTop1;
	cr_flow->cr_akt = fCrAct1;
	cr_flow->time_c = fTimeWat1;
	cr_flow->flow_b = fFlwTop1 * fFlwFactor1;
	cr_flow->aveHC=aveHC1;
	/* set return value if one limitation is on the limit */
	if((iret1 == 0) && (iret2 != 0)) 
		iRet = iret2;
	/* use flow ����ion only if flow is within limitation */		
	if((iret1 == 0) && (iret2 == 0)){
		cr_flow->flow_t = fFlwTop2;
		cr_flow->cr_akt = fCrAct2;
		cr_flow->time_c = fTimeWat2;
		cr_flow->flow_b = fFlwTop2 * fFlwFactor2;
		cr_flow->aveHC=aveHC2;
		iRet = 0;
	}
	if (1==tStage)
	{
		fprintf(erg_f, "\n\n\t Selected flow value iRet = %d", iRet);
		OutputFlow(erg_f, cr_flow);
	}
	return(iRet);
}


/// @����˵�� calculate position edge �ڱ�
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
void CPreProcess::CalculateEdgePosition(
	const	INPUTDATAPDI *pdiInPut,	/*PDI����	*/
	const	RALCOEFFDATA *lay, /* layout data */
	flow_cr *cr_flow,	/* flow value			*/
	PRERESULT *PreResult)		/* PREPROCESS telegram	*/
{
	int i_zero[] = { 50, 100, 200, 200, 100, 50, 50, 0, 0, 0, 0,   0,   0,   0,   0,  0};
	int i_edge = 200;   /*  index of position edge �ڱ�  */
	float   width = pdiInPut->width;
	float   thick = pdiInPut->thick * 1000.f;
	float   flow_t = cr_flow->flow_t;
	float   pos;
	float	fEdgeMin;	/*��Сvalue edging [mm] */
	float	fEdgeMax;	/* ���value edging [mm] */
	int		iDir;		/* -1: open to end of Acc, 1: narrows */
	int		nPoly;		/* polynomial interpolation */
	int		iRex;
	int		ipos;
	int iz;
	int ixx = 0, i;
	char	edge_datxx[PATH_MAX];

	CombinFilePath("EdgeMask_1.txt", PREPROCESS,edge_datxx);

	//лǫ���� ���߲��ڱεĳ�ʼvalue���� 2012-5-20
	for (iz=0;iz<cr_flow->anz_flow;iz++)
		cr_flow->edge[iz]=0;

	switch (cr_flow->switchx) 
	{
		case(3):
			pos  = 47.5f - 0.5456f*thick + 8.8095f*width + 0.1468f*flow_t;
			ipos = (int)pos + cr_flow->offset;
			iz = -1;
			while(++iz < NUZZLE_NUMBER){					
				if(iz > 0)
					cr_flow->edge[iz] = ipos -              // the last four the offset reduced  // 
					(int) (iz * cr_flow->offset / 5.);   // stepwise                          //
				else
					cr_flow->edge[iz] = ipos;              // the first 3 are equival to offset // 
			}
			/////////////////////////////////////////////////////////////////////////////////////
			break;
		case(4):
			iRex = 0;
			if((ReadEdgeInfo(cr_flow->flow_t, pdiInPut->width, edge_datxx, &iDir, &nPoly,&fEdgeMin, &fEdgeMax)) == FALSE)	  
				iRex++;
			if(iRex == 0) {
				if(CalculateInterpolationEdge(lay, iDir, nPoly, fEdgeMin, fEdgeMax, cr_flow->edge) == FALSE)
					iRex++;
			}
			if(iRex == 0)	  break;
			/* in case of FALSE default value will be in use */
		default:      /* No edge Masking */
			iz = -1;
			while(++iz<NUZZLE_NUMBER)  cr_flow->edge[iz] = -2000;
			break;
	}
	for (i = i_edge, iz = -1; i < (NUZZLE_NUMBER+i_edge); i++) 
	{
		iz++;
		PreResult->edgeMask[i] = (int) (500.f * lay->CoolWidth) - 	(int) (500.f * pdiInPut->width)+cr_flow->edge[iz];
		if ( PreResult->edgeMask[i] > 1400) PreResult->edgeMask[i] = 1400;  /* ��� way of edge masks */
		if ( PreResult->edgeMask[i] < 0)    PreResult->edgeMask[i] = 50;
	}               /*  header in operation    */

}


/// @����˵�� ���Ԥ����ʱ�Ĵ�����־
/// @���� st_err ������־ָ��
/// @���� id ���ID
/// @���� mess ������Ϣ
/// @func_n ��������ĺ�����
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
/// void CPreProcess::error_pre( FILE *st_err,    /* error FILE */
void CPreProcess::OutputErrorLog( FILE *st_err,    /* error FILE */
				const   char *id,       /* ��� ID   */
				const   char *mess,     /* error      */
				const   char *func_n)  /* function name */
{
	time_t    calc_zeit;
	time(&calc_zeit);
	logger::LogDebug("system","message:  %s",mess);
	fprintf(st_err,"\n\t  ��� ID: %s  %s", id, asctime(localtime(&calc_zeit)));
	fprintf(st_err,"\n\t  message:  %s", mess);
	fprintf(st_err,"\n\t  function: %s \n", func_n);
	fclose(st_err);
}

void CPreProcess::EstimationComputationalParam(FILE *erg_f, COMPUTATIONAL *compx, const INPUTDATAPDI *pdi, PHYSICAL *ax)
{
	/*      estimation of computational ����  */
	/*      TESTUNG DER MODULE IST FUER DIESE WERTE ERFOLGT !       */
	int     i_number;
	int     iz;
	float   xx;
	int     anz_nodes = NODES_NUMBER;
	float   EPS  = .01f;
	float   CR = 5.0f;

	i_number        =       ax->coolPhaseNumber;

	/*      data input      */
	compx->femCalculationOutput = 0;    /* NO !! output of table in computex.c  */
	compx->thickNodeNumber    = anz_nodes;            /* spaeter anpassen     */
	compx->testParameter = 1;

	compx->calculationStopTime = ax->duration[i_number];
	ax->sw_air     = ax->art[i_number];

	compx->integra = 2;
	compx->thickNodeNumber    = anz_nodes;

	if(ax->sw_air == 0) 
	{          
		iz = -1;
		xx = pdi->finishRollTemp + DD_TEMP;
		while(++iz < compx->thickNodeNumber) {
			compx->startTempDistribution[iz] = xx;
			compx->startAusteniteDistribution[iz] = 100.f;
		}
		compx->detaTime       = pdi->thick * 333.f;
		compx->isSymetric     = 0;    /* nonsymmetric problem */
		if(compx->calculationStopTime < EPS) 
			compx->calculationStopTemp = pdi->finishRollTemp;
	}
	else 	{
		xx = 100.f * pdi->thick /( pdi->cr + 1.f);
		if(xx > 1.f)  
			xx = 1.f;
		if(xx < .001f)
			xx = .01f;
		compx->detaTime       =  xx;
		compx->isSymetric     = 0;    /* nonsymmetric problem */
		if(compx->calculationStopTime < EPS) 
			compx->calculationStopTemp = pdi->targetFinishCoolTemp;
	}

	/* calculation of element sizes compx->lm[]	*/
	CalculationElementSizes(compx, 2, pdi->thick);
	compx->matrixElementNumber = 2 * compx->thickNodeNumber - 1;
}

float CPreProcess::InitAirVariable(COMPUTATIONAL *compx, const INPUTDATAPDI *pdi, PHYSICAL *ax)
{
	float	cr_air = 0.0f;
	float	temp_diff;
	/* preset ���� for start profile */
	/* only modify changing ����	   */
	compx->computeID = 0; /* use of precalculated stop temp */
	ax->duration[0] = 0.0f;
	ax->actualStopTime = 0.0f;

	/* calculation of cr for���� */
	temp_diff = pdi->finishRollTemp + DD_TEMP - compx->calculationStopTemp;
	if(compx->calculationStopTime > 0.0f) 
		cr_air = temp_diff / compx->calculationStopTime;
	compx->localCoolRate = cr_air; 
	return(cr_air);
}

/// @����˵�� ��ʼ�������������ò���
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int	CPreProcess::InitAirStartProfile(     COMPUTATIONAL *compx,
	const			  INPUTDATAPDI *pdi,
	PHYSICAL *ax)
{
	/* preset ���� for start profile */
	compx->computeID = 2;	/*	const Temperature distribution	*/
	ax->coolPhaseNumber = 0;
	ax->method = 1;	/* ����	*/
	ax->art[0] = 0;
	ax->duration[0] = 0.0f;
	ax->taskID = PREPROCESS;
	ax->sw_air = 0;	/*���� */
	ax->actualStopTime = 0.0f;
	ax->actualStartTemp = pdi->finishRollTemp + DD_TEMP;
	compx->calculationStopTemp = pdi->finishRollTemp;
	return(1);
}


/// @����˵�� ���������Ϣ
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::SaveAirTelegram(
	const   int n_max,
	const   float   finishRollTemp,   /* expected ��� Temperature */
	PRERESULT *PreResult,            /* Ԥ������ */
	const   float   *zl,            /* time value air curve */
	const    float   *tl)            /* Temperature value air curve */
{
	float   eps = .001f;    /* lowest Temperature value */
	int iz = -1;
	int i_first = 0;    /* first value for save */
	int i_last  = 0;
	int n_curve = 4;    /* ���value to save */
	int ixx;
	int index;

	/* estimate first value */
	while(++iz < n_max) {
		if(tl[iz] < finishRollTemp) 
			break;
	}

	i_first = iz -2;
	if(i_first < 0) 
		i_first = 0;
	/* estimate last index */
	iz = i_first -1;
	while(++iz < n_max) {
		if(tl[iz] < eps)
			break;
	}
	i_last = iz;
	if(i_last > (i_first + n_curve)) 
		i_last = i_first + n_curve;

	/* store data in telegram */
	iz = i_first -1;
	ixx = -1;
	while(++iz < i_last) {
		ixx++;
		index    = ixx;
		PreResult->airOne[index] = (int)(10.f * zl[i_first + ixx]);
		index    +=  n_curve;
		PreResult->airOne[index] = (int) tl[i_first + ixx];
	}
	return(ixx+1);
}


/// @����˵�� �����value������Ϣ
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::CalculateInterpolationFlow(
	const   int     n,          /* ���value */
	const   float   fCrReq,     /* required ��ȴ�ٶ�*/
	const   float   *fFlwAcc,   /* flow value */
	const   float   *fCrAcc,    /* cooling rates */
	const   float   *fTimeAcc,  /* corresponding cooling times */
	const   float   *fFlwRatio, /* flow ratios */
	const   float   *fFlwFacWidth, /* flow ����ion width */
	const   float   *fFlwHC, /* flow ����ion width */
	float   *fFlwAct,   /* ��value flow value */
	float   *fCrAct,    /* ʵ�� ��ȴ�ٶ�*/
	float   *fTimeAct,  /* ʵ�� required cooling time */
	float   *fFlwRatioAct,
	float *aveHC) /* flow ratio */
{
	int     iRet    = -1;
	int     iz      =  0;
	int     izM1;
	const   int     nM1     = n - 1;
	//const   float   fEpsCr  = 0.1f * fCrAcc[0];
	const   float   fEpsCr  = 0;  // xiugai [11/28/2013 лǫ]
	int     iMonotonFlw;
	int     iMonotonCr;
	float   xr; /* interpolation value */
	float   xr1m; 
	iMonotonFlw = CheckMonoton(n, 1, fFlwAcc);
	iMonotonCr  = CheckMonoton(n, 1, fCrAcc);
	/* �� cooling rates */
	if(((fCrReq < (fCrAcc[0] + fEpsCr)) 
		|| (n < 2)
		|| (iMonotonFlw == FALSE)
		|| (iMonotonCr  == FALSE))) {
		iRet = 2;   /* ��ȴ�ٶ�too small */
		*fFlwAct        = fFlwAcc[0];
		*fCrAct         = fCrAcc[0];
		*fTimeAct       = fTimeAcc[0];
		*fFlwRatioAct   = fFlwRatio[0] * fFlwFacWidth[0];
		*aveHC=fFlwHC[0];
		return(iRet);
	}

	/* high cooling rates */
	if(fCrReq > (fCrAcc[nM1] - fEpsCr)) {
		iRet = 3;   /* ��ȴ�ٶ�too high */
		*fFlwAct        = fFlwAcc[nM1];
		*fCrAct         = fCrAcc[nM1];
		*fTimeAct       = fTimeAcc[nM1];
		*fFlwRatioAct   = fFlwRatio[nM1] * fFlwFacWidth[nM1];
		*aveHC=fFlwHC[nM1];
		return(iRet);
	}
	/* estimate flow ��� */
	while(++iz < n) { /* start with index 1 */
		if(fCrAcc[iz] > fCrReq) break;
	}
	izM1 = iz - 1;
	xr      = (fCrReq - fCrAcc[izM1]) / (fCrAcc[iz] - fCrAcc[izM1]);
	xr1m    =   1.f - xr;
	*fFlwAct        = xr1m * fFlwAcc[izM1]      + xr * fFlwAcc[iz];
	*fCrAct         = xr1m * fCrAcc[izM1]       + xr * fCrAcc[iz] ;
	*fTimeAct       = xr1m * fTimeAcc[izM1]     + xr * fTimeAcc[iz];
	*fFlwRatioAct   = xr1m * fFlwRatio[izM1] * fFlwFacWidth[izM1] + xr * fFlwRatio[iz] * fFlwFacWidth[iz];

	*aveHC       = xr1m * fFlwHC[izM1]     + xr * fFlwHC[iz];  // add [12/31/2013 xie]

	iRet = 0;   /* can be achieved by modification of flow */
	return(iRet);

}


/// @����˵�� �������Ϣ
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::OutputFlow(
	FILE    *erg_f,
	const   flow_cr *cr_flow)
{
	const	float	eps = .1e-6f;
	fprintf(erg_f,"\n\t �ϼ���ˮ���ܶ�:  %6.2f [ltr/(m^2*min)]", cr_flow->flow_t);
	fprintf(erg_f,"\t �¼���ˮ���ܶ�: %6.2f [ltr/(m^2*min)]", cr_flow->flow_b);
	fprintf(erg_f,"\t ʵ������: %6.2f [K/s]", cr_flow->cr_akt);
	fprintf(erg_f,"\t ʵ����ȴʱ��: %6.2f [s]", cr_flow->time_c);
	if(cr_flow->flow_t > eps)
		fprintf(erg_f,"\t ���ϼ���ˮ��:  %6.2f [-]\n", cr_flow->flow_b / cr_flow->flow_t);
	return TRUE;
}

/// @����˵�� �������Ϣ
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::OutputFlow(
	FILE    *erg_f,
	const   float	flow_t,
	const	float	cr_akt,
	const	float	time_c,
	const	float	flow_b)
{
	const	float	eps = .1e-6f;

	fprintf(erg_f,"\n\t �ϼ���ˮ���ܶ�:  %6.2f [ltr/(m^2*min)]", flow_t);
	fprintf(erg_f,"\t �¼���ˮ���ܶ�: %6.2f [ltr/(m^2*min)]", flow_b);
	fprintf(erg_f,"\t ʵ������: %6.2f [K/s]", cr_akt);
	fprintf(erg_f,"\t ʵ����ȴʱ��:  %6.2f [s]", time_c);
	if(flow_t > eps)
		fprintf(erg_f,"\t ���ϼ���ˮ��: %6.2f [-]", flow_b / flow_t);
	return TRUE;
}


/// @����˵�� ��ȡ��Ե��Ϣ
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int	CPreProcess::ReadEdgeInfo(
	const	float	fFlwAct,	/* specific flow top */
	const	float	fWidth,		/* width of ��� */
	const	char	*sFileName,	/* �ļ��� */
	int		*iDir,		/* 1: narrows, -1: opens */
	int		*nPoly,		/* grade of polynomial approach */
	float	*fPosMin,	/* ��С��Եλ�� */
	float	*fPosMax)	/* ��� ��Եλ�� */
{
	int		iRet = FALSE;
	int		iDirX;
	int		nPolyX; /* 1: �����ڲ� 2: ������ */
	int		nReadW;
	int		nReadF;
	float	fW[10];
	float	fF[10];
	float	fFlw1 = 0.0f, fPosMin1 = 0.0f, fPosMax1 = 0.0f;
	float	fFlw2 = 0.0f, fPosMin2 = 0.0f, fPosMax2 = 0.0f; 
	float	rx = 1.f;
	float	fFacRed = 1.0f;
	const	int		nLine = PATH_MAX - 1;
	const	float	fWidth_mm = fWidth * 1000.f;
	const	float	fFlwEps = FLOWTMIN * 0.01f;
	char	*sFormat1 = "%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f";
	char	sLine[PATH_MAX];
	FILE	*fx = NULL;
	if(fFlwAct < FLOWTMIN)	return(iRet);
	if((fx = fopen(sFileName,"r")) == NULL)	{
		printf("\n\t not able to open FILE edge �ڱ�: %s",
			sFileName);
		return(iRet);
	}
	fgets(sLine, nLine, fx);
	fgets(sLine, nLine, fx);
	sscanf(sLine,"%d", &nPolyX);
	*nPoly = nPolyX;
	if(nPolyX < 1 || nPolyX > 2)	*nPoly = 1;
	fgets(sLine, nLine, fx);
	sscanf(sLine,"%d", &iDirX);
	if((iDirX != 1) && (iDirX != -1))	{
		fclose(fx);
		return(iRet);
	}
	*iDir = iDirX;
	fgets(sLine, nLine, fx);
	nReadW = sscanf(sLine, sFormat1,	&fW[0], &fW[1], &fW[2], &fW[3], &fW[4],
		&fW[5], &fW[6], &fW[7], &fW[8], &fW[9]);
	fgets(sLine, nLine, fx);
	nReadF = sscanf(sLine, sFormat1,	&fF[0], &fF[1], &fF[2], &fF[3], &fF[4],
		&fF[5], &fF[6], &fF[7], &fF[8], &fF[9]);

	if((nReadW == nReadF) && (nReadF > 2))
		fFacRed = GetLinearInterpolationFromAbscissa(fW, fF, nReadF - 1, fWidth_mm);

	if(fFacRed < 0.0f || fFacRed > 1.0f)	fFacRed = 1.0f;
	fgets(sLine, nLine, fx);

	while(fgets(sLine, nLine, fx) != NULL)	{
		nReadF = sscanf(sLine, sFormat1, &fFlw2, &fPosMin2, &fPosMax2);
		if(nReadF < 3)	break;
		if(fFlw2 > fFlwAct)	break;
		fFlw1		= fFlw2;
		fPosMin1	= fPosMin2;
		fPosMax1	= fPosMax2;
	}
	if(fFlw2 > (fFlw1 + fFlwEps)) rx = (fFlwAct - fFlw1) / (fFlw2 - fFlw1);
	*fPosMin = ((1.f - rx) * fPosMin1 + rx * fPosMin2) * fFacRed;
	*fPosMax = ((1.f - rx) * fPosMax1 + rx * fPosMax2) * fFacRed;
	if(*fPosMin < EDGEPOSMIN)	*fPosMin = EDGEPOSMIN + 0.2f * EDGEPOSMAX * fFacRed;
	if(*fPosMax > EDGEPOSMAX)	*fPosMax = EDGEPOSMAX * fFacRed;
	fclose(fx);
	return TRUE;
}

/// @����˵�� ������Ҫ˵��-���Ժ��� 
/*  interpolate position of single edge �ڱ�        */
/*  drives to fit curve for position                   */
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int	CPreProcess::CalculateInterpolationEdge(
	const	RALCOEFFDATA *lay, /* layout data */
	const	int		iDir,		/* 1: narrows, -1: opens */
	const	int		nPoly,		/* polynomial interpolation */
	const	float	fPosMin,	/* ��С��Եλ�� */
	const	float	fPosMax,	/* ��� ��Եλ�� */
	int		*iEdgePos)	/* edge pos. individual drives */
{
	int	iRet	= FALSE;
	int	iz		= -1;
	float	zetaEdge[ANZ_EDGE];
	float	zx; /* ʵ�� dimensionless co������of edge �ڱ� group */
	float	vx;	/* ʵ�� dimensionless position edge �ڱ� (/fPosMax) */
	const	float	fDiffPos = fPosMax - fPosMin;
	/* ������ value for �����߲�value */
	const	float	zeta2[] = {0.0f, 0.4f, 1.0f};
	/* ������value for �����߲�value */
	float	eta2[3];
	float	coeff[3];
	if(CalculateEdgePos(lay, zetaEdge) == FALSE) return(iRet);
	if(fPosMax < EDGEPOSMIN) return FALSE;
	if(fPosMin < EDGEPOSMIN) return FALSE;

	while(++iz < lay->nEdge) {
		zx = zetaEdge[iz];
		if(iDir == -1) zx = (1.f - zetaEdge[iz]);
		iEdgePos[iz] = (int) (fPosMin + zx * fDiffPos);
	}

	iz--;
	while(++iz < NUZZLE_NUMBER)	iEdgePos[iz] = 0;

	if(nPoly == 1)	
		return TRUE;

	/* refer to documentation edge �ڱ� */
	eta2[0] = fPosMin / fPosMax;
	eta2[1] = 0.8f;
	eta2[2] = 1.0f; /* = fPosMax / fPosMax */
	/* use linear approach if fails */
	if(CalculatePolynomialCoefficients(zeta2, eta2, coeff) == FALSE) return TRUE;
	iz = -1;
	while(++iz < lay->nEdge) {
		zx = zetaEdge[iz];
		if(iDir == -1) zx = (1.f - zetaEdge[iz]);
		vx = coeff[0] + zx * (coeff[1] + coeff[2] * zx);
		iEdgePos[iz] = (int) (vx * fPosMax);
	}
	return TRUE;
}

/* calculate ���value of all value x[iz]         */
/* value ���value will be = eps at ��С      */

/// @����˵�� ��������б�eps������value
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
float CPreProcess::GetMaxValue(
	const int n,
	const float eps,
	const float *x)
{
	int iz = -1;
	float   xx;
	float max_val_min =  eps;

	/* estimate��Сvalue �������� */
	iz = -1;
	while(++iz < n) {
		xx = x[iz];
		if(xx > max_val_min) max_val_min = xx;
	}
	return(max_val_min);
}

/* calculate��Сvalue of all value x[iz] > eps   */
/* in case of zeros eps will be returned               */

/// @����˵�� �����epsС����Сvalue 
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
float CPreProcess::GetMinValue(const int n, const float eps, const float *x)
{
	int iz = -1;
	float   xx;
	float min_val_max = FLT_MAX;
	/* estimate lowest value �������� */
	while(++iz < n) {
		xx = x[iz];
		if(xx < min_val_max) min_val_max = xx;
	}
	/* estimated��Сvalue must be at least >= eps */
	if(min_val_max < eps) min_val_max = eps;
	return(min_val_max);
}

/*  determine preferred �ٶ� for cooling              */
/*  (adjustment required for each individual           */
/*   installation)                                     */
/// @����˵�� �ⶨ��ѡ����ȴ�ٶ�
/// @���� fThickPlate ��ĺ��
/// @���� n ��ı��
/// @���� fThick ���value
/// @���� fSpd �ٶ�
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
/// float   CPreProcess::speed_pref_ACC(   
float   CPreProcess::AccPreferedSpeed(   
					const	float	fThickPlate,
					const	int		n,			
					const	float	*fThick,		
					const	float	*fSpd)			
{
	float   v_pref_ACC;
	v_pref_ACC =GetLinearInterpolationFromAbscissa( fThick, fSpd, n,fThickPlate);
	return(v_pref_ACC);
}


/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::CheckMonoton(
	const   int n,      /* ���value */
	const   int iDir,   /* -1: decreasing value 1: increasing value */
	const   float   *x) /*  value */
{
	int     iRet = FALSE;
	int     iz = 0;
	const   int     nM1 = n - 1;
	const   float   xDiffMax = iDir * (x[nM1] - x[0]);
	const   float   fEps = .1e-6f;
	float   fDiffStep;
	float   fVal;
	float   fValM1 = x[0];
	if((iDir != 1) && (iDir != -1)) return(iRet);
	if(xDiffMax < fEps) return(iRet);
	while(++iz < n) 
	{
		fVal        =   x[iz];
		fDiffStep = iDir * (fVal - fValM1);
		if(fDiffStep < fEps)    return(iRet);
		fValM1      =   fVal;
	}
	iRet = TRUE;
	return(iRet);
}


/*  calculate relative position of edge �ڱ�        */
/*  group within cooling (0: entry Acc 1: Exit Acc)    */

/// @����˵�� �����Ե�ڱε����λ��
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::CalculateEdgePos(
	const	RALCOEFFDATA *lay, /* layout data */
	float						   *zeta)
{
	int	iz = -1;
	int	iPos1;
	int	iPos2;
	float	fPos1;
	float	fPos2;
	float	fLength = lay->PosZon3Exit - lay->PosZon1Entry;
	if(lay->nEdge < 1)	return FALSE;
	while(++iz < lay->nEdge) zeta[iz] = (float)iz / (float)lay->nEdge;
	if(fLength < 0.01f)	return FALSE;
	iz = -1;
	while(++iz < lay->nEdge)	{
		iPos1 = lay->iPosEdge[iz];
		if(iz < (lay->nEdge - 1))	iPos2 = lay->iPosEdge[iz+1];
		else						iPos2 = lay->nHeader - 1;
		fPos1 = lay->fPosHeader[iPos1];
		fPos2 = lay->fPosHeader[iPos2];
		zeta[iz] = (0.5f * (fPos1 + fPos2) - lay->PosZon1Entry) / fLength;
	}
	return TRUE;
}


/// @����˵�� ȷ������ʽϵ��
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::CalculatePolynomialCoefficients(const	float	*zeta, const	float	*eta, float	*coeff)
{
	const	float	xn = zeta[1] * (1.f - zeta[1]);
	const	float	fEps = 0.1e-5f;
	float	xMax;
	int		iz = -1;
	coeff[0] = eta[0];
	if(xn < fEps)	return FALSE;
	coeff[1] = (eta[1] - eta[2] * zeta[1] * zeta[1]) / xn
		- eta[0] * (1.f + zeta[1]) / zeta[1];
	coeff[2] = (zeta[1] * eta[2] - eta[1]) / xn + eta[0] / zeta[1];
	if(fabs(coeff[2]) < fEps)	return FALSE;
	xMax = eta[0] - 0.25f * coeff[1] * coeff[1] / coeff[2];
	if(xMax > 1.0f)	{
		while(++iz < 3)	coeff[iz] /= xMax;
	}
	return TRUE;
}

// �����ڲ� of ������ value
int   CPreProcess::CalculateAbszValue( const float   *x, const float   *y,  const   int    n, const   float  ord, float	*absz)
{
	int     iz  = 0;
	float   eps = (float).1e-12;
	float   r = 0.0;
	float	y_min = 0.0;
	float	y_max = 0.0;

	y_min = GetArrayMinValue(y, n);
	y_max = GetArrayMaxValue(y, n);

	if(ord < y_min)    
		return(-1);
	if(ord > y_max)	   
		return(-1);

	while(++iz < n) {
		if( fabs(ord - y[iz]) < eps){
			*absz = x[iz];
			return(1);
		}
		if((ord < y[iz]) && (ord > y[iz-1]))	
			break;
		if((ord > y[iz]) && (ord < y[iz-1]))	
			break;	
	}

	r = y[iz] - y[iz-1];
	if (fabs(r) < eps){
		*absz = (float)(.5 * (x[iz] + x[iz-1]) );
		return(1);
	}

	r = (ord - y[iz-1])/r;
	*absz     = (float)((1. -r) * x[iz-1] + r * x[iz]);
	return(1);
}

/* Search for maxium value */

/// @����˵�� ��������е����value
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
float	CPreProcess::GetArrayMaxValue( const float *y, const	int	n)
{
	float	max_wertx = (float)-1.e18;
	int	iz = -1;

	while(++iz < n)	
		if(y[iz] > max_wertx) 
			max_wertx = y[iz];
	return(max_wertx);
}

/* Search for��Сvalue */
/// @����˵�� ��������е���Сvalue
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
float	CPreProcess::GetArrayMinValue( const float *y, const int	n)
{
	float	min_wertx = (float)1.e18;
	int	iz = -1;

	while(++iz < n)	
		if(y[iz] < min_wertx)
			min_wertx = y[iz];

	return(min_wertx);
}


/// @����˵�� �ж���Ϣ��Ч��
/// @���� pdi 
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
int	CPreProcess::CheckDataValid(INPUTDATAPDI *pdi)
{
	float	tf_max	=	(float)MAXTEMP;	/* ��� possible Tf	*/
	float	tf_min	=	600.f;	/* ��Сpossible Tf	*/
	float	tu_max  =	1000.f;	/* ��� ufc ֹͣ��ȴtemp.*/
	float	tu_min	= 	10.f;	/* ��Сufc ֹͣ��ȴtemp	*/
	float	tc_max  =	1000.f;	/* ��� ֹͣ��ȴtemp.*/
	float	tc_min	= 	10.f;	/* ��Сֹͣ��ȴtemp	*/
	float	thick_max =	0.50f;	/* ��� thickness of material */
	float	thick_min =	.003f;	/* ��Сthickness	*/
	float	dTempMin;	/* ��Сrequired Temperature difference */

	int err=0;
	if (pdi->acc_mode ==  NOACC)
	{
		return 0;
	}
	if((pdi->finishRollTemp + 51.f) > tf_max)	return(1);//����Temperature�������쳣
	if(pdi->finishRollTemp < tf_min)	        return(2);//����Temperature�������쳣
	if(pdi->targetFinishCoolTemp > tc_max)	        return(3);//Ŀ������Temperature�������쳣
	if(pdi->targetFinishCoolTemp < tc_min)	        return(4);//Ŀ������Temperature�������쳣
	if(pdi->finishRollTemp < pdi->targetFinishCoolTemp)	    return(5);//����С������Temperature�쳣
	if((pdi->acc_mode !=  IC)  &&
		(pdi->acc_mode !=  SC)  &&
		(pdi->acc_mode !=  ACC) &&
		(pdi->acc_mode !=  UFC) &&
		(pdi->acc_mode !=  DQ) &&
		(pdi->acc_mode !=  BACKACC)
		)           
		return(6);//��ȴģʽ�쳣
	if(pdi->thick > thick_max)	        return(7);//��ȳ������쳣
	if(pdi->thick < thick_min)	        return(8);//��ȳ������쳣

	if((dTempMin = CheckMinTemperatureDiff(pdi->thick)) > (pdi->finishRollTemp - pdi->targetFinishCoolTemp))
		//return(9);//��ȴ�½�value��С�쳣
		return(0);

	return(0);
}

// test for ��Сrequired Temperature difference 
/// @����˵�� ������Temperature����
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
float	CPreProcess::CheckMinTemperatureDiff(const	float dThick)
{
	float	dTempMin;	/* ��Сrequired Temp difference */
	const	double	dThick_mm = dThick * 1000.;
	const	int		iNthickx = 12;
	const	double	dThickx[] = {	 3.,  10., 15.,  20., 30., 40.,
		50.,  60., 80., 100., 200., 800.};
	const	double	dTempx[]  = {  180., 100., 80.,  60., 40.,  30.,
		30.,  30., 30.,  30., 30.,  30.}; 
	dTempMin = (float) CalculateInterpolationValue(iNthickx, dThick_mm, dThickx, dTempx);
	return(dTempMin);
}

/* �����ڲ� of function value y(absz)   */
/*                                                  */
/* Output data:                                     */
/* - ord:   ������value ord = y(absc) given by   */
/*          �����ڲ�                    */
/*                                                  */
/*                                                  */
/* Input data:                                      */
/* - absz:  ������ value                          */
/* - n:     No. of function value for curve        */
/* - x:     ������ value of curve                */
/* - y:     ������value for given abscissas     */
/*                                                  */
/* additional ��Ϣ:                          */
/* - linear extrapo�ation from first or last two    */
/*   value if absc                                 */
/*   is not within [x[0], x[n-1]]                   */
/* - return value y[0] if n == 1                    */
double   CPreProcess::CalculateInterpolationValue(
	const   int      n,     /* ���value        */
	const   double   absz,  /* ��value absc.   */
	const   double   *x,    /* ������ value      */
	const   double   *y)    /* ������value      */
{
	double   ord;            /* ��value value */
	int     iz  = 0;
	double   eps = .1e-12;
	double   r;

	if(n < 1)  return(0.0);
	if(n == 1) return(y[0]);
	if(absz < x[0])     {   /* linear extension */
		ord = y[0];
		r   = x[1] - x[0];
		if(fabs(r) < eps)   return(y[0]);
		r = (absz - x[0]) / r;
		ord     = (1.- r) * y[0] + r * y[1];
		return(ord);
	}                   /* linear extension */
	if(absz > x[n-1])   {   /* linear extension */
		ord = y[n-1];
		r   = x[n-1] - x[n-2];
		if(fabs(r) < eps)   return(y[0]);
		r = (absz - x[n-2]) / r;
		ord     = (1. - r) * y[n-2] + r * y[n-1];
		return(ord);
	}                   /* linear extension */

	while(++iz < n) {
		if(absz < x[iz])        break;
	}

	r  = x[iz] - x[iz-1];
	if (fabs(r) < eps)      {
		ord = .5 * (y[iz] + y[iz-1]);
		return(ord);
	}

	r = (absz - x[iz-1])/r;
	ord = (1. - r) * y[iz-1] + r * y[iz];
	return(ord);
}


int CPreProcess::OutputPreInfo( INPUTDATAPDI *xzz, FILE *erg_f)
{
	time_t		zeit;
	time(&zeit);

	fprintf(erg_f, "\n\t %8s\t%s time: %s\n",xzz->file_nam, xzz->plateID, asctime(localtime(&zeit)));
	fprintf(erg_f,"\n\t  �ְ�ID		   %s     date:  %s",xzz->plateID, ctime(&zeit));//���ID
	fprintf(erg_f,"\n\t  ���					%8g mm", 1000. * (xzz->thick));//Thickness
	fprintf(erg_f,"\n\t  ���					 %8g mm", 1000. * (xzz->width));//Width of the ���
	fprintf(erg_f,"\n\t  ����					 %8g mm", 1000. * (xzz->length));//Length of the ��� 
	fprintf(erg_f,"\n\t  ����Temperature				 %8g Grad C", xzz->finishRollTemp);//��� mill temp.
	fprintf(erg_f,"\n\t  ����Temperature				  %8g Grad C", xzz->targetFinishCoolTemp);//Cooling stop temp.
	fprintf(erg_f,"\n\t  Ŀ����ȴ�ٶ�			 %8g K/s", xzz->cr);//Target cooling rate
	fprintf(erg_f,"\n\t  ��ȴ��ʽ(1:����3:������)     %d", xzz->acc_mode);//ACC-mode
	fprintf(erg_f,"\n\t  ������ʽ(1:���Զ�2:�Զ�)     %d", xzz->op_mode);//Op-mode
	fprintf(erg_f,"\n\t  ��Ʒ����	     %12s", xzz->matID );//Material identifier

	fprintf(erg_f,"\n\t  �Ͻ�Ԫ�غ���");
	fprintf(erg_f,"\n\t  ̼			   %8g [C]", xzz->alloy_c);
	fprintf(erg_f,"\n\t  ��			   %8g [Cr]", xzz->alloy_cr);
	fprintf(erg_f,"\n\t  ͭ			   %8g [Cu]", xzz->alloy_cu);
	fprintf(erg_f,"\n\t  ��			   %8g [Mn]", xzz->alloy_mn);
	fprintf(erg_f,"\n\t  ��			   %8g [Mo]", xzz->alloy_mo);
	fprintf(erg_f,"\n\t  ��			   %8g [Ni]", xzz->alloy_ni);
	fprintf(erg_f,"\n\t  ��			   %8g [Si]", xzz->alloy_si);
	fprintf(erg_f,"\n\t  ��			   %8g [Ti]", xzz->alloy_ti);
	fprintf(erg_f,"\n\t  ��			   %8g [Nb]", xzz->alloy_nb);
	fprintf(erg_f,"\n\t  ��			   %8g [B]", xzz->alloy_b);
	fprintf(erg_f,"\n\t  ��			   %8g [V]\n\n", xzz->alloy_v);
	return(1);
}

/// @����˵�� ���������Ϣ
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
flow_cr CPreProcess::GetFlowInfo(
	FILE	*st_err, 
	const	INPUTDATAPDI *pdiInPut,
	const	STRATEGY   *accStrat1,
	PRERESULT *PreResult,
	const float waterFactorCoe
	)
{
	flow_cr   cr_flow ;
	int         formula, offset;
	int			iOpen = 1; /* 0: default value 1: read from FILE */
	float       flow_rate_top;
	float       thick_mm = pdiInPut->thick * 1000.f;  /* thickness of ��� in mm */
	int         iz = -1, izx;
	FILE        *flow_dat = NULL;
	float       flow_factor;	/* ��value flow ratio */
	float		flow_factor1, flow_factor2; /* value to be in use for interpolation */
	float		rx;		/* interpolation value thickness */
	//char        *flow_datxx;      
	char        fbuf[PATH_MAX];
	int         ixx = 0;
	int			nReadTck = 0;	/* thickness value */
	int			nReadFlw = 0;
	int			iReadErr = 0;
	float		fRatioDef = 2.9f; /* default ratio btm/top */
	int         nFlw;				/* No. of flow value */
	float       thx[ANZ_THICK];     /*different max. thicknesses for value in mm    */
	float       fhx[ANZ_THICK];     /*different flow ratios as function of thickness */

	int       nSlitFlw=0;                //��϶�εĲ���������

	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 

	///////////////////////Modified by Zed////////////////
	//const	float flow_rate_topx_ACC[]=	   
	//{ 400,450,500,550};
	//const	float flow_rate_topx[]=	   
	//{ 800,900,1000,1100};
	///* default wter ratios */   // �˴���Ҫ�޸�  ��Ӧ�ֳ���� 2012-7-12
	//const	float flow_factorx[] =   
	//{1.5+ufcs->waterFactorManual, 1.5+ufcs->waterFactorManual, 1.5+ufcs->waterFactorManual,1.5+ufcs->waterFactorManual};
	//const	int		nReadMax = PATH_MAX - 1;

	/* default water flows */   //лǫ ����ϸ�2800 �趨��϶��ˮ���ܶ�

	const	float flow_rate_topx_ACC[]=	   
	{   PreResult->readFlowDnLimit, PreResult->readFlowDnLimit+(PreResult->readFlowUpLimit-PreResult->readFlowDnLimit)/3, PreResult->readFlowDnLimit+(PreResult->readFlowUpLimit-PreResult->readFlowDnLimit)*2/3,PreResult->readFlowUpLimit
	};

	const	float flow_rate_topx[]=	   
	{   PreResult->readFlowDnLimitUFC,PreResult->readFlowDnLimitUFC+(PreResult->readFlowUpLimitUFC-PreResult->readFlowDnLimitUFC)/3,PreResult->readFlowDnLimitUFC+(PreResult->readFlowUpLimitUFC-PreResult->readFlowDnLimitUFC)*2/3,PreResult->readFlowUpLimitUFC
	};

	/* default wter ratios */   // �˴���Ҫ�޸�  ��Ӧ�ֳ���� 2012-7-12
	const	float flow_factorx[] ={PreResult->readWaterFactor,PreResult->readWaterFactor,PreResult->readWaterFactor,PreResult->readWaterFactor};
	const	int		nReadMax = PATH_MAX - 1;
	
	const	float gap_flow_rate_topx[]=	   
	{   PreResult->readFlowDnLimitUFC,PreResult->readFlowDnLimitUFC+(PreResult->readFlowUpLimitUFC-PreResult->readFlowDnLimitUFC)/3,PreResult->readFlowDnLimitUFC+(PreResult->readFlowUpLimitUFC-PreResult->readFlowDnLimitUFC)*2/3,PreResult->readFlowUpLimitUFC
	};

	const	float gap_flow_factorx[] = {PreResult->readWaterFactor,PreResult->readWaterFactor,PreResult->readWaterFactor,PreResult->readWaterFactor} ;

	iOpen=0;
	if(iOpen == 0) 
	{
		nFlw = sizeof(flow_rate_topx) / sizeof(flow_rate_topx[0]);
		nSlitFlw = sizeof(gap_flow_rate_topx) / sizeof(gap_flow_rate_topx[0]);

		cr_flow = InitFlow(nFlw,nSlitFlw);

		if ((BACKACC==pdiInPut->acc_mode)||   //2012-7-13 accģʽʱ ˮ���ܶȷ�ΧӦ��С
			(ACC==pdiInPut->acc_mode)){
			iz = -1;
			while(++iz < cr_flow.anz_flow) {
				cr_flow.flow_rate[iz]			= flow_rate_topx_ACC[iz];
				cr_flow.flow_factor[iz]		= flow_factorx[iz]+waterFactorCoe;
				cr_flow.flow_factor_width[iz]	= 1.f;
				cr_flow.flow_factor_dq[iz]		= flow_factorx[iz];
			}
		}else{
			iz = -1;
			while(++iz < cr_flow.anz_flow) {
				cr_flow.flow_rate[iz]			= flow_rate_topx[iz];
				cr_flow.flow_factor[iz]		= flow_factorx[iz]+waterFactorCoe;
				cr_flow.flow_factor_width[iz]	= 1.f;
				cr_flow.flow_factor_dq[iz]		= flow_factorx[iz];
			}
		}

		iz = -1;
		while(++iz < cr_flow.slit_anz_flow) {
			cr_flow.gap_flow_rate[iz]			= gap_flow_rate_topx[iz];
			cr_flow.gap_flow_factor[iz]		= gap_flow_factorx[iz];
			cr_flow.gap_flow_factor_width[iz]	= 1.f;
		}


		cr_flow.iadaption		= 1; /* use adaption (default) */
		cr_flow.write_adapt	= 1; /* write to adaption */
		cr_flow.reduction_factor = 1.f; /* no flow reduction vs. length */
		cr_flow.last_dq = NUZZLE_NUMBER - 1; /* last header in use for DQ */
		cr_flow.switchx = 4;	/* flow ����ion edge �ڱ� */
		cr_flow.offset    = 0; /* offset edge �ڱ� [mm] (default 0 ) */

		PreResult->calcWaterFactor=cr_flow.flow_factor[0];   //add by xie 11-26
	}else {	// can read ������Ϣ from FILE

		fgets(fbuf, nReadMax, flow_dat);   // read one comment line 
		iz = -1;
		fgets(fbuf, nReadMax, flow_dat);
		sscanf(fbuf,"%d", &nFlw);
		cr_flow = InitFlow(nFlw,nSlitFlw);

		fgets(fbuf, nReadMax, flow_dat);
		nReadTck = sscanf(fbuf,"%f%f%f%f%f%f%f%f%f%f%f", &(thx[0]),&(thx[1]),
			&(thx[2]),&(thx[3]),&(thx[4]),&(thx[5]),&(thx[6]),
			&(thx[7]),&(thx[8]),&(thx[9]),&(thx[10]));
		if(nReadTck < 2)	
			iReadErr++;
		if(DEBUG_wrong) 
			printf("\n\t anz_flow = %d  thx[0,9] : %f %f",cr_flow.anz_flow, thx[0], thx[9]);
		/* loop over differenet flows */
		iz = -1;
		while(++iz < cr_flow.anz_flow) {   /* loop over differenet flows */
			fgets(fbuf, nReadMax, flow_dat);
			nReadFlw = sscanf(fbuf,"%f%f%f%f%f%f%f%f%f%f%f", &flow_rate_top ,
				&(fhx[0]),&(fhx[1]),&(fhx[2]),&(fhx[3]),
				&(fhx[4]),&(fhx[5]),&(fhx[6]),&(fhx[7]),
				&(fhx[8]),&(fhx[9]));
			if(nReadFlw != nReadTck)
				iReadErr++;
			cr_flow.flow_rate[iz] = flow_rate_top;
			izx = -1;  
			while(++izx < (nReadFlw - 1)) { /* estimate ���� thickness class */
				if(thick_mm < thx[izx]) 
					break;
			} /* estimate ���� thickness class */

			if(iReadErr == 0){
				/* thck[0] beachten, ist 0 deshalb shift um 1 Index zwischen Dicken und ratios */
				flow_factor = fhx[izx-1]; /* second value for interpolation */
				if((izx > 1) && (izx < (nReadFlw - 1)))
				{	/* interpolate vs. thickness */
					flow_factor1 = fhx[izx-2];
					flow_factor2 = fhx[izx-1];
					rx = (thick_mm - thx[izx-1]) / (thx[izx] - thx[izx-1]);
					flow_factor = (1.f - rx) * flow_factor1 + rx * flow_factor2;
				}	/* interpolate vs. thickness */
			}else
				flow_factor = fRatioDef; /* set default flow ratio in exception case */

			cr_flow.flow_factor[iz]	= flow_factor;
			cr_flow.flow_factor_dq[iz] = flow_factor;
			if(DEBUG_wrong)
				printf("\n\t flow: %f flow_factor  %f  izx : %d", 
				flow_rate_top, flow_factor, izx);
		}                           /* loop over differenet flows */

		fgets(fbuf, nReadMax, flow_dat);
		sscanf(fbuf,"%d%d%d%d", &formula, &offset,  &(cr_flow.iadaption), &(cr_flow.write_adapt));


		fgets(fbuf, nReadMax, flow_dat);
		sscanf(fbuf,"%d%f",     &(cr_flow.last_dq), &(cr_flow.reduction_factor));

		if(formula<0 || formula>4) formula = 4;
		if(offset<-150 || offset>450)  offset = 0; /* weer */

		if(cr_flow.iadaption<0 || cr_flow.iadaption>1)
			cr_flow.iadaption = 0;
		if(cr_flow.reduction_factor > 1.2f)    cr_flow.reduction_factor = 1.f;
		if(cr_flow.reduction_factor < .8f) cr_flow.reduction_factor = .8f;
		if(cr_flow.last_dq > NUZZLE_NUMBER) cr_flow.last_dq = NUZZLE_NUMBER - 1;
		if(cr_flow.last_dq < cr_flow.anz_flow)
			cr_flow.last_dq = cr_flow.anz_flow;


		cr_flow.switchx   = formula;
		cr_flow.offset    = offset;

		fgets(fbuf, nReadMax, flow_dat);
		sscanf(fbuf,"%d", &cr_flow.iFlwLimit);

		/* set default to model flows to achieve good flatness */
		if((cr_flow.iFlwLimit < 1) || (cr_flow.iFlwLimit > 3)) 
			cr_flow.iFlwLimit = 2;

	}	/* can read   from FILE */

	strcpy_s(cr_flow.sPatternID, "0");
	cr_flow.fLengthTotal = 0.0f;
	cr_flow.vx_mean      = 0.0f;
	//	free(flow_datxx);
	if(iOpen == 1) 
		fclose(flow_dat);
	ModifyFlow(st_err, pdiInPut, accStrat1, &cr_flow);
	ReadInterpolateFlowWidth(st_err, pdiInPut, cr_flow.anz_flow, cr_flow.flow_rate, 
		cr_flow.flow_factor_width);

	return(cr_flow);
}

int	CPreProcess::ModifyFlow(
	FILE	*st_err, 
	const	INPUTDATAPDI *pdiInPut,
	const	STRATEGY *accStrat1,
	flow_cr *cr_flow)
{
	int		nFlw = cr_flow->anz_flow;
	const	float	fFlwMinMin = cr_flow->flow_rate[0];	     /* ultimate �� flow */
	const	float	fFlwMaxMax = cr_flow->flow_rate[nFlw-1]; /* ultimate high flow */
	float	fFlwMin;	/* ��  limit flow */
	float	fFlwMax;	/* high limit flow */
	const	float	fTck_m    = pdiInPut->thick;
	const	float	fDeltaTemp = pdiInPut->finishRollTemp - pdiInPut->targetFinishCoolTemp;
	float	fDeltaTempMin;

	fFlwMin = GetLinearInterpolationFromAbscissa(	accStrat1->fThick, accStrat1->fFlwMin, 
		accStrat1->nThick, fTck_m);
	fFlwMax = GetLinearInterpolationFromAbscissa(	accStrat1->fThick, accStrat1->fFlwMax, 
		accStrat1->nThick, fTck_m);
	fDeltaTempMin = GetLinearInterpolationFromAbscissa(	accStrat1->fThick, accStrat1->fFlwTmp, 
		accStrat1->nThick, fTck_m);

	if(	(fFlwMin < fFlwMinMin) &&
		(fFlwMax > fFlwMaxMax))	
		return(nFlw);

	if(	(fDeltaTemp < fDeltaTempMin) ||
		(fDeltaTempMin < DD_TEMP))
		return(nFlw);

	nFlw = ModifyLimits(nFlw, fFlwMin, fFlwMax, 
		cr_flow->flow_rate, cr_flow->flow_factor);

	return(nFlw);
}


/* read and interpolate flow factor bottom */
int	CPreProcess::ReadInterpolateFlowWidth(
	FILE	*st_err, 
	const	INPUTDATAPDI *pdiInPut,
	const	int		nFlw,
	const	float	*fFlw,
	float	*flow_factor_width)
{
	int		nRead = 0;
	int		iz = -1;
	int		iControl;
	int		iZeile;
	int		iLeft;
	int		nFlwRead = 0;
	float	*fFlwValue;
	float	*fFlwFactor;
	float	fRx;
	int		nReadWidth;
	int		nReadRow;
	const	int		nRowMax = PATH_MAX - 1;
	const	float	fWidth_mm = pdiInPut->width * 1000.f;
	const	float	fEpsWidth = fWidth_mm * .1e-04f;
	const	char	*sFormat = "%f%f%f%f%f%f%f%f%f%f%f%f";
	char	flow_datxx[PATH_MAX];
	char	fbuf[PATH_MAX];
	float	fFlwAct;
	float	fWx[11];
	FILE	*flow_dat = NULL;

	while(++iz < nFlw) flow_factor_width[iz] = 1.0f;

	CombinFilePath("FLOW_BC.DAT", PREPROCESS,flow_datxx);
	if((flow_dat = fopen(flow_datxx, "r")) == NULL)	
		return(nRead);

	fgets(fbuf, nRowMax, flow_dat);
	fgets(fbuf, nRowMax, flow_dat);
	sscanf(fbuf,"%d%d", &nFlwRead, &iControl);
	if((nFlwRead < 1) || (iControl == 0))	{
		fclose(flow_dat);
		return(nRead);
	}
	if((fFlwValue = CALLOC(nFlwRead, float)) == NULL) {
		fclose(flow_dat);
		return(nRead);
	}
	if((fFlwFactor = CALLOC(nFlwRead, float)) == NULL) {
		fclose(flow_dat);
		free(fFlwValue);
		return(nRead);
	}

	fgets(fbuf, nRowMax, flow_dat);
	nReadWidth = sscanf(fbuf, sFormat, &fFlwAct, &(fWx[0]),&(fWx[1]),
		&(fWx[2]),&(fWx[3]),&(fWx[4]),&(fWx[5]),&(fWx[6]),
		&(fWx[7]),&(fWx[8]),&(fWx[9]),&(fWx[10]));
	nReadWidth -= 1; /* first column is flow value */
	if(nReadWidth < 2)	{
		fclose(flow_dat);
		free(fFlwValue);
		free(fFlwFactor);
		return(nRead);
	}
	iz = 0;
	while(++iz < nReadWidth)	{
		if(fWx[iz] < (fWx[iz-1] + fEpsWidth))	{
			fclose(flow_dat);
			free(fFlwValue);
			free(fFlwFactor);
			return(nRead);
		}
		if(fWx[iz] > fWidth_mm)	break;
	}
	iLeft  = 0;
	fRx    = 0.0f;
	if(iz == nReadWidth)	{
		iLeft  = nReadWidth - 2;
		fRx    = 1.0f;
	}
	if((iz > 0) && (iz < nReadWidth))	{
		iLeft = iz - 1;
		fRx   = (fWidth_mm - fWx[iz-1]) / (fWx[iz] - fWx[iz-1]);
	}
	iZeile = 0;
	while((fgets(fbuf, nRowMax, flow_dat)) != NULL)	{
		nReadRow = sscanf(fbuf, sFormat, &fFlwAct, &(fWx[0]),&(fWx[1]),
			&(fWx[2]),&(fWx[3]),&(fWx[4]),&(fWx[5]),&(fWx[6]),
			&(fWx[7]),&(fWx[8]),&(fWx[9]),&(fWx[10]));
		nReadRow -= 1;
		if(nReadRow != nReadWidth)	break;
		if(iZeile == nFlwRead)		break;
		fFlwValue[iZeile]	 = fFlwAct;
		fFlwFactor[iZeile++] = (1.f - fRx) * fWx[iLeft] + fRx * fWx[iLeft+1];
	}
	iz = -1;
	while(++iz < nFlw)	{
		flow_factor_width[iz] = GetLinearInterpolationFromAbscissa(fFlwValue, fFlwFactor, iZeile, fFlw[iz]);
	}
	fclose(flow_dat);
	free(fFlwValue);
	free(fFlwFactor);
	return(nRead);
}


//??
flow_cr CPreProcess::InitFlow(const	int	nFlw,const int nslitFlw)
{
	/* allocate cr_flow, Number must also cover requirements for  */
	/* ��ȴģʽ refer to: SetCoolPatternFlowValues() for more details  */
	/* nMax = nHeader + 2 (distributed pattern each second        */
	/* is active + air ��ȴ�� Entry + Air ��ȴ�� exit */
	flow_cr	cr_flow ;
	int	nFlwX = NUZZLE_NUMBER + 4;  
	int nSlitFlwX = NUZZLE_NUMBER +4;

	if(nFlwX < nFlw)
		nFlwX = nFlw;

	cr_flow.anz_flow	= nFlw; /* No. of different flows */
	cr_flow.anz_width	= nFlw;

	cr_flow.time_c_highdensity=0;          //лǫ���� 2012-5-3 

	//лǫ���� 2012-5-7 ��϶�ε���Ϣ
	cr_flow.slit_anz_flow	= nslitFlw; /* No. of different flows */
	cr_flow.slit_anz_width	= nslitFlw;

	/* set value to default value */
	cr_flow.LengthMaskHeadTop		= 0.0f;
	cr_flow.LengthMaskHeadBtm		= 0.0f;
	cr_flow.flowMaskHeadTop		= 1.0f;
	cr_flow.flowMaskHeadBtm		= 1.0f;
	cr_flow.nuzzleMaskHeadTop			= 0;
	cr_flow.nuzzleMaskHeadBtm			= 0;
	cr_flow.LengthMaskTailTop		= 0.0f;
	cr_flow.LengthMaskTailBtm		= 0.0f;
	cr_flow.flowMaskTailTop		= 1.0f;
	cr_flow.fFlwMaskTailBtm		= 1.0f;
	cr_flow.nuzzleMaskTailTop			= 0;
	cr_flow.nuzzleMaskTailBtm			= 0;

	return(cr_flow);
}

/* set flow value if ��ȴģʽ is defined                        */
int	CPreProcess::SetCoolPatternFlowValues(
	flow_cr							*cr_flow,	/* flow value		*/	
	const	INPUTDATAPDI *pdi_mds,	/* PDI              */
	const	RALCOEFFDATA *lay,		/* layout data      */
struct T_AccSysPar				*SysPar,
struct T_AccPattern				*AccPattern)
{
	int		iz = -1;
	int		nHeader		= 0; /* Number of headers */
	int		nEdge       = ANZ_EDGE; /* No. of edge masks */
	int		nCoolZone	= 0; /* Number different cooling zones */
	int		iZoneStart	= 0; /* index header ʵ�� ��ȴ�� */
	int		iZoneStop;		 /* index last ��ȴ�� */
	int		iFlwValTop[NUZZLE_NUMBER];
	int		iFlwValBtm[NUZZLE_NUMBER];
	int		iFlwActTop[NUZZLE_NUMBER];
	int		iFlwActBtm[NUZZLE_NUMBER];
	int		iFlwDif[NUZZLE_NUMBER];
	int		iEdgeMaskMode[NUZZLE_NUMBER];
	float	vx = (float) AccPattern->m_Speed; /* preset �ٶ� */
	float	vxMax = lay->MaxSpeedCool[0];
	float	fLengthZone; /* length of ��ȴ�� */
	float	fLengthTotal = 0.0f;	/* length of impingement zones */
	float	fLengthHeader = lay->CoolBankLen;
	float	fFlwRatio; /* flow ratio bottom/top */
	float	fFacPerc = 0.01f; /* percent to value */
	int		iFlwTopMax = 0;
	int		iFlwBtmMax = 0;
	int		iFlwSpec; /* specific flow top [ltr/(m^2 * min)] */
	const	int		iDimFlw		= pdi_mds->controlAcc.iDimFlw;
	const	int		iDimEdge	= pdi_mds->controlAcc.iDimEdge;
	const	int		iDimLenMask = pdi_mds->controlAcc.iDimLenMask;
	const	float	fDimFlwX[]  = {1.0f, 1.0f, 1.0f};
	const	float	fAreaRef    = lay->CoolWidth * lay->fLengthHeader[0];
	float	fFacFlw     = fDimFlwX[iDimFlw];
	const	float	fDimLenX[]  = {1.0f, 0.001f, 1.f};
	/* length �ڱ� is required in [m] */
	const	float	fFacLenMask = fDimLenX[iDimLenMask];

	nHeader = NUZZLE_NUMBER;
	if (vx < lay->MinSpeedCool) 
		vx = lay->MinSpeedCool;
	if (vx > vxMax) vx = vxMax;
	/* specific flow is required in ltr/(m^2 * min)] */
	if(iDimFlw == 2) 
		fFacFlw /= fAreaRef; /* if flow per unit is given */ 
	/* convert strings from database structure */
	CSVToInt(AccPattern->m_FlowTop,		nHeader, iFlwValTop);
	CSVToInt(AccPattern->m_FlowBot,		nHeader, iFlwValBtm);
	CSVToInt(AccPattern->m_NozzleTop,	nHeader, iFlwActTop);
	CSVToInt(AccPattern->m_NozzleBot,	nHeader, iFlwActBtm);

	CSVToInt(AccPattern->m_EdgeMask,	nEdge, iEdgeMaskMode);
	/* check plausibility of value */
	if((nEdge < 0) || (nEdge > ANZ_EDGE))	{
		nEdge = ANZ_EDGE;
		iz = -1;
		while(++iz <nEdge) 
			iEdgeMaskMode[iz] = -1;
	}
	cr_flow->nEdgeMask = nEdge;
	iz = -1;
	while(++iz < nEdge)	{
		cr_flow->iEdgeMaskMode[iz] = iEdgeMaskMode[iz];
	}
	/* set value for ͷ�� �ڱ� */
	cr_flow->flowMaskHeadTop    = fFacFlw * fFacPerc * (float) AccPattern->m_HeadFlowTop;
	cr_flow->flowMaskHeadBtm    = fFacFlw * fFacPerc * (float) AccPattern->m_HeadFlowBot;
	cr_flow->LengthMaskHeadTop = (float) (fFacLenMask * AccPattern->m_HeadLengthTop);
	cr_flow->LengthMaskHeadBtm = (float) (fFacLenMask * AccPattern->m_HeadLengthBot);
	/* set value for β�� �ڱ� */
	cr_flow->flowMaskTailTop    = fFacFlw *  fFacPerc * (float) AccPattern->m_TailFlowTop;
	cr_flow->fFlwMaskTailBtm    = fFacFlw *  fFacPerc * (float) AccPattern->m_TailFlowBot;
	cr_flow->LengthMaskTailTop = (float) (fFacLenMask * AccPattern->m_TailLengthTop);
	cr_flow->LengthMaskTailBtm = (float) (fFacLenMask * AccPattern->m_TailLengthBot);


	/* selct active headers only */
	iz = -1;
	while(++iz < nHeader) {
		iFlwValTop[iz]			*= iFlwActTop[iz];
		if(iFlwValTop[iz] > iFlwTopMax) 
			iFlwTopMax =  iFlwValTop[iz];
		cr_flow->iFlwTop[iz]	 = (int) (iFlwValTop[iz] * fFacFlw);
		iFlwValBtm[iz]			*= iFlwActBtm[iz];
		if(iFlwValBtm[iz] > iFlwBtmMax) 
			iFlwBtmMax =  iFlwValBtm[iz];
		cr_flow->iFlwBtm[iz]	 = (int) (iFlwValBtm[iz] * fFacFlw);
	}

	/* determine number of different cooling zones */
	cr_flow->anz_flow = ColumnDiff(nHeader, iFlwValTop, iFlwValBtm, iFlwDif);

	/* generate specific flows, cooling time and kind of cooling */
	/* store flows and duration with start index 1 because 0 is in */
	/* use for���� before ACC */
	iz = -1;
	while(++iz < cr_flow->anz_flow) {
		iZoneStop	=	iFlwDif[iz];
		iFlwSpec	=	iFlwValTop[iZoneStart];
		if(iFlwSpec == 0) 
			fFlwRatio   = 0.0f;
		else 
			fFlwRatio   =	((float) iFlwValBtm[iZoneStart]) / 
			((float) iFlwSpec);
		fLengthZone =	fLengthHeader * (iZoneStop - iZoneStart);
		cr_flow->flow_rate[iz+1] = fFacFlw * (float)iFlwSpec;
		cr_flow->zeit_stop[iz+1] = fLengthZone / vx;
		cr_flow->flow_factor[iz+1] = fFlwRatio;
		cr_flow->fLengthTotal += fLengthZone;
		iZoneStart = iZoneStop;
	}
	cr_flow->flow_t =  fFacFlw * (float) iFlwTopMax;
	cr_flow->flow_b =  fFacFlw * (float) iFlwBtmMax;
	cr_flow->iadaption		= 1;
	cr_flow->write_adapt	= 1;
	cr_flow->vx_mean		= vx;
	cr_flow->fLengthTotal	= fLengthTotal;
	strcpy_s(cr_flow->sPatternID, AccPattern->m_PatternID);
	return TRUE;
}


int CPreProcess::ColumnDiff(
	const   int     n,          /* number of elements       */
	const   int     *iCol1,     /* basic column 1 [n]       */
	const   int     *iCol2,     /* basic column 2 [n]       */
	int     *iDif)      /* index of changing value  */
{
	int nDif    =   0; /* column value are the same */
	int iComp1  =   iCol1[0];
	int iComp2  =   iCol2[0];
	int iz      =   -1;
	iz = -1;
	while(++iz < n) iDif[iz] = n;

	iz = 0;
	while(++iz < n) {
		if( (iCol1[iz]      != iComp1)   ||
			(iCol2[iz]      != iComp2))  {  /* different value */
				iDif[nDif++]    =   iz;
				iComp1          =   iCol1[iz];
				iComp2          =   iCol2[iz];
		}                               /* different value */
	}
	return(++nDif);
}

/*  extract integer value from string csv-format        */
/*  features:                                            */
/*  - ignores whitspaces                                 */
/*  - ignores leading zeros                              */
/*  - ;; is interprated as ;0;                           */

/// @����˵�� ��CSV��ʽ��FILE��������������
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::CSVToInt(const   char    *sx, const   int     nMax, int     *iVal)  
{
	int     nVal = 0;
	const   char    *pStr = &*sx;
	const   int     nStrLength = strlen(sx);
	int     nSubLength;
	char    *sSub;
	int     iz = -1;
	int     iCountSub = 0;
	int     iEnde = 0;
	while(++iz < nMax) iVal[iz] = 0;

	if((sSub = (char *) calloc(nStrLength, sizeof(char))) == NULL)
		return(0);
	if( nStrLength == 0)    return(0);

	do  {
		if( (*pStr == ';' ) || (*pStr == ',' ) || (*pStr == '\0') || (*pStr == '\t'))
		{
			sSub[iCountSub++] = '\0';
			nSubLength      = strlen(sSub);
			if(nSubLength == 0) {
				/* ignore last delimiter */
				if(*pStr == '\0')   break;
				else                iVal[nVal++] = 0;
			}
			else
				iVal[nVal++]    = atoi(sSub);
			iCountSub = 0;
		}
		else    sSub[iCountSub++] = *pStr;
		if(*pStr == '\0') iEnde = 1; 
		++pStr;
	} while(iEnde == 0);
	free(sSub);
	return(nVal);
}


//  select preferred inactive cooling units caused by side sprays      
/// @����˵�� ѡ����ѡ��Ч���������������ȴװ��
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int	CPreProcess::SelectCollingUnits(
	const	int		nSideSpray,		/* No. of side sprays */
	const	int		*iStatus,		/* status of side sprays */
	const	int		*iFirstDeAct,	/* first to deactivate */
	const	int		*iSecondDeAct,	/* second to deactivate */
	int		*iDeAct)		/* indexes deactivation */
{
	int	nDeAct = 0;
	int	iz = -1;
	iz = -1; 
	while(++iz < nSideSpray)
		iDeAct[iz] = -1;
	iz = -1;
	while(++iz < nSideSpray) {
		if(iStatus[iz] == 1) {	/* side spray active */
			if(iFirstDeAct[iz] != -1)
				iDeAct[nDeAct++] = iFirstDeAct[iz] - 1;
		}						/* side spray active */
	}

	iz = -1;
	while(++iz < nSideSpray) {
		if(iStatus[iz] == 1)/* side spray active */
			if(iSecondDeAct[iz] != -1) iDeAct[nDeAct++] = iSecondDeAct[iz] - 1;
	}
	return(nDeAct);
}


/*  generate most distributed ��ȴģʽ                  */

/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int CPreProcess::GenerateMostDistCoolPattern(
	const	int		iHeaderDir,	/* header direction */
	const	int		nMax,		/* elements iSelect */
	const	int		nAct,		/* required active elements */
	const	int		nX,			/* No. preferred inactive elements */
	const	int		*iX,		/* preferred inactive elements [nX] */
	int		*iSelect)	/* selected active elements [nMax] */
{
	int	iLast		= nAct;
	int iMod;
	int nMod;
	int nShift = 0;
	int is;
	int ix;
	int iCntMod = 0;
	/* remaining activable elements */
	int iRemain[NUZZLE_NUMBER];
	int iSwapStill[NUZZLE_NUMBER];
	int	nRemain = GenerateRemainActiveUnits(nMax, nAct, nX, iX, iRemain);
	int nRemainStill = nRemain;
	int nSwap = nRemain; 
	int	iz = -1;
	/* preset to compact pattern */
	while(++iz < nAct)
		iSelect[iz] = 1;
	iz--;
	while(++iz < nMax)	
		iSelect[iz] = -1;
	if(nAct >= nMax)	
		return(nAct);
	if(nAct < 1)	
		return(nAct);
	/* reduce swap elements, if lower number is required only */
	if(nX < nRemain) 
		nSwap = nX;
	iz = -1;

	while(++iz < nSwap)	{
		iMod			= iX[iz];
		/* swap only if header already activated */
		if(iSelect[iMod]	== 1)	{ 
			iSelect[iMod]	= -1;
			iMod			= iRemain[iCntMod++];
			iSelect[iMod]	= 1;
			/* store No. of still remaining elements */
			nRemainStill--;
			/* set last activated element + 1 */ 
			iLast			= iMod + 1;
		}	/* swap only if header already activated */
	}
	if(nRemainStill < 1)	
		return(iLast);
	/* modify for distributed pattern (each second header) */
	nRemainStill = GenerateRemainAUSecondMax(nRemainStill, &iRemain[iCntMod]);
	/* ��� No. of required modifications: nMod */
	nMod = GetMaxSwapOperations(iLast, iSelect, iSwapStill);
	if(nRemainStill > nMod) 
		nRemainStill = nMod;

	/* swap additional elements if still remaining elements */
	ix = -1;
	while(++ix < nRemainStill)	{	/* remaining elements */
		is				= iSwapStill[ix];
		iMod			= iRemain[iCntMod+ix];
		iSelect[is]		= -1;
		iSelect[iMod]	= 1;
		iLast = iMod+1;
	}							/* remaining elements */


	if(iHeaderDir == -1){
		nShift = nMax - iLast;
		iz = iLast;
		while(--iz >= 0){
			if(iSelect[iz] == 1){
				iSelect[iz]			= -1;
				iSelect[iz+nShift]  =  1;
			}
		}
		iLast = nMax;
	}
	return(iLast);
}

/*  generate remaining active units                            */
int CPreProcess::GenerateRemainActiveUnits(
	const	int		nMax,		/* elements iSelect */
	const	int		nAct,		/* required active elements */
	const	int		nX,			/* No. preferred inactive elements */
	const	int		*iX,		/* preferred inactive elements [nX] */
	int		*iRemain)   /* selected active elements [nMax] */
{
	int	nRemain = 0;
	int iz = -1;
	int is;
	int iLock; /* 0: element can be used */
	while(++iz < nMax)
		iRemain[iz] = -1;
	iz = nAct -1;

	while(++iz < nMax)	{
		iLock = 0;
		is = -1;
		while(++is < nX){
			if(iX[is] == iz)	
				iLock = 1;
		}
		if(iLock == 0) 
			iRemain[nRemain++] = iz;
	}
	return(nRemain);
}

/*  generate remaining active units / each second element max  */
int CPreProcess::GenerateRemainAUSecondMax(
	const	int		nRemain,	/* elements iSelect */
	int		*iRemain)   /* selected active elements [nRemain] */
{
	int	nRemainMod = 0;
	int iz = -1;
	if(nRemain < 1)
		return(nRemainMod);
	iz = 0; 
	while(iz < nRemain)	{
		iRemain[iz++] = -1;
		if(iz >= nRemain)
			break;
		iz++;
		if(iz >= nRemain)
			break;
	}
	iz = -1;
	while(++iz < nRemain){
		if(iRemain[iz] > -1)
			iRemain[nRemainMod++] = iRemain[iz];
	}
	return(nRemainMod);
}

/*  ��� No. of required swap operations                    */
int CPreProcess::GetMaxSwapOperations(
	const	int	n,
	const	int *iDist,
	int *iSwap)
{
	int	nSwap = 0;
	int iz = -1;
	const	int	nM1 = n - 1;
	int	ixDist[NUZZLE_NUMBER];
	while(++iz < n) {
		ixDist[iz] = iDist[iz];
		iSwap[iz]  = -1;
	}
	iz = 0;
	while(++iz < nM1){
		if(	(ixDist[iz-1] > -1) &&(ixDist[iz]   > -1) && (ixDist[iz+1] > -1)) {
			iSwap[nSwap++] = iz;
			ixDist[iz] = -1;
		}
	}
	return(nSwap);
}





/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
PHYSICAL CPreProcess::InitPhysicalParam(
	const int	inumber, /* No. of  cool. zones */
	const int	imat,    /* Type of material */
	const int	iTask,	 /* task */
	FILE	*st_err)
{
	PHYSICAL ax;
	int	ixx = 0;
	int i;

	ax.sw_mat = imat;
	ax.adaptRatioFound = 0.0f;	/* initial ����Ӧvalue*/
	for (i=0;i<5;i++)
		ax.adaptKey[i] = 0l;		/* initial adaption key */

	ax.taskID   = iTask;
	ax.waterFactorCoe=0.0f;

	return(ax);
}



/* read cooling ���Բ��� */
/// @����˵�� ����ȴ���Բ��� 
/// @���� st_err
/// @���� pdiInPut
/// @���� lay
/// @���� lay
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
/// int	CPreProcess::inp_acc_strategy(const	INPUTDATAPDI *pdiInPut,
int	CPreProcess::ReadAccStrategyParam(const	INPUTDATAPDI *pdiInPut,
									RALCOEFFDATA *lay,
									STRATEGY *accStrat1)
{
	int		nRead = 0;
	int		iFile;
	int		iz = -1;
	int		iZeile;
	const	int		nRowMax = PATH_MAX - 1;
	const	char	*sFormat = "%f%f%f%f%f%f%f%f%f%f%f%f";
	const	int		nMaxStroke = ANZ_HUB;
	float	fThick[NMAXROW];
	float	fSpdPref[NMAXROW];
	float	fSpdMin[NMAXROW];
	float	fSpdMax[NMAXROW];
	float	fFlwLimitMin[NMAXROW];
	float	fFlwLimitMax[NMAXROW];
	float	fFlwLimitTmp[NMAXROW];
	float   fTempDropWater[NMAXROW];

	int		iTransfer = 1; /* pass through mode */

	fThick[0] = 0.0f;

	iFile=1;
	accStrat1->iHeaderDir = iFile;

	accStrat1->iZoneAct[0]=1;
	accStrat1->iZoneAct[1]=1;
	accStrat1->iZoneAct[2]=1;

	iZeile=0;
	if(	(iZeile < 0)	|| (iZeile > nMaxStroke))	
		iZeile = nMaxStroke;
	accStrat1->nStroke = iZeile;
	accStrat1->iTransferMode[0]=3;
	accStrat1->iTransferMode[1]=0;
	accStrat1->iTransferMode[2]=2;

	/* set pass through to highest priority */
	if(	(accStrat1->iTransferMode[0] < 1) ||
		(accStrat1->iTransferMode[0] > 3)) accStrat1->iTransferMode[0] = 3;
	/* exclude inner oscillation mode */
	if(	(accStrat1->iTransferMode[1] < 0) ||
		(accStrat1->iTransferMode[1] > 3)) accStrat1->iTransferMode[1] = 0;
	/* default: no oszillation allowed */
	if(	(accStrat1->iTransferMode[2] < 0) ||
		(accStrat1->iTransferMode[2] > 3)) accStrat1->iTransferMode[2] = 0;

	iZeile=1; /* 1: odd No. strokes 2: even No. strokes */
	if((iZeile < 1) || (iZeile > 2)) 
		iZeile = 2;
	accStrat1->iDirStroke = iZeile;

	iZeile=0; /* 1: modification No. strokes in online */
	if((iZeile < 0) || (iZeile > 1)) 
		iZeile = 0;
	accStrat1->iModStroke = iZeile;

	accStrat1->fLengthBreakStr=1.2f;  //��ʮ��
	if(accStrat1->fLengthBreakStr < 0.0f) 
		accStrat1->fLengthBreakStr = 0.0f;
	if(accStrat1->fLengthBreakStr > 20.f) 
		accStrat1->fLengthBreakStr = 0.0f;

	iZeile=1;
	if((iZeile != 1) && (iZeile != -1))	
		iZeile = 1;

	accStrat1->iEdgeMask = iZeile;

	iZeile=1;
	if((iZeile < 1) || (iZeile > lay->nHeader))	
		iZeile = 1;

	accStrat1->iHeaderFirst = iZeile;

	/* negative value are indicating use of distributed pattern from top only */
	//��13��
	iZeile=0;
	if((iZeile < -lay->nHeader) || (iZeile > lay->nHeader))	
		iZeile = 0;

	accStrat1->nHeaderAct = iZeile;

	/* negative value are indicating use of distributed pattern from top only */
	//��14��
	iZeile=0;
	if((iZeile < -lay->nHeader) || (iZeile > lay->nHeader))	
		iZeile = 0;

	accStrat1->nHeaderActNon = iZeile;

	//��15��
	iZeile=100;
	if((iZeile < 1) || (iZeile > lay->nHeader))	
		iZeile = lay->nHeader;

	accStrat1->nHeaderActPref = iZeile;

	//��16��
	iZeile=100;
	if((iZeile < 1) || (iZeile > lay->nHeader))	
		iZeile = lay->nHeader;

	accStrat1->nHeaderActMax = iZeile;

	//��17��
	iZeile=1;
	if((iZeile < 1) || (iZeile > 3))	
		iZeile = 1;

	accStrat1->iFlwReg = iZeile;

	//��18��
	iZeile=1;
	if((iZeile < 0) || (iZeile > 3))	
		iZeile = 1;

	accStrat1->iFlwDist = iZeile;

	//��19��
	accStrat1->fPosHeaderRef=0.0f;
	accStrat1->fPosHeaderRef *= 0.01f;
	if(accStrat1->fPosHeaderRef < 0.01f)		
		accStrat1->fPosHeaderRef = 0.5f;
	if(accStrat1->fPosHeaderRef > 0.99f)		
		accStrat1->fPosHeaderRef = 0.5f;

	//��20��
	accStrat1->fFlwHeaderRef=0.0f;
	accStrat1->fFlwHeaderRef *= 0.01f;
	if(accStrat1->fFlwHeaderRef < 0.0f)		
		accStrat1->fFlwHeaderRef = 1.0f;
	if(accStrat1->fFlwHeaderRef > 1.0f)		
		accStrat1->fFlwHeaderRef = 1.0f;

	//��21��
	accStrat1->fFlwHeaderLast=0.0f;
	accStrat1->fFlwHeaderLast *= 0.01f;
	if(accStrat1->fFlwHeaderLast < 0.0f)		
		accStrat1->fFlwHeaderLast = 1.0f;
	if(accStrat1->fFlwHeaderLast > 1.0f)		
		accStrat1->fFlwHeaderLast = 1.0f;

	iZeile=2;
	if((iZeile < 1) || (iZeile > 3))	
		iZeile = 2;

	accStrat1->iTempStopPos = iZeile;

	accStrat1->fTempStopDef=-1.f;
	/* reset to Ar3 (-1.f) if not plausible */
	if(accStrat1->fTempStopDef < 0.0f)		
		accStrat1->fFlwHeaderRef = -1.f;

	//��24��
	accStrat1->fCrDef=2.0f;
	/* reset to Ar3 (-1.f) if not plausible */
	if(accStrat1->fCrDef < 0.0f)		
		accStrat1->fCrDef = 0.0f;


	//��25��
	accStrat1->fCorSpd=1.0f;
	/* ����ion value �ٶ�  */
	if(	(accStrat1->fCorSpd < SPD_COR_MIN)	||
		(accStrat1->fCorSpd > SPD_COR_MAX))
		accStrat1->fCorSpd = 1.0f;

	//��26��
	accStrat1->fCorNuHeader=1.0f;
	/* ����ion value Number of headers */
	if(	(accStrat1->fCorNuHeader < HEADER_COR_MIN)	||
		(accStrat1->fCorNuHeader > HEADER_COR_MAX))
		accStrat1->fCorNuHeader = 1.0f;

	//��27��
	accStrat1->fCorNuStrokes=1.0f;
	/* ����ion value Number of headers */
	if(	(accStrat1->fCorNuStrokes < STROKE_COR_MIN)	||
		(accStrat1->fCorNuStrokes > STROKE_COR_MAX))
		accStrat1->fCorNuStrokes = 1.0f;

	iZeile = 0;

	fThick[0]= 0.0f;fSpdPref[0] = 1.8f;fSpdMin[0] = 1.5f;fSpdMax[0] = 2.0f;fTempDropWater[0] = 1.0f;
	fFlwLimitMin[0]	= FLOWTMIN;fFlwLimitMax[0]	= FLOWT;fFlwLimitTmp[0]	= 0.0f;
	fThick[1]= 8.0f;fSpdPref[1] = 1.8f;fSpdMin[1] = 1.3f;fSpdMax[1] = 2.0f;fTempDropWater[1] = 1.0f;
	fFlwLimitMin[1]	= FLOWTMIN;fFlwLimitMax[1]	= FLOWT;fFlwLimitTmp[1]	= 0.0f;
	fThick[2]= 12.0f;fSpdPref[2] = 1.5f;fSpdMin[2] = 1.3f;fSpdMax[2] = 2.0f;fTempDropWater[2] = 1.0f;
	fFlwLimitMin[2]	= FLOWTMIN;fFlwLimitMax[2]	= FLOWT;fFlwLimitTmp[2]	= 0.0f;
	fThick[3]= 25.0f;fSpdPref[3] = 1.5f;fSpdMin[3] = 0.6f;fSpdMax[3] = 2.0f;fTempDropWater[3] = 1.0f;
	fFlwLimitMin[3]	= FLOWTMIN;fFlwLimitMax[3]	= FLOWT;fFlwLimitTmp[3]	= 0.0f;
	fThick[4]= 32.0f;fSpdPref[4] = 1.2f;fSpdMin[4] = 0.6f;fSpdMax[4] = 2.0f;fTempDropWater[4] = 1.0f;
	fFlwLimitMin[4]	= FLOWTMIN;fFlwLimitMax[4]	= FLOWT;fFlwLimitTmp[4]	= 0.0f;
	fThick[5]= 39.0f;fSpdPref[5] = 1.1f;fSpdMin[5] = 0.5f;fSpdMax[5] = 1.7f;fTempDropWater[5] = 1.0f;
	fFlwLimitMin[5]	= FLOWTMIN;fFlwLimitMax[5]	= FLOWT;fFlwLimitTmp[5]	= 0.0f;
	fThick[6]= 44.0f;fSpdPref[6] = 1.1f;fSpdMin[6] = 0.5f;fSpdMax[6] = 1.5f;fTempDropWater[6] = 1.0f;
	fFlwLimitMin[6]	= FLOWTMIN;fFlwLimitMax[6]	= FLOWT;fFlwLimitTmp[6]	= 0.0f;
	fThick[7]= 54.0f;fSpdPref[7] = 1.0f;fSpdMin[7] = 0.5f;fSpdMax[7] = 1.5f;fTempDropWater[7] = 1.0f;
	fFlwLimitMin[7]	= FLOWTMIN;fFlwLimitMax[7]	= FLOWT;fFlwLimitTmp[7]	= 0.0f;
	fThick[8]= 64.0f;fSpdPref[8] = 0.9f;fSpdMin[8] = 0.5f;fSpdMax[8] = 1.5f;fTempDropWater[8] = 1.0f;
	fFlwLimitMin[8]	= FLOWTMIN;fFlwLimitMax[8]	= FLOWT;fFlwLimitTmp[8]	= 0.0f;
	fThick[9]= 84.0f;fSpdPref[9] = 0.9f;fSpdMin[9] = 0.5f;fSpdMax[9] = 1.5f;fTempDropWater[9] = 1.0f;
	fFlwLimitMin[9]	= FLOWTMIN;fFlwLimitMax[9]	= FLOWT;fFlwLimitTmp[9]	= 0.0f;
	fThick[10]= 200.0f;fSpdPref[10] = 0.8f;fSpdMin[10] = 0.5f;fSpdMax[10] = 1.5f;fTempDropWater[10] = 1.0f;
	fFlwLimitMin[10]	= FLOWTMIN;fFlwLimitMax[10]	= FLOWT;fFlwLimitTmp[10]	= 0.0f;

	iZeile=10;

	accStrat1->nThick = iZeile;
	iz = -1;
	while(++iz < accStrat1->nThick)	
	{
		accStrat1->fThick[iz]          = fThick[iz] * 0.001f;
		accStrat1->fSpdPref[iz]        = fSpdPref[iz];
		accStrat1->fSpdMin[iz]         = fSpdMin[iz];
		accStrat1->fSpdMax[iz]         = fSpdMax[iz];
		accStrat1->fFlwMin[iz]		   = fFlwLimitMin[iz];
		accStrat1->fFlwMax[iz]		   = fFlwLimitMax[iz];
		accStrat1->fFlwTmp[iz]		   = fFlwLimitTmp[iz];
		/*reset to 100% if not plaausible */
		if(fTempDropWater[iz] <0.0f || fTempDropWater[iz] >1.0)
			fTempDropWater[iz] = 1.0;
		accStrat1->fTempDropWater[iz]  = fTempDropWater[iz];
	}

	iz--;
	while(++iz < NTHK)	{
		accStrat1->fThick[iz]         = 0.0f;
		accStrat1->fSpdPref[iz]       = 0.0f;
		accStrat1->fSpdMin[iz]        = 0.0f;
		accStrat1->fSpdMax[iz]        = 0.0f;
		accStrat1->fTempDropWater[iz] = 0.0f;
		accStrat1->fFlwMin[iz]		   = 0.0f;
		accStrat1->fFlwMax[iz]		   = 0.0f;
		accStrat1->fFlwTmp[iz]		   = 0.0f;
	}

	////////////////////////////////  ע�⣺��������ȴ��Ӧ�ò���ڶ�ʽ��ȴģʽ����������ĳ���û������ /////////////////////////////
	if(	(pdiInPut->thick > THICK_OSC_MIN) &&
		((pdiInPut->finishRollTemp - pdiInPut->targetFinishCoolTemp) > TEMP_OSC_MIN) &&
		(pdiInPut->acc_mode != IC) &&
		(accStrat1->nStroke > 0))
	{
		accStrat1->nHeaderAct		= 1;
		accStrat1->nHeaderActNon	= 1;
		accStrat1->iHeaderFirst		= HEAD_OSC_FIRST;
		accStrat1->iDirStroke		= 1;
		accStrat1->nStroke			= ANZ_HUB;
		accStrat1->nHeaderActPref	= 0;
		iz = -1;
		while(++iz < lay->nHeader) lay->iStatusHeader[iz] = 0;
		iz = accStrat1->iHeaderFirst - 2;
		while(iz < lay->nHeader){
			iz += 2;
			if(iz > (lay->nHeader - 1)) break;
			lay->iStatusHeader[iz] = 1;
			accStrat1->nHeaderActPref += 1;
		}
		/* �� priority to pass through mode */
		accStrat1->iTransferMode[0]	= 0;
		/* high priority to outer oscillation mode */
		accStrat1->iTransferMode[2] = 3;
	}

	return TRUE;
}


int CPreProcess::InputInfomations(	
	FILE		*erg_f, 
	FILE		*st_err, 
	PHYSICAL *ver_par1, 
	PRERESULT *PreResult)
{
	/* input of structure PHYSICAL / water Temperature + air Temperature   */
	/* other significant Temperature value of production line */
	float   temp_H2O	= 24.f;	/* Ĭ��value */	
	float	temp_AIR	= 25.f;	/* Ĭ��value */
	float   temp_Press = 5.0;

	ver_par1->waterTemp = temp_H2O; /* water Temperature */
	ver_par1->t_umg      = temp_AIR;
	ver_par1->waterPressure = temp_Press;

	/* prefer water Temperature from input telegram */
	if((PreResult->tempWater > TEMP_WAT_MIN)&&(PreResult->tempWater < TEMP_WAT_MAX))
		ver_par1->waterTemp = PreResult->tempWater;
	else 
		fprintf(erg_f,"\n\t ��������׼ˮ��: %5.2f  !!",  temp_H2O); //use of standard value water Temperature

	/* prefer water press from input telegram */
	if((PreResult->WatPress > PRESS_WAT_MIN)&&(PreResult->WatPress < PRESS_WAT_MAX))
		ver_par1->waterPressure = PreResult->WatPress;
	else 
		fprintf(erg_f,"\n\t �������׼ѹ��: %5.2f !! \n", temp_Press);//use of standard value   WATER PRESS: %5.2f [��]

	/* prefer air Temperature from input telegram */
	if((PreResult->tempAir > TEMP_AIR_MIN) &&(PreResult->tempAir < TEMP_AIR_MAX))
		ver_par1->t_umg = PreResult->tempAir;
	else 
		fprintf(erg_f,"\n\t ��׼����Temperature: %5.2f  !! \n", temp_AIR); 

	if((ver_par1->alpha_coe<0.499) ||(ver_par1->alpha_coe>1.501))
		ver_par1->alpha_coe = 1.0f;      //���뱣������  2012-7-12

	ver_par1->furnaceTemp  = FURNACE_TEMP;
	ver_par1->maxPossibleMillTemp     = MAXTEMP;
	ver_par1->speed      = PLATE_SPEED;     /* �ٶ� of the ���   */

	fprintf(erg_f,"\n\t ˮ��		 = %g C", ver_par1->waterTemp);//water Temperature
	fprintf(erg_f,"\n\t ����Temperature	 = %g C", ver_par1->t_umg);//air   Temperature
	fprintf(erg_f,"\n\t ������ˮѹ	 = %g C \n", ver_par1->waterPressure);//ufc water press

	return TRUE;
}

/// @����˵�� calculate flow reduction for ͷ�� and β�� �ڱ�  ????????????????????????
/// @���� st_err ������־����ָ��
/// @���� pdiInPut PDI����
/// @���� flow value ����value
/// @���� PreResult  PREPROCESS telegram
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
//int CPreProcess::length_mask(FILE *st_err, const INPUTDATAPDI *pdiInPut, flow_cr *cr_flow, PRERESULT *PreResult)

// ���ٶ��˺��� �˽⺯������ [8/10/2013 лǫ]
int CPreProcess::CalculMaskFlowReduction(FILE *st_err, const INPUTDATAPDI *pdiInPut, flow_cr *cr_flow, PRERESULT *PreResult)
{
	int	nResult = FALSE;
	int	ix = LENGTH_MASK;
	int iLengthMaskPatHead; /* 1: pattern 2: improved default */
	int iLengthMaskPatTail; /* 1: pattern 2: improved default */

	char	flow_datxx[PATH_MAX];
	char	flow_datyy[PATH_MAX];

	memset(flow_datxx,0,PATH_MAX);
	memset(flow_datyy,0,PATH_MAX);

	switch(pdiInPut->acc_mode){
	case UFC:
		CombinFilePath("FlowHeadMask_ACC.txt", PREPROCESS,flow_datxx);
		CombinFilePath("FlowTailMask_ACC.txt", PREPROCESS,flow_datyy);
		break;
	case IC:
		CombinFilePath("FlowHeadMask_IC.txt", PREPROCESS,flow_datxx);
		CombinFilePath("FlowTailMask_IC.txt", PREPROCESS,flow_datyy);
		break;
	case ACC:
		CombinFilePath("FlowHeadMask_ACC.txt", PREPROCESS,flow_datxx);
		CombinFilePath("FlowTailMask_ACC.txt", PREPROCESS,flow_datyy);
		break;
	case SC:
		CombinFilePath("FlowHeadMask_SC.txt", PREPROCESS,flow_datxx);
		CombinFilePath("FlowTailMask_SC.txt", PREPROCESS,flow_datyy);
		break;
	case DQ:
		CombinFilePath("FlowHeadMask_DQ.txt", PREPROCESS,flow_datxx);
		CombinFilePath("FlowTailMask_DQ.txt", PREPROCESS,flow_datyy);
		break;
	default:
		CombinFilePath("FlowHeadMask_ACC.txt", PREPROCESS,flow_datxx);
		CombinFilePath("FlowTailMask_ACC.txt", PREPROCESS,flow_datyy);
	}

	/* 1) use value from pattern if non default */
	iLengthMaskPatHead = CheckLengthMask(1, pdiInPut, PreResult, cr_flow);
	iLengthMaskPatTail = CheckLengthMask(2, pdiInPut, PreResult, cr_flow);

	/* value ͷ�� �ڱ� (pattern or preset model) */
	ix = LENGTH_MASK;
	/* length ͷ�� mask top [mm] */
	PreResult->LengthMaskHeadTop = cr_flow->LengthMaskHeadTop;

	/* length ͷ�� mask btm [mm] */
	PreResult->LengthMaskHeadBtm = cr_flow->LengthMaskHeadBtm;


	/* get value in [%] */
	PreResult->flowMaskHeadTop = cr_flow->flowMaskHeadTop;
	PreResult->flowMaskHeadBtm = cr_flow->flowMaskHeadBtm;
	PreResult->nuzzleMaskHeadTop = cr_flow->nuzzleMaskHeadTop;
	PreResult->nuzzleMaskHeadBtm = cr_flow->nuzzleMaskHeadBtm;

	/* value β�� �ڱ� */
	/* length ͷ�� mask top [mm] */
	PreResult->LengthMaskTailTop = cr_flow->LengthMaskTailTop;

	/* length ͷ�� mask btm [mm] */
	PreResult->LengthMaskTailBtm = cr_flow->LengthMaskTailBtm;

	/* get value in [%] */
	PreResult->flowMaskTailTop = cr_flow->flowMaskTailTop;
	PreResult->fFlwMaskTailBtm = cr_flow->fFlwMaskTailBtm;
	PreResult->nuzzleMaskTailTop = cr_flow->nuzzleMaskTailTop;
	PreResult->nuzzleMaskTailBtm = cr_flow->nuzzleMaskTailBtm;

	/* 2) try to achieve value from FILE (+plausibility) */
	if(iLengthMaskPatHead == 2)
	{	/* use data from FILE */
		if(ReadHeadMaskingValues(pdiInPut, cr_flow, flow_datxx) == TRUE){
			ix = LENGTH_MASK;
			PreResult->LengthMaskHeadTop = cr_flow->LengthMaskHeadTop;
			/* length ͷ�� mask btm [mm] */
			PreResult->LengthMaskHeadBtm = cr_flow->LengthMaskHeadBtm;
			/* get value in [%] */
			//there is may be some error by wbxang 20120513
			if(cr_flow->flowMaskHeadTop * cr_flow->flow_t < FLOWTMIN)
				cr_flow->flowMaskHeadTop = FLOWTMIN / cr_flow->flow_t;
			/* get value in [%] */
			PreResult->flowMaskHeadTop = cr_flow->flowMaskHeadTop;
			PreResult->flowMaskHeadBtm = cr_flow->flowMaskHeadBtm;
			PreResult->nuzzleMaskHeadTop = cr_flow->nuzzleMaskHeadTop;
			PreResult->nuzzleMaskHeadBtm = cr_flow->nuzzleMaskHeadBtm;
			nResult			=	1;
		}else	{	/* use default value for ͷ�� and β�� �ڱ� */
			fprintf(st_err,"\n\t use default value for ͷ�� / β�� Masking");
			fprintf(st_err," ���: %s", pdiInPut->plateID);
			nResult			=	2;
		}
	}	/* use data from FILE */
	if(iLengthMaskPatTail == 2){
		if(ReadTailMaskingValues(pdiInPut, cr_flow, flow_datyy) == TRUE){
			/* length ͷ�� mask top [mm] */
			PreResult->LengthMaskTailTop = cr_flow->LengthMaskTailTop;

			/* length ͷ�� mask btm [mm] */
			PreResult->LengthMaskTailBtm = cr_flow->LengthMaskTailBtm;

			/* get value in [%] */
			//there is may be some error by wbxang 20120513
			if(cr_flow->flowMaskTailTop * cr_flow->flow_t < FLOWTMIN)
				cr_flow->flowMaskTailTop = FLOWTMIN / cr_flow->flow_t;
			PreResult->flowMaskTailTop = cr_flow->flowMaskTailTop;
			PreResult->fFlwMaskTailBtm = cr_flow->fFlwMaskTailBtm;
			PreResult->nuzzleMaskTailTop = cr_flow->nuzzleMaskTailTop;
			PreResult->nuzzleMaskTailBtm = cr_flow->nuzzleMaskTailBtm;
			nResult		   +=	10;
		}
		else	{	/* use default value for ͷ�� and β�� �ڱ� */
			fprintf(st_err,"\n\t use default value for ͷ�� / β�� Masking");
			fprintf(st_err," ���: %s", pdiInPut->plateID);
			nResult		   +=	20;
		}
	}

	return(nResult);
}

/*  read value for ͷ�� �ڱ� from FILE             */
int	CPreProcess::ReadHeadMaskingValues(
	const	INPUTDATAPDI *pdiInPut,	/*PDI����	*/
	flow_cr *cr_flow,	/* flow value			*/
	const	char	*sFileName)	/* �ļ��� */
{
	int		nLine	=	PATH_MAX - 1;
	char	sLine[PATH_MAX];
	char	sx[PATH_MAX];
	char	*sFormat1 = "%s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f";
	char	*sFormat2 = "%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f";
	int		nRead1; /* No. of ridden value flows */
	int		nRead2;	/* No. or ridden value ͷ�� and β�� �ڱ� */
	float	fFlw[48];
	float	fVal[48];
	float	fValInt[48]; /* ��value value */
	float	fValM1[48];
	float	fFlwM1 = 0.0f;
	float	fFlwAct	= cr_flow->flow_t;
	int		iz = 0;
	int		ix = 0;
	int		nFlw;	/* No. of different flows */
	int		nFlwM1;
	int		nVal = 0;
	int		nValFlw = 6; /* ���value stored for one individual flow */
	float	fThickRow;	/* thickness of row [mm] */
	float	fDiff;
	float	fThickM1 = 0.0f;	/* thickness predecessor line */
	float	fThickAct = pdiInPut->thick * 1000.f;
	float	r = 0.0f;	/* interpolation value */	
	const	float	fEpsThick	= 0.01f;
	const	float	fEpsFlw		= 10.f;

	FILE	*fx = NULL;
	if(fFlwAct < FLOWTMIN)	return FALSE;
	if((fx = fopen(sFileName,"r")) == NULL)	
	{
		printf("\n\t not able to open FILE ͷ�� and β�� �ڱ�: %s",sFileName);
		return FALSE;
	}
	fgets(sLine, nLine, fx);
	nRead1 = sscanf(sLine, sFormat1, sx,
		&fVal[0], &fVal[1],  &fVal[2],  &fVal[3], 
		&fVal[4], &fVal[5],  &fVal[6],  &fVal[7],
		&fVal[8], &fVal[9],  &fVal[10], &fVal[11],
		&fVal[12],&fVal[13], &fVal[14], &fVal[15],
		&fVal[16],&fVal[17], &fVal[18], &fVal[19],
		&fVal[20],&fVal[21], &fVal[22], &fVal[23]);
	nVal = nRead1 - 1; 
	while(iz < nVal)	{
		fFlw[ix] = fVal[iz];
		iz += nValFlw;
		ix +=1;
	}
	iz = -1;
	nFlw = ix;
	while(++iz < nFlw)	{ /* plausibility flows */
		if(fFlw[iz] < FLOWTMIN)           { fclose(fx); return FALSE; }
		if(fFlw[iz] > 2.0f * FLOWT)       { fclose(fx); return FALSE; }
		if(fFlw[iz] < (fFlwM1 + fEpsFlw)) { fclose(fx); return FALSE; }
		fFlwM1 = fFlw[iz];
	}
	iz = -1;
	while(++iz < nVal)	{
		fVal[iz] = 0.0f;
		fValM1[iz] = 0.0f;
	}
	fgets(sLine, nLine, fx);
	fgets(sLine, nLine, fx);
	while(fgets(sLine, nLine, fx) != NULL)	{
		nRead2 = sscanf(sLine, sFormat2, &fThickRow,
			&fVal[0], &fVal[1],  &fVal[2],  &fVal[3], 
			&fVal[4], &fVal[5],  &fVal[6],  &fVal[7],
			&fVal[8], &fVal[9],  &fVal[10], &fVal[11],
			&fVal[12],&fVal[13], &fVal[14], &fVal[15],
			&fVal[16],&fVal[17], &fVal[18], &fVal[19],
			&fVal[20],&fVal[21], &fVal[22], &fVal[23]);
		if(fThickRow > fThickAct)	break;
		if(fThickRow < (fThickM1 + fEpsThick))	{ fclose(fx); return FALSE; }
		fThickM1 = fThickRow;
		if(fThickRow < 0.01)	{ fclose(fx); return FALSE; }
		if(fThickRow > 1000.)	{ fclose(fx); return FALSE; }
		if(nRead1 != nRead2)	{ fclose(fx); return FALSE; }
		iz = -1;
		while(++iz < nVal)	fValM1[iz] = fVal[iz];
	}
	/* Interpolation vs. Thickness */
	fDiff = fThickRow - fThickM1;
	if(fabs(fDiff) < fEpsThick)		r	= 1.0f;
	else							r	= (fThickAct - fThickM1) / fDiff;
	iz = -1;	
	while(++iz < nVal)	{
		fValInt[iz] = (1.f - r) * fValM1[iz] + r * fVal[iz];
	}
	/* Interpolation vs. Flow */
	iz = -1;
	while(++iz < nFlw)	{
		if(fFlw[iz] > fFlwAct)	break;
		fFlwM1 = fFlw[iz];
	}
	fDiff	= fFlw[iz] - fFlwM1; 
	if(fabs(fDiff) < fEpsFlw)		r		= 1.0f;
	else							r		= (fFlwAct - fFlwM1) / fDiff;
	if(iz == 0)	{	/* �� flow limit */
		cr_flow->flowMaskHeadTop	= fValInt[0];
		cr_flow->flowMaskHeadBtm	= fValInt[1];
		cr_flow->LengthMaskHeadTop = fValInt[2];
		cr_flow->LengthMaskHeadBtm = fValInt[3];
		cr_flow->nuzzleMaskHeadTop		= FloatToInt(fValInt[4]);
		cr_flow->nuzzleMaskHeadBtm		= FloatToInt(fValInt[5]);
		fclose(fx);
		return TRUE;
	}			/* �� flow limit */
	nFlwM1 = nFlw - 1;
	if(fFlwAct > fFlw[nFlwM1])	{	/* high flow limit */
		ix = nValFlw * nFlwM1;
		cr_flow->flowMaskHeadTop	= fVal[ix];
		cr_flow->flowMaskHeadBtm	= fVal[ix+1];
		cr_flow->LengthMaskHeadTop = fVal[ix+2];
		cr_flow->LengthMaskHeadBtm = fVal[ix+3];
		cr_flow->nuzzleMaskHeadTop		= FloatToInt(fValInt[ix+4]);
		cr_flow->nuzzleMaskHeadBtm		= FloatToInt(fValInt[ix+5]);
		fclose(fx);
		return TRUE;
	}							/* high flow limit */
	/* ��value value */
	ix = nValFlw * (iz - 1);
	cr_flow->flowMaskHeadTop	= (1.f - r) * fValInt[ix] + r * fValInt[ix+nValFlw];
	ix++;
	cr_flow->flowMaskHeadBtm	= (1.f - r) * fValInt[ix] + r * fValInt[ix+nValFlw];
	ix++;
	cr_flow->LengthMaskHeadTop = (1.f - r) * fValInt[ix] + r * fValInt[ix+nValFlw];
	ix++;
	cr_flow->LengthMaskHeadBtm = (1.f - r) * fValInt[ix] + r * fValInt[ix+nValFlw];
	ix++;
	cr_flow->nuzzleMaskHeadTop = FloatToInt(fValInt[ix]);
	ix++;
	cr_flow->nuzzleMaskHeadBtm = FloatToInt(fValInt[ix]);
	fclose(fx);
	return TRUE;
}


/// @����˵�� ����ת���� ����0.5 +1 С��0.5 - 1
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/28/2013
int	CPreProcess::FloatToInt(	const	float	fVal)
{
	const	float	fx		= 0.49999999f;
	int		iVal	= (int) fVal;
	float	fDiff	= fVal - iVal;
	if(fVal > 0.0f)	{
		if(fDiff < fx)		return(iVal);
		return(iVal + 1);
	}
	if(fDiff > (-fx))	return(iVal);
	return(iVal - 1);
}

/*  read value for β�� �ڱ� from FILE             */
int	CPreProcess::ReadTailMaskingValues(
	const	INPUTDATAPDI *pdiInPut,	/*PDI����	*/
	flow_cr *cr_flow,	/* flow value			*/
	const	char	*sFileName)	/* �ļ��� */
{
	int		nLine	=	PATH_MAX - 1;
	char	sLine[PATH_MAX];
	char	sx[PATH_MAX];
	char	*sFormat1 = "%s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f";
	char	*sFormat2 = "%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f";
	int		nRead1; /* No. of ridden value flows */
	int		nRead2;	/* No. or ridden value ͷ�� and β�� �ڱ� */
	float	fFlw[48];
	float	fVal[48];
	float	fValInt[48]; /* ��value value */
	float	fValM1[48];
	float	fFlwM1 = 0.0f;
	float	fFlwAct	= cr_flow->flow_t;
	int		iz = 0;
	int		ix = 0;
	int		nFlw;	/* No. of different flows */
	int		nFlwM1;
	int		nVal = 0;
	int		nValFlw = 6; /* ���value stored for one individual flow */
	float	fThickRow;	/* thickness of row [mm] */
	float	fDiff;
	float	fThickM1 = 0.0f;	/* thickness predecessor line */
	float	fThickAct = pdiInPut->thick * 1000.f;
	float	r = 0.0f;	/* interpolation value */	
	const	float	fEpsThick	= 0.01f;
	const	float	fEpsFlw		= 10.f;

	FILE	*fx = NULL;
	if(fFlwAct < FLOWTMIN)	return FALSE;
	if((fx = fopen(sFileName,"r")) == NULL)	{
		printf("\n\t not able to open FILE ͷ�� and β�� �ڱ�: %s",
			sFileName);
		return FALSE;
	}
	fgets(sLine, nLine, fx);
	nRead1 = sscanf(sLine, sFormat1, sx,
		&fVal[0], &fVal[1],  &fVal[2],  &fVal[3], 
		&fVal[4], &fVal[5],  &fVal[6],  &fVal[7],
		&fVal[8], &fVal[9],  &fVal[10], &fVal[11],
		&fVal[12],&fVal[13], &fVal[14], &fVal[15],
		&fVal[16],&fVal[17], &fVal[18], &fVal[19],
		&fVal[20],&fVal[21], &fVal[22], &fVal[23]);
	nVal = nRead1 - 1; 
	while(iz < nVal)	{
		fFlw[ix] = fVal[iz];
		iz += nValFlw;
		ix +=1;
	}
	iz = -1;
	nFlw = ix;
	while(++iz < nFlw)	{ /* plausibility flows */
		if(fFlw[iz] < FLOWTMIN)			  { fclose(fx); return FALSE; }
		if(fFlw[iz] > 2.0f * FLOWT)		  { fclose(fx); return FALSE; }
		if(fFlw[iz] < (fFlwM1 + fEpsFlw)) { fclose(fx); return FALSE; }
		fFlwM1 = fFlw[iz];
	}
	iz = -1;
	while(++iz < nVal)	{
		fVal[iz] = 0.0f;
		fValM1[iz] = 0.0f;
	}
	fgets(sLine, nLine, fx);
	fgets(sLine, nLine, fx);
	while(fgets(sLine, nLine, fx) != NULL)	{
		nRead2 = sscanf(sLine, sFormat2, &fThickRow,
			&fVal[0], &fVal[1],  &fVal[2],  &fVal[3], 
			&fVal[4], &fVal[5],  &fVal[6],  &fVal[7],
			&fVal[8], &fVal[9],  &fVal[10], &fVal[11],
			&fVal[12],&fVal[13], &fVal[14], &fVal[15],
			&fVal[16],&fVal[17], &fVal[18], &fVal[19],
			&fVal[20],&fVal[21], &fVal[22], &fVal[23]);
		if(fThickRow > fThickAct)	break;
		if(fThickRow < (fThickM1 + fEpsThick)) { fclose(fx); return FALSE; }
		fThickM1 = fThickRow;
		if(fThickRow < 0.01)	{ fclose(fx); return FALSE; }
		if(fThickRow > 1000.)	{ fclose(fx); return FALSE; }
		if(nRead1 != nRead2)	{ fclose(fx); return FALSE; }
		iz = -1;
		while(++iz < nVal)	fValM1[iz] = fVal[iz];
	}
	/* Interpolation vs. Thickness */
	fDiff = fThickRow - fThickM1;
	if(fabs(fDiff) < fEpsThick)		r	= 1.0f;
	else							r	= (fThickAct - fThickM1) / fDiff;
	iz = -1;	
	while(++iz < nVal)	{
		fValInt[iz] = (1.f - r) * fValM1[iz] + r * fVal[iz];
	}
	/* Interpolation vs. Flow */
	iz = -1;
	while(++iz < nFlw)	{
		if(fFlw[iz] > fFlwAct)	break;
		fFlwM1 = fFlw[iz];
	}
	fDiff	= fFlw[iz] - fFlwM1; 
	if(fabs(fDiff) < fEpsFlw)		r		= 1.0f;
	else							r		= (fFlwAct - fFlwM1) / fDiff;
	if(iz == 0)	{	/* �� flow limit */
		cr_flow->flowMaskTailTop	= fValInt[0];
		cr_flow->fFlwMaskTailBtm	= fValInt[1];
		cr_flow->LengthMaskTailTop = fValInt[2];
		cr_flow->LengthMaskTailBtm = fValInt[3];
		cr_flow->nuzzleMaskTailTop		= FloatToInt(fValInt[4]);
		cr_flow->nuzzleMaskTailBtm		= FloatToInt(fValInt[5]);
		fclose(fx);
		return TRUE;
	}			/* �� flow limit */
	nFlwM1 = nFlw - 1;
	if(fFlwAct > fFlw[nFlwM1])	{	/* high flow limit */
		ix = nValFlw * nFlwM1;
		cr_flow->flowMaskTailTop	= fVal[ix];
		cr_flow->fFlwMaskTailBtm	= fVal[ix+1];
		cr_flow->LengthMaskTailTop = fVal[ix+2];
		cr_flow->LengthMaskTailBtm = fVal[ix+3];
		cr_flow->nuzzleMaskTailTop		= FloatToInt(fValInt[ix+4]);
		cr_flow->nuzzleMaskTailBtm		= FloatToInt(fValInt[ix+5]);
		fclose(fx);
		return TRUE;
	}							/* high flow limit */
	/* ��value value */
	ix = nValFlw * (iz - 1);
	cr_flow->flowMaskTailTop	= (1.f - r) * fValInt[ix] + r * fValInt[ix+nValFlw];
	ix++;
	cr_flow->fFlwMaskTailBtm	= (1.f - r) * fValInt[ix] + r * fValInt[ix+nValFlw];
	ix++;
	cr_flow->LengthMaskTailTop = (1.f - r) * fValInt[ix] + r * fValInt[ix+nValFlw];
	ix++;
	cr_flow->LengthMaskTailBtm = (1.f - r) * fValInt[ix] + r * fValInt[ix+nValFlw];
	ix++;
	cr_flow->nuzzleMaskTailTop = FloatToInt(fValInt[ix]);
	ix++;
	cr_flow->nuzzleMaskTailBtm = FloatToInt(fValInt[ix]);
	fclose(fx);
	return TRUE;
}

/* check length �ڱ� value of ��ȴģʽ      */
/* ����value:                                      */
/*   1:     use value of ��ȴģʽ              */
/*   2:     use value stored in files (default value */
/*          or data not plausible)                     */

/// @����˵�� check length �ڱ� value of ��ȴģʽ 
/// @���� iPos  1: ͷ�� 2: tail
/// @���� pdiInPut PDI
/// @���� PreResult Ԥ������
/// @���� cr_flow Ԥ������
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
int	CPreProcess::CheckLengthMask(const int iPos, const INPUTDATAPDI *pdiInPut, const PRERESULT *PreResult, flow_cr *cr_flow)
{
	int	iRet = 2;
	const	float	fEpsFlw = 0.01f;
	const	float	fEpsFlwMin = 1.f - fEpsFlw;
	const	float	fEpsFlwMax = 1.f + fEpsFlw;
	const	float	fEpsLength = 0.01f;
	const	float	fEpsLengthMin = 0.0f - fEpsLength;
	const	float	fEpsLengthMax = 0.0f + fEpsLength;
	const	float	fLengthMin = 0.0f;
	const	float	fLengthMax = 0.4f * pdiInPut->length;
	float	fFlwMin = 0.3f;
	const	float	fFlwMax = 1.0f;
	float	flowMaskHeadTop;
	const	float	fGrav = 9.806f;
	/* �ο� area */

	const	float	fAref =  PreResult->coolBankLen * PreResult->coolWidth;
	/* flow area top per �ο� area */
	const	float	fA1g = 318.557e-04f;
	/* flow area btm per �ο� area */
	const	float	fA2g = 248.107e-04f;
	/* �ٶ� bottom sprayer without ��� */
	const	float	fFlwSpdBtm = cr_flow->flow_b * fA2g / fAref;
	/* hight of spray */
	float	fFlwHightBtm;
	/* return time of water particel to ��� surface */
	float	fFlwTimeBtm;
	/* overcooled length ͷ�� end of the ��� */
	float	fLengthHead;

	int		ixx = 0;
	if(iPos == 1)	{
		/* check if data on default (from initilization or not spezified by pattern) */
		if(CheckValueBounds(cr_flow->LengthMaskHeadTop, fEpsLengthMin, fEpsLengthMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->LengthMaskHeadBtm, fEpsLengthMin, fEpsLengthMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->flowMaskHeadTop	 , fEpsFlwMin, fEpsFlwMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->flowMaskHeadBtm	 , fEpsFlwMin, fEpsFlwMax) == TRUE) ixx++;
		if(ixx == 4) return(2); /* use data from FILE */
		/* check if data within limits */
		ixx = 0;
		if(CheckValueBounds(cr_flow->LengthMaskHeadTop, fLengthMin, fLengthMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->LengthMaskHeadBtm, fLengthMin, fLengthMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->flowMaskHeadTop	 , fFlwMin, fFlwMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->flowMaskHeadBtm	 , fFlwMin, fFlwMax) == TRUE) ixx++;
		if(ixx < 4) return(2);
		/* check + set improved value if required */
		flowMaskHeadTop = cr_flow->flowMaskHeadTop;
		fFlwMin = flowMaskHeadTop * cr_flow->flow_t;
		if(fFlwMin < FLOWTMIN) {
			if(cr_flow->flow_t > 50.)
				cr_flow->flowMaskHeadTop = FLOWTMIN / cr_flow->flow_t;
		}
		fFlwHightBtm	= 0.5f * fFlwSpdBtm * fFlwSpdBtm / fGrav;
		fFlwTimeBtm		= 2.f * fFlwSpdBtm / fGrav;
		fLengthHead		= PreResult->vPred * fFlwTimeBtm;
		if(cr_flow->LengthMaskHeadTop < fLengthHead)
			cr_flow->LengthMaskHeadTop = fLengthHead;
		if(cr_flow->LengthMaskHeadBtm < fLengthHead)
			cr_flow->LengthMaskHeadBtm = fLengthHead;
		if(cr_flow->flowMaskHeadBtm > 0.99f)
			cr_flow->flowMaskHeadBtm = 0.8f;
		iRet = 1; /* use pattern data + modified if required */
	}
	/* β�� end */
	else	{
		/* check if data on default (from initilization or not spezified by pattern) */
		if(CheckValueBounds(cr_flow->LengthMaskTailTop, fEpsLengthMin, fEpsLengthMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->LengthMaskTailBtm, fEpsLengthMin, fEpsLengthMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->flowMaskTailTop	 , fEpsFlwMin, fEpsFlwMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->fFlwMaskTailBtm	 , fEpsFlwMin, fEpsFlwMax) == TRUE) ixx++;
		if(ixx == 4) return(2); /* read data from FILE */
		ixx = 0;
		iRet = 2;
		if(CheckValueBounds(cr_flow->LengthMaskTailTop, fLengthMin, fLengthMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->LengthMaskTailBtm, fLengthMin, fLengthMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->flowMaskTailTop	 , fFlwMin, fFlwMax) == TRUE) ixx++;
		if(CheckValueBounds(cr_flow->fFlwMaskTailBtm	 , fFlwMin, fFlwMax) == TRUE) ixx++;
		if(ixx == 4) iRet = 1; /* use pattern data */
	}
	return(iRet);
}

/* check if value is within bounds                     */
int	CPreProcess::CheckValueBounds(
	const	float	fValAct,	/* ʵ�� value for testing */
	const	float	fValMin,	/* ��С����value */
	const	float	fValMax)	/* ��� ����value */
{
	if(fValAct > fValMax)	return FALSE;
	if(fValAct < fValMin)	return FALSE;
	return TRUE;
}

/*  modify ���Բ��� if given from acc          */

/// @����˵�� �޸�ACC�Ĳ��Բ���
/// @���� pdiInPut
/// @���� lay
/// @���� accStrat1
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
//int	CPreProcess::modify_acc_strategy(
int	CPreProcess::ModifyAccStrategy(
	const	RALINDATA *ralInPut,
	const   RALCOEFFDATA *lay,
	STRATEGY *accStrat1)
{
	if(ralInPut->acc_mode == IC)
		if(	(ralInPut->iHeaderFirst > 0) && (ralInPut->iHeaderFirst < lay->nHeader))
			accStrat1->iHeaderFirst = ralInPut->iHeaderFirst;

	return TRUE;
}

/*  modifyPDI���� for ���Բ���     */
/// @����˵�� �޸�PDI���ݵĲ��Բ���
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
/// int	CPreProcess::modify_pdi(
	int	CPreProcess::ModifyPDIStrategy(FILE	*erg_f,	/* ���FILE */
	const	STRATEGY *accStrat,	
	INPUTDATAPDI *pdi,	/*PDI���� */
	RALINDATA *ralInPut,
	PRERESULT *PreResult)	/* preset telegram */
{
	const	float	fEpsTemp = 0.01f;
	float	fTempDropWater;
	const	float	fDiffTemp = pdi->finishRollTemp - pdi->targetFinishCoolTemp;

	fTempDropWater = GetLinearInterpolationFromAbscissa(accStrat->fThick,accStrat->fTempDropWater,accStrat->nThick,pdi->thick);

	if(	(fTempDropWater < (1.f + fEpsTemp)) &&
		(fTempDropWater > (1.f - fEpsTemp)))	return TRUE;
	if(	(fTempDropWater < TEMP_DROP_WATER_MIN) ||
		(fTempDropWater > TEMP_DROP_WATER_MAX))	return FALSE;
	//	pdi->targetFinishCoolTemp = pdi->finishRollTemp - fDiffTemp;
	pdi->targetFinishCoolTemp = pdi->finishRollTemp - fDiffTemp * fTempDropWater;
	PreResult->finishCoolTargetTemperature =  pdi->targetFinishCoolTemp;
	ralInPut->targetFinishCoolTemp = pdi->targetFinishCoolTemp;
	fprintf(erg_f, "\n\t percentage Temperature drop Water: %6.2f [%]", fTempDropWater);
	fprintf(erg_f, "\n\t modifed ֹͣ��ȴTemperature:  %6.2f [��]", pdi->targetFinishCoolTemp);
	return TRUE;
}


/// @����˵�� ���ģ�Ⲽ������
/// @���� lay ���ݽṹ��
/// @���� ffx FILEָ��
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
//int CPreProcess::outp_lay_sim(RALCOEFFDATA *lay, FILE *ffx )
int CPreProcess::OutputLayoutData(RALCOEFFDATA *lay, FILE *ffx )
{
	int     iz = -1;
	struct tm *newtime;
	time_t aclock;
	int i_NbCoolSect;
	char    *sHeader[] = {"Position","Length","FlwMinTop","FlwMaxTop","FlwMinBtm","FlwMaxBtm"};

	time( &aclock );                 /* Get time in seconds */

	newtime = localtime( &aclock );  /* Convert time to struct */
	/* tm form */

	fprintf(ffx,"\n\n\t LAYOUT data  %s \n\t ___________ \n", asctime(newtime));
	fprintf(ffx,"\n\t 1. max ACC-length 				%f m", lay->CoolerLength);

	fprintf(ffx,"\n\t 2. length one header 			%f m", lay->CoolBankLen);

	fprintf(ffx,"\n\t 3. No. of cooling sections 	%g", lay->NbCoolSect);

	i_NbCoolSect = (int) lay->NbCoolSect;
	while(++iz < i_NbCoolSect)  
		fprintf(ffx,"\n\t 3.%d  No. of headers  section %d : %d", iz,iz,lay->NbBankSect[iz]);

	fprintf(ffx,"\n\t  5. Width of ACC %f m", lay->CoolWidth);
	fprintf(ffx,"\n\t  6. total No. Nozz.  / header: %d", lay->NbNozzBank);
	fprintf(ffx,"\n\t  7. No. Nozz. Center / header: %g", lay->NbNozzCentr);
	fprintf(ffx,"\n\t  8. Dist Pyrometer 1 <-> Mill: %f m", lay->PosP1);
	fprintf(ffx,"\n\t  8. Dist Pyrometer 2 <-> Mill: %f m", lay->PosP2U);
	fprintf(ffx,"\n\t  8. Dist Pyrometer 3 <-> Mill: %f m", lay->PosP3U);
	fprintf(ffx,"\n\t  8. Dist Pyrometer 4 <-> Mill: %f m", lay->PosP4);
	fprintf(ffx,"\n\t  8. Dist Pyrometer 5 <-> Mill: %f m", lay->PosP5);

	fprintf(ffx,"\n\t  8. Dist Pyrometer 23 <-> Mill: %f m", lay->PosP23);
	fprintf(ffx,"\n\t  8. Dist Pyrometer 2L <-> Mill: %f m", lay->PosP2L);
	fprintf(ffx,"\n\t  8. Dist Pyrometer 3L <-> Mill: %f m", lay->PosP3L);

	fprintf(ffx,"\n\t  9. Dist Entry Zone1 <-> Mill  %f m", lay->PosZon1Entry);
	fprintf(ffx,"\n\t  9. Dist Entry Zone2 <-> Mill  %f m", lay->PosZon2Entry);
	fprintf(ffx,"\n\t  9. Dist Entry Zone3 <-> Mill  %f m", lay->PosZon3Entry);
	fprintf(ffx,"\n\t  10.Dist Exit  Zone3 <-> Mill  %f m", lay->PosZon3Exit);

	iz = -1;
	while(++iz < i_NbCoolSect) {
		/* 11. Min/Max for BOT/TOP flows Sect.  */
		fprintf(ffx,"\n\t %d MinBot %g  MinTop %g [m^3/h]",
			iz, lay->MinFlowBtm[iz], lay->MinFlowTop[iz]);
	}
	fprintf(ffx,"\n\t Sect 1: %7.2f Sect 2[0]:%7.2f Sect 2[1]:%7.2f Sect 3: %7.2f",
		lay->MaxFlowSect1, lay->MaxFlowSect2[0],
		lay->MaxFlowSect2[1],lay->MaxFlowSect3);

	/* 12. max. transfer �ٶ� */
	fprintf(ffx,"\n\t max. transfer �ٶ� %f [m/s]", lay->MaxSpeedTransf);

	/* 13. ACC - �ٶ� */
	fprintf(ffx,"\n\t Min ACC - �ٶ� %f [m/s]", lay->MinSpeedCool);
	/* 13. ACC - �ٶ� */
	fprintf(ffx,"\n\t Max. ACC-Speeds: %6.2f %6.2f %6.2f %6.2f %6.2f [m/s]",
		lay->MaxSpeedCool[0], lay->MaxSpeedCool[1],
		lay->MaxSpeedCool[2],lay->MaxSpeedCool[3], lay->MaxSpeedCool[4]);

	/* 14. thickness for �ٶ� */
	fprintf(ffx,"\n\t Thickness Class: %6.2f %6.2f %6.2f %6.2f %6.2f [mm]",
		lay->ThickUpLimit[0], lay->ThickUpLimit[1],
		lay->ThickUpLimit[2],lay->ThickUpLimit[3], lay->ThickUpLimit[4]);

	/* 15. delay first turn [s] */
	fprintf(ffx,"\n\t Delay first turn [s]   %f", lay->DelayFirstTurn);
	/* 16. delay subs turn [s] */
	fprintf(ffx,"\n\t Delay subs turn [s]    %f", lay->DelaySubsTurn);
	/* 15. Acc turn [s] */
	fprintf(ffx,"\n\t Acceleration reverse turn [m/s^2] %f", lay->AccTurn);
	/* 15. Water Temperature [��] */
	fprintf(ffx,"\n\t Water Temperature [��] %f", lay->WaterTemp);
	/* 15. Air   Temperature [��] */
	fprintf(ffx,"\n\t Air   Temperature [��] %f", lay->AirTemp);


	fprintf(ffx,"\n\t Level II Filter pyrometer: %d", lay->iFilterPyro);
	fprintf(ffx,"\n\t Number of light barriers: %d", lay->nBls);
	iz = -1;
	while(++iz < lay->nBls)
		fprintf(ffx,"\n\t %2d: Status %2d Position %12.4f [m] Name: %s", iz, 
		lay->iStatusBls[iz], lay->fPosBls[iz], lay->sNameBls[iz]);

	fprintf(ffx,"\n\t Number of edge �ڱ� drives: %d", lay->nEdge);
	iz = -1;
	while(++iz < lay->nEdge)
		fprintf(ffx,"\n\t %2d: Status %2d Position %2d", iz,
		lay->iStatusEdge[iz], lay->iPosEdge[iz]);
	fprintf(ffx,"\n\t Number of side sprays: %d", lay->nSideSpray);
	iz = -1;
	while(++iz < lay->nSideSpray)
		fprintf(ffx,"\n\t %2d: Status %2d Position %12.4f [m] Header1 %2d Header2 %2d",
		iz, lay->iStatusSideSpray[iz], lay->fPosSideSpray[iz], 
		lay->iHeaderSideSpray1[iz], lay->iHeaderSideSpray2[iz]);

	fprintf(ffx,"\n\t Number of Headers: %d Different Type of Headers: %d",
		lay->nHeader, lay->nHeaderType);
	fprintf(ffx,"\n\t Id Zone Type Stat %12s %12s %12s %12s %12s %12s",
		sHeader[0], sHeader[1], sHeader[2],
		sHeader[3], sHeader[4], sHeader[5]);
	iz = -1;
	while(++iz < lay->nHeader)  {
		fprintf(ffx,"\n\t %2d %4d %4d %4d %12.4f %12.4f %12.4f %12.4f %12.4f %12.4f", 
			lay->iIdHeader[iz],         lay->iZoneHeader[iz],
			lay->iTypeHeader[iz],       lay->iStatusHeader[iz],
			lay->fPosHeader[iz],        lay->fLengthHeader[iz],
			lay->fFlwMinHeaderTop[iz],  lay->fFlwMaxHeaderTop[iz],
			lay->fFlwMinHeaderBtm[iz],  lay->fFlwMaxHeaderBtm[iz]);
	}

	fprintf(ffx,"\n\t Take over point Interphase Cooling (IC)    : %12.4f [m]", lay->fPosTakeoverIC);
	fprintf(ffx,"\n\t Dist. reverse point of ��� during Rolling: %12.4f [m]", lay->fPosTakeoverMill);
	if(lay->iInnerOsc !=0)
		fprintf(ffx,"\n\t System capable for inner oscillation mode  : %d", lay->iInnerOsc);
	if(lay->iOuterOsc !=0)
		fprintf(ffx,"\n\t System capable for outer oscillation mode  : %d", lay->iOuterOsc);
	fprintf(ffx,"\n\t Entry pyrometer Interphase Cooling (IC)    : %d", lay->iTempIcEn);
	fprintf(ffx,"\n\t Exit  pyrometer Interphase Cooling (IC)    : %d", lay->iTempIcEx);
	return TRUE;
}

/// @����˵�� ���ģ��PDI����
/// @���� xzz PDI���ݽṹ��
/// @���� ffx ����FILEָ��
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
//int CPreProcess::outp_pdi_sim(RALINDATA *ralInPut, FILE *ffx)
int CPreProcess::OutputPDIData(RALINDATA *ralInPut, FILE *ffx)
{
	int		iz = -1;
	struct tm *newtime;
	time_t aclock;

	time( &aclock );                 /* Get time in seconds */

	newtime = localtime( &aclock );  /* Convert time to struct */
	/* tm form */

	/*	1. basic data */
	fprintf(ffx,"\n\tPDI���� (PDI)  %s", asctime(newtime)); 
	fprintf(ffx,"\n\t ________________________\n");
	fprintf(ffx,"\n\t  1. ��� ID (float) %s", ralInPut->plateID);
	fprintf(ffx,"\n\t 1.1 priority (float) %f", ralInPut->priority);
	fprintf(ffx,"\n\t  2. thickness 	      %f m", ralInPut->thick);
	fprintf(ffx,"\n\t  3. width of ���   %f m", ralInPut->width);
	fprintf(ffx,"\n\t  4. ��ĵĳ��� %f m", ralInPut->length);
	fprintf(ffx,"\n\t  5. ��� mill temp.%f [��]", ralInPut->finishRollTemp);
	fprintf(ffx,"\n\t  6. ֹͣ��ȴtemp	%f [��]", ralInPut->targetFinishCoolTemp);
	fprintf(ffx,"\n\t  7. target cooling rate	%f [K/s]",ralInPut->cr);
	fprintf(ffx,"\n\t  8. acc-mode	%f", ralInPut->acc_mode);

	/* 2. alloy */

	fprintf(ffx,"\n\t 9.  ���ϱ�ʾ	  %s", ralInPut->mat_id);
	fprintf(ffx,"\n\t 10. carbon percentage     %f ", ralInPut->alloy_c);
	fprintf(ffx,"\n\t 11. chrome percentage	  %f ", ralInPut->alloy_cr);
	fprintf(ffx,"\n\t 12. copper percentage	  %f ", ralInPut->alloy_cu);
	fprintf(ffx,"\n\t 13. manganese percentage  %f ", ralInPut->alloy_mn);
	fprintf(ffx,"\n\t 14. molybdenum percentage %f ", ralInPut->alloy_mo);
	fprintf(ffx,"\n\t 15. nickel percentage	  %f ", ralInPut->alloy_ni);
	fprintf(ffx,"\n\t 16. silicon percentage	  %f ", ralInPut->alloy_si);
	fprintf(ffx,"\n\t 17. titanium percentage   %f ", ralInPut->alloy_ti);
	fprintf(ffx,"\n\t 18. niobium percentage	  %f ", ralInPut->alloy_nb);
	fprintf(ffx,"\n\t 19. boron percentage	     %f ", ralInPut->alloy_b);
	fprintf(ffx,"\n\t 20. vanadium percentage	  %f ", ralInPut->alloy_v);

	/*	3.	levelling data */

	fprintf(ffx,"\n\t 21. pre-levelling mode (0/1) %d", ralInPut->taskID);
	fprintf(ffx,"\n\t ��Сspeed pre-leveller %f m/s",  ralInPut->v_min_PL);
	fprintf(ffx,"\n\t ��� �ٶ� pre-leveller %f m/s",  ralInPut->v_max_PL);
	fprintf(ffx,"\n\t ��Сspeed hot-leveller %f m/s", ralInPut->v_min_HL);
	fprintf(ffx,"\n\t ��� �ٶ� hot-leveller %f m/s", ralInPut->v_max_HL);

	/* 4. other data */

	fprintf(ffx,"\n\t %f", ralInPut->eval_pyro);	/* pyrometer evaluation */
	while(++iz < NODES_NUMBER)	{
		fprintf(ffx,"\n\t %3d %6.2f %6.2f %6.2f",
			iz, ralInPut->startTempDistribution[iz],ralInPut->austeniticDecompRate[iz],ralInPut->grainSize[iz]);
	}
	return TRUE;
}

int CPreProcess::LogPreInfo(   
	FILE        *erg_f,
	const	INPUTDATAPDI *pdiInPut,
	const   flow_cr     *cr_flow,
	const   PRERESULT *PreResult,
	const   int			levelNumber,
	const   float       adaptRatioFound)
{
	float   eps = .01f;
	//there is may be some error by wbxang 20120513
	float	area = PreResult->coolBankLen * PreResult->coolWidth;
	int i;
	int     i_top  =  0;//HEAD_1_TOP;   /* offset for FILE in   */
	int     i_bot  =  0;//HEAD_1_BOT;   /* offset for FILE in   */

	char	*sHead1[] = {"No.", "�ϼ�������", "�¼�������", "�ϼ���ˮ���ܶ�", "�¼���ˮ���ܶ�", "�߲��ڱ�"};
	char	*sHead2[] = {"[-]","[ltr/min]", "[ltr/(m^2*min)]", "[mm]"};
	LogPreResult(erg_f,cr_flow,PreResult,L_TEL); /* ��Сoutput of results */   //�˺������һЩ����ȷ����ˮ���ܶȲ�value����Ĳ���

	if (adaptRatioFound >  eps)      { /* found ����Ӧvalue*/
		fprintf(erg_f,"\n\t csc_cr = %g : adaption factor = %g at level %d",
			pdiInPut->cr, adaptRatioFound, levelNumber);
	}
	else {
		fprintf(erg_f,"\n\t ��� runs the first time with csc_cr = %g K/sec", pdiInPut->cr);
		if (fabs(pdiInPut->cr - .1 * cr_flow->cr_akt) > eps)
			fprintf(erg_f,"\n\t csc_cr is not possible,est. cr = %g (1/10)K/sec", cr_flow->cr_akt);
	}

	fprintf(erg_f,"\n\n\t ����������");
	fprintf(erg_f,"\n\t %3s %9s %9s %15s %15s %6s", sHead1[0], sHead1[1],sHead1[2],sHead1[3],sHead1[4],sHead1[5]);
	fprintf(erg_f,"\n\t %3s %9s %9s %15s %15s %6s", sHead2[0], sHead2[1], sHead2[1], sHead2[2],sHead2[2],sHead2[3]);
	i = -1;

	while(++i < NUZZLE_NUMBER) 
	{                              //�˴���� �����Ų���Ϣ лǫ 2012-5-12
		fprintf(erg_f,"\n\t %3d %9.0f %9.0f %15.0f %15.0f %6d", 
			i,  PreResult->upNuzzleNumber[i]*area*60/1000, PreResult->downNuzzleNumber[i]*area*60/1000,PreResult->upNuzzleNumber[i], PreResult->downNuzzleNumber[i], cr_flow->edge[i]); 
	}

	// ��ʱ�����ͷβ�ڱε���Ϣ [8/17/2013 лǫ]
	//fprintf(erg_f,"\n\n\t ͷ�� and β�� �ڱ�:");
	//fprintf(erg_f,"\n\t Specific Flow Top:    %8.2f [ltr/(m^2*min)]", cr_flow->flow_t);
	//fprintf(erg_f,"\n\t Specific Flow Btm:    %8.2f [ltr/(m^2*min)]", cr_flow->flow_b);
	///* ͷ�� Masking */
	//fprintf(erg_f,"\n\n\t ͷ�� Masking:");
	//fprintf(erg_f,"\n\t Factor ͷ�� Flow Top: %8.2f [-]",	cr_flow->flowMaskHeadTop);
	//fprintf(erg_f,"\n\t Length Mask ͷ�� Top: %8.2f [m]",cr_flow->LengthMaskHeadTop);
	//fprintf(erg_f,"\n\t Factor ͷ�� Flow Btm: %8.2f [-]",	cr_flow->flowMaskHeadBtm);
	//fprintf(erg_f,"\n\t Length Mask ͷ�� Btm: %8.2f [m]",cr_flow->LengthMaskHeadBtm);
	//fprintf(erg_f,"\n\t Mask Header ͷ�� Top: %8d [-]",	cr_flow->nuzzleMaskHeadTop);
	//fprintf(erg_f,"\n\t Mask Header ͷ�� Btm: %8d [-]",	cr_flow->nuzzleMaskHeadBtm);
	///* β�� Masking */
	//fprintf(erg_f,"\n\n\t β�� Masking:");
	//fprintf(erg_f,"\n\t Factor β�� Flow Top: %8.2f [-]",cr_flow->flowMaskTailTop);
	//fprintf(erg_f,"\n\t Length Mask β�� Top: %8.2f [m]",cr_flow->LengthMaskTailTop);
	//fprintf(erg_f,"\n\t Factor β�� Flow Btm: %8.2f [-]",	cr_flow->fFlwMaskTailBtm);
	//fprintf(erg_f,"\n\t Length Mask β�� Btm: %8.2f [m]",cr_flow->LengthMaskTailBtm);
	//fprintf(erg_f,"\n\t Mask Header ͷ�� Top: %8d [-]",	cr_flow->nuzzleMaskTailTop);
	//fprintf(erg_f,"\n\t Mask Header ͷ�� Btm: %8d [-] \n",	cr_flow->nuzzleMaskTailBtm);

	return(1);
}

int CPreProcess::LogPreResult(
	FILE *erg_f, 
	const	flow_cr		*cr_flow, 
	const	PRERESULT *PreResult, 
	const	int			n)
{
	int it = -1;
	fprintf(erg_f,"\n\t ����Ԥ�趨�ĸ�������ֵ���� �õ��ļ�����");
	fprintf(erg_f,"\n\t flow_t      fac_b  cr     ct     edge  fac_w  HC");
	fprintf(erg_f,"\n\t [l/m^2*min] [-]    [K/s]  [s]    [mm]    [-]  ");
	while(++it < cr_flow->anz_flow) 
	{
		fprintf(erg_f,"\n\t %6.2f    %6.2f  %6.2f  %6.2f  %4d %7.3f  %6.1f",
			cr_flow->flow_rate[it], cr_flow->flow_factor[it], cr_flow->cr_rate[it],
			cr_flow->zeit_stop[it], cr_flow->edge[it], cr_flow->flow_factor_width[it],cr_flow->aveHeatTC[it]);
	}
	return(1);
}



/*  set ��ȴ�ٶ�corresponding to �� flow      */
/*  if:                                             */
/*  iFlwLimit = 2 (flow_rr.dat) or                  */
/*  if pdi value for flow is not given (0)          */
/// @����˵�� ���õ�����ʱ����ȴ�ٶ�
/// @���� erg_f
/// @���� pdi
/// @���� iFlwLimit 1: PDI, 2: model 
/// @���� fCr
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/26/2013
///int		CPreProcess::setCrFlw(FILE	*erg_f, const INPUTDATAPDI *pdi,	const int iFlwLimit, float	*fCr)
int	CPreProcess::SetLowFlowCoolingRate(FILE	*erg_f, const INPUTDATAPDI *pdi,	const int iFlwLimit, float	*fCr)
{
	// modified Cr value [10 * K/s]
	float	cr_min;
	float	cr_max;
	const	float	fEpsCr = 0.01f;
	const	float	cr_req = pdi->cr; /* requested ��ȴ�ٶ�*/
	const	int  iCrCheck = SetCollingRateLimits(pdi->thick, &cr_min, &cr_max);   //�˴�������� ��С����ȴ����
	*fCr = pdi->cr;
	if(iFlwLimit == 3){
		fprintf(erg_f, "\n\t CR Limit = %d (nonchecked PDI)", iFlwLimit);
		fprintf(erg_f, "\n\t requested ��ȴ�ٶ�   : %8.4f [K/s]", cr_req);
		return TRUE;
	}

	if(	(iFlwLimit == 2) || (cr_req < cr_min))	{
		*fCr = SetCollingRate(pdi->thick, pdi->width);
		*fCr = cr_min;
	}
	if(	(cr_req > cr_max) || (iFlwLimit == 1))	
		*fCr = cr_max;

	//fprintf(erg_f, "\n\t switch 1: PDI 2: flatness : %2d", iFlwLimit);
	fprintf(erg_f, "\n\t Ŀ������   : %8.4f [K/s]", cr_req);
	fprintf(erg_f, "\n\t ��С����   : %8.4f [K/s] (compact pattern)", cr_min);
	fprintf(erg_f, "\n\t ��������   : %8.4f [K/s] (optimum flatness)", cr_max);
	fprintf(erg_f, "\n\t �޸ĺ������   : %8.4f [K/s]\n", *fCr);
	return TRUE;
}

float	CPreProcess::SetCollingRate(const	float dThick, const	float dWidth)
{
	float	dCr;	/* estimated ��ȴ�ٶ�*/
	float	dCrMin;
	float	dCrMax;
	const	double	dThick_mm = dThick * 1000.;
	const	double	dWidth_mm = dWidth * 1000.;
	int		nThickx;
	const	double	dThickx[] = {	 3.,  10., 15.,  20., 30., 40.,
		50.,  60., 80., 100., 200., 800.};
	const	double	dCrx[]    = {   25.,  25., 20.,  18., 15.,  12.,
		10.,   10.,  8.,   6.,  2., 1.}; 
	int		nWidthx;
	const	double	dWidthx[] = {	 0.,  2000., 3000., 3500., 4000.,
		4500.,  5000.};
	const	double	dCrWidth[] ={    0.,     0.,   0.3,  0.6,   1.0,
		1.0,    1.0};
	float	dCrRed;
	nThickx	= sizeof(dThickx) / sizeof(dThickx[0]);
	dCr = (float) CalculateInterpolationValue(nThickx, dThick_mm, dThickx, dCrx);
	SetCollingRateLimits(dThick, &dCrMin, &dCrMax);
	nWidthx	= sizeof(dWidthx) / sizeof(dWidthx[0]);
	dCrRed  = (float) CalculateInterpolationValue(nWidthx, dWidth_mm, dWidthx, dCrWidth);
	dCr = (1.f - dCrRed) * dCr + dCrRed * dCrMin;
	return(dCr);
}

int	CPreProcess::SetCollingRateLimits(const	float dThick, float *fCrMin, float *fCrMax)
{
	int     iCr;
	const	double	dThick_mm = dThick * 1000.;
	const	int		iNthickx = 12;
	const	double	dThickx[] = {	 3.,  10., 15.,  20., 30., 40.,
		50.,  60., 80., 100., 200., 800.};
	const	double	dCrxMin[] = {   15.,  15., 12.,  10., 7.5, 5.2,
		4.,  3.5,  3.,  2.5, 0.5, 0.5}; 
	//const	double	dCrxMax[] = {   50.,  50., 35.,  30., 24., 20.,
	//	15.,   12.,  10.,   8.,  2., 2.}; 
	const	double	dCrxMax[] = {   50.,  50., 39.,  34., 28., 24.,
		20,   12.,  10.,   8.,  2., 2.}; 

	*fCrMin = (float) CalculateInterpolationValue(iNthickx, dThick_mm, dThickx, dCrxMin);
	*fCrMax = (float) CalculateInterpolationValue(iNthickx, dThick_mm, dThickx, dCrxMax);
	iCr = 1;
	return(iCr);
}

int     CPreProcess::WritePreSSAB(RALCOEFFDATA *lay, RALOUTDATA *pre_out, NUZZLECONDITION *nuzzleCondtion)
{
	int     iz=0;   //лǫ 2012-5-16 
	int     n_header = 0;
	int     n_side = lay->nSideSpray; /* No of side sprays */


	//������¼��������λ��
	int countMin=NUZZLE_NUMBER-1;
	int countMax=0;
	//��nuzzleCondtion������� лǫ 2012-5-16
	for (iz=0;iz<NUZZLE_NUMBER;iz++) {
		pre_out->nuzzleStatusTop[iz]=nuzzleCondtion->working_status[iz];
		pre_out->nuzzleStatusBottom[iz]=nuzzleCondtion->working_status[iz];
		pre_out->headerWorkingStatus[iz]=nuzzleCondtion->working_status[iz];
		pre_out->waterFlowTop[iz]=nuzzleCondtion->flow_header_top[iz];
		pre_out->waterFlowDown[iz]=nuzzleCondtion->flow_header_bottom[iz];

		if (0==pre_out->waterFlowTop[iz])
			pre_out->waterFactor[iz]=0;
		else
			pre_out->waterFactor[iz]=(pre_out->waterFlowDown[iz])/pre_out->waterFlowTop[iz];

		if (pre_out->headerWorkingStatus[iz]>0){
			if (iz<countMin)
				countMin=iz;

			if(iz>countMax)   //лǫ ����bug
				countMax=iz;
		}
	}

	pre_out->PreResult.iHeaderFirst=countMin;
	pre_out->PreResult.iHeaderLast=countMax;

	while(++iz < n_side) 
		pre_out->valve_sid[iz] = 1;

	pre_out->fTimeAccHead = pre_out->PreResult.tcHead ;
	pre_out->fTimeAccTail = pre_out->PreResult.tcTail;
	return TRUE;
}

int CPreProcess::WirteTelegramPre(
	const   char*plate_id,
	const   PRERESULT *PreResult,
	FILE    *erg_f)
{
	char  name_b[PATH_MAX];
	FILE    *erg_b = NULL;

	CombinFile(POSTPROCESS,plate_id,PreResult, name_b);
	if((erg_b = fopen(name_b ,"wb")) == NULL) {
		fprintf(erg_f,"\n\t *** print of result in binary FILE :  %s failed !!! *****", name_b);
		return FALSE;
	}

	fclose(erg_b);
	return TRUE;
}

int CPreProcess::CombinFile( const int  taskID, const char *plate_id, const PRERESULT *PreResult,char *fn)
{ /* build up new name for FILE (fn) for use in postcalculation  */
	char fnxx[64];
	if(strlen(plate_id))	sprintf(fnxx, "%s", plate_id);
	else			sprintf(fnxx, "%s", PreResult->plateID);
	strcat(fnxx,".bda");
	CombinFilePath(fnxx,taskID, fn);
	return(1);
}


RALCOEFFDATA CPreProcess::LayoutSim(FILE *st_err, char datname[256])
{
	RALCOEFFDATA lay ;
	FILE        *ffx = NULL;
	int         max_in = PATH_MAX - 1;
	int     iz = -1;
	/////////NG4700 by Zed////////////////////
	lay.CoolerLength=19.2f;
	lay.CoolBankLen=1.6f;
	lay.NbCoolSect=3.0f;
	lay.NbBankSect[0]=3;
	lay.NbBankSect[1]=9;
	lay.NbBankSect[2]=0;
	lay.CoolWidth=4.7f;
	lay.NbNozzBank=360;
	lay.NbNozzCentr=360;
	lay.nPyr=9;  //�˱������廹����� ԭ����ע���� ���6 // ���¼����� [8/12/2013 лǫ]
	lay.PosP1=16.0f;lay.iStatusP1=1;lay.fTempLowP1=500;lay.fTempHighP1=1100;
	lay.PosP2U=75.0f;lay.iStatusP2U=1;lay.fTempLowP2U=500;lay.fTempHighP2U=1100;
	lay.PosP3U=101.1f;lay.iStatusP3U=1;lay.fTempLowP3U=300;lay.fTempHighP3U=1100;
	lay.PosP4=112.6f;lay.iStatusP4=1;lay.fTempLowP4=300;lay.fTempHighP4=1100;
	lay.PosP5=112.6f;lay.iStatusP5=1;lay.fTempLowP5=10;lay.fTempHighP5=300;
	lay.PosP6=136.1f;lay.iStatusP6=0;lay.fTempLowP6=10;lay.fTempHighP6=700;
	lay.PosP23=75.0f;lay.iStatusP23=1;lay.fTempLowP23=500;lay.fTempHighP23=1100;
	lay.PosP2L=65.0f;lay.iStatusP2L=1;lay.fTempLowP2L=500;lay.fTempHighP2L=1100;
	lay.PosP3L=90.1f;lay.iStatusP3L=1;lay.fTempLowP3L=500;lay.fTempHighP3L=1100;
	lay.PosZon1Entry=74.2f;
	lay.PosZon2Entry=84.2f;
	lay.PosZon3Entry=94.2f;
	lay.PosZon3Exit=94.2f;
	lay.MinFlowBtm[0]=1450.0f; lay.MinFlowTop[0]=100.0f;
	lay.MinFlowBtm[1]=1250.0f; lay.MinFlowTop[1]=100.0f;
	lay.MinFlowBtm[2]=1250.0f; lay.MinFlowTop[2]=100.0f;
	lay.MaxFlowSect1=900.0f;lay.MaxFlowSect2[0]=2080.0f;lay.MaxFlowSect2[1]=900.0f;lay.MaxFlowSect3=2080.0f;
	lay.MaxSpeedTransf=2.0f;
	lay.MinSpeedCool=0.5f;
	/*lay.MaxSpeedCool[0]=1.8f;lay.MaxSpeedCool[1]=1.65f;lay.MaxSpeedCool[2]=1.55f;lay.MaxSpeedCool[3]=1.45f;lay.MaxSpeedCool[4]=1.25f;
	                         lay.MaxSpeedCool[5]=1.25f;lay.MaxSpeedCool[6]=1.15f;lay.MaxSpeedCool[7]=0.9f;
	lay.ThickUpLimit[0]=5.0f;lay.ThickUpLimit[1]=12.0f;lay.ThickUpLimit[2]=16.0f;lay.ThickUpLimit[3]=20.0f;lay.ThickUpLimit[4]=20.1f;
	                         lay.ThickUpLimit[5]=35.0f;lay.ThickUpLimit[6]=50.0f;lay.ThickUpLimit[7]=120.0f;*/


	///////////////////////Velocity-Thick for NG4700 by Zed//////////////////////////////
	lay.MaxSpeedCool[0]=1.5f;lay.MaxSpeedCool[1]=1.4f;lay.MaxSpeedCool[2]=1.3f;lay.MaxSpeedCool[3]=1.25f;lay.MaxSpeedCool[4]=1.2f;
							 lay.MaxSpeedCool[5]=1.15f;lay.MaxSpeedCool[6]=1.1f;lay.MaxSpeedCool[7]=1.0f;
    lay.ThickUpLimit[0]=10.0f;lay.ThickUpLimit[1]=15.0f;lay.ThickUpLimit[2]=20.0f;lay.ThickUpLimit[3]=25.0f;lay.ThickUpLimit[4]=30.1f;
							 lay.ThickUpLimit[5]=35.0f;lay.ThickUpLimit[6]=40.0f;lay.ThickUpLimit[7]=50.0f;

	lay.DelayFirstTurn=1.5f; /* 15. delay first turn [s] */
	lay.AccTurn=1.2f;  /* 15. Acceleration turn [m/s^2] */

	if(lay.AccTurn < .1f * ACC_TURN_DEF) 
		lay.AccTurn = ACC_TURN_DEF;

	lay.WaterTemp=25.0f;   //15. Water Temperature [��] 
	lay.AirTemp=25.0f;      // 15. Air   Temperature [��] 
	lay.iFilterPyro=1;/* 1: additional filter Level 2 */
	lay.nBls=9;/* No. of synchronization points  */
	lay.fPosBls[0]=44.2f;lay.iStatusBls[0]=0;strcpy_s(lay.sNameBls[0],"HmdY");
	lay.fPosBls[1]=57.56f;lay.iStatusBls[1]=0;strcpy_s(lay.sNameBls[1],"BlsY");
	lay.fPosBls[2]=63.135f;lay.iStatusBls[2]=0;strcpy_s(lay.sNameBls[2],"HSy1");
	lay.fPosBls[3]=67.935f;lay.iStatusBls[3]=0;strcpy_s(lay.sNameBls[3],"HSy2");
	lay.fPosBls[4]=75.935f;lay.iStatusBls[4]=0;strcpy_s(lay.sNameBls[4],"HSy3");
	lay.fPosBls[5]=83.935f;lay.iStatusBls[5]=0;strcpy_s(lay.sNameBls[5],"HSy4");
	lay.fPosBls[6]=97.75f;lay.iStatusBls[6]=0;strcpy_s(lay.sNameBls[6],"BlsX");
	lay.fPosBls[7]=98.22f;lay.iStatusBls[7]=0;strcpy_s(lay.sNameBls[7],"HmdX");
	lay.fPosBls[8]=101.6f;lay.iStatusBls[8]=0;strcpy_s(lay.sNameBls[8],"PyrX");
	lay.fPosBls[9] = 0.0;lay.iStatusBls[9] = 0;strcpy_s(lay.sNameBls[9],"Vituell");
	lay.fPosBls[10] = 0.0;lay.iStatusBls[10] = 0;strcpy_s(lay.sNameBls[10],"Vituell");

	lay.nEdge=3;
	lay.iPosEdge[0]=0;lay.iStatusEdge[0]=1;
	lay.iPosEdge[1]=4;lay.iStatusEdge[1]=1;
	lay.iPosEdge[2]=8;lay.iStatusEdge[2]=1;
	lay.iPosEdge[3]=-1;lay.iStatusEdge[3]=0;//////�ڱβ�����Ҫȷ��

	lay.nSideSpray=4;
	lay.fPosSideSpray[0]=61.3f;lay.iStatusSideSpray[0]=0;lay.iHeaderSideSpray1[0]=-1;lay.iHeaderSideSpray2[0]=-1;
	lay.fPosSideSpray[1]=70.1f;lay.iStatusSideSpray[1]=0;lay.iHeaderSideSpray1[1]=-1;lay.iHeaderSideSpray2[1]=6;
	lay.fPosSideSpray[2]=78.1f;lay.iStatusSideSpray[2]=0;lay.iHeaderSideSpray1[2]=-1;lay.iHeaderSideSpray2[2]=11;
	lay.fPosSideSpray[3]=85.3f;lay.iStatusSideSpray[3]=0;lay.iHeaderSideSpray1[3]=-1;lay.iHeaderSideSpray2[3]=-1;////����λ����Ҫȷ��

	lay.nHeader=12; lay.nHeaderType=1;
	lay.iIdHeader[0]=1;lay.iZoneHeader[0]=1;lay.iTypeHeader[0]=1;lay.iStatusHeader[0]=1;lay.fPosHeader[0]=74.6f;lay.fLengthHeader[0]=1.6f;lay.fFlwMinHeaderTop[0]=100.0f;lay.fFlwMaxHeaderTop[0]=1250.0f;lay.fFlwMinHeaderBtm[0]=100.0f;lay.fFlwMaxHeaderBtm[0]=2100.0f;
	lay.iIdHeader[1]=2;lay.iZoneHeader[1]=1;lay.iTypeHeader[1]=1;lay.iStatusHeader[1]=1;lay.fPosHeader[1]=76.2f;lay.fLengthHeader[1]=1.6f;lay.fFlwMinHeaderTop[1]=100.0f;lay.fFlwMaxHeaderTop[1]=1250.0f;lay.fFlwMinHeaderBtm[1]=100.0f;lay.fFlwMaxHeaderBtm[1]=2100.0f;
	lay.iIdHeader[2]=3;lay.iZoneHeader[2]=1;lay.iTypeHeader[2]=1;lay.iStatusHeader[2]=1;lay.fPosHeader[2]=77.8f;lay.fLengthHeader[2]=1.6f;lay.fFlwMinHeaderTop[2]=100.0f;lay.fFlwMaxHeaderTop[2]=1250.0f;lay.fFlwMinHeaderBtm[2]=100.0f;lay.fFlwMaxHeaderBtm[2]=2100.0f;
	lay.iIdHeader[3]=4;lay.iZoneHeader[3]=2;lay.iTypeHeader[3]=1;lay.iStatusHeader[3]=1;lay.fPosHeader[3]=79.4f;lay.fLengthHeader[3]=1.6f;lay.fFlwMinHeaderTop[3]=100.0f;lay.fFlwMaxHeaderTop[3]=1250.0f;lay.fFlwMinHeaderBtm[3]=100.0f;lay.fFlwMaxHeaderBtm[3]=2100.0f;
	lay.iIdHeader[4]=5;lay.iZoneHeader[4]=2;lay.iTypeHeader[4]=1;lay.iStatusHeader[4]=1;lay.fPosHeader[4]=81.0f;lay.fLengthHeader[4]=1.6f;lay.fFlwMinHeaderTop[4]=100.0f;lay.fFlwMaxHeaderTop[4]=1250.0f;lay.fFlwMinHeaderBtm[4]=100.0f;lay.fFlwMaxHeaderBtm[4]=2100.0f;
	lay.iIdHeader[5]=6;lay.iZoneHeader[5]=2;lay.iTypeHeader[5]=1;lay.iStatusHeader[5]=1;lay.fPosHeader[5]=82.6f;lay.fLengthHeader[5]=1.6f;lay.fFlwMinHeaderTop[5]=100.0f;lay.fFlwMaxHeaderTop[5]=1250.0f;lay.fFlwMinHeaderBtm[5]=100.0f;lay.fFlwMaxHeaderBtm[5]=2100.0f;
	lay.iIdHeader[6]=7;lay.iZoneHeader[6]=2;lay.iTypeHeader[6]=1;lay.iStatusHeader[6]=1;lay.fPosHeader[6]=84.2f;lay.fLengthHeader[6]=1.6f;lay.fFlwMinHeaderTop[6]=100.0f;lay.fFlwMaxHeaderTop[6]=1250.0f;lay.fFlwMinHeaderBtm[6]=100.0f;lay.fFlwMaxHeaderBtm[6]=2100.0f;
	lay.iIdHeader[7]=8;lay.iZoneHeader[7]=2;lay.iTypeHeader[7]=1;lay.iStatusHeader[7]=1;lay.fPosHeader[7]=85.8f;lay.fLengthHeader[7]=1.6f;lay.fFlwMinHeaderTop[7]=100.0f;lay.fFlwMaxHeaderTop[7]=1250.0f;lay.fFlwMinHeaderBtm[7]=100.0f;lay.fFlwMaxHeaderBtm[7]=2100.0f;
	lay.iIdHeader[8]=9;lay.iZoneHeader[8]=2;lay.iTypeHeader[8]=1;lay.iStatusHeader[8]=1;lay.fPosHeader[8]=87.4f;lay.fLengthHeader[8]=1.6f;lay.fFlwMinHeaderTop[8]=100.0f;lay.fFlwMaxHeaderTop[8]=1250.0f;lay.fFlwMinHeaderBtm[8]=100.0f;lay.fFlwMaxHeaderBtm[8]=2100.0f;
	lay.iIdHeader[9]=10;lay.iZoneHeader[9]=2;lay.iTypeHeader[9]=1;lay.iStatusHeader[9]=1;lay.fPosHeader[9]=89.0f;lay.fLengthHeader[9]=1.6f;lay.fFlwMinHeaderTop[9]=100.0f;lay.fFlwMaxHeaderTop[9]=1250.0f;lay.fFlwMinHeaderBtm[9]=100.0f;lay.fFlwMaxHeaderBtm[9]=2100.0f;
	lay.iIdHeader[10]=11;lay.iZoneHeader[10]=2;lay.iTypeHeader[10]=1;lay.iStatusHeader[10]=1;lay.fPosHeader[10]=90.6f;lay.fLengthHeader[10]=1.6f;lay.fFlwMinHeaderTop[10]=100.0f;lay.fFlwMaxHeaderTop[10]=1250.0f;lay.fFlwMinHeaderBtm[10]=100.0f;lay.fFlwMaxHeaderBtm[10]=2100.0f;
	lay.iIdHeader[11]=12;lay.iZoneHeader[11]=2;lay.iTypeHeader[11]=1;lay.iStatusHeader[11]=1;lay.fPosHeader[11]=92.2f;lay.fLengthHeader[11]=1.6f;lay.fFlwMinHeaderTop[11]=100.0f;lay.fFlwMaxHeaderTop[11]=1250.0f;lay.fFlwMinHeaderBtm[11]=100.0f;lay.fFlwMaxHeaderBtm[11]=2100.0f;
	lay.fPosTakeoverIC=5.0f;   // Take over point IC 
	lay.fPosTakeoverMill=-50.0f;	/* distance of Reverse point to mill during rolling   */

	lay.iInnerOsc=0;	/* 1: System capable inner oscillation  mode */
	lay.iOuterOsc=1;	/* 1: System capable outer oscillation  mode */

	/* determine indes first active header */
	iz = -1;
	while(++iz < lay.nHeader)  {
		if(lay.iStatusHeader[iz] == 1) 
			break;
	}
	lay.iHeaderFirst = iz;
	iz = lay.nHeader;
	while(--iz >= 0) {
		if(lay.iStatusHeader[iz] == 1) break;
	}
	lay.iHeaderLast = iz + 1;

	lay.iTempIcEn=3;

	if((lay.iTempIcEn < 1) || (lay.iTempIcEn > 3)) 
		lay.iTempIcEn = 1;

	lay.iTempIcEx=3;  //Exit  Pyrometer Interphase cooling (1 to 5, possible, 5: default)
	if((lay.iTempIcEx < 1) || (lay.iTempIcEx > 5)) 
		lay.iTempIcEn = 5;

	return(lay);
}

int  CPreProcess::CalculatePre(
	FILE          *st_err,    /* �쳣���� */
	FILE          *erg_f,     /* ��Ľ��FILE */
	const   struct T_AccSysPar	*SysPar,    /* ϵͳ����  */ 
	const   struct T_AccPattern	*AccPattern,/* ��ȴģʽ    */
	INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
	const	  RALCOEFFDATA *lay,		/* layout data      */
	const	  STRATEGY *accStrat,	/* ���Բ��� */
	PRERESULT *PreResult,        /* Ԥ������    */
	PHYSICAL *ver_par1,  /* ��ȴ���� */
	flow_cr       *cr_flow,   /* ������Ϣ   */
	ADAPTATION *adapt,
	SLP		*SLP_post)
{
	float          real_temp;//, temp_diff;
	MATERIAL matal;
	COMPUTATIONAL compx;
	ADAPTATION adapt1; /* ����Ӧvalue */

	SLP adaptPara;

	float           cr_air;
	float       end_temp_air[NODES_NUMBER]; /*  temp end air dist.  */
	float       end_aust_air[NODES_NUMBER]; /*  temp end air dist.  */
	float      beforeAirCoolTemp=0.0;
	float      afterAirCoolTemp=0.0;
	int        adapt_step_ACC=2;
	int test=0;
	int i=0; //лǫ���� 2012-5-16

	int open_type=0;
	float tail_time = 0.0;

	//��ʼ����ѧϰϵ��
	m_SelfLearnFunction.InitAdaptation(&adapt1);

	strcpy_s(adapt1.sPatternID, "0");
	adapt1.vx_last = 0.0;
	pdiInPut->FlowRate = (int)cr_flow->flow_t;
	if( 1 == cr_flow->iadaption){
		ver_par1->levelNumber =m_SelfLearnFunction.AdaptionProcess( pdiInPut, PreResult->nStroke, &adapt1, ver_par1->adaptKey,
			&ver_par1->adaptRatioFound,ver_par1->waterTemp);
	}
	m_SelfLearnFunction.WriteAdaptationInfo(erg_f, &adapt1);

	PreResult->scanTSelflearn=adapt1.fSpare[2];  // ɨ���������ѧϰ ϵ�� [1/2/2014 xie]

	PreResult->readWaterFactorSl=adapt1.fSpare[1];  // ��ȡ ˮ�� ��ѧϰ���� [9/18/2014 qian]


	//��ѧϰ�޼���������[2014/12/8 by Zed]  // add [1/13/2015 qian]
	memset(&adaptPara,0,sizeof(SLP));
	m_SelfLearnFunction.AdaptationSLP_Init(&adaptPara);
	m_SelfLearnFunction.AdaptationSLP_Read(pdiInPut,&adaptPara);
	memcpy (SLP_post,&adaptPara,sizeof(SLP))
		;
	ver_par1->adaptRatioFound = adaptPara.SLP_AP;//�ָ�ֵ��SLP_Found,������ԭ��ѧϰϵ��adaptRatioFound[2014/12/10 by Zed]
	
	///////////////////////add by xie 10-22 judge selflearn /////////////////////////
	if (0==adapt1.rec_plates)
		PreResult->isWellSelfLearned=1;
	else if (adapt1.rec_plates<7)
		PreResult->isWellSelfLearned=2;
	else if (adapt1.rec_plates>=7)
		PreResult->isWellSelfLearned=3;
	////////////////////////////////////////////////
	

	*adapt = adapt1;
	if ((ver_par1->adaptRatioFound>0.5)&&(ver_par1->adaptRatioFound<=4.5))
	{
		fprintf(erg_f,"\n \t ��������ѧϰ����ϵ�� %4.3f \n",ver_par1->adaptRatioFound);
		//logger::LogDebug("system"," %s ��������ѧϰ����ϵ�� %4.3f ",pdiInPut->plateID,ver_par1->adaptRatioFound);
	}
		
	else if(ver_par1->adaptRatioFound<0.5){
		ver_par1->adaptRatioFound=0.5f;
		fprintf(erg_f,"\n \t ��ѧϰ����ϵ����С ����Ϊ 0.5");
		//logger::LogDebug("system"," %s ��ѧϰ����ϵ����С ����Ϊ %4.3f ",pdiInPut->plateID,ver_par1->adaptRatioFound);
	}else if(ver_par1->adaptRatioFound>4.5){
		ver_par1->adaptRatioFound=4.0f;
		fprintf(erg_f,"\n \t ��ѧϰ����ϵ������ ����Ϊ4");
		//logger::LogDebug("system"," %s ��ѧϰ����ϵ������ ����Ϊ %4.3f ",pdiInPut->plateID,ver_par1->adaptRatioFound);
	}
	
	fprintf(erg_f,"\n \t �߲�������ѧϰϵ�� %3.2f ",PreResult->scanTSelflearn);
	//logger::LogDebug("system","�߲�������ѧϰϵ�� %3.2f ",PreResult->scanTSelflearn);  // add [1/2/2014 xie]

	PreResult->selfLearnAlphaCoe=ver_par1->adaptRatioFound;

	/* 2.1) calculate ��ʼ���� for temp. TF     */
	/*���ÿ���ȷ����ȴǰ�ĳ�ʼTemperature�ֲ�                    */
	//ȷ�������������˴�Ϊȷ������Temperature�ֲ�����ȷ��
	InitAirStartProfile(&compx, pdiInPut, ver_par1);//�˴�ȷ���˼��㷽��Ϊ ���䣬������   лǫ���� 2012-4-26
	/*      build cr-air    */
	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
	//ȷ����������
	EstimationComputationalParam(erg_f,&compx, pdiInPut, ver_par1);
	m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);     /* SolveBoundary     */
	m_TemperatureField.CalculationTemperatureDistribution(st_err, erg_f, pdiInPut, &compx, ver_par1,&matal);  //�������ʱ �ǽ�����Temperature+50��  Ȼ����������Temperature

	SaveAirTelegram(TEMPERATURE_CURVE_NUMBER,pdiInPut->finishRollTemp, PreResult, compx.zx, compx.tx);

	PreResult->beforeAirCoolTemp=compx.tx[compx.computedNumber-1];

	PlateTailAirTempDrop(st_err,erg_f,pdiInPut,&compx,PreResult,lay,ver_par1,&matal); // �˴����ɸ����� [8/17/2013 лǫ]
	PlateHeadAirTempDrop(st_err,erg_f,pdiInPut,&compx,PreResult,lay,ver_par1,&matal);

	cr_air =InitAirVariable(&compx, pdiInPut, ver_par1);  //����valueΪ�������١�

	cr_air *=10.f;    /* output of cr_air in 1/10 K/s     */
	PreResult->coolRateAir = cr_air;
	PreResult->tempWater = ver_par1->waterTemp; 
	PreResult->tempAir = ver_par1->t_umg;  
    PreResult->vPred=lay->initSpeed;      // ת���ʼ���ٶ� [9/23/2013 лǫ]

	float lastTimeStartCoolingTemp=0.0f;
	if (adapt1.tf_last>700) 
		lastTimeStartCoolingTemp=adapt1.tf_last;  //add 11-8
	else
		lastTimeStartCoolingTemp=compx.tx[compx.computedNumber-1];

	float detaTempAir=0.5f*(lastTimeStartCoolingTemp-compx.tx[compx.computedNumber-1])-20;//add by xie 9-20  , modify to 0.7 10-10 xie  2015-5-13 ����ʹ�ð��Զ� ��ѧϰϵ������һ��ƫ�� �����¶�-20��

 	i=-1;
	while(++i < NODES_NUMBER){  //лǫ���� ���������Temperature 2012-5-16
		PreResult->airEndTemp[i]=compx.startTempDistribution[i]+detaTempAir;  //add by xie 9-20
		PreResult->airEndAust[i]=compx.startAusteniteDistribution[i];
	}

	real_temp = InitWaterCoolxq(&compx,              
		end_temp_air,
		end_aust_air,
		pdiInPut, 
		ver_par1,
		detaTempAir); //  �����������˿���ε���ȴ��� ��Ϊˮ��Ŀ���Temperature лǫ 

	fprintf(erg_f,"\n\t ��ʼ��ȴTemperature: %f",real_temp);
	fprintf(erg_f,"\n\t �Ͽ�ְ�Ŀ���Temperature: %5.2f",lastTimeStartCoolingTemp);
	fprintf(erg_f,"\n\t ����Temperature����ϵ��: %5.2f",detaTempAir);
	fprintf(erg_f,"\n\t Ŀ����ȴ�ٶ�: %f",pdiInPut->cr);
	fprintf(erg_f,"\n \t ��ʼ��������ȴ����ж�����: \n \t acc_mode %d, pass_mode %d, open_type %d, activeSecType %d, activeFirstJet %d",
		pdiInPut->acc_mode,pdiInPut->pass_mode,pdiInPut->open_type,pdiInPut->activeSecType,pdiInPut->activeFirstJet);

	//1 ����ȴģʽ�ж�
	switch (pdiInPut->acc_mode)
	{
	case ACC: //0.2kg
		open_type=AccOpenTypeDecide(st_err,erg_f,pdiInPut,lay,accStrat,PreResult,ver_par1,
			cr_flow,&compx,adapt1,real_temp,end_temp_air,end_aust_air);
		break;
	case UFC://UFC
		open_type=AccOpenTypeDecide(st_err,erg_f,pdiInPut,lay,accStrat,PreResult,ver_par1,
			cr_flow,&compx,adapt1,real_temp,end_temp_air,end_aust_air);
		break;
	case 3://UFC+ACC
		break;
	case 4: //ACC+UFC
		break;
	case DQ://Ԥ��
		open_type=DqOpenTypeDecide(st_err,erg_f,pdiInPut,lay,accStrat,PreResult,ver_par1,
			cr_flow,&compx,adapt1,real_temp,end_temp_air,end_aust_air);
		break;
	case BACKACC:  //ֻ����5�鼯��
		open_type=BackAccOpenTypeDecide(st_err,erg_f,pdiInPut,lay,accStrat,PreResult,ver_par1, // лǫ ���� 2012-7-12
			cr_flow,&compx,adapt1,real_temp,end_temp_air,end_aust_air);
		break;

	case 6: //Ԥ��
		fprintf(erg_f,"\n\t pdiInPut->acc_mode ������Ч");  // лǫ ���� 2012-7-12
		break;
	}

	if (pdiInPut->open_type  == OPEN_STATUS_SLIT2)
	{   //лǫ �޸� 2012-5-21 
		fprintf(erg_f,"\n\t ���ܶȶ��ϱ���ˮ���ܶ�: %f [ltr/(m^2*min)]",cr_flow->flow_t);
		fprintf(erg_f,"\n\t ���ܶȶ��±���ˮ���ܶ�: %f [ltr/(m^2*min)]",cr_flow->flow_b);
		fprintf(erg_f,"\n\t ��϶���ϱ���ˮ���ܶ�: %f [ltr/(m^2*min)]",cr_flow->gap_flow_t);
		fprintf(erg_f,"\n\t ��϶���±���ˮ���ܶ�: %f [ltr/(m^2*min)]",cr_flow->gap_flow_b);
		fprintf(erg_f,"\n\t ��϶����ȴtime: %f s",cr_flow->time_c-cr_flow->time_c_highdensity);
		fprintf(erg_f,"\n\t ���ܶ���ȴtime: %f s",cr_flow->time_c_highdensity);
		fprintf(erg_f,"\n\t ����ȴtime: %f s",cr_flow->time_c);
	} 
	else{
		fprintf(erg_f,"\n\t ���ܶȶ��ϱ���ˮ���ܶ�: %f [ltr/(m^2*min)]",cr_flow->flow_t);
		fprintf(erg_f,"\n\t ���ܶȶ��±���ˮ���ܶ�: %f [ltr/(m^2*min)]",cr_flow->flow_b);
		fprintf(erg_f,"\n\t ����ȴtime: %f s",cr_flow->time_c);
	}

	PreResult->coeCoolRate = (10000.0f * ver_par1->adaptRatioFound); 

	int iz;   //лǫ���� 2012-5-22
	iz = -1; 
	while(++iz < 10) 
		PreResult->crCalcLayer[iz] = (10.f* cr_flow->cr_rate[iz]);   //лǫ���� 2012- 5-22
		
	configeSocketSendMessage(PreResult,pdiInPut,&compx,cr_flow);

	return 0;
}

int  CPreProcess::CalcPreOSC_Out(
	FILE          *st_err,    /* �쳣���� */
	FILE          *erg_f,     /* ��Ľ��FILE */
	const   struct T_AccSysPar	*SysPar,    /* ϵͳ����  */ 
	const   struct T_AccPattern	*AccPattern,/* ��ȴģʽ    */
	INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
	const	  RALCOEFFDATA *lay,		/* layout data      */
	const	  STRATEGY *accStrat,	/* ���Բ��� */
	PRERESULT *PreResult,        /* Ԥ������    */
	PHYSICAL *ver_par1,  /* ��ȴ���� */
	flow_cr       *cr_flow,   /* ������Ϣ   */
	ADAPTATION *adapt) 
{
	float          real_temp;//, temp_diff;
	MATERIAL matal;
	COMPUTATIONAL compx;
	ADAPTATION adapt1; /* ����Ӧvalue */
	float           cr_air;
	float       end_temp_air[NODES_NUMBER]; /*  temp end air dist.  */
	float       end_aust_air[NODES_NUMBER]; /*  temp end air dist.  */
	float      beforeAirCoolTemp=0.0;
	float      afterAirCoolTemp=0.0;
	int        adapt_step_ACC=2;
	int test=0;
	int i=0; //лǫ���� 2012-5-16

	int open_type=0;
	float tail_time = 0.0;

	//��ʼ����ѧϰϵ��
	m_SelfLearnFunction.InitAdaptation(&adapt1);

	strcpy_s(adapt1.sPatternID, "0");
	adapt1.vx_last = 0.0;
	pdiInPut->FlowRate = (int)cr_flow->flow_t;
	if( 1 == cr_flow->iadaption){
		ver_par1->levelNumber =m_SelfLearnFunction.AdaptionProcess( pdiInPut, PreResult->nStroke, &adapt1, ver_par1->adaptKey,
			&ver_par1->adaptRatioFound,ver_par1->waterTemp);
	}
	m_SelfLearnFunction.WriteAdaptationInfo(erg_f, &adapt1);

	///////////////////////add by xie 10-22 judge selflearn /////////////////////////
	if (0==adapt1.rec_plates)
		PreResult->isWellSelfLearned=1;
	else if (adapt1.rec_plates<5)
		PreResult->isWellSelfLearned=2;
	else if (adapt1.rec_plates>=5)
		PreResult->isWellSelfLearned=3;
	////////////////////////////////////////////////

	*adapt = adapt1;
	if ((ver_par1->adaptRatioFound>0.5)&&(ver_par1->adaptRatioFound<=2.3))
	{
		fprintf(erg_f,"\n \t ��������ѧϰ����ϵ�� %4.3f \n",ver_par1->adaptRatioFound);
		logger::LogDebug("system"," %s ��������ѧϰ����ϵ�� %4.3f ",pdiInPut->plateID,ver_par1->adaptRatioFound);
	}

	else if(ver_par1->adaptRatioFound<0.5){
		ver_par1->adaptRatioFound=1.0f;
		fprintf(erg_f,"\n \t ��ѧϰ����ϵ����С ����Ϊ 1");
		logger::LogDebug("system"," %s ��ѧϰ����ϵ����С ����Ϊ %4.3f ",pdiInPut->plateID,ver_par1->adaptRatioFound);
	}else if(ver_par1->adaptRatioFound>2.3){
		ver_par1->adaptRatioFound=1.9f;
		fprintf(erg_f,"\n \t ��ѧϰ����ϵ������ ����Ϊ1.9");
		logger::LogDebug("system"," %s ��ѧϰ����ϵ������ ����Ϊ %4.3f ",pdiInPut->plateID,ver_par1->adaptRatioFound);
	}

	PreResult->selfLearnAlphaCoe=ver_par1->adaptRatioFound;

	/* 2.1) calculate ��ʼ���� for temp. TF     */
	/*���ÿ���ȷ����ȴǰ�ĳ�ʼTemperature�ֲ�                    */
	//ȷ�������������˴�Ϊȷ������Temperature�ֲ�����ȷ��
	InitAirStartProfile(&compx, pdiInPut, ver_par1);//�˴�ȷ���˼��㷽��Ϊ ���䣬������   лǫ���� 2012-4-26
	/*      build cr-air    */
	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
	//ȷ����������
	EstimationComputationalParam(erg_f,&compx, pdiInPut, ver_par1);
	m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);     /* SolveBoundary     */
	m_TemperatureField.CalculationTemperatureDistribution(st_err, erg_f, pdiInPut, &compx, ver_par1,&matal);  //�������ʱ �ǽ�����Temperature+50��  Ȼ����������Temperature

	SaveAirTelegram(TEMPERATURE_CURVE_NUMBER,pdiInPut->finishRollTemp, PreResult, compx.zx, compx.tx);

	PreResult->beforeAirCoolTemp=compx.tx[compx.computedNumber-1];

	PlateTailAirTempDrop(st_err,erg_f,pdiInPut,&compx,PreResult,lay,ver_par1,&matal); // �˴����ɸ����� [8/17/2013 лǫ]

	PlateHeadAirTempDrop(st_err,erg_f,pdiInPut,&compx,PreResult,lay,ver_par1,&matal);

	cr_air =InitAirVariable(&compx, pdiInPut, ver_par1);  //����valueΪ�������١�

	cr_air *=10.f;    /* output of cr_air in 1/10 K/s     */
	PreResult->coolRateAir = cr_air;
	PreResult->tempWater = ver_par1->waterTemp; 
	PreResult->tempAir = ver_par1->t_umg;  
	PreResult->vPred=lay->initSpeed;      // ת���ʼ���ٶ� [9/23/2013 лǫ]

	float lastTimeStartCoolingTemp=0.0f;
	if (adapt1.tf_last>700) 
		lastTimeStartCoolingTemp=adapt1.tf_last;  //add 11-8
	else
		lastTimeStartCoolingTemp=compx.tx[compx.computedNumber-1];

	float detaTempAir=0.5f*(lastTimeStartCoolingTemp-compx.tx[compx.computedNumber-1]);//add by xie 9-20  , modify to 0.7 10-10 xie

	i=-1;
	while(++i < NODES_NUMBER){  //лǫ���� ���������Temperature 2012-5-16
		PreResult->airEndTemp[i]=compx.startTempDistribution[i]+detaTempAir;  //add by xie 9-20
		PreResult->airEndAust[i]=compx.startAusteniteDistribution[i];
	}

	real_temp = InitWaterCoolxq(&compx,              
		end_temp_air,
		end_aust_air,
		pdiInPut, 
		ver_par1,
		detaTempAir); //  �����������˿���ε���ȴ��� ��Ϊˮ��Ŀ���Temperature лǫ 
	
	pdiInPut->open_type = 1;

	fprintf(erg_f,"\n\t ��ʼ��ȴTemperature: %f",real_temp);
	fprintf(erg_f,"\n\t �Ͽ�ְ�Ŀ���Temperature: %5.2f",lastTimeStartCoolingTemp);
	fprintf(erg_f,"\n\t ����Temperature����ϵ��: %5.2f",detaTempAir);
	fprintf(erg_f,"\n\t Ŀ����ȴ�ٶ�: %f",pdiInPut->cr);
	fprintf(erg_f,"\n \t ��ʼ��������ȴ����ж�����: \n \t acc_mode %d, pass_mode %d, open_type %d, activeSecType %d, activeFirstJet %d",
		pdiInPut->acc_mode,pdiInPut->pass_mode,pdiInPut->open_type,pdiInPut->activeSecType,pdiInPut->activeFirstJet);

	
	Osc_UfcOpenType(st_err,erg_f,pdiInPut,lay,accStrat,PreResult,ver_par1,
		cr_flow,&compx,adapt1,real_temp,end_temp_air,end_aust_air);

		fprintf(erg_f,"\n\t ���ܶȶ��ϱ���ˮ���ܶ�: %f [ltr/(m^2*min)]",cr_flow->flow_t);
		fprintf(erg_f,"\n\t ���ܶȶ��±���ˮ���ܶ�: %f [ltr/(m^2*min)]",cr_flow->flow_b);
		fprintf(erg_f,"\n\t ����ȴtime: %f s",cr_flow->time_c);
	

	PreResult->coeCoolRate = (10000.0f * ver_par1->adaptRatioFound); 

	int iz;   //лǫ���� 2012-5-22
	iz = -1; 
	while(++iz < 10) 
		PreResult->crCalcLayer[iz] = (10.f* cr_flow->cr_rate[iz]);   //лǫ���� 2012- 5-22

	configeSocketSendMessage(PreResult,pdiInPut,&compx,cr_flow);

	return 0;
}

void CPreProcess::configeSocketSendMessage(
	    PRERESULT *PreResult,
		INPUTDATAPDI *pdiInPut,		/* PDI data           */
	    COMPUTATIONAL *compx,			/* computational para.*/
	    flow_cr       *cr_flow   /* flow value		  */)  
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
    SocketLine1Define  *sock1=theApp->sock1;
	switch(pdiInPut->PltIcFlgTemp)
	{
	case 0:
		//strcpy_s(sock1->send2011.GfcPieceId,pdiInPut->plateID);
		//strcpy_s(sock1->send2011.GfcPltMatId,pdiInPut->matID);
		//strcpy_s(sock1->send2011.GfcPltMatCode,pdiInPut->matID);
		//sock1->send2011.GfcResPresetSpd=PreResult->vPred;
		//sock1->send2011.GfcPltAccType=pdiInPut->acc_mode;
		//sock1->send2011.GfcPltCoolMode1Pass=1;
		//SendGfcL2PresetAccResult_LP(&sock1->send2011);

		strcpy_s(sock1->send2031.GfcPieceId,pdiInPut->plateID);
		sock1->send2031.GfcResFnlColTimWat=cr_flow->time_c;
		sock1->send2031.GfcResFnlCr=compx->localCoolRate;
		sock1->send2031.GfcResFnlAveTmp=compx->tx[compx->computedNumber];
		sock1->send2031.GfcResFnlSufTmp=compx->tx_bot[compx->computedNumber];
		sock1->send2031.GfcResFnlColTimAir=0;
		SendGfcL2ExtColTimeRes_LP(&sock1->send2031);
		break;
	case 1:
		strcpy_s(sock1->send2012.GfcPieceId,pdiInPut->plateID);
		strcpy_s(sock1->send2012.GfcPltMatId,pdiInPut->matID);
		strcpy_s(sock1->send2012.GfcPltMatCode,pdiInPut->matID);
		sock1->send2012.GfcResSlbTcr1PresetSpd=PreResult->vPred;
		sock1->send2012.GfcSlbTcr1AccType=pdiInPut->acc_mode;
		sock1->send2012.GfcSlbTcr1CoolMode1Pass=1;
		SendGfcL2PresetAccResult_tcr1(&sock1->send2012);

		strcpy_s(sock1->send2032.GfcPieceId,pdiInPut->plateID);
		sock1->send2032.GfcResInterSlabTcr1ColTimAir=50/(PreResult->vPred+0.5);
		sock1->send2032.GfcResInterSlabTcr1ColTimWat=cr_flow->time_c;
		sock1->send2032.GfcResTcr1ColTimAir=50/(PreResult->vPred+0.5);
		sock1->send2032.GfcResTcr1ColTimSta=cr_flow->time_c;
		sock1->send2032.GfcResTcr1PresetSpd=PreResult->vPred;
		sock1->send2032.GfcPltTcr1AccType=pdiInPut->acc_mode;
		sock1->send2032.GfcPltTcr1CoolMode1Pass=1;
		sock1->send2032.GfcResTcr1Cr=compx->localCoolRate;
		sock1->send2032.GfcResTcr1AveTmp=compx->tx[compx->computedNumber];
		sock1->send2032.GfcResTcr1SufTmp=compx->tx_bot[compx->computedNumber];
		SendGfcL2ExtColTimeRes_tcr1(&sock1->send2032);
		break;
	case 2:

		strcpy_s(sock1->send2013.GfcPieceId,pdiInPut->plateID);
		strcpy_s(sock1->send2013.GfcPltMatId,pdiInPut->matID);
		strcpy_s(sock1->send2013.GfcPltMatCode,pdiInPut->matID);
		sock1->send2013.GfcResSlbTcr2PresetSpd=PreResult->vPred;
		sock1->send2013.GfcSlbTcr2AccType=pdiInPut->acc_mode;
		sock1->send2013.GfcSlbTcr2CoolMode1Pass=1;
		SendGfcL2PresetAccResult_tcr2(&sock1->send2013);

		strcpy_s(sock1->send2033.GfcPieceId,pdiInPut->plateID);
		sock1->send2033.GfcResInterSlabTcr2ColTimAir=50/(PreResult->vPred+0.5);
		sock1->send2033.GfcResInterSlabTcr2ColTimWat=cr_flow->time_c;
		sock1->send2033.GfcResTcr2ColTimAir=50/(PreResult->vPred+0.5);
		sock1->send2033.GfcResTcr2ColTimSta=cr_flow->time_c;
		sock1->send2033.GfcResTcr2PresetSpd=PreResult->vPred;
		sock1->send2033.GfcPltTcr2AccType=pdiInPut->acc_mode;
		sock1->send2033.GfcPltTcr2CoolMode1Pass=1;
		sock1->send2033.GfcResTcr2Cr=compx->localCoolRate;
		sock1->send2033.GfcResTcr2AveTmp=compx->tx[compx->computedNumber];
		sock1->send2033.GfcResTcr2SufTmp=compx->tx_bot[compx->computedNumber];
		SendGfcL2ExtColTimeRes_tcr2(&sock1->send2033);
		break;
	}
	
	return;

}


int CPreProcess::calc_slit_jet_flowrate(
	FILE          *st_err,		/* �쳣���� */
	FILE          *erg_f,			/* ��Ľ��FILE */
	INPUTDATAPDI *pdiInPut,		/* PDI data           */
	MATERIAL *matal,			/* material data      */
	PHYSICAL *ver_par1,		/* ��ȴ���� */
	COMPUTATIONAL *compx,			/* computational para.*/
	flow_cr       *cr_flow,       /* flow value		  */
	const    float         real_temp,		/* Temperature        */
	const    float         *end_temp_air,  /* air Temperature    */
	const    float         *end_aust_air,
	const	 RALCOEFFDATA *lay	/* layout data      */
	)  /* austenite decomp.  */     
{
	int iv;
	int it;
	int iz;

	iz = -1;

	//CTemperatureField m_TemperatureField;
	for (iv = 0; iv < cr_flow->slit_anz_flow; iv++){ /* loop over flow rates */
		ver_par1->topFlowRate = cr_flow->gap_flow_rate[iv];
		ver_par1->bottomFlowRate =  cr_flow->gap_flow_rate[iv] *cr_flow->gap_flow_factor[iv] *cr_flow->gap_flow_factor_width[iv];
		ver_par1->actualStopTime = 0.0f;
		ver_par1->actualStartTemp = real_temp;

		compx->computeID = 0;
		it  = -1;
		while(++it < NODES_NUMBER) {
			compx->startTempDistribution[it] = end_temp_air[it];
			compx->startAusteniteDistribution[it] = end_aust_air[it]; 
		}

		pdiInPut->working_stage=2;                  //slit �λ���ϵ����ȡ��־ лǫ 2012-5-8

		EstimationComputationalParam(erg_f,compx, pdiInPut, ver_par1);

		compx->calculationStopTime=(compx->activeSlitNumber*lay->CoolBankLen)/lay->initSpeed;

		m_TemperatureField.SolveBoundary(st_err, ver_par1, matal, pdiInPut);      /* SolveBoundary     */
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, compx, ver_par1, matal);

		cr_flow->zeit_stop[iv] = compx->calculationStopTime; //лǫ �ȼ����϶�ε���ȴtime   //2012-5-11 �޸� ȥ�� +=

		pdiInPut->working_stage=1;                  //jet �λ���ϵ����ȡ��־ лǫ 2012-5-8

		ver_par1->topFlowRate = cr_flow->flow_rate[iv];
		ver_par1->bottomFlowRate =  cr_flow->flow_rate[iv] *cr_flow->flow_factor[iv] *cr_flow->flow_factor_width[iv];

		//�˴��м������Ĵ洢���� compx->t_center ����value�� ��μ��㿪ʼ�洢������Ľ���취 �����ǰ���Ҫ��������ȡ����
		// �����µĽṹ����

		m_TemperatureField.SolveBoundary(st_err, ver_par1, matal, pdiInPut);      /* SolveBoundary     */
		m_TemperatureField.Temperatureolver(st_err, erg_f, pdiInPut, compx, ver_par1, matal);

		cr_flow->zeit_stop[iv] += compx->calculationStopTime;     //�����ȴtime

		cr_flow->zeit_highdensity_stop[iv]=compx->calculationStopTime;      //���ܶ���ȴtime

	}                   /*      loop over flow rates    */
	return TRUE;
}

int CPreProcess::gap_cr_pol(	FILE	*erg_f,		/* ���FILE */
	const	INPUTDATAPDI *pdiInPut,	/*PDI���� */
	const	ADAPTATION *adapt,		/* ����Ӧvalue */
	flow_cr *cr_flow,	/* flow value */
	const	float	qcr_find,/* ����ӦvalueCR */
	int tStage)	
{
	int         iret1 = 0; /* return value for interpolation */
	int         iret2 = 0; /* ����Ӧvalueis in use */
	int			iRet  = 0; /* summarized value */
	float       crCorrect; /* ����ed ��ȴ�ٶ�by ����Ӧvalue*/
	/* value for original configuration (no adaption) */
	float       fFlwTop1;       /* specific top flow */
	float       fCrAct1;        /* ��ȴ�ٶ�*/
	float       fTimeWat1;      /* cooling time in water */
	float       fFlwFactor1;    /* multiplier btm/top flow */
	/* value for modified configuration (adaption) */
	float       fFlwTop2=0.0f;       /* specific top flow */
	float       fCrAct2=0.0f;        /* ��ȴ�ٶ�*/
	float       fTimeWat2=0.0f;      /* cooling time in water */
	float       fFlwFactor2=0.0f;    /* multiplier btm/top flow */
	float       eps = 1.0e-6f;
	float		qcr_act = qcr_find;
	float aveHC;

	if (qcr_act <= eps) 
		qcr_act = 1.0f;  /* Not suitable adapt_value */
	crCorrect = pdiInPut->cr / qcr_act;
	iret1 = CalculateInterpolationFlow(cr_flow->anz_flow,
		pdiInPut->cr,
		cr_flow->gap_flow_rate,
		cr_flow->cr_rate,
		cr_flow->zeit_highdensity_stop,
		cr_flow->gap_flow_factor,
		cr_flow->gap_flow_factor_width,
		cr_flow->aveHeatTC,
		&fFlwTop1,
		&fCrAct1,
		&fTimeWat1,
		&fFlwFactor1,
		&aveHC);

	/* set to default if flow ����ion is impossible */

	iret1			= 2;
	iRet            = iret1;
	cr_flow->gap_flow_t = fFlwTop1;
	cr_flow->cr_akt = fCrAct1;
	cr_flow->time_c_highdensity = fTimeWat1;
	cr_flow->gap_flow_b = fFlwTop1 * fFlwFactor1;

	/* set return value if one limitation is on the limit */
	if((iret1 == 0) && (iret2 != 0)) 
		iRet = iret2;

	/* use flow ����ion only if flow is within limitation */		
	if((iret1 == 0) && (iret2 == 0))	{
		cr_flow->gap_flow_t = fFlwTop2;
		cr_flow->cr_akt = fCrAct2;
		cr_flow->time_c_highdensity = fTimeWat2;
		cr_flow->gap_flow_b = fFlwTop2 * fFlwFactor2;
		iRet = 0;
	}

	if (1==tStage){
		fprintf(erg_f, "\n\n\t Selected flow value iRet = %d", iRet);
		OutputFlow(erg_f, cr_flow);
	}

	return(iRet);
}

//�˺����еĸ�����������0.2MPa��������
int  CPreProcess::AccOpenTypeDecide(
	FILE          *st_err,    /* �쳣���� */
	FILE          *erg_f,     /* ��Ľ��FILE */
	INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
	const	  RALCOEFFDATA *lay,		/* layout data      */
	const	  STRATEGY *accStrat,	/* ���Բ��� */
	PRERESULT *PreResult,        /* Ԥ������    */
	PHYSICAL *ver_par1,  /* ��ȴ���� */
	flow_cr       *cr_flow, /* ������Ϣ   */
	COMPUTATIONAL *compx,
	ADAPTATION adapt1,
	float               real_temp,
	float   *end_temp_air,
	float   *end_aust_air
	) 
{
	float      temp_diff;
	float      crCorrect=0.0;
	MATERIAL matal;
	int it=0;
	int reachProperCR=0;
	int CR_cycle_count=0;
	float activeJetSepSucCoe = 1.75;
	int tStage=0;

	while ((0==reachProperCR)  &&   CR_cycle_count<4  ){ 
		switch(pdiInPut->open_type){

		case OPEN_STATUS_SUC_JET://������ȴ
			//�˴����������ֹͣ����
			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
			pdiInPut->working_stage=1;   // лǫ���� �����ж� jet or slit
			CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,	compx, cr_flow, real_temp, end_temp_air, end_aust_air);
			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;
			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++)
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];

			//���ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			reachProperCR=1;

			break;

		case OPEN_STATUS_SLIT2://2��϶

			compx->activeSlitNumber=2;

			//�˴����������ֹͣ����
			compx->calculationStopTime=compx->activeSlitNumber*lay->CoolBankLen/lay->initSpeed;

			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			calc_slit_jet_flowrate(st_err, erg_f, pdiInPut,&matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air,lay);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����

			for (it = 0; it < cr_flow->anz_flow; it++){
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];
			}

			//��ת�ж�����
			if(pdiInPut->cr>cr_flow->cr_rate[cr_flow->anz_flow-1])
			{
				reachProperCR=1;
				fprintf(erg_f,"\n \t �������ٳ��� ���п�����ʽ������Χ��open_typeĬ��Ϊ OPEN_STATUS_SLIT2");
			}
			else if(pdiInPut->cr<cr_flow->cr_rate[0]) 
			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;
			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++)
			{
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];
			}

			if(pdiInPut->cr>cr_flow->cr_rate[cr_flow->anz_flow-1])
			{
				reachProperCR=1;
				fprintf(erg_f,"\n \t �������ٳ��� ���п�����ʽ������Χ��open_typeĬ��Ϊ OPEN_STATUS_SLIT4");
			}
			else if(pdiInPut->cr<cr_flow->cr_rate[0]){
				pdiInPut->open_type = OPEN_STATUS_SLIT2;
				fprintf(erg_f,"\n \t open_type����Ϊ OPEN_STATUS_SLIT2");
				break;
			}
			//ACC��ȴģʽʱ�����ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);
			PreResult->coolModeFlg = gap_cr_pol(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);
			reachProperCR=1;

			break;
		case OPEN_STATUS_SEP_JET: //ϡ����ȴ

			//�˴����������ֹͣ����
			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,compx, cr_flow, real_temp, end_temp_air, end_aust_air);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����

			for (it = 0; it < cr_flow->anz_flow; it++)
				cr_flow->cr_rate[it] = temp_diff / (cr_flow->zeit_stop[it]*activeJetSepSucCoe);

			if(pdiInPut->cr>cr_flow->cr_rate[cr_flow->anz_flow-1]){
				pdiInPut->open_type = OPEN_STATUS_SUC_JET;  //лǫ�޸� 2012-5-10
				fprintf(erg_f,"\n \t open_type����Ϊ OPEN_STATUS_SUC_JET");
				break;
			}
			else if(pdiInPut->cr<cr_flow->cr_rate[0]){
				reachProperCR=1;
				fprintf(erg_f,"\n \t �������ٳ��� ���п�����ʽ������Χ��open_typeĬ��Ϊ OPEN_STATUS_SEP_JET");
			}

			//ACC��ȴģʽʱ�����ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);
			reachProperCR=1;
			break;
		}
		CR_cycle_count++;
	}

	tStage=1;

	PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);// лǫ ���� 2012-5-29

	return 1;
}

//�˺����еĸ�����������0.5MPa��������
int  CPreProcess::UfcOpenTypeDecide(
	FILE          *st_err,    /* �쳣���� */
	FILE          *erg_f,     /* ��Ľ��FILE */
	INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
	const	  RALCOEFFDATA *lay,		/* layout data      */
	const	  STRATEGY *accStrat,	/* ���Բ��� */
	PRERESULT *PreResult,        /* Ԥ������    */
	PHYSICAL *ver_par1,  /* ��ȴ���� */
	flow_cr       *cr_flow, /* ������Ϣ   */
	COMPUTATIONAL *compx,
	ADAPTATION adapt1,
	float               real_temp,
	float   *end_temp_air,
	float   *end_aust_air
	)
{
	float      temp_diff;
	float      crCorrect;
	MATERIAL matal;
	int it=0;
	int reachProperCR=0;
	int CR_cycle_count=0;
	float activeJetSepSucCoe = 1.75;
	int tStage=0;

	while ((0==reachProperCR)  &&   CR_cycle_count<4  ){ 
		switch(pdiInPut->open_type)
		{
		case OPEN_STATUS_SUC_JET://������ȴ
			//�˴����������ֹͣ����

			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air);
			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++)
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];

			//���ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);
			crCorrect = pdiInPut->cr / ver_par1->adaptRatioFound;
			if(crCorrect>cr_flow->cr_rate[cr_flow->anz_flow-1]){
				pdiInPut->open_type = OPEN_STATUS_SLIT2;
			}
			else if(crCorrect<cr_flow->cr_rate[0]){
				pdiInPut->open_type = OPEN_STATUS_SEP_JET;
			}
			else{
				reachProperCR=1;
			}
			break;

		case OPEN_STATUS_SLIT2: //2��϶
			compx->activeSlitNumber=2;

			//�˴����������ֹͣ����
			compx->calculationStopTime=compx->activeSlitNumber*lay->CoolBankLen/lay->initSpeed;

			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			calc_slit_jet_flowrate(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air,lay);


			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++)
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];

			/*//ACC��ȴģʽʱ�����ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�*/
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			PreResult->coolModeFlg = gap_cr_pol(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			crCorrect = pdiInPut->cr / ver_par1->adaptRatioFound;

			if(crCorrect>cr_flow->cr_rate[cr_flow->anz_flow-1]){
				pdiInPut->open_type = OPEN_STATUS_SLIT4;
			}
			else if(crCorrect<cr_flow->cr_rate[0]){
				pdiInPut->open_type = OPEN_STATUS_SUC_JET;
			}
			else{
				reachProperCR=1;
			}
			break;

		case OPEN_STATUS_SLIT4: //4 ��϶
			compx->activeSlitNumber=4;
			//�˴����������ֹͣ����
			compx->calculationStopTime=compx->activeSlitNumber*lay->CoolBankLen/lay->initSpeed;

			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			calc_slit_jet_flowrate(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air,lay);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++){
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];
			}

			//ACC��ȴģʽʱ�����ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			PreResult->coolModeFlg = gap_cr_pol(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			crCorrect = pdiInPut->cr / ver_par1->adaptRatioFound;
			if(crCorrect>cr_flow->cr_rate[cr_flow->anz_flow-1]){
				reachProperCR=1;
			}
			else if(crCorrect<cr_flow->cr_rate[0]){
				pdiInPut->open_type = OPEN_STATUS_SLIT2;
			}
			else{
				reachProperCR=1;
			}
			break;

		case OPEN_STATUS_SEP_JET: //ϡ����ȴ

			//�˴����������ֹͣ����

			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����

			for (it = 0; it < cr_flow->anz_flow; it++){
				cr_flow->cr_rate[it] = temp_diff / (cr_flow->zeit_stop[it]*activeJetSepSucCoe);
			}

			//ACC��ȴģʽʱ�����ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			//ϡ��ʽ��ȴ������ȴ��ʱ�� �ڼ��ܿ��չ����� ʵ�ּ���
			// cr_flow->time_c=cr_flow->time_c*1.75; //���Կ��ǵ�ϡ��ʽ�Ų���ˮ��Ӱ�� лǫ 2012-4-26

			crCorrect = pdiInPut->cr / ver_par1->adaptRatioFound;

			if(crCorrect>cr_flow->cr_rate[cr_flow->anz_flow-1]){
				pdiInPut->open_type = OPEN_STATUS_SEP_JET;
			}
			else if(crCorrect<cr_flow->cr_rate[0]){
				reachProperCR=1;
			}
			else{
				reachProperCR=1;
			}
			break;
		}
		CR_cycle_count++;
	}
	//�˴���Ƿȱ CoolRateFlows ����
	return 1;
}

int  CPreProcess::Osc_UfcOpenType(
	FILE          *st_err,    /* �쳣���� */
	FILE          *erg_f,     /* ��Ľ��FILE */
	INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
	const	  RALCOEFFDATA *lay,		/* layout data      */
	const	  STRATEGY *accStrat,	/* ���Բ��� */
	PRERESULT *PreResult,        /* Ԥ������    */
	PHYSICAL *ver_par1,  /* ��ȴ���� */
	flow_cr       *cr_flow, /* ������Ϣ   */
	COMPUTATIONAL *compx,
	ADAPTATION adapt1,
	float               real_temp,
	float   *end_temp_air,
	float   *end_aust_air
	)
{
	float      temp_diff;
	float      crCorrect;
	MATERIAL matal;
	int it=0;
	int reachProperCR=0;
	int CR_cycle_count=0;
	float activeJetSepSucCoe = 1.75;
	int tStage=0;

	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);


	CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,
		compx, cr_flow, real_temp, 
		end_temp_air, end_aust_air);
	temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

		for (it = 0; it < cr_flow->anz_flow; it++)
		cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];

	//���ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
	PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);
	crCorrect = pdiInPut->cr / ver_par1->adaptRatioFound;
	
	
	return 1;
	
}

//�˺����еĸ�����������0.5MPa��������
int  CPreProcess::DqOpenTypeDecide(
	FILE          *st_err,    /* �쳣���� */
	FILE          *erg_f,     /* ��Ľ��FILE */
	INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
	const	  RALCOEFFDATA *lay,		/* layout data      */
	const	  STRATEGY *accStrat,	/* ���Բ��� */
	PRERESULT *PreResult,        /* Ԥ������    */
	PHYSICAL *ver_par1,  /* ��ȴ���� */
	flow_cr       *cr_flow, /* ������Ϣ   */
	COMPUTATIONAL *compx,
	ADAPTATION adapt1,
	float               real_temp,
	float   *end_temp_air,
	float   *end_aust_air
	)
{
	float      temp_diff;
	float      crCorrect;
	MATERIAL matal;
	int it=0;
	int reachProperCR=0;
	int CR_cycle_count=0;
	float activeJetSepSucCoe = 1.75;
	int tStage=0;

	while ((0==reachProperCR)  &&   CR_cycle_count<4  ){ 
		switch(pdiInPut->open_type)
		{
		case OPEN_STATUS_SUC_JET://������ȴ
			//�˴����������ֹͣ����

			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����

			for (it = 0; it < cr_flow->anz_flow; it++)
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];

			//���ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			crCorrect = pdiInPut->cr / ver_par1->adaptRatioFound;
			if(crCorrect>cr_flow->cr_rate[cr_flow->anz_flow-1]){
				pdiInPut->open_type = OPEN_STATUS_SLIT2;
			}
			else if(crCorrect<cr_flow->cr_rate[0]){
				pdiInPut->open_type = OPEN_STATUS_SEP_JET;
			}
			else{
				reachProperCR=1;
			}
			break;

		case OPEN_STATUS_SLIT2: //2��϶
			compx->activeSlitNumber=2;

			//�˴����������ֹͣ����
			compx->calculationStopTime=compx->activeSlitNumber*lay->CoolBankLen/lay->initSpeed;

			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			calc_slit_jet_flowrate(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air,lay);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++){
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];
			}

			/*//ACC��ȴģʽʱ�����ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�*/
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			PreResult->coolModeFlg = gap_cr_pol(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			crCorrect = pdiInPut->cr / ver_par1->adaptRatioFound;

			if(crCorrect>cr_flow->cr_rate[cr_flow->anz_flow-1])
				pdiInPut->open_type = OPEN_STATUS_SLIT4;
			else if(crCorrect<cr_flow->cr_rate[0])
				pdiInPut->open_type = OPEN_STATUS_SUC_JET;
			else
				reachProperCR=1;
			break;

		case OPEN_STATUS_SLIT4: //4 ��϶
			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++)
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];

			//���ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			reachProperCR=1;
			break;

		case OPEN_STATUS_SEP_JET: //ϡ����ȴ

			//�˴����������ֹͣ����
			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

			CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++)
				cr_flow->cr_rate[it] = temp_diff / (cr_flow->zeit_stop[it]*activeJetSepSucCoe);

			//ACC��ȴģʽʱ�����ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);

			//ϡ��ʽ��ȴ������ȴ��ʱ�� �ڼ��ܿ��չ����� ʵ�ּ���
			crCorrect = pdiInPut->cr / ver_par1->adaptRatioFound;
			if(crCorrect>cr_flow->cr_rate[cr_flow->anz_flow-1])
				pdiInPut->open_type = OPEN_STATUS_SEP_JET;
			else if(crCorrect<cr_flow->cr_rate[0])
				reachProperCR=1;
			else
				reachProperCR=1;
			break;
		}
		CR_cycle_count++;
	}
	return 1;
}


int CPreProcess::CalcTempCurve(
	FILE 			*st_err,
	FILE 		*erg_f ,
	INPUTDATAPDI *pdiInPut,
	RALCOEFFDATA *lay,		/* layout data      */
	flow_cr       *cr_flow,              /* ������Ϣ   */
	PHYSICAL *ver_par1,
	NUZZLECONDITION   *nuzzleCondtion,
	PRERESULT *PreResult
	/* ��ȴ���� */
	)
{
	COMPUTATIONAL compx;
	MATERIAL matal;

	int i=0;
	int j=0;
	int k=0;
	int iz = -1;

	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
	compx.thickNodeNumber=NODES_NUMBER;
	compx.matrixElementNumber = 2 * compx.thickNodeNumber - 1;
	EstimationComputationalParam(erg_f,&compx, pdiInPut, ver_par1);
	while(++iz < 19){  //��ʼTemperature�ڵ�
		compx.startTempDistribution[iz] = PreResult->airEndTemp[iz];
		compx.startAusteniteDistribution[iz] = 100.f;
	}

	for (i=0;i<NUZZLE_NUMBER;i++){   //ѭ������Ӧ��Ϊ���ܵķֶ�������
		if (1==nuzzleCondtion->working_status[i]){  //���ܿ���
			compx.detaTime=0.2f;    //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ
			ver_par1->topFlowRate=nuzzleCondtion->flow_header_top[i];
			ver_par1->bottomFlowRate=nuzzleCondtion->flow_header_bottom[i];
			ver_par1->sw_air = 1;
			ver_par1->coolPhaseNumber = 1;
			ver_par1->art[1] = 1;	/* ˮ�� */
			ver_par1->duration[1] = 0.0f;
			compx.computeID = 2; 

			compx.calculationStopTime=lay->CoolBankLen/lay->initSpeed;

			pdiInPut->working_stage=1;              //jet�λ���ϵ����ȡ��־λ лǫ 2012-5-8

			m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, &compx, ver_par1, &matal);

			iz = -1;
			while(++iz < 4)	
			{
				PreResult->waterOne[iz]		= (int) (compx.zx[iz] * 1000.0f);
				PreResult->waterOne[iz+4]	= (int) (compx.tx[iz]);
			}
		} 
		else if (0==nuzzleCondtion->working_status[i]){ //���ܹر� ����
			compx.detaTime=0.2f;              //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ
			//ȷ�������������˴�Ϊȷ������Temperature�ֲ�����ȷ��
			compx.computeID = 2;	/*	const Temperature distribution	*/
			ver_par1->coolPhaseNumber = 0;
			ver_par1->method = 1;	/* ����	*/
			ver_par1->art[0] = 0;
			ver_par1->duration[0] = 0.0f;
			ver_par1->taskID = PREPROCESS;
			ver_par1->sw_air = 0;	/*���� */

			/*      data input      */
			compx.femCalculationOutput = 0;    /* NO !! output of table in computex.c  */
			compx.integra = 2;
			compx.isSymetric     = 0;    /* nonsymmetric problem */

			/* calculation of element sizes compx->lm[]	*/
			CalculationElementSizes(&compx, 2, pdiInPut->thick);

			compx.matrixElementNumber = 2 * compx.thickNodeNumber - 1;
			compx.calculationStopTime=lay->CoolBankLen/lay->initSpeed;
			m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, &compx, ver_par1, &matal);
		}			
		else if (2==nuzzleCondtion->working_status[i])  //���ܿ���  slit�� лǫ
		{
			compx.detaTime=0.2f;    //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ

			ver_par1->topFlowRate=nuzzleCondtion->flow_header_top[i];
			ver_par1->bottomFlowRate=nuzzleCondtion->flow_header_bottom[i];

			ver_par1->sw_air = 1;
			ver_par1->coolPhaseNumber = 1;
			ver_par1->art[1] = 1;	/* ˮ�� */
			ver_par1->duration[1] = 0.0f;
			compx.computeID = 2; 

			compx.calculationStopTime=lay->CoolBankLen/lay->initSpeed;

			pdiInPut->working_stage=2;              //slit�λ���ϵ����ȡ��־λ лǫ 2012-5-8

			m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, &compx, ver_par1, &matal);
		} 

		// �洢ÿһ�� �����Temperature���� 
		for(j=0;j<compx.computedNumber;j++){
			//compx->zx_temple[k]=compx->zx_temple[k-1]+compx->zx[j]	;        /* time value last cooling curve       */
			if (0==k)
				compx.zx_temple[k] =compx.zx[j]	;  
			else
				compx.zx_temple[k]=compx.zx_temple[k-1]+compx.zx[j]-compx.zx[j-1]	;

			compx.tx_temple[k]=compx.tx[j];        /* caloric mean Temperature value last curve       */
			compx.tx_cen_temple[k]=compx.tx_cen[j];    /* Temperature in the center line of ���          */
			compx.tx_top_temple[k]=compx.tx_top[j];    /* surface top Temperature value last curve        */
			compx.tx_bot_temple[k]=compx.tx_bot[j];    /* surface bottom temp. value last curve           */
			compx.cx_temple[k]=compx.cx[j];        /* ��ȴ�ٶ�in C                                */
			k++;
			//nuzzleCondtion->calcTempCurveANZK=k;   лǫ�޸� 2012-5-8 ����ȡ�˴����ٶ�
		}
	}
	///////////////////////////////////////////////////////////////////
	//// ����ȴ������ ��������
	///////////////////////////////////////////////////////////////////
	//ȷ�������������˴�Ϊȷ������Temperature�ֲ�����ȷ��

	compx.computeID = 2;	/*	const Temperature distribution	*/
	ver_par1->coolPhaseNumber = 0;
	ver_par1->method = 1;	/* ����	*/
	ver_par1->art[0] = 0;
	ver_par1->duration[0] = 0.0f;
	ver_par1->taskID = PREPROCESS;
	ver_par1->sw_air = 0;	/*���� */

	compx.femCalculationOutput = 0; 
	compx.integra = 2;
	compx.isSymetric     = 0;    /* nonsymmetric problem */
	/* calculation of element sizes compx->lm[]	*/
	CalculationElementSizes(&compx, 2, pdiInPut->thick);

	compx.matrixElementNumber = 2 * compx.thickNodeNumber - 1;
	compx.calculationStopTime=20/(lay->initSpeed+0.15f);

	m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
	m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, &compx, ver_par1, &matal);

	// �洢ÿһ�� �����Temperature���� 
	for(j=0;j<compx.computedNumber;j++){
		//compx->zx_temple[k]=compx->zx_temple[k-1]+compx->zx[j]	;        /* time value last cooling curve       */
		compx.zx_temple[k]=compx.zx_temple[k-1]+compx.zx[j] - compx.zx[j-1]	;
		compx.tx_temple[k]=compx.tx[j];        /* caloric mean Temperature value last curve       */
		compx.tx_cen_temple[k]=compx.tx_cen[j];    /* Temperature in the center line of ���          */
		compx.tx_top_temple[k]=compx.tx_top[j];    /* surface top Temperature value last curve        */
		compx.tx_bot_temple[k]=compx.tx_bot[j];    /* surface bottom temp. value last curve           */
		compx.cx_temple[k]=compx.cx[j];        /* ��ȴ�ٶ�in C    */
		k++;
		nuzzleCondtion->calcTempCurveANZK=k;
	}

	fprintf(erg_f,"\n \t �趨��̺��ģ�����Temperature����");
	fprintf(erg_f,"\n \t time       ƽ��      �ϱ���       �Ĳ�      ��ȴ�ٶ�      \n");
	for (i=0;i<nuzzleCondtion->calcTempCurveANZK;i++)
		fprintf(erg_f,"\t %f      %f       %f       %f       %f       \n",compx.zx_temple[i],compx.tx_temple[i],compx.tx_top_temple[i],compx.tx_cen_temple[i],compx.cx_temple[i]);
	return 0;
}

int CPreProcess::CoolSchduleDecide(   //лǫ ���� ���ܿ����ĺ��� 2012-5-3
	FILE 			*st_err,
	FILE 		*erg_f ,
	NUZZLECONDITION   *nuzzleCondtion,
	RALCOEFFDATA *lay,		/* layout data      */
	const  flow_cr    *cr_flow, 
	const		INPUTDATAPDI *pdi)
{
	int i=0;
	nuzzleCondtion->TempThick=pdi->thick;
	for (i;i<NUZZLE_NUMBER;i++){
		nuzzleCondtion->flow_header_top[i]=0;
		nuzzleCondtion->flow_header_bottom[i]=0;
		nuzzleCondtion->working_status[i]=0;
	}
	fprintf(erg_f,"\n \t ���������ȴ����ж�����: \n \t acc_mode %d, pass_mode %d, open_type %d, activeSecType %d, activeFirstJet %d",
		pdi->acc_mode,pdi->pass_mode,pdi->open_type,pdi->activeSecType,pdi->activeFirstJet);
	switch(pdi->acc_mode)
	{
	case ACC: //acc 
		switch (pdi->pass_mode)
		{
		case TCR_MODEL:
			nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
			nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

			if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
				fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
				nuzzleCondtion->header_no=NUZZLE_NUMBER;
			}

			if(0==nuzzleCondtion->header_no){            //лǫ ���� 2012-7-12
				nuzzleCondtion->header_no=1;
				fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
			}

			CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

			//for (i=0;i<nuzzleCondtion->header_no;i++){
			//	nuzzleCondtion->flow_header_top[1+i]=cr_flow->flow_t;
			//	nuzzleCondtion->flow_header_bottom[1+i]=cr_flow->flow_b;
			//	nuzzleCondtion->working_status[1+i]=1;
			//}
			if (pdi->cr<15)
			{
				CoolSchOpenFront(nuzzleCondtion,cr_flow);  // add [11/18/2013 лǫ]
			}
			else
			{
				CoolSchOpenFrontSlit(nuzzleCondtion,cr_flow);  // add [4/24/2014 лǫ]
			}
			
			i=0;
			fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);
			break;

		case PASS_THROUGH_MODEL: //����ͨ��ʽ

			switch(pdi->open_type)
			{
			case OPEN_STATUS_SUC_JET:  //��������
				//����lay �ṹ���м���������Ϣ
				switch(pdi->activeSecType)
				{
				case ACTIVE_SECTION_FRONT:
					switch(pdi->activeFirstJet)
					{
					case OPEN_FROM_FIRST_SLIT:

						nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
						nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.8); //����������ȴ������,ȡ��

						if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
							fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
							nuzzleCondtion->header_no=NUZZLE_NUMBER;
						}

						if(0==nuzzleCondtion->header_no){    //лǫ ���� 2012-7-12
							nuzzleCondtion->header_no=1;
							fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
						}

						CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

						for (i=0;i<nuzzleCondtion->header_no;i++){
							nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
							nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
							nuzzleCondtion->working_status[i]=1;
						}

						i=0;

						fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);///////One pair of slit header means double slit headers by Zed 

						break;

					case OPEN_FROM_FIRST_JET:

						nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
						nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

						if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
							fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
							nuzzleCondtion->header_no=NUZZLE_NUMBER;
						}

						if(0==nuzzleCondtion->header_no){            //лǫ ���� 2012-7-12
							nuzzleCondtion->header_no=1;
							fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
						}

						CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

						//for (i=0;i<nuzzleCondtion->header_no;i++){
						//	nuzzleCondtion->flow_header_top[1+i]=cr_flow->flow_t;
						//	nuzzleCondtion->flow_header_bottom[1+i]=cr_flow->flow_b;
						//	nuzzleCondtion->working_status[1+i]=1;
						//}
												
						if (pdi->cr<15)
						{
							CoolSchOpenFront(nuzzleCondtion,cr_flow);  // add [11/18/2013 лǫ]
						}
						else
						{
							CoolSchOpenFrontSlit(nuzzleCondtion,cr_flow);  // add [4/24/2014 лǫ]
						}

						i=0;
						fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);
						break;
					}

					break;

				case ACTIVE_SECTION_BACK:
					nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
					nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

					if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
						fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
						nuzzleCondtion->header_no=NUZZLE_NUMBER;
					}

					if(0==nuzzleCondtion->header_no){           //лǫ ���� 2012-7-12
						nuzzleCondtion->header_no=1;
						fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
					}

					CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

					if (1==nuzzleCondtion->header_no%3)   // add [11/2/2013 лǫ]
					{

						switch(nuzzleCondtion->header_no)
						{
							case 4:
								for(i=1;i<5;i=i+3) 
								{
									nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
									nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
									nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
									nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i-1]=cr_flow->flow_t;
									nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i-1]=cr_flow->flow_b;
									nuzzleCondtion->working_status[NUZZLE_NUMBER-i-1]=1;
								}
								break;
							case 7:
								for(i=1;i<4;i++) 
								{
									nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
									nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
									nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
								}
								for(i=4;i<8;i=i+3) 
								{
									nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
									nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
									nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
									nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i-1]=cr_flow->flow_t;
									nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i-1]=cr_flow->flow_b;
									nuzzleCondtion->working_status[NUZZLE_NUMBER-i-1]=1;
								}
								break;
							case 10:
								for(i=1;i<7;i++) 
								{
									nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
									nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
									nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
								}
								for(i=7;i<11;i=i+3) 
								{
									nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
									nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
									nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
									nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i-1]=cr_flow->flow_t;
									nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i-1]=cr_flow->flow_b;
									nuzzleCondtion->working_status[NUZZLE_NUMBER-i-1]=1;
								}
								break;
							}
						}
						else
						{
							for (i=1;i<nuzzleCondtion->header_no+1;i++){
								nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
								nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
								nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
							}
						}
					
					i=0;
					fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);

					break;

				case ACTIVE_SECTION_TWO:    

					nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
					nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

					if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
						fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
						nuzzleCondtion->header_no=NUZZLE_NUMBER;
					}

					if(0==nuzzleCondtion->header_no){             //лǫ ���� 2012-7-12
						nuzzleCondtion->header_no=1;
						fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
					}

					CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

					for (i=1;i<nuzzleCondtion->header_no+1;i++){
						nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
						nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
						nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
					}

					i=0;
					fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);

					break;
				}
				break;   //лǫ���� 2012-5-8

			case OPEN_STATUS_SLIT2: //2���϶

				nuzzleCondtion->flow_header_top[0]=cr_flow->gap_flow_t;
				nuzzleCondtion->flow_header_bottom[0]=cr_flow->gap_flow_b;
				nuzzleCondtion->working_status[0]=2;

				nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
				nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

				if(0==nuzzleCondtion->header_no){
					nuzzleCondtion->header_no=1;
					fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
				}

				if (nuzzleCondtion->header_no>NUZZLE_NUMBER-1){
					fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
					nuzzleCondtion->header_no=NUZZLE_NUMBER-1;
					CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);
				}
				else{

					CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);        // ���������ٶ� ��ǰ 2012-5-16
					nuzzleCondtion->header_no=nuzzleCondtion->header_no-1;   // �˴���������ȴ������������� ��ȥslit��

				}										

				for (i=0;i<nuzzleCondtion->header_no;i++){
					nuzzleCondtion->flow_header_top[1+i]=cr_flow->flow_t;
					nuzzleCondtion->flow_header_bottom[1+i]=cr_flow->flow_b;
					nuzzleCondtion->working_status[1+i]=1;
				}
				i=0;

				fprintf(erg_f,"\n\t ������������:%d",1+nuzzleCondtion->header_no);
				nuzzleCondtion->header_no=nuzzleCondtion->header_no+1;   //Ϊ�Ժ�Ĵ���in �ṹ����׼�� лǫ 2012-5-12
				break;

			case OPEN_STATUS_SLIT4: //�����϶����

				//add by xie ��ʱ�����λ�õ���������  9-17//////////////////
				nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
				nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.3); //����������ȴ������,ȡ��

				if (nuzzleCondtion->header_no>NUZZLE_NUMBER-1){
					fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
					nuzzleCondtion->header_no=NUZZLE_NUMBER-1;
				}

				if(0==nuzzleCondtion->header_no) {
					nuzzleCondtion->header_no=1;
					fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
				}

				CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

				for (i=0;i<nuzzleCondtion->header_no;i++){
					nuzzleCondtion->flow_header_top[1+i]=cr_flow->flow_t;
					nuzzleCondtion->flow_header_bottom[1+i]=cr_flow->flow_b;
					nuzzleCondtion->working_status[1+i]=1;
				}
				i=0;
				fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);

				break;
			case OPEN_STATUS_SEP_JET: //ϡ����ȴ
				nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
				nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��
				if (nuzzleCondtion->header_no>NUZZLE_NUMBER/2)  //�Ӹ��ܿ�ʼ�� лǫ 2012-6-25
				{
					fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
					nuzzleCondtion->header_no=4;    //лǫ 2012-5-12
				}

				if(0==nuzzleCondtion->header_no)    //лǫ ���� 2012-7-6
				{
					nuzzleCondtion->header_no=1;
					fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
				}


				CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

				int j=0;
				for (i=0, j=i;i<nuzzleCondtion->header_no;i=i+1)  //л 2012-5-22 �޸Ŀ��շ�ʽ
				{
					nuzzleCondtion->flow_header_top[1+j]=cr_flow->flow_t;
					nuzzleCondtion->flow_header_bottom[1+j]=cr_flow->flow_b;
					nuzzleCondtion->working_status[1+j]=1;
					j=j+2;
				}

				i=0;
				fprintf(erg_f,"\n\t ������������:%d",nuzzleCondtion->header_no);
				break;
			}
		case 2:   // ����ʽ
			break;
		case 3:  //�ڶ�ʽ
			break;
		}

		break;
	case UFC: // UFC
		switch (pdi->pass_mode)
		{
		case TCR_MODEL:
			nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
			nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

			if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
				fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
				nuzzleCondtion->header_no=NUZZLE_NUMBER;
			}

			if(0==nuzzleCondtion->header_no){            //лǫ ���� 2012-7-12
				nuzzleCondtion->header_no=1;
				fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
			}

			CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

			//for (i=0;i<nuzzleCondtion->header_no;i++){
			//	nuzzleCondtion->flow_header_top[1+i]=cr_flow->flow_t;
			//	nuzzleCondtion->flow_header_bottom[1+i]=cr_flow->flow_b;
			//	nuzzleCondtion->working_status[1+i]=1;
			//}

			CoolSchOpenFront(nuzzleCondtion,cr_flow);  // add [11/18/2013 лǫ]


			i=0;
			fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);
			break;

		 case PASS_THROUGH_MODEL: //����ͨ��ʽ

			switch(pdi->open_type)
			{
			case OPEN_STATUS_SUC_JET:  //��������
				//����lay �ṹ���м���������Ϣ
				switch(pdi->activeSecType)
				{
				case ACTIVE_SECTION_FRONT:
					switch(pdi->activeFirstJet)
					{
					case OPEN_FROM_FIRST_SLIT:

						nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
						nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.8); //����������ȴ������,ȡ��

						if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
							fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
							nuzzleCondtion->header_no=NUZZLE_NUMBER;
						}

						if(0==nuzzleCondtion->header_no){    //лǫ ���� 2012-7-12
							nuzzleCondtion->header_no=1;
							fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
						}

						CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

	/*					for (i=0;i<nuzzleCondtion->header_no;i++){
							nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
							nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
							nuzzleCondtion->working_status[i]=1;
						}*/
						if (pdi->cr<15)
						{
							CoolSchOpenFront(nuzzleCondtion,cr_flow);  // add [11/18/2013 лǫ]
						}
						else
						{
							CoolSchOpenFrontSlit(nuzzleCondtion,cr_flow);  // add [4/24/2014 лǫ]
						}

						i=0;

						fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);///////One pair of slit header means double slit headers by Zed 

						break;

					case OPEN_FROM_FIRST_JET:

						nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
						nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

						if (nuzzleCondtion->header_no>NUZZLE_NUMBER-1){
							fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
							nuzzleCondtion->header_no=NUZZLE_NUMBER-1;
						}

						if(0==nuzzleCondtion->header_no){            //лǫ ���� 2012-7-12
							nuzzleCondtion->header_no=1;
							fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
						}

						CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

						//for (i=0;i<nuzzleCondtion->header_no;i++){
						//	nuzzleCondtion->flow_header_top[1+i]=cr_flow->flow_t;
						//	nuzzleCondtion->flow_header_bottom[1+i]=cr_flow->flow_b;
						//	nuzzleCondtion->working_status[1+i]=1;
						//}
						if (pdi->cr<15)
						{
							CoolSchOpenFront(nuzzleCondtion,cr_flow);  // add [11/18/2013 лǫ]
						}
						else
						{
							CoolSchOpenFrontSlit(nuzzleCondtion,cr_flow);  // add [4/24/2014 лǫ]
						}


						i=0;
						fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);
						break;
					}

					break;

				case ACTIVE_SECTION_BACK:
					nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
					nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

					if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
						fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
						nuzzleCondtion->header_no=NUZZLE_NUMBER;
					}

					if(0==nuzzleCondtion->header_no){           //лǫ ���� 2012-7-12
						nuzzleCondtion->header_no=1;
						fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
					}

					CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

					if (1==nuzzleCondtion->header_no%3)   // add [11/2/2013 лǫ]
					{

						switch(nuzzleCondtion->header_no)
						{
						case 4:
							for(i=1;i<5;i=i+3) 
							{
								nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
								nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
								nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
								nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i-1]=cr_flow->flow_t;
								nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i-1]=cr_flow->flow_b;
								nuzzleCondtion->working_status[NUZZLE_NUMBER-i-1]=1;
							}
							break;
						case 7:
							for(i=1;i<4;i++) 
							{
								nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
								nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
								nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
							}
							for(i=4;i<8;i=i+3) 
							{
								nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
								nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
								nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
								nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i-1]=cr_flow->flow_t;
								nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i-1]=cr_flow->flow_b;
								nuzzleCondtion->working_status[NUZZLE_NUMBER-i-1]=1;
							}
							break;
						case 10:
							for(i=1;i<7;i++) 
							{
								nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
								nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
								nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
							}
							for(i=7;i<11;i=i+3) 
							{
								nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
								nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
								nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
								nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i-1]=cr_flow->flow_t;
								nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i-1]=cr_flow->flow_b;
								nuzzleCondtion->working_status[NUZZLE_NUMBER-i-1]=1;
							}
							break;
						}
					}
					else
					{
						for (i=1;i<nuzzleCondtion->header_no+1;i++){
							nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
							nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
							nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
						}
					}

					i=0;
					fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);

					break;

				case ACTIVE_SECTION_TWO:    

					nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
					nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

					if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
						fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
						nuzzleCondtion->header_no=NUZZLE_NUMBER;
					}

					if(0==nuzzleCondtion->header_no){             //лǫ ���� 2012-7-12
						nuzzleCondtion->header_no=1;
						fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
					}

					CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

					for (i=1;i<nuzzleCondtion->header_no+1;i++){
						nuzzleCondtion->flow_header_top[NUZZLE_NUMBER-i]=cr_flow->flow_t;
						nuzzleCondtion->flow_header_bottom[NUZZLE_NUMBER-i]=cr_flow->flow_b;
						nuzzleCondtion->working_status[NUZZLE_NUMBER-i]=1;
					}

					i=0;
					fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);

					break;
				}
				break;   //лǫ���� 2012-5-8

			case OPEN_STATUS_SLIT2: //2���϶

				nuzzleCondtion->flow_header_top[0]=cr_flow->gap_flow_t;
				nuzzleCondtion->flow_header_bottom[0]=cr_flow->gap_flow_b;
				nuzzleCondtion->working_status[0]=2;

				nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
				nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

				if(0==nuzzleCondtion->header_no){
					nuzzleCondtion->header_no=1;
					fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
				}

				if (nuzzleCondtion->header_no>NUZZLE_NUMBER-1){
					fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
					nuzzleCondtion->header_no=NUZZLE_NUMBER-1;
					CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);
				}
				else{

					CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);        // ���������ٶ� ��ǰ 2012-5-16
					nuzzleCondtion->header_no=nuzzleCondtion->header_no-1;   // �˴���������ȴ������������� ��ȥslit��

				}										

				for (i=0;i<nuzzleCondtion->header_no;i++){
					nuzzleCondtion->flow_header_top[1+i]=cr_flow->flow_t;
					nuzzleCondtion->flow_header_bottom[1+i]=cr_flow->flow_b;
					nuzzleCondtion->working_status[1+i]=1;
				}
				i=0;

				fprintf(erg_f,"\n\t ������������:%d",1+nuzzleCondtion->header_no);
				nuzzleCondtion->header_no=nuzzleCondtion->header_no+1;   //Ϊ�Ժ�Ĵ���in �ṹ����׼�� лǫ 2012-5-12
				break;

			case OPEN_STATUS_SLIT4: //�����϶����

				//add by xie ��ʱ�����λ�õ���������  9-17//////////////////
				nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
				nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.3); //����������ȴ������,ȡ��

				if (nuzzleCondtion->header_no>NUZZLE_NUMBER-1){
					fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
					nuzzleCondtion->header_no=NUZZLE_NUMBER-1;
				}

				if(0==nuzzleCondtion->header_no) {
					nuzzleCondtion->header_no=1;
					fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
				}

				CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

				for (i=0;i<nuzzleCondtion->header_no;i++){
					nuzzleCondtion->flow_header_top[1+i]=cr_flow->flow_t;
					nuzzleCondtion->flow_header_bottom[1+i]=cr_flow->flow_b;
					nuzzleCondtion->working_status[1+i]=1;
				}
				i=0;
				fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);

				break;
			case OPEN_STATUS_SEP_JET: //ϡ����ȴ
				nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
				nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��
				if (nuzzleCondtion->header_no>NUZZLE_NUMBER/2)  //�Ӹ��ܿ�ʼ�� лǫ 2012-6-25
				{
					fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
					nuzzleCondtion->header_no=4;    //лǫ 2012-5-12
				}

				if(0==nuzzleCondtion->header_no)    //лǫ ���� 2012-7-6
				{
					nuzzleCondtion->header_no=1;
					fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
				}


				CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

				int j=0;
				for (i=0, j=i;i<nuzzleCondtion->header_no;i=i+1)  //л 2012-5-22 �޸Ŀ��շ�ʽ
				{
					nuzzleCondtion->flow_header_top[1+j]=cr_flow->flow_t;
					nuzzleCondtion->flow_header_bottom[1+j]=cr_flow->flow_b;
					nuzzleCondtion->working_status[1+j]=1;
					j=j+2;
				}

				i=0;
				fprintf(erg_f,"\n\t ������������:%d",nuzzleCondtion->header_no);
				break;
			}
		case OSC_IN_MODEL:   // ����ʽ
			break;
		case OSC_OUT_MODEL:  //�ڶ�ʽ
			nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
			nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��
		
			if (nuzzleCondtion->header_no>NUZZLE_NUMBER){
				if (1==nuzzleCondtion->header_no%3)
				{
					nuzzleCondtion->header_no = nuzzleCondtion->header_no + 2;
				}
				else if (2==nuzzleCondtion->header_no%3)
				{
					nuzzleCondtion->header_no = nuzzleCondtion->header_no + 1;
				}
				//coolingpass = 3;						            
			}

			if(0==nuzzleCondtion->header_no){    //лǫ ���� 2012-7-12
				nuzzleCondtion->header_no=1;
				fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
			}

			CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

			nuzzleCondtion->header_no = nuzzleCondtion->header_no /3;
			fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);
			CoolSchOpenFront(nuzzleCondtion,cr_flow); 
			break;
		}
		break;
	case 3: //UFC+ACC
		break;
	case 4: //ACC+UFC
		break;
	case 5: //DQ
		break;

	case BACKACC:
		switch(pdi->open_type)
		{
		case OPEN_STATUS_SUC_JET:  //��������

			nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
			//nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��
			nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.75); //����������ȴ������,ȡ��

			if (nuzzleCondtion->header_no>9) {
				fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
				nuzzleCondtion->header_no=9;
			}

			if(0==nuzzleCondtion->header_no){
				nuzzleCondtion->header_no=1;
				fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
			}

			CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

			if (nuzzleCondtion->header_no<6) {			  
				for (i=0;i<nuzzleCondtion->header_no;i++){
					nuzzleCondtion->flow_header_top[4+i]=cr_flow->flow_t;
					nuzzleCondtion->flow_header_bottom[4+i]=cr_flow->flow_b;
					nuzzleCondtion->working_status[4+i]=1;
				}
			}
			else{
				for (i=0;i<5;i++){
					nuzzleCondtion->flow_header_top[4+i]=cr_flow->flow_t;
					nuzzleCondtion->flow_header_bottom[4+i]=cr_flow->flow_b;
					nuzzleCondtion->working_status[4+i]=1;
				}
				for (i=0;i<nuzzleCondtion->header_no-5;i++){
					nuzzleCondtion->flow_header_top[3-i]=cr_flow->flow_t;
					nuzzleCondtion->flow_header_bottom[3-i]=cr_flow->flow_b;
					nuzzleCondtion->working_status[3-i]=1;
				}
			}

			i=0;
			fprintf(erg_f,"\n\t ������������:  %d",nuzzleCondtion->header_no);

			break;

		case OPEN_STATUS_SEP_JET: //ϡ����ȴ

			nuzzleCondtion->header_no_modify_speed=(cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen;
			nuzzleCondtion->header_no=(int)((cr_flow->time_c*lay->initSpeed)/lay->CoolBankLen+0.5); //����������ȴ������,ȡ��

			if (nuzzleCondtion->header_no>3){  //�Ӹ��ܿ�ʼ�� лǫ 2012-6-25
				fprintf(erg_f,"\n \t ����Ŀ����������䳬��  %d",nuzzleCondtion->header_no);
				nuzzleCondtion->header_no=3;    //лǫ 2012-5-12
			}

			if(0==nuzzleCondtion->header_no) {   //лǫ ���� 2012-7-6
				nuzzleCondtion->header_no=1;
				fprintf(erg_f,"\n \t ����Ŀ�����������Ϊ�� ����Ϊ1");
			}

			CoolSchSpeedModify(erg_f,nuzzleCondtion,lay);

			int j=0;
			for (i=0;i<nuzzleCondtion->header_no;i=i+1) {  //л 2012-5-22 �޸Ŀ��շ�ʽ
				nuzzleCondtion->flow_header_top[4+j]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[4+j]=cr_flow->flow_b;
				nuzzleCondtion->working_status[4+j]=1;
				j=j+2;
			}
			j=0;
			i=0;
			fprintf(erg_f,"\n\t ������������:%d",nuzzleCondtion->header_no);
			break;
		}
		break;
	}

	fprintf(erg_f,"\n\t ���ܿ���״̬:");
	for (i=0;i<NUZZLE_NUMBER;i++)
	{
		fprintf(erg_f,"\n\t �� %d �鼯��: %d \t ",i+1,nuzzleCondtion->working_status[i]);
	}

	return 0;
}

int CPreProcess::PreDefineTechPara(RALINDATA *ralInPut, PHYSICAL *ver_par1)
{
	float acc_ufc_cool_ability_coe;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////// ��ȴģʽ�ж�    ///////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	acc_ufc_cool_ability_coe = 1/1.6;

	ralInPut->pass_mode = PASS_THROUGH_MODEL;

	if(BACKACC==ralInPut->acc_mode){   //лǫ 2012-7-12 BACKACCģʽ ֻ��ͨ��ʽ
		ralInPut->open_type = OPEN_STATUS_SUC_JET;
	}
		
	if(ralInPut->acc_mode == ACC){
		ralInPut->pass_mode = PASS_THROUGH_MODEL;//ͨ��ʽ
		
		//if ((ralInPut->thick*1000>=70))
		//{
		//	ralInPut->pass_mode = OSC_OUT_MODEL;  // ���� [3/21/2014 лǫ]
		//	//ralInPut->pass_mode = PASS_THROUGH_MODEL;//ͨ��ʽ
		//}

	}

	if(ralInPut->acc_mode == UFC){
		//if((strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0)){

		//	if((ralInPut->finishRollTemp-ralInPut->targetFinishCoolTemp)/ralInPut->cr>L_EQUIPMENT/SPD_MIN){
		//		if(ralInPut->length>L_OSC_MAX) 
		//			ralInPut->pass_mode = OSC_OUT_MODEL;//����ʽ
		//		else 
		//			ralInPut->pass_mode = OSC_IN_MODEL;      //�ڶ�ʽ

		//	}else{
		//		ralInPut->pass_mode = PASS_THROUGH_MODEL;//ͨ��ʽ
		//	}

		//}else {
		//	ralInPut->pass_mode = PASS_THROUGH_MODEL;//ͨ��ʽ
		//}
		if ((ralInPut->thick*1000>=80)&&(ralInPut->finishRollTemp-ralInPut->targetFinishCoolTemp>=150))
		{

			//ralInPut->pass_mode = OSC_OUT_MODEL;  // ���� [3/21/2014 лǫ]
			ralInPut->pass_mode = PASS_THROUGH_MODEL;//ͨ��ʽ
		}
		else
		{
		  
			ralInPut->pass_mode = PASS_THROUGH_MODEL;//ͨ��ʽ
		}
	}

	if(ralInPut->acc_mode == DQ){
		if((strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0)){

			if((ralInPut->finishRollTemp-ralInPut->targetFinishCoolTemp)/ralInPut->cr>L_EQUIPMENT/SPD_MIN){
				if(ralInPut->length>L_OSC_MAX) 
					ralInPut->pass_mode = OSC_OUT_MODEL;//����ʽ
				else 
					ralInPut->pass_mode = OSC_IN_MODEL;      //�ڶ�ʽ
			}
			else{
				ralInPut->pass_mode = PASS_THROUGH_MODEL;//ͨ��ʽ
			}
		}else{
			ralInPut->pass_mode = PASS_THROUGH_MODEL;//ͨ��ʽ
		}
	}


	if (1==ralInPut->PltIcFlgTemp)
	{
		ralInPut->pass_mode = TCR_MODEL;//�м�����ȴ  // add [3/19/2014 лǫ]
	}
	

	ThkCrMulPara(ralInPut,ver_par1);
	//if (( ACC == ralInPut->acc_mode)||(UFC == ralInPut->acc_mode))
	//{
	//	ralInPut->open_type=  OPEN_STATUS_SUC_JET;  //add by xie 9-17 Ĭ�������϶����
	//}

	ralInPut->open_type=  OPEN_STATUS_SUC_JET;
	//if ( UFC == ralInPut->acc_mode){
	//	ralInPut->open_type = OPEN_STATUS_SLIT2;

	//	if ((ralInPut->cr<ver_par1->SlitTwoCrMax)&&(ralInPut->cr>ver_par1->SlitTwoCrMin)){
	//		ralInPut->open_type = OPEN_STATUS_SLIT2;
	//	} 
	//	else if(ralInPut->cr>ver_par1->SlitTwoCrMax){
	//		ralInPut->open_type=  OPEN_STATUS_SLIT2;
	//	}
	//	if ((ralInPut->cr<ver_par1->SlitTwoCrMin)&&(ralInPut->cr>ver_par1->JetSucCrMin)){
	//		ralInPut->open_type = OPEN_STATUS_SUC_JET;
	//	} 
	//	else if(ralInPut->cr<ver_par1->JetSucCrMin){
	//		ralInPut->open_type = OPEN_STATUS_SEP_JET;
	//	}
	//}

	//if ( DQ == ralInPut->acc_mode){
	//	ralInPut->open_type = OPEN_STATUS_SLIT4;

	//	if ((ralInPut->cr<ver_par1->SlitFourCrMax)&&(ralInPut->cr>ver_par1->SlitFourCrMin)){
	//		ralInPut->open_type=  OPEN_STATUS_SLIT2;
	//	} 
	//	else if((ralInPut->cr<ver_par1->SlitFourCrMin)&&(ralInPut->cr>ver_par1->SlitTwoCrMin)){
	//		ralInPut->open_type = OPEN_STATUS_SLIT2;
	//	}
	//	else if ((ralInPut->cr<ver_par1->SlitTwoCrMin)&&(ralInPut->cr>ver_par1->JetSucCrMin)){
	//		ralInPut->open_type = OPEN_STATUS_SUC_JET;
	//	} 
	//	else if(ralInPut->cr<ver_par1->JetSucCrMin){
	//		ralInPut->open_type = OPEN_STATUS_SEP_JET;
	//	}
	//}

	//���½��ڿ�������ʱʹ��
	ralInPut->activeSecType = ACTIVE_SECTION_FRONT;

	//if((strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0))
	if((strcmp(ralInPut->s_slab_code,"")==0)||(strcmp(ralInPut->s_slab_code,"")==0)||(strcmp(ralInPut->s_slab_code,"")==0))//modified by Zed
	{
		if((OPEN_STATUS_SUC_JET == ralInPut->open_type)||(OPEN_STATUS_SEP_JET == ralInPut->open_type)){
			ralInPut->activeSecType = ACTIVE_SECTION_BACK;
		}
	}
	//else if((strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0))
	else if((strcmp(ralInPut->s_slab_code,"")==0)||(strcmp(ralInPut->s_slab_code,"")==0)||(strcmp(ralInPut->s_slab_code,"")==0))//modified by Zed
	{
		ralInPut->activeSecType = ACTIVE_SECTION_TWO;
	}
	else{
		ralInPut->activeSecType = ACTIVE_SECTION_FRONT;
	}


	//ralInPut->activeSecType = ACTIVE_SECTION_BACK;/////////////////Temporary add by Zed 2013/10/24
	ralInPut->activeSecType = ACTIVE_SECTION_FRONT;


	//�������������ڷ�϶������Ϊ���ܼ���ʹ��ʱ���жϣ�����������֣�������ü���������������ģʽ��ǰ�ο�������ʱ���Բ��÷�϶������Ϊ���ܿ��伯�ܿ���ʹ�á�

	ralInPut->activeFirstJet = OPEN_FROM_FIRST_JET;

	//if((strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0)||(strcmp(ralInPut->s_steel_code,"")==0)){
	//	if(((OPEN_STATUS_SUC_JET == ralInPut->open_type)||(OPEN_STATUS_SEP_JET == ralInPut->open_type))&&((ACTIVE_SECTION_TWO == ralInPut->activeSecType)||(ACTIVE_SECTION_FRONT == ralInPut->activeSecType)))
	//		ralInPut->activeFirstJet = OPEN_FROM_FIRST_SLIT;
	//}else{
	//	ralInPut->activeFirstJet = OPEN_FROM_FIRST_SLIT;  //modiffy by xie9-19
	//}

	return 1;
}

void CPreProcess::ThkCrMulPara(RALINDATA *ralInPut,PHYSICAL *ver_par1)
{
	int i=0;
	float fThick[10];
	float fSlitFourCrMax[10];
	float fSlitFourCrMin[10];
	float fSlitTwoCrMax[10];
	float fSlitTwoCrMin[10];
	float fJetSucCrMax[10];
	float fJetSucCrMin[10];
	float fJetSepCrMax[10];
	float fJetSepCrMin[10];

	for (i=0;i<10;i++){
		fSlitFourCrMax[i] = 2;
		fSlitFourCrMin[i] = 1;
		fSlitTwoCrMax[i] = 2;
		fSlitTwoCrMin[i] = 1;
		fJetSucCrMax[i] = 2;
		fJetSucCrMin[i] = 1;
		fJetSepCrMax[i] = 2;
		fJetSepCrMin[i] = 1;
	}

	ver_par1->SlitFourCrMax = 2;
	ver_par1->SlitFourCrMin = 1;
	ver_par1->SlitTwoCrMax = 2;
	ver_par1->SlitTwoCrMin = 1;
	ver_par1->JetSucCrMax = 2;
	ver_par1->JetSucCrMin = 1;
	ver_par1->JetSepCrMax = 2;
	ver_par1->JetSepCrMin = 1;

	//define 
	fThick[0] = 5.0f;//5mm
	fThick[1] = 10.0f;//10mm
	fThick[2] = 20.0f;//20mm
	fThick[3] = 30.0f;//30mm
	fThick[4] = 40.0f;//40mm
	fThick[5] = 50.0f;//50mm
	fThick[6] = 60.0f;//60mm
	fThick[7] = 70.0f;//70mm
	fThick[8] = 80.0f;//80mm
	fThick[9] = 200.0f;//200mm

	fSlitFourCrMax[0] = 300.0f;//5mm
	fSlitFourCrMax[1] = 145.0f;//10mm
	fSlitFourCrMax[2] = 63.0f;//20mm
	fSlitFourCrMax[3] = 37.8f;//30mm
	fSlitFourCrMax[4] = 26.0f;//40mm
	fSlitFourCrMax[5] = 18.75f;//50mm
	fSlitFourCrMax[6] = 14.4f;//60mm
	fSlitFourCrMax[7] = 11.5f;//70mm
	fSlitFourCrMax[8] = 8.8f;//80mm
	fSlitFourCrMax[9] = 2.1f;//200mm


	fSlitFourCrMin[0] = 90.0f;//5mm
	fSlitFourCrMin[1] = 43.5f;//10mm
	fSlitFourCrMin[2] = 18.9f;//20mm
	fSlitFourCrMin[3] = 11.34f;//30mm
	fSlitFourCrMin[4] = 7.8f;//40mm
	fSlitFourCrMin[5] = 5.625f;//50mm
	fSlitFourCrMin[6] = 4.32f;//60mm
	fSlitFourCrMin[7] = 3.45f;//70mm
	fSlitFourCrMin[8] = 2.64f;//80mm
	fSlitFourCrMin[9] = 0.63f;//200mm



	fSlitTwoCrMax[0] = 260.0f;//5mm
	fSlitTwoCrMax[1] = 127.0f;//10mm
	fSlitTwoCrMax[2] = 55.8f;//20mm
	fSlitTwoCrMax[3] = 33.9f;//30mm
	fSlitTwoCrMax[4] = 23.6f;//40mm
	fSlitTwoCrMax[5] = 17.25f;//50mm
	fSlitTwoCrMax[6] = 13.44f;//60mm
	fSlitTwoCrMax[7] = 10.9f;//70mm
	fSlitTwoCrMax[8] = 8.48f;//80mm
	fSlitTwoCrMax[9] = 2.06f;//200mm


	fSlitTwoCrMin[0] = 78.0f;//5mm
	fSlitTwoCrMin[1] = 38.1f;//10mm
	fSlitTwoCrMin[2] = 16.74f;//20mm
	fSlitTwoCrMin[3] = 10.2f;//30mm
	fSlitTwoCrMin[4] = 7.08f;//40mm
	fSlitTwoCrMin[5] = 5.18f;//50mm
	fSlitTwoCrMin[6] = 4.03f;//60mm
	fSlitTwoCrMin[7] = 3.27f;//70mm
	fSlitTwoCrMin[8] = 2.54f;//80mm
	fSlitTwoCrMin[9] = 0.618f;//200mm



	fJetSucCrMax[0] = 200.0f;//5mm
	fJetSucCrMax[1] = 100.0f;//10mm
	fJetSucCrMax[2] = 45.0f;//20mm
	fJetSucCrMax[3] = 28.0f;//30mm
	fJetSucCrMax[4] = 20.0f;//40mm
	fJetSucCrMax[5] = 15.0f;//50mm
	fJetSucCrMax[6] = 12.0f;//60mm
	fJetSucCrMax[7] = 10.0f;//70mm
	fJetSucCrMax[8] = 8.0f;//80mm
	fJetSucCrMax[9] = 2.0f;//200mm


	fJetSucCrMin[0] = 60.0f;//5mm
	fJetSucCrMin[1] = 30.0f;//10mm
	fJetSucCrMin[2] = 13.5f;//20mm
	fJetSucCrMin[3] = 8.4f;//30mm
	fJetSucCrMin[4] = 6.0f;//40mm
	fJetSucCrMin[5] = 4.5f;//50mm
	fJetSucCrMin[6] = 3.6f;//60mm
	fJetSucCrMin[7] = 3.0f;//70mm
	fJetSucCrMin[8] = 2.4f;//80mm
	fJetSucCrMin[9] = 0.6f;//200mm

	fJetSepCrMax[0] = 120.0f;//5mm
	fJetSepCrMax[1] = 60.0f;//10mm
	fJetSepCrMax[2] = 27.0f;//20mm
	fJetSepCrMax[3] = 16.8f;//30mm
	fJetSepCrMax[4] = 12.0f;//40mm
	fJetSepCrMax[5] = 9.0f;//50mm
	fJetSepCrMax[6] = 7.2f;//60mm
	fJetSepCrMax[7] = 6.0f;//70mm
	fJetSepCrMax[8] = 4.8f;//80mm
	fJetSepCrMax[9] = 1.2f;//200mm

	fJetSepCrMin[0] = 36.0f;//5mm
	fJetSepCrMin[1] = 18.0f;//10mm
	fJetSepCrMin[2] = 8.1f;//20mm
	fJetSepCrMin[3] = 5.0f;//30mm
	fJetSepCrMin[4] = 3.6f;//40mm
	fJetSepCrMin[5] = 2.7f;//50mm
	fJetSepCrMin[6] = 2.2f;//60mm
	fJetSepCrMin[7] = 1.8f;//70mm
	fJetSepCrMin[8] = 1.44f;//80mm
	fJetSepCrMin[9] = 0.36f;//200mm

	ver_par1->SlitFourCrMax =GetLinearInterpolationFromAbscissa(fThick, fSlitFourCrMax, 10,ralInPut->thick*1000.0f);
	ver_par1->SlitFourCrMin =GetLinearInterpolationFromAbscissa(fThick, fSlitFourCrMin, 10,ralInPut->thick*1000.0f);
	ver_par1->SlitTwoCrMax = GetLinearInterpolationFromAbscissa(fThick, fSlitTwoCrMax, 10,ralInPut->thick*1000.0f);
	ver_par1->SlitTwoCrMin =GetLinearInterpolationFromAbscissa(fThick, fSlitTwoCrMin, 10,ralInPut->thick*1000.0f);
	ver_par1->JetSucCrMax =GetLinearInterpolationFromAbscissa(fThick, fJetSucCrMax, 10,ralInPut->thick*1000.0f);
	ver_par1->JetSucCrMin = GetLinearInterpolationFromAbscissa(fThick, fJetSucCrMin, 10,ralInPut->thick*1000.0f);
	ver_par1->JetSepCrMax = GetLinearInterpolationFromAbscissa(fThick, fJetSepCrMax, 10,ralInPut->thick*1000.0f);
	ver_par1->JetSepCrMin = GetLinearInterpolationFromAbscissa(fThick, fJetSepCrMin, 10,ralInPut->thick*1000.0f);

	return;

}

void CPreProcess::CoolSchSpeedModify(FILE *erg_f, NUZZLECONDITION *nuzzleCondtion, RALCOEFFDATA *lay)
{
	fprintf(erg_f,"\n \t ԭ�ٶ�:  %f",lay->initSpeed);
	lay->initSpeed=(nuzzleCondtion->header_no/nuzzleCondtion->header_no_modify_speed)*lay->initSpeed;   //ԭ�ٶ�������ʽ���ܴ��� лǫ 2012-6-25

	if (lay->initSpeed>2){
		if (nuzzleCondtion->TempThick>0.025){
			fprintf(erg_f,"\n \t �����Ĺ��ٳ���  %d,����Ϊ 1.2",lay->initSpeed);
			lay->initSpeed=1.2f;
		}
		else{
			fprintf(erg_f,"\n \t �����Ĺ��ٳ���  %d,����Ϊ 2",lay->initSpeed);
			lay->initSpeed=2;
		}
	} 
	else if(lay->initSpeed<0.5){
		fprintf(erg_f,"\n \t �����Ĺ��ٳ���  %d,����Ϊ 0.5",lay->initSpeed);
		lay->initSpeed=0.5f;
	}
	fprintf(erg_f,"\n \t ��������ٶ�:  %f",lay->initSpeed);
	return;
}

float CPreProcess::InitWaterCoolxq(    COMPUTATIONAL *compx,           // лǫ ���� ���ڱ�������Temperature
	float  *end_temp_air, /* temp. distr. air cool. */
	float  *end_aust_air, /* aost �ֽ�    */
	const	INPUTDATAPDI *pdi,
	PHYSICAL *ax,
	float detaTemp)
{
	float real_temp;
	int	iv;

	real_temp = pdi->finishRollTemp;  /* finishRollTemp is the Temperature on ENTRY_ACC */
	real_temp=compx->tx[compx->computedNumber-1]+detaTemp;
	/* in SSAB-installation */
	iv = -1;
	while(++iv < NODES_NUMBER){
		end_temp_air[iv]=compx->startTempDistribution[iv]+detaTemp;
		end_aust_air[iv]=compx->startAusteniteDistribution[iv];
	}

	//real_temp = compx->targetFinishCoolTemp + temp_diff;	/*	����ed cal.	*/
	/*	mean Temperature */
	ax->sw_air = 1;
	ax->coolPhaseNumber = 1;
	ax->art[1] = 1;	/* ˮ�� */
	ax->duration[1] = 0.0f;
	compx->computeID = 2; 
	return(real_temp);
}

int CPreProcess::CalcTempCurvePost(
	FILE 			*st_err,
	FILE 		*erg_f ,
	INPUTDATAPDI *pdiInPut,
	const	  RALCOEFFDATA *lay,		/* layout data      */
	const      RALOUTDATA *preOut,   // лǫ���� 2012-5-16
	PHYSICAL *ver_par1,
	COMPUTATIONAL *compx)
{
	MATERIAL matal;

	int i=0;
	int j=0;
	int k=0;

	//��ʼ�� matal,��
	int iz = -1;

	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);

	compx->thickNodeNumber=NODES_NUMBER;
	compx->matrixElementNumber = 2 * compx->thickNodeNumber - 1;

	ver_par1->coolPhaseNumber=0;
	EstimationComputationalParam(erg_f,compx, pdiInPut, ver_par1);


	while(++iz < 19)  //��ʼTemperature�ڵ�
	{
		compx->startTempDistribution[iz] = preOut->PreResult.airEndTemp[iz];
		compx->startAusteniteDistribution[iz] = 100.f;
	}

	for (i=preOut->PreResult.iHeaderFirst;i<=preOut->PreResult.iHeaderLast;i++)   //ѭ������Ӧ��Ϊ���ܵķֶ�������
	{
		if (1==preOut->headerWorkingStatus[i])  //���ܿ���
		{
			compx->detaTime=0.2f;    //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ
			ver_par1->topFlowRate=preOut->waterFlowTop[i];
			ver_par1->bottomFlowRate=preOut->waterFlowDown[i];

			ver_par1->sw_air = 1;
			ver_par1->coolPhaseNumber = 1;
			ver_par1->art[1] = 1;	/* ˮ�� */
			ver_par1->duration[1] = 0.0f;
			compx->computeID = 2; 

			compx->calculationStopTime=lay->CoolBankLen/lay->initSpeed;

			pdiInPut->working_stage=1;              //jet�λ���ϵ����ȡ��־λ лǫ 2012-5-8

			m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);
		} 
		else if (0==preOut->headerWorkingStatus[i]) //���ܹر� ����
		{
			compx->detaTime=0.5;              //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ
			//ȷ�������������˴�Ϊȷ������Temperature�ֲ�����ȷ��
			compx->computeID = 2;	/*	const Temperature distribution	*/
			ver_par1->coolPhaseNumber = 0;
			ver_par1->method = 1;	/* ����	*/
			ver_par1->art[0] = 0;
			ver_par1->duration[0] = 0.0f;
			ver_par1->taskID = PREPROCESS;
			ver_par1->sw_air = 0;	/*���� */

			compx->femCalculationOutput = 0;
			compx->integra = 2;
			//compx->detaTime       = pdiInPut->thick * 333.f;
			compx->isSymetric     = 0;    /* nonsymmetric problem */
			/* calculation of element sizes compx->lm[]	*/
			CalculationElementSizes(compx, 2, pdiInPut->thick);

			compx->matrixElementNumber = 2 * compx->thickNodeNumber - 1;
			compx->calculationStopTime=lay->CoolBankLen/lay->initSpeed;

			pdiInPut->working_stage=0;   

			m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);
		}			
		else if (2==preOut->headerWorkingStatus[i])  //���ܿ���  slit�� лǫ
		{
			compx->detaTime=0.2f;    //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ

			ver_par1->topFlowRate=preOut->waterFlowTop[i];
			ver_par1->bottomFlowRate=preOut->waterFlowDown[i];

			ver_par1->sw_air = 1;
			ver_par1->coolPhaseNumber = 1;
			ver_par1->art[1] = 1;	/* ˮ�� */
			ver_par1->duration[1] = 0.0f;
			compx->computeID = 2; 
			compx->calculationStopTime=lay->CoolBankLen/lay->initSpeed;

			pdiInPut->working_stage=2;              //slit�λ���ϵ����ȡ��־λ лǫ 2012-5-8

			m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);
		} 

		// �洢ÿһ�� �����Temperature���� 
		for(j=0;j<compx->computedNumber;j++){
			if (0==k)
				compx->zx_temple[k] =compx->zx[j]	;  
			else
				compx->zx_temple[k]=compx->zx_temple[k-1]+compx->zx[j]-compx->zx[j-1]	;

			compx->tx_temple[k]=compx->tx[j];        /* caloric mean Temperature value last curve       */
			compx->tx_cen_temple[k]=compx->tx_cen[j];    /* Temperature in the center line of ���          */
			compx->tx_top_temple[k]=compx->tx_top[j];    /* surface top Temperature value last curve        */
			compx->tx_bot_temple[k]=compx->tx_bot[j];    /* surface bottom temp. value last curve           */
			compx->cx_temple[k]=compx->cx[j];        /* ��ȴ�ٶ�in C                                */
			k++;
		}
	}

	compx->computeID = 2;	/*	const Temperature distribution	*/
	ver_par1->coolPhaseNumber = 0;
	ver_par1->method = 1;	/* ����	*/
	ver_par1->art[0] = 0;
	ver_par1->duration[0] = 0.0f;
	ver_par1->taskID = PREPROCESS;
	ver_par1->sw_air = 0;	/*���� */
	compx->femCalculationOutput = 0; 
	compx->integra = 2;
	compx->isSymetric     = 0;    /* nonsymmetric problem */
	/* calculation of element sizes compx->lm[]	*/
	CalculationElementSizes(compx, 2, pdiInPut->thick);
	compx->matrixElementNumber = 2 * compx->thickNodeNumber - 1;
	compx->calculationStopTime=15/(lay->initSpeed+0.1f);
	m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
	m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);

	compx->calculationStopTemp=compx->tx[compx->computedNumber-1];  //лǫ 2012-9-14
	compx->calculationStopTime=compx->zx_temple[k-1];   //лǫ ����������ȴtime 2012-5-17

	return 0;
}

void CPreProcess::PlateHeadAirTempDrop
	(
	FILE          *st_err,    /* �쳣���� */
	FILE          *erg_f,     /* ��Ľ��FILE */
	INPUTDATAPDI *pdiInPut,
	COMPUTATIONAL *compx,
	PRERESULT *PreResult,       /* Ԥ������    */
	const	  RALCOEFFDATA *lay,		/* layout data      */
	PHYSICAL *ver_par1,  /* ��ȴ���� */
	MATERIAL *matal
	)
{
	int i=0;
	///////////////////////////////////////////////////////////////////////////////////////////////
	//if(1==��ԥtime���� && 1==Ԥ��ֱ������־λ)
	if( (pdiInPut->PltDlyTimRol>0) && (1==pdiInPut->PltPlActFlg) )  //��ԥ+Ԥ��ֱ��
	{
		compx->calculationStopTime=pdiInPut->PltDlyTimRol;  //��ǰ��ԥtime лǫ 2012-5-18
		compx->detaTime=1;
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, compx, ver_par1, matal);
		fprintf(erg_f,"\n \t �趨��ԥtime��%f",pdiInPut->PltDlyTimRol);
		fprintf(erg_f,"\n \t �趨��ԥtime��Temperature����");
		fprintf(erg_f,"\n \t time     ƽ���¶�     �ϱ����¶�   �Ĳ��¶�       ��ȴ�ٶ�      \n");
		for (i=0;i<compx->computedNumber;i++)
			fprintf(erg_f,"\t %f      %f       %f       %f       %f       \n",compx->zx[i],compx->tx[i],compx->tx_top[i],compx->tx_cen[i],compx->cx[i]);

		compx->calculationStopTime=LENGTH_PRELEVEL/lay->initSpeed;       // ���ٶ�ӦΪ(in_put->pl_speed+lay->initSpeed)/2
		pdiInPut->working_stage=1;
		m_TemperatureField.SolveBoundary(st_err, ver_par1, matal, pdiInPut);     // SolveBoundary,Ԥ��ֱ����ȴ����    
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, compx, ver_par1, matal);
		fprintf(erg_f,"\n \t �趨Ԥ��ֱtime��5");
		fprintf(erg_f,"\n \t �趨Ԥ��ֱtime��Temperature����");
		fprintf(erg_f,"\n \t time     ƽ���¶�     �ϱ����¶�   �Ĳ��¶�       ��ȴ�ٶ�      \n");
		for (i=0;i<compx->computedNumber;i++)
			fprintf(erg_f,"\t %f      %f       %f       %f       %f       \n",compx->zx[i],compx->tx[i],compx->tx_top[i],compx->tx_cen[i],compx->cx[i]);
	}
	else if (pdiInPut->PltDlyTimRol>0)//������ԥ
	{
		compx->calculationStopTime=pdiInPut->PltDlyTimRol;  //��ǰ��ԥtime лǫ 2012-5-18
		compx->detaTime=1;
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, compx, ver_par1, matal);
		fprintf(erg_f,"\n \t �趨��ԥtime��%f",pdiInPut->PltDlyTimRol);
		fprintf(erg_f,"\n \t �趨��ԥtime��Temperature����");
		fprintf(erg_f,"\n \t time     ƽ���¶�     �ϱ����¶�   �Ĳ��¶�       ��ȴ�ٶ�      \n");
		for (i=0;i<compx->computedNumber;i++)
			fprintf(erg_f,"\t %f      %f       %f       %f       %f       \n",compx->zx[i],compx->tx[i],compx->tx_top[i],compx->tx_cen[i],compx->cx[i]);
	}
	else if (1==pdiInPut->PltPlActFlg)//����Ԥ��ֱ��
	{
		//speed ͨ����value��ȡFILE��õ��ٶȡ�
		compx->calculationStopTime=DISTANCE_MILL_TO_COOL/2;   //Ӧ �����׸��ٶ� лǫ 2012-6-1
		compx->detaTime=1;
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, compx, ver_par1, matal);
		fprintf(erg_f,"\n \t ����time��%f",pdiInPut->PltDlyTimRol);
		fprintf(erg_f,"\n \t �ְ�������Ԥ��ֱ��Temperature����");
		fprintf(erg_f,"\n \t time     ƽ���¶�     �ϱ����¶�   �Ĳ��¶�       ��ȴ�ٶ�      \n");
		for (i=0;i<compx->computedNumber;i++)
			fprintf(erg_f,"\t %f %f       %f       %f       %f       \n",compx->zx[i],compx->tx[i],compx->tx_top[i],compx->tx_cen[i],compx->cx[i]);

		compx->calculationStopTime=LENGTH_PRELEVEL/lay->initSpeed;       // ���ٶ�ӦΪ(in_put->pl_speed+lay->initSpeed)/2
		compx->detaTime=1;
		m_TemperatureField.SolveBoundary(st_err, ver_par1, matal, pdiInPut);     /* SolveBoundary     */
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, compx, ver_par1, matal);

		fprintf(erg_f,"\n \t �趨Ԥ��ֱtime��Temperature����");
		fprintf(erg_f,"\n \t time     ƽ���¶�     �ϱ����¶�   �Ĳ��¶�       ��ȴ�ٶ�      \n");
		for (i=0;i<compx->computedNumber;i++)
			fprintf(erg_f,"\t %f      %f       %f       %f       %f       \n",compx->zx[i],compx->tx[i],compx->tx_top[i],compx->tx_cen[i],compx->cx[i]);
	}
	else
	{
		//speed ͨ����value��ȡFILE��õ��ٶȡ�
		compx->calculationStopTime=DISTANCE_MILL_TO_COOL/1.5f;   //Ӧ �����׸��ٶ� лǫ 2012-6-1
		compx->detaTime=1;
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, compx, ver_par1, matal);
		fprintf(erg_f,"\n \t �ְ���������ȴ�����¶�����");
		fprintf(erg_f,"\n \t time     ƽ���¶�     �ϱ����¶�   �Ĳ��¶�       ��ȴ�ٶ�      \n");
		for (i=0;i<compx->computedNumber;i++)
			fprintf(erg_f,"\t %f      %f       %f       %f       %f       \n",compx->zx[i],compx->tx[i],compx->tx_top[i],compx->tx_cen[i],compx->cx[i]);
	}

	PreResult->afterAirCoolTemp=compx->tx[compx->computedNumber-1];
	PreResult->airTempDrop=PreResult->beforeAirCoolTemp-PreResult->afterAirCoolTemp;         //лǫ���� ���ڱ�������½� 2012-5-18
}

void CPreProcess::PlateTailAirTempDrop
	(
	FILE          *st_err,    /* �쳣���� */
	FILE          *erg_f,     /* ��Ľ��FILE */
	INPUTDATAPDI *pdiInPut,
	COMPUTATIONAL *compx,
	PRERESULT *PreResult,       /* Ԥ������    */
	const	  RALCOEFFDATA *lay,		/* layout data      */
	PHYSICAL *ver_par1,  /* ��ȴ���� */
	MATERIAL *matal
	)
{
	COMPUTATIONAL newCompx;
	newCompx=*compx;

	float plateTailTime=0.0;
	int i=0;
	plateTailTime=pdiInPut->length/lay->initSpeed;

	//if(1==��ԥtime���� && 1==Ԥ��ֱ������־λ)
	if( (pdiInPut->PltDlyTimRol>0) && (1==pdiInPut->PltPlActFlg) ){
		newCompx.calculationStopTime=pdiInPut->PltDlyTimRol+plateTailTime;  //��ǰ��ԥtime лǫ 2012-5-18
		newCompx.detaTime=1;
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, &newCompx, ver_par1, matal);

		newCompx.calculationStopTime=LENGTH_PRELEVEL/lay->initSpeed;       // Ԥ��ֱtime лǫ 2012-5-18  2012-5-22
		pdiInPut->working_stage=1;
		m_TemperatureField.SolveBoundary(st_err, ver_par1, matal, pdiInPut);     // SolveBoundary     
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, &newCompx, ver_par1, matal);
	}
	else if (pdiInPut->PltDlyTimRol>0){
		newCompx.calculationStopTime=pdiInPut->PltDlyTimRol+plateTailTime;  //��ǰ��ԥtime лǫ 2012-5-18
		newCompx.detaTime=1;
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, &newCompx, ver_par1, matal);
	}
	else if (1==pdiInPut->PltPlActFlg)
	{
		//speed ͨ����value��ȡFILE��õ��ٶȡ�
		newCompx.calculationStopTime=DISTANCE_MILL_TO_COOL/lay->initSpeed;  //лǫ 2012-5-20  ������Ԥ��ֱ��time
		newCompx.detaTime=1;
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, &newCompx, ver_par1, matal);
		newCompx.calculationStopTime=LENGTH_PRELEVEL/lay->initSpeed;
		newCompx.detaTime=1;
		m_TemperatureField.SolveBoundary(st_err, ver_par1, matal, pdiInPut);     /* SolveBoundary     */
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, &newCompx, ver_par1, matal);
	}else{
		newCompx.calculationStopTime=DISTANCE_MILL_TO_COOL/lay->initSpeed;
		newCompx.detaTime=1;
		m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, &newCompx, ver_par1, matal);
	}

	PreResult->afterAirCoolTemp=newCompx.tx[newCompx.computedNumber-1];
	PreResult->airTailTempDrop=PreResult->beforeAirCoolTemp-PreResult->afterAirCoolTemp;

	return;
}

int  CPreProcess::BackAccOpenTypeDecide
	(
	FILE          *st_err,    /* �쳣���� */
	FILE          *erg_f,     /* ��Ľ��FILE */
	INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
	const	  RALCOEFFDATA *lay,		/* layout data      */
	const	  STRATEGY *accStrat,	/* ���Բ��� */
	PRERESULT *PreResult,        /* Ԥ������    */
	PHYSICAL    *ver_par1,  /* ��ȴ���� */
	flow_cr       *cr_flow, /* ������Ϣ   */
	COMPUTATIONAL *compx,
	ADAPTATION adapt1,
	float               real_temp,
	float   *end_temp_air,
	float   *end_aust_air
	)
{
	float      temp_diff;
	float      crCorrect=0.0;
	MATERIAL matal;
	int it=0;
	int reachProperCR=0;
	int CR_cycle_count=0;
	float activeJetSepSucCoe = 1.75;
	int tStage=0;

	while ((0==reachProperCR)  &&   CR_cycle_count<2  ){ 
		switch(pdiInPut->open_type)
		{
		case OPEN_STATUS_SUC_JET://������ȴ
			//�˴����������ֹͣ����
			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
			pdiInPut->working_stage=1;   // лǫ���� �����ж� jet or slit
			CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++)
				cr_flow->cr_rate[it] = temp_diff / cr_flow->zeit_stop[it];

			//���ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);
			reachProperCR=1;
			break;

		case OPEN_STATUS_SEP_JET: //ϡ����ȴ

			//�˴����������ֹͣ����
			matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
			CalcDifferentWaterFlow(st_err, erg_f, pdiInPut, &matal, ver_par1,
				compx, cr_flow, real_temp, 
				end_temp_air, end_aust_air);

			temp_diff = real_temp - pdiInPut->targetFinishCoolTemp;

			//���㲻ͬ����ʱ����ȴ����
			for (it = 0; it < cr_flow->anz_flow; it++){
				cr_flow->cr_rate[it] = temp_diff / (cr_flow->zeit_stop[it]*activeJetSepSucCoe);
			}

			if(pdiInPut->cr>cr_flow->cr_rate[cr_flow->anz_flow-1]){
				pdiInPut->open_type = OPEN_STATUS_SUC_JET;  //лǫ�޸� 2012-5-10
				fprintf(erg_f,"\n \t open_type����Ϊ OPEN_STATUS_SUC_JET");
				break;
			}
			else if(pdiInPut->cr<cr_flow->cr_rate[0]){
				reachProperCR=1;
				fprintf(erg_f,"\n \t �������ٳ��� ���п�����ʽ������Χ��open_typeĬ��Ϊ OPEN_STATUS_SEP_JET");
			}

			//ACC��ȴģʽʱ�����ݲ�ͬ�����µ���ȴ���ʣ�ȷ��Ŀ����ȴ��������Ҫ��ˮ���ܶ�
			PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);
			reachProperCR=1;
			break;
		}
		CR_cycle_count++;
	}

	tStage=1;

	PreResult->coolModeFlg = CoolRateFlows(erg_f, pdiInPut, &adapt1, cr_flow, ver_par1->adaptRatioFound,tStage);// лǫ ���� 2012-5-29

	return 1;
}

int CPreProcess::ReadExpSchFromDatabasePre(FILE 	*erg_f ,unsigned long  int  keynrs[5],PHYSICAL	*ver_par1)
{
	return  0;
}

int CPreProcess::CalcTempCurvePostOffLine(
	FILE 			*st_err,
	FILE 		*erg_f ,
	INPUTDATAPDI *pdiInPut,
	const	  RALCOEFFDATA *lay,		/* layout data      */
	const     RALOUTDATA *preOut,   // лǫ���� 2012-5-16
	PHYSICAL    *ver_par1,
	COMPUTATIONAL *compx,
	float aveSpeed)
{
	MATERIAL matal;

	int i=0;
	int j=0;
	int k=0;
	//��ʼ�� matal,��

	int iz = -1;
	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
	compx->thickNodeNumber=NODES_NUMBER;
	compx->matrixElementNumber = 2 * compx->thickNodeNumber - 1;

	ver_par1->coolPhaseNumber=0;
	EstimationComputationalParam(erg_f,compx, pdiInPut, ver_par1);

	while(++iz < 19){  //��ʼTemperature�ڵ�
		compx->startTempDistribution[iz] = preOut->PreResult.airEndTemp[iz];
		compx->startAusteniteDistribution[iz] = 100.f;
	}

	for (i=1;i<NUZZLE_NUMBER+GAP_NUZZLE_NUMBER;i++){   //ѭ������Ӧ��Ϊ���ܵķֶ�������
		if ((preOut->PreResult.realFlowUp[i]>=40) &&(preOut->PreResult.realFlowUp[i]<=400)){
			compx->detaTime=0.2f;    //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ

			compx->calculationStopTime=lay->CoolBankLen/preOut->PreResult.aveSpeed;  // add [4/16/2014 лǫ]
			if (
				(1==i)||(2==i)||(3==i)||(4==i))
			{  //add by xie 9-17 ��϶���� 2�鵱��һ��   // modify by xie
				ver_par1->topFlowRate=2*preOut->PreResult.realFlowUp[i]*1000/(1.6*5*60);
				ver_par1->bottomFlowRate=2*preOut->PreResult.realFlowUp[i]*1.5f*1000/(1.6*5*60);

				compx->calculationStopTime=0.5*lay->CoolBankLen/preOut->PreResult.aveSpeed;  // add [4/16/2014 лǫ]

			}
			else{
				ver_par1->topFlowRate=preOut->PreResult.realFlowUp[i]*1000/(1.6*5*60);
				ver_par1->bottomFlowRate=preOut->PreResult.realFlowUp[i]*1.5f*1000/(1.6*5*60);
				compx->calculationStopTime=lay->CoolBankLen/preOut->PreResult.aveSpeed;  // add [4/16/2014 лǫ]
			}

			ver_par1->sw_air = 1;
			ver_par1->coolPhaseNumber = 1;
			ver_par1->art[1] = 1;	/* ˮ�� */
			ver_par1->duration[1] = 0.0f;
			compx->computeID = 2;  
						
			pdiInPut->working_stage=1;              //jet�λ���ϵ����ȡ��־λ лǫ 2012-5-8
			m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);

		} 
		else if (preOut->PreResult.realFlowUp[i]<40) //���ܹر� ����
		{
			compx->detaTime=0.2f;              //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ
			//ȷ�������������˴�Ϊȷ������Temperature�ֲ�����ȷ��
			compx->computeID = 2;	/*	const Temperature distribution	*/
			ver_par1->coolPhaseNumber = 0;
			ver_par1->method = 1;	/* ����	*/
			ver_par1->art[0] = 0;
			ver_par1->duration[0] = 0.0f;
			ver_par1->taskID = PREPROCESS;
			ver_par1->sw_air = 0;	/*���� */

			compx->femCalculationOutput = 0; 
			compx->integra = 2;
			compx->isSymetric     = 0;    /* nonsymmetric problem */
			CalculationElementSizes(compx, 2, pdiInPut->thick);

			compx->matrixElementNumber = 2 * compx->thickNodeNumber - 1;
			compx->calculationStopTime=lay->CoolBankLen/preOut->PreResult.aveSpeed;

			pdiInPut->working_stage=0;   

			m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);
		}			
		else if (preOut->PreResult.realFlowUp[i]>400)  //���ܿ���  slit�� лǫ
		{
			//compx->detaTime=0.2f;    //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ

			//ver_par1->topFlowRate=preOut->PreResult.realFlowUp[i];
			//ver_par1->bottomFlowRate=preOut->PreResult.realFlowUp[i]*1.5f;

			//ver_par1->sw_air = 1;
			//ver_par1->coolPhaseNumber = 1;
			//ver_par1->art[1] = 1;	/* ˮ�� */
			//ver_par1->duration[1] = 0.0f;
			//compx->computeID = 2;  

			//compx->calculationStopTime=lay->CoolBankLen/preOut->PreResult.aveSpeed;

			//pdiInPut->working_stage=2;              //slit�λ���ϵ����ȡ��־λ лǫ 2012-5-8

			//m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			//m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);

			compx->detaTime=0.2f;    //�˴�ǿ��time���Ϊ0.5s �����д��о� лǫ

			compx->calculationStopTime=lay->CoolBankLen/preOut->PreResult.aveSpeed;  // add [4/16/2014 лǫ]
			if (
				(1==i)||(2==i)||(3==i)||(4==i))
			{  //add by xie 9-17 ��϶���� 2�鵱��һ��   // modify by xie
				ver_par1->topFlowRate=2*preOut->PreResult.realFlowUp[i]*1000/(1.6*5*60);
				ver_par1->bottomFlowRate=2*preOut->PreResult.realFlowUp[i]*1.5f*1000/(1.6*5*60);

				compx->calculationStopTime=0.5*lay->CoolBankLen/preOut->PreResult.aveSpeed;  // add [4/16/2014 лǫ]

			}
			else{
				ver_par1->topFlowRate=preOut->PreResult.realFlowUp[i]*1000/(1.6*5*60);
				ver_par1->bottomFlowRate=preOut->PreResult.realFlowUp[i]*1.5f*1000/(1.6*5*60);
				compx->calculationStopTime=lay->CoolBankLen/preOut->PreResult.aveSpeed;  // add [4/16/2014 лǫ]
			}

			ver_par1->sw_air = 1;
			ver_par1->coolPhaseNumber = 1;
			ver_par1->art[1] = 1;	/* ˮ�� */
			ver_par1->duration[1] = 0.0f;
			compx->computeID = 2;  

			pdiInPut->working_stage=2;              //jet�λ���ϵ����ȡ��־λ лǫ 2012-5-8  // modify [4/16/2014 лǫ]
			m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
			m_TemperatureField.TimeSolverNoFirstOutPut(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);

		} 

		// �洢ÿһ�� �����Temperature���� 
		for(j=0;j<compx->computedNumber;j++){
			if (0==k)
				compx->zx_temple[k] =compx->zx[j]	;  
			else
				compx->zx_temple[k]=compx->zx_temple[k-1]+compx->zx[j]-compx->zx[j-1]	;

			compx->tx_temple[k]=compx->tx[j];        /* caloric mean Temperature value last curve       */
			compx->tx_cen_temple[k]=compx->tx_cen[j];    /* Temperature in the center line of ���          */
			compx->tx_top_temple[k]=compx->tx_top[j];    /* surface top Temperature value last curve        */
			compx->tx_bot_temple[k]=compx->tx_bot[j];    /* surface bottom temp. value last curve           */
			compx->cx_temple[k]=compx->cx[j];        /* ��ȴ�ٶ�in C                                */
			k++;
		}
	}

	///////////////////////////////////////////////////////////////////
	//// ����ȴ������ �� ������¼�
	///////////////////////////////////////////////////////////////////
	//ȷ�������������˴�Ϊȷ������Temperature�ֲ�����ȷ��

	compx->computeID = 2;	//const Temperature distribution	
	ver_par1->coolPhaseNumber = 0;
	ver_par1->method = 1;	// ����	
	ver_par1->art[0] = 0;
	ver_par1->duration[0] = 0.0f;
	ver_par1->taskID = PREPROCESS;
	ver_par1->sw_air = 0;	//���� 
	compx->femCalculationOutput = 0;
	compx->integra = 2;
	compx->isSymetric     = 0;    //nonsymmetric problem 
	// calculation of element sizes compx->lm[]	
	CalculationElementSizes(compx, 2, pdiInPut->thick);
	compx->matrixElementNumber = 2 * compx->thickNodeNumber - 1;
	compx->calculationStopTime=20/(preOut->PreResult.aveSpeed+0.5f);
	m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal, pdiInPut);   
	m_TemperatureField.TimeSolver(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);

	compx->calculationStopTemp=2*compx->tx_temple[k-1]-compx->tx[compx->computedNumber-1];  //лǫ 2012-5-17   //+8 by xie 12-18  // remove +8 [4/16/2014 лǫ]
	compx->calculationStopTime=compx->zx_temple[k-1];   //лǫ ����������ȴtime 2012-5-17

	return 0;
}


/// @����˵�� 
/// 0) main module for PREPROCESS / declaration in calling software */
///    module with function calls for equipment specific purpose  */
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� xq
/// ���� 9/11/2013
pwr_tInt32  CPreProcess::PreProcessElite(
	RALINDATA *ralInPut,
	RALCOEFFDATA *lay,
	RALOUTDATA *pre_out,
	pwr_tInt32                     *proc_id,
struct T_AccSysPar			   *SysPar,
struct T_AccPattern			   *AccPattern)
{
	PHYSICAL ver_par1;   /* css : ver_mds */
//	NUZZLECONDITION       nuzzleCondtion;
	FILE            *st_err = NULL; /*      end material    */
	FILE            *erg_f = NULL;
	INPUTDATAPDI pdi_mds;
	flow_cr         cr_flow;
	int		i_top  = 0;//  HEAD_1_TOP;	/* offset for FILE in	*/
	int		i_bot  = 0;// HEAD_1_BOT;	/* offset for FILE in	*/
	int		i_check;
	int		ioutpxx	= IOUT;
	int 		iz;
	int		nStroke = 1;	/* No. of strokes */

	if((erg_f = fopen("Logs\\preprocess\\PreCalculateprocess.txt","w")) == NULL)
		return -1; 
	else {
		OutputPDIData(ralInPut,erg_f);
		OutputLayoutData(lay,erg_f);
		fclose(erg_f);
		erg_f = NULL;
	}

	if((st_err  = fopen("Logs\\preerror\\preerror.txt","a")) == NULL)
		return -1;

	ver_par1 = InitPhysicalParam(lay->nHeader + 2, MAT_STEEL,PREPROCESS, st_err);

	InitControlValues(&(pdi_mds.controlAcc));

	// 1) transfer of pdi-��Ϣ
	///////////// in[]��Ϊ�ܹ���ǰ�׶���Ϣ���ݵ���׶εĽṹ�� ///////////////////////////
	pdi_mds.taskID = PREPROCESS;
	iz = -1;

	memset(&pre_out->PreResult, 0, sizeof(pre_out->PreResult));

	FillPDIStruct(&pdi_mds,ralInPut,lay, &pre_out->PreResult, ioutpxx);
	strcpy_s(pre_out->PreResult.slabGrade,ralInPut->s_slab_code);
	strcpy_s(ralInPut->file_nam,pdi_mds.file_nam);

	InitControlValues(&(pdi_mds.controlAcc), &(pre_out->controlAcc));

	i_check = CheckDataValid(&pdi_mds); /* checking data �ж���Ϣ��Ч��*/

	//��UFC��ȴģʽʱ������PDI��UFC�½�С����Сvalue������ȴģʽ��ΪACC
	if(i_check==14)
	{
		ralInPut->acc_mode = ACC;
		i_check=0;
	}

	//UFCģʽʱ UFC�½�����Ҫ�� ACC���½�������Ҫ�� ʱ��ȴģʽ��ΪDQ
	if(i_check==15)
		i_check=0;

	if(i_check > 0)
	{
		fclose(st_err);
		st_err = NULL;
		logger::LogDebug("system","Ԥ����������Ч����֤���� ID:%d",i_check);
		return(i_check);	/* not plausible input	*/
	}

	// 1.1) output PDI - ��Ϣ on FILE and close 
	strcpy_s(pdi_mds.file_nam,"Logs\\postprocess\\TimeRequestData.txt");

	//�˴����FILE�� \Log\post_dat\�ļ��� ��   лǫ 2012-7-20
	errno_t err;
	//if((erg_f = fopen(pdi_mds.file_nam,"w")) == NULL)
	if((erg_f = fopen(pdi_mds.file_nam,"w+")) == NULL)
	{		
		_get_errno( &err );
		fprintf( st_err,"\t errno = %d\n", err );
		OutputErrorLog(st_err, pdi_mds.file_nam,"open FILE erg_f", "pre_org()");
		return -1;
		//exit(1);
	}

	//��ȡ����ϵ������ ����ver_par1->alpha_coe  лǫ ���� 2012-7-12
	ver_par1.waterTemp=lay->WaterTemp;
	ver_par1.alpha_coe=1.0f;
	ver_par1.waterFactorCoe=0.0f;
	pre_out->PreResult.alphaCoe=ver_par1.alpha_coe;

	OutputPreInfo(&pdi_mds,erg_f);

	// 1.2) read strategy data for cooling
	i_check = ReadAccStrategyParam(&pdi_mds, lay, &pre_out->accStrategy);

	if(i_check == FALSE)
	{
		OutputErrorLog(st_err, pdi_mds.file_nam,"���� cooling strategy", "pre_org()");
		fclose(erg_f);
		return -1;
	}

	ModifyAccStrategy(ralInPut, lay, &pre_out->accStrategy);
	ModifyPDIStrategy(erg_f, &pre_out->accStrategy, &pdi_mds, ralInPut, &pre_out->PreResult);

	//////////////////////////////////////////////////////��ȴģʽ�ж�    ///////////////////////////////////////////////////////
	i_check=PreDefineTechPara(ralInPut,&ver_par1);//change by wbxang 20120505
	if (1!=i_check)
	{
		OutputErrorLog(st_err, pdi_mds.plateID, "û����ȷ�涨��ȴ���", "PreDefineTechPara()");
		fclose(erg_f);
		erg_f=NULL;
		return -1;
	}

	readFileSch(ralInPut,&pre_out->PreResult,erg_f);  // add [3/17/2014 лǫ]

	pdi_mds.manualHC = pre_out->PreResult.readCoeAlpha; // add [3/17/2014 лǫ]

	if (1==ralInPut->ICtype)
	{
		pdi_mds.manualHC=1.3f;   // add �м������ [7/11/2014 лǫ]
	}

	pdi_mds.acc_mode = ralInPut->acc_mode;
	pdi_mds.open_type = ralInPut->open_type;
	pdi_mds.pass_mode = ralInPut->pass_mode;
	pdi_mds.activeSecType = ralInPut->activeSecType;
	pdi_mds.activeFirstJet = ralInPut->activeFirstJet;

	// 2) read ������Ϣ
	i_check = InputInfomations( erg_f, st_err, &ver_par1, &pre_out->PreResult);
	cr_flow = GetFlowInfo(st_err, &pdi_mds, &pre_out->accStrategy, &pre_out->PreResult,ver_par1.waterFactorCoe);	/* flow data input */
	SetLowFlowCoolingRate(erg_f, &pdi_mds, cr_flow.iFlwLimit, &pdi_mds.cr);  //�˹�����Ҫ�޸� лǫ 2012-5-10
	pre_out->PreResult.coolRate = pdi_mds.cr;

	if(AccPattern != NULL)
		SetCoolPatternFlowValues(&cr_flow, &pdi_mds, lay, SysPar, AccPattern);

	//  3) calculate Temperature for different flows
	const	int	 nThick = sizeof(lay->ThickUpLimit) / sizeof(lay->ThickUpLimit[0]); //��ȵĲ�𣿣� лǫ

	lay->initSpeed = AccPreferedSpeed(pdi_mds.thick * 1000.f, nThick, 	lay->ThickUpLimit,	lay->MaxSpeedCool); // ���ݺ�� ���� �ٶȲ�value

	fprintf(erg_f,"\n\t ��ʼ�ٶ�: %f",lay->initSpeed);

	pdi_mds.initFinishRollingTem = pdi_mds.targetFinishCoolTemp;  // лǫ ���� �洢ԭ�趨Temperature 2012-6-5
	
	i_check =   CalculatePre(st_err, erg_f, SysPar, AccPattern,	&pdi_mds, lay, &pre_out->accStrategy, &pre_out->PreResult, &ver_par1,&cr_flow,&pre_out->adapt,&pre_out->SLP_Post);
		
	fprintf(erg_f,"\n\t -----------------------------------------Ԥ�������------------------------------------------");

	if(erg_f != NULL)
	{
		fclose(erg_f);
		erg_f = NULL;
	}
	if(st_err != NULL){
		fclose(st_err);
		st_err = NULL;
	}
	/* detailed output fine cooling calculation */
	return(i_check);	/* return of value 1 for OSAP-application	*/
}



void CPreProcess::CoolSchOpenFront(NUZZLECONDITION *nuzzleCondtion,const flow_cr *cr_flow)
{
	int num[12]={};
	int num1[12]={0,0,1,0,0,0,0,0,0,0,0,0};
	int num2[12]={0,0,0,0,1,1,0,0,0,0,0,0};
	int num3[12]={0,0,0,1,1,1,0,0,0,0,0,0};
	int num4[12]={0,0,0,0,1,1,0,1,1,0,0,0};
	int num5[12]={0,0,0,1,1,1,0,1,1,0,0,0};
	int num6[12]={0,0,0,1,1,1,1,1,1,0,0,0};
	int num7[12]={0,0,0,1,1,1,0,1,1,0,1,1};
	int num8[12]={0,0,0,1,1,1,1,1,1,0,1,1};
	int num9[12]={0,0,0,1,1,1,1,1,1,1,1,1};
	int num10[12]={0,1,1,0,1,1,1,1,1,1,1,1};
	int num11[12]={0,1,1,1,1,1,1,1,1,1,1,1};
	int num12[12]={1,1,1,1,1,1,1,1,1,1,1,1};
	int i =0;
	if(nuzzleCondtion->header_no<1)
		nuzzleCondtion->header_no = 1;
	if(nuzzleCondtion->header_no>12)
		nuzzleCondtion->header_no = 12;

	switch(nuzzleCondtion->header_no)
	{
	case 1:

		for(i=0;i<12;i++)
		{
			num [i] = num1[i];
		}

		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 2:

		for(i=0;i<12;i++)
		{
			num [i] = num2[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 3:

		for(i=0;i<12;i++)
		{
			num [i] = num3[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 4:

		for(i=0;i<12;i++)
		{
			num [i] = num4[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 5:

		for(i=0;i<12;i++)
		{
			num [i] = num5[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 6:

		for(i=0;i<12;i++)
		{
			num [i] = num6[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 7:

		for(i=0;i<12;i++)
		{
			num [i] = num7[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 8:

		for(i=0;i<12;i++)
		{
			num [i] = num8[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 9:

		for(i=0;i<12;i++)
		{
			num [i] = num9[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 10:

		for(i=0;i<12;i++)
		{
			num [i] = num10[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 11:

		for(i=0;i<12;i++)
		{
			num [i] = num11[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 12:

		for(i=0;i<12;i++)
		{
			num [i] = num12[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	}
	return;
}

void CPreProcess::CoolSchOpenFrontSlit(NUZZLECONDITION *nuzzleCondtion,const flow_cr *cr_flow)
{
	int num[12]={};
	int num1[12]={0,0,1,0,0,0,0,0,0,0,0,0};
	int num2[12]={0,0,0,0,1,1,0,0,0,0,0,0};
	int num3[12]={0,0,0,1,1,1,0,0,0,0,0,0};
	int num4[12]={0,0,0,0,1,1,0,1,1,0,0,0};
	int num5[12]={0,0,0,1,1,1,0,1,1,0,0,0};
	int num6[12]={0,0,0,1,1,1,1,1,1,0,0,0};
	int num7[12]={0,1,1,1,1,1,0,1,1,0,0,0};
	int num8[12]={0,1,1,1,1,1,1,1,1,0,0,0};
	int num9[12]={0,1,1,1,1,1,0,1,1,0,1,1};
	int num10[12]={0,1,1,1,1,1,1,1,1,0,1,1};
	int num11[12]={0,1,1,1,1,1,1,1,1,1,1,1};
	int num12[12]={1,1,1,1,1,1,1,1,1,1,1,1};
	int i =0;
	if(nuzzleCondtion->header_no<1)
		nuzzleCondtion->header_no = 1;
	if(nuzzleCondtion->header_no>12)
		nuzzleCondtion->header_no = 12;

	switch(nuzzleCondtion->header_no)
	{
	case 1:

		for(i=0;i<12;i++)
		{
			num [i] = num1[i];
		}

		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 2:

		for(i=0;i<12;i++)
		{
			num [i] = num2[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 3:

		for(i=0;i<12;i++)
		{
			num [i] = num3[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 4:

		for(i=0;i<12;i++)
		{
			num [i] = num4[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 5:

		for(i=0;i<12;i++)
		{
			num [i] = num5[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 6:

		for(i=0;i<12;i++)
		{
			num [i] = num6[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 7:

		for(i=0;i<12;i++)
		{
			num [i] = num7[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 8:

		for(i=0;i<12;i++)
		{
			num [i] = num8[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 9:

		for(i=0;i<12;i++)
		{
			num [i] = num9[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 10:

		for(i=0;i<12;i++)
		{
			num [i] = num10[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 11:

		for(i=0;i<12;i++)
		{
			num [i] = num11[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	case 12:

		for(i=0;i<12;i++)
		{
			num [i] = num12[i];
		}
		for (i=0;i<12;i++)
		{
			nuzzleCondtion->working_status[i] = num[i];
			if(1==nuzzleCondtion->working_status[i])
			{
				nuzzleCondtion->flow_header_top[i]=cr_flow->flow_t;
				nuzzleCondtion->flow_header_bottom[i]=cr_flow->flow_b;
			}
		}
		break;
	}
	return;
}