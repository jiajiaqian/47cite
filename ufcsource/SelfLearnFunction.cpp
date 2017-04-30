/*! 
* Copyright (c) 2013, 东北大学轧制技术及连轧自动化国家重点实验室(RAL)
* All rights reserved
* 自学习控制类
* FILE SelfLearnFunction.cpp
* 说明 概述  
*  
* 详细说明
*  
* 版本 1.0 作者 Hanbing 日期 5/6/2013
* 
*/

#include "stdafx.h"
#include <stdlib.h>
#include "SelfLearnFunction.h"

#ifndef	MESSAGE_MAX_LENGHT
#define	MESSAGE_MAX_LENGHT	256
#endif

// This is the constructor of a class that has been exported.
// see SelfLearnFunction.h for the class definition
CSelfLearnFunction::CSelfLearnFunction()
{
}

int CSelfLearnFunction::SuchWrite(		
	FILE		*erg_f,		/* report FILE */	
	const	FATsegs		*xx,		/* Statistical value    */
	const	PRERESULT *PreResult,
	const	INPUTDATAPDI *pdiInPut, 
	PHYSICAL *ver_par1, 
	float qcr,
	ADAPTATION *adaptUsed)
{
	unsigned long int        key1_nrs[5], key2_nrs[5];
	unsigned int             key1_write, key2_write, key3_write, ixx = 0;

#ifdef	DEBUG_FILE_WRITE_BDA
	return TRUE;
#endif
	key1_write = 0;
	key2_write = 0;
	key3_write = 0;
	KeepAdaptionKey(key1_nrs,pdiInPut,ver_par1->waterTemp);
	int i;
	for (i=0;i<5;i++){
		ver_par1->adaptKey[i] = key1_nrs[i];
	}

	KeepAdaptionKey2(key2_nrs,pdiInPut,ver_par1->waterTemp);	

	if(ver_par1->isWriteAdaptValue > 0){
		key1_write = SuchWrite(erg_f, xx, PreResult, pdiInPut, key1_nrs, 1, qcr, adaptUsed);
		key2_write = SuchWrite(erg_f, xx, PreResult, pdiInPut, key2_nrs, 2, qcr, adaptUsed);
	}
	else{ /* write only not later reading */
		key1_write = SuchWrite(erg_f, xx, PreResult, pdiInPut, key1_nrs, 4, qcr, adaptUsed);
	}		  
	ixx = 1;
	return(ixx);
}

/* x.1) called in SuchWrite()                           */
/* calculate and write 自适应value                  */
int CSelfLearnFunction::SuchWrite(		
	FILE		*erg_f,		/* report FILE */	
	const	FATsegs		*xx,    /* Statistical value */
	const	PRERESULT *PreResult,	/* data block preset  */
	const	INPUTDATAPDI *pdi,
	const	unsigned long int	key_b[5], 
	const	int			use_dat, 
	float		qcr,
	ADAPTATION *adaptUsed)
{
	int             keyfound = 0;
	int             iz = -1;
	int             i;
	int             iret;
	int			  nMaxRead = MAX_ADAPT; /* 最大 entries in FILE */
	int			  iPatternFound;
	char            *ver1xx;
	FILE            *very = NULL;
	const  int			nStroke = PreResult->nStroke; /* No. of strokes (for 表名 adaption) */
	char			sAdaptReport[2*PATH_MAX];	/* adaption report */

	const int			nLine = 2*PATH_MAX - 1;  

	ADAPTATION adaptRead;  /* data of record set read from FILE */
	ADAPTATION adaptWrite; /* new / modified record set to FILE */
	static  ADAPTATION adaptWriteL1; /* written to level 1 */
	char          *FileBuf[MAX_ADAPT]; /* keep lines in Memory */
	const	  MODELCONTROL *ctrlCool = &pdi->controlAcc;


	/* generate Adaption FileName */
	ver1xx =  EnsureAdaptationFileName(pdi, use_dat,PREPROCESS, nStroke);  // gai wei preprocess [11/16/2013 谢谦]
	strcpy_s(adaptUsed->sFileAdapt, ver1xx);

	/* may be FILE is empty */
	if ((very = fopen(ver1xx, "r")) == NULL)
		nMaxRead = 0; 

	InitAdaptation(&adaptRead);
	InitAdaptation(&adaptWrite);
	if(use_dat == 1)	
		InitAdaptation(&adaptWriteL1);

	if(pdi->iSimMode == SIMULATION){
		ReadAdaptationInfo(pdi->plateID, adaptUsed);
	}
	strcpy_s(adaptRead.sFileAdapt, ver1xx);
	strcpy_s(adaptWrite.sFileAdapt, ver1xx);

	while(++iz < nMaxRead  && very != NULL) {  /* loop over all lines in FILE */
		if((fgets(sAdaptReport, nLine, very)) == NULL) 
			break;

		iret = ReadAdaptationFromString(sAdaptReport, &adaptRead);
		if((iret == EOF)||(iret == 0))	
			break;

		iPatternFound = strcmp(adaptRead.sPatternID, adaptUsed->sPatternID);
		if ((key_b[0] == adaptRead.adap_key[0]) && (key_b[1] == adaptRead.adap_key[1]) && (key_b[2] == adaptRead.adap_key[2]) && (key_b[3] == adaptRead.adap_key[3]) && (key_b[4] == adaptRead.adap_key[4]) && (iPatternFound == 0)) 
		{		/*      new 自适应valuewas calculated       */
			keyfound = 1;
			/* shift 信息 and initialize copy */
			InitAdaptation(&adaptRead, pdi->plateID, &adaptWrite);
			/* use 纠正 key, if 信息 is from other adaption level */
			//changed by wbxang 20120516
			for (i=0;i<5;i++){
				adaptWrite.adap_key[i]		= key_b[i];
			}
			/* calculate 自适应value */
			SetAdaptionValues(qcr, keyfound, iz, xx, PreResult, pdi, use_dat, 
				&adaptRead, adaptUsed, &adaptWriteL1, &adaptWrite);
			WriteAdaptationToString(&adaptWrite, sAdaptReport);

			FileBuf[iz] = _strdup(sAdaptReport);
			/* store adaptation history to FILE */
			if(use_dat == 1) 
				LogAdapt(PreResult, sAdaptReport);
		}			/*      new 自适应valuewas calculated       */
		else {	/*		keep old entry with unchanged value	*/
			FileBuf[iz] = _strdup(sAdaptReport);
		}	
	}	    /*      loop over all lines	adaptation FILE		*/
	if(keyfound == 0){		/*      was not found must be added				*/
		/* init is required otherwise last record from FILE will be in use */
		InitAdaptation(&adaptRead);
		strcpy_s(adaptRead.sFileAdapt, ver1xx);
		/* shift 信息 and initialize copy with 实际 板材 value */
		InitAdaptation(adaptUsed, pdi->plateID, &adaptWrite);
		/* use 纠正 key, if 信息 is from other adaption level */
		for (i=0;i<5;i++)
		{
			adaptWrite.adap_key[i]		= key_b[i];
		}				 /* calculate 自适应value */

		SetAdaptionValues(qcr, keyfound, iz, xx, PreResult, pdi, use_dat, 
			&adaptRead, adaptUsed, &adaptWriteL1, &adaptWrite);

		WriteAdaptationToString(&adaptWrite, sAdaptReport);

		FileBuf[iz++] = _strdup(sAdaptReport);
		/* store adaptation history to FILE */
		if(use_dat == 1) 
			LogAdapt(PreResult, sAdaptReport);
	}		/*      was not found must be added				*/
	if(use_dat == 1) 
		CopyAdaptation( &adaptWrite,  &adaptWriteL1); /* save Level 1 value */
	if(very != NULL) 
		fclose(very);

	/* Write FileBuffer to FILE */
	if ((very = fopen(ver1xx, "w")) != NULL){
		for(i=0;i<iz;i++){
			fputs(FileBuf[i],very);
			free(FileBuf[i]);
		}
		fclose(very);
	}

	WriteAdaptationInfo(erg_f, &adaptWrite);

	return(keyfound);
}

/*  write adaption history to log FILE                              */
int	CSelfLearnFunction::LogAdapt(
	const	PRERESULT *PreResult,
	const	char	*sAdapt)		/* string for storage */
{
	FILE	*erg_f = NULL;
	char			sFileRep[PATH_MAX];		/* adaption report */
	struct tm		*newtime;
	time_t			aclock;
	char			sStr1[PATH_MAX];
	char            sStr2[2*PATH_MAX];

	CombinFilePath("1ver.log",POSTPROCESS, sFileRep);
	if ((erg_f = fopen(sFileRep, "a")) == NULL){
		printf("*** 1Error opening FILE: %s  ***", sFileRep);
		return FALSE;	
	}
	time( &aclock );                 /* Get time in seconds */
	newtime = localtime( &aclock );  /* Convert time to struct tm form*/

	BitToString(PreResult->iNonUseAdapt, 32, sStr1);

	strcpy_s(sStr2,sAdapt); /* remove '\n' at the end */
	sStr2[strlen(sStr2)-1] = '\0';

	/* new line is member of asctime */
	fprintf(erg_f, "%s %s %4f %4f %12.2f %s", 
		sStr2, sStr1, PreResult->finishRollTemperature, PreResult->finishCoolTargetTemperature, PreResult->plateThick,  asctime(newtime));
	fclose(erg_f);
	return TRUE;
}

/* adaption criteria   1) absolute limitations          */
int	CSelfLearnFunction::checkLimitAdapt(const float qcr)
{
	if(qcr < MIN_ADAPT_VAL)	
		return(0);
	if(qcr > MAX_ADAPT_VAL)
		return(0);
	return(1);
}

/* adaption criteria   2) step for change               */
/* precondition : criteria 1) already passed            */
int	CSelfLearnFunction::checkStepAdapt(
	float *qcr_new,		/* calculated 自适应value*/
	const float qcr_file,		/* 自适应valuefrom FILE  */
	const float fAdaptStepSize, /* step size adaption */
	const float fAdaptMin,		/* 最小自适应value*/
	const float fAdaptMax)	    /* 最大 自适应value*/
{
	if((*qcr_new - qcr_file) >  fAdaptStepSize) 
		*qcr_new = qcr_file + fAdaptStepSize;
	if((*qcr_new - qcr_file) < -fAdaptStepSize) 
		*qcr_new = qcr_file - fAdaptStepSize;
	if(*qcr_new > fAdaptMax) 
		*qcr_new = fAdaptMax;
	if(*qcr_new < fAdaptMin) 
		*qcr_new = fAdaptMin;
	return(1);
}

