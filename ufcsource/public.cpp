/*! 
* Copyright (c) 2013, 东北大学轧制技术及连轧自动化国家重点实验室(RAL)
* All rights reserved
* 公共函数
* FILE public.cpp
* 说明 概述  
*  
* 详细说明
*  
* 版本 1.0 作者 Hanbing 日期 8/6/2013
* 
*/

#include "stdafx.h"

#include "public.H"


/// @函数说明 拷贝原始数据到pdi结构体和in数组
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/26/2013
//int pdifill(
int FillPDIStruct(
	INPUTDATAPDI *pdi,
	RALINDATA *ralInPut,
	RALCOEFFDATA *lay,
	PRERESULT *PreResult,
	const int								 nOutput)
{
	float     fAlloyMult     = 1.f;	/* from PDI */
	float     fAlloyMultTelegram = 1000.f;	/* to telegram */
	float		eps = 0.01f;

	strcpy_s(pdi->plateID,ralInPut->plateID);
	pdi->thick = (float) ralInPut->thick;
	pdi->width = (float) ralInPut->width; 
	pdi->length =(float) ralInPut->length;
	pdi->finishRollTemp = (float)ralInPut->finishRollTemp;
	pdi->targetFinishCoolTemp = (float) ralInPut->targetFinishCoolTemp;
	pdi->initFinishRollingTem=(float) ralInPut->targetFinishCoolTemp;

	pdi->PltIcFlg[0]=ralInPut->PltIcFlg[0];
	pdi->PltIcFlgTemp=ralInPut->PltIcFlgTemp;

	float fDiffTemp = pdi->finishRollTemp - pdi->targetFinishCoolTemp;

	pdi->cr = (float) ralInPut->cr;

	pdi->acc_mode = (int)ralInPut->acc_mode;
	if(pdi->acc_mode == IC)
	{ 
		pdi->targetFinishCoolTemp = pdi->finishRollTemp - IC_ALPHA_TEMP * fDiffTemp;
		pdi->IcColTime = ralInPut->IcColTime;
		PreResult->IcColTime = ralInPut->IcColTime;
		pdi->IcColTimeAir = ralInPut->IcColTimeAir;
		PreResult->IcColTimeAir = ralInPut->IcColTimeAir;
	}
	pdi->op_mode = (int)ralInPut->op_mode;
	strcpy_s(pdi->matID, ralInPut->mat_id);
	pdi->alloy_c = (float)ralInPut->alloy_c  * fAlloyMult;
	pdi->alloy_cr =(float)ralInPut->alloy_cr    * fAlloyMult;
	pdi->alloy_cu =(float)ralInPut->alloy_cu    * fAlloyMult;
	pdi->alloy_mn =(float)ralInPut->alloy_mn    * fAlloyMult;
	pdi->alloy_mo =(float)ralInPut->alloy_mo    * fAlloyMult;
	pdi->alloy_ni =(float)ralInPut->alloy_ni    * fAlloyMult;
	pdi->alloy_si =(float)ralInPut->alloy_si    * fAlloyMult;
	pdi->alloy_ti =(float)ralInPut->alloy_ti    * fAlloyMult;
	pdi->alloy_nb =(float)ralInPut->alloy_nb    * fAlloyMult;
	pdi->alloy_b  =(float)ralInPut->alloy_b    * fAlloyMult;
	pdi->alloy_v  =(float)ralInPut->alloy_v    * fAlloyMult;

	//025 56885639

	pdi->alloy_equC = (float)ralInPut->alloy_c + (float)ralInPut->alloy_mn/6.0f;

	pdi->v_min_PL = (float) ralInPut->v_min_PL;  /* min 速度 预矫直机 [m/s]  */
	pdi->v_max_PL = (float) ralInPut->v_max_PL;  /* max 速度 预矫直机 [m/s]  */
	pdi->v_min_HL = (float) ralInPut->v_min_HL;  /* min 速度 热矫直机 [m/s]  */
	pdi->v_max_HL = (float) ralInPut->v_max_HL;  /* max 速度 热矫直机 [m/s]  */
	pdi->pl_mode = 0;
	pdi->priority = (int) ralInPut->priority;
	int in1 = ralInPut->iPat;
	if((in1 < 0) || (in1 > 1))
		in1 = 1; /* 0: compact 1: use distributed pattern */
	pdi->iPat = in1;
	in1 = ralInPut->iDirLastPass;
	if((in1 < -1) || (in1 > 1)) 
		in1 = -1;   /* direction last pass -1: furnace 1: Acc */
	if((pdi->length + pdi->controlAcc.fDistSafety) < lay->fPosHeader[0]) 
		in1 = -1;
	pdi->iDirLastPass = in1;
	in1 = ralInPut->iHeaderFirst - 1; /* first header [0] is header 1 on HMI */
	if((in1 < 0) || (in1 > FIRSTHEADERMAX)) 
		in1 = 0;    /* index first active header */
	pdi->iHeaderFirst = in1;

	if(pdi->acc_mode == IC)
		pdi->iHeaderFirst = ralInPut->iHeaderFirst-1;

	int		nStep = -1;
	while(++nStep < NODES_NUMBER)	{
		pdi->startTempDistribution[nStep] = (float) ralInPut->startTempDistribution[nStep];  /* Temperature  [度] */
		pdi->aust_start[nStep] = (float) ralInPut->austeniticDecompRate[nStep]; /* austenit[%]  */
		pdi->grain_size[nStep] = (float) ralInPut->grainSize[nStep]; /* grain size [mue]*/
	}

	char      dbstry[PATH_MAX];
	GetPlateTaskFileName(ralInPut->plateID,pdi->acc_mode,dbstry); 
	CombinFilePath(dbstry,POSTPROCESS,pdi->file_nam);

	int nExponentAlloy = 0;	/* estimate exponent for alloy */
	nStep = -1;
	float 	xx = 1.f;
	while(++nStep < 5)	{
		xx  *=  10.f;
		nExponentAlloy++;
		if(fabs(fAlloyMultTelegram - xx) < eps) break;
	}						/* estimate exponent for alloy */

	strcpy_s(PreResult->plateID,ralInPut->plateID);
	strcpy_s(PreResult->MatId,ralInPut->mat_id);   //add 7-25

	PreResult->plateThick = pdi->thick * 1000.f;
	PreResult->plateWidth = pdi->width * 1000.f;
	PreResult->plateLength = pdi->length*1000.f;    //add 7-25 


	PreResult->finishRollTemperature = pdi->finishRollTemp;
	PreResult->finishRollTargetTemperature = ralInPut->t_finish_target;  //add by xie 12-3
    PreResult->finishCoolTargetTemperature = pdi->targetFinishCoolTemp;
	PreResult->tFinishCoolCalc = pdi->finishRollTemp - fDiffTemp;
	PreResult->speedEXP = pdi->SpeedEXP;
	PreResult->acclerEXP = pdi->AcclerEXP;

	PreResult->coolRate = pdi->cr;
	PreResult->coolTypeMode = pdi->acc_mode;
	PreResult->alloyExponent = nExponentAlloy;
	PreResult->alloyC = pdi->alloy_c;
	PreResult->alloyCr = pdi->alloy_cr;
	PreResult->alloyCu = pdi->alloy_cu;
	PreResult->alloyMn = pdi->alloy_mn;
	PreResult->alloyMo = pdi->alloy_mo;
	PreResult->alloyNi = pdi->alloy_ni;
	PreResult->alloySi = pdi->alloy_si;
	PreResult->alloyTi = pdi->alloy_ti;
	PreResult->alloyNb = pdi->alloy_nb;
	PreResult->alloyB = pdi->alloy_b;
	PreResult->alloyV = pdi->alloy_v;
	PreResult->tempWater = lay->WaterTemp;
	PreResult->tempAir = lay->AirTemp;
	PreResult->WatPress = lay->WaterPress;
	PreResult->operateMode = pdi->op_mode;
	PreResult->coolBankLen = lay->CoolBankLen;
	PreResult->coolWidth = lay->CoolWidth;
	PreResult->preLeverVMax = pdi->v_max_PL;
	PreResult->preLeverVMin = pdi->v_min_PL;
	PreResult->hotLeverVMax = pdi->v_max_HL;
	PreResult->hotLeverVMin = pdi->v_min_HL;

	PreResult->isHeadFlated=ralInPut->isHeadFlated;  
	pdi->isHeadFlated=ralInPut->isHeadFlated; 
	return TRUE;
}

