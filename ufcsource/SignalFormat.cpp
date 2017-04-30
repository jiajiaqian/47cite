/*! 
* Copyright (c) 2013, 东北大学轧制技术及连轧自动化国家重点实验室(RAL)
* All rights reserved
* 格式处理
* FILE SignalFormat.cpp
* 说明 概述  
*  
* 详细说明
*  
* 版本 1.0 作者 Hanbing 日期 8/6/2013
* 
*/

#include "StdAfx.h"
#include "SignalFormat.h"
#include "public.H"
#include <stdlib.h>

CSignalFormat::CSignalFormat(void)
{
}


CSignalFormat::~CSignalFormat(void)
{
}

/* x.1) put data in telegram format (DB)          */
/// @函数说明 函数简要说明-测试函数 
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/29/2013
int CSignalFormat::PostDataFormat(const    INPUTDATAPDI *pdi,const    RALCOEFFDATA *lay,RALMESDATA *mes,
								RALOUTDATA *pre_out,dbk_t   data_block[], FILE    *st_err, FILE	 *erg_f)
{
	const	float   fPDILength = pdi->length;
	const	float	fEpsLength = fPDILength * 0.1f; /* allowed tracking error */
	int		iStatusBit = 0;			/* error indication bit format  */

	float   lSampleLength = L_SEG;
	float   fPDIMeasure = fPDILength; /* more measured value than from PDI */
	int     nval = MAX_MEASURE_VALUE;
	float   fMaxSegments = fPDILength / SEGANZ; /* 最大 20 segments */

	float	eps_bls1 = 0.2f;
	float   fTempMinPyEntry = pdi->finishRollTemp - D_TEMP_P1;
	float   fTempMaxPyEntry = pdi->finishRollTemp + D_TEMP_P1;
	float   fTempMinPyExit  = pdi->targetFinishCoolTemp - D_TEMP_P5;
	float   fTempMaxPyExit  = pdi->targetFinishCoolTemp + D_TEMP_P5;
	/* a) determine 最小Position */

	float   tempMinP3U = lay->fTempLowP3U;
	float	tempMinP4 = lay->fTempLowP4; /* may be modified for quenching */
	float	tempMinP5 = lay->fTempLowP5; /* may be modified for quenching */
	float	tempMinP6 = lay->fTempLowP6; /* may be modified for quenching */
	
	//GetMinIndex 功能是算出最小的位置id号
	int nStartLastPass= GetMinIndex(mes->CurIndex, -0.01f, lay->PosP3U, mes->Position); //返回最小的 mes->Position 的参数value
	//mes->CurIndex = CheckTimeIncrements(mes->CurIndex, TIME_INCR_MIN, mes->TimeSinceP1);   // 信号有问题暂时 屏蔽 [11/1/2013 谢谦]

	/* b) 纠正 start position to fTimePos > 0 (start from 最小position) */
	if(nStartLastPass < 0) 
		nStartLastPass = 0;

	const	float fEpsPos = 0.1f;
	int nStartZeroPos = GetSignalIndex(mes->CurIndex - nStartLastPass, 0, fEpsPos, &mes->Position[nStartLastPass]);

	if((nStartZeroPos < 0) || (nStartZeroPos > (mes->CurIndex - 1)))
		nStartZeroPos = 0;
	nStartLastPass += nStartZeroPos; 
	if(fTempMinPyExit  < T_MIN_P5) 
		fTempMinPyExit  = T_MIN_P5;
	
	if(pdi->acc_mode == IC) 
		fTempMinPyEntry = TGFIMIN;
	if(fTempMinPyEntry < TGFIMIN)  
		fTempMinPyEntry = TGFIMIN;
	/* determine segment size of 板材 */
	if(lSampleLength < fMaxSegments) 
		lSampleLength = fMaxSegments;
	mes->fLengthSeg = lSampleLength;
	/* delete position and time elements after valid measurement */
	if(mes->CurIndex+1 < nval)     
		nval = mes->CurIndex+1;
	InitMES(mes);
	int nStep = -1;
	while(++nStep < MAX_MEASURE_VALUE)  {
		if(mes->TempP1[nStep]  < fTempMinPyEntry) 
			mes->TempP1[nStep]  = 0.0f;
		if(mes->TempP2L[nStep] < fTempMinPyEntry) 
			mes->TempP2L[nStep] = 0.0f;
		if(mes->TempP2U[nStep] < fTempMinPyEntry) 
			mes->TempP2U[nStep] = 0.0f;

		if(mes->TempP23[nStep]  < fTempMinPyExit) 
			mes->TempP23[nStep]  = 0.0f;

		if(mes->TempP1[nStep]  > fTempMaxPyEntry) 
			mes->TempP1[nStep]  = 0.0f;
		if(mes->TempP2L[nStep] > fTempMaxPyEntry) 
			mes->TempP2L[nStep] = 0.0f;
		if(mes->TempP2U[nStep] > fTempMaxPyEntry) 
			mes->TempP2U[nStep] = 0.0f;
		if(mes->TempP23[nStep]  > fTempMaxPyEntry) 
			mes->TempP23[nStep]  = 0.0f;
		if(mes->TempP4[nStep]  < fTempMinPyExit)  
			mes->TempP4[nStep]  = 0.0f;
		if(mes->TempP5[nStep]  < 50)  
			mes->TempP5[nStep]  = 0.0f;
		if(mes->TempP3U[nStep]  < fTempMinPyExit)  
			mes->TempP3U[nStep]  = 0.0f;
		if(mes->TempP3L[nStep]  < fTempMinPyExit)  
			mes->TempP3L[nStep]  = 0.0f;
		if(mes->TempP4[nStep]  > fTempMaxPyExit)  
			mes->TempP4[nStep]  = 0.0f;
		if(mes->TempP5[nStep]  > fTempMaxPyExit)  
			mes->TempP5[nStep]  = 0.0f;
		if(mes->TempP3L[nStep]  > fTempMaxPyExit)  
			mes->TempP3L[nStep]  = 0.0f;
		if(mes->TempP3U[nStep]  > fTempMaxPyExit)  
			mes->TempP3U[nStep]  = 0.0f;
	}

	nStep = nval-1;

	while(++nStep < MAX_MEASURE_VALUE)  
	{
		mes->Position[nStep] = 0.0f;
		mes->TimeSinceP1[nStep] = 0.0f;
	}

	if(pdi->acc_mode == IC) 
		nval = GetMaxValuePos(nval, lSampleLength, mes->Position);

	/* p1测量信息处理first entry pyrometer*/
	int nStatusTempP1 = FilterArray(nval - nStartLastPass, &mes->TempP1[nStartLastPass], TGFIMIN);

	int		nStart, nEnde;
	float	fMean, fVar, fMin, fMax;
	AnalyseArray(nval - nStartLastPass, &mes->TempP1[nStartLastPass], TGFIMIN, &fMean, &fVar, &fMin, &fMax, &nStart, &nEnde);

	if(nStatusTempP1 > 0)	
		iStatusBit = SetBit(8, iStatusBit);
	nStatusTempP1 = 0; /* delete if contractual relevant */
	if((fMax - fMin) > EPS_P_ENTRY_SPAN){
		nStatusTempP1 +=1;
		iStatusBit = SetBit(9, iStatusBit);
	}
	if(fabs(fMax - pdi->finishRollTemp) > EPS_P_ENTRY_PDI){
		nStatusTempP1 +=1;
		iStatusBit = SetBit(10, iStatusBit);
	}

	//offset_bls1 函数功能需继续了解
	float bls1_P1 = OffsetBls1(nval - nStartLastPass, TGFIMIN, lay->PosP1, 
		&mes->Position[nStartLastPass], &mes->TempP1[nStartLastPass]);

	/* P2U测量信息处理second entry pyrometer */
	int nStatusTempP2U = FilterArray(nval - nStartLastPass, &mes->TempP2U[nStartLastPass], TGFIMIN);

	AnalyseArray(nval - nStartLastPass, &mes->TempP2U[nStartLastPass], TGFIMIN, &fMean, &fVar, &fMin, &fMax, &nStart, &nEnde);

	if(nStatusTempP2U > 0)	
		iStatusBit = SetBit(11, iStatusBit);
	nStatusTempP2U = 0; /* delete if contractual relevant */

	if((fMax - fMin) > EPS_P_ENTRY_SPAN){
		nStatusTempP2U +=1;
		iStatusBit = SetBit(12, iStatusBit);
	}

	if(fabs(fMax - pdi->finishRollTemp) > EPS_P_ENTRY_PDI){
		nStatusTempP2U +=1;
		iStatusBit = SetBit(13, iStatusBit);
	}

	float bls1_P2U = OffsetBls1(nval - nStartLastPass, TGFIMIN,
		lay->PosP2U, &mes->Position[nStartLastPass], 
		&mes->TempP2U[nStartLastPass]);

	if((pdi->acc_mode == IC) && (lay->iTempIcEn == 3))
		bls1_P2U = OffsetBls1(nval - nStartLastPass, TGFIMIN,
		lay->PosP2U, &mes->Position[nStartLastPass], &mes->TempP2U[nStartLastPass]);

	/* P3U测量信息处理*/
	ForceSignalToDefaultOutside(0.0f, lay->PosP3U - fEpsLength, lay->PosP3U + fPDILength + fEpsLength, 
		mes->CurIndex, 0, mes->CurIndex, mes->Position, mes->TempP3U);

	if((pdi->targetFinishCoolTemp <= lay->fTempLowP3U) ||(pdi->targetFinishCoolTemp <= T_STOP_MIN))	{
		ForceSignalToDefaultInside(pdi->targetFinishCoolTemp, lay->PosP3U, lay->PosP3U + fPDILength, mes->CurIndex, 0,
			mes->CurIndex, mes->Position, mes->TempP3U);
		tempMinP3U = pdi->targetFinishCoolTemp - 1.f;
	}
	float bls1_P3U = OffsetBls1(nval, tempMinP3U,lay->PosP3U, mes->Position, mes->TempP3U);


	/* P4测量信息处理*/
	ForceSignalToDefaultOutside(0.0f, lay->PosP4 - fEpsLength, lay->PosP4 + fPDILength + fEpsLength, 
		mes->CurIndex, 0, mes->CurIndex, mes->Position, mes->TempP4);

	if((pdi->targetFinishCoolTemp <= lay->fTempLowP4) ||(pdi->targetFinishCoolTemp <= T_STOP_MIN)){
		ForceSignalToDefaultInside(pdi->targetFinishCoolTemp, lay->PosP4, lay->PosP4 + fPDILength, mes->CurIndex, 0,
			mes->CurIndex, mes->Position, mes->TempP4);
		tempMinP4 = pdi->targetFinishCoolTemp - 1.f;
	}
	float bls1_P4 = OffsetBls1(nval, tempMinP4,lay->PosP4, mes->Position, mes->TempP4);

	/* P5测量信息处理*/
	ForceSignalToDefaultOutside(0.0f, lay->PosP5 - fEpsLength , lay->PosP5 + fPDILength + fEpsLength, 
		mes->CurIndex, 0, mes->CurIndex, mes->Position, mes->TempP5);

	if((pdi->targetFinishCoolTemp <= lay->fTempLowP5) ||(pdi->targetFinishCoolTemp <= 50)){
		ForceSignalToDefaultInside(pdi->targetFinishCoolTemp, lay->PosP5, lay->PosP5 + fPDILength, mes->CurIndex, 0,
			mes->CurIndex, mes->Position, mes->TempP5);
		tempMinP5 = pdi->targetFinishCoolTemp - 1.f;
	}
	
	float bls1_P5 = OffsetBls1(nval, tempMinP5,lay->PosP5, mes->Position, mes->TempP5);

	float   bls1 = bls1_P2U;
	if((bls1_P1 > eps_bls1) && (bls1_P2U < eps_bls1))
		bls1 = bls1_P1;
	if((bls1_P3U > eps_bls1) && (bls1_P2U < eps_bls1)) 
		bls1 = bls1_P3U;

	bls1 = 0.0f;

	float   zeit_P[MAX_MEASURE_VALUE],val_P[MAX_MEASURE_VALUE];
	InitFloatArray(nval, zeit_P);   /* first measuring pyrometer */
	InitFloatArray(nval, val_P);

	int nx_P1 = GetLinearInterpolation(lSampleLength, lay->PosP1 - bls1, nval - nStartLastPass,
		&mes->TimeSinceP1[nStartLastPass], &mes->Position[nStartLastPass], 
		&mes->TempP1[nStartLastPass],zeit_P, val_P);

	TransformTelegramTime(2001, nx_P1, zeit_P, &(data_block[1].data_word[0]));
	TransformTelegramTemperature(2002, nx_P1, val_P,  &(data_block[2].data_word[0]));

	InitFloatArray(nval, zeit_P);   /* second pyrometer */
	InitFloatArray(nval, val_P);

	int nx_P2U = GetLinearInterpolation(lSampleLength, lay->PosP2U - bls1, nval - nStartLastPass, 
		&mes->TimeSinceP1[nStartLastPass], &mes->Position[nStartLastPass], &mes->TempP2U[nStartLastPass], zeit_P, val_P);
	if((pdi->acc_mode == IC) && (lay->iTempIcEn == 3))
		nx_P2U = GetLinearInterpolation(lSampleLength, lay->PosP2U - bls1, nval - nStartLastPass, 
		&mes->TimeSinceP1[nStartLastPass], &mes->Position[nStartLastPass], &mes->TempP2U[nStartLastPass], zeit_P, val_P);

	if(nx_P2U < 1)
		nx_P2U = GetLinearInterpolation(lSampleLength, lay->PosP2U - bls1, nval - nStartLastPass, 
		&mes->TimeSinceP1[nStartLastPass], 
		&mes->Position[nStartLastPass], &mes->TempP2U[nStartLastPass], zeit_P, val_P);
	TransformTelegramTime(2003, nx_P2U, zeit_P, &(data_block[3].data_word[0]));
	TransformTelegramTemperature(2004, nx_P2U, val_P,  &(data_block[4].data_word[0]));

	InitFloatArray(nval, zeit_P);   /* exit pyrometer */
	InitFloatArray(nval, val_P);
	int nx_P3U = GetLinearInterpolation(lSampleLength, lay->PosP3U - bls1, nval - nStartLastPass,
		&mes->TimeSinceP1[nStartLastPass], &mes->Position[nStartLastPass], &mes->TempP3U[nStartLastPass],	zeit_P, val_P);

	TransformTelegramTime(2007, nx_P3U, zeit_P, &(data_block[7].data_word[0]));
	TransformTelegramTemperature(2008, nx_P3U, val_P,  &(data_block[8].data_word[0]));
	/* 低 range Temperature measurement */
	TransformTelegramTemperature(2009, nx_P3U, val_P,  &(data_block[9].data_word[0]));

	InitFloatArray(nval, zeit_P);   /* exit pyrometer */
	InitFloatArray(nval, val_P);
	int nx_P4 = GetLinearInterpolationLengthLimit(lSampleLength, lay->PosP4 - bls1-2, nval - nStartLastPass,
		&mes->TimeSinceP1[nStartLastPass], &mes->Position[nStartLastPass], &mes->TempP4[nStartLastPass], zeit_P, val_P);

	TransformTelegramTime(2010, nx_P4, zeit_P, &(data_block[10].data_word[0]));
	TransformTelegramTemperature(2011, nx_P4, val_P,  &(data_block[11].data_word[0]));
	/* 低 range Temperature measurement */
	InitFloatArray(nval, zeit_P);   /* exit pyrometer */
	InitFloatArray(nval, val_P);
	int nx_P5 = GetLinearInterpolationLengthLimit(lSampleLength, lay->PosP5 - bls1-2, nval - nStartLastPass,
		&mes->TimeSinceP1[nStartLastPass], &mes->Position[nStartLastPass], &mes->TempP5[nStartLastPass],zeit_P, val_P);
	TransformTelegramTime(2012, nx_P5, zeit_P, &(data_block[12].data_word[0]));
	TransformTelegramTemperature(2013, nx_P5, val_P,  &(data_block[13].data_word[0]));

	InitFloatArray(nval, zeit_P);   /* ENTRY_ACC */
	if((nx_P1 * lSampleLength) > fPDIMeasure) 
		fPDIMeasure = nx_P1 * lSampleLength;
	if((nx_P2U * lSampleLength) > fPDIMeasure)
		fPDIMeasure = nx_P2U * lSampleLength;
	if((nx_P3U * lSampleLength) > fPDIMeasure) 
		fPDIMeasure = nx_P3U * lSampleLength;
	if((nx_P4 * lSampleLength) > fPDIMeasure) 
		fPDIMeasure = nx_P4 * lSampleLength;
	
	// 抽离出开冷时间 [9/26/2013 谢谦]
	int nx_1 = GetInterpolationTime(lSampleLength, fPDIMeasure, lay->PosZon1Entry - bls1, mes->CurIndex - nStartLastPass,
		&mes->TimeSinceP1[nStartLastPass], &mes->Position[nStartLastPass], zeit_P);
	TransformTelegramTime(2005, nx_1, zeit_P, &(data_block[5].data_word[0]));

	InitFloatArray(nval, zeit_P);   /* EXIT_ACC */
	float l_acc = ActiveCoolingLength(&pre_out->PreResult, lay->PosZon1Entry - bls1, lay->PosZon3Exit - bls1);

	int nx_2 = GetInterpolationTime(lSampleLength, fPDIMeasure, lay->PosZon1Entry+l_acc - bls1, 
		mes->CurIndex - nStartLastPass,	&mes->TimeSinceP1[nStartLastPass], &mes->Position[nStartLastPass], zeit_P);
	TransformTelegramTime(2006, nx_1, zeit_P, &(data_block[6].data_word[0]));

 
	for(nStep=0;nStep<lay->nHeader;nStep++){
		if(pre_out->PreResult.upNuzzleNumber[nStep] !=0 || pre_out->PreResult.downNuzzleNumber[nStep] != 0)
			break;
	}

	mes->InitSpeed = AnalysePostion(nval - nStartLastPass, lay->fPosHeader[nStep] - bls1, &mes->Position[nStartLastPass], &mes->Speed[nStartLastPass]);
	mes->ExitSpeed = AnalysePostion(nval - nStartLastPass, lay->fPosHeader[nStep] + fPDILength - bls1,	&mes->Position[nStartLastPass], &mes->Speed[nStartLastPass]);

	if((nStatusTempP1 > 0) && (nStatusTempP2U > 0)) 
		iStatusBit = SetBit(0, iStatusBit);
	return(iStatusBit);
}