/* set adaption window to value                        */
/* - influence of number of plates                      */
/* - Temperature difference                             */
/* - cooling mode (IC: 低 Temperature difference)      */
int		CSelfLearnFunction::setLimitAdapt(
	const	INPUTDATAPDI *pdi,			/*PDI数据 */
	const	int			nPlate,			/* No. of recorded plates */
	const	int			iAdaptLevel,	/* level of adaption */
	const	ADAPTATION *adaptWriteL1,	/* wriiten to level 1 */
	float		*fAdaptStep,	/* step size */
	float		*fAdaptMin,		/* min. value window */
	float		*fAdaptMax)		/* max. value window */
{
	int		iRet = 0;
	const	float	fAdaptTempL1 = adaptWriteL1->tr_foundMean;
	const	float	fAdaptEps    = 0.01f;
	const	float	fDeltaTemp = pdi->finishRollTemp - pdi->targetFinishCoolTemp;
	const	MODELCONTROL *ctrlCool = &pdi->controlAcc;
	*fAdaptStep	= ctrlCool->fAdaptStepSize;
	*fAdaptMin	= ctrlCool->fAdaptLimitMin;
	*fAdaptMax	= ctrlCool->fAdaptLimitMax;

	if(fDeltaTemp > D_TEMP_ADAPT)
		*fAdaptStep *= D_TEMP_ADAPT/ fDeltaTemp;  
	/* limit for interface cooling */
	if(pdi->acc_mode == IC){
		iRet       = 1;
		*fAdaptMin = MIN_ADAPT_LOWACC;
		*fAdaptMax = MAX_ADAPT_LOWACC;
	}
	/* limit for first 板材 */
	if(nPlate < 1){
		iRet       = 1;
		*fAdaptMin = MIN_ADAPT_FIRST;
		*fAdaptMax = MAX_ADAPT_FIRST;
	}
	/* set limits to level 1 自适应value */
	if((iAdaptLevel > 1) && (fAdaptTempL1 > fAdaptEps)){
		/* 下限 bound to level 1 value */
		if(	(fAdaptTempL1 < 1.0f) && (fAdaptTempL1 > *fAdaptMin)) 
			*fAdaptMin = fAdaptTempL1;
		/* 上限 bound to level 1 value */
		if(	(fAdaptTempL1 > 1.0f) && (fAdaptTempL1 < *fAdaptMax)) 
			*fAdaptMax = fAdaptTempL1;
	}
	return(iRet);
}

/* calculate 自适应value                            */
/* - weight results if already adapted to class         */
/* - works also if new class (n = 0)                    */
/* - adaptRead must be initilized with InitAdaptation()     */
int		CSelfLearnFunction::SetAdaptionValues(
	float	qcr,			/* 自适应valuecr */
	const	int		keyfound,		/* 0: no key 1: found key */
	const	int		iLine,			/* modify line */
	const	FATsegs	*xx,			/* Statistical value    */
	const	PRERESULT *PreResult,			/* data block preset */
	const	INPUTDATAPDI *pdi,			/*PDI数据 */
	const	int		iAdaptLevel,	/* level of adaption */
	const	ADAPTATION *adaptRead,		/* record set from FILE */
	const	ADAPTATION *adaptUsed,		/* 实际 板材 */
	const	ADAPTATION *adaptWriteL1,	/* written to level 1 */
	ADAPTATION *adaptWrite)	/* modify to write */
{	
	time_t		  iAdaptTimeAct;
	long int        nup_f, n;
	float			  alpha; /* weighting factor for adaption */
	float           qcra_f = adaptRead->adaptRatioFound;	 /* Cr 自适应value */
	float           qtra_f = adaptRead->tr_foundMean;	 /* Ts 自适应value */
	float			  qcra_pred; /* modified adaption without limits */
	float	          qtra_pred;
	int			  n_adapt_plate = N_ADAPT_PLATE; /* weight of single 板材 */
	float			  fAdaptStepSize = STEP_ADAPT_VAL;  /* possible step size */
	float			  fAdaptMin = MIN_ADAPT_VAL;
	float			  fAdaptMax = MAX_ADAPT_VAL;
	const	  float			  qtrMean = xx->res[2];
	const	  float			  fEpsAdapt = 0.1f * fAdaptMin;

	/* iNonUseTracking = 1: don't adapt, caused by tracking error */
	int             iNonUseAdapt = 0;

	time(&iAdaptTimeAct);
	if(qcra_f < fEpsAdapt) 
		qcra_f = 1.0f;
	if(qtra_f < fEpsAdapt) 
		qtra_f = 1.0f;

	nup_f					= adaptRead->rec_plates;
	if(nup_f < 0) 
		nup_f = 0;
	n = nup_f;
	/* set window limits for 纠正 板材 number */
	setLimitAdapt(pdi, n, iAdaptLevel, adaptWriteL1, &fAdaptStepSize, &fAdaptMin, &fAdaptMax);

	/* increase sensivity for high gap time */
	if(	(n > 0)	&& ((iAdaptTimeAct - adaptRead->iAdaptTimeLast) > N_ADAPT_GAP)){
		n = n/2; /* increse weight of 实际 板材 */
		if(n < 1) 
			n = 1;
		fAdaptStepSize *= 2.f; /* increse possible step size */
	}
	if (n >= N_ADAPT_PLATE) 
		n = N_ADAPT_PLATE;
	alpha = 1.f / (float)(n+1);

	/* limit step size */
	checkStepAdapt(&qcr, qcra_f, fAdaptStepSize, fAdaptMin, fAdaptMax);
	qcra_pred = ((qcra_f * n) + qcr) / (n+1);
	/* set defaut for iControl == 2 */
	qtra_pred =   qtra_f + (qtrMean - 1.f) * alpha*2;   // *2 by xie 11-6
	/* weighted 平均value for write only AND not read in PREPROCESS */
	if(adaptRead->iControl == 3)	
		qtra_pred = ((qtra_f * n) + qtrMean) / (n+1);
	/* read only in PREPROCESS ; no change of adaptation value */

	iNonUseAdapt = CheckBit(23, PreResult->iNonUseAdapt);
	if(	(adaptRead->iControl == 1) || (iNonUseAdapt == TRUE)) /* failed 平均value 速度 */
		qtra_pred = qtra_f;
	/* do not read or write */
	if(adaptRead->iControl == 0)	
		qtra_pred = qtra_f;
	/* check step size for change of modification  */
	checkStepAdapt(&qtra_pred, qtra_f, fAdaptStepSize, fAdaptMin, fAdaptMax);
	adaptWrite->iAdaptTimeLast  = (long)iAdaptTimeAct;
	adaptWrite->iAdaptTimeDiff  = (long)iAdaptTimeAct - adaptRead->iAdaptTimeLast;
	adaptWrite->adaptRatioFound		= s_val_relay(qcra_pred, qcra_f, fAdaptMin, fAdaptMax);
	adaptWrite->tr_foundMean = s_val_relay(qtra_pred, qtra_f, fAdaptMin, fAdaptMax);
	adaptWrite->tr_foundHead = adaptWrite->tr_foundMean;
	adaptWrite->tr_foundTail = adaptWrite->tr_foundMean;
	adaptWrite->iFoundWrite  = iLine; /* write data to line */
	/* if modified data store line Number */
	if(keyfound == 1)	
		adaptWrite->iFoundRead   = iLine;
	adaptWrite->rec_plates  += 1;
	/* set required value of last 板材: speed, cooling time ... */

	SetLastActualPlate(xx, PreResult, adaptWrite,adaptRead);
	return TRUE;
}

/* determine transfer value of 实际 板材 (_last)    */
int	CSelfLearnFunction::SetLastActualPlate(
	const	FATsegs	*xx,	/* Statistical value    */
	const	PRERESULT *PreResult,	/* preset DB */
	ADAPTATION *adapt,
	const ADAPTATION *adaptRead)	/* adapted value add by xie 10-17*/ 
{
	/* length of active 冷却区 (sum active headers) */
	const	float	lx_last = PreResult->activeCoolZoneLen;	  
	/* cooling time preset */
	const	float	t_acc_def  = 0.5f * (PreResult->tcHead + PreResult->tcTail);
	const	float	l_header   = 0.001f * PreResult->coolBankLen;
	/* limit value one stroke */
	const	float	t_acc_max1 = NUZZLE_NUMBER * l_header / SPD_MIN;
	const	float	t_acc_min1 = l_header / SPD_MAX; /* lowest cooling time */
	const	float	t_acc_eps  = 0.1f * t_acc_min1;
	int		nStroke = PreResult->nStroke;
	float	t_acc;		/* cooling time below active headers */
	float	vx_acc = PreResult->vPred;		/* 速度 preset */
	float	t_acc_min;  /* 最小cooling time */
	float	t_acc_max;  /* 最大 cooling time */

	if(nStroke < 1)			
		nStroke = 1;
	if(nStroke > ANZ_HUB)	
		nStroke = 1;
	t_acc_min = nStroke * t_acc_min1;
	t_acc_max = nStroke * t_acc_max1;
	t_acc   =  s_val_relay(	xx->ti_cool_sv.mean, 
		t_acc_def, 
		t_acc_min, 
		t_acc_max);
	if(t_acc > t_acc_eps) 
		vx_acc = lx_last / t_acc;
	adapt->lx_last = lx_last;

	adapt->fSpare[1]=adaptRead->fSpare[0];
	adapt->fSpare[0]=adaptRead ->tf_last;   //add by xie 10-17
	int rec_plates_temp=adaptRead->rec_plates;  //add by xie 11-27 记录前60块的平均Temperature

	if (adaptRead->rec_plates>60){
		rec_plates_temp=60;
	}

	adapt->tf_last = (rec_plates_temp*adaptRead->tf_last+xx->Tm_p2U_sv.mean)/(rec_plates_temp+1);   //add by xie 11-8

	adapt->ts_last = xx->Tr_fct_sv.mean;
	adapt->vx_last = vx_acc;
	adapt->tw_last = PreResult->tempWater;

	if (PreResult->plateWidth>2.8) // add 宽板进行自学习 [1/6/2014 谢谦]
	{
	  adapt->fSpare[2]+=(PreResult->aveSpCenter-PreResult->aveSpEdge)/(2*PreResult->aveSpCenter);  // 边部流量遮蔽自学习 [1/2/2014 xie]
	}
	
	if(adapt->fSpare[2]>0.1)   // 自学习保护 [1/2/2014 xie]
		adapt->fSpare[2]=0.1;

	if(adapt->fSpare[2]<-0.1)
		adapt->fSpare[2]=-0.1;

	// 水比自学习 [9/18/2014 qian]
	adapt->fSpare[1]=PreResult->flowFactor;
	if(adapt->fSpare[1]>2.5)   // 自学习保护 [1/2/2014 xie]
		adapt->fSpare[1]=2.5;

	if(adapt->fSpare[1]<0.6)
		adapt->fSpare[1]=0.6;

	return TRUE;
}

