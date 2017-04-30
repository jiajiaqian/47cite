/*! 
* Copyright (c) 2013, 东北大学轧制技术及连轧自动化国家重点实验室(RAL)
* All rights reserved
*
* FILE PostCaculateCore.h
* 说明 概述  
*  
* 详细说明
*  
* 版本 1.0 作者 Hanbing 日期 7/25/2013
* 
*/
#pragma once
#include "../BaseCoolStruct.h"
#include "SignalFormat.h"
#include "SelfLearnFunction.h"
#include "TemperatureField.h"

class CPostCaculateCore
{
public:
	CPostCaculateCore(void);
	~CPostCaculateCore(void);
public:
	
	int PostCoreProcessing
		(FILE *erg_f,
		FILE *st_err,
		const       int     isimula, 
		RALOUTDATA *pre_out,
		const       RALINDATA *ralInPut,
		const       RALCOEFFDATA *lay,
		RALMESDATA *mes,
		dbk_t   data_block[]);

	int	OutputMeasuredResult(
		FILE	*erg_f,
		int		ioutput_flow,
		RALMESDATA *mes,
		RALCOEFFDATA *lay,
		RALOUTDATA *pre_out);

private:

	int	CheckTemperature(
		const	float	temp_p1,
		const	float	temp_p2,
		const	float	temp_p12_min,
		const	float	temp_p5,
		const	float	temp_p5_min);

	int	InitMeasurements(
		FILE	*erg_f,
		int		ioutput_flow,
		RALMESDATA *mes,
		RALCOEFFDATA *lay,
		RALOUTDATA *pre_out);

	int CopyInfoInSSAB(           
		const	int	netseg, /* No. of segments passed */         
		FATsegs	*Seg_sv,	/* statistical value	*/
		RALMESDATA *mes);

	int	RangeReplace(
		const	int			nData,		/* complete data trace */
		const	int			nMax,		/* 最大 No. of report data */
		const	float		fEpsMin,	/* 低 limit */
		float		*fVal);		/* I/O: value */

	int	prn_add2(
		FILE	*erg_f,
		const RALMESDATA *mes);

	int AnalysisItems(
		FILE *erg_f,
		const	int	netseg, /* No. of segments passed */
		const   RALINDATA *ralInPut,
		const   RALCOEFFDATA *lay,
		const	RALOUTDATA *pre_out,
		FATsegs	*Seg_sv,	/* statistical value	*/
		const   RALMESDATA *mes);
	
	int     AnalysisPyrometers(
		const   int     n,      /* 序号value   */
		const   float   min_val, /*最小value  */
		int   *val1,  /* first value     */
		int   *val2,  /* second value    */
		const   float   *timex, /*time 信息 */
		const   float   *valex); /*value 信息 */

	float   AnalysisFlow(
		const   int     n,      /* 序号value   */
		const   float   min_val, /*最小value  */
		const   float   *f_top, /* top flow */
		const   float   *f_bot);

	int	CalculateStatisticalInfo(
		const		INPUTDATAPDI *pdiInPut,
		FATsegs		*Seg_sv,	/* statistical value	*/
		RALMESDATA *mes);

	int	GetFirstAndLastPos(
		int	*i_first, /* O: first index > eps  */
		int	*i_last,  /* O: last  index > eps  */
		const	int	n,	  /* No. of elements array */
		const	float	eps,	  /* 最小 accepted value */
		const	float	*x);

	int	EstimateTemperatureProfileStatisticalValues(
		s_val_t	*s,
		const	int	i_first, 
		const	int	i_last, 
		const	float	*x);

	INPUTDATAPDI InitPostPDI(        FILE	*st_err,
		const  int     isimula,
		PRERESULT *PreResult,
		dbk_t 	data_block[],
		const char* plate_id);

	int	CalculateStartSegmentInfo(		int		    *tolseg,
		const	int			iStatusPyro,
		const	float		dtemp_max,
		const   float		target_temp,
		const	dbk_t 		*db_temp);

