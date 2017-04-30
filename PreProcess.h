#pragma once
#include "../BaseCoolStruct.h"
#include <cstring>

#define  BITX(N)    (1 << (N))
//////////////////////////////////�ṹ�嶨��////////////////////////////////


struct T_AccPattern
{
	long	m_ID;
	char	m_PatternID[256];
	char	m_NozzleTop[256];
	char	m_NozzleBot[256];
	char	m_FlowTop[256];
	char	m_FlowBot[256];
	char	m_SideSpray[256];
	char	m_SideSprayFlow[256];
	double	m_Width;
	double	m_Thickness;
	double	m_EntryTemp;
	double	m_ExitTemp;
	double	m_Speed;
	double	m_HeadFlowTop;
	double	m_HeadFlowBot;
	double	m_HeadLengthTop;
	double	m_HeadLengthBot;
	double	m_TailFlowTop;
	double	m_TailFlowBot;
	double	m_TailLengthTop;
	double	m_TailLengthBot;
	int		m_FreezeSpeed;
	char    m_EdgeMask[256];
	char	m_SpeedProfile[256];
	char	m_PositionProfile[256];

};

struct T_AccSysPar
{
	char	m_ID[256];
	long	m_NoCoolingUnits;
	long	m_NoEdgeMask;
	long	m_NoSideSprays;
	double	m_WidthRange;
	double	m_ThicknessRange;
	double	m_EntryTempRange;
	double	m_ExitTempRange;
};

struct flowcr
{
	char  sPatternID[256];
	int	iFlwLimit;     /* 1: use flow specified by PDI */

	float flow_rate[20];    // flow_rate for topFlowRate    �˴�����û������ˮ��
	float cr_rate[20];      /* Calculated CR_rate from 10 Flow_Values */
	float flow_factor[20];  /* RATIO of bottomFlowRate/topFlowRate */
	float	flow_factor_width[20]; /* flow factor width [anz_width] */

	//лǫ ����ķ�϶�� ˮ��������
	float gap_flow_rate[20];    /* flow_rate for topFlowRate  */
	float gap_cr_rate[20];      /* Calculated CR_rate from 10 Flow_Values */
	float gap_flow_factor[20];  /* RATIO of bottomFlowRate/topFlowRate */
	float	gap_flow_factor_width[20]; /* flow factor width [anz_width] */

	float  flow_factor_dq[20];    /*  ratio top/bottom for    */

	float  cr_akt;        /* CSC_CR to S5/MC in (1/10 K/sec) */
	float  flow_t;	/* specific flow top body [ltr/(m^2*min)] */
	float  flow_b;	/* specific flow btm body [ltr/(m^2*min)] */
	float aveHC;  // add [12/31/2013 xie]

	//лǫ ����ķ�϶�� ˮ��������
	float  gap_flow_t; /* specific flow top body [ltr/(m^2*min)] */
	float  gap_flow_b; /* specific flow btm body [ltr/(m^2*min)] */

	float  add_flow;  /* additional flow bottom in percent */
	float  lin_flow;  /* linear add of flow bottom for first header */
	int   switchx;       /*      number of method for edge pos   */

	int   anz_flow;  /*  number of different flows are in use  ʹ�ü������*/
	int	anz_width;  /* No. different flows for width ����ion */

	int   slit_anz_flow;  /*  number of different flows are in use  ��϶��ʹ�ü������*/
	int	slit_anz_width;  /* No. different flows for width ����ion */

	int   edge[NUZZLE_NUMBER]; /* position of edge �ڱ� in mm    */
	/* in case fixed ��ȴģʽ is specified */
	int   iFlwTop[NUZZLE_NUMBER]; /* specific flow top header    */
	int   iFlwBtm[NUZZLE_NUMBER]; /* specific flow bottom header */
	int    offset;    /*  additional position for header  */
	/*  inside ���            */
	int   iadaption;     /*      adaption is activated == 1      */
	int   write_adapt;   // write ����Ӧvalue not activated      == 0 
	/*      not activated         == 0      */
	int    last_dq;   //  last header with nonreduced flow 
	float reduction_factor;   /*  value for reduction of  */

	float zeit_stop[20];
	float zeit_highdensity_stop[20];  // лǫ ���� 2012-5-3
	float acc_zeit_stop[20];
	float aveHeatTC[10];

	float time_c;
	float time_c_highdensity;   //лǫ ���� 2012-5-3

