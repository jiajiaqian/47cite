
/*=====================================================================================
*Copyright (c) 2009, ������ѧ���Ƽ����������Զ��������ص�ʵ����(RAL)
*All rights reserved
*--------------------------------------------------------------------------------------
*�ļ����ƣ�
*FILE��ʶ��
*ժ    Ҫ��
*--------------------------------------------------------------------------------------
*��ǰ�汾��1.0
*��    ��: 
*������ڣ�2009��10��10��
*---------------------------------------------------------------------------------------
*ȡ���汾��1.0
*ԭ����  ��
*������ڣ�2009��10��10��
======================================================================================*/
/*--------------------------------------------------------------------------------------
*�������ܣ���������Temperature����������ƽ��Temperature
*���������
*���������
*����value  ��
*������  ��

*�Ӻ���  ��
--------------------------------------------------------------------------------------*/
#pragma once
//#include <crtdbg.h>
#include <cmath>
//////////////////////////////////////////////������������////////////////////////////////////////////////
/*����*/

#define	pwr_tInt32                 int
#define  pwr_tFloat32               float
#define  pwr_tChar               	char
#define	pwr_tBoolean					int

#define CALLOC(n,x)     ( (x*) calloc(n,sizeof(x)) )
//#define MALLOC(x)       ( (x*) malloc(sizeof(x)) )

/////////////////////////////////////////////���ճ���///////////////////////////////////////////////

#define		DIR_PRE_RES	"Log\\postprocess\\"

#define  VALUE_PER_CURVE  101 /* value per curve    */
#define		N	500		//s7 �籨����
#define	 NUMBER_OF_POST_RESULT	40	/* No. of results POSTPROCESS */
#define  MAX(a,b) ((a)>(b) ? (a) : (b))
#define  MIN(a,b) ((a)<(b) ? (a) : (b))

#define  BITX(N)    (1 << (N))  

	/* define ��� �ļ���С of report files (bytes)		*/
	/* if report FILE exceeds limit FILE will be renamed	*/
	/* form: name.txt to: name_prev.txt */
#define	MAX_NUMBER_ERROR_FILE	128000L
	/*  Macros for space allocation */
	/*     declaration of data types   */

	/* ���� different tasks */
#define  PREPROCESS       1
#define  POSTPROCESS      2
#define  PRE_CALC_SIM   3  /* simulation PREPROCESS */

#define  ONLINE_FIRST          1  /* online - first call in PREPROCESS */
#define  ONLINE_SURCCESSFULL          2  /* online - successor calls ONLINE */

#define  FALSE          0
#define  TRUE           1

#define LATENT_HEAT_MULTIPLIER           0.1f /* multiplier for latent heat */
#define NODES_NUMBER           19    /* number of thickNodeNumber     */
#define AUSTENITIC_PERCENT         101    /* percent of ������ֽ� */
#define TEMPERATURE_CURVE_NUMBER            100  /* number of points for RR */
	/* pre_result curves       */

#define L_TEL    500			/* length of output telegram 250 */
#define PATH_MAX 256			/* ��� length of �ļ��� + path */
#define	MAX_ID			40		/* ��� number of characters for ��� ID */
	/* and ��ȴģʽ */
#define NMAXROW			100		/* No. of ��� rows input data table */

#define LENGTH_MASK       180    /* index of first value length �ڱ� in telegram 1300 */
#define AIR_1              40 /* index curve���� (10 value) */
#define CR_ST              60    /* index of first CR-element (10 value ) */
#define WAT_1			  230   /* first index ˮ�� curve (8 value) */

#define	ANZ_SECT		   3  /* No. of different cooling sections */
#define NUZZLE_NUMBER      12  // number of headers  NG4700 by Zed
#define GAP_NUZZLE_NUMBER   2  // ��϶���ܸ��� [11/13/2013 лǫ]

#define L_EQUIPMENT        18.0 //add by wbxang 20120505
#define L_OSC_MAX          15.0 //add by wbxang 20120506

#define ANZ_EDGE		   4  /* No. edge �ڱ� drives */
#define ANZ_SIDESPRAY	   4  /* No. of side sprays */
#define FIRSTHEADERMAX	   4  /* index first active header / max. value */
 
#define EDGEWIDTHMIN   0.1 /* [m]  switch width for edgemasking active */
#define EDGEPOSMIN     10  /* [mm] ��Сfor edgemasking  */
#define EDGEPOSMAX     160 /* [mm] ��Сfor edgemasking  */