/// @函数说明 函数简要说明-测试函数 
/// @参数 nmax
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 8/1/2013
float   CSignalFormat::AnalysePostion(
	const   int     nmax, /* 最大 possible number of value */
	const   float   fTimePos,  /* 参考 Position or time for value */
	const   float   *x,   /* array with position or times */
	const   float   *y)   /* array with value over position or times */
{
	float   eps = 0.01f;
	int nMaxFound = nmax;  /* nMaxFound - last increasing index <= nmax*/

	if(fTimePos < x[0])      
		return(y[0]);

	int nStep = -1;
	while(++nStep < nmax){  /* estimate last increasing position or time */
		if(x[nStep] < eps)
			break;
	}   /* estimate last increasing position or time */
	nMaxFound = nStep;

	if(fTimePos > x[nMaxFound-1])  
		return(y[nMaxFound-1]);

	nStep = 0;
	while(++nStep < nMaxFound){
		if(fTimePos < x[nStep]) 
			break;
	}
	if((nStep+1) > (nMaxFound-1))
		return(y[nMaxFound-1]);
	
	float diff = x[nStep] - x[nStep-1];
	if(diff < eps) 
		return (y[nStep]);
	float r = (fTimePos - x[nStep-1])/diff;
	return y[nStep-1] * (1.f-r) + y[nStep] *r;
}

/*  estimate active cooling length of the system */
float CSignalFormat::ActiveCoolingLength(
	const    PRERESULT *PreResult,      /* PREPROCESS telegram */
	const    float   entry_acc,  /* position ENTRY_ACC */
	const    float   exit_max_acc) /* end position last header */
{
	float fMaxSegments =  exit_max_acc -  entry_acc;
	int nOpenHeader = 0;
	int nStep = -1;

	//int nMax = PreResult->headerNo + 1;   // 从后面开启时存在问题 [9/26/2013 谢谦]
	//while(++nStep < nMax) {
	//	if(PreResult->upNuzzleNumber[nStep] > 0) 
	//		nOpenHeader++;
	//}

	//float fAcc = PreResult->coolBankLen * nOpenHeader;  // 屏蔽 [9/26/2013 谢谦]
	float fAcc = PreResult->coolBankLen * PreResult->headerNo;

	if(fAcc > fMaxSegments) 
		fAcc = fMaxSegments;

	return fAcc;
}

