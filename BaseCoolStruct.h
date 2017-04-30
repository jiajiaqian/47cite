#pragma once

//////////////////////////////////////////////////供全局使用的基本结构体//////////////////////////////////////


//////////////////////////////////////////////////输入输出结构体//////////////////////////////////////
#include "BaseCoolPara.h"
	/* 控制参数 for online and adaption */

///////////////////////////////////////CURVE_1结构体//////////////////////////////////////
//曲线 用于插value
typedef struct tagCurve {
	int     n;  /* number of points per curve */
	/* first value on index 0 : last - n-1*/
	int	  iabl; /* no. of derivative	*/
	float   x_min; /* 最小横坐标*/
	float   x_max; /* 最大 横坐标*/
	float   y_min; /* 最小纵坐标*/
	float   y_max; /* 最大 纵坐标*/
	float   c_id; /* 曲线标示符 */
	float   x[150]; /* 横坐标value            */
	float   y[150]; /* 纵坐标value            */
	float   yp1[150]; /* 一阶导数 */
	float   yp2[150]; /* 二阶导数 */
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

//整个模型计算的控制参数结构体
typedef struct tagModelControl{
		/* 在线模式的控制参数 */
		int		bSmoothTempEntry;	/* 平滑 entry Temperature */
		int		bSmoothTempExit;	/* 平滑 exit  Temperature */
		int		bCorTempAct;		/* 纠正 Temperature 偏差 Entry from PDI */
		int		bCorSpdAct;			/* 纠正 速度 profile by measured 速度value */
		float	fCorDevSpd;			/* allowed 速度 偏差 for 纠正 ion [% * 0.01] */
		int		bCorSpdSynchro;		/* additional 纠正ion by synchronization points */
		float	fCorTimeSynchro;	/* time window passing synchronisation points [sec] */
		float	fCorPosSynchro;		/* position window synchronization [m] */
		int     iBlsSynchro;        /* 参考 synchronization point (0: BLS1, 1: HMD) */
		float	fGainDevSpd;		/* gain for 速度 纠正 ion */
		float	fGainDevTemp;		/* gain for entry Temperature 偏差 */
		float	fDistSafety;		/* safety distance 头部 to first header [m] */
		/* 控制参数 adaption (PreCalc + PostCalc) */
		int		iAdaptLevel;		/* use adaption level 1 to iAdaptLevel */
		int		nAdaptPlates;		/* No. of plates for adaption */
		int		bAdaptHeadRead;		/* 适应 头部 遮蔽 read 估value */
		int		bAdaptTailRead;		/* 适应 尾部 遮蔽 read 估value */
		int		bAdaptHeadWrite;	/* 适应 头部 遮蔽 write 估value */
		int		bAdaptTailWrite;	/* 适应 尾部 遮蔽 write 估value */
		float	fAdaptDifTime;		/* differential time use higher adaption level [hr] */
		float	fAdaptLimitMin;		/* 最小自适应限制 */
		float	fAdaptLimitMax;		/* 最大 自适应限制 */
		float	fAdaptStepSize;		/* 最大 step size from one piece to next */
		float	fAdaptStepSizeTmp;	/* 最大 step size from one piece to next Temperature */
		float	fAdaptLimitHeadMin; /* 最小自适应限制 头部 */
		float	fAdaptLimitHeadMax; /* 最大 自适应限制 头部 */
		float	fAdaptLimitTailMin; /* 最小自适应限制 尾部 */
		float	fAdaptLimitTailMax; /* 最大 自适应限制 尾部 */
		float	fAdaptAddFlwHead;	/* additional flow allowed on 头部 [% * 0.01] */
		float	fAdaptAddLenHead;	/* 最大 allowed 头部 length 纠正ion [m] */
		float	fAdaptAddFlwTail;	/* additional flow allowed on 尾部 [% * 0.01] */
		float	fAdaptAddLenTail;	/* 最大 allowed 尾部 length 纠正ion [m] */
		/* contractual 参数 */
		float	fMinMaxTempEntry;	/* allowed min-max Temperature difference Entry [K] */	//入口Temperature偏差
		float	fMinMaxTempExit;	/* allowed min-max Temperature difference Exit  [K] */
		float	fFacDevTemp;		/* multiplier for additional Temperature 偏差 Entry */
		float	fAdaptDevSpd;		/* 最大 allowed 速度 偏差 [% * 0.01] */
		float	fFacDevSpd;			/* multiplier for additional 速度 偏差 */
		/* read 冷却模式 参数 */
		/* read 层流信息 from 冷却模式 */
		int		iDimFlw;            /* 1: ltr/(m^2*min) {specific} 2: ltr/min (unit) */ //水流密度
		int		iDimEdge;			/* 1: mm 2: m */				//边部遮蔽
		/* lengthes 头部 and 尾部 遮蔽 */
		int		iDimLenMask;		/* 1: mm 2: m */
}MODELCONTROL;

//预计算结果
typedef struct tagPreResult 
{
	float airTempDrop;  //谢谦加入 在进入冷前区前的温降 2012-5-22
	float airTailTempDrop;  //谢谦加入 在进入冷前区前的温降 2012-5-21
	float  beforeAirCoolTemp;
	float  afterAirCoolTemp;

	float  crCalcLayer[10]; //谢谦加入 2012-5-22 原in[60] 十位

	float preSpeed;   //谢谦加入 2012-7-25 给二级数据库发送
	float accSpeed;   //谢谦加入 2012-7-25 给二级数据库发送

	float aveTmP1;  //谢谦加入 2012-8-8 给轧机二级数据库发送
	float aveTmP2u;  //谢谦加入 2012-11-5
	float aveTmP3u;
	float aveTmP3l;
	float aveTmP4;

	float aveSpCenter;  // add 扫描测温仪中部 [1/2/2014 xie]
	float aveSpEdge;
	float aveSpEdge1;   // add [3/31/2015 谢谦]  
	
	float aveSpeed;
	float realFlowUp[NUZZLE_NUMBER+GAP_NUZZLE_NUMBER];// 缝隙集管分组特殊 [11/13/2013 谢谦]
	float realWaterFactor[NUZZLE_NUMBER+GAP_NUZZLE_NUMBER];// 缝隙集管分组特殊 [11/13/2013 谢谦]

	float realWaterPressure;
	float flowFactor;

	int isHeadFlated;  //add by xie qian 是否压头标志位 10-16
	int isWellSelfLearned; //add by xie 自学习是否到位 10-22

	float calcWaterFactor;   //add by xie 11-26 存储程序读到的水比 计算总流量用 
	float calcTotalFlow;
	float actTotalFlow;
	float frame1Gap;
	float frame2Gap;
	int openSideSprayNo;    // 5位数
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
	float finishRollTargetTemperature; // 目标终轧温度 [8/10/2013 谢谦]
	float finishCoolTargetTemperature;  // 目标终冷温度 [8/10/2013 谢谦]
	float coolRate;
	int coolTypeMode;   // acc ufc DQ [8/10/2013 谢谦] 
	char CcoolType[2]; //"only last pass 	A-no ACC, 		W(default)-ACC, 		U-UFC, 
	//	E-ACC+UFC, 		D-UFC+ACC, 		Q-DQ, 		N-other, no limit, NEU will decide how to use water"  //  [11/21/2013 谢谦]
	int coolModeFlg;
	int operateMode;   // 手动 半自动 全自动 [8/10/2013 谢谦]
	int coolThroughMethod;  //通过模式 通过时 往复式 摆动式   //  [11/21/2013 谢谦]
	int coolOpenMode;  //集管开启模式 0 = front ACC 1 = back ACC		2 = two part ACC		3 = sparse稀疏ACC(2 vs 2)		4 = front UFC		5 = back UFC
		//6 = two part UFC		7 = sparse稀疏UFC(2 vs 2)		8 = DQ cooling mode		9 =  经验模式  //  [11/21/2013 谢谦]
	
	float alloyC;
	int alloyExponent; // 碳当量 [8/10/2013 谢谦]
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
	float upNuzzleNumber[NUZZLE_NUMBER]; //集管开闭状态
	float downNuzzleNumber[NUZZLE_NUMBER];

	float upNuzzleFlow[NUZZLE_NUMBER];
	float downNuzzleFlow[NUZZLE_NUMBER];
	int  edgeMask[NUZZLE_NUMBER];  //谢谦修改 2012-6-23 from float to int
	int   waterOne[10];
	int   airOne[10];			//用处不大

	// 这部分暂时没有实际用处 [8/10/2013 谢谦]
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
	int nuzzleMaskTailTop;//谢谦修改 2012-6-23 from float to int
	int nuzzleMaskTailBtm;//谢谦修改 2012-6-23 from float to int

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

	float  airEndTemp[NODES_NUMBER]; //  temp end air dist.  谢谦 加入 2012-5-16 保存空冷后的初始Temperature
	float  airEndAust[NODES_NUMBER]; //    temp end air dist.  

	pwr_tChar                            FrtTime[16];
	pwr_tChar                            FctTime[16];

	float readSpeed;    // add 读取文件规程用 [11/19/2013 谢谦]
	float readFlowUpLimit;
	float readFlowDnLimit;
	float readWaterFactor;
	float readWaterFactorMod; // add [4/16/2014 谢谦]

	float readWaterFactorSl;//  [9/18/2014 qian]

	float readFlowUpLimitUFC;
	float readFlowDnLimitUFC;
	float readAceSpeed; 
	int readWidShield;
	int readDetaWidShield;

	float readSideFlowFactor;// add [12/25/2014 谢谦]

	float readCoeAlpha;
	char cSideSpray[20]; // add [11/30/2013 谢谦]

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
	float scanTSelflearn;  // add 边部流量控制自学习系数 [1/2/2014 xie]

	float tarEftRateT1;
	float aveEftRateT1;
	float tarEftRateT4;
	float aveEftRateT4;

	int rowNumInfile; // add [3/22/2014 xie]

	int SLPflg;// 应用无极检索标志 [12/16/2014 wzq]  // add [1/13/2015 qian]

	int turning; // add [1/14/2015 谢谦]

}PRERESULT;

//输入的数据
typedef struct tagInputDataPDI{
		char    plateID[20];   /* 板材ID */
		char    matID[PATH_MAX];     /* 材料标示          */
		char    slabGrade[50];     /* 材料标示          */
		float   thick;      /* 板材厚度   [mm]    */
		float   width;      /*板材的宽度       [mm]    */
		float   length;     /* 板材的长度          [mm]    */
		float   tcrThickTemp;      /* 板材厚度   [mm]    */
		float   tcrWidthTemp;      /*板材的宽度       [mm]    */
		float   tcrLengthTemp;     /* 板材的长度          [mm]    */
		float   tcrThickTemp2;      /* 板材厚度   [mm]    */
		float   tcrWidthTemp2;      /*板材的宽度       [mm]    */
		float   tcrLengthTemp2;     /* 板材的长度          [mm]    */
		float   finishRollTemp;   // 终轧温度 [8/10/2013 谢谦]
		float   startCoolTemp;
		float   targetFinishCoolTemp;     /* 停止冷却Temperature [Celsius]   */
		float   cr;         /* 冷却速度[K/s]   */

		float   ic1FinishTemp;   // 精轧 Temperature  [Celsius] 
		float   headUpFinishRollTemp;//终轧平均Temperature
		float   headDownFinishRollTemp;//终轧平均Temperature
		float   headCenterFinishRollTemp;//终轧平均Temperature
		float   medUpFinishRollTemp;//终轧平均Temperature
		float   medDownFinishRollTemp;//终轧平均Temperature
		float   medCenterFinishRollTemp;//终轧平均Temperature
		float   TailUpFinishRollTemp;//终轧平均Temperature
		float   TailDownFinishRollTemp;//终轧平均Temperature
		float   TailCenterFinishRollTemp;//终轧平均Temperature
		float   ic1StartTemp;
		float   ic1StopTemp;     /* 停止冷却Temperature [Celsius]   */
		float   ic1CoolRate;         /* 冷却速度[K/s]   */

		float   ic2FinishRollTemp;   /* 精轧 Temperature  [Celsius]   */
		float   ic2FinishRollTempCenter;
		float   ic2FinishRollTempSur;
		float   ic2StartTemp;
		float   ic2StopTemp;     /* 停止冷却Temperature [Celsius]   */
		float   ic2CoolRate;         /* 冷却速度[K/s]   */
		
		float   t_stage1_finish;//目标开冷
		float   stage1_cr;
		float   stage2_cr;
	    int     IdTsk;//手动处理钢板(1);仿真预设定(2);仿真操作Gost (3);time计算请求 (4);删除PDI(5)
	   
		char	 PltIcFlg[1];//0:空冷1:IC1 水冷2:IC2 水冷3:IC1 and IC2水冷
		int     PltIcFlgTemp;   ////0:空冷1:IC1 水冷2:IC2 水冷   // 只有 0，1,2 用于time request [9/23/2013 谢谦]

		float  PltDlyTimRol;//冷前弛豫time
		float  PltDlyTimCol;//两阶段中弛豫time
		int	  PltHprActFlg;//预矫直机前除磷激活标志
		int	  PltPlActFlg;//预矫直机激活标志
		float	PltPplTimHead;    //钢板头部矫直time
		float	PltPplSpdHead;    //钢板头部矫直速度		
		float	PltPplLenHead;    //钢板头部矫直长度
		float	PltPplTimBody;    //钢板板身矫直time
		float	PltPplSpdBody;    //钢板板身矫直速度
		float	PltPplLenBody;    //钢板板身矫直长度
		float	PltPplTimTail;    //钢板尾部矫直time
		float	PltPplSpdTail;    //钢板尾部矫直速度
		float	PltPplLenTail;    //钢板尾部矫直长度

		int     priority;       /* 1: Temp, 2: CR 3: 速度 0: 速度 */
		int	  iSimMode;	/* 1: simulation mode 2: 过程 mode */
		int     acc_mode;   /* cooling mode             */
		int     op_mode;  
		int     coolStatus; //IC, ACC 
				
		char    file_nam[PATH_MAX];
		float   alloy_equC;
		float   alloy_c;        /* 合金 carbon         */
		float   alloy_cr;   /* 合金 crom           */
		float   alloy_cu;   /* 合金 copper         */
		float   alloy_mn;   /* 合金 mangan         */
		float   alloy_mo;   /* 合金 molybdaen      */
		float   alloy_ni;   /* 合金 nickel         */
		float   alloy_si;   /* 合金 silicium       */
		float   alloy_ti;   /* 合金 titanium       */
		float   alloy_nb;   /* 合金 niobium        */
		float   alloy_b;       /* 合金 boron       */
		float   alloy_v;    /* 合金 vanadium     */
		int     pl_mode;        /* pre-levelling mode           */
		int     taskID;     /* 1: PREPROCESS 2: POSTPROCESS     */
		float    v_min_PL;  /* 最小speed 预矫直机 [m/s]  */
		float    v_max_PL;  /* 最大 速度 预矫直机 [m/s]  */
		float    v_min_HL;  /* 最小speed 热矫直机 [m/s]  */
		float    v_max_HL;  /* 最大 速度 热矫直机 [m/s]  */
		int		iPat;	   /* pattern: 0: use compact 1: distrtributed if CR requires */
		int		iDirLastPass; /* -1: to furnace 1: direction Acc */
		int		iHeaderFirst; /* index of first active Header if iDirLastPass = 1*/
		float    eval_pyro;
		float    startTempDistribution[NODES_NUMBER];     /* 开始分配 Temperature (option) [度] */
		float    aust_start[NODES_NUMBER];  /* 开始分配 austenit    (option) [%]  */
		float    grain_size[NODES_NUMBER];  /* 开始分配 grain size  (option)[mue] */
		float    IcColTime;  
		float	IcColTimeAir;
		int      FlowRate;   
		MODELCONTROL	controlAcc;	   /* 控制参数 Acc */

		float   SpeedEXP;
    	float   AcclerEXP;
		int     pass_mode;
		int     open_type;
		int     activeFirstJet;
		int     activeSecType;
			//changed by wbxang 20120506

		float initFinishRollingTem;   //谢谦加入 用于保存原有的终冷Temperature
		int  working_stage;     //谢谦加入 用于记录当前水流密度计算判定情况 应该有四种
		int isHeadFlated;  //add by xie qian 是否压头标志位 10-16
		float manualHC;

		float waterTemperature;  // add [1/13/2015 qian]


	}INPUTDATAPDI;

//材料参数
typedef struct  tagMaterialParameters
{
	int   matn; /* 向量长度     */
	int   matno_lam; /* 材料标示 for lambda-curve    */
	int   matno_rc;  /* 材料标示 for rho * cp curve  */
	float ar1;  //ar1 相变点
	float ar3;  //ar3 相变点
	char  datname[50]; /* name of FILE with cp-value  */
	CURVE austenitTransPercent;  /* 奥氏体分解 %*/
	float lambda[MAXTEMP+1];    //heat conductivity 向量
	float rhoCp[MAXTEMP+1];    /*向量 of heat capacity  */
	float austenitEnthalpy[MAXTEMP+1];    // austenit enthalpy austenitEnthalpy
	float rhoCpg[MAXTEMP+1]; // heat cap.*rho AUSTENIT
	float ferritEnthalpy[MAXTEMP+1];    /* ferrit enthalpy */
	float rhoCpa[MAXTEMP+1]; /* heat cap.*rho FERRIT */
	float trans[MAXTEMP+1];   /* 奥氏体分解 */
	float trans_p[MAXTEMP+1]; /* 一阶导数 aust. dec. */
	char  rho_dat[PATH_MAX];  /* 文件名 density */
	char  cp_dat[PATH_MAX];  /* 文件名 heat capacity */
	char  lam_dat[PATH_MAX];  /* 文件名 lambda */
	char  air_dat[PATH_MAX];  /* 文件名 alpha air-cool */
	char  wat_dat[PATH_MAX];  /* 文件名 alpha wat-cool */
}MATERIALPARAM;

	/*   3. 定义 alpha-value structure     */
	//最初系数
typedef struct tagAlphaValues
{
	int   alphan;   /* 向量长度     */
	float top[MAXTEMP+1];   /* alpha value on top-side     */
	float bot[MAXTEMP+1];   /* alpha value on bottom-side  */
}ALPHAVALUES;


/*  structure for alpha , lambda and rho * cp value    */
//材料+alpha
typedef struct tagMaterlal
{
	MATERIALPARAM  para;
	ALPHAVALUES    alpha;
} MATERIAL;

/*   4. 定义 computational 参数  */
	//过程参数 模型计算出来的参数
typedef	struct tagComputational  //verfahren 进程 过程的意思
{
	int   computeID;     // distribution identifier 0= normal    1 = only one 表面Temperature  
	int  femCalculationOutput;        // output of value during fem-calculation
	int   meanCalculationValue;        // 序号 value between two outputs 
	float calculationStopTime;      // 计算多长时间 stop calculation after calculationStopTime sec.   outp.) time at targetFinishCoolTemp        */
	float calculationStopTemp;       /* stop calculation after targetFinishCoolTemp K  */
	/* outp.) mean caloric Temperature  */
	/* if calculationStopTime > 0           */
	int testParameter;      /* test paprameter          */
	float   detaTime;     /* time increment for solution      */
	float calculationTotalTime;         //  实际冷却过程的总时间 time over whole cooling sequence 
	float localCoolRate;              // local 冷却速度 last int. point 
	int     integra;    /* parameter for solution chema in time integration algorithem       */
	int thickNodeNumber;      // number of thickNodeNumber in thickness dir. 
	int matrixElementNumber;     /* number of matrix elements        */
	int isSymetric;       // == 0 : non-symetric problem    == 1 : symmetriccal problem    
	float rhoCpMeanValue;      /* 平均value of rho_cp over thickness */
	float austenitePercentMean;           /* 平均value of 奥氏体分解 */
	float austenitePercentMeanNextStep;           // 平均value forgoing time step
	float latentHeat;               // latent heat 
	float startTempDistribution[NODES_NUMBER];        /* Temperature distribution (20 thickNodeNumber)  */
	/* outp.) last temp. distribution   */
	/* at calculationStopTime / targetFinishCoolTemp          */
	float startAusteniteDistribution[NODES_NUMBER];    /* distribution of 奥氏体分解 */
	float TimeKAusteniteDistribution[NODES_NUMBER];    /* distribution of 奥氏体分解 time step k*/
	float TimeKm1AusteniteDistribution[NODES_NUMBER];    /* distribution of 奥氏体分解  k-1*/
	float TimeKpredAusteniteDistribution[NODES_NUMBER];
	float TimeKRhoCp[NODES_NUMBER];     /* rho*cp- value in time step k */
	float TimeKm1RhoCp[NODES_NUMBER];     /* rho*cp- value in time step k-1*/
	float TimeKpredRhoCp[NODES_NUMBER];  /* rho*cp- value in predictor step */
	float TimeKEnthalpy[NODES_NUMBER];            /* enthalpy in time step k */
	float relasedEnergy[NODES_NUMBER];          /* released energy in time Step */
	float elementLength[NODES_NUMBER];           /* length of the elements       */
	float outputParameter[10];        // computational 参数 for output

	int     computedNumber;         /* number of computed value < TEMPERATURE_CURVE_NUMBER      */
	float   zx[TEMPERATURE_CURVE_NUMBER];        /* time value last cooling curve       */
	float   tx[TEMPERATURE_CURVE_NUMBER];        /* caloric mean Temperature value last curve       */
	float   tx_cen[TEMPERATURE_CURVE_NUMBER];    /* Temperature in the center line of 板材          */
	float   tx_top[TEMPERATURE_CURVE_NUMBER];    /* surface top Temperature value last curve        */
	float   tx_bot[TEMPERATURE_CURVE_NUMBER];    /* surface bottom temp. value last curve           */
	float   cx[TEMPERATURE_CURVE_NUMBER];        /* 冷却速度in C                                */

	//谢谦加入 测试用 2012-4-24 流量计算处 暂时不需要 知道所有的时刻的Temperature。

	int     gapComputedNumber;         /* number of computed value < TEMPERATURE_CURVE_NUMBER      */

	float   zx_temple[4*TEMPERATURE_CURVE_NUMBER];        /* time value last cooling curve       */
	float   tx_temple[4*TEMPERATURE_CURVE_NUMBER];        /* caloric mean Temperature value last curve       */
	float   tx_cen_temple[4*TEMPERATURE_CURVE_NUMBER];    /* Temperature in the center line of 板材          */
	float   tx_top_temple[4*TEMPERATURE_CURVE_NUMBER];    /* surface top Temperature value last curve        */
	float   tx_bot_temple[4*TEMPERATURE_CURVE_NUMBER];    /* surface bottom temp. value last curve           */
	float   cx_temple[4*TEMPERATURE_CURVE_NUMBER];        /* 冷却速度in C                                */
	int activeSlitNumber;  
				
}COMPUTATIONAL;          /* calculationStopTime or targetFinishCoolTemp equal zero  -/+ are possible   */

/*   5. 定义 physical 参数   */
//物性参数
typedef struct tagPhysical
{
	int taskID;     // 1: 预计算 2: 后计算   
	int mat_id;     /* 材料标示          */
	char datname[PATH_MAX];    /* name of cp-FILE          */
	float   speed;      /* 速度 of the 板材 from furnace to   */
	/* active 冷却区          */
	int method;     // 1 : 过程 version      
	/* 2 : simulation           */
	int isWriteAdaptValue;    // 0: write 自适应value for staistic 1: write adaption for later use 
	
	unsigned long int        adaptKey[5]; // adaption key

	float  adaptRatioFound; // 自适应value found     adaptRatioFound

	int   levelNumber;      // level no. for adaption

	int  furnaceTemp;  /* output Temperature from furnace  */
	int maxPossibleMillTemp;     /* 最大 possible mill Temperature    */

	float topFlowRate;     /* flow rate top    [l/(min * m**2)]    */
	float bottomFlowRate;     /* flow rate bottom [l/(min * m**2)]    */
		
	float waterTemp;   /* temp. of 水冷 [Celsius]     */
	float waterPressure;   /* temp. of 水冷 [Celsius]     */

	float t_umg;        /* Temperature of air [Celsius]     */
	int   sw_air;       /* switch (0:air 1:water 2:spray)   */
	int   sw_mat;     /* 1: steel 2: other materials */
	int coolPhaseNumber;     /* parameter for different cooling */
	/* phases (from 0 to max. 9)        */

	float actualStopTime;    /* stop time for 实际 冷却区        */
	float actualStartTemp;  /* start Temperature active 冷却区  */
	float   duration[10];  /* duration of cooling phase        */
	int art[10];       /* type of cooling phase        */
	float alpha_coe;
	float waterFactorCoe;  //谢谦 加入 2012-7-16 水比修正系数

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
	/* = 默认value */
	/* iControl = 0: don't write adapted value AND don't read */
	/* iControl = 1: don't write adapted value AND read value for use */
	/*               in PRECALC */  
	/* iControl = 2: write adapted value AND read (default) */
	/* iControl = 3; write only for learning, do not read */
	//自适应参数
typedef struct tagAdapt{
	int			iFoundRead;				/* found adaptation key during read */
	int			iFoundWrite;			/* adaptation key already exist */
	int			max_plates;				/* 最大 板材记录数量*/
	int			num_plates;				/* total adapted plates for class   */
	int			rec_plates;				/* 板材记录数量 */
	int			iControl;				/* 自适应控制模式 */
	//add by wbxang 20120516
	unsigned long int adap_key[5];  	    /* Adaption_Key + Level */
	long int    iAdaptTimeLast;			/* last adaption time */
	long int    iAdaptTimeDiff;			/* time difference last 板材 [s] */
	char		plate_id_m1[MAX_ID];	/* last adapted 板材 ID */
	char		plate_id_m2[MAX_ID];	/* -2 adapted 板材 ID */
	char		plate_id_m3[MAX_ID];	/* -3 adapted 板材 ID */
	float       adaptRatioFound;				/* 自适应value冷却速度*/
	float		tr_foundMean;			/* 自适应valueTemperature Mean */
	float		tr_foundHead;			/* 自适应valueTemperature 头部 */
	float		tr_foundTail;			/* 自适应valueTemperature 尾部 */
	float		vx_last;				/* 速度 of last 板材 (mean)  */
	float		lx_last;				/* 活动头部长度 */
	float		tf_last;				/* 完成 Temperature */
	float		ts_last;				/* last 停止冷却temp */
	float		tw_last;				/* last Temperature 水冷 */
	long int	iSpare[3];				/* 预留 自适应value */
	float		fSpare[3];				/* 预留 自适应value */
	char		sPatternID[PATH_MAX];	/* ID 冷却模式 */
	char		sFileAdapt[PATH_MAX];	/* FILE / 表名 */
}ADAPTATION;


/* cooling 策略参数 */
/* refer to STRATEGY .doc for documentation */
//ACC层流冷却策略结果
typedef struct tagAccStrategy_t{
	int			iHeaderDir;		/* -1: header activated beginning last header */
	int			iZoneAct[ANZ_SECT];	/* 1: use 冷却区 */
	int			nStroke;		/* No. of allowed strokes */
	int			iDirStroke;		/* 1: odd No. of strokes 2: even No. of strokes */
	int         iModStroke;     /* 1: modification number of strokes during online allowed */
	int			iTransferMode[3]; /* 板材 transfer modes priority */
	float		fLengthBreakStr;	/* additional breaking length in case of */
	/* oszillations */
	int			iEdgeMask;		/* -1: reset edge 遮蔽 to zero */
	int			iHeaderFirst;	/* No. of preferred first header */
	/* header sequence active, deactivated, active, ... */
	int			nHeaderAct;		/* preferred No. of   activated headers */
	int			nHeaderActNon;	/* preferred No. of deactivated headers */
	int			nHeaderActPref;	/* No. of preferre active headers */
	int			nHeaderActMax;	/* Max. No. of active headers */
	int			iFlwReg;		/* flow regime: 1: constant all headers */
	int			iFlwDist;		/* flow distribution */
	float		fPosHeaderRef;	/* rel. 参考 position flow header */
	float		fFlwHeaderRef;	/* rel. 参考 flow on fPosHeader */
	float		fFlwHeaderLast;	/* rel. 参考 flow on last header */
	int			iTempStopPos;	/* 1: Surface, 2: Mean 3: Center */
	float		fTempStopDef;	/* 默认value stop Temperature (-1: Ar3) */
	float		fCrDef;			/* default 冷却速度*/
	float		fCorSpd;		/* 纠正ion value 速度 */
	float		fCorNuHeader;	/* 纠正ion value number of headers */
	float		fCorNuStrokes;	/* 纠正ion value number of strokes */
	int			nThick;			/* No. of thickness value */
	float		fThick[NTHK];	/* thickness value */
	float		fSpdPref[NTHK];	/* preferred 速度value */
	float		fSpdMin[NTHK];	/* 最小速度value */
	float		fSpdMax[NTHK];	/* 最大 速度value */
	float		fTempDropWater[NTHK];/* required Temperature drop in water (1: 100%) */
	float		fFlwMin[NTHK];	/* 最小flow value */
	float		fFlwMax[NTHK];	/* 最大 flow value */
	/* above Temperature difference (T_start - T_stop) flow limits will be applied */
	float		fFlwTmp[NTHK];
}STRATEGY;

//跟踪结构体  L1传来的数据
typedef struct tagTracking
{
		float TimeHeadRef;  /* 参考 time 板材 头部 */
		float TimeHeadRt;   /* time 辊表 头部 */
		float TimeHeadPlt;  /* time 板材 头部 */
		float TimeTailRef; /* 参考 time 板材 尾部 */    
		float TimeTailRt;  /* time 辊表 尾部 */
		float TimeTailPlt; /* time 板材 尾部 */
		/* 参考 value 速度 */
		float fSpdRefMean;	
		float fSpdRefMin;	
		float fSpdRefMax;	
		/* 实际 速度 信息 */
		float fSpdActMean;	
		float fSpdActMin;	
		float fSpdActMax;	
		/* 速度 偏差  */
		float fSpdDevMean;	
		float fSpdDevMin;	
		float fSpdDevMax;	
		int   iBlsEntry;    /* sync start Bls depend on Acc-length, speed-profile, valid mea data*/
		int   iBlsExit;     /* sync start Bls depend on Acc-length, speed-profile, valid mea data*/
}TRACKING;

///////////////////////////////////////输出结构体////////////////////////////////////

//预计算输出 
typedef struct tagOutputData
{
  pwr_tBoolean                        nuzzleStatusTop[NUZZLE_NUMBER];
  pwr_tBoolean                        nuzzleStatusBottom[NUZZLE_NUMBER];
  pwr_tBoolean                        headerWorkingStatus[NUZZLE_NUMBER];   //谢谦 加入 2012-4-28 集管开闭状态
  pwr_tFloat32                        waterFactor[NUZZLE_NUMBER];   //谢谦 加入 2012-4-28  水比  6-23 from int to float
  pwr_tFloat32                        waterFlowTop[NUZZLE_NUMBER];    //谢谦 加入 2012-5-16 存储每组集管的水流密度
  pwr_tFloat32                        waterFlowDown[NUZZLE_NUMBER];  //谢谦 加入 2012-5-16
  pwr_tBoolean                        valve_sid[NUZZLE_NUMBER];
  pwr_tFloat32                        edgePosition[ANZ_EDGE];
  pwr_tFloat32                        speedPosition[MAX_MEASURE_VALUE];                   // 谢谦 速度 位置 ？ 意义不明 
  pwr_tFloat32                        speed[MAX_MEASURE_VALUE];
    
  pwr_tFloat32						  fTimeAccHead;
  pwr_tFloat32						  fTimeAccTail;
   /* interphase cooling: remaining空冷 time */
  pwr_tFloat32						  fTimeAirIC;
  pwr_tFloat32						  fTimeCalReqWatIC; 
  pwr_tFloat32						  fTimeCalReqAirIC; 

  ADAPTATION                          adapt;
  STRATEGY							  accStrategy;
  MODELCONTROL						  controlAcc;
  PRERESULT			                  PreResult;
  SLP								  SLP_Post;   // add [1/13/2015 qian]
  
}RALOUTDATA;


//给MES的工艺参数信息
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

	

	pwr_tFloat32                        FlowZ1Top[MAX_MEASURE_VALUE];  // 涉及CopyInfoInSSAB函数，此部分信息可能不需要保持 [8/11/2013 谢谦]
	pwr_tFloat32                        FlowZ2Top[MAX_MEASURE_VALUE];
	pwr_tFloat32                        FlowZ3Top[MAX_MEASURE_VALUE];
	pwr_tFloat32                        FlowZ1Btm[MAX_MEASURE_VALUE];
	pwr_tFloat32                        FlowZ2Btm[MAX_MEASURE_VALUE];
	pwr_tFloat32                        FlowZ3Btm[MAX_MEASURE_VALUE];
		
	
	/* time 头部 end of 板材 passes synchronization points */
	pwr_tFloat32                        fTimeBlsHead[ANZ_HUB][ANZ_BLS];
	/* time 尾部 end of 板材 passes synchronization points */
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

	pwr_tFloat32                        SetPtFlowTop[NFLOW];  /* Voest 参考 Specific Flow*/
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


//输入
typedef struct tagInputDataRAL{


	int isOnlineCalc; //add by xie qian  是否在线计算的标志位
	int isCooling; //add by xie qian  是否冷却的标志位 9-11
	int isHeadFlated;  //add by xie qian 是否压头标志位 10-17

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
    ////////////////////////////////add by yuanda  2012.4     中间坯规格//////////////////////////////
	pwr_tFloat32                        tcrThickTemp;
	pwr_tFloat32                        tcrWidthTemp;
	pwr_tFloat32                        tcrLengthTemp;
	pwr_tFloat32                        tcrThickTemp2;
	pwr_tFloat32                        tcrWidthTemp2;
	pwr_tFloat32                        tcrLengthTemp2;
    pwr_tInt32                          IdTsk;//手动处理钢板(1);仿真预设定(2);仿真操作Gost (3);time计算请求 (4);删除PDI(5)
	char							    PltIcFlg[1];//0:空冷1:IC1 水冷2:IC2 水冷3:IC1 and IC2水冷
	pwr_tInt32							PltIcFlgTemp;//0:空冷1:IC1 水冷2:IC2 水冷3:IC1 and IC2水冷

	pwr_tFloat32						PltDlyTimRol;//冷前弛豫time
	pwr_tFloat32						PltDlyTimCol;//两阶段中弛豫time
	pwr_tFloat32						PltDlyTimLev;//预矫直前弛豫time

	pwr_tFloat32                        finishRollTemp;//终轧平均Temperature
	pwr_tFloat32                        t_finish_target;//终轧平均Temperature   //add by xie 12-3 传入工艺表格

	pwr_tFloat32                        headUpFinishRollTemp;//终轧平均Temperature
	pwr_tFloat32                        headDownFinishRollTemp;//终轧平均Temperature
	pwr_tFloat32                        headCenterFinishRollTemp;//终轧平均Temperature
	pwr_tFloat32                        medUpFinishRollTemp;//终轧平均Temperature
	pwr_tFloat32                        medDownFinishRollTemp;//终轧平均Temperature
	pwr_tFloat32                        medCenterFinishRollTemp;//终轧平均Temperature
	pwr_tFloat32                        TailUpFinishRollTemp;//终轧平均Temperature
	pwr_tFloat32                        TailDownFinishRollTemp;//终轧平均Temperature
	pwr_tFloat32                        TailCenterFinishRollTemp;//终轧平均Temperature

	pwr_tFloat32                        startCoolTemp;//目标开冷
	pwr_tFloat32                        targetFinishCoolTemp;//目标终冷
	pwr_tFloat32                        t_red;//目标终冷

	pwr_tFloat32                        stage1TargetFinishTemp;//一阶段目标终冷  stage1TargetFinishTemp
	pwr_tFloat32                        stage1CoolRate;//一阶段冷却速度
	pwr_tFloat32                        stage2CoolRate;//二阶段冷却速度
		
	pwr_tInt32							hprActFlg;//预矫直机前除磷激活标志
	pwr_tInt32							prelevelActFlg;//预矫直机激活标志
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

	pwr_tFloat32                        t_sec_start;//目标开冷  二阶段
	pwr_tFloat32                        t_sec_stop;//目标终冷  二阶段
	pwr_tFloat32                        sec_cr;
	
	pwr_tFloat32                        t_ic_finish_tmp;
	pwr_tFloat32                        t_acc_finish_tmp;
	pwr_tFloat32                        t_ic_stop_tmp;
	pwr_tFloat32                        t_acc_stop_tmp;

	pwr_tInt32                          acc_mode;
	pwr_tChar                           c_acc_mode[2];   //2012-6-27  用于接收数据库的信息

	pwr_tInt32                          acc_request;
	pwr_tChar                           c_acc_request[2]; //2012-6-27  用于接收数据库的信息

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

	pwr_tFloat32	PltPplTimHead;    //钢板头部矫直time
	pwr_tFloat32	PltPplSpdHead;    //钢板头部矫直速度		
	pwr_tFloat32	PltPplLenHead;    //钢板头部矫直长度
	pwr_tFloat32	PltPplTimBody;    //钢板板身矫直time
	pwr_tFloat32	PltPplSpdBody;    //钢板板身矫直速度
	pwr_tFloat32	PltPplLenBody;    //钢板板身矫直长度
	pwr_tFloat32	PltPplTimTail;    //钢板尾部矫直time
	pwr_tFloat32	PltPplSpdTail;    //钢板尾部矫直速度
	pwr_tFloat32	PltPplLenTail;    //钢板尾部矫直长度
		
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

	pwr_tFloat32                        alloy_w;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_re;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_pb;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_ca;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_n2;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_h2;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_o2;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_alt;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_ceq;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_zr;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_mg;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_sn;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_as;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_co;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_te;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_bi;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_sb;//新加入的元素种类 谢谦 2012-6-27
	pwr_tFloat32                        alloy_zn;//新加入的元素种类 谢谦 2012-6-27

	pwr_tInt32                          taskID;
	pwr_tInt32						    iSimMode;	/* 1: simulation mode 2: 过程 mode */
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

	pwr_tInt32							activeFirstJet;  // 开启模式等信息 [8/12/2013 谢谦]
	pwr_tInt32						    activeSecType;

	float                               manualHC;  // add[11/15/2013 谢谦]

}RALINDATA;

	////////////////////////////////////   冷却参数结构体 //////////////////////////////////////////
typedef struct tagMDSCoolCoeff 
{
	pwr_tFloat32  initSpeed;              //预计算初始速度，根据读表获得，待细化
	pwr_tFloat32  CoolerLength;	       // length of ACC 	[m] 	20	  冷却区长度
	pwr_tFloat32  CoolBankLen;	     // one header		[m]	 1	 一组集管的长度
	pwr_tFloat32	NbCoolSect;		/* No. of cooling sections 3*/
	pwr_tInt32		NbBankSect[ANZ_SECT];	/* Headers per section 3,9,8 */
	pwr_tFloat32  CoolWidth;		/* Width of ACC [m]  4.3	 */
	pwr_tInt32		NbNozzBank;		/* No of nozzles per header */
	pwr_tFloat32	NbNozzCentr;	/* No of nozzles Center of header */
	pwr_tInt32    nPyr;			/* No. of pyrometers */

	pwr_tFloat32  PosP1;			/* Distance Mill Pyrometer 1 [m] */
	pwr_tInt32    iStatusP1;		/* 状态测温仪 */
	pwr_tFloat32  fTempLowP1;		/* 低  限制测温仪 */
	pwr_tFloat32  fTempHighP1;		/* High 限制测温仪 */

	pwr_tFloat32  PosP4;			/* Distance Mill Pyrometer 4 [m] */
	pwr_tInt32    iStatusP4;		/* 状态测温仪 */
	pwr_tFloat32  fTempLowP4;		/* 低  限制测温仪 */
	pwr_tFloat32  fTempHighP4;		/* High 限制测温仪 */
	pwr_tFloat32  PosP5;			/* Distance Mill Pyrometer 5 [m] */
	pwr_tInt32    iStatusP5;		/* 状态测温仪 */
	pwr_tFloat32  fTempLowP5;		/* 低  限制测温仪 */
	pwr_tFloat32  fTempHighP5;		/* High 限制测温仪 */
	pwr_tFloat32  PosP6;			/* Distance Mill Pyrometer 6 [m] */
	pwr_tInt32    iStatusP6;		/* 状态测温仪 */
	pwr_tFloat32  fTempLowP6;		/* 低  限制测温仪 */
	pwr_tFloat32  fTempHighP6;		/* High 限制测温仪 */

	pwr_tFloat32  PosP23;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP23;		/* 状态测温仪 */
	pwr_tFloat32  fTempLowP23;		/* 低  限制测温仪 */
	pwr_tFloat32  fTempHighP23;		/* High 限制测温仪 */

	pwr_tFloat32  PosP2L;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP2L;		/* 状态测温仪 */
	pwr_tFloat32  fTempLowP2L;		/* 低  限制测温仪 */
	pwr_tFloat32  fTempHighP2L;		/* High 限制测温仪 */

	pwr_tFloat32  PosP3L;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP3L;		/* 状态测温仪 */
	pwr_tFloat32  fTempLowP3L;		/* 低  限制测温仪 */
	pwr_tFloat32  fTempHighP3L;		/* High 限制测温仪 */

	pwr_tFloat32  PosP2U;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP2U;		/* 状态测温仪 */
	pwr_tFloat32  fTempLowP2U;		/* 低  限制测温仪 */
	pwr_tFloat32  fTempHighP2U;		/* High 限制测温仪 */

	pwr_tFloat32  PosP3U;			/* Distance Mill Pyrometer ufc exit [m] */
	pwr_tInt32    iStatusP3U;		/* 状态测温仪 */
	pwr_tFloat32  fTempLowP3U;		/* 低  限制测温仪 */
	pwr_tFloat32  fTempHighP3U;		/* High 限制测温仪 */

	pwr_tInt32    iTempIcEn;		/* Entry pyromter IC (1 to 3) */
	pwr_tInt32    iTempIcEx;		/* Exit  pyromter IC (1 to 5) */
	pwr_tFloat32  PosZon1Entry;	/* Distance Mill Entry Zone 1 [m]	*/
	pwr_tFloat32  PosZon2Entry;	/* Distance Mill Entry Zone 2 [m]	*/
	pwr_tFloat32  PosZon3Entry;	/* Distance Mill Entry Zone 3 [m]	*/
	pwr_tFloat32  PosZon3Exit;	/* Distance Mill Exit Zone 3 [m]	*/

	pwr_tFloat32  MinFlowTop[3];	/* 最小flow Top Sections ^[m^3/h] */
	pwr_tFloat32  MinFlowBtm[3];	/* 最小flow Bot Sections ^[m^3/h] */

	pwr_tFloat32  MaxFlowSect1;	/* 最大 flow Top Section1 ^[m^3/h] */
	pwr_tFloat32  MaxFlowSect2[2]; /* max.  flow Top Section2 ^[m^3/h] */
	pwr_tFloat32  MaxFlowSect3;	/* 最大 flow Top Section1 ^[m^3/h] */
		
	pwr_tFloat32  MaxSpeedTransf; /* max. transfer 速度 to ACC [m/s]  */
	pwr_tFloat32  MinSpeedCool;   /* min. 速度 inside ACC [m/s]  */

	pwr_tFloat32  MaxSpeedCool[8]; /* 速度 for thickness classes [m/s]  */  // modify by xie from 5 to 8   12-1
	pwr_tFloat32  ThickUpLimit[8]; /* corresponding thickness classes [mm] */ // modify by xie from 5 to 8
	
	pwr_tFloat32  DelayFirstTurn;  /* delay time first turn [s]		*/
	pwr_tFloat32  DelaySubsTurn;  /* delay time subsequent turns turn [s]	*/
	pwr_tFloat32  AccTurn;		   /* acceleration reverse turn [m/s^2] */
	pwr_tFloat32  WaterTemp;  	/* water Temperature [度]	*/
	pwr_tFloat32  WaterPress;
	pwr_tFloat32  AirTemp;			/* air Temperature   [度]	*/
	pwr_tFloat32  AlphaCoe;
	pwr_tFloat32  AlphaCoeIndex;

	pwr_tFloat32  AlphaCoeAccWatTemp;
		
	pwr_tInt32    iFilterPyro;		/* 0: no additional level 2 filter */
	/* Synchronization Points: BLS1, BLS2 and gamma rays */
	pwr_tInt32    nBls;     /* No. of synchronization Poits */
	pwr_tInt32    iStatusBls[ANZ_BLS];   /* Status synchronization Poits */
	pwr_tFloat32  fPosBls[ANZ_BLS]; /* position synchronization points */
	char		   sNameBls[ANZ_BLS][50]; /* description */

	pwr_tInt32    nEdge;     /* No. of edge 遮蔽 drives */
	pwr_tInt32    iStatusEdge[ANZ_EDGE];   /* Status drives */
	pwr_tInt32	   iPosEdge[ANZ_EDGE]; /* first cooling unit of drive */
	/* side spray 信息 */

	pwr_tInt32    nSideSpray;     // No. of side sprays  侧喷的相关信息
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
	/* typical distance of 板材 to mill during rolling is required to keep */
	/* 板材 dry if last pass is in direction to ACC */
	/* direction of the last pass is given with the PDI for each 板材 */
	pwr_tFloat32  fPosTakeoverMill; /* Reverse point of 板材 during rolling */ 
}RALCOEFFDATA;


//跟踪数据结构体
typedef struct tagMDSCoolTrackData
{
	RALINDATA RalPdi;
	RALCOEFFDATA RalCoeff;
	RALMESDATA RalMes;
	RALOUTDATA RalOutPut;
}TRACKDATARAL;


//显示规程 显示该钢是怎么冷的
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

	int widShield;  // 宽度遮蔽 [11/29/2013 xie]
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

//测量数据
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


/////////定义用于数据库的结构体 add by Zed 2013.11////////////
typedef struct DBSTORGEMES
{
	/////用于读取返红数据库的变量/////
	char DB_S_Grade[20];
	char DB_P_Grade[20];
	float DB_Thick;
	float DB_RedTemp;

	/////用于存储至MES数据库的变量////
	char Ms_ID[20];
	float Ms_len;
	float Ms_wid;
	float Ms_thk;
	int Ms_CoolUse;
	char Ms_StaTim[32];///开冷时刻
	char Ms_StpTim[32];///终冷时刻
	float Ms_WatTemp;///水温
	float Ms_WatPres;///水压
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
	float Ms_AftRol_Time;////轧后弛豫时间
	float Ms_OperMode;
	float Ms_ThroghMode;
	float Ms_CoolMethod;
	float Ms_CoolMode;
	float Ms_CalTemp_1;////一阶段计算冷却温度
}DBStorgeMes;


	/////////////////////////////////////////////////////////////////////////////////////////
	//冷却
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

////////////数据库结构体//////////
struct DBVal
{
	char DB_Grade[20];
	float DB_Thick;
	float DB_RedTemp;
};

