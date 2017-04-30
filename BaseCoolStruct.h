#pragma once

//////////////////////////////////////////////////��ȫ��ʹ�õĻ����ṹ��//////////////////////////////////////


//////////////////////////////////////////////////��������ṹ��//////////////////////////////////////
#include "BaseCoolPara.h"
	/* ���Ʋ��� for online and adaption */

///////////////////////////////////////CURVE_1�ṹ��//////////////////////////////////////
//���� ���ڲ�value
typedef struct tagCurve {
	int     n;  /* number of points per curve */
	/* first value on index 0 : last - n-1*/
	int	  iabl; /* no. of derivative	*/
	float   x_min; /* ��С������*/
	float   x_max; /* ��� ������*/
	float   y_min; /* ��С������*/
	float   y_max; /* ��� ������*/
	float   c_id; /* ���߱�ʾ�� */
	float   x[150]; /* ������value            */
	float   y[150]; /* ������value            */
	float   yp1[150]; /* һ�׵��� */
	float   yp2[150]; /* ���׵��� */
}CURVE;




// add [1/13/2015 qian]
typedef struct SelfLearnPara{
	float	SLP_Path;
	int		SLP_CM;
	float	SLP_WT;
	float	SLP_RT;
	float	SLP_T;
	float	SLP_C;
	float	SLP_CR;
	float	SLP_Nb;
	float	SLP_AP;
	int		SLP_Seq;
}SLP;

//����ģ�ͼ���Ŀ��Ʋ����ṹ��
typedef struct tagModelControl{
		/* ����ģʽ�Ŀ��Ʋ��� */
		int		bSmoothTempEntry;	/* ƽ�� entry Temperature */
		int		bSmoothTempExit;	/* ƽ�� exit  Temperature */
		int		bCorTempAct;		/* ���� Temperature ƫ�� Entry from PDI */
		int		bCorSpdAct;			/* ���� �ٶ� profile by measured �ٶ�value */
		float	fCorDevSpd;			/* allowed �ٶ� ƫ�� for ���� ion [% * 0.01] */
		int		bCorSpdSynchro;		/* additional ����ion by synchronization points */
		float	fCorTimeSynchro;	/* time window passing synchronisation points [sec] */
		float	fCorPosSynchro;		/* position window synchronization [m] */
		int     iBlsSynchro;        /* �ο� synchronization point (0: BLS1, 1: HMD) */
		float	fGainDevSpd;		/* gain for �ٶ� ���� ion */
		float	fGainDevTemp;		/* gain for entry Temperature ƫ�� */
		float	fDistSafety;		/* safety distance ͷ�� to first header [m] */
		/* ���Ʋ��� adaption (PreCalc + PostCalc) */
		int		iAdaptLevel;		/* use adaption level 1 to iAdaptLevel */
		int		nAdaptPlates;		/* No. of plates for adaption */
		int		bAdaptHeadRead;		/* ��Ӧ ͷ�� �ڱ� read ��value */
		int		bAdaptTailRead;		/* ��Ӧ β�� �ڱ� read ��value */
		int		bAdaptHeadWrite;	/* ��Ӧ ͷ�� �ڱ� write ��value */
		int		bAdaptTailWrite;	/* ��Ӧ β�� �ڱ� write ��value */
		float	fAdaptDifTime;		/* differential time use higher adaption level [hr] */
		float	fAdaptLimitMin;		/* ��С����Ӧ���� */
		float	fAdaptLimitMax;		/* ��� ����Ӧ���� */
		float	fAdaptStepSize;		/* ��� step size from one piece to next */
		float	fAdaptStepSizeTmp;	/* ��� step size from one piece to next Temperature */
		float	fAdaptLimitHeadMin; /* ��С����Ӧ���� ͷ�� */
		float	fAdaptLimitHeadMax; /* ��� ����Ӧ���� ͷ�� */
		float	fAdaptLimitTailMin; /* ��С����Ӧ���� β�� */
		float	fAdaptLimitTailMax; /* ��� ����Ӧ���� β�� */
		float	fAdaptAddFlwHead;	/* additional flow allowed on ͷ�� [% * 0.01] */
		float	fAdaptAddLenHead;	/* ��� allowed ͷ�� length ����ion [m] */
		float	fAdaptAddFlwTail;	/* additional flow allowed on β�� [% * 0.01] */
		float	fAdaptAddLenTail;	/* ��� allowed β�� length ����ion [m] */
		/* contractual ���� */
		float	fMinMaxTempEntry;	/* allowed min-max Temperature difference Entry [K] */	//���Temperatureƫ��
		float	fMinMaxTempExit;	/* allowed min-max Temperature difference Exit  [K] */
		float	fFacDevTemp;		/* multiplier for additional Temperature ƫ�� Entry */
		float	fAdaptDevSpd;		/* ��� allowed �ٶ� ƫ�� [% * 0.01] */
		float	fFacDevSpd;			/* multiplier for additional �ٶ� ƫ�� */
		/* read ��ȴģʽ ���� */
		/* read ������Ϣ from ��ȴģʽ */
		int		iDimFlw;            /* 1: ltr/(m^2*min) {specific} 2: ltr/min (unit) */ //ˮ���ܶ�
		int		iDimEdge;			/* 1: mm 2: m */				//�߲��ڱ�
		/* lengthes ͷ�� and β�� �ڱ� */
		int		iDimLenMask;		/* 1: mm 2: m */
}MODELCONTROL;