/* returns signal index if signal exceeds limit */
/* and(nSearch starts from 0 or                 */
/*     fSignal[iSerch -1] < fLimit)             */
/* if not returns -1                            */
int	CSignalFormat::GetSignalIndex(
	const	int		nPos,			/* 序号value */
	const	int		nSearch,	/* start index for search */
	const	float	fLimit,		/* 限制value*/
	const	float	*fSignal)	/* Signal [nPos] */
{
	int	nIndex = nSearch - 1;

	if((nSearch < 0) || (nSearch > nPos) || (nPos < 1))	return(-1);

	int iHighM1 = 1;
	if(nIndex == -1) 
		iHighM1 = 0;
	else 
		if(fSignal[nIndex] < fLimit) 
			iHighM1 = 0;

	float	fSignalAct;
	while(++nIndex < nPos){
		fSignalAct = fSignal[nIndex]; 
		if((fSignalAct >  fLimit) && (iHighM1 == 0))	
			return nIndex;
		if(fSignalAct <= fLimit) 
			iHighM1 = 0;
	}
	return(-1);
}

/*  determines 序号value with increasing       */
/*  time increments                                           */
int	CSignalFormat::CheckTimeIncrements(
	const	int		nPos,			/* 序号value */
	const	float	fEpsTime,	/* required time increment between time steps */
	const	float	*fTimeVal)	/* time value */
{
	int nStep = 0;
	float	fTimePrev = fTimeVal[0];
	float	fTimeAct;
	while(++nStep < nPos)
	{
		fTimeAct = fTimeVal[nStep];
		if((fTimeAct - fTimePrev) < fEpsTime) 
			break;
		fTimePrev = fTimeAct;
	}

	return nStep;
}

/* estimate index with最小value                 */
/* stop search, if 最大value was achieved first  */
/* time                                              */
/* n - No. of elements                               */
/* Exceptions                                        */
/* returns -1 if no 最小found                    */
int	CSignalFormat::GetMinIndex(
	const	int		nPos,
	const	float	fEps,
	const	float	fMax,
	const	float	*fVal)
{
	int		nStep		= -1;
	int		nMin	= -1;
	float	fMin	= fVal[0] - fEps;
	while(++nStep < nPos){
		if(fVal[nStep] > fMax)	
			break;
		if(fVal[nStep] < fMin){
			fMin = fVal[nStep];
			nMin = nStep;
		}
	}

	return nMin;
}

int CSignalFormat::InitFloatArray(const   int n, float   *x)
{
	int nStep = -1;
	while(++nStep < n) x[nStep] = 0.0f;
	return(1);
}

/* initilaize mes value, not set by 过程      */
int	CSignalFormat::InitMES(RALMESDATA *mes)
{
	int nStep = -1;
	while(++nStep < ANZ_HUB){
		mes->Tracking[nStep].TimeHeadRef = 0.0f;
		mes->Tracking[nStep].TimeHeadRt	= 0.0f;
		mes->Tracking[nStep].TimeHeadPlt = 0.0f;
		mes->Tracking[nStep].TimeTailRef = 0.0f;
		mes->Tracking[nStep].TimeTailRt	= 0.0f;
		mes->Tracking[nStep].TimeTailPlt = 0.0f;
		mes->Tracking[nStep].fSpdRefMean = 0.0f;
		mes->Tracking[nStep].fSpdRefMin	= 0.0f;
		mes->Tracking[nStep].fSpdRefMax	= 0.0f;
		mes->Tracking[nStep].fSpdActMean = 0.0f;
		mes->Tracking[nStep].fSpdActMin	= 0.0f;
		mes->Tracking[nStep].fSpdActMax  = 0.0f;
		mes->Tracking[nStep].fSpdDevMean = 0.0f;
		mes->Tracking[nStep].fSpdDevMin	= 0.0f;
		mes->Tracking[nStep].fSpdDevMax  = 0.0f;
	}
	mes->TempSurfCalcStartMin = 0.0f;
	mes->TempSurfCalcStartMax = 0.0f;
	mes->TempSurfCalcStartAve = 0.0f;
	mes->TempCentCalcStartAve = 0.0f;
	mes->TempSurfMeasStartAve = 0.0f;
	mes->TempSurfCalcStopMin = 0.0f;
	mes->TempSurfCalcStopMax = 0.0f;
	mes->TempSurfCalcStopAve = 0.0f;
	mes->TempCentCalcStopAve = 0.0f;
	mes->TempSurfMeasStopAve = 0.0f;
	mes->TempSurfMeasStopStd = 0.0f;
	mes->CoolRateSurfCalcMin = 0.0f;
	mes->CoolRateSurfCalcMax = 0.0f;
	mes->CoolRateSurfCalcAve = 0.0f;
	mes->CoolRateCentCalcAve = 0.0f;
	mes->TempAveCalcStartAve = 0.0f;
	mes->TempAveCalcStopAve = 0.0f;
	mes->TempAveCalcStopMin = 0.0f;	
	mes->TempAveCalcStopMax = 0.0f;	
	mes->CoolRateAveCalcAve = 0.0f;
	mes->CoolRateAveCalcMin = 0.0f;	
	mes->CoolRateAveCalcMax = 0.0f;	
	mes->CoolTempColStopCalMinSeg = 0.0f;    
	mes->CoolTempColStopCalMaxSeg = 0.0f;    
	mes->CoolRateAvgCalMinSeg = 0.0f;    
	mes->CoolRateAvgCalMaxSeg = 0.0f;    
	mes->CoolRateAvgCalLimitOkSeg = 0;
	return TRUE;
}

float   CSignalFormat::OffsetBls1(
	const   int		nmax,		/* 最大 number of value */
	const   float   fMinVal,	/* 最小 allowed measured value */
	const   float   syn,		/* position of synchronization point */
	const   float   *x,			/* position value */
	float   *y)			/* corresponding value vs. position */
{
	int nStep = -1;
	while(++nStep < nmax)  {
		if(y[nStep] > fMinVal) 
			break;
		y[nStep] = 0.0f; /* reset 低 value to zero */
	}

	float   offsetx = 0.0f;
	if(nStep < (nmax - 2))
		offsetx = syn - x[nStep];
	nStep = nmax-2;
	while(++nStep < MAX_MEASURE_VALUE){
		if(y[nStep] < fMinVal)	
			y[nStep] = 0.0f;
	}
	return offsetx;
}

/*  force signal to fDef inside 间隔          */
/*  [xa;xb] if y[i] < fDef                        */
/*  Start point of serch:                         */
/*  x[nStart] to x[nEnde-1] (exclude)             */
int	CSignalFormat::ForceSignalToDefaultInside(
	const	float	fDef,	/* 默认value */
	const	float	xa,		/* 间隔 [xa,xb] */
	const	float	xb,		/* 间隔 [xa,xb] */
	const	int		nPos,		/* No of value */
	const	int		nStart,	/* 开始搜索位置 */
	const	int		nEnde,	/* 结束搜索位置 */
	const	float	*x,		/* 横坐标 value */
	float	*y)		/* 纵坐标value */
{
	if(	(nStart < 0) ||  (nEnde  < nStart) || (nStart > nPos) || (nPos < 1))	
		return FALSE;

	int		nLast	= nEnde;
	if(nEnde > nPos)	
		nLast = nPos;

	float	fAbsc;
	int		nStep		= nStart - 1;
	while(++nStep < nLast)	{
		fAbsc = x[nStep];
		if(	(fAbsc >= xa) && 
			(fAbsc <= xb) &&
			(y[nStep] < fDef))	y[nStep] = fDef;
	}
	return TRUE;
}

/*  force signal to fDef outside 间隔         */
/*  [xa;xb]                                       */
/*  Start point of serch:                         */
/*  x[nStart] to x[nEnde-1] (exclude)             */
int	CSignalFormat::ForceSignalToDefaultOutside(
	const	float	fDef,	/* 默认value */
	const	float	xa,		/* 间隔 [xa,xb] */
	const	float	xb,		/* 间隔 [xa,xb] */
	const	int		nPos,		/* No of value */
	const	int		nStart,	/* 开始搜索位置 */
	const	int		nEnde,	/* 结束搜索位置 */
	const	float	*x,		/* 横坐标 value */
	float	*y)		/* 纵坐标value */
{
	if(	(nStart < 0) || (nEnde  < nStart) || (nStart > nPos) || (nPos < 1))	
		return FALSE;

	int		nLast	= nEnde;
	if(nEnde > nPos)	
		nLast = nPos;

	float	fAbsc;
	int		nStep		= nStart - 1;
	while(++nStep < nLast)	{
		fAbsc = x[nStep];
		if((fAbsc < xa) || (fAbsc > xb))	
			y[nStep] = fDef;
	}
	return TRUE;
}

int CSignalFormat::TransformTelegramTime(
	const   int     id,     /* id of the telegram */
	const   int     nx,     /* No. of time value */
	const   float   *tx,    /* time in seconds */
	int *ti)    /* integer format */
{
	int nStep = -1;
	int nTel = OFFSET_SEG - 1;

	nStep = -1;
	while(++nStep < L_TEL)	
		ti[nStep] = 0;

	ti[0]  = id;
	ti[2]  = (int)tx[0];
	ti[3]  = (int)(100.f*tx[0] - 100.f * ti[2]);
	float fTimeOffset = (float)ti[2] + 0.01f*(float)ti[3];

	nStep = -1;
	while(++nStep < nx)
	{
		nTel++;
		ti[nTel] = (int)(100.f*(tx[nStep] - fTimeOffset));
	}
	return nStep;
}

int CSignalFormat::TransformTelegramTemperature(
	const   int     id,     /* id of the telegram */
	const   int     nx,     /* No. of time value */
	const   float   *tx,    /* temp in C */
	int *ti)    /* integer format */
{
	int nTel = OFFSET_SEG - 1;

	int nStep = -1;
	while(++nStep < L_TEL)	
		ti[nStep] = 0;
	ti[0]  = id;

	nStep = -1;
	while(++nStep < nx){
		nTel++;
		ti[nTel] = (int)tx[nStep];
	}
	return nStep;
}