float CSelfLearnFunction::SuchRead(const INPUTDATAPDI *pdi,		/*PDI数据 */
	const	unsigned long int	key_b[5],		/* adaption key       */
	const	int			use_dat,	/* adaption level (1: most dist) */
	const	int			taskID,		/* PreCalc, PostCalc */
	const	int			nStroke,	/* No. of stokes */
	ADAPTATION *adapt)		/* O: found adaption 信息 */
{
	time_t		  iAdaptTimeUse;
	int             keyfound1;
	int			  nScan = 0;	/* No. identified identities in line */
	int			  iZeile = -1;   /* line with suitable adation value */
	int			  iPatternFound = 0;
	float           qcrnew = 0.0f;
	float			  qtr_fileMean;
	float			  qtr_fileHead;
	float			  qtr_fileTail;
	float			  vx_lastSave = adapt->vx_last; /* already set */
	char			  sPatternID_Save[MESSAGE_MAX_LENGHT];
	/* qtr = 0 indicates: no 自适应valuewas found */
	float			  qtrnew = 0.0f;
	int			  iControl = 2;
	char            *ver1xx;
	char			sLine[2*MESSAGE_MAX_LENGHT];
	int			nLine = 2*MESSAGE_MAX_LENGHT - 1;
	FILE		    *ver1 = NULL;
	ADAPTATION adaptRead;
	int i;

	if((ver1xx =  EnsureAdaptationFileName(pdi, use_dat, taskID, nStroke)) == NULL)
		nScan = -1;

	strcpy_s(sPatternID_Save, adapt->sPatternID); /* save value */
	/* set adaptation structure to default value */
	InitAdaptation(adapt);

	for(i=0;i<5;i++)
		adapt->adap_key[i]		= key_b[i];

	adapt->adaptRatioFound		= 1.0f;
	strcpy_s(adapt->sFileAdapt, ver1xx);
	/* reset to saved value */
	adapt->vx_last		= vx_lastSave;
	strcpy_s(adapt->sPatternID, sPatternID_Save);


	if(nScan == -1)	
		return(qtrnew);

	if ((ver1 = fopen(ver1xx, "r")) == NULL)
		printf("\n\t Adaption FILE: %s  lev.: %d don't exist ", ver1xx, use_dat);
	else 
	{
		keyfound1 = 0;
		qcrnew = 0.0f;
		while (!feof(ver1) && (keyfound1 != 1))
		{
			fgets(sLine, nLine, ver1);
			iZeile++;
			nScan = ReadAdaptationFromString(sLine, &adaptRead);
			iPatternFound = strcmp(adaptRead.sPatternID, adapt->sPatternID);
			if ((key_b[0] == adaptRead.adap_key[0]) && (key_b[1] == adaptRead.adap_key[1]) && 
				(key_b[2] == adaptRead.adap_key[2]) && (key_b[3] == adaptRead.adap_key[3]) && 
				(key_b[4] == adaptRead.adap_key[4]) && (iPatternFound == 0)){
					keyfound1			= 1;
					for (i=0;i<5;i++)
						adapt->adap_key[i]     = key_b[i];

					adapt->iFoundRead	= iZeile;
					adapt->adaptRatioFound		= adaptRead.adaptRatioFound;
					qcrnew				= adaptRead.adaptRatioFound;

					if(nScan > 4)	
						qtrnew              = adaptRead.tr_foundMean;
					qtr_fileMean        = adaptRead.tr_foundMean;
					qtr_fileHead        = adaptRead.tr_foundHead;
					qtr_fileTail		= adaptRead.tr_foundTail;
					adapt->iControl		= adaptRead.iControl;
					/* not allowed to read */
					if(	(iControl == 3) || (iControl == 0)) {
						qtr_fileMean = 1.f;
						qtr_fileHead = 1.f;
						qtr_fileTail = 1.f;
					}
					adapt->tr_foundMean		= qtr_fileMean;
					adapt->tr_foundHead		= qtr_fileHead;
					adapt->tr_foundTail		= qtr_fileTail;
					adapt->rec_plates	= adaptRead.rec_plates;
					adapt->vx_last		= adaptRead.vx_last;
					adapt->tf_last		= adaptRead.tf_last;
					adapt->ts_last		= adaptRead.ts_last;
					adapt->lx_last      = adaptRead.lx_last;
					adapt->tw_last      = adaptRead.tw_last;
					adapt->iSpare[0]    = adaptRead.iSpare[0];
					adapt->iSpare[1]    = adaptRead.iSpare[1];
					adapt->iSpare[2]    = adaptRead.iSpare[2];
					adapt->fSpare[0]    = adaptRead.fSpare[0];
					adapt->fSpare[1]    = adaptRead.fSpare[1];
					adapt->fSpare[2]    = adaptRead.fSpare[2];
					strcpy_s(adapt->plate_id_m1, adaptRead.plate_id_m1);
					strcpy_s(adapt->plate_id_m2, adaptRead.plate_id_m2);
					strcpy_s(adapt->plate_id_m3, adaptRead.plate_id_m3);
					adapt->iAdaptTimeLast = adaptRead.iAdaptTimeLast;
					time(&iAdaptTimeUse);
					adapt->iAdaptTimeDiff = (long)iAdaptTimeUse - adaptRead.iAdaptTimeLast;
			}
		}
			fclose(ver1);
	}


#ifdef DEBUG_FILE_WRITE_ADAPT
	WriteAdaptation(pdi->plateID, adapt);
#endif

	return(qtrnew);
}

int	CSelfLearnFunction::WriteAdaptation(const	char	*sPlateID,	const	ADAPTATION *adapt)
{
	char			sFAdapt[MESSAGE_MAX_LENGHT];
	char			sFAdaptPath[2*MESSAGE_MAX_LENGHT];
	FILE			*fadapt = NULL;
	strcpy_s(sFAdapt, sPlateID);
	strcat(sFAdapt, "_adapt.txt");
	CombinFilePath(sFAdapt, POSTPROCESS, sFAdaptPath);
	if((fadapt = fopen(sFAdaptPath,"w")) == NULL) return FALSE;
	fprintf(fadapt,"%40s   Adaption FILE\n",adapt->sFileAdapt);
	fprintf(fadapt,"%20lu  Adaption Key (First digit Level)\n",adapt->adap_key[0]);  //谢谦 2012-5-17
	fprintf(fadapt,"%20lu  Adaption Key (First digit Level)\n",adapt->adap_key[1]);
	fprintf(fadapt,"%20lu  Adaption Key (First digit Level)\n",adapt->adap_key[2]);
	fprintf(fadapt,"%20lu  Adaption Key (First digit Level)\n",adapt->adap_key[3]);
	fprintf(fadapt,"%20lu  Adaption Key (First digit Level)\n",adapt->adap_key[4]);

	fprintf(fadapt,"%20.4f    CR - Found\n",adapt->adaptRatioFound);
	fprintf(fadapt,"%20d    iControl\n",adapt->iControl);
	fprintf(fadapt,"%20d    iFoundRead (found in RecSet)\n",adapt->iFoundRead);
	fprintf(fadapt,"%20d    iFoundWrite (written to RecSet)\n",adapt->iFoundWrite);
	fprintf(fadapt,"%20d    max_plates\n",adapt->max_plates);
	fprintf(fadapt,"%20d    num_plates\n",adapt->num_plates);
	fprintf(fadapt,"%20s    plate_id_m1\n",adapt->plate_id_m1);
	fprintf(fadapt,"%20s    plate_id_m2\n",adapt->plate_id_m2);
	fprintf(fadapt,"%20s    plate_id_m3\n",adapt->plate_id_m3);
	fprintf(fadapt,"%20d    rec_plates (Plates in class)\n",adapt->rec_plates);
	fprintf(fadapt,"%40s    sPatternID\n",adapt->sPatternID);
	fprintf(fadapt,"%20.4f    tf_last  [度]\n",adapt->tf_last);
	fprintf(fadapt,"%20.4f    tr_foundMean [-]\n",adapt->tr_foundMean);
	fprintf(fadapt,"%20.4f    tr_foundHead [-]\n",adapt->tr_foundHead);
	fprintf(fadapt,"%20.4f    tr_foundTail [-]\n",adapt->tr_foundTail);
	fprintf(fadapt,"%20.4f    ts_last  [度]\n",adapt->ts_last);
	fprintf(fadapt,"%20.4f    vx_last  [m/s]\n",adapt->vx_last);
	fprintf(fadapt,"%20.4f    lx_last  [m]\n",adapt->lx_last);
	fprintf(fadapt,"%20.4f    tw_last  [度]\n",adapt->tw_last);
	fprintf(fadapt,"%20ld     iSpare[0]\n", adapt->iSpare[0]);
	fprintf(fadapt,"%20ld     iSpare[1]\n", adapt->iSpare[1]);
	fprintf(fadapt,"%20ld     iSpare[2]\n", adapt->iSpare[2]);
	fprintf(fadapt,"%20.4f    fSpare[0]\n", adapt->fSpare[0]);
	fprintf(fadapt,"%20.4f    fSpare[1]\n", adapt->fSpare[1]);
	fprintf(fadapt,"%20.4f    fSpare[2]\n", adapt->fSpare[2]);
	fprintf(fadapt,"%20ld    Last time adapted [abs]\n", adapt->iAdaptTimeLast);
	fprintf(fadapt,"%20ld    time difference [s]", adapt->iAdaptTimeDiff);
	fclose(fadapt);
	return TRUE;
}

int	CSelfLearnFunction::WriteAdaptationInfo(FILE	*fadapt,const	ADAPTATION *adapt)
{
	fprintf(fadapt,"\n\t 自学习报表");
	fprintf(fadapt,"\n\t 系数读取文件:  %40s", adapt->sFileAdapt);
	fprintf(fadapt,"\n\t 自学习索引0: %20lu (First digit Level)",adapt->adap_key[0]);  //谢谦 2012-5-17
	fprintf(fadapt,"\n\t 自学习索引1: %20lu (First digit Level)",adapt->adap_key[1]);
	fprintf(fadapt,"\n\t 自学习索引2: %20lu (First digit Level)",adapt->adap_key[2]);
	fprintf(fadapt,"\n\t 自学习索引3: %20lu (First digit Level)",adapt->adap_key[3]);
	fprintf(fadapt,"\n\t 自学习索引4: %20lu (First digit Level)",adapt->adap_key[4]);

	fprintf(fadapt,"\n\t 读到的修正系数:   %20.4f",adapt->adaptRatioFound);
	fprintf(fadapt,"\n\t 控制模式:     %20d (2: Read + Write)",adapt->iControl);
	fprintf(fadapt,"\n\t iFoundRead:   %20d (found in RecSet)",adapt->iFoundRead);
	fprintf(fadapt,"\n\t iFoundWrite:  %20d (written to RecSet)",adapt->iFoundWrite);
	fprintf(fadapt,"\n\t 最大学习数量:   %20d (-1: no limitation)",adapt->max_plates);
	fprintf(fadapt,"\n\t 已学习数量:   %20d",adapt->num_plates);
	fprintf(fadapt,"\n\t plateID前一:  %20s",adapt->plate_id_m1);
	fprintf(fadapt,"\n\t plateID前二:  %20s",adapt->plate_id_m2);
	fprintf(fadapt,"\n\t plateID前三:  %20s",adapt->plate_id_m3);
	fprintf(fadapt,"\n\t 已学习数量:   %20d",adapt->rec_plates);
	//fprintf(fadapt,"\n\t sPatternID:   %40s",adapt->sPatternID);
	fprintf(fadapt,"\n\t 前一 终轧温度:      %20.4f [度]",adapt->tf_last);
	fprintf(fadapt,"\n\t 平均修正系数: %20.4f [-]",adapt->tr_foundMean);
	fprintf(fadapt,"\n\t 头部修正系数: %20.4f [-]",adapt->tr_foundHead);
	fprintf(fadapt,"\n\t 尾部修正系数: %20.4f [-]",adapt->tr_foundTail);
	fprintf(fadapt,"\n\t 前一终冷温度:      %20.4f [度]",adapt->ts_last);
	fprintf(fadapt,"\n\t 前一辊速:      %20.4f [m/s]",adapt->vx_last);
	fprintf(fadapt,"\n\t 前一长度:      %20.4f [m]",adapt->lx_last);
	fprintf(fadapt,"\n\t 前一 水温       %20.4f[度]\n",adapt->tw_last);


	fprintf(fadapt,"\n\t %4.2f    fSpare[0]\n", adapt->fSpare[0]);  // add [9/19/2014 qian]
	fprintf(fadapt,"\n\t %4.2f    fSpare[1]\n", adapt->fSpare[1]);
	fprintf(fadapt,"\n\t %4.2f    fSpare[2]\n", adapt->fSpare[2]);

	fprintf(fadapt,"\n\t time Last Ad: %20d [abs]",adapt->iAdaptTimeLast);
	fprintf(fadapt,"\n\t time Diff.:   %20d [s]\n",adapt->iAdaptTimeDiff);
	return TRUE;
}

