#pragma once
#include "../BaseCoolStruct.h"
#include "SelfLearnFunction.h"
#include "public.H"
#include <time.h>
#include <cmath>
#include <stdlib.h>
#include <conio.h>
#include <float.h>
#include <cstring>

class  CTemperatureField {
public:
	CTemperatureField(void);

private:
	int 	IntegrationTimeEnd( float , float , COMPUTATIONAL *);
	void 	OutputFunc(               int     i_output,
		FILE 	*ffx,
		FILE 	*ffy,
		float 	*x,
		COMPUTATIONAL *sy,
		float 	*y,
		int 	i_it);

	int EnergyReport(FILE *erg_f, const INPUTDATAPDI *pdi, const COMPUTATIONAL *compx, float *xx);
	float 	EstimationTemperatureDifference( float detaTime, float dt_zero, float *temp_km1,
		float *temp_k, int izaehler, int icc);

	void SetColumn(	float	*x_init,
		const	float	*x,	
		const	int	n);	

	void ColCrNi1(	float	*x_kph,	/* halve time step	*/
		const	float	*TimeKm1AusteniteDistribution,	/* forgoing  k-1	*/
		const	float	*TimeKAusteniteDistribution,		/* predicted step k	*/
		const	int	n);		/* dimension		*/

	float	Norm2FindMax(	const	float	*x,	/*	first column	*/
		const	float	*y,	/*	second column	*/
		const	int	n);	/*	dimension	*/

	float  InitTemperatureDistribution(	COMPUTATIONAL *compx,
		const	MATERIAL *matal,
		float  *temp_k,
		float  *temp_km1);

	void OutputComputational( FILE *ffx, COMPUTATIONAL *xy);


	void    SystemMatrices(       float           *k_mod,  /* modified stiffness-matrix */
		float           *force,  /* modified force column     */
		COMPUTATIONAL *sy,
		const    float          *temp_k,        /* temp: step k               */
		const    float         *temp_kph,      /* temp: step k+1/2   */
		const    float          detaTime,
		const    PHYSICAL *ax,
		const    MATERIAL *matal);

	float   CopyValue(
		const	int		 sw_mat,	/* material type */
		const  MATERIAL  *matal,
		COMPUTATIONAL *sy,
		const   int     i_node,
		const   float   x_old,
		const   float   t_kph,  /* Temp. k + 1/2 in C */
		const   float   t_k);    /* Temp. k  in C */

	float   SearchXValue(    
		const   CURVE *cv,            /* curve */
		const   float   x_old,  /* old 横坐标value */
		const float     y_old,  /* old 纵坐标value */
		const float     y_new,  /* new 纵坐标value */
		const float		t_zykl); /* 实际 time in 冷却区 */

	int	CheckInterval(
		const	float	xa,  /* left boarder 间隔 */
		const	float	xb,  /* right border inteval  */
		const	float	x);   /* value */

	float CalculationMeanTemperature( COMPUTATIONAL *sy, float *y);

	float MiddleArit(      const   COMPUTATIONAL *sy,
		const   float *y);

	void SolveAlgebraicMatrix( float *k,   /*  stiffness matrix  bandwidth 3 symm. */
		float *r,   /*  right upper matrix                  */
		float *y,   /*  column forward substitution         */
		float *b,   /*  column right side                   */
		float *c,   /*  calculated value                   */
		int n);     /*  dimension of the problem            */

	void outpx( float *x, int n, char *str);

public:
	void   SolveBoundary( FILE       *st_err,
		const		  PHYSICAL *xx,
		MATERIAL     *yy,
		const		  INPUTDATAPDI *pdi);


	MATERIAL  CalcMaterialAttribute( FILE *st_err, const	INPUTDATAPDI *pdi, PHYSICAL *xx);

	float	SearchAbsz(	const CURVE *cx,
		const float   x_old, /* old Absz. value */
		const float   y_new);

	int  CalculationTemperatureDistribution(	FILE 			*st_err,
		FILE 		*erg_f ,
		const		INPUTDATAPDI *pdi,
		COMPUTATIONAL *compx,
		const		PHYSICAL *ax,
		const		MATERIAL  	*matal);