int	CSignalFormat::StatisticalValueResult(PRERESULT *PreResult,/* setup 信息 */
	                                                             FATsegs		*xx,  /* Struktur     */
																 const	INPUTDATAPDI *pdi,
	                                                             FILE		*erg_f,       /* 结果FILE */	
																 const	int			icall,	  /* No. of call  */
																 int offLineSelfLearn)      
{
	float	xMin, xMax;
	float	fPosSeg[SEGANZ];
	float	fLengthSeg = pdi->length;
	float	fTempFilterMean = 0.0f;
	float	fWeight;
	float	fSumCR = 0.0f;
	float	fSumTx = 0.0f;

	float fLengthRatioAcc = CoolingLengthRatio(PreResult);

	int		nResult	= 0;
	nResult += CalculateStatisticalValue(&xx->Tm_p1_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tm_p2U_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tm_p3U_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tc_sct_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tc_fct_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tm_p4_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tb_p4_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tm_p5_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tb_p5_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tr_fct_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tx_fct_sv, icall);
	nResult += CalculateStatisticalValue(&xx->CRcalc_sv, icall);
	nResult += CalculateStatisticalValue(&xx->CRreal_sv, icall);
	nResult += CalculateStatisticalValue(&xx->ti_cool_sv, icall);
	nResult += CalculateStatisticalValue(&xx->air1_sv, icall);
	nResult += CalculateStatisticalValue(&xx->air2_sv, icall);
	nResult += CalculateStatisticalValue(&xx->CRcore_sv, icall);
	nResult += CalculateStatisticalValue(&xx->CRsurf_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tss_sct_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tsc_sct_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tss_fct_sv, icall);
	nResult += CalculateStatisticalValue(&xx->Tsc_fct_sv, icall);

	xx->seg = icall;
	float	fTcMean = xx->Tc_fct_sv.mean; //cal
	float	fTrMean = xx->Tr_fct_sv.mean; //mea + cal
	if(icall > 0) 
		fLengthSeg /= icall;

	int nStep = -1;
	float	fTdiff;
	float	fTimeAcc;
	float	fTemp1; /* first  predicted Temperature */
	float	fTemp2U; /* second predicted Temperature */
	while(++nStep < icall)	{
		fPosSeg[nStep]      = nStep * fLengthSeg;
		fTdiff			 = xx->fTc_fct[nStep] - fTcMean;
		fTemp1			 = fTdiff + fTrMean;
		fTemp2U		 = xx->fTr_fct[nStep];
		if(fTemp2U > fTemp1) 
			fTemp1 = fTemp2U;
		xx->fTx_fct[nStep]  = fTemp1;
		fWeight          = (float)nStep / (float)(nStep+1);
		fTempFilterMean  = fTempFilterMean * fWeight + fTemp1/ (nStep + 1);
		fTimeAcc         =  xx->fti_cool[nStep];
		if(fTimeAcc < 0.1f * xx->ti_cool_sv.mean) 
			fTimeAcc = xx->ti_cool_sv.mean;
		if(fTimeAcc < 0.1f) 
			fTimeAcc = 1.0f;
		xx->fCR_fct[nStep]  = (xx->fTc_sct[nStep] - fTemp1) * fLengthRatioAcc / fTimeAcc;
		fSumCR += xx->fCR_fct[nStep];
		fSumTx += xx->fTx_fct[nStep];
	}
	xx->Tx_fct_sv.mean = fTempFilterMean;
	if(icall > 0)
	{
		xx->fCR_fctMean = fSumCR / icall;
		xx->fTx_fctMean = fSumTx / icall;
	}
	PrintStatisticalResult(xx, erg_f);

	if (PreResult->aveTmP1<700) {
		PreResult->aveTmP1=xx->Tm_p1_sv.mean;   //谢谦 加入 2012-8-7
	}
	PreResult->aveTmP2u=xx->Tm_p2U_sv.mean;  //谢谦 加入 2012-11-5
	PreResult->aveTmP3u=xx->Tm_p3U_sv.mean;   //谢谦 加入 2012-8-7
	//PreResult->aveTmP4=xx->Tm_p4_sv.mean;      //谢谦 加入 2012-8-7   屏蔽 2015-5-11 有时不能正确的进行后计算，导致此数值不准

	nResult = CalculateAdaptionValues(xx, pdi, erg_f,offLineSelfLearn);

	int		nStart, nEnde;

	ModifySignalFilterLow2Peak(pdi->targetFinishCoolTemp, pdi->targetFinishCoolTemp - 0.5f * EPS_P_EXIT_SPAN, 
		icall, fPosSeg, xx->fTx_fct);
	AnalyseArray(icall, xx->fTx_fct, 0.5f * T_MIN_P5, &fTrMean, &fSumCR, 
		&xMin, &xMax, &nStart, &nEnde); 
	ModifySignalFilterMeanStandard(pdi->targetFinishCoolTemp, 0.5f * EPS_P_EXIT_SPAN, 1.0f - TSmoothEXIT, nEnde + 1, 
		fPosSeg, xx->fTx_fct, xx->fTx_fct);
	xx->fTx_fctMean = fTrMean;
	PrintStatisticalResultSecondRow(xx, erg_f);
	return(nResult);
}
	
//冷却长度比
float	CSignalFormat::CoolingLengthRatio(const	PRERESULT *PreResult)
{
	float	fLengthRatioAcc = 1.0;
	int		nSumTop = 0; /* No. of active headers */
	int		nDiff;
	int		nStep = -1;//HEAD_1_TOP - 1;
	int		iFirstActive	= -1;	/* index first active header */
	int		iLastActive		= -1;	/* index last active header */
	int		nEnde = PreResult->headerNo + 1;//HEAD_1_TOP + PreResult->headerNo + 1;
	while(++nStep < nEnde)	{
		if(PreResult->upNuzzleNumber[nStep] > 0)	{	/* flow > 0 */
			/* determine first active header */
			if(iFirstActive == -1)	
				iFirstActive = nStep;
			/* set last active header */
			iLastActive = nStep;
			/* increment active header number */
			nSumTop++;
		} /* flow > 0 */
	}
	nDiff = iLastActive - iFirstActive + 1;
	if(nDiff < 1)		return (fLengthRatioAcc);
	if(nSumTop < 1)		return (fLengthRatioAcc);
	if(nSumTop > nDiff)	return (fLengthRatioAcc);
	fLengthRatioAcc = ((float)nSumTop) / ((float)nDiff);
	return(fLengthRatioAcc);
}

int	CSignalFormat::CalculateStatisticalValue(	s_val_t	*s,	/*	statistical value	*/	
	                                                                    const	int		iv)	/* = 0: first call		*/
{
	int	ivx = s->n;
	float std2;
	const float fEpsStdMin = 0.1e-4f;
	s->mean = 0.0f;
	s->std  = 0.0f;
	if(ivx != iv)
		return FALSE;
	if(ivx < 1)	
		return FALSE;
	s->mean = s->s1/ivx;
	if(ivx < 2)
		return TRUE;

	std2 = (s->s2 - ivx * s->mean * s->mean)/(ivx-1);
	if((float) std2 < fEpsStdMin)	
		s->std = 0.0f;
	else							
		s->std = (float)sqrt(std2);
	return TRUE;
}

/*      print result of statistical value first print */
int	CSignalFormat::PrintStatisticalResult(FATsegs *xx, FILE		*erg_f)
{
	char *format =" %5.1f %6.1f %6.1f %5.1f %5.1f %6.1f %4.1f %4.1f";
	fprintf(erg_f,"\n MEAN %5.1f", xx->Tm_p1_sv.mean);
	fprintf(erg_f,format,
		xx->Tm_p2U_sv.mean, xx->Tc_sct_sv.mean, xx->Tc_fct_sv.mean,
		xx->Tm_p4_sv.mean, xx->Tb_p4_sv.mean, xx->Tr_fct_sv.mean,
		xx->CRcalc_sv.mean, xx->CRreal_sv.mean);
	fprintf(erg_f," %5.1f %5.1f %5.1f", xx->air1_sv.mean,xx->ti_cool_sv.mean,
		xx->air2_sv.mean);
	fprintf(erg_f,"\n STD  %5.2f", xx->Tm_p1_sv.std);
	fprintf(erg_f,format,
		xx->Tm_p2U_sv.std, xx->Tc_sct_sv.std, xx->Tc_fct_sv.std,
		xx->Tm_p4_sv.std, xx->Tb_p4_sv.std, xx->Tr_fct_sv.std,
		xx->CRcalc_sv.std, xx->CRreal_sv.std);
	fprintf(erg_f," %5.1f %5.1f %5.1f", xx->air1_sv.std,xx->ti_cool_sv.std,
		xx->air2_sv.std);
	fprintf(erg_f,"\n MAX  %5.1f", xx->Tm_p1_sv.max);
	fprintf(erg_f,format,
		xx->Tm_p2U_sv.max, xx->Tc_sct_sv.max, xx->Tc_fct_sv.max,
		xx->Tm_p4_sv.max, xx->Tb_p4_sv.max, xx->Tr_fct_sv.max,
		xx->CRcalc_sv.max, xx->CRreal_sv.max);
	fprintf(erg_f," %5.1f %5.1f %5.1f", xx->air1_sv.max,xx->ti_cool_sv.max,
		xx->air2_sv.max);
	fprintf(erg_f,"\n MIN  %5.1f", xx->Tm_p1_sv.min);
	fprintf(erg_f,format,
		xx->Tm_p2U_sv.min, xx->Tc_sct_sv.min, xx->Tc_fct_sv.min,
		xx->Tm_p4_sv.min, xx->Tb_p4_sv.min, xx->Tr_fct_sv.min,
		xx->CRcalc_sv.min, xx->CRreal_sv.min);
	fprintf(erg_f," %5.1f %5.1f %5.1f", xx->air1_sv.min,xx->ti_cool_sv.min,	xx->air2_sv.min);
	return TRUE;
}

