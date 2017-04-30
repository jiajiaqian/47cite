/*! 
* Copyright (c) 2013, ������ѧ���Ƽ����������Զ��������ص�ʵ����(RAL)
* All rights reserved
* POSTPROCESS����
* FILE PostProcess.cpp
* ˵�� ����  
*  
* ��ϸ˵��
*  
* �汾 1.0 ���� Hanbing ���� 7/25/2013
* 
*/

#include "stdafx.h"
#include    <stdio.h>
#include    <stdlib.h>
#include    <time.h>
#include    <string.h>
#include	<math.h>
#include "PostProcess.h"


CPostProcess::CPostProcess()
{
}
CPostProcess::~CPostProcess()
{
}

/// @����˵��  main function for POSTPROCESS 
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
pwr_tInt32  CPostProcess::PostProcessing(
						RALINDATA *ralInPut,
						RALCOEFFDATA *lay,
						RALMESDATA *mes,
						RALOUTDATA *pre_out,
						pwr_tInt32               proc_id,
						struct T_AccSysPar		 *SysPar,
						struct T_AccPattern		 *AccPattern)
{
	int     i_status = 0;
	dbk_t   data_block[BLOCK_N];
	double  duration, duration1;
	INPUTDATAPDI pdi;
	clock_t calc_zeit, start, finish;
	FILE    *erg_f = NULL;
	FILE    *st_err = NULL;
	int     iStatusTemp;
	int	    iStatusTrack=0;
	int     iz = -1;
	const		char	sFileReport[] = "Logs\\posterror\\post_err.txt";  ////����·����LogFILE��  xiaoxiaoshine  20101227
	char	sFileName[PATH_MAX];
	struct  tm *newtime;
	time_t    aclock;
	FILE    *mes_file=NULL;

	time( &aclock );                 /* Get time in seconds */
	newtime = localtime( &aclock );  /* Convert time to struct */

	/* Init integer data block [250] with zero  */
	InitDataBlock(data_block);

	start = clock();

	FillPDIStruct(&pdi, ralInPut, lay, &pre_out->PreResult, 0);

	strcpy_s(ralInPut->file_nam,pdi.file_nam);

	/* Get filename of results FILE with ��� number to f_name */

	if(ralInPut->iSimMode == SIMULATION)
		MakeOnlineResultFileName(ralInPut->plateID,&pre_out->PreResult,ralInPut->file_nam);

	/* read telegram PREPROCESS results  lc */
	/* first open error FILE */ 
	if((st_err = fopen(sFileReport,"a")) == NULL){
		logger::LogDebug("system","impossible to open: post_err.txt");
		return(-1);
	}

	CombinFilePath(CONTROL1,PREPROCESS, sFileName);

	//���Ʋ�����ȡ
	if(InitPostControlValues(sFileName, &(pdi.controlAcc)) == FALSE){
		fprintf(st_err, "input ���Ʋ��� from default", sFileName);
		InitControlValues(&(pdi.controlAcc));
	}

	InitControlValues(&(pdi.controlAcc), &(pre_out->controlAcc));

	if(strcmp(ralInPut->file_nam,"")==0)
		strcpy_s(ralInPut->file_nam,"\\Logs\\postprocess\\0000.txt");

	if((erg_f = fopen(ralInPut->file_nam,"a")) == NULL) {
		logger::LogDebug("system","*** unable to open FILE : %s *** \n",ralInPut->file_nam);
		fprintf(st_err,"\n\t unable to open FILE : %s",ralInPut->file_nam);
		fclose(st_err);
		return(-1);
	}


	PreparationOutput(ralInPut, pre_out, st_err);

	logger::LogDebug("system","-PostCalc -%s-  ",ralInPut->file_nam);

	m_PostCaculateCore.OutputMeasuredResult(erg_f, 0, mes, lay, pre_out);

	/* make data transformations */
	pre_out->PreResult.iPyroBit = SetPyrometers(lay);


	iStatusTemp  = gSignalFormat.PostDataFormat(&pdi,
		lay,
		mes,
		pre_out,
		data_block,
		st_err,
		erg_f);

	//iStatusTrack = CheckAccuracy(&pdi,
	//	lay,
	//	mes, 
	//	pre_out, 
	//	data_block, 
	//	st_err, 
	//	erg_f);

	pre_out->PreResult.iNonUseAdapt = iStatusTemp + iStatusTrack;

