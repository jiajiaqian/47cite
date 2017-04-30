/*! 
* Copyright (c) 2013, ������ѧ���Ƽ����������Զ��������ص�ʵ����(RAL)
* All rights reserved
*
* FILE PostCal.h
* ˵�� ����  
*  
* ��ϸ˵��
*  
* �汾 1.0 ���� Hanbing ���� 7/25/2013
* 
*/
#pragma once
#include "../BaseCoolStruct.h"
#include "SelfLearnFunction.h"
#include "Mathematical.h"
#include "public.H"
#include "SignalFormat.h"
#include "PostCaculateCore.h"
////////////////////////////////�ṹ�嶨��////////////////////////////
#define  Tzero(D)   ((float)D->data_word[2]+.01*D->data_word[3]);

class  CPostProcess {

private:

	//build �ļ��� for output of online
	int MakeOnlineResultFileName(
		const char *plate_id,
		const PRERESULT *PreResult,
		char *fn);

	/* x.2) check ���� of material tracking       */
	int CheckAccuracy(
		const	INPUTDATAPDI *pdi,
		const	RALCOEFFDATA *lay,
		RALMESDATA *mes,
		const	RALOUTDATA *pre_out,
		const	dbk_t							data_block[],
		FILE						   *st_err,
		FILE						   *erg_f);


	int InitDataBlock(dbk_t    data_block[]);

	int	SetPyrometers(const  RALCOEFFDATA *lay);

	// x.2.1) report check ���� of tracking
	int	ReportTracking(
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
		FILE	*erg_f);	 /* ���FILE */

public:
	// TODO: add your methods here.

	pwr_tInt32 PostProcessing(    
		RALINDATA *ralInPut,
		RALCOEFFDATA *lay,
		RALMESDATA *mes,
		RALOUTDATA *pre_out,
		pwr_tInt32                       proc_id,
	struct T_AccSysPar				*SysPar,
	struct T_AccPattern				*AccPattern);
	CPostProcess();
	~CPostProcess();
};

static CPostProcess gPostprocess;