int CSignalFormat::CalculateAdaptionValues(	FATsegs	*xx,
	const	INPUTDATAPDI *pdi,
	FILE	*erg_f,
	int offLineselfLearn)
{
	const	float	fTempEntryMeas		= 800.0f;  //add by xie 9-22
	float	fTempExitMeas=0.0f;
	float	fTempExitMeasMax=0.0f;
	float	fTempDiffAim=0.0f;
	if (1==offLineselfLearn){
		fTempExitMeas		= xx->Tc_fct_sv.mean;
		fTempExitMeasMax= xx->Tc_fct_sv.min;   // modify by xie 8-22
	}
	else{
		fTempExitMeas		= xx->Tm_p4_sv.mean;
		fTempExitMeasMax	= xx->Tm_p4_sv.max;
	}

	const	float	fTempDiffMeas		= fTempEntryMeas - fTempExitMeas;
	const	float	fTempDiffMeasMin	= fTempEntryMeas - fTempExitMeasMax;
	const	float	fTempDiffCalc		= fTempEntryMeas - xx->Tc_fct_sv.mean;

	float offLineDevation=abs(xx->Tc_fct_sv.mean-xx->Tm_p4_sv.mean);   //add by xie 9-3
	float onLineDevation=abs(xx->Tm_p4_sv.mean-pdi->targetFinishCoolTemp);    //add by xie 9-3

	if (1==offLineselfLearn)
		fTempDiffAim		= fTempEntryMeas - xx->Tm_p4_sv.mean;
	else
		fTempDiffAim		= fTempEntryMeas - pdi->targetFinishCoolTemp;

	const	float	fEpsTemp			= 0.1f;  
	float   xx1 = 1.f;
	float   xx2 = 1.f;
	float	xx1Max;	/* 最大 自适应value板材 */
	float	xx1Min; /* 最小自适应value板材 */

	const	float	dTemMax = pdi->controlAcc.fAdaptStepSizeTmp;
	if(xx->CRcalc_sv.n > IMIN_AD) {
		xx->res[0] = 1.f;	/* number of 自适应value	*/
		xx->res[1] = xx->CRreal_sv.mean/xx->CRcalc_sv.mean;	/* whole length	*/
		if(fTempDiffAim > fEpsTemp)	{
			if (1==offLineselfLearn){
				if ((offLineDevation>5.0f) && (offLineDevation<150.0f) ){
					xx1 =    fTempDiffAim/fTempDiffMeas;
				}				
			}
			else{
				if ((onLineDevation>5.0f) &&(onLineDevation<150.0f)){
					xx1 = fTempDiffMeas  / fTempDiffAim;
				}
			}

			xx->res[16] = xx1;

			xx1Min = (fTempDiffAim - dTemMax) / fTempDiffAim;
			xx1Max = (fTempDiffAim + dTemMax) / fTempDiffAim;
			if(	xx1Min < pdi->controlAcc.fAdaptLimitMin) 
				xx1Min = pdi->controlAcc.fAdaptLimitMin;
			if(	xx1Max > pdi->controlAcc.fAdaptLimitMax) 
				xx1Max = pdi->controlAcc.fAdaptLimitMax;
			if(xx1 < xx1Min) 
				xx1 = xx1Min;
			if(xx1 > xx1Max)
				xx1 = xx1Max;
			//xx2 = fTempDiffMeasMin / fTempDiffAim;
			if (1==offLineselfLearn) {
				if ((offLineDevation>5.0f)&&(offLineDevation<150.0f))
					xx2 =    fTempDiffAim/fTempDiffMeasMin;
			}
			else{
				if ((onLineDevation>5.0f)&&(onLineDevation<150.0f))
					xx2 = fTempDiffMeasMin / fTempDiffAim;
			}

			if(xx2 < xx1Min)
				xx2 = xx1Min;
			if(xx2 > xx1Max) 
				xx2 = xx1Max;
			xx->res[2]		= (1.f - WGHT_MEAN_MAX) * xx1 + WGHT_MEAN_MAX * xx2;
			xx->res[17]	= xx1Min;
			xx->res[18]	= xx1Max;
			if(pdi->acc_mode == DQ)
				xx->res[2] = xx2;
		}

		fprintf(erg_f,"\n\t adaption No. Segments      : %8d", xx->CRcalc_sv.n);
		fprintf(erg_f,"\n\t 自适应valueCR          : %8.4f", xx->res[1]);
		fprintf(erg_f,"\n\t 自适应valueTemperature : %8.4f (Mean Temp)", xx1);
		fprintf(erg_f,"\n\t 自适应valueTemperature : %8.4f (Max  Temp)", xx2);
		fprintf(erg_f,"\n\t 自适应valueTemperature : %8.4f (Combination No Limits)",
			xx->res[16]);
		fprintf(erg_f,"\n\t adaption Limit Temperature : %8.4f [度]", dTemMax);
		fprintf(erg_f,"\n\t 自适应value低 Limit   : %8.4f", xx->res[17]);
		fprintf(erg_f,"\n\t 自适应valueHigh Limit  : %8.4f", xx->res[18]);
		fprintf(erg_f,"\n\t 自适应valueTemperature : %8.4f (Combination, Limits)", 
			xx->res[2]);

		return TRUE;
	}
	fprintf(erg_f,"\n\t no adaption         : %8.4f Seg : %3d", xx->res[1], xx->CRcalc_sv.n);
	return FALSE;
}

/*  calculate statistical value for report purpose */
int	CSignalFormat::CalculateStatisticalValueForReport(
	const	INPUTDATAPDI *pdi,		/* PDI 信息 */
	FATsegs *xx,		/* Struktur     */
	const	TiTemp  Seg,		/* Segment data */
	const int		icall)		/* No. of call  */
{
	int		nResult = 0;
	int		icallM1 = icall - 1;
	const	int		nAccMode = pdi->acc_mode;

	if(icallM1 < 0)
		icallM1 = 0;
	if (Seg.Tm_p1>700)
		nResult += StatisticalValue(&xx->Tm_p1_sv,icall,Seg.Tm_p1,3);  //add by xie 11-6

	nResult += StatisticalValue(&xx->Tm_p2U_sv,icall,Seg.Tm_p2U,3);
	nResult += StatisticalValue(&xx->Tm_p3U_sv,icall,Seg.Tm_p3U,3);
	nResult += StatisticalValue(&xx->Tc_sct_sv,icall,Seg.Tc_sct,3);
	nResult += StatisticalValue(&xx->Tc_fct_sv,icall,Seg.Tc_fct,3);
	nResult += StatisticalValue(&xx->Tm_p4_sv,icall,Seg.Tm_p4,3);
	nResult += StatisticalValue(&xx->Tb_p4_sv,icall,Seg.Tb_p4,3);
	/* switch to surface value for IC */
	if(nAccMode == IC)
		nResult += StatisticalValue(&xx->Tr_fct_sv,icall,Seg.Tm_p4,3);
	else			
		nResult += StatisticalValue(&xx->Tr_fct_sv,icall,Seg.Tr_fct,3);
	nResult += StatisticalValue(&xx->CRcalc_sv,icall,Seg.CRcalc,3);
	/* switch to surface value for IC */
	if(nAccMode == IC) 
		nResult += StatisticalValue(&xx->CRreal_sv,icall,Seg.CRsurf,3);
	else			
		nResult += StatisticalValue(&xx->CRreal_sv,icall,Seg.CRreal,3);
	nResult += StatisticalValue(&xx->ti_cool_sv,icall,Seg.ti_cool,3);
	nResult += StatisticalValue(&xx->air1_sv,icall,Seg.air1,3);
	nResult += StatisticalValue(&xx->air2_sv,icall,Seg.air2,3);

	nResult += StatisticalValue(&xx->CRcore_sv,icall,Seg.CRcore,3);
	nResult += StatisticalValue(&xx->CRsurf_sv,icall,Seg.CRsurf,3);
	nResult += StatisticalValue(&xx->Tss_sct_sv,icall,Seg.Tss_sct,3);
	nResult += StatisticalValue(&xx->Tsc_sct_sv,icall,Seg.Tsc_sct,3);
	nResult += StatisticalValue(&xx->Tss_fct_sv,icall,Seg.Tss_fct,3);
	nResult += StatisticalValue(&xx->Tsc_fct_sv,icall,Seg.Tsc_fct,3);

	xx->fti_cool[icallM1] = Seg.ti_cool;
	xx->fTc_sct[icallM1]  = Seg.Tc_sct;
	xx->fTc_fct[icallM1]  = Seg.Tc_fct;

	/* switch to surface value for IC */
	if(nAccMode == IC)	{
		xx->fCR_fct[icallM1]  = Seg.CRsurf;
		xx->fTr_fct[icallM1]  = Seg.Tm_p4;
	}
	else {
		xx->fCR_fct[icallM1]  = Seg.CRreal;
		xx->fTr_fct[icallM1]  = Seg.Tr_fct;
	}
	
	float fTempFilter  = Seg.Tr_fct;
	if(Seg.Tr_fct < Seg.Tm_p4)	{
		fTempFilter = Seg.Tm_p4;
	}
	xx->fTx_fct[icallM1] = fTempFilter;
	if(nAccMode == IC)		
		nResult += StatisticalValue(&xx->Tx_fct_sv,icall, Seg.Tm_p4,3);
	else			
		nResult += StatisticalValue(&xx->Tx_fct_sv,icall, fTempFilter,3);

	return nResult;
}

int	CSignalFormat::StatisticalValue(s_val_t	*s,	/*	statistical value	*/
	const	int		iv,	/* = 1: first call		*/
	const	float		val,	/*	value		*/
	const	int		ime)	/* method of calcul.		*/
{
	if(ime < 1 )
		return FALSE;
	if(iv == 1)	{	/* set initial value for statistal value	*/
		s->n    =  0;
		s->max  =  FLT_MIN;
		s->min  =  FLT_MAX;
		s->mean =  0.0f;
		s->s1	=  0.0f;
		s->std  =  0.0f;
		s->s2	=  0.0f;
	}

	if(val > s->max)	
		s->max = val;
	if(val < s->min)	
		s->min = val;
	s->n++;
	if(ime < 2)		
		return TRUE;
	/*	subsequent calls	*/
	s->s1 += val;	/*	new 平均value	*/

	if(ime < 3)		
		return TRUE;
	s->s2 += val * val;
	return TRUE;
}


/* filter 低 value if within peaks             */
/* modify original signal                       */
/* fValMin      - 最小peak value (below     */
/*                this value no local 最大   */
/*                will be determined)           */
int	CSignalFormat::ModifySignalFilterLow2Peak(
	const	float	fValTarget,		/* 目标value */
	const	float	fValMin,		/* 最小允许value */
	const	int		n,				/* 序号value */
	const	float	*x,				/* 横坐标 value */
	float	*y)				/* 纵坐标value I/O */
{
	const int nIntervalMax = (n-1)/2;
	int	nFirstRise  = GetSignalIndex(n, 0, fValMin, y);
	int nFirstDrop  = SignalDrop(n, nFirstRise, fValMin, y);

	int	nMod = 0;	/* No. of modified value */
	if(nFirstDrop == -1)	
		return nMod;

	int nCtrInterval = -1;
	int nSecondRise;
	int nSecondDrop;
	while(++nCtrInterval < nIntervalMax)	
	{
		nSecondRise = GetSignalIndex(n, nFirstDrop, fValMin, y);
		if(nSecondRise == -1)	return(nMod);
		nMod += GetSignalFilterLowInteger(fValTarget, fValMin, nFirstDrop, nSecondRise, n, x, y, y);
		nSecondDrop = SignalDrop(n, nSecondRise,fValMin, y);
		if(nSecondDrop == -1)	return(nMod);
		nFirstDrop = nSecondDrop;
	}

	return nMod;
}