//Ԥ������
typedef struct tagPreResult 
{
	float airTempDrop;  //лǫ���� �ڽ�����ǰ��ǰ���½� 2012-5-22
	float airTailTempDrop;  //лǫ���� �ڽ�����ǰ��ǰ���½� 2012-5-21
	float  beforeAirCoolTemp;
	float  afterAirCoolTemp;

	float  crCalcLayer[10]; //лǫ���� 2012-5-22 ԭin[60] ʮλ

	float preSpeed;   //лǫ���� 2012-7-25 ���������ݿⷢ��
	float accSpeed;   //лǫ���� 2012-7-25 ���������ݿⷢ��

	float aveTmP1;  //лǫ���� 2012-8-8 �������������ݿⷢ��
	float aveTmP2u;  //лǫ���� 2012-11-5
	float aveTmP3u;
	float aveTmP3l;
	float aveTmP4;

	float aveSpCenter;  // add ɨ��������в� [1/2/2014 xie]
	float aveSpEdge;
	float aveSpEdge1;   // add [3/31/2015 лǫ]  
	
	float aveSpeed;
	float realFlowUp[NUZZLE_NUMBER+GAP_NUZZLE_NUMBER];// ��϶���ܷ������� [11/13/2013 лǫ]
	float realWaterFactor[NUZZLE_NUMBER+GAP_NUZZLE_NUMBER];// ��϶���ܷ������� [11/13/2013 лǫ]

	float realWaterPressure;
	float flowFactor;

	int isHeadFlated;  //add by xie qian �Ƿ�ѹͷ��־λ 10-16
	int isWellSelfLearned; //add by xie ��ѧϰ�Ƿ�λ 10-22

	float calcWaterFactor;   //add by xie 11-26 �洢���������ˮ�� ������������ 
	float calcTotalFlow;
	float actTotalFlow;
	float frame1Gap;
	float frame2Gap;
	int openSideSprayNo;    // 5λ��
	float flowFactor_2;   //add by xie 11-26
	float flowFactor_3;
	int relaxTime;  //add by xie 12-3
	char plateID[32];
	char MatId[32];
	char slabGrade[32];
	float plateThick;
	float plateWidth;
	float plateLength;
	float finishRollTemperature;
	float finishRollTargetTemperature; // Ŀ�������¶� [8/10/2013 лǫ]
	float finishCoolTargetTemperature;  // Ŀ�������¶� [8/10/2013 лǫ]
	float coolRate;
	int coolTypeMode;   // acc ufc DQ [8/10/2013 лǫ] 
	char CcoolType[2]; //"only last pass 	A-no ACC, 		W(default)-ACC, 		U-UFC, 
	//	E-ACC+UFC, 		D-UFC+ACC, 		Q-DQ, 		N-other, no limit, NEU will decide how to use water"  //  [11/21/2013 лǫ]
	int coolModeFlg;
	int operateMode;   // �ֶ� ���Զ� ȫ�Զ� [8/10/2013 лǫ]
	int coolThroughMethod;  //ͨ��ģʽ ͨ��ʱ ����ʽ �ڶ�ʽ   //  [11/21/2013 лǫ]
	int coolOpenMode;  //���ܿ���ģʽ 0 = front ACC 1 = back ACC		2 = two part ACC		3 = sparseϡ��ACC(2 vs 2)		4 = front UFC		5 = back UFC
		//6 = two part UFC		7 = sparseϡ��UFC(2 vs 2)		8 = DQ cooling mode		9 =  ����ģʽ  //  [11/21/2013 лǫ]
	
	float alloyC;
	int alloyExponent; // ̼���� [8/10/2013 лǫ]
	float alloyCr;
	float alloyCu;
	float alloyMn;
	float alloyMo;
	float alloyNi;
	float alloySi;
	float alloyTi;
	float alloyNb;
	float alloyB;
	float alloyV;
	float upNuzzleNumber[NUZZLE_NUMBER]; //���ܿ���״̬
	float downNuzzleNumber[NUZZLE_NUMBER];

	float upNuzzleFlow[NUZZLE_NUMBER];
	float downNuzzleFlow[NUZZLE_NUMBER];
	int  edgeMask[NUZZLE_NUMBER];  //лǫ�޸� 2012-6-23 from float to int
	int   waterOne[10];
	int   airOne[10];			//�ô�����

	// �ⲿ����ʱû��ʵ���ô� [8/10/2013 лǫ]
	float LengthMaskHeadTop;
	float LengthMaskHeadBtm;
	float flowMaskHeadTop;
	float flowMaskHeadBtm;
	int nuzzleMaskHeadTop;
	int nuzzleMaskHeadBtm;
	float LengthMaskTailTop;
	float LengthMaskTailBtm;
	float flowMaskTailTop;
	float fFlwMaskTailBtm;
	int nuzzleMaskTailTop;//лǫ�޸� 2012-6-23 from float to int
	int nuzzleMaskTailBtm;//лǫ�޸� 2012-6-23 from float to int

	int preLevelerMode;
	float tempAir;
	float tempWater;
	float headerModifyNumber;
	float coeCoolRate;
	int iHeaderFirst;
	int iHeaderLast;
	int headerNo;
	int iPyroBit;
	int iNonUseAdapt;

	float tempFinishCoolCalc;

	float tFinishCoolCalc;
	int nStroke;
	float speedEXP;
	float acclerEXP;
	float preLeverVMin;
	float preLeverVMax;
	float hotLeverVMin;
	float hotLeverVMax;

	float IcColTime;
	float IcColTimeAir;
	float fTimeAirIC;
	float fTimeDwellWatIC;
	float fTimeDwellAirIC;
	float fLengthBreak;

	float tcHead;
	float tcTail;
	float vPred;
	float coolBankLen;
	float coolWidth;

	float activeCoolZoneLen;
	float coolRateWaterProc;
	float coolRateAir;
	int writeAdapt;
	float tempAccFinishCoolCalc;
	float WatPress;

	int iStatusTempTrack;
	float lenActiveCoolZone;
	float timeAirMid;
	float timeAirMidTail;
	float coolTime;
	float deltaTimeTailAcc;
	float airMidCoolRate;
	
	float alphaCoe;
	float selfLearnAlphaCoe;

	float  airEndTemp[NODES_NUMBER]; //  temp end air dist.  лǫ ���� 2012-5-16 ��������ĳ�ʼTemperature
	float  airEndAust[NODES_NUMBER]; //    temp end air dist.  

	pwr_tChar                            FrtTime[16];
	pwr_tChar                            FctTime[16];

	float readSpeed;    // add ��ȡ�ļ������ [11/19/2013 лǫ]
	float readFlowUpLimit;
	float readFlowDnLimit;
	float readWaterFactor;
	float readWaterFactorMod; // add [4/16/2014 лǫ]

	float readWaterFactorSl;//  [9/18/2014 qian]

	float readFlowUpLimitUFC;
	float readFlowDnLimitUFC;
	float readAceSpeed; 
	int readWidShield;
	int readDetaWidShield;

	float readSideFlowFactor;// add [12/25/2014 лǫ]

	float readCoeAlpha;
	char cSideSpray[20]; // add [11/30/2013 лǫ]

	float waterPressure2;
	float frameHeight1;
	float frameHeight2;
	float shieldPos1;
	float shieldPos2;
	float shieldPos3;
	float flowdenC;
	float flowdenL;

	float realTotalFlow;  // add [12/31/2013 xie]
	float aveHC;
	float aveCr;  // add [1/2/2014 xie]
	float scanTSelflearn;  // add �߲�����������ѧϰϵ�� [1/2/2014 xie]

	float tarEftRateT1;
	float aveEftRateT1;
	float tarEftRateT4;
	float aveEftRateT4;

	int rowNumInfile; // add [3/22/2014 xie]

	int SLPflg;// Ӧ���޼�������־ [12/16/2014 wzq]  // add [1/13/2015 qian]

	int turning; // add [1/14/2015 лǫ]

}PRERESULT;

//���������
typedef struct tagInputDataPDI{
		char    plateID[20];   /* ���ID */
		char    matID[PATH_MAX];     /* ���ϱ�ʾ          */
		char    slabGrade[50];     /* ���ϱ�ʾ          */
		float   thick;      /* ��ĺ��   [mm]    */
		float   width;      /*��ĵĿ��       [mm]    */
		float   length;     /* ��ĵĳ���          [mm]    */
		float   tcrThickTemp;      /* ��ĺ��   [mm]    */
		float   tcrWidthTemp;      /*��ĵĿ��       [mm]    */
		float   tcrLengthTemp;     /* ��ĵĳ���          [mm]    */
		float   tcrThickTemp2;      /* ��ĺ��   [mm]    */
		float   tcrWidthTemp2;      /*��ĵĿ��       [mm]    */
		float   tcrLengthTemp2;     /* ��ĵĳ���          [mm]    */
		float   finishRollTemp;   // �����¶� [8/10/2013 лǫ]
		float   startCoolTemp;
		float   targetFinishCoolTemp;     /* ֹͣ��ȴTemperature [Celsius]   */
		float   cr;         /* ��ȴ�ٶ�[K/s]   */

		float   ic1FinishTemp;   // ���� Temperature  [Celsius] 
		float   headUpFinishRollTemp;//����ƽ��Temperature
		float   headDownFinishRollTemp;//����ƽ��Temperature
		float   headCenterFinishRollTemp;//����ƽ��Temperature
		float   medUpFinishRollTemp;//����ƽ��Temperature
		float   medDownFinishRollTemp;//����ƽ��Temperature
		float   medCenterFinishRollTemp;//����ƽ��Temperature
		float   TailUpFinishRollTemp;//����ƽ��Temperature
		float   TailDownFinishRollTemp;//����ƽ��Temperature
		float   TailCenterFinishRollTemp;//����ƽ��Temperature
		float   ic1StartTemp;
		float   ic1StopTemp;     /* ֹͣ��ȴTemperature [Celsius]   */
		float   ic1CoolRate;         /* ��ȴ�ٶ�[K/s]   */

		float   ic2FinishRollTemp;   /* ���� Temperature  [Celsius]   */
		float   ic2FinishRollTempCenter;
		float   ic2FinishRollTempSur;
		float   ic2StartTemp;
		float   ic2StopTemp;     /* ֹͣ��ȴTemperature [Celsius]   */
		float   ic2CoolRate;         /* ��ȴ�ٶ�[K/s]   */
		
		float   t_stage1_finish;//Ŀ�꿪��
		float   stage1_cr;
		float   stage2_cr;
	    int     IdTsk;//�ֶ�����ְ�(1);����Ԥ�趨(2);�������Gost (3);time�������� (4);ɾ��PDI(5)
	   
		char	 PltIcFlg[1];//0:����1:IC1 ˮ��2:IC2 ˮ��3:IC1 and IC2ˮ��
		int     PltIcFlgTemp;   ////0:����1:IC1 ˮ��2:IC2 ˮ��   // ֻ�� 0��1,2 ����time request [9/23/2013 лǫ]

		float  PltDlyTimRol;//��ǰ��ԥtime
		float  PltDlyTimCol;//���׶��г�ԥtime
		int	  PltHprActFlg;//Ԥ��ֱ��ǰ���׼����־
		int	  PltPlActFlg;//Ԥ��ֱ�������־
		float	PltPplTimHead;    //�ְ�ͷ����ֱtime
		float	PltPplSpdHead;    //�ְ�ͷ����ֱ�ٶ�		
		float	PltPplLenHead;    //�ְ�ͷ����ֱ����
		float	PltPplTimBody;    //�ְ�����ֱtime
		float	PltPplSpdBody;    //�ְ�����ֱ�ٶ�
		float	PltPplLenBody;    //�ְ�����ֱ����
		float	PltPplTimTail;    //�ְ�β����ֱtime
		float	PltPplSpdTail;    //�ְ�β����ֱ�ٶ�
		float	PltPplLenTail;    //�ְ�β����ֱ����

		int     priority;       /* 1: Temp, 2: CR 3: �ٶ� 0: �ٶ� */
		int	  iSimMode;	/* 1: simulation mode 2: ���� mode */
		int     acc_mode;   /* cooling mode             */
		int     op_mode;  
		int     coolStatus; //IC, ACC 
				
		char    file_nam[PATH_MAX];
		float   alloy_equC;
		float   alloy_c;        /* �Ͻ� carbon         */
		float   alloy_cr;   /* �Ͻ� crom           */
		float   alloy_cu;   /* �Ͻ� copper         */
		float   alloy_mn;   /* �Ͻ� mangan         */
		float   alloy_mo;   /* �Ͻ� molybdaen      */
		float   alloy_ni;   /* �Ͻ� nickel         */
		float   alloy_si;   /* �Ͻ� silicium       */
		float   alloy_ti;   /* �Ͻ� titanium       */
		float   alloy_nb;   /* �Ͻ� niobium        */
		float   alloy_b;       /* �Ͻ� boron       */
		float   alloy_v;    /* �Ͻ� vanadium     */
		int     pl_mode;        /* pre-levelling mode           */
		int     taskID;     /* 1: PREPROCESS 2: POSTPROCESS     */
		float    v_min_PL;  /* ��Сspeed Ԥ��ֱ�� [m/s]  */
		float    v_max_PL;  /* ��� �ٶ� Ԥ��ֱ�� [m/s]  */
		float    v_min_HL;  /* ��Сspeed �Ƚ�ֱ�� [m/s]  */
		float    v_max_HL;  /* ��� �ٶ� �Ƚ�ֱ�� [m/s]  */
		int		iPat;	   /* pattern: 0: use compact 1: distrtributed if CR requires */
		int		iDirLastPass; /* -1: to furnace 1: direction Acc */
		int		iHeaderFirst; /* index of first active Header if iDirLastPass = 1*/
		float    eval_pyro;
		float    startTempDistribution[NODES_NUMBER];     /* ��ʼ���� Temperature (option) [��] */
		float    aust_start[NODES_NUMBER];  /* ��ʼ���� austenit    (option) [%]  */
		float    grain_size[NODES_NUMBER];  /* ��ʼ���� grain size  (option)[mue] */
		float    IcColTime;  
		float	IcColTimeAir;
		int      FlowRate;   
		MODELCONTROL	controlAcc;	   /* ���Ʋ��� Acc */

		float   SpeedEXP;
    	float   AcclerEXP;
		int     pass_mode;
		int     open_type;
		int     activeFirstJet;
		int     activeSecType;
			//changed by wbxang 20120506

		float initFinishRollingTem;   //лǫ���� ���ڱ���ԭ�е�����Temperature
		int  working_stage;     //лǫ���� ���ڼ�¼��ǰˮ���ܶȼ����ж���� Ӧ��������
		int isHeadFlated;  //add by xie qian �Ƿ�ѹͷ��־λ 10-16
		float manualHC;

		float waterTemperature;  // add [1/13/2015 qian]


	}INPUTDATAPDI;

