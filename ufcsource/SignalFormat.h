#pragma once
#include "../BaseCoolStruct.h"
#include "SelfLearnFunction.h"
#include "Mathematical.h"
#include <float.h>

#define		L_SEG		0.25f	/* sample length */

#define		SEGANZ		23	/* segments total for PostCalc (> 10) */
#define		NSEGSEND	10  /* reported segments */
#define		SEGZX		1	/* segments on 头部 and 尾部 end not used for statistical calculations change from 2 to 0 by xie*/

#define		SEGMIN		 4	/* 最小No. of segments for adaption */

/* 1) define Temperature window */
/* 定义 Temperature for filter purpose and reliability */
#define		TGFIMIN		650 /* 最小完成 rolling Temperature */
#define		T_MIN_P4	 50	/*	150 UP TO 500	*/
#define		T_MIN_P5	 300	/*	350	  900	*/
#define     T_MIN_PUx	 50	/*	350	  900	*/
//#define		T_STOP_MIN	200	/*	minimal stop	Temperature */
#define		T_STOP_MIN	105	/*	minimal stop	Temperature  */
/* 定义 error windows Temperature */
#define		D_TEMP_P1	200.f	/*	allowed Temperature	偏差 P1 / P2  */
/////////////// change by wbxang 20100629 //////////////////////////////////
#define		D_TEMP_P5	400.f	/*	allowed Temperature	偏差 P3 to P5 */
#define		D_TEMP_PUx	400.f	/*	allowed Temperature	偏差 P3 to P5 */

#define		EPS_P_ENTRY_SPAN 25.f /* precondition entry Temperature span */
#define		EPS_P_ENTRY_PDI  35.f /* precondition entry Temperature dev. max */
#define		EPS_P_EXIT_SPAN  70.f /* allowed Temperature span exit */
/* filter: 平滑ing parameter Entry, Exit Temperature 0 < 平滑 < 1 */
/* 1: complete 平滑ing to 平均value 0: no 平滑ing */
#define		TSmoothENTRY 0.3f /* smothing Parameter */
#define		TSmoothEXIT 0.3f /* smothing Parameter */

/* 2) define material tracking window */
/* define allowed max 速度 偏差 (no adaption if exceeded) */
#define		D_SPD_ADAPT_MAX		30.0f /* allowed max 速度 偏差 +/-[%] */
/* define allowed mean 速度 偏差 (no adaption if exceeded) */
#define		D_SPD_ADAPT_MEAN	0.30f /* allowed mean 速度 偏差 +/-[m/sec] */
/* D_SPD_ADAPT* must be valid from position: EntryACC + D_POS_ADAPT */
 
#define		D_POS_ADAPT 6.1f /* reduce position window by: [m] */

/* data_struct for the 13 post in DATA */
typedef struct	data_bl
{
	int data_word[N];
}dbk_t;

#define OFFSET_SEG      20      /* offset of segments           */

#define IMIN_AD 2	   /* 最小number for adaption */

#define		X_DIRECTORY	"./post_dat/"
#define		WATER_DIR	"./pre_dat/"

class CSignalFormat
{
public:
	CSignalFormat(void);
	~CSignalFormat(void);

	int PostDataFormat(const    INPUTDATAPDI *pdi,	const    RALCOEFFDATA *lay,	RALMESDATA *mes,RALOUTDATA *pre_out,dbk_t   data_block[],FILE    *st_err, FILE	 *erg_f);
	int	StatisticalValueResult(PRERESULT *PreResult, FATsegs *xx, const	INPUTDATAPDI *pdi, FILE		*erg_f,  const	int			icall, int offLineSelfLearn);
	int	CalculateStatisticalValueForReport(const	INPUTDATAPDI *pdi, 	FATsegs *xx,const	TiTemp  Seg,	const int		icall);

	/* x.3) check 精度 of 头部 and 尾部 遮蔽   */
	int CheckHeadTailMask( 	const	INPUTDATAPDI *pdi,	const	RALCOEFFDATA *lay,	RALMESDATA *mes,const	RALOUTDATA *pre_out,const	dbk_t	data_block[],FATsegs *xx,		/* Statistical value  */
								FILE	*st_err,	FILE  *erg_f);	
	int	LeastSquareFitData(	const	int		n,	const	float	*x,	const	float	*y,	const	int		nPolyTar,float	*yMod);	/* modified data */
private:
	