int	CSelfLearnFunction::ReadAdaptationInfo(const	char	*sPlateID, ADAPTATION *adapt)
{
	char			sFAdapt[MESSAGE_MAX_LENGHT];
	char			sFAdaptPath[2*MESSAGE_MAX_LENGHT];
	FILE			*fadapt = NULL;
	int				nMax = MESSAGE_MAX_LENGHT - 1;
	char			sLine[MESSAGE_MAX_LENGHT];

	strcpy_s(sFAdapt, sPlateID);
	strcat(sFAdapt, "_adapt.txt");
	CombinFilePath(sFAdapt, POSTPROCESS, sFAdaptPath);
	if((fadapt = fopen(sFAdaptPath,"r")) == NULL) {
		InitAdaptation(adapt);
		return FALSE;
	}
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%s",&adapt->sFileAdapt);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%lu",&adapt->adap_key[0]);          //谢谦 2012-5-17

	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%lu",&adapt->adap_key[1]);          //谢谦 2012-5-17
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%lu",&adapt->adap_key[2]);          //谢谦 2012-5-17
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%lu",&adapt->adap_key[3]);          //谢谦 2012-5-17
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%lu",&adapt->adap_key[4]);          //谢谦 2012-5-17

	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->adaptRatioFound);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%d",&adapt->iControl);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%d",&adapt->iFoundRead);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%d",&adapt->iFoundWrite);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%d",&adapt->max_plates);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%d",&adapt->num_plates);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%s",&adapt->plate_id_m1);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%s",&adapt->plate_id_m2);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%s",&adapt->plate_id_m3);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%d",&adapt->rec_plates);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%s",&adapt->sPatternID);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->tf_last);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->tr_foundMean);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->tr_foundHead);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->tr_foundTail);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->ts_last);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->vx_last);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->lx_last);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->tw_last);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%ld",&adapt->iSpare[0]);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%ld",&adapt->iSpare[1]);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%ld",&adapt->iSpare[2]);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->fSpare[0]);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->fSpare[1]);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%f",&adapt->fSpare[2]);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%ld",&adapt->iAdaptTimeLast);
	fgets(sLine, nMax, fadapt);
	sscanf(sLine,"%ld",&adapt->iAdaptTimeDiff);
	fclose(fadapt);
	return TRUE;
}


int	CSelfLearnFunction::ReadAdaptationFromString(const	char	*sRead,ADAPTATION *adapt)
{
	int iRet = 0;
	const	char	*sFormatRead =	"%ld%lu %lu %lu %lu %lu %s%f %ld%d%f%f%f"
		"%s%s%s%f%f%f%f%f"
		"%ld%ld%ld%f%f%f";
	iRet = sscanf(sRead, sFormatRead, 
		&(adapt->iAdaptTimeLast),

		&(adapt->adap_key[0]),   //此处需要读5个数 谢谦 2012-5-17
		&(adapt->adap_key[1]),  
		&(adapt->adap_key[2]),  
		&(adapt->adap_key[3]),  
		&(adapt->adap_key[4]),  

		adapt->sPatternID, 
		&(adapt->adaptRatioFound), 
		&(adapt->rec_plates),			
		&(adapt->iControl), 
		&(adapt->tr_foundMean), 
		&(adapt->tr_foundHead), 
		&(adapt->tr_foundTail), 
		adapt->plate_id_m1, 
		adapt->plate_id_m2,
		adapt->plate_id_m3,
		&(adapt->vx_last), 
		&(adapt->tf_last), 
		&(adapt->ts_last), 
		&(adapt->lx_last), 
		&(adapt->tw_last),
		&(adapt->iSpare[0]), 
		&(adapt->iSpare[1]), 
		&(adapt->iSpare[2]), 
		&(adapt->fSpare[0]), 
		&(adapt->fSpare[1]), 
		&(adapt->fSpare[2]));
	adapt->iFoundRead		= -1;
	adapt->iFoundWrite		= -1;
	adapt->max_plates		= 0;
	adapt->num_plates		= 0;
	adapt->iAdaptTimeDiff	= 0L;
	return(iRet);
}

int	CSelfLearnFunction::WriteAdaptationToString(const	ADAPTATION *adapt,char	*sWrite)
{
	const  char	*sFormatWrite = "%12ld %12lu  %12lu %12lu %12lu %12lu %40s %12.4f  %12ld %6d"
		" %12.4f %12.4f %12.4f %20s %20s %20s"
		" %12.4f %12.4f %12.4f %12.4f %12.4f"
		"%12ld %12ld %12ld %12.4f %12.4f %12.4f\n";
	int iRet = sprintf(sWrite, sFormatWrite, 
		adapt->iAdaptTimeLast,

		adapt->adap_key[0],  //谢谦 2012-5-17 加入
		adapt->adap_key[1], 
		adapt->adap_key[2], 
		adapt->adap_key[3], 
		adapt->adap_key[4], 

		adapt->sPatternID, 
		adapt->adaptRatioFound, 
		adapt->rec_plates,			
		adapt->iControl, 
		adapt->tr_foundMean, 
		adapt->tr_foundHead, 
		adapt->tr_foundTail, 
		adapt->plate_id_m1, 
		adapt->plate_id_m2,
		adapt->plate_id_m3,
		adapt->vx_last, 
		adapt->tf_last, 
		adapt->ts_last, 
		adapt->lx_last, 
		adapt->tw_last,
		adapt->iSpare[0], 
		adapt->iSpare[1], 
		adapt->iSpare[2], 
		adapt->fSpare[0], 
		adapt->fSpare[1], 
		adapt->fSpare[2]);

	return(iRet);
}

/*  initilize adaptation value                                    */
int	CSelfLearnFunction::InitAdaptation(ADAPTATION *adapt)
{
	int i;
	strcpy_s(adapt->sFileAdapt, "0");
	for (i=0;i<5;i++)
		adapt->adap_key[i]    = 0L;

	adapt->adaptRatioFound    = 0.0f;
	adapt->iControl    = 2;	 /* 2: default read and write */
	adapt->iFoundRead  = -1; /* read from line (record) No. */ 
	adapt->iFoundWrite = -1; /* write to  line (record) No. */
	adapt->max_plates  = -1; /* no limitation of Numberes */
	adapt->num_plates  = 0;
	strcpy_s(adapt->plate_id_m1, "0");
	strcpy_s(adapt->plate_id_m2, "0");
	strcpy_s(adapt->plate_id_m3, "0");
	adapt->rec_plates  = 0; /* no adaption FILE in this class */
	strcpy_s(adapt->sPatternID, "0");
	adapt->tf_last      = 0.0f;
	adapt->tr_foundMean = 0.0f;
	adapt->tr_foundHead = 0.0f;
	adapt->tr_foundTail = 0.0f;
	adapt->ts_last      = 0.0f;
	adapt->vx_last      = 0.0f;
	adapt->lx_last      = 0.0f;
	adapt->tw_last      = 0.0f;
	adapt->iAdaptTimeLast = 0l;
	adapt->iAdaptTimeDiff = 0l;
	adapt->iSpare[0]    = 0l;
	adapt->iSpare[1]    = 0l;
	adapt->iSpare[2]    = 0l;
	adapt->fSpare[0]    = 0.0f;
	adapt->fSpare[1]    = 0.0f;
	adapt->fSpare[2]    = 0.0f;
	return TRUE;
}


/// @函数说明 函数简要说明-测试函数 
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/28/2013
int	CSelfLearnFunction::CopyAdaptation(
	const	ADAPTATION *adapt_nm1,	/* basis 信息 */
	ADAPTATION *adapt)		/* copy */
{
	int i;
	strcpy_s(adapt->sFileAdapt, adapt_nm1->sFileAdapt);
	for (i=0;i<5;i++)
		adapt->adap_key[i]			= adapt_nm1->adap_key[i];

	adapt->adaptRatioFound			= adapt_nm1->adaptRatioFound;
	adapt->iControl			= adapt_nm1->iControl;
	adapt->iFoundRead		= adapt_nm1->iFoundRead; 
	adapt->iFoundWrite		= adapt_nm1->iFoundWrite;
	adapt->max_plates		= adapt_nm1->max_plates;
	adapt->num_plates		= adapt_nm1->num_plates;
	strcpy_s(adapt->plate_id_m1, adapt_nm1->plate_id_m1);
	strcpy_s(adapt->plate_id_m2, adapt_nm1->plate_id_m2);
	strcpy_s(adapt->plate_id_m3, adapt_nm1->plate_id_m3);
	/* add new 板材 later if new 自适应value are successful calculated */
	adapt->rec_plates		= adapt_nm1->rec_plates; 
	strcpy_s(adapt->sPatternID, adapt_nm1->sPatternID);
	adapt->tf_last			= adapt_nm1->tf_last;
	adapt->tr_foundMean		= adapt_nm1->tr_foundMean;
	adapt->tr_foundHead		= adapt_nm1->tr_foundHead;
	adapt->tr_foundTail		= adapt_nm1->tr_foundTail;
	adapt->ts_last			= adapt_nm1->ts_last;
	adapt->vx_last			= adapt_nm1->vx_last;
	adapt->lx_last			= adapt_nm1->lx_last;
	adapt->tw_last			= adapt_nm1->tw_last;
	adapt->iAdaptTimeLast	= adapt_nm1->iAdaptTimeLast;
	adapt->iAdaptTimeDiff	= adapt_nm1->iAdaptTimeDiff;
	adapt->iSpare[0]		= adapt_nm1->iSpare[0];
	adapt->iSpare[1]		= adapt_nm1->iSpare[1];
	adapt->iSpare[2]		= adapt_nm1->iSpare[2];
	adapt->fSpare[0]		= adapt_nm1->fSpare[0];
	adapt->fSpare[1]		= adapt_nm1->fSpare[1];
	adapt->fSpare[2]		= adapt_nm1->fSpare[2];
	return TRUE;
}

/*  initilize adaptation value based on other 自适应value     */
/*  shift to next entry (modified copy)                            */
int	CSelfLearnFunction::InitAdaptation(
	const	ADAPTATION *adapt_nm1,	/* basis 信息 */
	const	char	*sPlateID,	/* 实际 板材 ID */
	ADAPTATION *adapt)		/* copy and shift */
{
	int i;
	for (i=0;i<5;i++)
		adapt->adap_key[i]    = adapt_nm1->adap_key[i];

	adapt->adaptRatioFound    = 0.0f;
	adapt->iControl    = adapt_nm1->iControl;	 /* 2: default read and write */
	adapt->iFoundRead  = adapt_nm1->iFoundRead; /* read from line (record) No. */ 
	adapt->iFoundWrite = adapt_nm1->iFoundWrite; /* write to  line (record) No. */
	adapt->max_plates  = adapt_nm1->max_plates; /* no limitation of Numberes */
	adapt->num_plates  = adapt_nm1->num_plates;
	strcpy_s(adapt->plate_id_m1, sPlateID);
	strcpy_s(adapt->plate_id_m2, adapt_nm1->plate_id_m1); /* shift by one */
	strcpy_s(adapt->plate_id_m3, adapt_nm1->plate_id_m2);
	/* add new 板材 later if new 自适应value are successful calculated */
	adapt->rec_plates  = adapt_nm1->rec_plates; 
	strcpy_s(adapt->sPatternID, adapt_nm1->sPatternID);
	adapt->tf_last      = 0.0f;
	adapt->tr_foundMean = 0.0f;
	adapt->tr_foundHead = 0.0f;
	adapt->tr_foundTail = 0.0f;
	adapt->ts_last      = 0.0f;
	adapt->vx_last      = 0.0f;
	adapt->lx_last      = 0.0f;
	adapt->tw_last      = 0.0f;
	adapt->iSpare[0]    = adapt_nm1->iSpare[0];
	adapt->iSpare[1]    = adapt_nm1->iSpare[1];
	adapt->iSpare[2]    = adapt_nm1->iSpare[2];
	adapt->fSpare[0]    = adapt_nm1->fSpare[0];
	adapt->fSpare[1]    = adapt_nm1->fSpare[1];
	adapt->fSpare[2]    = adapt_nm1->fSpare[2];
	return TRUE;
}