//���ϲ���
typedef struct  tagMaterialParameters
{
	int   matn; /* ��������     */
	int   matno_lam; /* ���ϱ�ʾ for lambda-curve    */
	int   matno_rc;  /* ���ϱ�ʾ for rho * cp curve  */
	float ar1;  //ar1 ����
	float ar3;  //ar3 ����
	char  datname[50]; /* name of FILE with cp-value  */
	CURVE austenitTransPercent;  /* ������ֽ� %*/
	float lambda[MAXTEMP+1];    //heat conductivity ����
	float rhoCp[MAXTEMP+1];    /*���� of heat capacity  */
	float austenitEnthalpy[MAXTEMP+1];    // austenit enthalpy austenitEnthalpy
	float rhoCpg[MAXTEMP+1]; // heat cap.*rho AUSTENIT
	float ferritEnthalpy[MAXTEMP+1];    /* ferrit enthalpy */
	float rhoCpa[MAXTEMP+1]; /* heat cap.*rho FERRIT */
	float trans[MAXTEMP+1];   /* ������ֽ� */
	float trans_p[MAXTEMP+1]; /* һ�׵��� aust. dec. */
	char  rho_dat[PATH_MAX];  /* �ļ��� density */
	char  cp_dat[PATH_MAX];  /* �ļ��� heat capacity */
	char  lam_dat[PATH_MAX];  /* �ļ��� lambda */
	char  air_dat[PATH_MAX];  /* �ļ��� alpha air-cool */
	char  wat_dat[PATH_MAX];  /* �ļ��� alpha wat-cool */
}MATERIALPARAM;

	/*   3. ���� alpha-value structure     */
	//���ϵ��
typedef struct tagAlphaValues
{
	int   alphan;   /* ��������     */
	float top[MAXTEMP+1];   /* alpha value on top-side     */
	float bot[MAXTEMP+1];   /* alpha value on bottom-side  */
}ALPHAVALUES;


/*  structure for alpha , lambda and rho * cp value    */
//����+alpha
typedef struct tagMaterlal
{
	MATERIALPARAM  para;
	ALPHAVALUES    alpha;
} MATERIAL;

/*   4. ���� computational ����  */
	//���̲��� ģ�ͼ�������Ĳ���
typedef	struct tagComputational  //verfahren ���� ���̵���˼
{
	int   computeID;     // distribution identifier 0= normal    1 = only one ����Temperature  
	int  femCalculationOutput;        // output of value during fem-calculation
	int   meanCalculationValue;        // ��� value between two outputs 
	float calculationStopTime;      // ����೤ʱ�� stop calculation after calculationStopTime sec.   outp.) time at targetFinishCoolTemp        */
	float calculationStopTemp;       /* stop calculation after targetFinishCoolTemp K  */
	/* outp.) mean caloric Temperature  */
	/* if calculationStopTime > 0           */
	int testParameter;      /* test paprameter          */
	float   detaTime;     /* time increment for solution      */
	float calculationTotalTime;         //  ʵ����ȴ���̵���ʱ�� time over whole cooling sequence 
	float localCoolRate;              // local ��ȴ�ٶ� last int. point 
	int     integra;    /* parameter for solution chema in time integration algorithem       */
	int thickNodeNumber;      // number of thickNodeNumber in thickness dir. 
	int matrixElementNumber;     /* number of matrix elements        */
	int isSymetric;       // == 0 : non-symetric problem    == 1 : symmetriccal problem    
	float rhoCpMeanValue;      /* ƽ��value of rho_cp over thickness */
	float austenitePercentMean;           /* ƽ��value of ������ֽ� */
	float austenitePercentMeanNextStep;           // ƽ��value forgoing time step
	float latentHeat;               // latent heat 
	float startTempDistribution[NODES_NUMBER];        /* Temperature distribution (20 thickNodeNumber)  */
	/* outp.) last temp. distribution   */
	/* at calculationStopTime / targetFinishCoolTemp          */
	float startAusteniteDistribution[NODES_NUMBER];    /* distribution of ������ֽ� */
	float TimeKAusteniteDistribution[NODES_NUMBER];    /* distribution of ������ֽ� time step k*/
	float TimeKm1AusteniteDistribution[NODES_NUMBER];    /* distribution of ������ֽ�  k-1*/
	float TimeKpredAusteniteDistribution[NODES_NUMBER];
	float TimeKRhoCp[NODES_NUMBER];     /* rho*cp- value in time step k */
	float TimeKm1RhoCp[NODES_NUMBER];     /* rho*cp- value in time step k-1*/
	float TimeKpredRhoCp[NODES_NUMBER];  /* rho*cp- value in predictor step */
	float TimeKEnthalpy[NODES_NUMBER];            /* enthalpy in time step k */
	float relasedEnergy[NODES_NUMBER];          /* released energy in time Step */
	float elementLength[NODES_NUMBER];           /* length of the elements       */
	float outputParameter[10];        // computational ���� for output

	int     computedNumber;         /* number of computed value < TEMPERATURE_CURVE_NUMBER      */
	float   zx[TEMPERATURE_CURVE_NUMBER];        /* time value last cooling curve       */
	float   tx[TEMPERATURE_CURVE_NUMBER];        /* caloric mean Temperature value last curve       */
	float   tx_cen[TEMPERATURE_CURVE_NUMBER];    /* Temperature in the center line of ���          */
	float   tx_top[TEMPERATURE_CURVE_NUMBER];    /* surface top Temperature value last curve        */
	float   tx_bot[TEMPERATURE_CURVE_NUMBER];    /* surface bottom temp. value last curve           */
	float   cx[TEMPERATURE_CURVE_NUMBER];        /* ��ȴ�ٶ�in C                                */

	//лǫ���� ������ 2012-4-24 �������㴦 ��ʱ����Ҫ ֪�����е�ʱ�̵�Temperature��

	int     gapComputedNumber;         /* number of computed value < TEMPERATURE_CURVE_NUMBER      */

	float   zx_temple[4*TEMPERATURE_CURVE_NUMBER];        /* time value last cooling curve       */
	float   tx_temple[4*TEMPERATURE_CURVE_NUMBER];        /* caloric mean Temperature value last curve       */
	float   tx_cen_temple[4*TEMPERATURE_CURVE_NUMBER];    /* Temperature in the center line of ���          */
	float   tx_top_temple[4*TEMPERATURE_CURVE_NUMBER];    /* surface top Temperature value last curve        */
	float   tx_bot_temple[4*TEMPERATURE_CURVE_NUMBER];    /* surface bottom temp. value last curve           */
	float   cx_temple[4*TEMPERATURE_CURVE_NUMBER];        /* ��ȴ�ٶ�in C                                */
	int activeSlitNumber;  
				
}COMPUTATIONAL;          /* calculationStopTime or targetFinishCoolTemp equal zero  -/+ are possible   */