/* build up 文件名 for results                                */
int	GetPlateTaskFileName(const  char *plate_id, const  int  taskID, char *fn)
{
	sprintf(fn,"%s_%i.txt",plate_id,taskID);
	return TRUE;
}

/* initialize control value adaption and online  */
/// @函数说明 初始化适应控制参数 和 在线控制参数
/// @参数 x  控制参数结构体
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/26/2013
//int init_control_t(control_t    *x)
int InitControlValues(MODELCONTROL *x)
{
	//初始化在线参数
	x->bSmoothTempEntry     = 1;    /* Smooth entry Temperature */
	x->bSmoothTempExit      = 1;    /* Smooth exit  Temperature */
	x->bCorTempAct          = 1;    /* 纠正 Temperature 偏差 Entry from PDI */
	x->bCorSpdAct           = 1;    /* 纠正 速度 profile by measured 速度value */
	x->fCorDevSpd           = 0.1f; /* allowed 速度 偏差 for 纠正ion [% * 0.01] */ 
	x->bCorSpdSynchro       = 1;    /* additional 纠正ion by synchronization points */
	x->fCorTimeSynchro      = 2.f;  /* time window passing synchronisation points [sec] */
	x->fCorPosSynchro       = 3.2f; /* position window synchronization [m] */
	x->iBlsSynchro          = 2;    /* index of synchronization point (0: BLS1, 1: HMD)  */
	x->fGainDevSpd          = 1.0f; /* gain for 速度 纠正ion */
	x->fGainDevTemp         = 1.0f; /* gain for entry Temperature 偏差 */
	x->fDistSafety          = 5.0f; /* safety distance 头部 to first active header */

	//初始化适应控制参数
	x->iAdaptLevel          = 2;    /* use adaption level 1 to iAdaptLevel */
	x->nAdaptPlates         = 4;    /* No. of plates for adaption */
	x->bAdaptHeadRead       = 1;    /* 适应 头部 遮蔽 read 估value */
	x->bAdaptTailRead       = 1;    /* 适应 尾部 遮蔽 read 估value */
	x->bAdaptHeadWrite      = 1;    /* 适应 头部 遮蔽 write 估value */
	x->bAdaptTailWrite      = 1;    /* 适应 尾部 遮蔽 write 估value */
	x->fAdaptDifTime        = 4380.f; /* differential time use higher adaption level [hr] */
	x->fAdaptLimitMin		= 0.51f;  /* 最小自适应限制 */
	x->fAdaptLimitMax		= 2.3f;	 /* 最大 自适应限制 */   //modify by xie 11-8
	x->fAdaptStepSize		= 0.1f;	 /* 最大 step size from one piece to next */
	x->fAdaptStepSizeTmp	= 20.f;	 /* 最大 step size from one piece to next Temperature */
	x->fAdaptLimitHeadMin	= 0.6f;	 /* 最小自适应限制 头部 */
	x->fAdaptLimitHeadMax   = 1.3f;	 /* 最大 自适应限制 头部 */
	x->fAdaptLimitTailMin   = 0.8f;	 /* 最小自适应限制 尾部 */
	x->fAdaptLimitTailMax   = 1.2f;	 /* 最大 自适应限制 尾部 */
	x->fAdaptAddFlwHead     = 0.15f; /* additional flow allowed on 头部 [% * 0.01] */
	x->fAdaptAddLenHead     = 8.0f;	 /* 最大 allowed 头部 length 纠正ion [m] */
	x->fAdaptAddFlwTail     = 0.1f;	 /* additional flow allowed on 尾部 [% * 0.01] */
	x->fAdaptAddLenTail     = 3.0f;	 /* 最大 allowed 尾部 length 纠正ion [m] */

	//合同参数
	x->fMinMaxTempEntry     = 60.f; /* allowed min-max Temperature difference Entry [K] */
	x->fMinMaxTempExit      = 35.f; /* allowed min-max Temperature difference Exit  [K] */
	x->fFacDevTemp          = 2.5f; /* multiplier for additional Temperature 偏差 Entry */
	x->fAdaptDevSpd         = 0.01f; /* 最大 allowed 速度 偏差 [% * 0.01] */
	x->fFacDevSpd           = 1.f;  /* multiplier for additional 速度 偏差 [tc * CR] */

	//改造冷却模式信息
	x->iDimFlw              = 2; /* ltr / min */
	x->iDimEdge             = 2; /* mm */
	x->iDimLenMask          = 1; /* m */
	return TRUE;
}

