#pragma once
#include "../BaseCoolStruct.h"
#include "public.H"

#include <cstring>
#include "time.h"
#include	<stdio.h>

#ifndef MAX
#define	MAX(a,b) ((a >= b) ? a : b)
#endif
#ifndef MIN
#define	MIN(a,b) ((a <= b) ? a : b)
#endif

# define  BIT(N)   	(1 << (N))	/* modified 1L to 1	*/
# define  setbit(VAL,N)	(VAL |= BIT(N))
# define  clrbit(VAL,N)	(VAL &= ~BIT(N))
# define  chgbit(VAL,N)	(VAL ^= BIT(N))
# define  bitis0(VAL,N) ((VAL & BIT(N))==0)
# define  bitis1(VAL,N) ((VAL & BIT(N))!=0)


#define  Tzero(D)   ((float)D->data_word[2]+.01*D->data_word[3]);


#ifndef N
#define	N 500
#endif

#define BLOCK_N 25

#define MAX_ADAPT  4000  /* ��� number of ����Ӧvalue in */

#define WGHT_MEAN_MAX   0.3f
#define MIN_ADAPT_VAL   0.5f /* ��С����value for adaption */
#define MAX_ADAPT_VAL   2.3f /* ��� ����value for adaption */   // modify by xie 11-8

/* D_TEMP_ADAPT	= 200 */
#define	D_TEMP_ADAPT	200.f 
#define MIN_ADAPT_FIRST   0.85f /* ��С����value for adaption first ��� */
#define MAX_ADAPT_FIRST   1.15f /* ��� ����value for adaption first ��� */
#define MIN_ADAPT_LOWACC 0.5f /*minimum ����value for adaption �� cooling */
#define MAX_ADAPT_LOWACC 1.5f /*maximum ����value for adaption �� cooling */
#define STEP_ADAPT_VAL  0.15f /* ��� allowed change in adaption (Default 0.1) */
#define N_ADAPT_PLATE     3  /* No. of plates of adaption (>= 4) Default 4 */
#define N_ADAPT_GAP    7200  /* time [s] if exceeds n_adapt_plate reduced */
#define THICK_ADAPT_P5   19  /* if thickness > ��Ӧ from this Pyrometer */
/* value from one ��� to the next */

#define NZUSB 5         /* number of segments in one group      */
#define OFFSET_SEG      20      /* offset of segments           */
#define SA   4          /* number of segments, which not used   */

//statistical
struct	s_val	{
	int	n;		/*	number of value		*/
	float	max;	/*	maximim value			*/
	float	min;	/*	minimum value			*/
	float	mean;	/*	ƽ��value				*/
	float	std;	/*	��׼��	*/
	float	s1;	/*	sum(xi)					*/
	float	s2;	/*	sum(xi^2)				*/
};

typedef struct s_val  s_val_t;

struct	spl { 		/*	spline interpolant */
	int		ix;		/*	points -ix up to +ix are incorp. */
	/*	in spline approximation */
	int		i;			/*	index of intervall i */
	int		ii;		/*	index first point of interpolant */
	int		n;			/*	number of function value of */
	/*	function to interpolate */
	int		imat;		/*	number of matrix elements */
	int		ir;		/*	number of elem. right side      */
	int		ibw;		/*	bandwidth of nonsymm. matrix */
	float		a;			/*	coefficient a[i] for intervall i */
	float		b;			/*	coefficient b[i] for intervall i */
	float		c;			/*	coefficient c[i] for intervall i */
	float		d;			/*	coefficient d[i] for intervall i */
	float		y2[50];		/*	second derivation				-	ir */
	float		rb[50];		/*	column right side				-	ir */
	float		mat[50];   	/*	matrix  algebr. system		-	imat  */
	int		izw;		/*	output of calculated value Y/N  */
};

typedef	struct	spl	spline_t;