	float newTime; //лǫ ���ڴ洢ȷ�����ܿ���״�������ȴtime
	float time_acc;
	float fLengthTotal; /* tatal length of impingement zone */
	float vx_mean;      /* expected transfer �ٶ� within ACC */

	int	nEdgeMask; /* No. of individual drives edge mask */
	int	iEdgeMask;
	int	iLengthMaskMode;
	int	iEdgeMaskMode[NUZZLE_NUMBER];
	/* specific flows are multipliers for set points of flow */
	/* flowMaskHeadTop = 0.85 means 85% of set point flow flow_t */
	float	LengthMaskHeadTop;	/* ͷ�����γ��� [m] */
	float	LengthMaskHeadBtm;	/* ͷ�����γ��� [m] */
	float	flowMaskHeadTop;	/*��������value [-] */ 
	float	flowMaskHeadBtm;	/*��������value [-] */ 
	float	LengthMaskTailTop;	/* ͷ�����γ��� [m] */
	float	LengthMaskTailBtm;	/* ͷ�����γ��� [m] */
	float	flowMaskTailTop;	/*��������value [-] */ 
	float	fFlwMaskTailBtm;	/*��������value [-] */ 
	int	nuzzleMaskHeadTop;	/* No. �ر���ȴװ��ͷ�� top */
	int	nuzzleMaskHeadBtm;	/* No. �ر���ȴװ��ͷ�� btm */
	int	nuzzleMaskTailTop;	/* No. �ر���ȴװ��β�� top */
	int	nuzzleMaskTailBtm;	/* No. �ر���ȴװ��β�� btm */
};

typedef struct flowcr flow_cr;


struct tagNuzzleCondition
{
	int     header_no;
	float   flow_header_top[NUZZLE_NUMBER];
	float   flow_header_bottom[NUZZLE_NUMBER];
	int  header_bit;
	int  working_status[NUZZLE_NUMBER];    //лǫ���� 2012-5-3
	float header_no_modify_speed;     //лǫ���� 2012-5-8
	float   total_acc_flow;
	float   total_flow;

	int calcTempCurveANZK;   // лǫ���� ���ڼ�����ȴ���ߵļ���λ
	float airCoolingTempModify;  //лǫ ������������ʱ�� Temperature��value
	float TempThick;  //add by xie 11-2
};
typedef struct tagNuzzleCondition NUZZLECONDITION;


class CPreProcess
{

private:

	int   CalculateAbszValue( const float   *x,const float   *y,const   int    n,const   float  ord,float	*absz);

	float GetMinValue(const int n,const float eps,const float *x);
	float GetMaxValue(const int n,const float eps,const float *x);
	float	GetArrayMinValue( const float *y, const int	n);
	float	GetArrayMaxValue( const float *y, const	int	n);
	float	CheckMinTemperatureDiff(const	float dThick);

	int	ModifyFlow(
		FILE	*st_err, 
		const	INPUTDATAPDI *pdiInPut,
		const	STRATEGY *accStrat1,
		flow_cr *cr_flow);

	int	ReadInterpolateFlowWidth(
		FILE	*st_err, 
		const	INPUTDATAPDI *pdiInPut,
		const	int		nFlw,
		const	float	*fFlw,
		float	*flow_factor_width);

	int	CheckLengthMask(
		const int     iPos,     /* 1: ͷ�� 2: β�� */
		const INPUTDATAPDI *pdiInPut,	/*PDI����	*/
		const PRERESULT *PreResult,    /* pdi + setup */
		flow_cr *cr_flow);	/* flow value	*/