	int TimeSolver(	FILE 			*st_err,
		FILE 		*erg_f ,
		const		INPUTDATAPDI *pdi,
		COMPUTATIONAL *compx,
		const		PHYSICAL *ax,
		const		MATERIAL  	*matal);

	int Temperatureolver(	FILE 			*st_err,
		FILE 		*erg_f ,
		const		INPUTDATAPDI *pdi,
		COMPUTATIONAL *compx,
		const		PHYSICAL *ax,
		const		MATERIAL  	*matal);

	int TimeSolverNoFirstOutPut(	FILE 			*st_err,
		FILE 		*erg_f ,
		const		INPUTDATAPDI *pdi,
		COMPUTATIONAL *compx,
		const		PHYSICAL *ax,
		const		MATERIAL  	*matal);

private:
	void AlphaFile( FILE *st_err, ALPHAVALUES *alpha, const	PHYSICAL *xy);

	void AlphaFileByDifferent(
		int	id_file,	/* id for integer FILE	*/
		FILE *st_err,	/* error output			*/
		ALPHAVALUES *alpha,	/* alpha value			*/
		const	   PHYSICAL *xy);

	int	SetAlphaLimits( 	const	PHYSICAL *xy,const	int			ianz,float			*alpha);

	void	AlphaStrip(ALPHAVALUES *alpha,
		const			PHYSICAL *xy,
		const			INPUTDATAPDI *pdi);

	void AlphaAir(FILE *st_err, ALPHAVALUES *alpha, const PHYSICAL *xy);

	void AlphaSprayCool( FILE *st_err, ALPHAVALUES *alpha, const	PHYSICAL *xy);

	void AlphaAirTop( float *alpha, const	PHYSICAL *xy);

private:

	void AlphaFileNotMetal( FILE       *st_err,
		ALPHAVALUES *alpha,
		const			  MATERIALPARAM *ccv,
		const			  PHYSICAL *xy);


	float alphal( int iseite, float v, float temp, float t_um, float grau);

private:
	void    SteelMaterial(FILE       *st_err,
		MATERIALPARAM *ccv,
		const		INPUTDATAPDI *pdi,
		PHYSICAL *xcc);

	void NotSteelMaterial(    FILE       *st_err,
		MATERIALPARAM *ccv,
		const			  INPUTDATAPDI *pdi,
		const			  PHYSICAL *xcc);

	int LoadCurve(char  *f_name, /* name of FILE */
		float	*xx);     /* function value */

	int   EstimationParamName(  char *mat_name, char **file_name);

	void CalcLambda(FILE *st_err, float *lxl, const	INPUTDATAPDI *pdi, 
		const int tmaxi);

	float CalcRhoCx(	FILE 	*st_err, /* error messages	*/
		const  int   c_type,
		MATERIALPARAM *ccv, /* cp - value */
		const	INPUTDATAPDI *pdi,	 /*PDI数据	*/
		PHYSICAL  *xxn); /* 过程 参数	*/

	int	CalculateEnthalphy(	
		const	INPUTDATAPDI *pdi,
		const	int			c_type,
		MATERIALPARAM *ccv, /* cp - value */
		PHYSICAL *xxn); /* 过程 参数	*/

	int	EstimateBasicCoolingCurve(
		const	INPUTDATAPDI *pdi,
		char *air_fname,
		char *wat_fname);

	int	CalculateCP(
		float   *x,  /* cp_alpha */
		float   *y);  /* cp_gamma */

	int	CalculateH(
		float   *x,  /* h_alpha */
		float   *y);  /* h_gamma */

	int     CalcRhoCpx(  float   *rhoc,
		const int ianz);

	float   CalculateAr3TemperatureSAB(   const INPUTDATAPDI *pdi,
		const int     c_type);

	int  ae1_ae3(   const float ae1,
		const float ae3,
		const float ar3,
		const int   ianz,
		float *tempx);

	CURVE InitCurve(  const  float *x,
		const  float *y,
		const  int    n,
		const  int    init, /* 0: zero init 1: init */
		const  int    iabl, /* no. of derivatives */
		const  float  c_id);

	CURVE init_curve_f(
		const  char *f_name,
		const  int    iabl, /* no. of derivatives */
		const  float  c_id);

	int AddValue(
		const int   coolPhaseNumber,
		const int   ianf,
		const int   iende,
		float *xx, /* absz. value */
		float *yy, /* ord. value */
		const CURVE *cx);