typedef struct {
	float  ti_p1;	/* time at P1 */
	float  Tm_p1;	/* Measured Temperature at P1 */
	//	 float  ti_p2;	/* time at P2 */
	//	 float  Tm_p2;	/* Measured Temperature at P2 */
	float  ti_p2U;	/* time at P2 */
	float  Tm_p2U;	/* Measured Temperature at P2 */
	float  ti_p3U;	/* time at P2 */
	float  Tm_p3U;	/* Measured Temperature at P2 */
	float  ti_sct;	/* time at ACC entry */
	float  Tc_sct;	/* Tmean at ACC entry Start Cooling Temp */
	float  Tss_sct;/* T_surface at ACC entry Start Cooling Temp */
	float  Tsc_sct;/* T_core    at ACC entry Start Cooling Temp */
	float  Tss_fct;/* T_surface at ACC exit  Cooling Temp */
	float  Tsc_fct;/* T_core    at ACC exit  Cooling Temp */
	float  ti_fct;	/* time at ACC exit */
	float  Tc_fct;	/* Tmean at ACC exit Finish Cooling Temp */
	float  ti_p23;
	float  Tm_p23;	/* Measured Temperature at P2 */
	float  ti_p2L;	/* time at P2 */
	float  Tm_p2L;	/* Measured Temperature at P2 */

	float  Tc_p23;	/* Tmean Temperature at P5 */
	float  Tb_p23;	/* Bottom Surface Temp at P5 *////////////// add by wbxang 20100629 ///////////////////////


	float  Tc_p2L;	/* Tmean Temperature at P5 */
	float  Tb_p2L;	/* Bottom Surface Temp at P5 */
	float  ti_p3L;	/* time at P2 */
	float  Tm_p3L;	/* Measured Temperature at P2 */
	float  Tc_p3L;	/* Tmean Temperature at P5 */
	float  Tb_p3L;	/* Bottom Surface Temp at P5 */


	float  ti_fut;	/* time at ACC entry */
	//float  Tc_fut;	/* Tmean at ACC entry Start Cooling Temp */   // ������ʱû��  [9/26/2013 лǫ]
	//float  Tss_fut;/* T_surface at ACC entry Start Cooling Temp */
	//float  Tsc_fut;/* T_core    at ACC entry Start Cooling Temp */

	float  ti_sut;	/* time at ACC entry */
	//float  Tc_sut;	/* Tmean at ACC entry Start Cooling Temp */   // ������ʱû��  [9/26/2013 лǫ]
	//float  Tss_sut;/* T_surface at ACC entry Start Cooling Temp */
	//float  Tsc_sut;/* T_core    at ACC entry Start Cooling Temp */

	float  ti_p4;	/* time at P5 */
	float  Tm_p4;	/* Measured Temperature at P5 (bottom) */
	float  Tc_p4;	/* Tmean Temperature at P5 */
	float  Tb_p4;	/* Bottom Surface Temp at P5 */

	float  ti_p5;	/* time at P5 */
	float  Tm_p5;	/* Measured Temperature at P5 (bottom) */
	float  Tc_p5;	/* Tmean Temperature at P5 */
	float  Tb_p5;	/* Bottom Surface Temp at P5 */
	float  Tr_fct;	/* Real Finish Cooling Temp */

	//float  Tr_fut;	/* Real UFC Finish Cooling Temp */  // ������ʱû��  [9/26/2013 лǫ]

	float  CRreal; /* real CR (from measured data) */
	float  CRcalc; /* CR from calculation */
	float   CRcore; /* CR in centerline of ��� - calculated */
	float   CRsurf; /* CR on surface - calculated */


	float  ti_cool; /* ʵ�� coolinfg time	*/
	float  ti_acc_cool; /* ʵ�� coolinfg time	*/

	float  air1;	  /* time between P1 and entry ACC */
	float  air2;	  /* time between P4 and exit  ACC */
	float  air_mid;	  /* time between P4 and exit  ACC */
	float  max_diff_stop;	/* |Tr_fct - target Temperature|	*/
} TiTemp;	//������ʱ

//ͳ�Ƹ����׶εĲ�����Ϣ
struct svxx{
	int	   seg;
	float	res[NUMBER_OF_POST_RESULT];			/* calculated results */
	s_val_t  Tm_p1_sv;		/* measured entry temp	 */
	//	 s_val_t  *Tm_p2_sv;		/* measured entry temp	 */
	s_val_t  Tm_p2U_sv;
	s_val_t  Tb_p2U_sv;
	s_val_t  Tm_p3U_sv;
	s_val_t  Tb_p3U_sv;
	s_val_t  Tm_p5_sv;		/* measured exit  temp	 */
	s_val_t  Tm_p4_sv;		/* measured exit  temp	 */
	s_val_t  Tm_p23_sv;		/* measured entry temp	 */
	s_val_t  Tm_p2L_sv;		/* measured entry temp	 */
	s_val_t  Tm_p3L_sv;		/* measured exit  temp	 */