/* filter 低 value if within peaks             */
/* keep original signal                         */
/* fValMin      - 最小peak value (below     */
/*                this value no local 最大   */
/*                will be determined)           */
/*                                              */
/* in call function can use yx = y              */
int	CSignalFormat::GetSignalFilterLow2Peak(
	const	float	fValTarget,		/* 目标value */
	const	float	fValMin,		/* 最小允许value */
	const	int		n,				/* 序号value */
	const	float	*x,				/* 横坐标 value */
	const	float	*y,				/* 纵坐标value */
	float	*yx)			/*滤波信号*/
{
	const int nIntervalMax = (n-1)/2;
	int	nFirstRise  = GetSignalIndex(n, 0,          fValMin, y);
	int nFirstDrop  = SignalDrop(n, nFirstRise, fValMin, y);
	int	nStep = -1;
	while(++nStep < n) 
		yx[nStep] = y[nStep];

	int	nSecondRise;
	int nSecondDrop;

	int	nMod = 0;	/* No. of modified value */
	if(nFirstDrop == -1)	
		return nMod;

	int nCtrInterval = -1;
	while(++nCtrInterval < nIntervalMax)	{
		nSecondRise = GetSignalIndex(n, nFirstDrop, fValMin, y);
		if(nSecondRise == -1)	
			return nMod;
		/* modify value within 低 value 间隔 */
		nMod += GetSignalFilterLowInteger(fValTarget, fValMin, nFirstDrop, nSecondRise, n, x, y, yx);
		nSecondDrop = SignalDrop(n, nSecondRise,fValMin, y);
		if(nSecondDrop == -1)	return(nMod);
		/* set 间隔 value for next 间隔 */
		nFirstDrop = nSecondDrop;
	}
	return nMod;
}

/*滤波信号                               */
/* modify original signal                       */
/* - keep 平均value of signal                  */
/* - reduce 标准差 (Std) with       */
/*   StdNew = fRed * StdOld                     */
/* - fRed specifies allowed reduction (0<fRed<1)*/
/*                                              */
/* in call function can use yx = y              */
int	CSignalFormat::ModifySignalFilterMeanStandard(
	const	float	fValTarget,		/* 目标value */
	const	float	fValDev,		/* allowed 偏差 value */
	const	float	fRed,			/* allowed reduction */
	const	int		n,				/* 序号value */
	const	float	*x,				/* 横坐标 value */
	const	float	*y,				/* 纵坐标value */
	float	*yx)			/* filterred signal */
{
	float	fMean;
	float	fStdInp, fMinInp, fMaxInp;
	const	int		nStatist = CalculateStatisticalValue(n, y, &fMean, &fStdInp, &fMinInp, &fMaxInp); 
	const	float	fEpsStd = .1e-6f;
	const	float	fLimitLow  = fValTarget - fValDev;
	const	float	fLimitHigh = fValTarget + fValDev;
	/* ensure function call with yx = y */
	int	nStep = -1;	
	while(++nStep < n) 
		yx[nStep] = y[nStep];

	if(	(fRed < 0.0f) || (fRed > 1.0f) || (n < 2) || (fStdInp < fEpsStd) ||	(fValDev < 0.0f))	
		return 0;
	/* signal already fulfills requirement */
	if((fMinInp > fLimitLow) && (fMaxInp < fLimitHigh))	
		return 0;

	float	alpha1, alpha2, alpha, alphax;
	alpha  = fRed;	/* set to 默认value */
	if(	(fabs(fMaxInp - fMean) > fEpsStd) && (fabs(fMean - fMinInp) > fEpsStd))	{
		alpha1 = (float) fabs((fLimitHigh - fMean) / (fMaxInp - fMean));
		alpha2 = (float) fabs((fMean - fLimitLow)  / (fMean - fMinInp));
		alphax = alpha1;
		if(alpha2 < alpha1)
			alphax = alpha2;
		if((alphax > fRed) && (alphax < 1.0f))
			alpha = alphax;
	}
	return ModifySignalFilterSmoothMean(fMean, fStdInp, alpha, n, x, y, yx);
}

/* 平滑ing signal                             */
/* - keep original signal                       */
/* - keep 平均value                            */
/* in call function can use yx = y              */
int	CSignalFormat::ModifySignalFilterSmoothMean(
	const	float	fValMean,       /* 平均value signal y */
	const	float	fValStd,        /* 标准差 signal y */
	const	float	fRed,			/* 减少value Std */
	const	int		nPos,				/* 序号value */
	const	float	*x,				/* 横坐标 value */
	const	float	*y,				/* 纵坐标value */
	float	*yx)			/*滤波信号*/
{
	const	float	fEpsStd		= .1e-06f;
	const	float	fValConst	= (1.f - fRed) * fValMean;
	int	nStep = -1;
	while(++nStep < nPos) yx[nStep] = y[nStep];
	if(	(fRed < 0.0f) || (fRed > 1.0f) || (nPos < 2) || (fValStd < fEpsStd))	
		return(0);
	nStep = -1;
	while(++nStep < nPos)	yx[nStep] = fValConst + fRed * y[nStep];
	return(nStep);
}

/* filter 低 value if within peaks             */
/*                                              */
/* fValMin      - 最小accepted value        */
/*                (guarantees)                  */
/* fValPeakMin  - 最小peak value (below     */
/*                this value no local 最大   */
/*                will be determined)           */
/*                                              */
/* in call function can use yx = y              */
/* if safety test was passed (ensure in call)   */

int	CSignalFormat::GetSignalFilterLowInteger(
	const	float	fValTarget,		/* 目标value */
	const	float	fValMin,		/* 最小允许value */
	const	int		iDrop,			/* 指数下降信号*/
	const	int		iRise,			/* 指数上升信号 */
	const	int		nPos,				/* 序号value */
	const	float	*x,				/* 横坐标 value */
	const	float	*y,				/* 纵坐标value */
	float	*yx)			/* 滤波后的信号 */
{
	const	float	fEpsLength = .1e-6f;
	int nStep = iDrop - 1;
	int ia = nStep;
	int ib = iRise;
	float	rx;
	const	int	nM1 = nPos - 1;
	if(	((iRise - iDrop) < 1) || (iDrop < 0) || (iRise > nM1) || (nPos < 3)) 
		return 0;

	if(nStep < 0)	
		ia = 0;

	float fa = y[ia];
	float fb = y[ib];
	float dx = x[ib] - x[ia];
	if(dx < fEpsLength) return(0);

	const	float fDelta = fValTarget - fValMin;
	if(fa > (fValTarget + fDelta)) fa = fValTarget + 0.667f * fDelta;
	if(fb > (fValTarget + fDelta)) fb = fValTarget + 0.500f * fDelta;

	int	nMod = 0;
	while(++nStep < iRise)	{
		rx = (x[nStep] - x[ia]) / dx;
		if(y[nStep] < fValMin)	{
			yx[nStep] = (1.f - rx) * fa + rx * fb;
			nMod++;
		}
	}
	return nMod;
}

/* returns signal index if signal drops below   */
/* limit and (fSignal[index-1] < fLimit)        */
/* returns signal index if signal drops below limit */
/* from higher value */

int	CSignalFormat::SignalDrop(const	int	nPos, const	int		nSearch,	const	float	fLimit,	const	float	*fSignal)
{
	int	nIndex = nSearch - 1;
	float	fSignalAct;

	int iHighM1 = 0;
	if((nSearch < 0) || (nSearch > nPos) || (nPos < 1))	
		return -1;
	if((nIndex == -1) && (fSignal[0] >= fLimit)) 
		iHighM1 = 1;
	while(++nIndex < nPos)	{
		fSignalAct = fSignal[nIndex]; 
		if((fSignalAct <  fLimit) && (iHighM1 == 1))	
			return nIndex;
		if(fSignalAct >= fLimit) 
			iHighM1 = 1;
	}

	return -1;
}

/* print results of statistical value second row */
int	CSignalFormat::PrintStatisticalResultSecondRow(FATsegs *xx, FILE		*erg_f)
{
	char *format =" %5.1f %6.1f %6.1f %5.1f %5.1f";
	char *sTitle[] = {"Seg", "TStart", "TStop", "tACC", "CR"};
	fprintf(erg_f,"\n\n      Tss_s Tsc_s  Tss_f  Tsc_f  CRsurf CRcore");
	fprintf(erg_f,"\n MEAN %5.1f", xx->Tss_sct_sv.mean);
	fprintf(erg_f,format,xx->Tsc_sct_sv.mean, xx->Tss_fct_sv.mean, xx->Tsc_fct_sv.mean,	xx->CRsurf_sv.mean, xx->CRcore_sv.mean);
	fprintf(erg_f,"\n STD  %5.2f", xx->Tss_sct_sv.std); 
	fprintf(erg_f,format,xx->Tsc_sct_sv.std, xx->Tss_fct_sv.std, xx->Tsc_fct_sv.std,xx->CRsurf_sv.std, xx->CRcore_sv.std);
	fprintf(erg_f,"\n MAX  %5.1f", xx->Tss_sct_sv.max);
	fprintf(erg_f,format,xx->Tsc_sct_sv.max, xx->Tss_fct_sv.max, xx->Tsc_fct_sv.max,xx->CRsurf_sv.max, xx->CRcore_sv.max);
	fprintf(erg_f,"\n MIN  %5.1f", xx->Tss_sct_sv.min);
	fprintf(erg_f,format,xx->Tsc_sct_sv.min, xx->Tss_fct_sv.min, xx->Tsc_fct_sv.min,xx->CRsurf_sv.min, xx->CRcore_sv.min);

// 这部分信息不准 暂不输出 [9/27/2013 谢谦]	
	//fprintf(erg_f,"\n\n %3s %12s %12s %12s %12s", sTitle[0], sTitle[1],sTitle[2],sTitle[3],sTitle[4]);
	//int		nStep = -1;
	//while(++nStep < xx->Tss_sct_sv.n)	
	//{
	//	fprintf(erg_f,"\n %3d %12.4f %12.4f %12.4f %12.4f", nStep+1, xx->fTc_sct[nStep], xx->fTx_fct[nStep] , xx->fti_cool[nStep], xx->fCR_fct[nStep]);
	//}

	return TRUE;
}