	int	CheckValueBounds(
		const	float	fValAct,	/* ʵ�� value for testing */
		const	float	fValMin,	/* ��С����value */
		const	float	fValMax);	/* ��� ����value */


private:
	int	FloatToInt(	const	float	fVal);
	int InputInfomations(FILE *erg_f, FILE *st_err, PHYSICAL  *ver_par1,PRERESULT *PreResult);
	int OutputPreInfo( INPUTDATAPDI *pdiInPut, FILE *db_mds2);
	int	CheckDataValid(INPUTDATAPDI *pdi);	/* data telegram    */
	int	SetLowFlowCoolingRate(FILE	*erg_f, const	INPUTDATAPDI *pdi, const	int	iFlwLimit, float	*fCr);		/* �޸� ��ȴ�ٶ� [10 * K/s] */
	int CombinFile(const int taskID, const char *plate_id, const PRERESULT *PreResult, char *name_b);
	flow_cr GetFlowInfo(FILE *st_err, const	INPUTDATAPDI *pdiInPut, const	STRATEGY *accStrat1, PRERESULT *PreResult, const float waterFactorCoe);
	flow_cr	InitFlow(const 	int	nFlw,const int nslitFlw);
	int CoolRateFlows(	FILE	*erg_f,		/* ���FILE */
						const	INPUTDATAPDI *pdiInPut,	/*PDI���� */
						const	ADAPTATION *adapt,		/* ����Ӧvalue */
						flow_cr *cr_flow,	/* flow value */
						const	float	qcr_find,/* ����ӦvalueCR */
						int tStage);	
	void CalculateEdgePosition(const INPUTDATAPDI *pdiInPut,	/*PDI����	*/
								const	RALCOEFFDATA *lay, /* layout data */
								flow_cr *cr_flow,	/* flow value			*/
								PRERESULT *PreResult);		/* PREPROCESS telegram	*/
	int	ReadEdgeInfo(const	float	fFlwAct,	/* specific flow top */
						const	float	fWidth,		/* width of ��� */
						const	char	*sFileName,	/* �ļ��� */
						int		*iDir,		/* 1: narrows, -1: opens */
						int		*nPoly,		/* grade of polynomial approach */
						float	*fPosMin,	/* ��С��Եλ�� */
						float	*fPosMax);	/* ��� ��Եλ�� */

	int	CalculateInterpolationEdge(const	RALCOEFFDATA *lay, /* layout data */
									const	int		iDir,		/* 1: narrows, -1: opens */
									const	int		nPoly,		/* polynomial interpolation */
									const	float	fPosMin,	/* ��С��Եλ�� */
									const	float	fPosMax,	/* ��� ��Եλ�� */
									int		*iEdgePos);	/* edge pos. individual drives */
	int CalculMaskFlowReduction(FILE	*st_err, const	INPUTDATAPDI *pdiInPut, flow_cr *cr_flow,	PRERESULT *PreResult);		/* PREPROCESS telegram	*/
	int	ReadHeadMaskingValues(const	INPUTDATAPDI *pdiInPut,	flow_cr *cr_flow,	const	char	*sFileName); /* �ļ��� */
	int	ReadTailMaskingValues(const	INPUTDATAPDI *pdiInPut,	flow_cr *cr_flow,	const	char	*sFileName); /* �ļ��� */
	int LogPreResult(FILE *erg_f, const	flow_cr *cr_flow, const	PRERESULT *PreResult, const int n);
	int WirteTelegramPre(const	char *plate_id, const PRERESULT *PreResult, FILE *erg_f);
	int LogPreInfo(FILE        *erg_f,
					const	INPUTDATAPDI *pdiInPut,
					const   flow_cr     *cr_flow,
					const   PRERESULT *PreResult,
					const   int			levelNumber,
					const   float       adaptRatioFound);
	int pre_out_fileDe(   
		FILE        *erg_f,
		const	INPUTDATAPDI *pdiInPut,
		const   flow_cr     *cr_flow,
		const   int			*in,
		const   int			levelNumber,
		const   float       adaptRatioFound);
	void 	OutputErrorLog( FILE *st_err, const   char *id, const   char *mess, const   char *func_n);  /* function name */
	int	ReadAccStrategyParam(const	INPUTDATAPDI *pdiInPut,RALCOEFFDATA *lay, STRATEGY *accStrat1);
	int	ModifyPDIStrategy(
							FILE	*erg_f,	/* ���FILE */
							const	STRATEGY *accStrat,	
							INPUTDATAPDI *pdi,	/*PDI���� */
							RALINDATA *ralInPut,
							PRERESULT *PreResult);	/* preset telegram */
	int	ModifyAccStrategy(
		const	RALINDATA *ralInPut,
		const   RALCOEFFDATA *lay,
		STRATEGY *accStrat1);

	INPUTDATAPDI *inp_sim(FILE *st_err, char *datname);

	int OutputPDIData(RALINDATA *ralInPut,FILE *ffx);
	int OutputLayoutData(RALCOEFFDATA *lay,FILE *ffx);

	RALOUTDATA *pre_out(FILE *st_err);