//ACC段自学习关键字的确定                              //
unsigned long int CSelfLearnFunction::KeepAdaptionKey(unsigned long int key_b[5],const	INPUTDATAPDI *pk1,float waterTemp)
{
	unsigned long int knc,kam1, kcs1, kpm1, ksm1, kjm1, ktf1, ktf2,kts1,kts2,kts3,kts4,kts5,kcr1,kcr2,kd1,kd2,kw1,kl1,kl2,knb1,kc1,kmn1, kfl1,kfl2,kfl3,kfl4,ksp;
	unsigned long int kwaterTemp;
	//const int iAccMode = (int) (pk1->acc_mode);
	const int iAccMode = BACKACC;
	//const int iCoolState = (int) (pk1->coolStatus);  useless
	const int iCoolState = 1;

	const int iPassMode = 1;
	const int iSecMode = 1;
	const int iJetMode = 1;

	const	float iTempFinish  	= 0;  
	//const	float iTempStop	    = pk1->targetFinishCoolTemp;
	const	float iTempStop	    = pk1->initFinishRollingTem;  //谢谦 保存的终冷Temperature信息
	//const float iCoolingRate  = pk1->cr;
	const float iCoolingRate  = 0.0;
	const	float	iThick		=  pk1->thick * 1000.f;
	const float iWidth    =  0.0;
	const float iLength   =  0.0;

	const float iAlloyNb = pk1->alloy_nb;
	float iAlloyC    = pk1->alloy_c;
	const float iAlloyMn   = pk1->alloy_mn;
	const float iWaterTemp   = waterTemp;

	const	float	iFlowRate  = 0;	

	kam1=0;
	if(iAccMode==1) kam1=1;
	if(iAccMode==2) kam1=2;
	if(iAccMode==3) kam1=3;
	if(iAccMode==4) kam1=4;
	if(iAccMode==5) kam1=5;
	if(iAccMode==6) kam1=6;

	kcs1=0;
	if(iCoolState==1) kcs1=1;
	if(iCoolState==2) kcs1=2;

	kpm1=0;
	if(iPassMode==1) kpm1=1;
	if(iPassMode==2) kpm1=2;
	if(iPassMode==3) kpm1=3;

	ksm1=0;
	if(iSecMode==1) ksm1=1;
	if(iSecMode==2) ksm1=2;
	if(iSecMode==3) ksm1=3;

	kjm1=0;
	if(iJetMode==1) kjm1=1;
	if(iJetMode==2) kjm1=2;
	if(iJetMode==3) kjm1=3;
	if(iJetMode==4) kjm1=4;
	if(iJetMode==5) kjm1=5;
	if(iJetMode==6) kjm1=6;

	ktf1 = 0;
	if (iTempFinish > 711)	  ktf1 = 1;
	if (iTempFinish > 731)	  ktf1 = 2;
	if (iTempFinish > 751)	  ktf1 = 3;
	if (iTempFinish > 771)	  ktf1 = 4;
	if (iTempFinish > 791)	  ktf1 = 5;
	if (iTempFinish > 811)	  ktf1 = 6; 
	if (iTempFinish > 831)	  ktf1 = 7; 
	if (iTempFinish > 851)	  ktf1 = 8;
	if (iTempFinish > 871)	  ktf1 = 9;
	ktf2 = 0;
	if (iTempFinish > 891)	  ktf2 = 1;
	if (iTempFinish > 911)	  ktf2 = 2;
	if (iTempFinish > 931)	  ktf2 = 3;
	if (iTempFinish > 951)	  ktf2 = 4;
	if (iTempFinish > 971)	  ktf2 = 5;
	if (iTempFinish > 991)	  ktf2 = 6;
	if (iTempFinish > 1011)	  ktf2 = 7;
	if (iTempFinish > 1031)	  ktf2 = 8;
	if (iTempFinish > 1051)	  ktf2 = 9;

	kts1 = 0; 
	if (iTempStop > 91) 	  kts1 = 1;
	if (iTempStop > 111)	  kts1 = 2;
	if (iTempStop > 131)	  kts1 = 3;
	if (iTempStop > 151)	  kts1 = 4;
	if (iTempStop > 171)	  kts1 = 5;
	if (iTempStop > 191)	  kts1 = 6;
	if (iTempStop > 211)	  kts1 = 7;
	if (iTempStop > 231)	  kts1 = 8;
	if (iTempStop > 251)	  kts1 = 9;

	kts2 = 0;
	if (iTempStop > 271)	  kts2 = 1;
	if (iTempStop > 291)	  kts2 = 2;
	if (iTempStop > 311)	  kts2 = 3;
	if (iTempStop > 331)	  kts2 = 4;
	if (iTempStop > 351)	  kts2 = 5;
	if (iTempStop > 371)	  kts2 = 6;
	if (iTempStop > 391)	  kts2 = 7;
	if (iTempStop > 411)	  kts2 = 8;
	if (iTempStop > 431)	  kts2 = 9;

	kts3 = 0;  
	if (iTempStop > 451)	  kts3 = 1;
	if (iTempStop > 471)	  kts3 = 2;
	if (iTempStop > 491)	  kts3 = 3;
	if (iTempStop > 511)	  kts3 = 4;
	if (iTempStop > 531)	  kts3 = 5;
	if (iTempStop > 551)	  kts3 = 6;
	if (iTempStop > 571)	  kts3 = 7;
	if (iTempStop > 591)	  kts3 = 8;
	if (iTempStop > 611)	  kts3 = 9;

	kts4 = 0;
	if (iTempStop > 631)	  kts4 = 1;
	if (iTempStop > 651)	  kts4 = 2;
	if (iTempStop > 671)	  kts4 = 3;
	if (iTempStop > 691)	  kts4 = 4;
	if (iTempStop > 711)	  kts4 = 5;
	if (iTempStop > 731)	  kts4 = 6;
	if (iTempStop > 751)	  kts4 = 7;
	if (iTempStop > 771)	  kts4 = 8;
	if (iTempStop > 791)	  kts4 = 9;

	kts5 = 0;
	if (iTempStop > 811)	  kts5 = 1;
	if (iTempStop > 831)	  kts5 = 2;
	if (iTempStop > 851)	  kts5 = 3;
	if (iTempStop > 871)	  kts5 = 4;
	if (iTempStop > 891)	  kts5 = 5;
	if (iTempStop > 911)	  kts5 = 6;
	if (iTempStop > 931)	  kts5 = 7;
	if (iTempStop > 951)	  kts5 = 8;
	if (iTempStop > 971)	  kts5 = 9;

	kcr1=0;
	if (iCoolingRate > 2)  kcr1=1;
	if (iCoolingRate > 5)  kcr1=2;
	if (iCoolingRate > 10) kcr1=3;
	if (iCoolingRate > 20) kcr1=4;
	if (iCoolingRate > 30) kcr1=5;
	if (iCoolingRate > 40) kcr1=6;
	if (iCoolingRate > 50) kcr1=7;
	if (iCoolingRate > 60) kcr1=8;
	if (iCoolingRate > 70) kcr1=9;

	kcr2=0;
	if (iCoolingRate > 80)  kcr2=1;
	if (iCoolingRate > 90)  kcr2=2;
	if (iCoolingRate > 100) kcr2=3;
	if (iCoolingRate > 110) kcr2=4;
	if (iCoolingRate > 120) kcr2=5;
	if (iCoolingRate > 130) kcr2=6;
	if (iCoolingRate > 140) kcr2=7;
	if (iCoolingRate > 150) kcr2=8;
	if (iCoolingRate > 160) kcr2=9;

	kd1 = 0;
	if (iThick > 10.8) kd1 = 1;
	if (iThick > 12.8) kd1 = 2;
	if (iThick > 14.8) kd1 = 3;
	if (iThick > 16.8) kd1 = 4;
	if (iThick > 18.8) kd1 = 5;
	if (iThick > 20.8) kd1 = 6;
	if (iThick > 23.8) kd1 = 7;
	if (iThick > 25.8) kd1 = 8;
	if (iThick > 28.8) kd1 = 9;
	kd2 = 0;
	if (iThick > 31.8)  kd2 = 1;
	if (iThick > 34.8)  kd2 = 2;
	if (iThick > 37.8)  kd2 = 3;
	if (iThick > 40.8)  kd2 = 4;
	if (iThick > 45.8)  kd2 = 5;
	if (iThick > 50.8)  kd2 = 6;
	if (iThick > 55.8)  kd2 = 7;
	if (iThick > 60.8) kd2 = 8;
	if (iThick > 65.8) kd2 = 9;

	kw1=0;
	if(iThick>70.8) kw1=1;
	if(iThick>75.8) kw1=2;
	if(iThick>80.8) kw1=3;
	if(iThick>85.8) kw1=4;
	if(iThick>90.8) kw1=5;

	kl1=0;
	if(iLength>2000)  kl1=1;
	if(iLength>3000)  kl1=2;
	if(iLength>4000)  kl1=3;
	if(iLength>5000)  kl1=4;
	if(iLength>7000)  kl1=5;
	if(iLength>9000)  kl1=6;
	if(iLength>11000) kl1=7;
	if(iLength>13000) kl1=8;
	if(iLength>15000) kl1=9;
	kl2=0;
	if(iLength>20000) kl2=1;
	if(iLength>25000) kl2=2;
	if(iLength>30000) kl2=3;
	if(iLength>35000) kl2=4;
	if(iLength>40000) kl2=5;
	if(iLength>45000) kl2=6;
	if(iLength>50000) kl2=7;
	if(iLength>55000) kl2=8;
	if(iLength>60000) kl2=9;


	knb1=0;
	if (iAlloyNb>=0.0f) knb1=1;
	if (iAlloyNb>=0.005) knb1=2;
	if (iAlloyNb>0.04) knb1=3;
	if (iAlloyNb>0.1) knb1=4;

	kc1=0;
	if(iAlloyC>0.01) kc1=1;
	if(iAlloyC>0.06) kc1=2;
	if(iAlloyC>0.09)  kc1=3;
	if(iAlloyC>0.15)  kc1=4;

	kmn1=0;
	if(iAlloyMn>0.04) kmn1=1;
	if(iAlloyMn>0.5) kmn1=2;
	if(iAlloyMn>1.0) kmn1=3;
	if(iAlloyMn>1.7) kmn1=4;
	if(iAlloyMn>2.0) kmn1=5;

	kwaterTemp=0;
	if(iWaterTemp>10) kwaterTemp=1;
	if(iWaterTemp>15) kwaterTemp=2;
	if(iWaterTemp>20) kwaterTemp=3;
	if(iWaterTemp>25) kwaterTemp=4;
	if(iWaterTemp>30) kwaterTemp=5;
	if(iWaterTemp>35) kwaterTemp=6;


	kfl1 = 0;  
	if (iFlowRate > 100)	  kfl1 = 1;
	if (iFlowRate > 200)	  kfl1 = 2;
	if (iFlowRate > 300)	  kfl1 = 3;
	if (iFlowRate > 400)	  kfl1 = 4;
	if (iFlowRate > 500)	  kfl1 = 5;
	if (iFlowRate > 600)	  kfl1 = 6;
	if (iFlowRate > 700)	  kfl1 = 7;
	if (iFlowRate > 800)	  kfl1 = 8;
	if (iFlowRate > 900)	  kfl1 = 9;
	kfl2 = 0;  
	if (iFlowRate > 1000)	  kfl2 = 1;
	if (iFlowRate > 1100)	  kfl2 = 2;
	if (iFlowRate > 1200)	  kfl2 = 3;
	if (iFlowRate > 1300)	  kfl2 = 4;
	if (iFlowRate > 1400)	  kfl2 = 5;
	if (iFlowRate > 1500)	  kfl2 = 6;
	if (iFlowRate > 1600)	  kfl2 = 7;
	if (iFlowRate > 1700)	  kfl2 = 8;
	if (iFlowRate > 1800)	  kfl2 = 9;
	kfl3 = 0;  
	if (iFlowRate > 1900)	  kfl3 = 1;
	if (iFlowRate > 2000)	  kfl3 = 2;
	if (iFlowRate > 2100)	  kfl3 = 3;
	if (iFlowRate > 2200)	  kfl3 = 4;
	if (iFlowRate > 2300)	  kfl3 = 5;
	if (iFlowRate > 2400)	  kfl3 = 6;
	if (iFlowRate > 2500)	  kfl3 = 7;
	if (iFlowRate > 2600)	  kfl3 = 8;
	if (iFlowRate > 2700)	  kfl3 = 9;
	kfl4 = 0;  
	if (iFlowRate > 2800)	  kfl4 = 1;
	if (iFlowRate > 2900)	  kfl4 = 2;
	if (iFlowRate > 3000)	  kfl4 = 3;
	if (iFlowRate > 3100)	  kfl4 = 4;
	if (iFlowRate > 3200)	  kfl4 = 5;
	if (iFlowRate > 3300)	  kfl4 = 6;
	if (iFlowRate > 3400)	  kfl4 = 7;
	if (iFlowRate > 3500)	  kfl4 = 8;
	if (iFlowRate > 3600)	  kfl4 = 9;


	ksp = 0;
	ksp=pk1->isHeadFlated;  //add by xie 10-17

	knc = 1;        

	key_b [0]= 0;
	key_b [0] += (knc   * 1000000l);   
	key_b [0] += (kam1  *  100000l);  
	key_b [0] += (kcs1  *   10000l);   
	key_b [0] += (kpm1  *    1000l);  
	key_b [0] += (ksm1  *     100l);  
	key_b [0] += (kjm1  *      10l);   
	key_b [0] += (ksp   *       1l);   

	key_b [1]=0;
	key_b [1] += (knc   * 1000000000l);   
	key_b [1] += (ktf1  *  100000000l);  
	key_b [1] += (ktf2  *   10000000l);  
	key_b [1] += (kts1  *    1000000l); 
	key_b [1] += (kts2  *     100000l);   
	key_b [1] += (kts3  *      10000l);   
	key_b [1] += (kts4  *       1000l); 
	key_b [1] += (kcr1  *        100l);  
	key_b [1] += (kcr2  *         10l);   
	key_b [1] += (ksp   *          1l);  

	key_b [2]= 0;
	key_b [2] += (knc  * 1000000l);  
	key_b [2] += (kd1  *  100000l);  
	key_b [2] += (kd2  *   10000l); 
	key_b [2] += (kw1  *    1000l);  
	key_b [2] += (kl1  *     100l);
	key_b [2] += (kl2  *      10l); 
	key_b [2] += (ksp  *       1l);  

	key_b [3]=0;
	key_b [3] += (kwaterTemp   * 100000l);  
	key_b [3] += (knc   * 10000l);  
	key_b [3] += (knb1 *  1000l); 
	key_b [3] += (kc1   *   100l);  
	key_b [3] += (kmn1  *    10l);  
	key_b [3] += (ksp   *     1l); 

	key_b [4]=0;
	key_b [4] += (knc   * 100000l);  
	key_b [4] += (kfl1  *  10000l);
	key_b [4] += (kfl2  *   1000l);  
	key_b [4] += (kfl3  *    100l);  
	key_b [4] += (kfl4  *     10l);  
	key_b [4] += (ksp   *      1l);   

	return 0;

}