	int intervall_id(
		const int   n,
		const int   ix,   /* direction of search */
		const int   istart,
		const float *y,	/* FILE */
		const float ys);

	int SearchFirstPos(
		const int   n,
		const int   istart,
		const float *y,	/* FILE */
		const float ys);

	int SearchFirstNegative(
		const int   istart,
		const float *y,	/* FILE */
		const float ys);

	int intervall_no( const CURVE *cx,
		const float absz);

	int	ausg_curve( CURVE *x,
		char *name,
		int  icall);

	int	spline_1	(	const	CURVE *cx,	/*	curve value */
		const	int	ix,		/*	value +/- ix are incorp. */
		const	float	x,			/*	absz. for interpolation */
		float	*y);		/*	插value value y[0] */

	//no use xie 2012-4-2
	int	spline_2	(	const	CURVE *cx,	/*	curve value */
		const	int	ix,		/*	value +/- ix are incorp. */
		const	int	iend,		/*	end for interpolation */
		float	*y,		/*	插value value  */
		float *yp1,		/* first 偏差	   */
		float *yp2);		/* sec 偏差	   */

	spline_t  InitSpline(	const	int	n,	/* number of function value */
		const	int	i,	/*	required order of spline */
		/* +/- i absz. value */
		const	int	izw);

	float	CalculateSplineValue(	const	spline_t	*spl,	/*	pointer to spline */
		const	float		*xx,	/*	横坐标value */
		const	float		x);		/*	横坐标value */

	float	CalculateFirstDerivative(	const	spline_t	*spl,	/*	pointer to spline */
		const	float		*xx,	/*	横坐标value */
		const	float		x);	/*	横坐标value */

	float	CalculateSecondDerivative(	const	spline_t	*spl,	/*	pointer to spline */
		const	float		*xx,	/*	横坐标value */
		const	float		x);		/*	横坐标value */

	int	EstimateStartIndex(	const	float	x,
		const	int	imax,		/*	maximum index */
		const	float	*xx);	/*	横坐标value */

	int	coef_spline(	const	CURVE *cx,	/*	curve     */
		spline_t *spl);	/*	spline    */

	int	CalculateMat(	const float		*xi,
		const	float		*yi,
		spline_t	*spl);

	int	IntervalIndexStart(const	spline_t	*spl);

private:

	void alpha_calc_ne( FILE       *st_err,
		ALPHAVALUES *alpha,
		const				  MATERIALPARAM *ccv,
		const				  PHYSICAL *xy);

	int alpha_ne_wat( int        seite,
		float     *val,
		const			  MATERIALPARAM *ccv,
		const			  PHYSICAL *xy);

	float	CalcCarbenGam(	const float temp);

	float	CalcCarbenAl(	const float t);

	float CalculateIntegalPoly(	const float x,
		const int order,
		const float am1,
		const float *a);

	float	CalcHGam(	const float temp);

	float	CalcHAl(	const float t);

	float InterpolateAbszizzeY(
		const	CURVE *cx,
		const	int iint,
		const float	y_new);

	int vealb(				float	*a,
		const	float	elmax,
		const	int	n,
		const	int	ibw);

	int	 voruva(		const	float	*a,
		const	float	*b,
		float	*x,
		const	int	ibw,
		const	int	m,
		const	int	n,
		const	int	mb);

	void	fehler_id(      int	igroup,		/* Fehlergruppe */
		const	char	*melde); 	/*	Botschaft */

	void   r_cp_file( const	INPUTDATAPDI *pdi, const int taskID,  char *file_name);


private:

	int TimeEnd( float timex, float time_mean, COMPUTATIONAL *compx);

	int TemperatureEnd( float timex, float time_mean, COMPUTATIONAL *compx);

	int AlphaACCJET( ALPHAVALUES *alpha, const PHYSICAL *xy,float thickness); /* 参数   */

	int AlphaACCSLIT( ALPHAVALUES *alpha, const PHYSICAL *xy); /* 参数   */

	int AlphaUFCJET( ALPHAVALUES *alpha, const PHYSICAL *xy); /* 参数   */

	int AlphaUFCSLIT( ALPHAVALUES *alpha, const PHYSICAL *xy); /* 参数   */

};


static CTemperatureField m_TemperatureField;