#pragma once
#include "../BaseCoolStruct.h"
#include "SelfLearnFunction.h"
#include "Mathematical.h"
#include <float.h>

#define		L_SEG		0.25f	/* sample length */

#define		SEGANZ		23	/* segments total for PostCalc (> 10) */
#define		NSEGSEND	10  /* reported segments */
#define		SEGZX		1	/* segments on ͷ�� and β�� end not used for statistical calculations change from 2 to 0 by xie*/

#define		SEGMIN		 4	/* ��СNo. of segments for adaption */

/* 1) define Temperature window */
/* ���� Temperature for filter purpose and reliability */
#define		TGFIMIN		650 /* ��С��� rolling Temperature */
#define		T_MIN_P4	 50	/*	150 UP TO 500	*/
#define		T_MIN_P5	 300	/*	350	  900	*/
#define     T_MIN_PUx	 50	/*	350	  900	*/
//#define		T_STOP_MIN	200	/*	minimal stop	Temperature */
#define		T_STOP_MIN	105	/*	minimal stop	Temperature  */
/* ���� error windows Temperature */
#define		D_TEMP_P1	200.f	/*	allowed Temperature	ƫ�� P1 / P2  */
/////////////// change by wbxang 20100629 //////////////////////////////////
#define		D_TEMP_P5	400.f	/*	allowed Temperature	ƫ�� P3 to P5 */
#define		D_TEMP_PUx	400.f	/*	allowed Temperature	ƫ�� P3 to P5 */

#define		EPS_P_ENTRY_SPAN 25.f /* precondition entry Temperature span */
#define		EPS_P_ENTRY_PDI  35.f /* precondition entry Temperature dev. max */
#define		EPS_P_EXIT_SPAN  70.f /* allowed Temperature span exit */
/* filter: ƽ��ing parameter Entry, Exit Temperature 0 < ƽ�� < 1 */
/* 1: complete ƽ��ing to ƽ��value 0: no ƽ��ing */
#define		TSmoothENTRY 0.3f /* smothing Parameter */
#define		TSmoothEXIT 0.3f /* smothing Parameter */

/* 2) define material tracking window */
/* define allowed max �ٶ� ƫ�� (no adaption if exceeded) */
#define		D_SPD_ADAPT_MAX		30.0f /* allowed max �ٶ� ƫ�� +/-[%] */
/* define allowed mean �ٶ� ƫ�� (no adaption if exceeded) */
#define		D_SPD_ADAPT_MEAN	0.30f /* allowed mean �ٶ� ƫ�� +/-[m/sec] */
/* D_SPD_ADAPT* must be valid from position: EntryACC + D_POS_ADAPT */
 
#define		D_POS_ADAPT 6.1f /* reduce position window by: [m] */

/* data_struct for the 13 post in DATA */
typedef struct	data_bl
{
	int data_word[N];
}dbk_t;

#define OFFSET_SEG      20      /* offset of segments           */

#define IMIN_AD 2	   /* ��Сnumber for adaption */

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

	/* x.3) check ���� of ͷ�� and β�� �ڱ�   */
	int CheckHeadTailMask( 	const	INPUTDATAPDI *pdi,	const	RALCOEFFDATA *lay,	RALMESDATA *mes,const	RALOUTDATA *pre_out,const	dbk_t	data_block[],FATsegs *xx,		/* Statistical value  */
								FILE	*st_err,	FILE  *erg_f);	
	int	LeastSquareFitData(	const	int		n,	const	float	*x,	const	float	*y,	const	int		nPolyTar,float	*yMod);	/* modified data */