	calc_zeit = clock() - start;
	finish = clock();
	duration = (double) (finish - start)/CLOCKS_PER_SEC;

	/*  run POSTPROCESS with  data structure CSC */
	start = clock();
	/* copy required because ipdi will be overriden */
	i_status = m_PostCaculateCore.PostCoreProcessing(erg_f,
		st_err,
		0,
		pre_out,
		ralInPut, 
		lay, 
		mes, 
		data_block);

	finish = clock();
	duration1 = (double) (finish - start)/CLOCKS_PER_SEC;

	fprintf(erg_f,"\n\n\t �����ת��������ʱ: %8.5f sec date: %s",duration, asctime(newtime));
	fprintf(erg_f,"\n\t  ����� ��ʱ: %8.5f", duration1);

	fclose(erg_f);
	fclose(st_err);
	LimitFileSize(MAX_NUMBER_ERROR_FILE, sFileReport, "prev_");
	return(pre_out->PreResult.iNonUseAdapt);
}

/// @����˵�� x.2) check ���� of material tracking
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int CPostProcess::CheckAccuracy(
				const	INPUTDATAPDI *pdi,
				const	RALCOEFFDATA *lay,
				RALMESDATA *mes,
				const	RALOUTDATA *pre_out,
				const	dbk_t							data_block[],
				FILE						   *st_err,
				FILE						   *erg_f)
{
	int	iStatusTrack = 0;
	int iz = -1;
	int iz1, iz2; /* index light barrier */
	float fTimeHeadBls; /* run time measured by BLS */
	float fTimeTailBls; /* run time measured by BLS */
	int	  iBlsEntry = 0; /* last light barrier before active Acc */
	int	  iBlsExit  = lay->nBls - 1;  /* first light barrier after active Acc */
	int iDirection; 
	int nSpdMax = MAX_MEASURE_VALUE - 1; /* ��� possible No. �ٶ�value */
	int nSpd;	 /* No. of �ٶ�value */
	int	iLimit1 = 0; /* 1 percent limit �ٶ� */
	int	iLimit2 = 0; /* 2 percent limit �ٶ� */
	int	iLimit5 = 0; /* 5 percent limit �ٶ� */
	int iLimitAdaptMax = 5;
	int iLimitStop = 0; /* ��� stpeed */
	int iSearchSpd;
	const	float	fLimit1 = 0.01f; /* �ٶ� error 1 % */
	const	float	fLimit2 = 0.02f; /* �ٶ� error 2 % */
	const	float	fLimit5 = 0.05f; /* �ٶ� error 5 % */

	const	float	fLimitAdaptMaxSpd  = D_SPD_ADAPT_MAX  * 0.01f;
	const	float	fLimitAdaptMeanSpd = D_SPD_ADAPT_MEAN;
	const	float	fLimitAdaptPos = D_POS_ADAPT;
	/* min. value position window (refer: definition �ٶ� profile) */
	const	float	fPosAdaptMin = pre_out->speedPosition[1] + fLimitAdaptPos;
	/* max. value position window (refer: definition �ٶ� profile) */
	/* const	float	fPosAdaptMax = pre_out->speedPosition[NSPDPROFILE-2] - fLimitAdaptPos; */
	const	int		iHeaderFirst = pre_out->PreResult.iHeaderFirst;
	float	fPosHeaderFirst = lay->fPosHeader[iHeaderFirst];
	const	int		iHeaderLast = pre_out->PreResult.iHeaderLast - 1;
	float	fPosHeaderLast = lay->fPosHeader[iHeaderLast]
	+ lay->fLengthHeader[iHeaderLast];
	const	float	fPosAdaptMax =  fPosHeaderLast - fLimitAdaptPos;
	const	float	fTimeErr = 0.5f; /* run time errror 0.5 s */
	const	float	fSpdMin = lay->MinSpeedCool;
	/* ��� runtime */
	float   fTimeRunMax;
	const	float	*pfSpdRef = pre_out->speed;
	float   fSpdAct; /* ʵ�� measured value �ٶ� */
	float   fSpdRef;
	float	fSpdEps;
	float	fTimeEps;
	const	char	*sHeadEng[] = {"Name BLS1",  "Act.1", "Name BLS2", "Act.2", "From","To",  "HeadRef", "HeadAct", "HeadRadar", "TailRef", "TailAct", "TailRadar"};
	const	char	*sHeadDeu[] = {"Name BLS1", "Verf.1","Name BLS2", "Verf.2", "Von","Nach", "FussRef", "FussAct", "FussRadar","KopfRef", "KopfAct", "KopfRadar"};
	float	fTimeHeadRef;
	float	fTimeHeadAct;
	float	fTimeTailRef;
	float	fTimeTailAct;
	float	fSpdDevMax = 0.0f; /* value ��� �ٶ� ƫ�� */
	float	fSpdDevPos = 0.0f; /* ���λ�� */
	float	fSpdDevAct;			/* ʵ�� �ٶ� ƫ�� */
	float	fSpdDevMean = 0.0f; /* ƽ��value �ٶ� ƫ�� */
	int		iSpdDevCtr  = 0;    /* counter value �ٶ� ƫ�� */
	float	fSpdDevWght;	/* weighting factor mean �ٶ� ƫ�� */
	const	int		nSpdVal = MAX_MEASURE_VALUE;
	const	float	*pfPosRef = pre_out->speedPosition;
	float   fPosAct;
	const	int		iStart  = GetInterpolatedIntervalId(lay->PosZon1Entry, 
		mes->CurIndex, mes->Position);
	const	int		iEnde   = GetInterpolatedIntervalId(lay->PosZon3Exit + pdi->length,  
		mes->CurIndex, mes->Position);
	int     iBlsValidIndex; 
	float   fPosProfileMax = 0.0f;
	float   fPosProfileMin = 9999.0f;

	for(iBlsValidIndex=0;iBlsValidIndex<lay->nBls;iBlsValidIndex++){
		if (mes->fTimeBlsHead[mes->CurPass][iBlsValidIndex] == 0 || mes->fTimeBlsTail[mes->CurPass][iBlsValidIndex] == 0)
			break;
	}

	if(iBlsValidIndex) iBlsValidIndex--;
	fTimeRunMax = (lay->fPosBls[iBlsValidIndex] - lay->fPosBls[0]) / fSpdMin;

	iz = iStart;
	while(++iz < iEnde)
	{
		fSpdAct = mes->Speed[iz];
		fPosAct = mes->Position[iz];
		iSearchSpd = GetLinearInterpolation(fPosAct, nSpdVal, 
			pfPosRef, pfSpdRef, &fSpdRef);
		if(fabs(fSpdRef) > fSpdMin)	{	/* plausible �ٶ� */
			fSpdEps = (float)fabs((fSpdAct - fSpdRef) / fSpdRef);
			if(  fSpdEps > fLimit1) iLimit1++;
			if(  fSpdEps > fLimit2) iLimit2++;
			if(  fSpdEps > fLimit5) iLimit5++;
			if(	 fSpdAct < fSpdMin) iLimitStop++;
			/* tracking condition no adaption */
			if(	(fPosAct > fPosAdaptMin) && (fPosAct < fPosAdaptMax)) 
			{
				fSpdDevAct = fSpdRef - fSpdAct;
				if(fabs(fSpdDevAct) > fabs(fSpdDevMax))	
				{
					fSpdDevMax = fSpdDevAct;
					fSpdDevPos = fPosAct;
				}
				iSpdDevCtr += 1;
				fSpdDevWght = 1.f / iSpdDevCtr;
				fSpdDevMean = fSpdDevMean * (1.f - fSpdDevWght) + fSpdDevAct * fSpdDevWght;
				if(fSpdEps > fLimitAdaptMaxSpd) iLimitAdaptMax++;
			} /* tracking condition no adaption */
		}							/* plausible �ٶ� */
	}
	if(iLimit1			> 0)	iStatusTrack = SetBit(16, iStatusTrack);
	if(iLimit2			> 0)	iStatusTrack = SetBit(17, iStatusTrack);
	if(iLimit5			> 0)	iStatusTrack = SetBit(18, iStatusTrack);
	if(iLimitStop		> 0)	iStatusTrack = SetBit(19, iStatusTrack);
	if(iLimitAdaptMax	> 0)	iStatusTrack = SetBit(22, iStatusTrack);
	if(fabs(fSpdDevMean) > fLimitAdaptMeanSpd) iStatusTrack = SetBit(23, iStatusTrack);
	if((iLimit1 + iLimit2 + iLimit5 + iLimitStop) > 0)
		iStatusTrack = SetBit(1, iStatusTrack);

	iz = -1;
	while(++iz < MAX_MEASURE_VALUE)	
		if(pfSpdRef[iz] < fSpdMin * 0.50) break;

	nSpd = iz;

	/* start check for valid �ٶ� profile length */
	for(iz=0;iz<nSpd;iz++){
		if(pfPosRef[iz]<fPosProfileMin) fPosProfileMin = pfPosRef[iz];
		if(pfPosRef[iz]>fPosProfileMax) fPosProfileMax = pfPosRef[iz];
	}
	if(fPosHeaderFirst < fPosProfileMin) fPosHeaderFirst = fPosProfileMin;
	if(fPosHeaderLast  > fPosProfileMax) fPosHeaderLast  = fPosProfileMax;
	/* end check for valid �ٶ� profile length */

	iz = -1;
	while(++iz < lay->nBls)
		if(lay->fPosBls[iz] > fPosHeaderFirst) break;

	iBlsEntry = iz; 
	iz = lay->nBls;
	while(--iz >= 0)
		if(lay->fPosBls[iz] < fPosHeaderLast) break;

	iBlsExit = iz + 1;

	if(iBlsExit>iBlsValidIndex) iBlsExit = iBlsValidIndex;

	fTimeHeadRef = GetIntervalTimeSpeed(	
		lay->fPosBls[iBlsEntry], 
		lay->fPosBls[iBlsExit], 
		nSpd, 
		pfPosRef, 
		pfSpdRef);

	fTimeHeadAct = GetIntervalTimeAvailable(
		lay->fPosBls[iBlsEntry],
		lay->fPosBls[iBlsExit], 
		mes->CurIndex,
		mes->TimeSinceP1,
		mes->Position);

	fTimeTailRef = GetIntervalTimeSpeed(	
		lay->fPosBls[iBlsEntry] + pdi->length, 
		lay->fPosBls[iBlsExit]  + pdi->length, 
		nSpd, 
		pfPosRef, 
		pfSpdRef);

	fTimeTailAct = GetIntervalTimeAvailable(
		lay->fPosBls[iBlsEntry] + pdi->length,
		lay->fPosBls[iBlsExit]  + pdi->length, 
		mes->CurIndex,
		mes->TimeSinceP1,
		mes->Position);

	// �˴��ݲ����  ��Ҫʱ�ٿ���[8/16/2013 лǫ]ReportTracking(	iBlsEntry, lay->sNameBls[iBlsEntry],	iBlsExit,  lay->sNameBls[iBlsExit],iLimit1, iLimit2,iLimit5, iLimitStop, iLimitAdaptMax, fTimeHeadRef, fTimeHeadAct, fTimeTailRef, fTimeTailAct,	fSpdDevMax, fSpdDevPos, fSpdDevMean, mes, erg_f);


	mes->Tracking[mes->CurPass].TimeHeadRef  = fTimeHeadRef;
	mes->Tracking[mes->CurPass].TimeHeadRt   = fTimeHeadAct;
	mes->Tracking[mes->CurPass].TimeHeadPlt  =	 mes->fTimeBlsHead[mes->CurPass][iBlsExit] -
		mes->fTimeBlsHead[mes->CurPass][iBlsEntry];
	mes->Tracking[mes->CurPass].TimeTailRef = fTimeTailRef;
	mes->Tracking[mes->CurPass].TimeTailRt  = fTimeTailAct;
	mes->Tracking[mes->CurPass].TimeTailPlt =	mes->fTimeBlsTail[mes->CurPass][iBlsExit] -
		mes->fTimeBlsTail[mes->CurPass][iBlsEntry];

	mes->Tracking[mes->CurPass].iBlsEntry = iBlsEntry; 
	mes->Tracking[mes->CurPass].iBlsExit  = iBlsExit;  

	fprintf(erg_f, "\n\n");

	fprintf(erg_f, "\n\t ��������դλ�õ�ʱ��: %d", 	pdi->controlAcc.iBlsSynchro);
	fprintf(erg_f, "\n\t %12s %6s %12s %6s %4s %4s %12s %12s %12s %12s %12s %12s",  
		sHeadEng[0], 
		sHeadEng[1], sHeadEng[2], sHeadEng[3], sHeadEng[4], sHeadEng[5],
		sHeadEng[6], sHeadEng[7], sHeadEng[8], sHeadEng[9],  sHeadEng[10], sHeadEng[11]); 

	iz = -1;
	while(++iz < lay->nBls){
		iDirection = 1;
		iz1 = pdi->controlAcc.iBlsSynchro;
		iz2 = iz;
		if(iz1 > iz2) {
			swapInt(&iz1, &iz2);
			iDirection = -1;
		}
		fTimeHeadRef = GetIntervalTimeSpeed(	
			lay->fPosBls[iz1], 
			lay->fPosBls[iz2], 
			nSpd, 
			pfPosRef, 
			pfSpdRef);
		fTimeHeadAct = GetIntervalTimeAvailable(
			lay->fPosBls[iz1],
			lay->fPosBls[iz2], 
			mes->CurIndex,
			mes->TimeSinceP1,
			mes->Position);
		fTimeTailRef = GetIntervalTimeSpeed(	
			lay->fPosBls[iz1] + pdi->length, 
			lay->fPosBls[iz2] + pdi->length, 
			nSpd, 
			pfPosRef, 
			pfSpdRef);
		fTimeTailAct = GetIntervalTimeAvailable(
			lay->fPosBls[iz1] + pdi->length,
			lay->fPosBls[iz2] + pdi->length, 
			mes->CurIndex,
			mes->TimeSinceP1,
			mes->Position);

		fTimeHeadBls = mes->fTimeBlsHead[mes->CurPass][iz2]-mes->fTimeBlsHead[mes->CurPass][iz1];

		if( (fTimeHeadBls >  fTimeRunMax) ||
			(fTimeHeadBls < -fTimeRunMax) ||
			(lay->iStatusBls[iz1] == 0) ||
			(lay->iStatusBls[iz2] == 0) ||
			(iz1 >iBlsValidIndex) ||    
			(iz2 >iBlsValidIndex))		
			fTimeHeadBls = 0.0f;
		fTimeTailBls = mes->fTimeBlsTail[mes->CurPass][iz2]-mes->fTimeBlsTail[mes->CurPass][iz1];
		if( (fTimeTailBls >  fTimeRunMax) ||
			(fTimeTailBls < -fTimeRunMax) ||
			(lay->iStatusBls[iz1] == 0) ||
			(lay->iStatusBls[iz2] == 0) ||
			(iz1 >iBlsValidIndex) ||   
			(iz2 >iBlsValidIndex))     
			fTimeTailBls = 0.0f;
		fprintf(erg_f, "\n\t %12s %6d %12s %6d %4d %4d %12.4f %12.4f %12.4f"
			" %12.4f %12.4f %12.4f", 
			lay->sNameBls[iz1], lay->iStatusBls[iz1], lay->sNameBls[iz2], 
			lay->iStatusBls[iz2] ,iz1, iz2, 
			fTimeHeadRef, fTimeHeadAct, fTimeHeadBls,
			fTimeTailRef, fTimeTailAct, fTimeTailBls);
	}
	fprintf(erg_f, "\n\n");

	fTimeEps = 0.0f;
	if(fTimeHeadRef > fLimit1){
		fTimeEps = (fTimeHeadRef - fTimeHeadAct);
		if(fabs(fTimeEps) > fTimeErr){
			iStatusTrack = SetBit(1, iStatusTrack);
			iStatusTrack = SetBit(20,iStatusTrack);
		}
	}
	fTimeEps = 0.0f;
	if(fTimeTailRef > fLimit1){
		fTimeEps = (fTimeTailRef - fTimeTailAct);
		if(fabs(fTimeEps) > fTimeErr){
			iStatusTrack = SetBit(1, iStatusTrack);
			iStatusTrack = SetBit(21,iStatusTrack);
		}
	}
	return(iStatusTrack);
}