#define PASS_THROUGH_MODEL 1
#define OSC_IN_MODEL 2
#define OSC_OUT_MODEL 3
#define TCR_MODEL 4

#define OPEN_STATUS_SLIT4 1
#define OPEN_STATUS_SLIT2 2
#define OPEN_STATUS_SUC_JET 3
#define OPEN_STATUS_SEP_JET 4

#define ACTIVE_SECTION_FRONT 1
#define ACTIVE_SECTION_BACK 2
#define ACTIVE_SECTION_TWO 3

#define OPEN_FROM_FIRST_SLIT 1
//#define OPEN_FROM_THIRD_SLIT 2   //лǫ ��ʱ���ô˱�־λ
#define OPEN_FROM_FIRST_JET 3





#define ANZ_PYRO           6    /* No of pyrometers */
/* synchronization points: Entry BLS, Exit BLS, Gamma Rays */
#define	ANZ_BLS			   11   /* No. of synchronization points */
#define ANZ_HUB			   10	/* No. of transfers during cooling */

#define NTHK             50 /* ��� No. of thickness steps */
#define PLATE_SPEED   2.0f  /* transfer �ٶ� of the ��� to cooling */
#define PLATE_SPD_HPL 1.0f  /* transfer �ٶ� ��� to HPL [m/s] */
#define PLATE_EX_HPL  2.0f  /* safety distance to switch [m] */	
#define	SPD_MIN		  0.5f  /* ��Сspeed roller table     see layout.dat*///changed by wbxang 20120505
#define SPD_MAX		  2.0f  /* ��� �ٶ� roller table     see layout.dat*/
#define	ACC_TURN_DEF  1.0f  /* default acceleration RT [m/s^2] */
#define NSPDPROFILE	    20  /* ���value �ٶ� profile */
#define EPSPOS       30.0f  /* allowed position error */
#define TIME_INCR_MIN 0.001f /* ��Сtime increment [sec] for plausible data */

/* required for size of arrays parameter value */
#define MAXTEMP         1800	/* ��� Temperature range */
#define FURNACE_TEMP    1150    /* Temperature at furnace exit   */
#define DD_TEMP     50.f    /* add air-temp for initial temp distr. vs. thickness */
/* interphase cooling, IC_ALPHA_TEMP = 0.7 indicate 70% ˮ�� */
/* remaining 30%���� for bloom back purpose */ 
#define IC_ALPHA_TEMP   0.66f 
// #define IC_ALPHA_TEMP  

/* x1) bounds for ���Ʋ��� */
/* bounds for Temperature drop in water (0.7 => 70 % Temperature drop in water */
#define TEMP_DROP_WATER_MIN	0.5f
#define TEMP_DROP_WATER_MAX	1.2f

/* �ٶ� ����ion (determined �ٶ� profile is multiplied with value */
#define SPD_COR_MIN	0.8f
#define SPD_COR_MAX	1.2f
/* ����ion No. of headers */
#define HEADER_COR_MIN	0.5f
#define HEADER_COR_MAX	1.5f
/* ����ion No. of strokes oscillation mode */
#define STROKE_COR_MIN	0.4f
#define STROKE_COR_MAX	1.6f

/* ��Сthickness [m] required for oscillations */
#define	THICK_OSC_MIN	0.06f
/* ��СTemperature difference required for oscillation */
#define	TEMP_OSC_MIN	100.0f
#define	HEAD_OSC_FIRST	4	/* first header oscillation */
/* end x1) */

#define FLOW_N          10		/* No. of flow steps for calculation */

#define FLOWT           750.f     /* highest possible flow rate at top     */
#define FLOWTMIN        450.f     /* lowest possible flow rate at top     */
#define FLOWB          2180.f     /* highest possible flow at bottom       */
#define PEDGE           100.f     /* position of edge �ڱ� in mm        */

#define	MAT_STEEL		1	/* steel material */
#define MAT_ALU			2	/* aluminum material */
#define MAT_CU			3	/* copper */
#define MAT_OTHER		4	/* other material: properties from files */

#define	TEMP_EQUALIZE_AIR	15.0	/* max. allowed Temperature for bloomback */

/* Air Temperature 1 m beside line */
/* water Temperature measured in overhead tank or main distribution pipe */