unsigned long int CSelfLearnFunction::KeepAdaptionKey2(unsigned long int key_b[5],const	INPUTDATAPDI *pk1,float waterTemp)
{
	unsigned long int knc,kam1, kcs1, kpm1, ksm1, kjm1, ktf1, ktf2,kts1,kts2,kts3,kts4,kts5,kcr1,kcr2,kd1,kd2,kw1,kl1,kl2,knb1,kc1,kmn1, kfl1,kfl2,kfl3,kfl4,ksp;
	unsigned long int kwaterTemp;
	const int iAccMode = BACKACC;
	const int iCoolState = 1;

	const int iPassMode = 1;
	const int iSecMode = 1;
	const int iJetMode = 1;

	const	float iTempFinish  	= 0;  
	const	float iTempStop	    = pk1->initFinishRollingTem;  //谢谦 保存的终冷Temperature信息
	const float iCoolingRate  = 0.0;
	const	float	iThick		=  pk1->thick * 1000.f;
	const float iWidth    =  0.0;
	const float iLength   =  0.0;

	const float iAlloyNb = pk1->alloy_nb;
	float iAlloyC    = pk1->alloy_c;
	const float iAlloyMn   = pk1->alloy_mn;
	const float iWaterTemp   = waterTemp;

	const	float	iFlowRate  = 0;	

	kam1=0;
	if(iAccMode==1) kam1=1;
	if(iAccMode==2) kam1=2;
	if(iAccMode==3) kam1=3;
	if(iAccMode==4) kam1=4;
	if(iAccMode==5) kam1=5;
	if(iAccMode==6) kam1=6;

	kcs1=0;
	if(iCoolState==1) kcs1=1;
	if(iCoolState==2) kcs1=2;

	kpm1=0;
	if(iPassMode==1) kpm1=1;
	if(iPassMode==2) kpm1=2;
	if(iPassMode==3) kpm1=3;

	ksm1=0;
	if(iSecMode==1) ksm1=1;
	if(iSecMode==2) ksm1=2;
	if(iSecMode==3) ksm1=3;

	kjm1=0;
	if(iJetMode==1) kjm1=1;
	if(iJetMode==2) kjm1=2;
	if(iJetMode==3) kjm1=3;
	if(iJetMode==4) kjm1=4;
	if(iJetMode==5) kjm1=5;
	if(iJetMode==6) kjm1=6;

	ktf1 = 0;
	if (iTempFinish > 711)	  ktf1 = 1;
	if (iTempFinish > 731)	  ktf1 = 2;
	if (iTempFinish > 751)	  ktf1 = 3;
	if (iTempFinish > 771)	  ktf1 = 4;
	if (iTempFinish > 791)	  ktf1 = 5;
	if (iTempFinish > 811)	  ktf1 = 6; 
	if (iTempFinish > 831)	  ktf1 = 7; 
	if (iTempFinish > 851)	  ktf1 = 8;
	if (iTempFinish > 871)	  ktf1 = 9;
	ktf2 = 0;
	if (iTempFinish > 891)	  ktf2 = 1;
	if (iTempFinish > 911)	  ktf2 = 2;
	if (iTempFinish > 931)	  ktf2 = 3;
	if (iTempFinish > 951)	  ktf2 = 4;
	if (iTempFinish > 971)	  ktf2 = 5;
	if (iTempFinish > 991)	  ktf2 = 6;
	if (iTempFinish > 1011)	  ktf2 = 7;
	if (iTempFinish > 1031)	  ktf2 = 8;
	if (iTempFinish > 1051)	  ktf2 = 9;

	kts1 = 0; 
	if (iTempStop > 91) 	  kts1 = 1;
	if (iTempStop > 111)	  kts1 = 2;
	if (iTempStop > 131)	  kts1 = 3;
	if (iTempStop > 151)	  kts1 = 4;
	if (iTempStop > 171)	  kts1 = 5;
	if (iTempStop > 191)	  kts1 = 6;
	if (iTempStop > 211)	  kts1 = 7;
	if (iTempStop > 231)	  kts1 = 8;
	if (iTempStop > 251)	  kts1 = 9;

	kts2 = 0;
	if (iTempStop > 271)	  kts2 = 1;
	if (iTempStop > 291)	  kts2 = 2;
	if (iTempStop > 311)	  kts2 = 3;
	if (iTempStop > 331)	  kts2 = 4;
	if (iTempStop > 351)	  kts2 = 5;
	if (iTempStop > 371)	  kts2 = 6;
	if (iTempStop > 391)	  kts2 = 7;
	if (iTempStop > 411)	  kts2 = 8;
	if (iTempStop > 431)	  kts2 = 9;

	kts3 = 0;  
	if (iTempStop > 451)	  kts3 = 1;
	if (iTempStop > 471)	  kts3 = 2;
	if (iTempStop > 491)	  kts3 = 3;
	if (iTempStop > 511)	  kts3 = 4;
	if (iTempStop > 531)	  kts3 = 5;
	if (iTempStop > 551)	  kts3 = 6;
	if (iTempStop > 571)	  kts3 = 7;
	if (iTempStop > 591)	  kts3 = 8;
	if (iTempStop > 611)	  kts3 = 9;

	kts4 = 0;
	if (iTempStop > 631)	  kts4 = 1;
	if (iTempStop > 651)	  kts4 = 2;
	if (iTempStop > 671)	  kts4 = 3;
	if (iTempStop > 691)	  kts4 = 4;
	if (iTempStop > 711)	  kts4 = 5;
	if (iTempStop > 731)	  kts4 = 6;
	if (iTempStop > 751)	  kts4 = 7;
	if (iTempStop > 771)	  kts4 = 8;
	if (iTempStop > 791)	  kts4 = 9;

	kts5 = 0;
	if (iTempStop > 811)	  kts5 = 1;
	if (iTempStop > 831)	  kts5 = 2;
	if (iTempStop > 851)	  kts5 = 3;
	if (iTempStop > 871)	  kts5 = 4;
	if (iTempStop > 891)	  kts5 = 5;
	if (iTempStop > 911)	  kts5 = 6;
	if (iTempStop > 931)	  kts5 = 7;
	if (iTempStop > 951)	  kts5 = 8;
	if (iTempStop > 971)	  kts5 = 9;

	kcr1=0;
	if (iCoolingRate > 2)  kcr1=1;
	if (iCoolingRate > 5)  kcr1=2;
	if (iCoolingRate > 10) kcr1=3;
	if (iCoolingRate > 20) kcr1=4;
	if (iCoolingRate > 30) kcr1=5;
	if (iCoolingRate > 40) kcr1=6;
	if (iCoolingRate > 50) kcr1=7;
	if (iCoolingRate > 60) kcr1=8;
	if (iCoolingRate > 70) kcr1=9;

	kcr2=0;
	if (iCoolingRate > 80)  kcr2=1;
	if (iCoolingRate > 90)  kcr2=2;
	if (iCoolingRate > 100) kcr2=3;
	if (iCoolingRate > 110) kcr2=4;
	if (iCoolingRate > 120) kcr2=5;
	if (iCoolingRate > 130) kcr2=6;
	if (iCoolingRate > 140) kcr2=7;
	if (iCoolingRate > 150) kcr2=8;
	if (iCoolingRate > 160) kcr2=9;

	kd1 = 0;
	if (iThick > 10.8) kd1 = 1;
	if (iThick > 12.8) kd1 = 2;
	if (iThick > 14.8) kd1 = 3;
	if (iThick > 16.8) kd1 = 4;
	if (iThick > 18.8) kd1 = 5;
	if (iThick > 20.8) kd1 = 6;
	if (iThick > 23.8) kd1 = 7;
	if (iThick > 25.8) kd1 = 8;
	if (iThick > 28.8) kd1 = 9;
	kd2 = 0;
	if (iThick > 31.8)  kd2 = 1;
	if (iThick > 34.8)  kd2 = 2;
	if (iThick > 37.8)  kd2 = 3;
	if (iThick > 40.8)  kd2 = 4;
	if (iThick > 45.8)  kd2 = 5;
	if (iThick > 50.8)  kd2 = 6;
	if (iThick > 55.8)  kd2 = 7;
	if (iThick > 60.8) kd2 = 8;
	if (iThick > 65.8) kd2 = 9;

	kw1=0;
	if(iThick>70.8) kw1=1;
	if(iThick>75.8) kw1=2;
	if(iThick>80.8) kw1=3;
	if(iThick>85.8) kw1=4;
	if(iThick>90.8) kw1=5;

	kl1=0;
	if(iLength>2000)  kl1=1;
	if(iLength>3000)  kl1=2;
	if(iLength>4000)  kl1=3;
	if(iLength>5000)  kl1=4;
	if(iLength>7000)  kl1=5;
	if(iLength>9000)  kl1=6;
	if(iLength>11000) kl1=7;
	if(iLength>13000) kl1=8;
	if(iLength>15000) kl1=9;
	kl2=0;
	if(iLength>20000) kl2=1;
	if(iLength>25000) kl2=2;
	if(iLength>30000) kl2=3;
	if(iLength>35000) kl2=4;
	if(iLength>40000) kl2=5;
	if(iLength>45000) kl2=6;
	if(iLength>50000) kl2=7;
	if(iLength>55000) kl2=8;
	if(iLength>60000) kl2=9;


	knb1=0;
	if (iAlloyNb>=0.0f) knb1=1;
	if (iAlloyNb>=0.005) knb1=2;
	if (iAlloyNb>0.04) knb1=3;
	if (iAlloyNb>0.1) knb1=4;

	kc1=0;
	if(iAlloyC>0.01) kc1=1;
	if(iAlloyC>0.06) kc1=2;
	if(iAlloyC>0.09)  kc1=3;
	if(iAlloyC>0.15)  kc1=4;

	kmn1=0;
	if(iAlloyMn>0.04) kmn1=1;
	if(iAlloyMn>0.5) kmn1=2;
	if(iAlloyMn>1.0) kmn1=3;
	if(iAlloyMn>1.7) kmn1=4;
	if(iAlloyMn>2.0) kmn1=5;

	kwaterTemp=0;
	if(iWaterTemp>10) kwaterTemp=1;
	if(iWaterTemp>15) kwaterTemp=2;
	if(iWaterTemp>20) kwaterTemp=3;
	if(iWaterTemp>25) kwaterTemp=4;
	if(iWaterTemp>30) kwaterTemp=5;
	if(iWaterTemp>35) kwaterTemp=6;

	kfl1 = 0;  
	if (iFlowRate > 100)	  kfl1 = 1;
	if (iFlowRate > 200)	  kfl1 = 2;
	if (iFlowRate > 300)	  kfl1 = 3;
	if (iFlowRate > 400)	  kfl1 = 4;
	if (iFlowRate > 500)	  kfl1 = 5;
	if (iFlowRate > 600)	  kfl1 = 6;
	if (iFlowRate > 700)	  kfl1 = 7;
	if (iFlowRate > 800)	  kfl1 = 8;
	if (iFlowRate > 900)	  kfl1 = 9;
	kfl2 = 0;  
	if (iFlowRate > 1000)	  kfl2 = 1;
	if (iFlowRate > 1100)	  kfl2 = 2;
	if (iFlowRate > 1200)	  kfl2 = 3;
	if (iFlowRate > 1300)	  kfl2 = 4;
	if (iFlowRate > 1400)	  kfl2 = 5;
	if (iFlowRate > 1500)	  kfl2 = 6;
	if (iFlowRate > 1600)	  kfl2 = 7;
	if (iFlowRate > 1700)	  kfl2 = 8;
	if (iFlowRate > 1800)	  kfl2 = 9;
	kfl3 = 0;  
	if (iFlowRate > 1900)	  kfl3 = 1;
	if (iFlowRate > 2000)	  kfl3 = 2;
	if (iFlowRate > 2100)	  kfl3 = 3;
	if (iFlowRate > 2200)	  kfl3 = 4;
	if (iFlowRate > 2300)	  kfl3 = 5;
	if (iFlowRate > 2400)	  kfl3 = 6;
	if (iFlowRate > 2500)	  kfl3 = 7;
	if (iFlowRate > 2600)	  kfl3 = 8;
	if (iFlowRate > 2700)	  kfl3 = 9;
	kfl4 = 0;  
	if (iFlowRate > 2800)	  kfl4 = 1;
	if (iFlowRate > 2900)	  kfl4 = 2;
	if (iFlowRate > 3000)	  kfl4 = 3;
	if (iFlowRate > 3100)	  kfl4 = 4;
	if (iFlowRate > 3200)	  kfl4 = 5;
	if (iFlowRate > 3300)	  kfl4 = 6;
	if (iFlowRate > 3400)	  kfl4 = 7;
	if (iFlowRate > 3500)	  kfl4 = 8;
	if (iFlowRate > 3600)	  kfl4 = 9;


	ksp = 0;
	ksp=pk1->isHeadFlated;  //add by xie 10-17

	knc = 1;        

	key_b [0]= 0;
	key_b [0] += (knc   * 1000000l);   
	key_b [0] += (kam1  *  100000l);  
	key_b [0] += (kcs1  *   10000l);   
	key_b [0] += (kpm1  *    1000l);  
	key_b [0] += (ksm1  *     100l);  
	key_b [0] += (kjm1  *      10l);   
	key_b [0] += (ksp   *       1l);   

	key_b [1]=0;
	key_b [1] += (knc   * 1000000000l);   
	key_b [1] += (ktf1  *  100000000l);  
	key_b [1] += (ktf2  *   10000000l);  
	key_b [1] += (kts1  *    1000000l); 
	key_b [1] += (kts2  *     100000l);   
	key_b [1] += (kts3  *      10000l);   
	key_b [1] += (kts4  *       1000l); 
	key_b [1] += (kcr1  *        100l);  
	key_b [1] += (kcr2  *         10l);   
	key_b [1] += (ksp   *          1l);  

	key_b [2]= 0;
	key_b [2] += (knc  * 1000000l);  
	key_b [2] += (kd1  *  100000l);  
	key_b [2] += (kd2  *   10000l); 
	key_b [2] += (kw1  *    1000l);  
	key_b [2] += (kl1  *     100l);
	key_b [2] += (kl2  *      10l); 
	key_b [2] += (ksp  *       1l);  

	key_b [3]=0;
	key_b [3] += (kwaterTemp   * 100000l);  
	key_b [3] += (knc   * 10000l);  
	key_b [3] += (knb1 *  1000l); 
	key_b [3] += (kc1   *   100l);  
	key_b [3] += (kmn1  *    10l);  
	key_b [3] += (ksp   *     1l); 

	key_b [4]=0;
	key_b [4] += (knc   * 100000l);  
	key_b [4] += (kfl1  *  10000l);
	key_b [4] += (kfl2  *   1000l);  
	key_b [4] += (kfl3  *    100l);  
	key_b [4] += (kfl4  *     10l);  
	key_b [4] += (ksp   *      1l);   

	return 0;
}