// read control value adaption and online
int InitPostControlValues(const   char        *sFName, MODELCONTROL *x)     /* control structure */
{
	FILE        *ffx = NULL;
	char    line[PATH_MAX];
	int		max_in = PATH_MAX - 1;
	if((ffx = fopen(sFName,"r")) == NULL) {
		return FALSE;
	}
	/* 在线模式的控制参数 */
	fgets(line, max_in, ffx);                       
	sscanf(line,"%d", &(x->bSmoothTempEntry));   /* Smooth entry Temperature */

	fgets(line, max_in, ffx);                       
	sscanf(line,"%d", &(x->bSmoothTempExit));    /* Smooth exit  Temperature */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->bCorTempAct));  /* 纠正 Temperature 偏差 Entry from PDI */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->bCorSpdAct));  /* 纠正 速度 profile by measured 速度value */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fCorDevSpd)); /* allowed 速度 偏差 for 纠正ion [% * 0.01] */ 
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->bCorSpdSynchro)); /* additional 纠正ion by synchronization points */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fCorTimeSynchro));  /* time window passing synchronisation points [sec] */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fCorPosSynchro)); /* position window synchronization [m] */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->iBlsSynchro)); /* index 参考 synchronization point (0: BLS1, 1: HMD) */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fGainDevSpd));    /* gain for 速度 纠正ion */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fGainDevTemp));   /* gain for entry Temperature 偏差 */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fDistSafety));	/* safety distance 板材 -> Acc [m] */
	/* 控制参数 adaption (PreCalc + PostCalc) */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->iAdaptLevel));    /* use adaption level 1 to iAdaptLevel */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->nAdaptPlates));   /* No. of plates for adaption */

	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->bAdaptHeadRead)); /* 适应 头部 遮蔽 read 估value */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->bAdaptTailRead)); /* 适应 尾部 遮蔽 read 估value */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->bAdaptHeadWrite));/* 适应 头部 遮蔽 write 估value */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->bAdaptTailWrite));    /* 适应 尾部 遮蔽 write 估value */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptDifTime));  /* differential time use higher adaption level [hr] */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptLimitMin)); /* 最小limit adaption */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptLimitMax)); /* 最大 limit adaption */

	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptStepSize)); /* 最大 step size one piece to next */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptStepSizeTmp)); /* 最大 step size Temperature one piece to next */	

	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptLimitHeadMin)); /* 最小limit adaption 头部 */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptLimitHeadMax)); /* 最大 limit adaption 头部 */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptLimitTailMin)); /* 最小limit adaption 尾部 */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptLimitTailMax)); /* 最大 limit adaption 尾部 */

	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptAddFlwHead)); /* maxim additional flow 头部 [% * 0.01]*/
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptAddLenHead)); /* 最大 limit adaption 头部 [m] */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptAddFlwTail)); /* maxim additional flow 尾部 [% * 0.01]*/
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptAddLenTail)); /* 最大 limit adaption 尾部 [m] */

	/* contractual 参数 */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fMinMaxTempEntry));   /* allowed min-max Temperature difference Entry [K] */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fMinMaxTempExit));    /* allowed min-max Temperature difference Exit  [K] */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fFacDevTemp));    /* multiplier for additional Temperature 偏差 Entry */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fAdaptDevSpd)); /* 最大 allowed 速度 偏差 [% * 0.01] */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%f", &(x->fFacDevSpd)); /* multiplier for additional 速度 偏差 [tc * CR] */

	/* read 信息 冷却模式 */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->iDimFlw)); /* flow unit */

	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->iDimEdge)); /* edge 遮蔽 unit */
	fgets(line, max_in, ffx); 						
	sscanf(line,"%d", &(x->iDimLenMask)); /* unit 头部 and 尾部 遮蔽 */
	fclose(ffx);
	return TRUE;
}