private:
	
	float   AnalysePostion(
		const   int     nmax, /* ��� possible number of value */
		const   float   pos,  /* �ο� Position or time for value */
		const   float   *x,   /* array with position or times */
		const   float   *y);   /* array with value over position or times */

	float ActiveCoolingLength(
		const    PRERESULT *PreResult,      /* PREPROCESS telegram */
		const    float   entry_acc,  /* position ENTRY_ACC */
		const    float   exit_max_acc); /* end position last header */


	int	GetSignalIndex(
		const	int		n,			/* ���value */
		const	int		iSearch,	/* start index for search */
		const	float	fLimit,		/* ����value*/
		const	float	*fSignal);	/* Signal [n] */

	int	CheckTimeIncrements(
		const	int		n,			/* ���value */
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
		   const   int		nmax,		/* ��� number of value */
		   const   float   min_val,	/* ��С allowed measured value */
		   const   float   syn,		/* position of synchronization point */
		   const   float   *x,			/* position value */
		   float   *y);			/* corresponding value vs. position */

	   int	ForceSignalToDefaultInside(
		   const	float	fDef,	/* Ĭ��value */
		   const	float	xa,		/* ��� [xa,xb] */
		   const	float	xb,		/* ��� [xa,xb] */
		   const	int		n,		/* No of value */
		   const	int		iStart,	/* ��ʼ����λ�� */
		   const	int		iEnde,	/* ��������λ�� */
		   const	float	*x,		/* ������ value */
		   float	*y);		/* ������value */

	   int	ForceSignalToDefaultOutside(
		   const	float	fDef,	/* Ĭ��value */
		   const	float	xa,		/* ��� [xa,xb] */
		   const	float	xb,		/* ��� [xa,xb] */
		   const	int		n,		/* No of value */
		   const	int		iStart,	/* ��ʼ����λ�� */
		   const	int		iEnde,	/* ��������λ�� */
		   const	float	*x,		/* ������ value */
		   float	*y);		/* ������value */

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
	
	   //�µĹ���
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
			   const	float	fValTarget,		/* Ŀ��value */
			   const	float	fValMin,		/* ��С����value */
			   const	int		n,				/* ���value */
			   const	float	*x,				/* ������ value */
			   float	*y)		;		/* ������value I/O */

		   int	GetSignalFilterLow2Peak(
			   const	float	fValTarget,		/* Ŀ��value */
			   const	float	fValMin,		/* ��С����value */
			   const	int		n,				/* ���value */
			   const	float	*x,				/* ������ value */
			   const	float	*y,				/* ������value */
			   float	*yx);			/*�˲��ź�*/

		   int	ModifySignalFilterMeanStandard(
			   const	float	fValTarget,		/* Ŀ��value */
			   const	float	fValDev,		/* allowed ƫ�� value */
			   const	float	fRed,			/* ������� */
			   const	int		n,				/* ���value */
			   const	float	*x,				/* ������ value */
			   const	float	*y,				/* ������value */
			   float	*yx);			/* �˲�����ź� */

		   int	ModifySignalFilterSmoothMean(
			   const	float	fValMean,       /* ƽ��value signal y */
			   const	float	fValStd,        /* ��׼�� signal y */
			   const	float	fRed,			/* ����value Std */
			   const	int		n,				/* ���value */
			   const	float	*x,				/* ������ value */
			   const	float	*y,				/* ������value */
			   float	*yx)	;		/*�˲��ź�*/

		   int	GetSignalFilterLowInteger(
			   const	float	fValTarget,		/* Ŀ��value */
			   const	float	fValMin,		/* ��С����value */
			   const	int		iDrop,			/* ָ���½��ź�*/
			   const	int		iRise,			/* ָ�������ź� */
			   const	int		n,				/* ���value */
			   const	float	*x,				/* ������ value */
			   const	float	*y,				/* ������value */
			   float	*yx);			/* �˲�����ź� */

		   int	SignalDrop(
			   const	int		n,			/* ���value */
			   const	int		iSearch,	/* start index for search */
			   const	float	fLimit,		/* ����value*/
			   const	float	*fSignal);	/* Signal [n] */

		   int	PrintStatisticalResultSecondRow(FATsegs *xx,       /* Struktur     */
			   FILE		*erg_f);

		   int	CalculateStatisticalValue(
			   const	int		n,		/* ���value */
			   const	float	*y,		/* data ������*/
			   float	*fMean, /* ƽ��value */
			   float	*fStd,	/* ��׼�� */
			   float	*fMin,	/*��Сvalue */
			   float	*fMax);	/* ���value */

		   //new add

		   int	LeastSquareFitPolynomial(
			   const	int		n,			/* ���value */
			   const	float	*x,			/* data ������ */
			   const	float	*y,			/* data ������*/
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