int     CSelfLearnFunction::AdaptionProcess(
	const	INPUTDATAPDI *pdiInPut,
	const	int			nStroke,		   /* No. of strokes */
	ADAPTATION *adapt,		   	   /* adaptation structure */
	unsigned long int   adap_key[5], /* Adaption_Key */
	float      *adaptRatioFound,/* Adaption_Factor */
	float   waterTemp)		   //add by xie 8-21 水温
{
	int     levelNumber;
	int     i;

	unsigned long    int     keynr1s[5], keynr2s[5];
	long	int		iAdaptTimeDiff;
	const	long int iTimeLevel2 = (long int) (pdiInPut->controlAcc.fAdaptDifTime * 3600.f);
	float   eps = .1e-6f;
	time_t	iAdaptTimeUse;

	*adaptRatioFound = 0.0f;
	for(i=0;i<5;i++)
		adap_key[i] = 0;

	KeepAdaptionKey(keynr1s,pdiInPut,waterTemp);
	levelNumber  = 1;
	for(i=0;i<5;i++)
		adap_key[i] = keynr1s[i];

	*adaptRatioFound = SuchRead(pdiInPut, keynr1s, 1, PREPROCESS, nStroke, adapt);
	time(&iAdaptTimeUse);
	iAdaptTimeDiff = (long)iAdaptTimeUse - adapt->iAdaptTimeLast;
	/* no time limit for Level 1 adaption if controlAcc.fAdaptDifTime < 0 */
	if(iTimeLevel2 < 0) 
		iAdaptTimeDiff = iTimeLevel2 - 1L; 
	if (	((*adaptRatioFound < eps) ||
		(iAdaptTimeDiff > iTimeLevel2)) &&
		(pdiInPut->controlAcc.iAdaptLevel > 1)){
		KeepAdaptionKey2(keynr2s,pdiInPut,waterTemp);
		printf("\n\t Level_2 Key::%lu  %lu  %lu  %lu  %lu  ", keynr1s[0],keynr1s[1],keynr1s[2],keynr1s[3],keynr1s[4]);

		for(i=0;i<5;i++)
			adap_key[i] = keynr2s[i];

		*adaptRatioFound = SuchRead(pdiInPut, keynr2s, 2, PREPROCESS, nStroke, adapt);
		levelNumber = 2;
	}
	if(*adaptRatioFound < eps) 
		levelNumber  = 0; /* nothing found */
	return(levelNumber);
}