//xx initialize control value adaption and online
int InitControlValues(const MODELCONTROL *x, MODELCONTROL *y) 
{
	*y = *x;
	return TRUE;

	/* 控制参数 online */
	y->bSmoothTempEntry     = x->bSmoothTempEntry;
	y->bSmoothTempExit      = x->bSmoothTempExit;
	y->bCorTempAct          = x->bCorTempAct;
	y->bCorSpdAct           = x->bCorSpdAct;
	y->fCorDevSpd           = x->fCorDevSpd;     
	y->bCorSpdSynchro       = x->bCorSpdSynchro;
	y->fCorTimeSynchro      = x->fCorTimeSynchro;
	y->fCorPosSynchro       = x->fCorPosSynchro;
	y->iBlsSynchro          = x->iBlsSynchro;
	y->fGainDevSpd          = x->fGainDevSpd; 
	y->fGainDevTemp         = x->fGainDevTemp;
	y->fDistSafety          = x->fDistSafety;
	/* 控制参数 adaption */
	y->iAdaptLevel          = x->iAdaptLevel;   
	y->nAdaptPlates         = x->nAdaptPlates;    


	y->bAdaptHeadRead       = x->bAdaptHeadRead;
	y->bAdaptTailRead       = x->bAdaptTailRead;
	y->bAdaptHeadWrite      = x->bAdaptHeadWrite;
	y->bAdaptTailWrite      = x->bAdaptTailWrite;
	y->fAdaptDifTime        = x->fAdaptDifTime;
	y->fAdaptLimitMin       = x->fAdaptLimitMin;
	y->fAdaptLimitMax       = x->fAdaptLimitMax;


	y->fAdaptStepSize       = x->fAdaptStepSize;
	y->fAdaptStepSizeTmp    = x->fAdaptStepSizeTmp;



	y->fAdaptLimitHeadMin   = x->fAdaptLimitHeadMin;
	y->fAdaptLimitHeadMax   = x->fAdaptLimitHeadMax;
	y->fAdaptLimitTailMin   = x->fAdaptLimitTailMin;
	y->fAdaptLimitTailMax   = x->fAdaptLimitTailMax;
	y->fAdaptAddFlwHead     = x->fAdaptAddFlwHead;
	y->fAdaptAddLenHead     = x->fAdaptAddLenHead;
	y->fAdaptAddFlwTail     = x->fAdaptAddFlwTail;
	y->fAdaptAddLenTail     = x->fAdaptAddLenTail;
	/* contractual 参数 */
	y->fMinMaxTempEntry     = x->fMinMaxTempEntry;
	y->fMinMaxTempExit      = x->fMinMaxTempExit;
	y->fFacDevTemp          = x->fFacDevTemp;
	y->fAdaptDevSpd         = x->fAdaptDevSpd;  
	y->fFacDevSpd           = x->fFacDevSpd;
	/* read 冷却模式 */
	y->iDimFlw              = x->iDimFlw;
	y->iDimEdge             = x->iDimEdge;
	y->iDimLenMask          = x->iDimLenMask;
	return TRUE;
}