/// @����˵��  x.2.1) report check ���� of tracking
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int	CPostProcess::ReportTracking(
	const	int		iBls1,
	const	char	*sNameBls1,
	const	int		iBls2,
	const	char	*sNameBls2,
	const	int		iLimit1,	/* �γ��� 1% ���� */
	const	int		iLimit2,	/* �γ��� 2% ���� */
	const	int		iLimit5,	/* �γ��� 5% ���� */
	const	int		iLimitStop,	/*���ֹͣλ��*/
	const	int		iLimitAdapt,	/* �γ��� Adapt ���� */
	const	float	fTimeHeadRef, /* ��Ҫ����time ͷ�� */
	const	float	fTimeHeadAct, /* ����time ͷ���ٶ� integrated */
	const	float	fTimeTailRef, /* ��Ҫ����time β�� */
	const	float	fTimeTailAct, /* ����time ͷ���ٶ� integrated */
	const	float	fSpdDevMax,   /* max. �ٶ�ƫ����Ӧ��Χ */
	const	float	fSpdDevPos,	  /* ���λ�� */
	const	float	fSpdDevMean,  /* mean �ٶ�ƫ����Ӧ��Χ */
	const	RALMESDATA *mes,
	FILE	*erg_f)		 /* ���FILE */
{
	fprintf(erg_f, "\n\n\t Status Material Tracking:  %s to %s",
		sNameBls1, sNameBls2);
	fprintf(erg_f, "\n\t 1 Percent Limit:    %8d  No. Samples failed", iLimit1);
	fprintf(erg_f, "\n\t 2 Percent Limit:    %8d  No. Samples failed", iLimit2);
	fprintf(erg_f, "\n\t 5 Percent Limit:    %8d  No. Samples failed", iLimit5);
	fprintf(erg_f, "\n\t ��� stop:         %8d\n", iLimitStop);
	fprintf(erg_f, "\n\t Adaption Limit:     %8d (> 0: No Adaption recommended!)", iLimitAdapt);
	fprintf(erg_f, "\n\t Max. ƫ�� Spd: %8.4f [m/s]", fSpdDevMax);
	fprintf(erg_f, "\n\t at Position:	     %8.4f [m]", fSpdDevPos);
	fprintf(erg_f, "\n\t Mean ƫ�� Spd: %8.4f [m/s]\n", fSpdDevMean);
	fprintf(erg_f, "\n\t Runtime ͷ�� Ref:   %8.2f [s]", fTimeHeadRef);
	fprintf(erg_f, "\n\t Runtime ͷ�� Act:   %8.2f [s] (Speed Integration)", fTimeHeadAct);
	fprintf(erg_f, "\n\t Runtime ͷ�� Act:   %8.2f [s] (Radar)", 
		mes->fTimeBlsHead[mes->CurPass][iBls2]-mes->fTimeBlsHead[mes->CurPass][iBls1]);
	fprintf(erg_f, "\n\t Runtime β�� Ref:   %8.2f [s]", fTimeTailRef);
	fprintf(erg_f, "\n\t Runtime β�� Act:   %8.2f [s] (Speed Integration)", fTimeTailAct);
	fprintf(erg_f, "\n\t Runtime β�� Act:   %8.2f [s] (Radar)", 
		mes->fTimeBlsTail[mes->CurPass][iBls2]-mes->fTimeBlsTail[mes->CurPass][iBls1]);
	fprintf(erg_f, "\n\t End Status Report Material Tracking\n\n");

	return TRUE;
}