	s_val_t  Tc_sct_sv;	/* calculated start temp.*/
	s_val_t  Tc_fct_sv; 	/* calculated stop temp. */
	//s_val_t  Tc_sut_sv;	/* calculated start temp.*/    // ���� ��ʱû�� [9/26/2013 лǫ]
	//s_val_t  Tc_fut_sv; 	/* calculated stop temp. */
	
	s_val_t  Tb_p5_sv;		/* measured exit  temp	 */
	s_val_t  Tb_p4_sv;		/* measured exit  temp	 */

	s_val_t  Tr_fct_sv; 	/* recalculated stop temp. */
	/* based on measurement */
	s_val_t  Tx_fct_sv; 	/* recalculated stop temp. */
	/* based on measurement + filter */

	//s_val_t  Tr_fut_sv; 	/* recalculated stop temp. */  // ���� ��ʱû�� [9/26/2013 лǫ]
	///* based on measurement */
	//s_val_t  Tx_fut_sv; 	/* recalculated stop temp. */
	///* based on measurement + filter */

	s_val_t  Tb_p23_sv;		/* measured exit  temp	 */
	s_val_t  Tr_p23_sv; 	/* recalculated stop temp. */
	/* based on measurement */
	s_val_t  Tx_p23_sv; 	/* recalculated stop temp. */
	/* based on measurement + filter */

	s_val_t  Tb_p2L_sv;		/* measured exit  temp	 */
	s_val_t  Tr_p2L_sv; 	/* recalculated stop temp. */
	/* based on measurement */
	s_val_t  Tx_p2L_sv; 	/* recalculated stop temp. */
	/* based on measurement + filter */

	s_val_t  Tb_p3L_sv;		/* measured exit  temp	 */
	s_val_t  Tr_p3L_sv; 	/* recalculated stop temp. */
	/* based on measurement */
	s_val_t  Tx_p3L_sv; 	/* recalculated stop temp. */
	/* based on measurement + filter */

	s_val_t  CRcalc_sv;	/* cooling rate			 */
	s_val_t  CRreal_sv;	/* cooling rate			 */
	s_val_t  CRcore_sv;	/* cooling rate	core of ���	*/
	s_val_t  CRsurf_sv;	/* cooling rate	surface ���	*/
	s_val_t  Tss_sct_sv;	/* ����Temperature entry ACC */
	s_val_t  Tsc_sct_sv;	/* Core    Temperature entry ACC */
	s_val_t  Tss_fct_sv;	/* ����Temperature exit  ACC*/
	s_val_t  Tsc_fct_sv;	/* Core    Temperature exit  ACC*/
	s_val_t  ti_cool_sv;	/* cooling time			 */
	//s_val_t  Tss_sut_sv;	/* ����Temperature entry ACC */  // ���� ��ʱû�� [9/26/2013 лǫ]
	//s_val_t  Tsc_sut_sv;	/* Core    Temperature entry ACC */
	//s_val_t  Tss_fut_sv;	/* ����Temperature exit  ACC*/
	//s_val_t  Tsc_fut_sv;	/* Core    Temperature exit  ACC*/
	s_val_t  ti_acc_cool_sv;	/* cooling time			 */

	s_val_t  air1_sv;	/* air time P1 -> Entry ACC */
	s_val_t  air2_sv;	/* air time EXIT ACC -> P4*/
	s_val_t  air_mid_sv;	/* air time EXIT ACC -> P4*/
	/* contract value */
	float	  fTempDevTempEntry; /* allowed Temperature ƫ�� caused by entry Temp */
	float	  fTempDevSpd; /* allowed Temperature ƫ�� caused by �ٶ� ƫ�� */
	float	  fTc_sct[N]; /* calculated start Temperature */
	float	  fTc_fct[N]; /* calculated stop Temperature */
	float	  fTr_fct[N]; /* ����ed stop Temperature */
	float	  fTx_fct[N]; /* ����ed stop Temperature + filter */
	float	  fTx_fctMean; /* ����ed stop Temperature + filter mean*/

	//float	  fTc_sut[N]; /* calculated start Temperature */  // ���� ��ʱû�� [9/26/2013 лǫ]
	//float	  fTr_sut[N]; /* ����ed stop Temperature */
	//float	  fTx_sut[N]; /* ����ed stop Temperature + filter */
	//float	  fTx_sutMean; /* ����ed stop Temperature + filter mean*/
	//float	  fTc_fut[N]; /* calculated stop Temperature */
	//float	  fTr_fut[N]; /* ����ed stop Temperature */
	//float	  fTx_fut[N]; /* ����ed stop Temperature + filter */
	//float	  fTx_futMean; /* ����ed stop Temperature + filter mean*/