	float   AnalysePostion(
		const   int     nmax, /* 最大 possible number of value */
		const   float   pos,  /* 参考 Position or time for value */
		const   float   *x,   /* array with position or times */
		const   float   *y);   /* array with value over position or times */

	float ActiveCoolingLength(
		const    PRERESULT *PreResult,      /* PREPROCESS telegram */
		const    float   entry_acc,  /* position ENTRY_ACC */
		const    float   exit_max_acc); /* end position last header */


	int	GetSignalIndex(
		const	int		n,			/* 序号value */
		const	int		iSearch,	/* start index for search */
		const	float	fLimit,		/* 限制value*/
		const	float	*fSignal);	/* Signal [n] */

	int	CheckTimeIncrements(
		const	int		n,			/* 序号value */
		const	float	fEpsTime,	/* required time increment between time steps */
		const	float	*fTimeVal);	/* time value */
	
	int	GetMinIndex(
		const	int		n,
		const	float	fEps,
		const	float	fMax,
		const	float	*fVal);

	int InitFloatArray(const   int n, float   *x);

    int	InitMES(RALMESDATA *mes);


	   float   OffsetBls1(
		   const   int		nmax,		/* 最大 number of value */
		   const   float   min_val,	/* 最小 allowed measured value */
		   const   float   syn,		/* position of synchronization point */
		   const   float   *x,			/* position value */
		   float   *y);			/* corresponding value vs. position */

	   int	ForceSignalToDefaultInside(
		   const	float	fDef,	/* 默认value */
		   const	float	xa,		/* 间隔 [xa,xb] */
		   const	float	xb,		/* 间隔 [xa,xb] */
		   const	int		n,		/* No of value */
		   const	int		iStart,	/* 开始搜索位置 */
		   const	int		iEnde,	/* 结束搜索位置 */
		   const	float	*x,		/* 横坐标 value */
		   float	*y);		/* 纵坐标value */

	   int	ForceSignalToDefaultOutside(
		   const	float	fDef,	/* 默认value */
		   const	float	xa,		/* 间隔 [xa,xb] */
		   const	float	xb,		/* 间隔 [xa,xb] */
		   const	int		n,		/* No of value */
		   const	int		iStart,	/* 开始搜索位置 */
		   const	int		iEnde,	/* 结束搜索位置 */
		   const	float	*x,		/* 横坐标 value */
		   float	*y);		/* 纵坐标value */

	   int TransformTelegramTime(
		   const   int     id,     /* id of the telegram */
		   const   int     nx,     /* No. of time value */
		   const   float   *tx,    /* time in seconds */
		   int *ti);    /* integer format */

	   int TransformTelegramTemperature(
		   const   int     id,     /* id of the telegram */
		   const   int     nx,     /* No. of time value */
		   const   float   *tx,    /* temp in C */
		   int *ti);    /* integer format */
	
	   //新的功能
	   private:

		   float	CoolingLengthRatio( const	PRERESULT *PreResult);
	
		   int	CalculateStatisticalValue(	s_val_t	*s,	  const	int		iv);

		   int	PrintStatisticalResult(FATsegs *xx,    FILE		*erg_f);

		   int CalculateAdaptionValues(	FATsegs	*xx,   const	INPUTDATAPDI *pdi,   FILE	*erg_f,	   int offLineselfLearn); // add by xie 8-21 


		   int	StatisticalValue(s_val_t	*s,	/*	statistical value	*/
			   const	int		iv,	/* = 1: first call		*/
			   const	float		val,	/*	value		*/
			   const	int		ime);	/* method of calcul.		*/

		   int	ModifySignalFilterLow2Peak(
			   const	float	fValTarget,		/* 目标value */
			   const	float	fValMin,		/* 最小允许value */
			   const	int		n,				/* 序号value */
			   const	float	*x,				/* 横坐标 value */
			   float	*y)		;		/* 纵坐标value I/O */

