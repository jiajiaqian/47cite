#pragma once
#ifndef   MATHEMATICAL
#define   MATHEMATICAL

#include "../BaseCoolStruct.h"

//replace zero entry within array (one value)
int	FilterArray(const	int		nMax, float	*pfValueArray, const	float	eps);	/*��Сvalue */

// add  [2/19/2014 xie]
void AnalyseOrthostate(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,/*��Сvalue */
	const	float	diff,  //ƫ�Χ
	const   float  TarValue,
	float	*aveEftRate,
	float	*tarEftRate);


//analysis of array x length n
int	AnalyseArray(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,	/*��Сvalue */
	float	*pfAverage,	/* ƽ��value	*/
	float	*pfStandard,	/* ��׼�� */
	float	*pfMin,	/*��Сvalue */
	float	*pfMax,	/* ���value */
	int		*pnNonzeroStart, /*��һ������ */
	int		*pnNonzeroEnd); /* ���һ������ */

// ���ڴ���ɨ������ǵ����� [1/2/2014 xie]
int	AnalyseArraySp(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,	/*��Сvalue */
	float	*pfAverage,	/* ƽ��value	*/
	float	*pfStandard,	/* ��׼�� */
	float	*pfMin,	/*��Сvalue */
	float	*pfMax,	/* ���value */
	int		*pnNonzeroStart, /*��һ������ */
	int		*pnNonzeroEnd); /* ���һ������ */



// analysis of array x with nx nonzero elements reverse direction     
int	AnalyseArrayReverse(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,	/*��Сvalue */
	float	*pfAverage,	/* ƽ��value	*/
	float	*pfStandard,	/* ��׼�� */
	float	*pfMin,	/*��Сvalue */
	float	*pfMax,	/* ���value */
	int		*pnNonzeroStart, /*��һ������ */
	int		*pnNonzeroEnd); /* ���һ������ */

//check for value within limits 
int	GetValidValue(const float fActual, const float fMin, const float fMax, const float fDefault, float *fUse);


//�����ڲ� : single value / linear
int GetLinearInterpolation(const	float fAbscissaValue, const int nPos, const	float *fAbscissaArray, const float *y, float *ord);


// estimate intervall id for ��value value 
int	GetInterpolatedIntervalId(const	float fAbscissaValue, const int	nPos, const	float *fAbscissaArray);


// estimate intervall id for ��value value reverese direction 
int	GetInterpolatedIntervalIdRev(const	float fAbscissaValue, const int	nPos, const	float *fAbscissaArray);

// �����ڲ� : array , const incr. / �����ڲ� of value and time (t, p increasing) 
int GetLinearInterpolation(const	float dx, const   float xp,  const    int  nPos, const	float *fTimeArray, const   float *p, const   float *pfValue, float   *fInterpolationTimeArray,  float    *fInterpolationArray);

int GetLinearInterpolationLengthLimit(const	float dx, const   float xp, const    int  nPos, const	float *fTime, const   float *p, const   float *pfValue, float *fInterpolationTimeArray,  float *fInterpolationArray);

int GetInterpolationTime(const	float fInterpolationIncrement,	const	float fLength, const   float fPoint, const    int  n,	const	float *fTime, const   float *fPosition, float	*fInterpolationTimeArray) ;

// run time between points 1 and 2                           
// time and position value are available                    
float	GetIntervalTimeAvailable(const   float fPointOne, const   float fPointTwo,  const    int  n,	const	float *fTime,		const   float *fPosition);

// run time between points 1 and 2                            
// position and �ٶ�value are available                    
float	GetIntervalTimeSpeed(const   float fPointOne,const   float fPointTwo, const    int  n,	const	float *fPosition,	const   float *fSpeed);
//  modify function to limit value                 
//  - first ������ value is fLimitLow             
//  - highest ������ value is fLimitHigh          
//  - don't change value if limit value outside   
//    ������ value                               
int	ModifyLimits(const	int	n,	const float fLimitLow, const float fLimitHigh, float *x, float	*y);


// �����ڲ� of function value y(absz)
float   Interpolation(const   int      n, const   float   fAbsz, const   float   *x, const   float   *y);

float MaxF(const	float	x1,const	float	x2);
float MinF(const	float	x1,const	float	x2);

int	AnalyseArrayFilter(const	int		n, const float	*x, const	float	eps, float	*fMean,	float	*var, float	*fMin, float	*fMax, int		*nStart, int		*nEnd);

#endif