	float	  fti_cool[N]; /* calculated stop Temperature */
	//float	  fti_acc_cool[N]; /* calculated stop Temperature */ // ���� ��ʱû�� [9/26/2013 лǫ]

	float    fCR_fct[N]; /* calculated ��ȴ�ٶ�*/
	float    fCR_fctMean; /* calculated ��ȴ�ٶ�mean*/


};

typedef struct svxx FATsegs;


#define  Tzero(D)   ((float)D->data_word[2]+.01*D->data_word[3]);

class  CSelfLearnFunction 
{

public:
	CSelfLearnFunction(void);

	int SuchWrite(FILE		*erg_f,		/* report FILE */	
					const	FATsegs		*xx,		/* Statistical value    */
					const	PRERESULT *PreResult,
					const	INPUTDATAPDI *pdiInPut, 
					PHYSICAL *ver_par1, 
					float qcr,
					ADAPTATION *adaptUsed);

	int	WriteAdaptationInfo(FILE	*fadapt,const	ADAPTATION *adapt);

	/*      level for adaption factor       */
	int     AdaptionProcess(
		const	INPUTDATAPDI *pdiInPut,
		const	int			nStroke,		   /* No. of strokes */
		ADAPTATION *adapt,			   /* adaptation structure */
		unsigned long int   *adap_key, /* Adaption_Key */
		float      *adaptRatioFound,
		float waterTemp);		   /* Adaption_Factor */

	int	InitAdaptation(ADAPTATION *adapt);

	unsigned long int KeepAdaptionKey(unsigned long int key_b[5],const	INPUTDATAPDI *pk1,float watertemp);
	unsigned long int KeepAdaptionKey2(unsigned long int key_b[5],const	INPUTDATAPDI *pk1,float waterTemp);

	// add [1/13/2015 qian]
	void AdaptationSLP_Init(SLP *adaptPara);//��ʼ����ѧϰ�޼�����[2014/12/7 by Zed]
	int AdaptationSLP_Read(
		const	INPUTDATAPDI *pdiInput,
		SLP		*adaptPara);
	int AdaptationSLP_Write(
		const	INPUTDATAPDI *pdiInput,
		const	PRERESULT *PreResult,
		const	FATsegs *seg,
		SLP		*adaptPara,
		int offLineFlag
		);
	int AdaptionSLP_Get(
		const	INPUTDATAPDI *pdiInput,
		SLP		*adaptPara);

private:

	int	checkLimitAdapt(const float qcr);

	int	checkStepAdapt(
		float *qcr_new,		/* calculated ����Ӧvalue*/
		const float qcr_file,		/* ����Ӧvaluefrom FILE  */
		const float fAdaptStepSize, /* step size adaption */
		const float fAdaptMin,		/* ��С����Ӧvalue*/
		const float fAdaptMax);	    /* ��� ����Ӧvalue*/

	int		setLimitAdapt(
		const	INPUTDATAPDI *pdi,			/*PDI���� */
		const	int			nPlate,			/* No. of recorded plates */
		const	int			iAdaptLevel,	/* level of adaption */
		const	ADAPTATION *adaptWriteL1,	/* wriiten to level 1 */
		float		*fAdaptStep,	/* step size */
		float		*fAdaptMin,		/* min. value window */
		float		*fAdaptMax);	/* max. value window */


	/*  write adaption history to FILE                      */
	int	LogAdapt(
		const	PRERESULT *PreResult,			/* setup ��Ϣ */
		const	char	*sAdapt);		/* string for storage */

	/* calculate ����Ӧvalue                            */
	int		SetAdaptionValues(
		float	qcr,			/* ����Ӧvaluecr */
		const	int		keyfound,		/* 0: no key 1: found key */
		const	int		iLine,			/* modify line */
		const	FATsegs	*xx,			/* Statistical value    */
		const	PRERESULT *PreResult,			/* data block preset */
		const	INPUTDATAPDI *pdi,			/*PDI���� */
		const	int		iAdaptLevel,	/* level of adaption */
		const	ADAPTATION *adaptRead,		/* record set from FILE */
		const	ADAPTATION *adaptUsed,		/* ʵ�� ��� */
		const	ADAPTATION *adaptWriteL1,	/* written to level 1 */
		ADAPTATION *adaptWrite);	/* modify to write */