#define	TEMP_WAT_MIN	0.f
#define	TEMP_WAT_MAX   75.f

#define	TEMP_AIR_MIN  -30.f
#define	TEMP_AIR_MAX  75.f

#define PRESS_WAT_MIN 0.0f
#define PRESS_WAT_MAX 10.0f


	// used to define Log Language LANGUAGE_ENGLISH LANGUAGE_GERMAN
#define LANGUAGE_ENGLISH

	
#define NSEGREP 20 /* ���value report FILE */


#define		ST_ERR		"st_err.txt"
#define		CONTROL1	"control1.dat"

//#define MAX_MEASURE_VALUE    800  /* ��� No. of measured positions / times */   ԭ 2012-5-31
#define MAX_MEASURE_VALUE    1200  /* ��� No. of measured positions / times */
	/* 3 min timout 4 samples/sec */
#define NFLOW   200  /* set points flow zones */

	/* ------- remove lines for ���� installation SSAB: start --------- */

	/* ���� status ��Ϣ */

#define	SIMULATION		1	
#define	����			2	

	/* ���� operational modes for cooling model */
/////////////////////// changed by wbxang 20120506
#define	NOACC			   0		/* no ACC - no activity */
#define ACC                1		/* accelerated cooling	*/
#define	UFC				   2		/* interphase cooling	*/
#define ACCUFC             3       /* ultra fast cooling   */
#define UFCACC             4		/* direct quenching     */
#define DQ				   5		/* soft cooler */
#define EXP				   6		/* experience */  //------added by xiaoxiaohsine 20110610------//

#define BACKACC     7       //ֻ���������鼯�ܵ�ģʽ  лǫ 2012-7-12

#define IC                 10
#define SC                 20

#define MESSAGE_MAX_LENGHT     256     /* ��� length of message*/


/* ��� Temperature will be ����ed based on layout and thickness */
#define	TEMPMIN		-40.0		/* �� limit measured Temperature */
#define	TEMPMAX		1200.0		/* ��� limit measured Temperature */


//////////////////////////////////////////���̳���//////////////////////////////////////////

#define	ERR_FIL								"error\\errorhelp"


///////////////////////////////////// error alarm ////////////////////////////////////////                                    

//  [8/8/2013 raincls ���õ�]
#define	 ERR_AFT_FINISH_EXIST_FILE		    300
#define  ERR_AFT_FINISH_FULL_FILE			ERR_AFT_FINISH_EXIST_FILE + 1
#define  ERR_AFT_LAMINAR_EXIST_FILE		    ERR_AFT_FINISH_EXIST_FILE + 7
#define  ERR_UND_LAMINAR_EXIST_FILE			ERR_AFT_FINISH_EXIST_FILE + 10
#define  ERR_UND_LAMINAR_FULL_FILE			ERR_AFT_FINISH_EXIST_FILE + 11	
#define  ERR_AFT_RBT_ID_EMPTY_FILE			ERR_AFT_FINISH_EXIST_FILE + 14
#define  ERR_TAIL_FCT_ID_EMPTY_FILE			ERR_AFT_FINISH_EXIST_FILE + 16

///////////////////from pre_ctrl.c/////////////////////
#define IOUT 2

///////////////////from d_flowrx.c//////////////////
#define ANZ_THICK   9      /* max. number of thickness classes in FILE: flow_rr.dat */
#define DEBUG_wrong 0

//add by wbxang 20120521 
//#define DISTANCE_MILL_TO_COOL 20.0f
#define DISTANCE_MILL_TO_COOL 74.2f

#define LENGTH_PRELEVEL 6.5f

/////////////////////////Tracking FLAG//////////////////////////////
#define	 AUTO_MODE         2

/////////////////////////////////////////////////////////////////////////////
//#define FIN_QUE_NUM        8
#define FIN_QUE_NUM        1  //лǫ �޸� 2012-7-4  // from 4 to 1 [12/31/2013 xie]

#define AFT_FIN_QUE_NUM    3
#define UND_COOL_QUE_NUM   3
#define AFT_COOL_QUE_NUM   3


#define EDGEMASKING_NUMBER 3
#define POSITION_NUMBER 20
#define SIDESPRAY_NUMBER 8

#define DAT_ACC      "Logs\\NG_lay1.dat"
#define ST_ERR_FILE  "Logs\\st_err.txt"