	int MakeOnlineResultFileName(const char *plate_id,const int  *in,char *fn);
	int inp_mes_ssab_sim(RALMESDATA *xzz,	FILE *st_err, char *datname);
	int inp_mes_vagb_sim(RALMESDATA *xzz, FILE *st_err, char *datname);
	int outp_mes_sim(RALMESDATA *xzz, FILE *st_err, char *datname);
	
	//��ַ���䣬��ʼ��
	PHYSICAL InitPhysicalParam(
								const int	inumber, /* No. of  cool. zones */
								const int	imat,    /* Type of material */
								const int	iTask,	 /* task */
								FILE	*st_err);

	int	SetCoolPatternFlowValues(
		flow_cr							*cr_flow,	/* flow value		*/	
		const	INPUTDATAPDI *pdi_mds,	/* PDI              */
		const	RALCOEFFDATA *lay,		/* layout data      */
	struct T_AccSysPar			   *SysPar,
	struct T_AccPattern			   *AccPattern);
	int     WritePreSSAB(
		RALCOEFFDATA *lay,
		RALOUTDATA *pre_out,
		NUZZLECONDITION *nuzzleCondtion);

	double   CalculateInterpolationValue(
		const   int      n,     /* ���value        */
		const   double   absz,  /* ��value absc.   */
		const   double   *x,    /* ������ value      */
		const   double   *y);    /* ������value      */


	/* set ��ȴ�ٶ�to reasonable value if no PDI ��Ϣ */
	float	SetCollingRate	(const	float dThick,const	float dWidth);
	int	SetCollingRateLimits (const	float dThick, float *fCrMin, float *fCrMax);
	int	CSVToInt(const	char	*sx,const	int		nMax,int		*iVal);
	int	ColumnDiff(
		const	int		n,			/* number of elements		*/
		const	int		*iCol1,		/* basic column	1			*/
		const	int		*iCol2,		/* basic column	2			*/
		int		*iDif);		/* index of changing value  */

	int	InitAirStartProfile(     COMPUTATIONAL *compx,	const		  INPUTDATAPDI *pdi,	PHYSICAL *ax);
	float	InitAirVariable(    COMPUTATIONAL *compx,	const		  INPUTDATAPDI *pdi,	PHYSICAL *ax);

	int CalculateInterpolationFlow(
		const   int     n,          /* ���value */
		const   float   fCrReq,     /* required ��ȴ�ٶ�*/
		const   float   *fFlwAcc,   /* flow value */
		const   float   *fCrAcc,    /* cooling rates */
		const   float   *fTimeAcc,  /* corresponding cooling times */
		const   float   *fFlwRatio, /* flow ratios */
		const   float   *fFlwFacWidth, /* flow ����ion width */
		const   float   *fFlwHC, /* flow ����ion width */
		float   *fFlwAct,   /* ��value flow value */
		float   *fCrAct,    /* ʵ�� ��ȴ�ٶ�*/
		float   *fTimeAct,  /* ʵ�� required cooling time */
		float   *fFlwRatioAct,/* flow ratio */
		float *aveHC); 

	int CheckMonoton(
		const   int n,          /* ���value */
		const   int iDir,       /* -1: decreasing value 1: increasing value */
		const   float   *x);    /*  value */

	int OutputFlow(
		FILE    *erg_f,
		const   float	flow_t,
		const	float	cr_akt,
		const	float	time_c,
		const	float	flow_b);

	/* output relevant flow value */
	int OutputFlow(	FILE    *erg_f,	const   flow_cr *cr_flow);

	int CPreProcess::SaveAirTelegram(
		const   int n_max,
		const   float   finishRollTemp,   /* expected ��� Temperature */
		PRERESULT *PreResult,            /* Ԥ������ */
		const   float   *zl,            /* time value air curve */
		const    float   *tl);           /* Temperature value air curve */

	int CalcDifferentWaterFlow(
		FILE  *st_err,
		FILE  *erg_f,
		INPUTDATAPDI *pdiInPut,
		MATERIAL *matal,
		PHYSICAL *ver_par1,
		COMPUTATIONAL *compx,
		flow_cr    *cr_flow,
		const float   real_temp,
		const float   *end_temp_air,
		const float   *end_aust_air);

	int CalculateEdgePos(const	RALCOEFFDATA *lay, float *zeta);
	int   CalculatePolynomialCoefficients(const	float *zeta, const float *eta, float *coeff);