/************************************************************************/
/* 函数名:AdaptationSLP_Write
/* 作  者:Zed
/* 时  间:2014/12/9
/* 功  能:自学习存储
/************************************************************************/
int CSelfLearnFunction::AdaptationSLP_Write(
	const INPUTDATAPDI *pdiInput,
	const PRERESULT *PreResult,
	const FATsegs *seg,
	SLP *adaptPara,
	int offLineFlag)
{
	float AP_before  = adaptPara->SLP_AP;
	float AP_current = seg->res[2];
	float AP_next = 1.0;
	float AP_extent = 1.0;
	FILE *fp = NULL;
//	char sLine[170];
	int Seq = 0;
	//自行添加记录变量
	float SpareF1,SpareF2;

	//自定义作测试
	//AP_current = 1.0;

	SpareF1=0.0;
	SpareF2=0.0;
	//char grade_steel[PATH_MAX];
	//strcpy_s(grade_steel,pdiInput->matID);


	// add [1/16/2015 谢谦]
	float offLineDevation=abs(seg->Tc_fct_sv.mean-seg->Tm_p4_sv.mean);   //add by xie 9-3

	AP_next = AP_before + (AP_current - 1.0) * AP_extent;
	
	if (offLineFlag)
	{
		if (offLineDevation>30)
		{
			AP_next=AP_before+(AP_current-1)*0.8;
		}
		else if (offLineDevation>15)
		{
			AP_next=AP_before+(AP_current-1)*0.5;
		}
		else
			AP_next=AP_before+(AP_current-1)*0.2;
	}
	else
	{
		if (abs(PreResult->aveTmP4-pdiInput->targetFinishCoolTemp)>30)
		{
			AP_next=AP_before+(AP_current-1)*0.8;
		}
		else if (abs(PreResult->aveTmP4-pdiInput->targetFinishCoolTemp)>15)
		{
			AP_next=AP_before+(AP_current-1)*0.5;
		}
		else
			AP_next=AP_before+(AP_current-1)*0.2;
	}
		

	logger::LogDebug("system","离线状态 %d  上块系数:%f当前系数：%f下块系数：%f计算的平均温度：%f",
		offLineFlag,AP_before,AP_current,AP_next,PreResult->aveTmP4);

	if((AP_next<0.5)||(AP_next>4))
		AP_next = 1.111;	

	//adaptPara->SLP_Seq=6;

	char szLine[100];
	char szNew[200];

	long nLineStart=0;

	if((fp = fopen("Logs\\AdaptionPara.txt","r+")) == NULL)
		logger::LogDebug("system","该文件不存在！");
	else
		//while(!feof(fp))
		while(fgets(szLine,100,fp)){
			int nSize = strlen(szLine);	//获得一下当前行字符数
			sscanf(szLine,"%d",&Seq);	//获得该行序列号

			if(adaptPara->SLP_Seq == Seq){		//判断序列号是否为替换行 23是测试值
				fseek(fp,nLineStart,SEEK_SET);	//定位到该行行首，nLineStart值是有上一次循环确定，由于fgets每次读取一行，
				//上一次循环读取的位置是上一行的结尾（换行符后），也就是本行的开始
				sprintf(szNew,"%d\t%d\t%.1f \t%.1f \t%.1f \t%.1f \t%.1f \t%.1f \t%05.3f \t%.1f \t%.1f",
					adaptPara->SLP_Seq,
					adaptPara->SLP_CM,
					adaptPara->SLP_WT,
					adaptPara->SLP_RT,
					adaptPara->SLP_T,
					adaptPara->SLP_C,
					adaptPara->SLP_CR,
					adaptPara->SLP_Nb,
					AP_next,
					SpareF1,
					SpareF2);					//获得需要替换的改行内容，值为测试使用，这里请保证与原文本长度一致
				fprintf(fp,szNew);				//覆盖到文件中

				//nSize = strlen(szNew);	//获得一下当前行字符数
				//for(int i=nSize+1; i < 100; i++)
				//	fprintf(fp," "); //空格补齐
				//fprintf(fp,"\r\n");	//最后加上换行符

				fclose(fp);		//替换完成，关闭文件
				logger::LogDebug("system","原自学习系数覆盖 序号：%d",adaptPara->SLP_Seq);
				return 1;
			}

			nLineStart = ftell(fp);				//记录读取位置，当前位置是改行的行尾，即下一行的开始。

		}


		sprintf(szNew,"%d\t%d\t%.1f \t%.1f \t%.1f \t%.1f \t%.1f \t%.1f \t%05.3f \t%.1f \t%.1f\n",
			adaptPara->SLP_Seq,
			adaptPara->SLP_CM,
			adaptPara->SLP_WT,
			adaptPara->SLP_RT,
			adaptPara->SLP_T,
			adaptPara->SLP_C,
			adaptPara->SLP_CR,
			adaptPara->SLP_Nb,
			AP_next,
			SpareF1,
			SpareF2);					//获得需要替换的改行内容，值为测试使用，这里请保证与原文本长度一致
		fprintf(fp,szNew);				//覆盖到文件中

		//int nSize = strlen(szNew);	//获得一下当前行字符数
		//for(int i=nSize+1; i < 100; i++)
		//	fprintf(fp," "); //空格补齐
		//fprintf(fp,"\r\n");	//最后加上换行符

		fclose(fp);
		logger::LogDebug("system","新自学习系数添加 序号：%d",adaptPara->SLP_Seq);
		return 0;
}

/************************************************************************/
/* 函数名:InitAdaptationSLP
/* 作  者:Zed
/* 时  间:2014/12/7
/* 功  能:初始化无级索引自学习参数
/************************************************************************/
void CSelfLearnFunction::AdaptationSLP_Init(SLP *adaptPara)
{
	adaptPara->SLP_Path = 0.000f;	//精确到0.001
	adaptPara->SLP_CM = 1;			//默认ACC模式
	adaptPara->SLP_WT = 2;
	adaptPara->SLP_RT = 2;		//目标返红取整数
	adaptPara->SLP_T = 2;
	adaptPara->SLP_C = 2;
	adaptPara->SLP_CR = 2;
	adaptPara->SLP_Nb = 2;
	adaptPara->SLP_AP = 2;		//精确到0.001
}
/************************************************************************/
/* 函数名:AdaptionSLP_Get
/* 作  者:Zed
/* 时  间:2014/12/8
/* 功  能:自学习关键字确定
/************************************************************************/
int CSelfLearnFunction::AdaptionSLP_Get(const INPUTDATAPDI *pdiInput, SLP *adaptPara)
{
	float	SLP_Key[10]		= {	1,2,3,4,5,6,7,8,9,10};
	float	SLP_KeyB[5]	= {	1,2,3,4,5};
	float	SLP_KeyC[20]		= {	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

	float	WaterTemp[5]	= {	  10,   20,   30,   40,   50};
	float	RedTemp[10]		= {	 300,  350,  400,  450,  500,  550,  600,  650,  700,  750};
	float	Thickness[20]	= {	 10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,170,190,210,230,250};
	float	AlloyC[5]		= {	  0.1,  0.3,  0.5, 1.0,  2.0};
	float	AlloyNb[5]		= {	  0,   0.03, 0.09, 0.15, 0.3};
	float	coolingRate[5]		= {	5,20,35,50,65  };


	int nWT = sizeof(WaterTemp)/sizeof(WaterTemp[0]);
	int nRT = sizeof(RedTemp)/sizeof(RedTemp[0]);
	int nTck= sizeof(Thickness)/sizeof(Thickness[0]);
	int nAC = sizeof(AlloyC)/sizeof(AlloyC[0]);
	int nCr= sizeof(coolingRate)/sizeof(coolingRate[0]);
	int nANb= sizeof(AlloyNb)/sizeof(AlloyNb[0]);

	adaptPara->SLP_CM = pdiInput->acc_mode;
	adaptPara->SLP_WT = GetLinearInterpolationFromAbscissa(WaterTemp,SLP_KeyB,nWT,pdiInput->waterTemperature);
	adaptPara->SLP_RT = GetLinearInterpolationFromAbscissa(RedTemp,SLP_Key,nRT,pdiInput->targetFinishCoolTemp);
	adaptPara->SLP_T  = GetLinearInterpolationFromAbscissa(Thickness,SLP_KeyC,nTck,pdiInput->thick*1000);
	adaptPara->SLP_C  = GetLinearInterpolationFromAbscissa(AlloyC,SLP_KeyB,nAC,pdiInput->alloy_c);
	//adaptPara->SLP_CR = GetLinearInterpolationFromAbscissa(coolingRate,SLP_KeyB,nCr,pdiInput->alloy_mn);
	adaptPara->SLP_CR=0;
	adaptPara->SLP_Nb = GetLinearInterpolationFromAbscissa(AlloyNb,SLP_KeyB,nANb,pdiInput->alloy_nb);
	

	return 0;
}
/************************************************************************/
/* 函数名:AdaptationSLP_Read
/* 作  者:Zed
/* 时  间:2014/12/8
/* 功  能:自学习无级索引过程
/************************************************************************/
int CSelfLearnFunction::AdaptationSLP_Read(const INPUTDATAPDI *pdiInput, SLP *adaptPara)
{
	FILE *AP_log = NULL;
	char sLine[100];
	float AP[7]={};
	float AP_Path[3]={25.0,25.0,25.0};	//[0]最小,[2]最大[2014/12/8 by Zed]
	float AP_AP[3]={1.0,1.0,1.0};
	int AP_CM = 1;
	int Seq = 0;
	AdaptionSLP_Get(pdiInput,adaptPara);
	memset(sLine,0,100);
	if ((AP_log = fopen("Logs\\AdaptionPara.txt","r")) == NULL)
		logger::LogDebug("system","自学习日志文件不存在！");
	else
	{
		while(!feof(AP_log)){
			fgets(sLine,sizeof(sLine),AP_log);
			sscanf(sLine,"%d %d %f %f %f %f %f %f %f",&Seq,&AP_CM,
					&AP[0],&AP[1],&AP[2],&AP[3],&AP[4],&AP[5],&AP[6]);
			if(adaptPara->SLP_CM==AP_CM)
			{
				float p1 = (adaptPara->SLP_WT - AP[0])*(adaptPara->SLP_WT - AP[0]);
				float p2 = (adaptPara->SLP_RT - AP[1])*(adaptPara->SLP_RT - AP[1]);
				float p3 = (adaptPara->SLP_T - AP[2])*(adaptPara->SLP_T - AP[2]);
				float p4 = (adaptPara->SLP_C - AP[3])*(adaptPara->SLP_C - AP[3]);
				float p5 = (adaptPara->SLP_CR - AP[4])*(adaptPara->SLP_CR - AP[4]);
				float p6 = (adaptPara->SLP_Nb - AP[5])*(adaptPara->SLP_Nb - AP[5]);

				adaptPara->SLP_Path = sqrt(p1+p2+p3+p4+p5+p6);
				adaptPara->SLP_Path = int(adaptPara->SLP_Path * 1000);
				adaptPara->SLP_Path = adaptPara->SLP_Path/1000;

				if(adaptPara->SLP_Path<0.1){
					adaptPara->SLP_AP = AP[6];
					adaptPara->SLP_Seq = Seq;
					fclose(AP_log);
					return 1;
				}
				else
				{
					if(adaptPara->SLP_Path<AP_Path[0]){
						AP_Path[2] = AP_Path[1];
						AP_Path[1] = AP_Path[0];
						AP_Path[0] = adaptPara->SLP_Path;
						AP_AP[0]   = AP[6];
					}
					else if(adaptPara->SLP_Path<AP_Path[1]){
						AP_Path[2] = AP_Path[1];
						AP_Path[1] = adaptPara->SLP_Path;
						AP_AP[1]   = AP[6];
					}
					else if(adaptPara->SLP_Path<AP_Path[2]){
						AP_Path[2] = adaptPara->SLP_Path;
						AP_AP[2]   = AP[6];
					}
				}
			}
		}
		adaptPara->SLP_AP = (AP_Path[1]*AP_Path[2]*AP_AP[0]+AP_Path[0]*AP_Path[2]*AP_AP[1]+AP_Path[0]*AP_Path[1]*AP_AP[2])/
							(AP_Path[0]*AP_Path[1]+AP_Path[1]*AP_Path[2]+AP_Path[0]*AP_Path[2]);
		adaptPara->SLP_Seq = Seq+1;
	}
	fclose(AP_log);
	return 0;
}


#undef MESSAGE_MAX_LENGHT