	int	CalculateStartDistributionOfTemperature(	PHYSICAL *ver_par1,
		COMPUTATIONAL *compx,
		INPUTDATAPDI *pdiInPut,
		FILE		*st_err,
		FILE		*erg_f,
		float		*temp_diffx);

	int	CheckDataPlausibility(		PHYSICAL *ver_par1,
		COMPUTATIONAL *compx,
		INPUTDATAPDI *pdiInPut,
		FILE		*st_err,
		FILE		*erg_f,
		float		*temp_diffx,
		const		TiTemp		Seg);

	int	SetWaterCoolingParam(		PHYSICAL *ver_par1,
		INPUTDATAPDI *pdiInPut,
		const	PRERESULT *PreResult,
		const	dbk_t		*DB1);

	int	CalculateWaterCooling(		PHYSICAL *ver_par1,
		COMPUTATIONAL *compx,
		INPUTDATAPDI *pdiInPut,
		FILE		*st_err,
		FILE		*erg_f,
		const	TiTemp		Seg);

	int	CalculateSegmentData(	const		COMPUTATIONAL *compx,TiTemp		*Seg);

	int	CalculateAirCoolMeasurePos(	PHYSICAL *ver_par1,
		COMPUTATIONAL *compx,
		INPUTDATAPDI *pdiInPut,
		FILE		*st_err,
		FILE		*erg_f,
		const	TiTemp		Seg);

	int	EstimateAdaptonValues(	const		COMPUTATIONAL *compx,
		const		int			is,
		const		INPUTDATAPDI *pdiInPut,
		TiTemp		*Seg);


	int	OutputTableHeader(	FILE *SEGdat, 
		int	isimula, 
		int is_start, 
		int tolseg);


	int	OutputEachSegment(  
		FILE   *SEGdat,
		const  int    isimula,
		const  int    iMode, 
		const  int    netseg,
		const  TiTemp Seg,
		const  int    iSeg,        /* 实际 segment No. */
		const  float  fLengthSeg);  /* segment length */
	

	int	SelectPreferredMeasuringPoint(
		const	PRERESULT *PreResult,
		const	float	fTimeP4,
		const	int		iTempP4,
		const	float	fTimeP5,
		const	int		iTempP5,
		float	*fTemp);

	float	 dxx(	
		const float	temp,
		const float	thick,
		const float	timex);

	int	ValidateTemprature(
		const	int		iStartP1,	/* start index P1 Entry */
		const	int		iStartP2,	/* start index P2 Entry */
		const	int		iStartP4,	/* start index P4 Exit  */
		const	int		iStartP5,	/* start index P5 Exit  */
		const	int		nTolP1,	    /* tolerable segments P1 */
		const	int		nTolP2,	    /* tolerable segments P2 */
		const	int		nTolP4,	    /* tolerable segments P4 */
		const	int		nTolP5,	    /* tolerable segments P5 */
		int		*iStart);	/* start index */

	int	ReportUnpostTemperature(
		FILE	*erg_f,
		const	int		nTolP1,	    /* tolerable segments P1 */
		const	int		nTolP2,	    /* tolerable segments P2 */
		const	int		nTolP4,	    /* tolerable segments P4 */
		const	int		nTolP5);    /* tolerable segments P5 */
	
	private:
		int InitControlValues(const MODELCONTROL *x, MODELCONTROL *y);

		FATsegs	InitStatisticalData(int *iret);
		

		s_val_t	InitSVal(	const	int inr);

		int	ResetSVal(s_val_t	*s,	/*	statistical value	*/
			const	int		iv,	/* = 1: first call		*/
			const	float		val,	/*	value		*/
			const	int		ime);	/* method of calcul.		*/

		int	SetSVal(	s_val_t	*s,	/*	statistical value	*/
			const	int		iv);	/* = 0: first call		*/

		int CombinFileName(const char *plate_id,
			const int  taskID,
			char *fn);


};

static CPostCaculateCore m_PostCaculateCore;