	int	SelectCollingUnits(
		const	int		nSideSpray,		/* No. of side sprays */
		const	int		*iStatus,		/* status of side sprays */
		const	int		*iFirstDeAct,	/* first to deactivate */
		const	int		*iSecondDeAct,	/* second to deactivate */
		int		*iDeAct);		/* indexes deactivation */

	int GenerateMostDistCoolPattern(
		const	int		iHeaderDir,	/* header direction */
		const	int		nMax,		/* elements iSelect */
		const	int		nAct,		/* required active elements */
		const	int		nX,			/* No. preferred inactive elements */
		const	int		*iX,		/* preferred inactive elements [nX] */
		int		*iSelect);	/* selected active elements [nMax] */

	int GenerateRemainActiveUnits(
		const	int		nMax,		/* elements iSelect */
		const	int		nAct,		/* required active elements */
		const	int		nX,			/* No. preferred inactive elements */
		const	int		*iX,		/* preferred inactive elements [nX] */
		int		*iRemain);  /* selected active elements [nMax] */

	int GenerateRemainAUSecondMax(
		const	int		nRemain,	/* elements iSelect */
		int		*iRemain);  /* selected active elements [nRemain] */

	int GetMaxSwapOperations(
		const	int	n,
		const	int *iDist,
		int *iSwap);


public:

	void readFileSch(
		RALINDATA		*ralInPut,
		PRERESULT		*PreResult,
		FILE *erg_f);

	void readFileSchAcc(
		RALINDATA		*ralInPut,
		PRERESULT		*PreResult,
		FILE *erg_f);

	void readFileSchZheng(
		RALINDATA		*ralInPut,
		PRERESULT		*PreResult,
		FILE *erg_f);
	
	pwr_tInt32 PreProcess(
		RALINDATA *ralInPut,
		RALCOEFFDATA *lay,
		RALOUTDATA *pre_out,
		pwr_tInt32                      *proc_id,
	struct T_AccSysPar				*SysPar,
	struct T_AccPattern				*AccPattern);

	pwr_tInt32  CPreProcess::PreProcessElite(
		RALINDATA *ralInPut,
		RALCOEFFDATA *lay,
		RALOUTDATA *pre_out,
		pwr_tInt32                     *proc_id,
	struct T_AccSysPar			   *SysPar,
	struct T_AccPattern			   *AccPattern);


	void CoolSchOpenFront(NUZZLECONDITION *nuzzleCondtion,const flow_cr *cr_flow);
	void CoolSchOpenFrontSlit(NUZZLECONDITION *nuzzleCondtion,const flow_cr *cr_flow);  // add [4/24/2014 лǫ]


	RALCOEFFDATA LayoutSim(FILE *st_err,	char datname[256]);  // лǫ �޸� �ĳɹ̶������ַ��� 2012-7-8
	float   AccPreferedSpeed(   
		const	float	fThickPlate,	/* ��ĺ�� [mm] */
		const	int		n,				/* ���value */
		const	float	*fThick,		/* ���value (n) [mm] */
		const	float	*fSpd);			/* �ٶ�value (n) [m/sec] */

	CPreProcess();
	~CPreProcess();

	int CalcTempCurvePost(
		FILE 			*st_err,
		FILE 		*erg_f ,
		INPUTDATAPDI *pdiInPut,
		const	  RALCOEFFDATA *lay,		/* layout data      */
		const      RALOUTDATA *preOut,   // лǫ���� 2012-5-16
		PHYSICAL *ver_par1,
		COMPUTATIONAL *compx/* ��ȴ���� */
		);

	int CalcTempCurvePostOffLine(
		FILE 			*st_err,
		FILE 		*erg_f ,
		INPUTDATAPDI *pdiInPut,
		const	  RALCOEFFDATA *lay,		/* layout data      */
		const     RALOUTDATA *preOut,   // лǫ���� 2012-5-16
		PHYSICAL *ver_par1,
		COMPUTATIONAL *compx,
		float aveSpeed	/* ��ȴ���� */
		);

private:
	void EstimationComputationalParam(FILE *erg_f,COMPUTATIONAL *compx,const	 INPUTDATAPDI *pdi,	PHYSICAL *ax);
	int  CalculatePre(
		FILE          *st_err,    /* �쳣���� */
		FILE          *erg_f,     /* ��Ľ��FILE */
		const   struct T_AccSysPar	*SysPar,    /* ϵͳ����  */ 
		const   struct T_AccPattern	*AccPattern,/* ��ȴģʽ    */
		INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
		const	  RALCOEFFDATA *lay,		/* layout data      */
		const	  STRATEGY *accStrat,	/* ���Բ��� */
		PRERESULT *PreResult,        /* Ԥ������    */
		PHYSICAL *ver_par1,  /* ��ȴ���� */
		flow_cr       *cr_flow,   /* ������Ϣ   */
		ADAPTATION *adapt,
		SLP		*SLP_post); 