/*   5. ���� physical ����   */
//���Բ���
typedef struct tagPhysical
{
	int taskID;     // 1: Ԥ���� 2: �����   
	int mat_id;     /* ���ϱ�ʾ          */
	char datname[PATH_MAX];    /* name of cp-FILE          */
	float   speed;      /* �ٶ� of the ��� from furnace to   */
	/* active ��ȴ��          */
	int method;     // 1 : ���� version      
	/* 2 : simulation           */
	int isWriteAdaptValue;    // 0: write ����Ӧvalue for staistic 1: write adaption for later use 
	
	unsigned long int        adaptKey[5]; // adaption key

	float  adaptRatioFound; // ����Ӧvalue found     adaptRatioFound

	int   levelNumber;      // level no. for adaption

	int  furnaceTemp;  /* output Temperature from furnace  */
	int maxPossibleMillTemp;     /* ��� possible mill Temperature    */

	float topFlowRate;     /* flow rate top    [l/(min * m**2)]    */
	float bottomFlowRate;     /* flow rate bottom [l/(min * m**2)]    */
		
	float waterTemp;   /* temp. of ˮ�� [Celsius]     */
	float waterPressure;   /* temp. of ˮ�� [Celsius]     */

	float t_umg;        /* Temperature of air [Celsius]     */
	int   sw_air;       /* switch (0:air 1:water 2:spray)   */
	int   sw_mat;     /* 1: steel 2: other materials */
	int coolPhaseNumber;     /* parameter for different cooling */
	/* phases (from 0 to max. 9)        */

	float actualStopTime;    /* stop time for ʵ�� ��ȴ��        */
	float actualStartTemp;  /* start Temperature active ��ȴ��  */
	float   duration[10];  /* duration of cooling phase        */
	int art[10];       /* type of cooling phase        */
	float alpha_coe;
	float waterFactorCoe;  //лǫ ���� 2012-7-16 ˮ������ϵ��

	//add by wbxang 20120505
	float   SlitFourCrMax;
	float   SlitFourCrMin;
	float   SlitTwoCrMax;
	float   SlitTwoCrMin;

	float   JetSucCrMax;
	float   JetSucCrMin;
	float   JetSepCrMax;
	float   JetSepCrMin;

	float  SLP_Found;  // add [1/13/2015 qian]
}PHYSICAL;



	/* structure for adaptation */
	/* iControl set in adaption FILE, if not found iControl = 2 is in use */
	/* = Ĭ��value */
	/* iControl = 0: don't write adapted value AND don't read */
	/* iControl = 1: don't write adapted value AND read value for use */
	/*               in PRECALC */  
	/* iControl = 2: write adapted value AND read (default) */
	/* iControl = 3; write only for learning, do not read */
	//����Ӧ����
typedef struct tagAdapt{
	int			iFoundRead;				/* found adaptation key during read */
	int			iFoundWrite;			/* adaptation key already exist */
	int			max_plates;				/* ��� ��ļ�¼����*/
	int			num_plates;				/* total adapted plates for class   */
	int			rec_plates;				/* ��ļ�¼���� */
	int			iControl;				/* ����Ӧ����ģʽ */
	//add by wbxang 20120516
	unsigned long int adap_key[5];  	    /* Adaption_Key + Level */
	long int    iAdaptTimeLast;			/* last adaption time */
	long int    iAdaptTimeDiff;			/* time difference last ��� [s] */
	char		plate_id_m1[MAX_ID];	/* last adapted ��� ID */
	char		plate_id_m2[MAX_ID];	/* -2 adapted ��� ID */
	char		plate_id_m3[MAX_ID];	/* -3 adapted ��� ID */
	float       adaptRatioFound;				/* ����Ӧvalue��ȴ�ٶ�*/
	float		tr_foundMean;			/* ����ӦvalueTemperature Mean */
	float		tr_foundHead;			/* ����ӦvalueTemperature ͷ�� */
	float		tr_foundTail;			/* ����ӦvalueTemperature β�� */
	float		vx_last;				/* �ٶ� of last ��� (mean)  */
	float		lx_last;				/* �ͷ������ */
	float		tf_last;				/* ��� Temperature */
	float		ts_last;				/* last ֹͣ��ȴtemp */
	float		tw_last;				/* last Temperature ˮ�� */
	long int	iSpare[3];				/* Ԥ�� ����Ӧvalue */
	float		fSpare[3];				/* Ԥ�� ����Ӧvalue */
	char		sPatternID[PATH_MAX];	/* ID ��ȴģʽ */
	char		sFileAdapt[PATH_MAX];	/* FILE / ���� */
}ADAPTATION;


/* cooling ���Բ��� */
/* refer to STRATEGY .doc for documentation */
//ACC������ȴ���Խ��
typedef struct tagAccStrategy_t{
	int			iHeaderDir;		/* -1: header activated beginning last header */
	int			iZoneAct[ANZ_SECT];	/* 1: use ��ȴ�� */
	int			nStroke;		/* No. of allowed strokes */
	int			iDirStroke;		/* 1: odd No. of strokes 2: even No. of strokes */
	int         iModStroke;     /* 1: modification number of strokes during online allowed */
	int			iTransferMode[3]; /* ��� transfer modes priority */
	float		fLengthBreakStr;	/* additional breaking length in case of */
	/* oszillations */
	int			iEdgeMask;		/* -1: reset edge �ڱ� to zero */
	int			iHeaderFirst;	/* No. of preferred first header */
	/* header sequence active, deactivated, active, ... */
	int			nHeaderAct;		/* preferred No. of   activated headers */
	int			nHeaderActNon;	/* preferred No. of deactivated headers */
	int			nHeaderActPref;	/* No. of preferre active headers */
	int			nHeaderActMax;	/* Max. No. of active headers */
	int			iFlwReg;		/* flow regime: 1: constant all headers */
	int			iFlwDist;		/* flow distribution */
	float		fPosHeaderRef;	/* rel. �ο� position flow header */
	float		fFlwHeaderRef;	/* rel. �ο� flow on fPosHeader */
	float		fFlwHeaderLast;	/* rel. �ο� flow on last header */
	int			iTempStopPos;	/* 1: Surface, 2: Mean 3: Center */
	float		fTempStopDef;	/* Ĭ��value stop Temperature (-1: Ar3) */
	float		fCrDef;			/* default ��ȴ�ٶ�*/
	float		fCorSpd;		/* ����ion value �ٶ� */
	float		fCorNuHeader;	/* ����ion value number of headers */
	float		fCorNuStrokes;	/* ����ion value number of strokes */
	int			nThick;			/* No. of thickness value */
	float		fThick[NTHK];	/* thickness value */
	float		fSpdPref[NTHK];	/* preferred �ٶ�value */
	float		fSpdMin[NTHK];	/* ��С�ٶ�value */
	float		fSpdMax[NTHK];	/* ��� �ٶ�value */
	float		fTempDropWater[NTHK];/* required Temperature drop in water (1: 100%) */
	float		fFlwMin[NTHK];	/* ��Сflow value */
	float		fFlwMax[NTHK];	/* ��� flow value */
	/* above Temperature difference (T_start - T_stop) flow limits will be applied */
	float		fFlwTmp[NTHK];
}STRATEGY;

//���ٽṹ��  L1����������
typedef struct tagTracking
{
		float TimeHeadRef;  /* �ο� time ��� ͷ�� */
		float TimeHeadRt;   /* time ���� ͷ�� */
		float TimeHeadPlt;  /* time ��� ͷ�� */
		float TimeTailRef; /* �ο� time ��� β�� */    
		float TimeTailRt;  /* time ���� β�� */
		float TimeTailPlt; /* time ��� β�� */
		/* �ο� value �ٶ� */
		float fSpdRefMean;	
		float fSpdRefMin;	
		float fSpdRefMax;	
		/* ʵ�� �ٶ� ��Ϣ */
		float fSpdActMean;	
		float fSpdActMin;	
		float fSpdActMax;	
		/* �ٶ� ƫ��  */
		float fSpdDevMean;	
		float fSpdDevMin;	
		float fSpdDevMax;	
		int   iBlsEntry;    /* sync start Bls depend on Acc-length, speed-profile, valid mea data*/
		int   iBlsExit;     /* sync start Bls depend on Acc-length, speed-profile, valid mea data*/
}TRACKING;

///////////////////////////////////////����ṹ��////////////////////////////////////

