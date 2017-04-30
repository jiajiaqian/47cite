/*! 
* Copyright (c) 2013, 东北大学轧制技术及连轧自动化国家重点实验室(RAL)
* All rights reserved
*
* FILE PostCal.h
* 说明 概述  
*  
* 详细说明
*  
* 版本 1.0 作者 Hanbing 日期 7/25/2013
* 
*/
#pragma once
#include "../BaseCoolStruct.h"
#include "SelfLearnFunction.h"
#include "Mathematical.h"
#include "public.H"
#include "SignalFormat.h"
#include "PostCaculateCore.h"
////////////////////////////////结构体定义////////////////////////////
#define  Tzero(D)   ((float)D->data_word[2]+.01*D->data_word[3]);

class  CPostProcess {

private:

	//build 文件名 for output of online
	int MakeOnlineResultFileName(
		const char *plate_id,
		const PRERESULT *PreResult,
		char *fn);

	/* x.2) check 精度 of material tracking       */
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

	// x.2.1) report check 精度 of tracking
	int	ReportTracking(
		const	int		iBls1,
		const	char	*sNameBls1,
		const	int		iBls2,
		const	char	*sNameBls2,
		const	int		iLimit1,	/* 段超过 1% 精度 */
		const	int		iLimit2,	/* 段超过 2% 精度 */
		const	int		iLimit5,	/* 段超过 5% 精度 */
		const	int		iLimitStop,	/*板材停止位置*/
		const	int		iLimitAdapt,	/* 段超过 Adapt 精度 */
		const	float	fTimeHeadRef, /* 需要运行time 头部 */
		const	float	fTimeHeadAct, /* 运行time 头部速度 integrated */
		const	float	fTimeTailRef, /* 需要运行time 尾部 */
		const	float	fTimeTailAct, /* 运行time 头部速度 integrated */
		const	float	fSpdDevMax,   /* max. 速度偏差适应范围 */
		const	float	fSpdDevPos,	  /* 相对位置 */
		const	float	fSpdDevMean,  /* mean 速度偏差适应范围 */
		const	RALMESDATA *mes,
		FILE	*erg_f);	 /* 结果FILE */

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