/// @����˵�� ��ʼ�����ݿ�
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/27/2013
int CPostProcess::InitDataBlock(dbk_t data_block[])
{
	int iz = -1;
	int izx;

	while(++iz < BLOCK_N ){
		izx = -1;
		while(++izx < N) 
			data_block[iz].data_word[izx] = 0;
	}
	return(1);
}

int	CPostProcess::SetPyrometers(const  RALCOEFFDATA *lay)
{
	int	iPyroBit = 0;
	if(lay->iStatusP1 == 1) iPyroBit = SetBit(1, iPyroBit);
	if(lay->iStatusP2U == 1) iPyroBit = SetBit(2, iPyroBit);
	if(lay->iStatusP3U == 1) iPyroBit = SetBit(3, iPyroBit);
	if(lay->iStatusP4 == 1) iPyroBit = SetBit(4, iPyroBit);
	if(lay->iStatusP5 == 1) iPyroBit = SetBit(5, iPyroBit);
	if(lay->iFilterPyro == 1)   iPyroBit = SetBit(0, iPyroBit);
	return(iPyroBit);
}

/// @����˵��  output results of ONLINE simulation and ����
/// @���� 
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int	CPostProcess::MakeOnlineResultFileName(const char *plate_id,const PRERESULT *PreResult,char *fn)

{   
	char fnxx[255];
	if(strlen(plate_id))	sprintf(fnxx, "%s", plate_id); /* Remove Spaces */
	else			sprintf(fnxx, "%3.3s", PreResult->plateID);
	strcat(fnxx,".txt");
	strcpy(fn,DIR_PRE_RES);
	strcat(fn,fnxx);
	return TRUE;
}