int	PreparationOutput(RALINDATA *ralInPut,RALOUTDATA *pre_out,FILE *st_err)
{   
	FILE		*ffx;
	time_t long_time;
	struct tm *newtime;

	if(ralInPut->iSimMode != SIMULATION)
		return  TRUE;

	char szFileName[100];
	sprintf(szFileName, "%s%s.bda", DIR_PRE_RES,ralInPut->plateID);

	if((ffx = fopen(szFileName,"rb")) == NULL){
		time(&long_time);
		newtime = localtime(&long_time);

		fprintf(st_err,"\n\t *** datei: %s nicht zu oeffnen %s ***", szFileName, asctime(newtime));
		return FALSE;
	}

	fclose(ffx);
	return TRUE;
}

void CalculateLinearInterpolation(float *a, float *x, int anz, int maxi)
{
	int     i = 0;
	int     nStep = -2;
	int     ixx = 0;
	int     ianf;
	int     iende = 0;
	float   ord1, ord2, a1, xhelp;
	int     itxx;

	while(i < anz){
		i++;
		ixx += 2;
		if(x[ixx] > maxi)
			break;
	}
	itxx = i;

	i = 0;
	ixx = 0;
	ord2 = x[1];
	while(i < itxx){
		i++;
		nStep += 2;
		ianf = iende;
		xhelp = x[nStep];
		iende = (int) xhelp;
		if(maxi < iende)
			iende = maxi;
		ord1 = ord2;
		ord2 = x[nStep+1];
		if(iende == ianf)
			a1 = 0.f;
		else
			a1 = (ord2 - ord1)/(iende - ianf);
		while(ixx <= iende)
			a[ixx++] = ord1 + a1 * (ixx - ianf -1);
	}
	while(ixx < maxi)	
		a[ixx++] = ord2;

}