//Ԥ������� 
typedef struct tagOutputData
{
  pwr_tBoolean                        nuzzleStatusTop[NUZZLE_NUMBER];
  pwr_tBoolean                        nuzzleStatusBottom[NUZZLE_NUMBER];
  pwr_tBoolean                        headerWorkingStatus[NUZZLE_NUMBER];   //лǫ ���� 2012-4-28 ���ܿ���״̬
  pwr_tFloat32                        waterFactor[NUZZLE_NUMBER];   //лǫ ���� 2012-4-28  ˮ��  6-23 from int to float
  pwr_tFloat32                        waterFlowTop[NUZZLE_NUMBER];    //лǫ ���� 2012-5-16 �洢ÿ�鼯�ܵ�ˮ���ܶ�
  pwr_tFloat32                        waterFlowDown[NUZZLE_NUMBER];  //лǫ ���� 2012-5-16
  pwr_tBoolean                        valve_sid[NUZZLE_NUMBER];
  pwr_tFloat32                        edgePosition[ANZ_EDGE];
  pwr_tFloat32                        speedPosition[MAX_MEASURE_VALUE];                   // лǫ �ٶ� λ�� �� ���岻�� 
  pwr_tFloat32                        speed[MAX_MEASURE_VALUE];
    
  pwr_tFloat32						  fTimeAccHead;
  pwr_tFloat32						  fTimeAccTail;
   /* interphase cooling: remaining���� time */
  pwr_tFloat32						  fTimeAirIC;
  pwr_tFloat32						  fTimeCalReqWatIC; 
  pwr_tFloat32						  fTimeCalReqAirIC; 

  ADAPTATION                          adapt;
  STRATEGY							  accStrategy;
  MODELCONTROL						  controlAcc;
  PRERESULT			                  PreResult;
  SLP								  SLP_Post;   // add [1/13/2015 qian]
  
}RALOUTDATA;