	/* determine transfer value of ʵ�� ��� (_last)    */
	int		SetLastActualPlate(
		const	FATsegs	*xx,		/* Statistical value    */
		const	PRERESULT *PreResult,		/* preset DB */
		ADAPTATION *adapt,
		const ADAPTATION *adaptRead);	/* adapted value */

	float	s_val_relay(
		const	float	val,		/* value for check	*/
		const	float	val_def,    /* Ĭ��value	*/
		const	float	val_min,	/* ����      */
		const	float	val_max)	/* ����      */
	{
		float	val_set = val;
		if(val < val_min)	val_set = val_def;
		if(val > val_max)	val_set = val_def;
		return(val_set);
	};

private:

	float SuchRead(
		const	INPUTDATAPDI *pdi,		/*PDI���� */
		const	unsigned long int	key_b[5],		/* adaption key       */
		const	int			use_dat,	/* adaption level (1: most dist) */
		const	int			taskID,		/* PreCalc, PostCalc */
		const	int			nStroke,	/* No. of stokes */
		ADAPTATION *adapt);		/* O: found adaption ��Ϣ */

	/* x.1) called in SuchWrite()                           */
	/* calculate and write ����Ӧvalue                  */

	int SuchWrite(		
		FILE		*erg_f,		/* report FILE */	
		const	FATsegs		*xx,    /* Statistical value */
		const	PRERESULT *PreResult,	/* data block preset  */
		const	INPUTDATAPDI *pdi,
		const	unsigned long int	key_b[5], 
		const	int			use_dat, 
		float		qcr,
		ADAPTATION *adaptUsed);


	int	WriteAdaptation(const	char	*sPlateID,const	ADAPTATION *adapt);

	int	ReadAdaptationInfo(
		const	char	*sPlateID,
		ADAPTATION *adapt);


	int	ReadAdaptationFromString(const	char	*sRead,	ADAPTATION *adapt);


	int	WriteAdaptationToString(const	ADAPTATION *adapt,	char	*sWrite);

	int	CopyAdaptation(
		const	ADAPTATION *adapt_nm1,	/* basis ��Ϣ */
		ADAPTATION *adapt);		/* copy */


	int	InitAdaptation(
		const	ADAPTATION *adapt_nm1,	/* basis ��Ϣ */
		const	char	*sPlateID,	/* ʵ�� ��� ID */
		ADAPTATION *adapt);		/* copy and shift */


private:
	/*  estimate ���� adaption �ļ��� */

	char	*EnsureAdaptationFileName(
		const	INPUTDATAPDI *pdi,		/*PDI���� */
		const	int			use_dat,	/* adaption level (1: most dist) */  
		const	int			taskID,		/* PreCalc, PostCalc */
		const	int			nStroke)
	{
		static char	psFileNameAdaptDir[PATH_MAX];
		char	psFileNameAdapt[PATH_MAX];
		char	sStrStroke[32];


		switch(pdi->acc_mode)
		{
		case(ACC):	/* normal ACC Plates */
			sprintf(psFileNameAdapt, "%iver", use_dat);
			break;
		case(DQ):	/* direct quench */
			sprintf(psFileNameAdapt, "%iver_dq", use_dat);
			break;
		case(IC):	/* interphase cooling */
			sprintf(psFileNameAdapt, "%iver_ic", use_dat);
			break;
		case(SC):	/* soft cooling */
			sprintf(psFileNameAdapt, "%iver_sc", use_dat);
			break;
		case(UFC):
			//sprintf(psFileNameAdapt, "%iver_ufc", use_dat);
			sprintf(psFileNameAdapt, "%iver", use_dat);
			break;
		default:
			sprintf(psFileNameAdapt, "%iver", use_dat);
		}
		if((nStroke > 1) && (nStroke <= ANZ_HUB)) 
		{
			sprintf(sStrStroke,"Hub_%1d", nStroke); 
			strcat(psFileNameAdapt, sStrStroke);
		}
		strcat(psFileNameAdapt, ".dat");
		CombinFilePath(psFileNameAdapt, taskID, psFileNameAdaptDir);
		return(psFileNameAdaptDir);
	};	/* No. of strokes */
};



static	CSelfLearnFunction m_SelfLearnFunction;