/* calculate basic statistical value           */
int	CSignalFormat::CalculateStatisticalValue(const	int		nPos, const	float	*y,	float	*fMean, float	*fStd,	float	*fMin, float	*fMax)	
{
	if(nPos < 2)	
		return 0;

	double	dVal = (double) y[0];
	double	dSum1 = dVal;
	double	dSum2 = dVal * dVal;
	double	dMin  = dVal;
	double	dMax  = dVal;
	*fMean	 = y[0];
	*fStd	 = 0.0f;
	*fMin	 = y[0];
	*fMax    = y[0];

	int nStep		= 0;
	
	while(++nStep < nPos)	{
		dVal = (double) y[nStep];
		if(dVal < dMin) dMin = dVal;
		if(dVal > dMax) dMax = dVal;
		dSum1  += dVal;
		dSum2  += dVal * dVal;
	}
	*fMean = (float) (dSum1 / nStep);
	*fStd  = (float) (sqrt((dSum2 - dSum1 * dSum1 / nStep) / (nStep - 1.0)));
	*fMin  = (float) dMin;
	*fMax  = (float) dMax;
	
	return 4;
}

/* x.3) check 精度 of 头部 and 尾部 遮蔽   */
// 此函数需要进一步修改 [8/10/2013 谢谦]
int CSignalFormat::CheckHeadTailMask( 
	const	INPUTDATAPDI *pdi,
	const	RALCOEFFDATA *lay,
	RALMESDATA *mes,
	const	RALOUTDATA *pre_out,
	const	dbk_t							data_block[],
	FATsegs *xx,		/* Statistical value  */
	FILE						   *st_err,
	FILE						   *erg_f)
{
	int	nStatusMask = 0;
	float xMean1, xVar1, xMin1, xMax1;
	float xMean5, xVar5, xMin5, xMax5;
	float xMean6, xVar6, xMin6, xMax6;
	float xMeanHead5, xVarHead5, xMinHead5, xMaxHead5;
	float xMeanHead6, xVarHead6, xMinHead6, xMaxHead6;
	float fPos[MAX_MEASURE_VALUE*6];
	float fAppPyro[MAX_MEASURE_VALUE*6]; /* approximation pyrometer data */
	float fDevPyro[MAX_MEASURE_VALUE*6]; /* 偏差 pyrometer data from linear curve fit */
	float fAdaptHead = 0.0f;
	float fTempDiffP6 = 0.0f;
	const	char	*sHead[] = {"Pos.", "P1", "P5", "P6"};
	const	float	fLengthHeadMaskTop = pre_out->PreResult.LengthMaskHeadTop;
	const	float	fLengthHeadMaskBtm = pre_out->PreResult.LengthMaskHeadBtm;
	const	float	fLengthHeadMask    = MaxF(fLengthHeadMaskTop, fLengthHeadMaskBtm);
	const	float	fLengthTailMaskTop = pre_out->PreResult.LengthMaskTailTop;
	const	float	fLengthTailMaskBtm = pre_out->PreResult.LengthMaskTailBtm;
	const	float	fLengthTailMask    = MaxF(fLengthTailMaskTop, fLengthTailMaskBtm);

	int nStart, nEnde;
	int nMaxData;
	int nMaxData1, nMaxData2, nMaxData3;
	int nOffset = MAX_MEASURE_VALUE;
	int nStep = -1; 
	while(++nStep < MAX_MEASURE_VALUE*6) {
		fPos[nStep]     = 0.0f;
		fAppPyro[nStep] = 0.0f;
		fDevPyro[nStep] = 0.0f;
	}

	int nVal = mes->CurIndex;
	AnalyseArray(nVal, mes->TempP1, TGFIMIN, &xMean1, &xVar1, &xMin1, &xMax1, &nStart, &nEnde);
	nMaxData = nMaxData1 = nEnde - nStart + 1;
	LeastSquareFitData(nMaxData, &mes->Position[nStart], &mes->TempP1[nStart], 1, fAppPyro);

	float fPosZero;
	if(nStart == 0) 
		fPosZero = mes->Position[1] - mes->Position[0];
	else
		fPosZero = mes->Position[nStart] - mes->Position[nStart-1];

	nStep = -1; 
	while(++nStep < nMaxData) {
		fPos[nStep]     = mes->Position[nStart+nStep] - mes->Position[nStart] + fPosZero;
		fDevPyro[nStep] = fAppPyro[nStep] - mes->TempP1[nStart+nStep];
	}

	AnalyseArray(nVal, mes->TempP5, lay->fTempLowP5, &xMean5, &xVar5, &xMin5, &xMax5, 
		&nStart, &nEnde);
	if(nStart == 0) 
		fPosZero = mes->Position[1] - mes->Position[0];
	else			
		fPosZero = mes->Position[nStart] - mes->Position[nStart-1];
	nMaxData2 = nEnde - nStart + 1;
	LeastSquareFitData(nMaxData2, &mes->Position[nStart], &mes->TempP5[nStart],
		1, &fAppPyro[nOffset]);
	nStep = -1; 
	while(++nStep < nMaxData2) {
		fPos[nStep+nOffset]     = mes->Position[nStart+nStep] - mes->Position[nStart] + fPosZero;
		fDevPyro[nStep+nOffset] = fAppPyro[nStep+nOffset] - mes->TempP5[nStart+nStep];
	}
	if(nMaxData2 > nMaxData) nMaxData = nMaxData2;

	nOffset += MAX_MEASURE_VALUE;
	AnalyseArrayReverse(nVal, mes->TempP6, lay->fTempLowP6, 
		&xMean6, &xVar6, &xMin6, &xMax6, 
		&nStart, &nEnde);
	if(nStart == 0) fPosZero = mes->Position[1] - mes->Position[0];
	else			fPosZero = mes->Position[nStart] - mes->Position[nStart-1];
	nMaxData3 = nEnde - nStart + 1;
	LeastSquareFitData(nMaxData3, &mes->Position[nStart], &mes->TempP6[nStart],
		1, &fAppPyro[nOffset]);
	nStep = -1; 
	while(++nStep < nMaxData3) {
		fPos[nStep+nOffset]     = mes->Position[nStart+nStep] - mes->Position[nStart] + fPosZero;
		fDevPyro[nStep+nOffset] = fAppPyro[nStep+nOffset] - mes->TempP6[nStart+nStep];
	}
	if(nMaxData3 > nMaxData) nMaxData = nMaxData3;
	/* determine 自适应value 头部 / 尾部 遮蔽 */
	int nHead5 = GetInterpolatedIntervalId(fLengthHeadMask, nMaxData1, &fPos[MAX_MEASURE_VALUE]);
	AnalyseArray(nHead5, &fDevPyro[MAX_MEASURE_VALUE], -MAXTEMP, &xMeanHead5, 
		&xVarHead5, &xMinHead5, &xMaxHead5, 
		&nStart, &nEnde);
	int nHead6 = GetInterpolatedIntervalId(fLengthHeadMask, nMaxData2, &fPos[MAX_MEASURE_VALUE*2]);
	AnalyseArray(nHead6, &fDevPyro[MAX_MEASURE_VALUE*2], -MAXTEMP, &xMeanHead6, 
		&xVarHead6, &xMinHead6, &xMaxHead6, 
		&nStart, &nEnde);

	//fprintf(erg_f,"\n\n\t 分析表面温度和头尾遮蔽");   // 用处不大 暂不输出 [8/15/2013 谢谦]

	//fprintf(erg_f,"\n\t Nr. | %6s %8s | %6s %8s | %6s %8s|", sHead[0], sHead[1], sHead[0], sHead[2], sHead[0], sHead[3]);
	//nStep = -1;
	//while(++nStep < nMaxData){
	//	fprintf(erg_f,"\n\t %3d | %6.2f %8.2f | %6.2f %8.2f | %6.2f %8.2f|", nStep,
	//		fPos[nStep],			fDevPyro[nStep],
	//		fPos[nStep+MAX_MEASURE_VALUE],		fDevPyro[nStep+MAX_MEASURE_VALUE],
	//		fPos[nStep+nOffset],	fDevPyro[nStep+nOffset]);
	//}
	//fprintf(erg_f,"\n");
	//fprintf(erg_f,"\n\t P5 L鋘ge Fuss: %6.2f [m]  Abweichung: %6.2f [K] Min:  %6.2f [K] Max:  %6.2f [K]", fLengthHeadMask, xMeanHead5, xMinHead5, xMaxHead5);
	//fprintf(erg_f,"\n\t P6 L鋘ge Fuss: %6.2f [m]  Abweichung: %6.2f [K] Min:  %6.2f [K] Max:  %6.2f [K]", fLengthHeadMask, xMeanHead6, xMinHead6, xMaxHead6);

	xx->res[10] = xMean6;
	fTempDiffP6 = xx->Tr_fct_sv.mean - xx->Tm_p4_sv.mean;
	if(fTempDiffP6 < 0.0f) fTempDiffP6 = 0.0f;
	xx->res[11]	= xx->res[10] + fTempDiffP6;
	xx->res[12]	= xMean5 + fTempDiffP6;

	//fprintf(erg_f,"\n\t Oberfl鋍hentemperatur Oben gemittelt P6: %6.1f [度]", xx->res[10]);
	//fprintf(erg_f,"\n\t Kalorische Temperatur      gemittelt P6: %6.1f [度]", xx->res[11]);
	//fprintf(erg_f,"\n\t Oberfl鋍hentemperatur Unten gerechnet:   %6.1f [度]", xx->res[12]);

	mes->fTempMeanSurfTopP6 = xx->res[10];
	mes->fTempMeanSurfBtmP6 = xx->res[12];
	mes->fTempMeanCalP6		= xx->res[11];

	return nStatusMask;
}

/*  least square fit for signal data            */
/*                                              */
/*  returns polynomial Order                    */
/*  n < nPolyTar: nPolyTar = n                  */
/*                                              */
/*  yMod - 纵坐标data of aproximation        */
int	CSignalFormat::LeastSquareFitData(
	const	int		nPos,			/* 序号value */
	const	float	*x,			/* data 横坐标 */
	const	float	*y,			/* data 纵坐标*/
	const	int		nPolyTar,	/* target polynomial Order */
	float	*yMod)		/* modified data */
{
	int nStep       = -1;
	int is;
	float   *fCoef;
	const	float  xMin = x[0];
	const	float  xMax = x[nPos-1];
	const	float  fLen = xMax - xMin;

	int	nPoly    = 0;
	if((fCoef = (float *) calloc(nPolyTar + 1, sizeof(float))) == NULL) {
		while(++nStep < nPos) yMod[nStep] = y[nStep];
		return nPoly;
	}	
	nPoly = LeastSquareFitPolynomial(nPos, x, y, nPolyTar, fCoef);
	nStep = -1;	

	float	fSum;
	float  zeta;
	float  zetaN;
	while(++nStep < nPos)	{
		zeta		= (x[nStep] - xMin) / fLen;
		zetaN       = 1.0f;
		fSum		= fCoef[0];
		is = 0; 
		while(++is < (nPoly + 1)){
			zetaN    *= zeta;
			fSum	 += fCoef[is] * zetaN;
		}
		yMod[nStep]	= fSum;
	}

	free(fCoef);
	
	return 1;
}