/* 线性内插 of 纵坐标value (ord) for one given  */
/* 横坐标 value (absz)                                       */
/// @函数说明 根据给定的横坐标value求得纵坐标value的线性插value
/// @参数 x 横坐标序列
/// @参数 y 纵坐标序列
/// @参数 n 坐标数量
/// @参数 absz 给定的横坐标value
/// 返回value 线性插value
/// 作者 Hanb
/// 日期 7/26/2013
//float   lin_interpol(const float *x, const float *y, const   int     n, const   float   absz)
float   GetLinearInterpolationFromAbscissa(const float *x, const float *y, const   int  n, const   float   absz)
{
	float   eps = .1e-12f;

	if(absz < x[0])         
		return(y[0]);
	if(absz > x[n-1])       
		return(y[n-1]);

	int     nStep  = 0;
	while(++nStep < n) {
		if(absz < x[nStep])        
			break;
	}

	float r = x[nStep] - x[nStep-1];
	float   ord; 
	if (fabs(r) < eps) {
		ord = .5f * (y[nStep] + y[nStep-1]);
		return(ord);
	}
	r       = (absz - x[nStep-1])/r;
	ord     = (1.f -r) * y[nStep-1] + r * y[nStep];
	return(ord);
}

/* estimate index第一个非零 element              */
/* n - No. of elements                               */
/* Exceptions                                        */
/* returns n: no nonzero element                     */
/* returns -1: n < 1                                 */

int	GetFirstNonzeroElement(const	int		n,const	float	fEps,const	float	*fVal)
{
	int nStep = -1;
	while(++nStep < n)	{
		if(fVal[nStep] > fEps)	
			break;
	}
	return nStep;
}

/* estimate index 最后一个非零 element               */
/* n - No. of elements                               */
/* Exceptions                                        */
/* returns -1 if no nonzero element                  */
/* returns (n - 1) if(n < 1)                         */
int	GetLastNonzeroElement(const	int		n,const	float	fEps,const	float	*fVal)
{
	int nStep = n;
	while(--nStep > -1)	{
		if(fVal[nStep] > fEps)	break;
	}
	return(nStep);
}

/* estimate index 最大value                      */
/* n - No. of elements                               */
/* Exceptions                                        */
/* returns -1 if no 最大 found                    */
int	GetMaxValuePos(const	int		n,const	float	fEps,const	float	*fVal)
{
	int		nStep		= -1;
	int		nMax	= -1;
	float	fMax = fVal[0] + fEps;
	while(++nStep < n)	{
		if(fVal[nStep] > fMax){
			fMax = fVal[nStep];
			nMax = nStep;
		}
	}
	return(nMax);
}

/* estimate index最小value                      */
/* n - No. of elements                               */
/* Exceptions                                        */
/* returns -1 if no 最小found                    */
int	GetMinValuePos(const	int		n,	const	float	fEps,	const	float	*fVal)
{
	int		nStep		= -1;
	int		nMin	= -1;
	float	fMin	= fVal[0] - fEps;
	while(++nStep < n)	{
		if(fVal[nStep] < fMin){
			fMin = fVal[nStep];
			nMin = nStep;
		}
	}
	return(nMin);
}

/* limits 文件大小 to given 限制valuelSizeMax    */
/* if 实际 文件大小 lSizeAct > lSizeMax           */
/* new 文件名 sFileNameSave will be created given */
/* by:  sFilePrev + sFileName                        */