	///////////////////////����ʽ��ȴԤ���� add by Zed 2013.12.5////////////////
	int  CalcPreOSC_Out(
		FILE          *st_err,    /* �쳣���� */
		FILE          *erg_f,     /* ��Ľ��FILE */
		const   struct T_AccSysPar	*SysPar,    /* ϵͳ����  */ 
		const   struct T_AccPattern	*AccPattern,/* ��ȴģʽ    */
		INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
		const	  RALCOEFFDATA *lay,		/* layout data      */
		const	  STRATEGY *accStrat,	/* ���Բ��� */
		PRERESULT *PreResult,        /* Ԥ������    */
		PHYSICAL *ver_par1,  /* ��ȴ���� */
		flow_cr       *cr_flow,   /* ������Ϣ   */
		ADAPTATION *adapt
		);
	///////////////////////����ʽ��ȴԤ���� add by Zed 2013.12.5////////////////
	// ���ø�socket���͵ĸ���Ϣ����Ҫ��203ϵ��[9/12/2013 лǫ]
	void configeSocketSendMessage(
		PRERESULT *PreResult,
		INPUTDATAPDI *pdiInPut,		/* PDI data           */
		COMPUTATIONAL *compx,			/* computational para.*/
		flow_cr       *cr_flow);

	int calc_slit_jet_flowrate(
		FILE  *st_err,
		FILE  *erg_f,
		INPUTDATAPDI *pdiInPut,
		MATERIAL *matal,
		PHYSICAL *ver_par1,
		COMPUTATIONAL *compx,
		flow_cr    *cr_flow,
		const float   real_temp,
		const float   *end_temp_air,
		const float   *end_aust_air,
		const RALCOEFFDATA *lay	/* layout data      */
		);

	int gap_cr_pol(	FILE	*erg_f,		/* ���FILE */
		const	INPUTDATAPDI *pdiInPut,	/*PDI���� */
		const	ADAPTATION *adapt,		/* ����Ӧvalue */
		flow_cr *cr_flow,	/* flow value */
		const	float	qcr_find,/* ����ӦvalueCR */
		int tStage);

	int  AccOpenTypeDecide(
		FILE          *st_err,    /* �쳣���� */
		FILE          *erg_f,     /* ��Ľ��FILE */
		INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
		const	  RALCOEFFDATA *lay,		/* layout data      */
		const	  STRATEGY *accStrat,	/* ���Բ��� */
		PRERESULT *PreResult,        /* Ԥ������    */
		PHYSICAL *ver_par1,  /* ��ȴ���� */
		flow_cr       *cr_flow, /* ������Ϣ   */
		COMPUTATIONAL *compx,
		ADAPTATION adapt1,
		float               real_temp,
		float   *end_temp_air,
		float   *end_aust_air
		);     

	int  UfcOpenTypeDecide(
		FILE          *st_err,    /* �쳣���� */
		FILE          *erg_f,     /* ��Ľ��FILE */
		INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
		const	  RALCOEFFDATA *lay,		/* layout data      */
		const	  STRATEGY *accStrat,	/* ���Բ��� */
		PRERESULT *PreResult,        /* Ԥ������    */
		PHYSICAL *ver_par1,  /* ��ȴ���� */
		flow_cr       *cr_flow, /* ������Ϣ   */
		COMPUTATIONAL *compx,
		ADAPTATION adapt1,
		float               real_temp,
		float   *end_temp_air,
		float   *end_aust_air
		);   

