#pragma once
#ifndef   MATHEMATICAL
#define   MATHEMATICAL

#include "../BaseCoolStruct.h"

//replace zero entry within array (one value)
int	FilterArray(const	int		nMax, float	*pfValueArray, const	float	eps);	/*最小value */

// add  [2/19/2014 xie]
void AnalyseOrthostate(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,/*最小value */
	const	float	diff,  //偏差范围
	const   float  TarValue,
	float	*aveEftRate,
	float	*tarEftRate);


//analysis of array x length n
int	AnalyseArray(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,	/*最小value */
	float	*pfAverage,	/* 平均value	*/
	float	*pfStandard,	/* 标准差 */
	float	*pfMin,	/*最小value */
	float	*pfMax,	/* 最大value */
	int		*pnNonzeroStart, /*第一个非零 */
	int		*pnNonzeroEnd); /* 最后一个非零 */

// 用于处理扫描测温仪的数据 [1/2/2014 xie]
int	AnalyseArraySp(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,	/*最小value */
	float	*pfAverage,	/* 平均value	*/
	float	*pfStandard,	/* 标准差 */
	float	*pfMin,	/*最小value */
	float	*pfMax,	/* 最大value */
	int		*pnNonzeroStart, /*第一个非零 */
	int		*pnNonzeroEnd); /* 最后一个非零 */



// analysis of array x with nx nonzero elements reverse direction     
int	AnalyseArrayReverse(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,	/*最小value */
	float	*pfAverage,	/* 平均value	*/
	float	*pfStandard,	/* 标准差 */
	float	*pfMin,	/*最小value */
	float	*pfMax,	/* 最大value */
	int		*pnNonzeroStart, /*第一个非零 */
	int		*pnNonzeroEnd); /* 最后一个非零 */

//check for value within limits 
int	GetValidValue(const float fActual, const float fMin, const float fMax, const float fDefault, float *fUse);


//线性内插 : single value / linear
int GetLinearInterpolation(const	float fAbscissaValue, const int nPos, const	float *fAbscissaArray, const float *y, float *ord);


// estimate intervall id for 插value value 
int	GetInterpolatedIntervalId(const	float fAbscissaValue, const int	nPos, const	float *fAbscissaArray);


// estimate intervall id for 插value value reverese direction 
int	GetInterpolatedIntervalIdRev(const	float fAbscissaValue, const int	nPos, const	float *fAbscissaArray);

// 线性内插 : array , const incr. / 线性内插 of value and time (t, p increasing) 
int GetLinearInterpolation(const	float dx, const   float xp,  const    int  nPos, const	float *fTimeArray, const   float *p, const   float *pfValue, float   *fInterpolationTimeArray,  float    *fInterpolationArray);

int GetLinearInterpolationLengthLimit(const	float dx, const   float xp, const    int  nPos, const	float *fTime, const   float *p, const   float *pfValue, float *fInterpolationTimeArray,  float *fInterpolationArray);

int GetInterpolationTime(const	float fInterpolationIncrement,	const	float fLength, const   float fPoint, const    int  n,	const	float *fTime, const   float *fPosition, float	*fInterpolationTimeArray) ;

// run time between points 1 and 2                           
// time and position value are available                    
float	GetIntervalTimeAvailable(const   float fPointOne, const   float fPointTwo,  const    int  n,	const	float *fTime,		const   float *fPosition);

// run time between points 1 and 2                            
// position and 速度value are available                    
float	GetIntervalTimeSpeed(const   float fPointOne,const   float fPointTwo, const    int  n,	const	float *fPosition,	const   float *fSpeed);
//  modify function to limit value                 
//  - first 横坐标 value is fLimitLow             
//  - highest 横坐标 value is fLimitHigh          
//  - don't change value if limit value outside   
//    横坐标 value                               
int	ModifyLimits(const	int	n,	const float fLimitLow, const float fLimitHigh, float *x, float	*y);


// 线性内插 of function value y(absz)
float   Interpolation(const   int      n, const   float   fAbsz, const   float   *x, const   float   *y);

float MaxF(const	float	x1,const	float	x2);
float MinF(const	float	x1,const	float	x2);

int	AnalyseArrayFilter(const	int		n, const float	*x, const	float	eps, float	*fMean,	float	*var, float	*fMin, float	*fMax, int		*nStart, int		*nEnd);

#endif