long int	LimitFileSize(
	const	long int	lSizeMax,	/* 最大 文件大小 */
	const	char		*sFileName,	/* original 文件名 */
	const	char		*sFilePref)	/* prefix for last safety copy */
{
	FILE		*fStream;
	long int	lSizeAct = -1L;

	if(strlen(sFilePref) < 1) return(lSizeAct);
	if((fStream = fopen(sFileName,"r")) == NULL){
		printf("\n\t Not able to open: %s in: %s line: %d", sFileName, __FILE__, __LINE__);
		return(lSizeAct);
	}
	fseek(fStream, 0L, SEEK_END);
	lSizeAct = ftell(fStream);
	fclose(fStream);
	if(lSizeAct > lSizeMax){
		char		*sFileNameSave;
		int nFileNameLength = strlen(sFileName) + strlen(sFilePref) + 1;
		if((sFileNameSave = (char *)calloc(nFileNameLength, sizeof(char))) == NULL){
			printf("\n\t not able to allocate: sFileNameSave in: %s", __FILE__);
			return(-1L);
		}
		strcpy(sFileNameSave, sFilePref);
		strcat(sFileNameSave, sFileName);
		remove(sFileNameSave); /* remove old backup FILE */
		rename(sFileName, sFileNameSave);
		free(sFileNameSave);
	}
	return(lSizeAct);
}

void ComputationalParameters(COMPUTATIONAL *compx, const INPUTDATAPDI *pdiInPut, const PHYSICAL *ax)
{
	/*      estimation of computational 参数  */
	/*      TESTUNG DER MODULE IST FUER DIESE WERTE ERFOLGT !       */
	int     nStep = -1;
	float   xx;
	int     ihilf;
	float   xhilf; // 终冷Temperature的中间变量
	int     nodesNumber = NODES_NUMBER;
	float   EPS  = 0.01f;

	compx->thickNodeNumber    = nodesNumber;         /* spaeter anpassen     */

	/*      data input      */
	compx->testParameter = 1;
	compx->femCalculationOutput = 0; 
	compx->calculationStopTime = ax->actualStopTime;
	if ( compx->computeID == 1){
		xx = ax->actualStartTemp;
		while(++nStep < compx->thickNodeNumber) {
			compx->startTempDistribution[nStep] = xx;
			compx->startAusteniteDistribution[nStep] = 100.f;
		}
	}

	compx->integra = 2;

	if(ax->sw_air == 0){ /*空冷  */

		if(compx->calculationStopTime < EPS)	
			compx->calculationStopTemp = pdiInPut->finishRollTemp;
		compx->detaTime    = pdiInPut->thick * 333;
		compx->isSymetric  = 0;    /* non symmetric problem */

		nStep = 0;
		if( compx->computeID == 0) 
			compx->testParameter = 2;  
	}
	else{

		compx->thickNodeNumber         = nodesNumber;
		xx           = (100 * pdiInPut->thick)/( pdiInPut->cr + 1);
		if(xx > 1.f)
			xx = 1.f;
		if(xx < 0.001f)
			xx = 0.01f;
		ihilf  = (int) (1000 * xx);
		compx->detaTime    = (float) (ihilf * 0.001);
		compx->calculationStopTemp   = 0.0f;
		compx->isSymetric  = 0;    /* nonsymmetric problem */
		if(compx->calculationStopTime < EPS){
			xhilf = pdiInPut->targetFinishCoolTemp;
			if( xhilf < 100.f)
				xhilf = 540.f;
			compx->calculationStopTemp = xhilf;
		}
	}
	CalculationElementSizes(compx, 2, pdiInPut->thick);
	compx->matrixElementNumber = 2 * compx->thickNodeNumber - 1;
}


int     CalculationElementSizes(COMPUTATIONAL *compx,const   int   iv,               /* 1: same length 2: var. length        */
								const   float   l)                      /* length of mesh  */
{
	float   p = 1.333f;      /* Exponent  */

	int nEle    = compx->thickNodeNumber - 1;
	if(iv == 1) {               /*  same element size  */
		float lmx     = (1.f - compx->isSymetric * .5f) * l/nEle;
		int nStep      = -1;
		while(++nStep < nEle)  
			compx->elementLength[nStep] = lmx;
		return TRUE;
	}
	if(iv == 2)	{               /*  modified element size       */
		return netx1(compx->isSymetric, nEle, l, p, &(compx->elementLength[0]));
	}
	return FALSE;
}