	int  Osc_UfcOpenType(
		FILE          *st_err,    /* �쳣���� */
		FILE          *erg_f,     /* ��Ľ��FILE */
		INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
		const	  RALCOEFFDATA *lay,		/* layout data      */
		const	  STRATEGY *accStrat,	/* ���Բ��� */
		PRERESULT *PreResult,        /* Ԥ������    */
		PHYSICAL *ver_par1,  /* ��ȴ���� */
		flow_cr       *cr_flow, /* ������Ϣ   */
		COMPUTATIONAL *compx,
		ADAPTATION adapt1,
		float               real_temp,
		float   *end_temp_air,
		float   *end_aust_air
		);  

	int  DqOpenTypeDecide(
		FILE          *st_err,    /* �쳣���� */
		FILE          *erg_f,     /* ��Ľ��FILE */
		INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
		const	  RALCOEFFDATA *lay,		/* layout data      */
		const	  STRATEGY *accStrat,	/* ���Բ��� */
		PRERESULT *PreResult,        /* Ԥ������    */
		PHYSICAL *ver_par1,  /* ��ȴ���� */
		flow_cr       *cr_flow, /* ������Ϣ   */
		COMPUTATIONAL *compx,
		ADAPTATION adapt1,
		float               real_temp,
		float   *end_temp_air,
		float   *end_aust_air);   

	int  BackAccOpenTypeDecide
		(
		FILE          *st_err,    /* �쳣���� */
		FILE          *erg_f,     /* ��Ľ��FILE */
		INPUTDATAPDI *pdiInPut,   /* PDI�ṹ��      */
		const	  RALCOEFFDATA *lay,		/* layout data      */
		const	  STRATEGY *accStrat,	/* ���Բ��� */
		PRERESULT *PreResult,        /* Ԥ������    */
		PHYSICAL *ver_par1,  /* ��ȴ���� */
		flow_cr       *cr_flow, /* ������Ϣ   */
		COMPUTATIONAL *compx,
		ADAPTATION adapt1,
		float               real_temp,
		float   *end_temp_air,
		float   *end_aust_air);  

	int CalcTempCurve(
		FILE 			*st_err,
		FILE 		*erg_f ,
		INPUTDATAPDI *pdiInPut,
		RALCOEFFDATA *lay, /* layout data      */
		flow_cr       *cr_flow,
		PHYSICAL *ver_par1,
		NUZZLECONDITION   *nuzzleCondtion,/* ������Ϣ   */
		PRERESULT *PreResult);

	int CoolSchduleDecide(
		FILE 			*st_err,
		FILE 		*erg_f ,
		NUZZLECONDITION   *nuzzleCondtion,
		RALCOEFFDATA *lay,		/* layout data      */
		const  flow_cr    *cr_flow, 
		const		INPUTDATAPDI *pdi);

	int PreDefineTechPara(RALINDATA *ralInPut,PHYSICAL *ver_par1);
	void ThkCrMulPara(RALINDATA *ralInPut,	PHYSICAL *ver_par1);
	void CoolSchSpeedModify	(FILE 		*erg_f ,NUZZLECONDITION   *nuzzleCondtion,RALCOEFFDATA *lay);
	float InitWaterCoolxq(    COMPUTATIONAL *compx,           // лǫ ���� ���ڱ�������Temperature
		float  *end_temp_air, /* temp. distr. air cool. */
		float  *end_aust_air, /* aost �ֽ�    */
		const	INPUTDATAPDI *pdi,
		PHYSICAL *ax,
		float detaTemp);

	void PlateHeadAirTempDrop(
		FILE          *st_err,    /* �쳣���� */
		FILE          *erg_f,     /* ��Ľ��FILE */
		INPUTDATAPDI *pdiInPut,
		COMPUTATIONAL *compx,
		PRERESULT *PreResult,       /* Ԥ������    */
		const	  RALCOEFFDATA *lay,		/* layout data      */
		PHYSICAL *ver_par1,  /* ��ȴ���� */
		MATERIAL *matal);

	void PlateTailAirTempDrop(
		FILE          *st_err,    /* �쳣���� */
		FILE          *erg_f,     /* ��Ľ��FILE */
		INPUTDATAPDI *pdiInPut,
		COMPUTATIONAL *compx,
		PRERESULT *PreResult,       /* Ԥ������    */
		const	  RALCOEFFDATA *lay,		/* layout data      */
		PHYSICAL *ver_par1,  /* ��ȴ���� */
		MATERIAL *matal);

	int ReadExpSchFromDatabasePre(FILE 		*erg_f ,unsigned long    int     keynrs[5],PHYSICAL *ver_par1);
};

static CPreProcess gPreprocess;