//��MES�Ĺ��ղ�����Ϣ
typedef struct tagMesData{
	
	pwr_tChar                           plateID[50];
	pwr_tChar                           slabID[50]; 

	pwr_tFloat32                        Thickness;
	pwr_tInt32                           CurIndex;
	pwr_tFloat32                        fLengthSeg; /* segment length */
	pwr_tInt32                           CurPass;
	pwr_tFloat32                        TimeSinceP1[MAX_MEASURE_VALUE];
	pwr_tFloat32                        Speed[MAX_MEASURE_VALUE];
	pwr_tFloat32                        Position[MAX_MEASURE_VALUE];
	
	int     TempTH1[MAX_MEASURE_VALUE];
	int     TempTH2[MAX_MEASURE_VALUE];
	int     TempTH3[MAX_MEASURE_VALUE];

	pwr_tFloat32                        TempP1[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempP2L[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempP2U[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempP23[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempP3L[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempP3U[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempP4[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempP5[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempP6[MAX_MEASURE_VALUE];
	
	pwr_tFloat32                        TempSp4[MAX_MEASURE_VALUE];  // add [1/2/2014 xie]
	pwr_tFloat32                        TempSp5[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempSp7[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempSp8[MAX_MEASURE_VALUE];

	pwr_tFloat32                        TempSp1[MAX_MEASURE_VALUE];  // add [21/2/2014 xie]
	pwr_tFloat32                        TempSp2[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempSp3[MAX_MEASURE_VALUE];
	pwr_tFloat32                        TempSp6[MAX_MEASURE_VALUE];

	

	pwr_tFloat32                        FlowZ1Top[MAX_MEASURE_VALUE];  // �漰CopyInfoInSSAB�������˲�����Ϣ���ܲ���Ҫ���� [8/11/2013 лǫ]
	pwr_tFloat32                        FlowZ2Top[MAX_MEASURE_VALUE];
	pwr_tFloat32                        FlowZ3Top[MAX_MEASURE_VALUE];
	pwr_tFloat32                        FlowZ1Btm[MAX_MEASURE_VALUE];
	pwr_tFloat32                        FlowZ2Btm[MAX_MEASURE_VALUE];
	pwr_tFloat32                        FlowZ3Btm[MAX_MEASURE_VALUE];
		
	
	/* time ͷ�� end of ��� passes synchronization points */
	pwr_tFloat32                        fTimeBlsHead[ANZ_HUB][ANZ_BLS];
	/* time β�� end of ��� passes synchronization points */
	pwr_tFloat32                        fTimeBlsTail[ANZ_HUB][ANZ_BLS];
	pwr_tInt32                          IndPupP1;
	pwr_tInt32                          IndDropP1;
	pwr_tInt32                          IndPupP2U;
	pwr_tInt32                          IndDropP2U;
	pwr_tInt32                          IndPupP3U;
	pwr_tInt32                          IndDropP3U;

	pwr_tInt32                          IndPupP4;
	pwr_tInt32                          IndDropP4;
	pwr_tInt32                          IndPupP5;
	pwr_tInt32                          IndDropP5;
	pwr_tInt32                          IndPupP23;
	pwr_tInt32                          IndDropP23;
	pwr_tInt32                          IndPupP2L;
	pwr_tInt32                          IndDropP2L;
	pwr_tInt32                          IndPupP3L;
	pwr_tInt32                          IndDropP3L;
	pwr_tInt32                          IndPupHL;
	pwr_tInt32                          IndDropHL;
	pwr_tFloat32                        TempSurfCalcStartMin;
	pwr_tFloat32                        TempSurfCalcStartMax;
	pwr_tFloat32                        TempSurfCalcStartAve;

	pwr_tFloat32                        TempCentCalcStartMin;
	pwr_tFloat32                        TempCentCalcStartMax;
	pwr_tFloat32                        TempCentCalcStartAve;

	pwr_tFloat32                        TempSurfMeasStartMin;
	pwr_tFloat32                        TempSurfMeasStartMax;
	pwr_tFloat32                        TempSurfMeasStartAve;
	
	pwr_tFloat32                        TempSurfCalcStopMin;
	pwr_tFloat32                        TempSurfCalcStopMax;
	pwr_tFloat32                        TempSurfCalcStopAve;

	pwr_tFloat32                        TempCentCalcStopMin;
	pwr_tFloat32                        TempCentCalcStopMax; 
	pwr_tFloat32                        TempCentCalcStopAve;
	pwr_tFloat32                        TempSurfMeasStopMin; 
	pwr_tFloat32                        TempSurfMeasStopMax;
	pwr_tFloat32                        TempSurfMeasStopAve;
	pwr_tFloat32                        TempSurfMeasStopStd; 
	pwr_tFloat32                        CoolRateSurfCalcMin;
	pwr_tFloat32                        CoolRateSurfCalcMax;
	pwr_tFloat32                        CoolRateSurfCalcAve;


	pwr_tFloat32                        CoolRateCentCalcAve;

	pwr_tFloat32                        TempAveCalcStartAve;
	pwr_tFloat32                        TempAveCalcStopAve;
	pwr_tFloat32                        TempAveCalcStopMin;
	pwr_tFloat32                        TempAveCalcStopMax;
	pwr_tFloat32                        CoolRateAveCalcAve;
	pwr_tFloat32                        CoolRateAveCalcMin;
	pwr_tFloat32                        CoolRateAveCalcMax;	
	pwr_tFloat32                        AveFlow[3];
	pwr_tFloat32                        AveSpeed;

	pwr_tFloat32                        SpeedEXP;
	pwr_tFloat32                        AcclerEXP;

	pwr_tInt32                          NCoolBank;

	pwr_tFloat32                        CoolCoeff[16];

	pwr_tFloat32                        P1uAve;
	pwr_tFloat32                        P2uAve;
	pwr_tFloat32                        P2lAve;
	pwr_tFloat32                        P3bAve;
	pwr_tFloat32                        P3lAve;
	pwr_tFloat32                        P23Ave;

	pwr_tFloat32                        InitSpeed;
	pwr_tFloat32                        ExitSpeed;

	pwr_tFloat32                        AveCoolTimeHead;
	pwr_tFloat32                        AveCoolTimeRoot;
	pwr_tFloat32                        AveAccCoolTimeHead;
	pwr_tFloat32                        AveAccCoolTimeRoot;

	pwr_tFloat32                        SetupFlowTop[3];
	pwr_tFloat32                        SetupFlowBtm[3];


	pwr_tFloat32                        StartFlowTop[3];
	pwr_tFloat32                        StartFlowBtm[3];

	pwr_tFloat32                        SetPtFlowTop[NFLOW];  /* Voest �ο� Specific Flow*/
	pwr_tFloat32                        SetPtFlowBtm[NFLOW];  /* for each cooling header*/
	pwr_tInt32                          SegCount;                       
	pwr_tFloat32                        CoolTempColStartCalSeg[NFLOW];  
	pwr_tFloat32                        CoolTempColStopCalSeg[NFLOW];   
	pwr_tFloat32                        CoolTempColStopCalSegMean; 
	pwr_tFloat32                        CoolTempColStopCalMinSeg;    
	pwr_tFloat32                        CoolTempColStopCalMaxSeg;   
	pwr_tFloat32                        CoolRateAvgCalSeg[NFLOW]; 
	pwr_tFloat32                        CoolRateAvgCalSegMean;  
	pwr_tFloat32                        CoolRateAvgCalMinSeg;  
	pwr_tFloat32                        CoolRateAvgCalMaxSeg;   
	pwr_tFloat32                        CoolRateAvgCalLimitOkSeg; 
	pwr_tFloat32						fTempMeanSurfTopP6;
	pwr_tFloat32						fTempMeanSurfBtmP6;
	pwr_tFloat32						fTempMeanCalP6;
	TRACKING    Tracking[ANZ_HUB]; 

}RALMESDATA;


//����
typedef struct tagInputDataRAL{


	int isOnlineCalc; //add by xie qian  �Ƿ����߼���ı�־λ
	int isCooling; //add by xie qian  �Ƿ���ȴ�ı�־λ 9-11
	int isHeadFlated;  //add by xie qian �Ƿ�ѹͷ��־λ 10-17

	pwr_tFloat32                       plate_number; /* only for backward compatibility to float ID Frames (TKS) not used inside model kernel*/
	pwr_tChar                           plateID[32];
	pwr_tChar                           msg_flag[100];
	pwr_tInt32                          seq_num;
	pwr_tChar                           slabID[32];  
	pwr_tChar                           s_steel_code[32];
	pwr_tChar                           s_slab_code[32];
	pwr_tFloat32                        priority;
	pwr_tFloat32                        thick;
	pwr_tFloat32                        width;
	pwr_tFloat32                        length;
	pwr_tFloat32                        slab_thick;
	pwr_tFloat32                        slab_width;
	pwr_tFloat32                        slab_length;
    ////////////////////////////////add by yuanda  2012.4     �м������//////////////////////////////
	pwr_tFloat32                        tcrThickTemp;
	pwr_tFloat32                        tcrWidthTemp;
	pwr_tFloat32                        tcrLengthTemp;
	pwr_tFloat32                        tcrThickTemp2;
	pwr_tFloat32                        tcrWidthTemp2;
	pwr_tFloat32                        tcrLengthTemp2;
    pwr_tInt32                          IdTsk;//�ֶ�����ְ�(1);����Ԥ�趨(2);�������Gost (3);time�������� (4);ɾ��PDI(5)
	char							    PltIcFlg[1];//0:����1:IC1 ˮ��2:IC2 ˮ��3:IC1 and IC2ˮ��
	pwr_tInt32							PltIcFlgTemp;//0:����1:IC1 ˮ��2:IC2 ˮ��3:IC1 and IC2ˮ��

	pwr_tFloat32						PltDlyTimRol;//��ǰ��ԥtime
	pwr_tFloat32						PltDlyTimCol;//���׶��г�ԥtime
	pwr_tFloat32						PltDlyTimLev;//Ԥ��ֱǰ��ԥtime

	pwr_tFloat32                        finishRollTemp;//����ƽ��Temperature
	pwr_tFloat32                        t_finish_target;//����ƽ��Temperature   //add by xie 12-3 ���빤�ձ��

	pwr_tFloat32                        headUpFinishRollTemp;//����ƽ��Temperature
	pwr_tFloat32                        headDownFinishRollTemp;//����ƽ��Temperature
	pwr_tFloat32                        headCenterFinishRollTemp;//����ƽ��Temperature
	pwr_tFloat32                        medUpFinishRollTemp;//����ƽ��Temperature
	pwr_tFloat32                        medDownFinishRollTemp;//����ƽ��Temperature
	pwr_tFloat32                        medCenterFinishRollTemp;//����ƽ��Temperature
	pwr_tFloat32                        TailUpFinishRollTemp;//����ƽ��Temperature
	pwr_tFloat32                        TailDownFinishRollTemp;//����ƽ��Temperature
	pwr_tFloat32                        TailCenterFinishRollTemp;//����ƽ��Temperature

	pwr_tFloat32                        startCoolTemp;//Ŀ�꿪��
	pwr_tFloat32                        targetFinishCoolTemp;//Ŀ������
	pwr_tFloat32                        t_red;//Ŀ������

	pwr_tFloat32                        stage1TargetFinishTemp;//һ�׶�Ŀ������  stage1TargetFinishTemp
	pwr_tFloat32                        stage1CoolRate;//һ�׶���ȴ�ٶ�
	pwr_tFloat32                        stage2CoolRate;//���׶���ȴ�ٶ�
		
	pwr_tInt32							hprActFlg;//Ԥ��ֱ��ǰ���׼����־
	pwr_tInt32							prelevelActFlg;//Ԥ��ֱ�������־
	pwr_tFloat32                        cr;
	pwr_tFloat32                        ic1FinishTemp;
	pwr_tFloat32                        ic1FinishTempSurface;
	pwr_tFloat32                        ic1FinishTempCenter;
	pwr_tFloat32                        ic1StartCoolTemp;
	pwr_tFloat32                        ic1StopCoolTemp;
	pwr_tFloat32                        ic1BackRedTemp;
	pwr_tFloat32                        ic1CoolRate;
	pwr_tFloat32                        ic2FinishRollTemp;
	pwr_tFloat32                        ic2FinishRollTempSur;
	pwr_tFloat32                        ic2FinishRollTempCenter;
	pwr_tFloat32                        ic2StartCoolTemp;
	pwr_tFloat32                        ic2StopCoolTemp;
	pwr_tFloat32                        ic2BackRedTemp;
	pwr_tFloat32                        ic2CoolRate;		

	pwr_tFloat32                        t_sec_start;//Ŀ�꿪��  ���׶�
	pwr_tFloat32                        t_sec_stop;//Ŀ������  ���׶�
	pwr_tFloat32                        sec_cr;
	
	pwr_tFloat32                        t_ic_finish_tmp;
	pwr_tFloat32                        t_acc_finish_tmp;
	pwr_tFloat32                        t_ic_stop_tmp;
	pwr_tFloat32                        t_acc_stop_tmp;

	pwr_tInt32                          acc_mode;
	pwr_tChar                           c_acc_mode[2];   //2012-6-27  ���ڽ������ݿ����Ϣ

	pwr_tInt32                          acc_request;
	pwr_tChar                           c_acc_request[2]; //2012-6-27  ���ڽ������ݿ����Ϣ

	pwr_tInt32                          ICtype;
	pwr_tInt32                          pass_mode;
	pwr_tInt32                          press_type;
	pwr_tInt32                          open_type;
	pwr_tInt32                          pass_no;

	pwr_tInt32                          op_mode;   
	pwr_tInt32                          AdaptFlg;
	pwr_tInt32                          offLineAdaptFlg;   //add by xie qian 8-18

	pwr_tInt32                          CoolStatus;
	pwr_tInt32                          retval;
	pwr_tChar                           mat_id[50];
	pwr_tChar                           me_nr[50]; 

	pwr_tFloat32	PltPplTimHead;    //�ְ�ͷ����ֱtime
	pwr_tFloat32	PltPplSpdHead;    //�ְ�ͷ����ֱ�ٶ�		
	pwr_tFloat32	PltPplLenHead;    //�ְ�ͷ����ֱ����
	pwr_tFloat32	PltPplTimBody;    //�ְ�����ֱtime
	pwr_tFloat32	PltPplSpdBody;    //�ְ�����ֱ�ٶ�
	pwr_tFloat32	PltPplLenBody;    //�ְ�����ֱ����
	pwr_tFloat32	PltPplTimTail;    //�ְ�β����ֱtime
	pwr_tFloat32	PltPplSpdTail;    //�ְ�β����ֱ�ٶ�
	pwr_tFloat32	PltPplLenTail;    //�ְ�β����ֱ����
		
	char                                filler_0[2];
	pwr_tChar                           file_nam[255];
	char                                filler_1[2];
		
	pwr_tFloat32                        alloy_c;
	pwr_tFloat32                        alloy_cr;
	pwr_tFloat32                        alloy_cu;
	pwr_tFloat32                        alloy_mn;
	pwr_tFloat32                        alloy_mo;
	pwr_tFloat32                        alloy_ni;
	pwr_tFloat32                        alloy_si;
	pwr_tFloat32                        alloy_ti;
	pwr_tFloat32                        alloy_nb;
	pwr_tFloat32                        alloy_als;		
	pwr_tFloat32                        alloy_b;
	pwr_tFloat32                        alloy_v;
	pwr_tFloat32                        alloy_p;
	pwr_tFloat32                        alloy_s;

	pwr_tFloat32                        alloy_w;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_re;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_pb;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_ca;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_n2;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_h2;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_o2;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_alt;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_ceq;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_zr;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_mg;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_sn;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_as;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_co;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_te;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_bi;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_sb;//�¼����Ԫ������ лǫ 2012-6-27
	pwr_tFloat32                        alloy_zn;//�¼����Ԫ������ лǫ 2012-6-27

	pwr_tInt32                          taskID;
	pwr_tInt32						    iSimMode;	/* 1: simulation mode 2: ���� mode */
	pwr_tInt32                          acc_dir;
	pwr_tInt32                          tcr_flg;
	pwr_tFloat32                        v_min_PL;
	pwr_tFloat32                        v_max_PL;
	pwr_tFloat32                        v_min_HL;
	pwr_tFloat32                        v_max_HL;
	pwr_tFloat32                        eval_pyro;
	pwr_tFloat32                        startTempDistribution[NODES_NUMBER];
	pwr_tFloat32                        grainSize[NODES_NUMBER];
	pwr_tFloat32                        austeniticDecompRate[NODES_NUMBER];


	pwr_tFloat32                        IcColTime;  
	
	pwr_tFloat32						IcColTimeAir;
  
	pwr_tInt32    iPat; /* pattern: 0: use compact 1: distrtributed if CR requires */
	pwr_tInt32    iDirLastPass;  /* -1: to furnace 1: direction Acc */
	pwr_tInt32    iHeaderFirst; /* No. of first active Header if iDirLastPass = 1*/
	
	pwr_tInt32	ResActId;			/**/

	pwr_tInt32							activeFirstJet;  // ����ģʽ����Ϣ [8/12/2013 лǫ]
	pwr_tInt32						    activeSecType;

	float                               manualHC;  // add[11/15/2013 лǫ]

}RALINDATA;

	////////////////////////////////////   ��ȴ�����ṹ�� //////////////////////////////////////////
typedef struct tagMDSCoolCoeff 
{
	pwr_tFloat32  initSpeed;              //Ԥ�����ʼ�ٶȣ����ݶ����ã���ϸ��
	pwr_tFloat32  CoolerLength;	       // length of ACC 	[m] 	20	  ��ȴ������
	pwr_tFloat32  CoolBankLen;	     // one header		[m]	 1	 һ�鼯�ܵĳ���
	pwr_tFloat32	NbCoolSect;		/* No. of cooling sections 3*/
	pwr_tInt32		NbBankSect[ANZ_SECT];	/* Headers per section 3,9,8 */
	pwr_tFloat32  CoolWidth;		/* Width of ACC [m]  4.3	 */
	pwr_tInt32		NbNozzBank;		/* No of nozzles per header */
	pwr_tFloat32	NbNozzCentr;	/* No of nozzles Center of header */
	pwr_tInt32    nPyr;			/* No. of pyrometers */

	pwr_tFloat32  PosP1;			/* Distance Mill Pyrometer 1 [m] */
	pwr_tInt32    iStatusP1;		/* ״̬������ */
	pwr_tFloat32  fTempLowP1;		/* ��  ���Ʋ����� */
	pwr_tFloat32  fTempHighP1;		/* High ���Ʋ����� */

	pwr_tFloat32  PosP4;			/* Distance Mill Pyrometer 4 [m] */
	pwr_tInt32    iStatusP4;		/* ״̬������ */
	pwr_tFloat32  fTempLowP4;		/* ��  ���Ʋ����� */
	pwr_tFloat32  fTempHighP4;		/* High ���Ʋ����� */
	pwr_tFloat32  PosP5;			/* Distance Mill Pyrometer 5 [m] */
	pwr_tInt32    iStatusP5;		/* ״̬������ */
	pwr_tFloat32  fTempLowP5;		/* ��  ���Ʋ����� */
	pwr_tFloat32  fTempHighP5;		/* High ���Ʋ����� */
	pwr_tFloat32  PosP6;			/* Distance Mill Pyrometer 6 [m] */
	pwr_tInt32    iStatusP6;		/* ״̬������ */
	pwr_tFloat32  fTempLowP6;		/* ��  ���Ʋ����� */
	pwr_tFloat32  fTempHighP6;		/* High ���Ʋ����� */

	pwr_tFloat32  PosP23;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP23;		/* ״̬������ */
	pwr_tFloat32  fTempLowP23;		/* ��  ���Ʋ����� */
	pwr_tFloat32  fTempHighP23;		/* High ���Ʋ����� */

	pwr_tFloat32  PosP2L;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP2L;		/* ״̬������ */
	pwr_tFloat32  fTempLowP2L;		/* ��  ���Ʋ����� */
	pwr_tFloat32  fTempHighP2L;		/* High ���Ʋ����� */

	pwr_tFloat32  PosP3L;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP3L;		/* ״̬������ */
	pwr_tFloat32  fTempLowP3L;		/* ��  ���Ʋ����� */
	pwr_tFloat32  fTempHighP3L;		/* High ���Ʋ����� */

	pwr_tFloat32  PosP2U;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP2U;		/* ״̬������ */
	pwr_tFloat32  fTempLowP2U;		/* ��  ���Ʋ����� */
	pwr_tFloat32  fTempHighP2U;		/* High ���Ʋ����� */

	pwr_tFloat32  PosP3U;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP3U;		/* ״̬������ */
	pwr_tFloat32  fTempLowP3U;		/* ��  ���Ʋ����� */
	pwr_tFloat32  fTempHighP3U;		/* High ���Ʋ����� */

	pwr_tInt32    iTempIcEn;		/* Entry pyromter IC (1 to 3) */
	pwr_tInt32    iTempIcEx;		/* Exit  pyromter IC (1 to 5) */
	pwr_tFloat32  PosZon1Entry;	/* Distance Mill Entry Zone 1 [m]	*/
	pwr_tFloat32  PosZon2Entry;	/* Distance Mill Entry Zone 2 [m]	*/
	pwr_tFloat32  PosZon3Entry;	/* Distance Mill Entry Zone 3 [m]	*/
	pwr_tFloat32  PosZon3Exit;	/* Distance Mill Exit Zone 3 [m]	*/

	pwr_tFloat32  MinFlowTop[3];	/* ��Сflow Top Sections ^[m^3/h] */
	pwr_tFloat32  MinFlowBtm[3];	/* ��Сflow Bot Sections ^[m^3/h] */

	pwr_tFloat32  MaxFlowSect1;	/* ��� flow Top Section1 ^[m^3/h] */
	pwr_tFloat32  MaxFlowSect2[2]; /* max.  flow Top Section2 ^[m^3/h] */
	pwr_tFloat32  MaxFlowSect3;	/* ��� flow Top Section1 ^[m^3/h] */
		
	pwr_tFloat32  MaxSpeedTransf; /* max. transfer �ٶ� to ACC [m/s]  */
	pwr_tFloat32  MinSpeedCool;   /* min. �ٶ� inside ACC [m/s]  */

	pwr_tFloat32  MaxSpeedCool[8]; /* �ٶ� for thickness classes [m/s]  */  // modify by xie from 5 to 8   12-1
	pwr_tFloat32  ThickUpLimit[8]; /* corresponding thickness classes [mm] */ // modify by xie from 5 to 8
	
	pwr_tFloat32  DelayFirstTurn;  /* delay time first turn [s]		*/
	pwr_tFloat32  DelaySubsTurn;  /* delay time subsequent turns turn [s]	*/
	pwr_tFloat32  AccTurn;		   /* acceleration reverse turn [m/s^2] */
	pwr_tFloat32  WaterTemp;  	/* water Temperature [��]	*/
	pwr_tFloat32  WaterPress;
	pwr_tFloat32  AirTemp;			/* air Temperature   [��]	*/
	pwr_tFloat32  AlphaCoe;
	pwr_tFloat32  AlphaCoeIndex;

	pwr_tFloat32  AlphaCoeAccWatTemp;
		
	pwr_tInt32    iFilterPyro;		/* 0: no additional level 2 filter */
	/* Synchronization Points: BLS1, BLS2 and gamma rays */
	pwr_tInt32    nBls;     /* No. of synchronization Poits */
	pwr_tInt32    iStatusBls[ANZ_BLS];   /* Status synchronization Poits */
	pwr_tFloat32  fPosBls[ANZ_BLS]; /* position synchronization points */
	char		   sNameBls[ANZ_BLS][50]; /* description */

	pwr_tInt32    nEdge;     /* No. of edge �ڱ� drives */
	pwr_tInt32    iStatusEdge[ANZ_EDGE];   /* Status drives */
	pwr_tInt32	   iPosEdge[ANZ_EDGE]; /* first cooling unit of drive */
	/* side spray ��Ϣ */

	pwr_tInt32    nSideSpray;     // No. of side sprays  ����������Ϣ
	pwr_tInt32    iStatusSideSpray[ANZ_SIDESPRAY];   /* Status side spray */
	pwr_tFloat32  fPosSideSpray[ANZ_SIDESPRAY]; /* position side sprays */
	/* side spray will have influence on header */
	pwr_tInt32    iHeaderSideSpray1[ANZ_SIDESPRAY];   /* influence header */
	pwr_tInt32    iHeaderSideSpray2[ANZ_SIDESPRAY];   /* influence header */

	pwr_tInt32	   iInnerOsc; /* 1: system capable for inner oscillation mode */
	pwr_tInt32	   iOuterOsc; /* 1: system capable for outer oscillation mode */
	pwr_tInt32    nHeader;		/* No. of headers */
	/* iHeaderFirst , iHeaderLast calculated based on iStatusHeader[] */
	pwr_tInt32	   iHeaderFirst; /* index first active header */
	pwr_tInt32	   iHeaderLast;  /* index last  active header + 1 */
	pwr_tInt32    nHeaderType;	/* No. of different header types */

	pwr_tInt32    iIdHeader[NUZZLE_NUMBER];
	pwr_tInt32    iZoneHeader[NUZZLE_NUMBER];
	pwr_tInt32    iTypeHeader[NUZZLE_NUMBER];
	pwr_tInt32    iStatusHeader[NUZZLE_NUMBER];
	pwr_tFloat32  fPosHeader[NUZZLE_NUMBER];
	pwr_tFloat32  fLengthHeader[NUZZLE_NUMBER];
	pwr_tFloat32  fFlwMinHeaderTop[NUZZLE_NUMBER];
	pwr_tFloat32  fFlwMaxHeaderTop[NUZZLE_NUMBER];
	pwr_tFloat32  fFlwMinHeaderBtm[NUZZLE_NUMBER];
	pwr_tFloat32  fFlwMaxHeaderBtm[NUZZLE_NUMBER];
			
	pwr_tFloat32  fPosTakeoverIC; /* take over point interface cooling */ 
	/* typical distance of ��� to mill during rolling is required to keep */
	/* ��� dry if last pass is in direction to ACC */
	/* direction of the last pass is given with the PDI for each ��� */
	pwr_tFloat32  fPosTakeoverMill; /* Reverse point of ��� during rolling */ 
}RALCOEFFDATA;


//�������ݽṹ��
typedef struct tagMDSCoolTrackData
{
	RALINDATA RalPdi;
	RALCOEFFDATA RalCoeff;
	RALMESDATA RalMes;
	RALOUTDATA RalOutPut;
}TRACKDATARAL;


//��ʾ��� ��ʾ�ø�����ô���
typedef struct  tagSetPointsShow
{
	double  v;			//pass speed, m/s
	double  a;			//pass acceleration, m/s
	//double  qtop[NUZZLE_NUMBER];		//  m3/hr
	//double  qbtm[NUZZLE_NUMBER];
	//int		up_valve_pat[NUZZLE_NUMBER];
	//int		down_valve_pat[NUZZLE_NUMBER];

	////////////////////Add number of array by Zed//////////////////
	double  qtop[14];		//  m3/hr
	double  qbtm[14];
	int		up_valve_pat[14];
	int		down_valve_pat[14];

	int     side_jet[SIDESPRAY_NUMBER];	     //side spray status
	double  mask_cal[EDGEMASKING_NUMBER];

	int     Blow[3];
	
	float     head_btm_length;
	float     tail_btm_length;
	float     tail_btm_coe;			//
	float	  head_btm_coe;		    //

	double	w_ratio[14];	 	
	double	u_ratio[14];	 	
	
	double   plateHeadPosition[POSITION_NUMBER];
	double   plateHeadSpeed[POSITION_NUMBER];
	int    acc_mode;
	int    acc_dir;
	int    inter_acc_req;
	int    activeFlg;
	int isWellSelfLearned; 

	int widShield;  // ����ڱ� [11/29/2013 xie]
	int widDetaShield;

}SETPOINTSSHOW;


typedef struct tagScrData
{
	bool  send_flg;
	SETPOINTSSHOW  setpoints;
}SCRDATA;

typedef struct tagTrackingInfo
{
    int  oper_mode;
	bool learnFlg;
	int  Cool_status;
}TRACKINGINFO;

//��������
typedef struct tagMeasureData
{
  char                         plateID[32];
  short                        number;
  int                          CurIndex[MAX_MEASURE_VALUE];
  short                        realIndex;
  float                        TimeSinceP1[MAX_MEASURE_VALUE];
  float                        Speed[MAX_MEASURE_VALUE];
  float                        Position[MAX_MEASURE_VALUE];
  float						   TempT0[MAX_MEASURE_VALUE];
  float						   TempTH1[MAX_MEASURE_VALUE];
  float						   TempTH2[MAX_MEASURE_VALUE];
  float						   TempTH3[MAX_MEASURE_VALUE];
  float						   TempTc[MAX_MEASURE_VALUE];
  float					       TempT1[MAX_MEASURE_VALUE];
  float						   TempT2[MAX_MEASURE_VALUE];
  float						   TempT3[MAX_MEASURE_VALUE];
  float						   TempT4[MAX_MEASURE_VALUE];

  float						   TempSp1[MAX_MEASURE_VALUE];
  float						   TempSp2[MAX_MEASURE_VALUE];
  float						   TempSp3[MAX_MEASURE_VALUE];
  float						   TempSp4[MAX_MEASURE_VALUE];
  float						   TempSp5[MAX_MEASURE_VALUE];
  float						   TempSp6[MAX_MEASURE_VALUE];
  float						   TempSp7[MAX_MEASURE_VALUE];
  float						   TempSp8[MAX_MEASURE_VALUE];

}MESUREDATA;


typedef struct COOLSTRUCT
{
	  SCRDATA   scr;
	  MESUREDATA TrkFrtMes;
	  MESUREDATA TrkMes[5];
}COOLSTRUCT;


/////////�����������ݿ�Ľṹ�� add by Zed 2013.11////////////
typedef struct DBSTORGEMES
{
	/////���ڶ�ȡ�������ݿ�ı���/////
	char DB_S_Grade[20];
	char DB_P_Grade[20];
	float DB_Thick;
	float DB_RedTemp;

	/////���ڴ洢��MES���ݿ�ı���////
	char Ms_ID[20];
	float Ms_len;
	float Ms_wid;
	float Ms_thk;
	int Ms_CoolUse;
	char Ms_StaTim[32];///����ʱ��
	char Ms_StpTim[32];///����ʱ��
	float Ms_WatTemp;///ˮ��
	float Ms_WatPres;///ˮѹ
	float Ms_CoeHead;
	float Ms_CoeTail;
	float Ms_LenHead;
	float Ms_LenTail;
	float Ms_InUpTemp;
	float Ms_InDnTemp;
	float Ms_OutUpTemp;
	float Ms_OutDnTemp;
	float Ms_OutAveTemp;
	float Ms_Cr;///Cooling Rate
	float Ms_Side[20];
	float Ms_GapUp[4];
	float Ms_GapRatio[4];
	float Ms_HDUp[10];
	float Ms_HDratio[10];
	float Ms_Spd;
	float Ms_AcSpd;
	float Ms_Cr_1;/////part1 calculated cooling rate 
	float Ms_Cr_2;/////part2 calculated cooling rate 
	float Ms_AftRol_Time;////�����ԥʱ��
	float Ms_OperMode;
	float Ms_ThroghMode;
	float Ms_CoolMethod;
	float Ms_CoolMode;
	float Ms_CalTemp_1;////һ�׶μ�����ȴ�¶�
}DBStorgeMes;


	/////////////////////////////////////////////////////////////////////////////////////////
	//��ȴ
struct UFCDefine
{
	//--------------------------Cooling tracking structure define----------------------//
	SCRDATA  scr;
	TRACKINGINFO  trk;
	
		
	TRACKDATARAL FINISH_ROLL[FIN_QUE_NUM];
	TRACKDATARAL AFT_FINISH_ROLL[AFT_FIN_QUE_NUM];
	TRACKDATARAL UND_LAMINAR_COOL[UND_COOL_QUE_NUM];
	TRACKDATARAL AFT_LAMINAR_COOL[AFT_COOL_QUE_NUM];
	TRACKDATARAL RefreshTrackData;
	TRACKDATARAL pdiPrepareTrackData;
	TRACKDATARAL furTrackData;

	//cool model struct
	COOLSTRUCT m_coolStruct;
	//cool_data
	int Cool_status;
	float rbt_tar;
	int CurIndex1;
	int CurIndex2;
	int CurIndex3;
    //Goracle m_Goracle;
   char CSpdiAdcos[2000];
	//CString  CSpdiAdcos;

	int CurPltIcFlag;
	float waterFactorManual;
	float HeatCoefManual;
	DBStorgeMes m_DBStorgeMes;

	int rowNum;

	SLP			 adaptionParaTransfer;  // add [1/13/2015 qian]

};  

////////////���ݿ�ṹ��//////////
struct DBVal
{
	char DB_Grade[20];
	float DB_Thick;
	float DB_RedTemp;
};