		   int	GetSignalFilterLow2Peak(
			   const	float	fValTarget,		/* 目标value */
			   const	float	fValMin,		/* 最小允许value */
			   const	int		n,				/* 序号value */
			   const	float	*x,				/* 横坐标 value */
			   const	float	*y,				/* 纵坐标value */
			   float	*yx);			/*滤波信号*/

		   int	ModifySignalFilterMeanStandard(
			   const	float	fValTarget,		/* 目标value */
			   const	float	fValDev,		/* allowed 偏差 value */
			   const	float	fRed,			/* 允许减少 */
			   const	int		n,				/* 序号value */
			   const	float	*x,				/* 横坐标 value */
			   const	float	*y,				/* 纵坐标value */
			   float	*yx);			/* 滤波后的信号 */

		   int	ModifySignalFilterSmoothMean(
			   const	float	fValMean,       /* 平均value signal y */
			   const	float	fValStd,        /* 标准差 signal y */
			   const	float	fRed,			/* 减少value Std */
			   const	int		n,				/* 序号value */
			   const	float	*x,				/* 横坐标 value */
			   const	float	*y,				/* 纵坐标value */
			   float	*yx)	;		/*滤波信号*/

		   int	GetSignalFilterLowInteger(
			   const	float	fValTarget,		/* 目标value */
			   const	float	fValMin,		/* 最小允许value */
			   const	int		iDrop,			/* 指数下降信号*/
			   const	int		iRise,			/* 指数上升信号 */
			   const	int		n,				/* 序号value */
			   const	float	*x,				/* 横坐标 value */
			   const	float	*y,				/* 纵坐标value */
			   float	*yx);			/* 滤波后的信号 */

		   int	SignalDrop(
			   const	int		n,			/* 序号value */
			   const	int		iSearch,	/* start index for search */
			   const	float	fLimit,		/* 限制value*/
			   const	float	*fSignal);	/* Signal [n] */

		   int	PrintStatisticalResultSecondRow(FATsegs *xx,       /* Struktur     */
			   FILE		*erg_f);

		   int	CalculateStatisticalValue(
			   const	int		n,		/* 序号value */
			   const	float	*y,		/* data 纵坐标*/
			   float	*fMean, /* 平均value */
			   float	*fStd,	/* 标准差 */
			   float	*fMin,	/*最小value */
			   float	*fMax);	/* 最大value */

		   //new add

		   int	LeastSquareFitPolynomial(
			   const	int		n,			/* 序号value */
			   const	float	*x,			/* data 横坐标 */
			   const	float	*y,			/* data 纵坐标*/
			   const	int		nPolyTar,	/* target polynomial Order */
			   float	*fCoef);	/* polynomial Coefficents */

		   int	GetOrthogonalMatrix(
			   const	int		nRow_ax,	/* No. of rows ax */
			   const	int		nCol_ax,	/* No. of columns ax */
			   const	double	*ax,		/* matrix ax */
			   const	int		nRow_bx,	/* No. of rows bx */
			   const	int		nCol_bx,	/* No. of columns bx */
			   const	double  *bx,		/* matrix bx */
			   double	*axTbx);		/* result */

		   int GetPositiveDefiniteMatrix(
			   const	int		n,				/* dimension a (nXn) */
			   const	int		nb,				/* No. columns b */
			   const	double	*a,				/* matrix value */
			   const	double	*b,				/* matrix value */
			   double	*x);				/* solution */

		   int	GetPositiveDefiniteMatrixEx1(
			   const	int		nb,				/* No. columns b */
			   const	double	fEps,			/* error value */
			   const	double	*a,				/* matrix value */
			   const	double	*b,				/* matrix value */
			   double	*x);				/* solution */

		   int	GetPositiveDefiniteMatrixEx2(
			   const	int		nb,				/* No. columns b */
			   const	double	fEps,			/* error value */
			   const	double	*a,				/* matrix value */
			   const	double	*b,				/* matrix value */
			   double	*x);				/* solution */

		   int	GetPositiveDefiniteMatrixEx3(
			   const	int		nb,				/* No. columns b */
			   const	double	fEps,			/* error value */
			   const	double	*a,				/* matrix value */
			   const	double	*b,				/* matrix value */
			   double	*x);				/* solution */

	};
static CSignalFormat gSignalFormat;