/*  least square fit for signal data            */
/*                                              */
/*  returns polynomial Order                    */
/*  n < nPolyTar: nPolyTar = n                  */
int	CSignalFormat::LeastSquareFitPolynomial(
	const	int		nPos,			/* 序号value */
	const	float	*x,			/* data 横坐标 */
	const	float	*y,			/* data 纵坐标*/
	const	int		nPolyTar,	/* target polynomial Order */
	float	*fCoef)	/* polynomial Coefficents */
{
	const	double  xMin = (double)x[0];
	const	double  xMax = (double)x[nPos-1];
	const	double  fLen = xMax - xMin;
	if(nPos < 1 || fLen < .1e-12) 
		return 0;
	const	int nPolyMax = 2;	/* polynomial order of approximation */
	double  fVal;
	double	*rx;
	double  *yx;
	double	*ax;
	double	*axTax;
	double  *zeta;



	int nPolyTestMax = nPolyTar;
	if(nPos < nPolyMax) nPolyTestMax = nPos;
	if(nPolyTar > nPolyMax) nPolyTestMax = nPolyMax;
	int n1 = nPolyTestMax + 1;
	int n2 = nPos * n1;
	if((rx = (double *) calloc(n1, sizeof(double))) == NULL) 
		return 0;
	if((yx = (double *) calloc(nPos , sizeof(double))) == NULL) {
		free(rx);
		return 0;
	}
	if((ax = (double *) calloc(n2, sizeof(double))) == NULL) {
		free(yx);
		free(rx);
		return 0;
	}
	if((axTax = (double *) calloc(n1 * n1, sizeof(double))) == NULL) {
		free(ax);
		free(yx);
		free(rx);
		return 0;
	}
	if((zeta = (double *) calloc(nPos , sizeof(double))) == NULL) {
		free(ax);
		free(yx);
		free(rx);
		free(axTax);
		return 0;
	}
	fVal = 1.0 / nPos;
	int nStep = -1;	
	while(++nStep < nPos)	{
		ax[nStep] = fVal;
		zeta[nStep] = (x[nStep] - xMin) / fLen;
		yx[nStep] = (double) y[nStep];
	}
	int is = 0;
	int	nCtr = -1;
	while(++is < n1){
		nStep = -1;
		while(++nStep < nPos){
			nCtr++;
			ax[nCtr+nPos] = ax[nCtr] * zeta[nStep];
		}
	}
	GetOrthogonalMatrix(nPos, n1, ax, nPos, n1, ax, axTax);
	GetOrthogonalMatrix(nPos, n1, ax, nPos,  1, yx, rx);
	GetPositiveDefiniteMatrix(n1, 1, axTax, rx, yx);
	is = -1;
	while(++is < n1)	
		fCoef[is] = (float) (fVal * yx[is]);
	is--;

	while(++is < (nPolyTar + 1)) fCoef[is] = 0.0f;
	int nPolyRes = nPolyTestMax;
	free(rx);
	free(ax);
	free(yx);
	free(axTax);
	free(zeta);
	return nPolyRes;
}

/*  ax(transpose) X bx = axTbx                  */
/*                                              */
/*  Input                                       */
/*  ax (nRow_ax X nCol_ax)                      */
/*  bx (nRow_bx X nCol_bx)                      */
/*                                              */
/*  Output                                      */
/*  axTbx (nCol_ax X nCol_bx)                   */
//正交矩阵
int	CSignalFormat::GetOrthogonalMatrix(
	const	int		nRow_ax,	/* No. of rows ax */
	const	int		nCol_ax,	/* No. of columns ax */
	const	double	*ax,		/* matrix ax */
	const	int		nRow_bx,	/* No. of rows bx */
	const	int		nCol_bx,	/* No. of columns bx */
	const	double  *bx,		/* matrix bx */
	double	*axTbx)		/* result */
{

	if(	(nRow_ax < 1) || (nRow_ax != nRow_bx) || (nCol_ax < 1) || (nCol_bx < 1))
		return 0;

	int	nStep;
	int	nCtr	= 0;
	int nColumnIndex = -1; /* column index bx */
	int nOffset = -nRow_bx;
	int nInner;
	int iRowCtr;
	double	dSum;
	while(++nColumnIndex < nCol_bx){	/* columns bx */
		iRowCtr     = 0;
		nOffset    += nRow_bx;
		nStep = -1;
		while(++nStep < nCol_ax)
		{	/* rows ax(transpose) */
			dSum = 0.0;
			nInner = -1;
			while(++nInner < nRow_ax)	dSum += ax[iRowCtr++] * bx[nOffset + nInner];
			axTbx[nCtr++] = dSum;
		}	/* rows ax(transpose) */
	}	/* columns bx */
	return 1;
}

/*  solves linear algebraic system              */
/*  a * x = b                                   */
/*  returns b                                   */
/*  - 最大 dimension 3                       */
/*  - x can be used identical to b              */
//获得正定矩阵
int CSignalFormat::GetPositiveDefiniteMatrix(
	const	int		n,				/* dimension a (nXn) */
	const	int		nb,				/* No. columns b */
	const	double	*a,				/* matrix value */
	const	double	*b,				/* matrix value */
	double	*x)				/* solution */
{
	int		nRet = 0;
	int		nCtr = -1;
	const	double	dEps = .1e-12;
	switch(n)
	{
	case(1):
		nRet = GetPositiveDefiniteMatrixEx1(nb, dEps, a, b, x);	
		break;
	case(2):
		nRet = GetPositiveDefiniteMatrixEx2(nb, dEps, a, b, x);	
		break;
	case(3):
		nRet = GetPositiveDefiniteMatrixEx3(nb, dEps, a, b, x);	
		break;
	default:
		while(++nCtr < (n*nb)) x[nCtr] = b[nCtr];
		nRet = 0;
		break;
	}
	return nRet;
}

/*  solves linear algebraic system              */
/*  a * x = b                                   */
/*  returns b                                   */
/*  - matrix dimension 1                        */
/*  - x can be used identical to b              */
int	CSignalFormat::GetPositiveDefiniteMatrixEx1(
	const	int		nb,				/* No. columns b */
	const	double	fEps,			/* error value */
	const	double	*a,				/* matrix value */
	const	double	*b,				/* matrix value */
	double	*x)				/* solution */
{
	if(nb < 1)	
		return 0;

	double	ax = a[0];
	if(fabs(ax) < fEps)	
		return 0;

	int	nStep = -1;
	while(++nStep < nb)
		x[nStep] = b[nStep] / ax;

	return 1;
}

/*  solves linear algebraic system              */
/*  a * x = b                                   */
/*  returns b                                   */
/*  - matrix dimension 2                        */
/*  - x can be used identical to b              */
int	CSignalFormat::GetPositiveDefiniteMatrixEx2(
	const	int		nb,				/* No. columns b */
	const	double	fEps,			/* error value */
	const	double	*a,				/* matrix value */
	const	double	*b,				/* matrix value */
	double	*x)				/* solution */
{
	if(nb < 1)	
		return 0;

	double	dDet0;
	double	dDet1;
	double	b0;
	double	b1;
	const	double	a0 = a[0];
	const	double	a1 = a[1];
	const	double	a2 = a[2];
	const	double	a3 = a[3];
	const	double	ax = a0 * a3 - a1 * a2;
	if(fabs(ax) < fEps)	
		return 0;

	int		nStep = -1;
	int		nStart = -2;
	while(++nStep < nb){
		nStart	+= 2;
		b0 = b[nStart];
		b1 = b[nStart+1];
		dDet0 = b0 * a3 - b1 * a2;
		dDet1 = b1 * a0 - b0 * a1;
		x[nStart]	= dDet0 / ax;
		x[nStart+1] = dDet1 / ax;
	}
	return 1;
}

/*  solves linear algebraic system              */
/*  a * x = b                                   */
/*  returns b                                   */
/*  - matrix dimension 3                        */
/*  - x can be used identical to b              */
int	CSignalFormat::GetPositiveDefiniteMatrixEx3(
	const	int		nb,				/* No. columns b */
	const	double	fEps,			/* error value */
	const	double	*a,				/* matrix value */
	const	double	*b,				/* matrix value */
	double	*x)				/* solution */
{
	const	double	a0 = a[0];
	const	double	a1 = a[1];
	const	double	a2 = a[2];
	const	double	a3 = a[3];
	const	double	a4 = a[4];
	const	double	a5 = a[5];
	const	double	a6 = a[6];
	const	double	a7 = a[7];
	const	double	a8 = a[8];
	const	double	ax =	a0 * (a4 * a8 - a5 * a7) +
		a3 * (a2 * a7 - a1 * a8) +
		a6 * (a1 * a5 - a2 * a4);
	if(fabs(ax) < fEps)
		return 0;
	if(nb < 1)	
		return 0;

	int		nStep = -1;
	int		nStart = -3;
	double	dDet0;
	double	dDet1;
	double	dDet2;
	double	b0;
	double	b1;
	double  b2;
	while(++nStep < nb){
		nStart	+= 3;
		b0 = b[nStart];
		b1 = b[nStart+1];
		b2 = b[nStart+2];
		dDet0 =	b0 * (a4 * a8 - a5 * a7) +
			a3 * (b2 * a7 - b1 * a8) +
			a6 * (b1 * a5 - b2 * a4);
		dDet1 =	a0 * (b1 * a8 - b2 * a7) +
			b0 * (a2 * a7 - a1 * a8) +
			a6 * (a1 * b2 - b1 * a2);
		dDet2 =	a0 * (b2 * a4 - b1 * a5) +
			a3 * (b1 * a2 - b2 * a1) +
			b0 * (a1 * a5 - a2 * a4);
		x[nStart]	=  dDet0 / ax;
		x[nStart+1] =  dDet1 / ax;
		x[nStart+2] =  dDet2 / ax;
	}
	return 1;
}