int     netx1(  const   int     isSymetric,   /* 0: non symm, 1: symm */
	const int       nEle, /* number of elements   */
	const   float   fAreaLen,              /* length of area             */
	const float fExponent,          /* exponent             */
	float *y)   /* element lengthes     */
{
	float   fcalc = (float)nEle*0.5f;
	float   flow;
	float   fhil;
	float   fSum = 0.0f;      /* cummulation of thickness */
	float   fUp = 0.0f;
	int nStep = -1;
	if(isSymetric == 1)   {       /* symmetrical case */
		while(++nStep < nEle) {
			flow = fUp;
			fUp = (float)(nStep+1)/(float)nEle;
			fUp = (float)pow(fUp,fExponent);
			fhil  = (fUp - flow)*fAreaLen;
			fSum += fhil;
			y[nStep] = fhil;
		}
		return TRUE;
	}                               /* symmetrical case */
	else  {       /* nonsymmetrical case */
		int nCalc = nEle/2; /* Anzahl der zu berechnenden Elementteilungen */
		float l2    = 0.5f * fAreaLen;
		while(++nStep < nCalc) {
			flow = fUp;
			fUp  = (float)(nStep+1)/fcalc;
			fUp = (float)pow(fUp,fExponent);
			fhil =  (fUp - flow)*l2;
			y[nStep]        = fhil;    /* lower SolveBoundary element */
			y[nEle-nStep-1] = fhil; /* upper SolveBoundary element */
			fSum += 2*fhil;
		}
		if((fcalc-(float)nCalc) > .1)   { /* kein Knotem in PMF */
			y[nCalc] = fAreaLen-fSum;
		}
		return TRUE;
	}                                       /* nonsymmetrical case */
}



/// @函数说明 函数简要说明-测试函数 
/// @参数 C1
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013      ?????????
void	CombinFilePath(const	char	*f_name, const int taskID, char	*pf_name)
{
	char szFilePreProcess[100];
	if(taskID == POSTPROCESS)     
		strcpy_s(szFilePreProcess,"Logs\\postprocess\\");
	else                        
		strcpy_s(szFilePreProcess,"Logs\\preprocess\\");
	strcpy(pf_name, szFilePreProcess);
	strcat(pf_name, f_name);
}

int	SetBit(const int nPos, int nVal)
{
	int	nMax = 8 * sizeof(int)	- 1;
	if(nPos > nMax)			
		return	nVal;
	if(nPos < 0)			
		return	nVal;
	nVal |= BITX(nPos);

	return nVal;
}

int	CheckBit(const	int	nPos,int nVal)
{
	if((nVal & BITX(nPos))!=0)	
		return	 TRUE;
	return FALSE;
}


int swapInt(int *x1, int *x2)
{
	int Temp = *x2;
	*x2 = *x1;
	*x1 = Temp;
	return TRUE;
}


/// @函数说明 convert bit pattern to string format
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
char*	BitToString(const int nVal, int nSize, char* szResult)
{
	int	i = -1;
	int nPos = nSize;
	while(++i < nSize)	{
		nPos -=1;
		if (CheckBit(nPos, nVal))	szResult[i] = '1';
		else						szResult[i] = '0';			
	}
	szResult[i] = '\0';

	return szResult;
}

//  convert bit pattern to StatusComment
char*	BitToStatusComment(const	int	nVal, int nSize)
{
	char *opmode[] =
	{ " Temperaturfehler\n", // Bit 0
	" Trackingfehler\n",
	"",
	"",
	"",
	"",
	"",
	"",
	" Filter Einlauftemperatur P1\n", // Bit 8
	" fTmpMax - fTmpMin > 40k\n",
	" fTmpMax - fTempPdi(EntryP1)\n",
	" Filter Einlauftemperatur P2\n",
	" TmpMax - fTmpMin > 40k\n",
	" fTmpMax - fTempPdi(EntryP2)\n",
	"",
	"",
	" fEpsSpd > 1%\n", // Bit 16
	" fEpsSpd > 2%\n",
	" fEpsSpd > 5%\n",
	" Blech bleibt liegen\n",
	" fEpsTimeAccHead\n",
	" fEpsTimeAccTail\n",
	"",
	"",
	"EOF"             // End of List marker      
	};

	static char message[1024]; // hold status message
	char boolstring[129];
	int i = 0;
	*message = '\0';

	BitToString(nVal,nSize,boolstring);
	while(strcmp(opmode[i],"EOF") != 0){
		if(boolstring[strlen(boolstring)-1-i] =='1')
			strcat(message,opmode[i]);
		i++;
	}

	return message;
}


