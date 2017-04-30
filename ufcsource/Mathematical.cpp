/*! 
* Copyright (c) 2013, ������ѧ���Ƽ����������Զ��������ص�ʵ����(RAL)
* All rights reserved
* ��ѧ��ʽ��
* FILE Mathematical.cpp
* ˵�� ����  
*  
* ��ϸ˵��
*  
* �汾 1.0 ���� Hanbing ���� 8/6/2013
* 
*/
#pragma once
#include    "stdafx.h"
#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	<stdlib.h>
#include	<float.h>
#include	"Mathematical.h"


#define  EPS .1e-12f
#ifndef FALSE
#define FALSE  0
#endif



// add  [2/19/2014 xie]
void AnalyseOrthostate(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,/*��Сvalue */
	const	float	diff,  //ƫ�Χ
	const   float  TarValue,
	float	*aveEftRate,
	float	*tarEftRate)
{
	float	fSum1  = 0.0f;
	float	fSum2  = 0.0f;
	float totalNum=0;
	int effectNum=0;
	int tarEftNum=0;
	int i=0;

	for (i=0;i<nMax;i++)
	{
		if (pfValueArray[i]>eps)
		{
			fSum1+=pfValueArray[i];
			totalNum++;
			if ((pfValueArray[i]>TarValue-diff)&&(pfValueArray[i]<TarValue+diff))
			{
				tarEftNum++;
			}
		}
	}

	if (totalNum>1) 	
	{
		float aveValue=fSum1/totalNum;
		for (i=0;i<nMax;i++)
		{
			if ((pfValueArray[i]>aveValue-diff)&&(pfValueArray[i]<aveValue+diff))
			{
				fSum2+=fSum2;
				effectNum++;
			}
		}
		*aveEftRate=effectNum/totalNum;
		*tarEftRate=tarEftNum/totalNum;
	}
	else
	{
		*aveEftRate=0.0;
		*tarEftRate=0.0;
	}
}



/// @����˵�� ����x��NX����Ԫ�ط��� 
///
/// -	analysis of data from��һ������ element
/// 	up to next zero element
/// 	-	calculation of 
/// 	a)	ƽ��value of nonzero elements
/// 	b)	��׼��
/// 
/// Definition  :
/// 
/// nonzero element:	elment x with |x| > eps
/// 	eps > 0
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int	AnalyseArray(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,	/*��Сvalue */
	float	*pfAverage,	/* ƽ��value	*/
	float	*pfStandard,	/* ��׼�� */
	float	*pfMin,	/*��Сvalue */
	float	*pfMax,	/* ���value */
	int		*pnNonzeroStart, /*��һ������ */
	int		*pnNonzeroEnd) /* ���һ������ */
{
	
	float	fSum1  = 0.0f;
	float	fSum2  = 0.0f;
	*pfAverage = 0.0f;
	*pfStandard = 0.0f;
	*pfMin = 0.0f;
	*pfMax = 0.0f;
	*pnNonzeroStart = 0;
	*pnNonzeroEnd  = 0;

	int	nPos = -1;
	int	nResult = 0;
	float	fValue = 0.0f;
	while(++nPos < nMax)	{				/* all elements */
		fValue = pfValueArray[nPos];
		if(fabs(fValue) > eps )	{	/* nonzero el. */
			if(nResult == 0)	{
				*pfMin = fValue;
				*pfMax = fValue;
			}
			nResult++;
			*pnNonzeroEnd = nPos;
			fSum1  += fValue;
			fSum2  += fValue * fValue;
			if(fValue < *pfMin) *pfMin = fValue;
			if(fValue > *pfMax) *pfMax = fValue;
		}							/* nonzero el. */
		else {					/* check for last value */	
			if(nResult > 0)	
				break;
			else			
				*pnNonzeroStart = nPos+1;
		}							/* check for last value */	
	}								/* all elements */
	if(nResult > 0) 
		*pfAverage  =  fSum1/nResult;
	if(nResult > 1) 
		*pfStandard =  (float)(sqrt((fSum2 - fSum1*fSum1/nResult)/(nResult-1)));

	return nResult;
}

//int	AnalyseArraySp(
//	const	int		nMax,		/* max. no. val */
//	const	float	*pfValueArray,		/* value		*/
//	const	float	eps,	/*��Сvalue */
//	float	*pfAverage,	/* ƽ��value	*/
//	float	*pfStandard,	/* ��׼�� */
//	float	*pfMin,	/*��Сvalue */
//	float	*pfMax,	/* ���value */
//	int		*pnNonzeroStart, /*��һ������ */
//	int		*pnNonzeroEnd) /* ���һ������ */
//{
//
//	float	fSum1  = 0.0f;
//	float	fSum2  = 0.0f;
//	*pfAverage = 0.0f;
//	*pfStandard = 0.0f;
//	*pfMin = 0.0f;
//	*pfMax = 0.0f;
//	*pnNonzeroStart = 0;
//	*pnNonzeroEnd  = 0;
//
//	int	nPos = 100;
//	int	nResult = 0;
//	float	fValue = 0.0f;
//	while(++nPos < nMax)	{				/* all elements */
//		fValue = pfValueArray[nPos];
//		if(fabs(fValue) > eps )	{	/* nonzero el. */
//			if(nResult == 0)	{
//				*pfMin = fValue;
//				*pfMax = fValue;
//			}
//			nResult++;
//			*pnNonzeroEnd = nPos;
//			fSum1  += fValue;
//			fSum2  += fValue * fValue;
//			if(fValue < *pfMin) *pfMin = fValue;
//			if(fValue > *pfMax) *pfMax = fValue;
//		}							/* nonzero el. */
//		else {					/* check for last value */	
//			if(nResult > 10)	
//				break;
//			else			
//				*pnNonzeroStart = nPos+1;
//		}							/* check for last value */	
//	}								/* all elements */
//	if(nResult > 0) 
//		*pfAverage  =  fSum1/nResult;
//	if(nResult > 1) 
//		*pfStandard =  (float)(sqrt((fSum2 - fSum1*fSum1/nResult)/(nResult-1)));
//
//	return nResult;
//}

	//ti chu qianliangge dian2015-11-12 xie
int	AnalyseArraySp(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,	/*��Сvalue */
	float	*pfAverage,	/* ƽ��value	*/
	float	*pfStandard,	/* ��׼�� */
	float	*pfMin,	/*��Сvalue */
	float	*pfMax,	/* ���value */
	int		*pnNonzeroStart, /*��һ������ */
	int		*pnNonzeroEnd) /* ���һ������ */
{

	float	fSum1  = 0.0f;
	float	fSum2  = 0.0f;
	*pfAverage = 0.0f;
	*pfStandard = 0.0f;
	*pfMin = 0.0f;
	*pfMax = 0.0f;
	*pnNonzeroStart = 0;
	*pnNonzeroEnd  = 0;

	int	nPos = 100;
	int	nResult = 0;
	float	fValue = 0.0f;
	//int nBreakCount=0;

	while(++nPos < nMax)	{				/* all elements */
		fValue = pfValueArray[nPos];
		if(fabs(fValue) > eps )	{	/* nonzero el. */
			if(nResult == 0)	{
				*pfMin = fValue;
				*pfMax = fValue;
			}
			nResult++;
			*pnNonzeroEnd = nPos;
			if(nResult>2)
			{
				fSum1  += fValue;
				fSum2  += fValue * fValue;
				if(fValue < *pfMin) *pfMin = fValue;
				if(fValue > *pfMax) *pfMax = fValue;
			}			
		}							/* nonzero el. */
		else {					/* check for last value */	
			if(nResult > 10)	
				break;
			else			
				*pnNonzeroStart = nPos+1;
		}							/* check for last value */	
	}								/* all elements */

	nResult=nResult-2;  //��ȥ��ͷ������Ԫ��

	if(nResult > 0) 
		*pfAverage  =  fSum1/nResult;
	if(nResult > 1) 
		*pfStandard =  (float)(sqrt((fSum2 - fSum1*fSum1/nResult)/(nResult-1)));

	return nResult;
}



/*
-	analysis of data from ���һ������ element
down to next zero element
-	calculation of 
a)	ƽ��value of nonzero elements
b)	��׼��
*/
/// @����˵�� ������Ҫ˵��-���Ժ��� ????????
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int	AnalyseArrayReverse(
	const	int		nMax,		/* max. no. val */
	const	float	*pfValueArray,		/* value		*/
	const	float	eps,	/*��Сvalue */
	float	*pfAverage,	/* ƽ��value	*/
	float	*pfStandard,	/* ��׼�� */
	float	*pfMin,	/*��Сvalue */
	float	*pfMax,	/* ���value */
	int		*pnNonzeroStart, /*��һ������ */
	int		*pnNonzeroEnd) /* ���һ������ */
{
	

	int	nResult = 0;	
	float	fValue = 0.0f;
	float	fSum1  = 0.0f;
	float	fSum2  = 0.0f;
	*pfAverage = 0.0f;
	*pfStandard = 0.0f;
	*pfMin = 0.0f;
	*pfMax = 0.0f;
	*pnNonzeroStart = 0;
	*pnNonzeroEnd  = 0;
	int	nPos = nMax;
	while(--nPos >= 0){				/* all elements */
		fValue = pfValueArray[nPos];
		if(fabs(fValue) > eps ){	
			if(nResult == (nMax - 1)){
				*pfMin = fValue;
				*pfMax = fValue;
			}
			nResult++;
			*pnNonzeroStart = nPos;
			fSum1  += fValue;
			fSum2  += fValue * fValue;
			if(fValue < *pfMin) *pfMin = fValue;
			if(fValue > *pfMax) *pfMax = fValue;
		}						
		else{					/* check for last value */	
			if(nResult > 0)	
				break;
			else			
				*pnNonzeroEnd = nPos - 1;
		}							
	}								
	if(nResult > 0) *pfAverage  =  fSum1/nResult;
	if(nResult > 1) *pfStandard =  (float)(sqrt((fSum2 - fSum1*fSum1/nResult)/(nResult-1)));
	return(nResult);
}

/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// @���� nMax ���ֵ
/// @���� pfValueArray ��Ҫ����������
///	@���� eps ����
/// ����
/// ���� Hanb
/// ���� 7/25/2013
int	FilterArray(const	int		nMax, float	*pfValueArray, const	float	eps)
{
	int	nPos = -1;
	int	nResult = 0;	
	float	fValue;
	float	fPointOne;
	float	xm1;

	while(++nPos < nMax)	{	/* all elements */
		fValue = pfValueArray[nPos];
		if(fabs(fValue) < eps ){	/* found zero element */
			if(	nPos < (nMax -1)){		/* not last element */
				fPointOne = pfValueArray[nPos+1];
				if((nPos == 0) && (fPointOne > eps)){
					nResult++;
					pfValueArray[0] = fPointOne;
				}
				if(	(nPos > 0) &&	(fPointOne > eps)){	/* element within */
					xm1 = pfValueArray[nPos-1];
					if(xm1 > eps){
						nResult++;
						pfValueArray[nPos] = 0.5f * (xm1 + fPointOne);
					}
				}			
			}			
		}				
	}
	return nResult;
}

/// @����˵�� �ж� y_act �Ƿ���ָ���ķ�Χ��
/// �������ָ���ķ�Χ���򷵻�Ĭ��value
/// ����
/// ���� Hanb
/// ���� 7/25/2013
int	GetValidValue(const float fActual, const float fMin, const float fMax, const float fDefault, float *fUse) 
{
	if(fActual < fMin){
		*fUse = fDefault;
		return(-1);
	}
	if(fActual > fMax){
		*fUse = fDefault;
		return(1);
	}
	*fUse = fActual;
	return(0);
}


/* estimate intervall id for ��value value */
/* output: left ������ value */
/* output:  -1: fAbsz < x[0]     */
/* output: n-1: fAbsz > x[n-1]   */
/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013

int	GetInterpolatedIntervalId(const	float fAbscissaValue, const int	nPos, const	float *fAbscissaArray)
{
	int nResult = -1;
	while(++nResult < nPos)	
	{
		if(fAbscissaValue < fAbscissaArray[nResult]) 
			return nResult;
	}
	return nResult;
}

/* estimate intervall id for ��value value */
/* reverese direction                           */
/* output: left ������ value */
/* output:  -1: fAbsz < x[n-1]     */
/* output: n-1: fAbsz > x[0]   */
/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int	GetInterpolatedIntervalIdRev(const	float fAbscissaValue, const int	nPos, const	float *fAbscissaArray)
{
	int nResult = -1;
	while(++nResult < nPos)	{
		if(fAbscissaValue > fAbscissaArray[nResult]) 
			return nResult-1;
	}
	return nResult-1;
}

/* �����ڲ� : single value / linear */
/* output:  -1: fAbsz < x[0]     */
/* output: n-1: fAbsz > x[n-1]   */
/* output:  -2: |x[nId+1]-x[nId]| < eps */
/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int GetLinearInterpolation(const	float fAbscissaValue, const int nPos, const	float *fAbscissaArray, const float *fOrdinateArray, float *pfOrdinateValue)/* ��value value */ 
{
	int nId = GetInterpolatedIntervalId(fAbscissaValue, nPos, fAbscissaArray);
	float r, x1, x2;

	if(nId == -1){	/* fAbsz. < x[0]   */
		*pfOrdinateValue = fOrdinateArray[0];
		return(-1);
	}
	if(nId == (nPos-1))	{	/* fAbsz. > x[n-1] */
		*pfOrdinateValue = fOrdinateArray[nPos-1];
		return(nPos-1);
	}
	x1 = fAbscissaArray[nId];
	x2 = fAbscissaArray[nId+1];
	if(fabs(x2-x1) < EPS) {
		*pfOrdinateValue = fOrdinateArray[nId];
		return(-2);
	}
	r= (fAbscissaValue-x1)/(x2-x1);
	*pfOrdinateValue = (1.f-r) * fOrdinateArray[nId] + r * fOrdinateArray[nId+1];

	return nId;
}

/// @����˵�� �����ڲ� : array , const incr. / �����ڲ� of value and time (fTime, fPosition increasing) 
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int GetLinearInterpolation(const	float fInterpolationIncrement, const   float fPoint,  const    int  nPos, const	float *fTimeArray, const   float *fPosition, 
							const   float *pfValue, float   *fInterpolationTimeArray,  float    *fInterpolationArray)
{
	int nStep = -1;
	int	nStart, nEnd, nIndex;
	float fMean, fStandard, fMin, fMax;
	int nx  = AnalyseArray(nPos,pfValue,EPS,&fMean,&fStandard,&fMin, &fMax, &nStart, &nEnd); /* value */

	float fMeasured = fPosition[nEnd] - fPosition[nStart]; /* measured over length */
	GetInterpolatedIntervalId(fPoint,nPos,fPosition);
	if(fPosition[nEnd] < fPoint)
		return(-1);	/* didn'fTime passed measured length completely */
	
	float fAbscissaValue = fPosition[nStart]-fInterpolationIncrement;
	nx = (int)(1.01 + fMeasured/fInterpolationIncrement);

	float r, r1m, x1, x2;
	int nId = 0;
	while(++nStep < nx)	
	{	/* loop over measured length */
		fAbscissaValue += fInterpolationIncrement;
		nId = GetInterpolatedIntervalId(fAbscissaValue,nPos,fPosition);
		nIndex = nId;
		x1	= fPosition[nId]; /* nId < 0 impossible */
		if(nId >= nEnd){
			x2  = x1;
			nIndex  = nEnd; /* n-1 is possible */
		}
		else	
			x2	= fPosition[nId+1];

		if(fabs(x2-x1) < EPS) {
			fInterpolationTimeArray[nStep]   = fTimeArray[nIndex];
			fInterpolationArray[nStep] = pfValue[nIndex];
		}else{
			r= (fAbscissaValue-x1)/(x2-x1);
			r1m = 1.f - r;
			fInterpolationTimeArray[nStep]   = r1m * fTimeArray[nId]   + r * fTimeArray[nId+1];
			fInterpolationArray[nStep] = r1m * pfValue[nId] + r * pfValue[nId+1];
		}
	}					/* loop over measured length */
	return(nx);
}

/// @����˵�� �����ڲ� : array , const incr. / �����ڲ� of value and time (fTime, fPosition increasing)
/// ��ԭ���ĺ������������� ���ݵ㳤������ by xie
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int GetLinearInterpolationLengthLimit(const	float fInterpolationIncrement, const   float fPoint, const    int  nPos, const	float *fTime, const   float *fPosition, const   float *pfValue, 
										float *fInterpolationTimeArray,  float *fInterpolationArray)
{
	float fMean, fStandard, fMin, fMax,fAbsz;
	int	nStart, nEnd, i_status, index;
	int nx = AnalyseArray(nPos,pfValue,EPS,&fMean,&fStandard,&fMin, &fMax, &nStart, &nEnd); /* value */
	float fMeasured = fPosition[nEnd] - fPosition[nStart]; /* measured over length */

	if (fMeasured>40)
		fMeasured=40;

	i_status = GetInterpolatedIntervalId(fPoint,nPos,fPosition);
	if(fPosition[nEnd] < fPoint)
		return(-1);	/* didn'fTime passed measured length completely */	
	fAbsz = fPosition[nStart]-fInterpolationIncrement;
	nx = (int)(1.01 + fMeasured/fInterpolationIncrement);

	int nId;
	float r, r1m, x1, x2;
	int n = -1;
	while(++n < nx){	/* loop over measured length */
		fAbsz += fInterpolationIncrement;
		nId = GetInterpolatedIntervalId(fAbsz,nPos,fPosition);
		index = nId;
		x1	= fPosition[nId]; /* nId < 0 impossible */
		if(nId >= nEnd){
			x2  = x1;
			index  = nEnd; /* n-1 is possible */
		}
		else	
			x2	= fPosition[nId+1];
		if(fabs(x2-x1) < EPS){
			fInterpolationTimeArray[n]   = fTime[index];
			fInterpolationArray[n] = pfValue[index];
		}else {
			r= (fAbsz-x1)/(x2-x1);
			r1m = 1.f - r;
			fInterpolationTimeArray[n]   = r1m * fTime[nId]   + r * fTime[nId+1];
			fInterpolationArray[n] = r1m * pfValue[nId] + r * pfValue[nId+1];
		}
	}					/* loop over measured length */
	return(nx);
}



/// @����˵��
/// /* interpolation of time value                        */
/// /* typical application run time in POSTPROCESS           */
/// /* ����value:									   */
/// /*			-1	:	fPoint > fPosition[n-1]	 no measured point	   */
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int GetInterpolationTime(const	float fInterpolationIncrement,	const	float fLength, const   float fPoint, const    int  n,
							const	float *fTime, const   float *fPosition, float	*fInterpolationTimeArray) 
{
	int nx = (int) (fLength/fInterpolationIncrement) + 1;	
	int nStart = GetInterpolatedIntervalId(fPoint,n,fPosition);

	float fAbsz   = fPosition[nStart]-fInterpolationIncrement;//��ʼλ��
	float fEnd = fPosition[nStart]+fLength;//����ʱλ��

	if(fEnd > fPosition[n-1])	
		nx = (int)((fPosition[n-1] - fPosition[nStart])/fInterpolationIncrement) +1;

	int nId = 0;
	float r, r1m, x1, x2;
	int nPos = -1;
	while(++nPos < nx)	
	{	/* loop over measured length */
		fAbsz += fInterpolationIncrement;
		/* start interpolation within last ��� */
		nId = GetInterpolatedIntervalId(fAbsz,n-nStart,&fPosition[nStart]);
		nId += nStart;
		nStart = nId;	/* set new left intervall */
		x1	= fPosition[nId]; /* nId < 0 || nId > n-2 impossible */
		x2	= fPosition[nId+1];
		if(fabs(x2-x1) < EPS) 
			fInterpolationTimeArray[nPos]   = fTime[nId];
		else{
			r= (fAbsz-x1)/(x2-x1);
			r1m = 1.f - r;
			fInterpolationTimeArray[nPos]   = r1m * fTime[nId]   + r * fTime[nId+1];
		}
	}					
	return nx;
}

/// @����˵�� 
/// run time between points 1 and 2                            */
///	/* time and position value are available                     */
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
float	GetIntervalTimeAvailable(const   float fPointOne, const   float fPointTwo,  const    int  n,	const	float *fTime,		const   float *fPosition)
{
	if(fPointTwo < fPointOne || fPosition[0] > fPointOne || fPosition[n-1] < fPointTwo)		
		return 0.0;

	float fTime1, fTime2;
	GetLinearInterpolation(fPointOne, n, fPosition, fTime, &fTime1);
	GetLinearInterpolation(fPointTwo, n, fPosition, fTime, &fTime2);

	return fTime1 - fTime2;
}

/// @����˵�� ������Ҫ˵��-���Ժ��� 
/* run time between points 1 and 2                            */
/* position and �ٶ�value are available                    */
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
float	GetIntervalTimeSpeed(const   float fPointOne,const   float fPointTwo, const    int  n,	const	float *fPosition,	const   float *fSpeed)  
{
	float	fActualTime;		/* ʵ�� time value */
	float	fActualPosition;		/* ʵ�� position value */
	float	fActualSpeed;		/* ʵ�� �ٶ� value */
	float	fMean;		/* ƽ��value �ٶ� */
	float	*fTrackTime;		/* time value tracking profile */
	float	*fTrackingPosition;		/* position value tracking profile */
	float	*fTrackingSpeed;		/* �ٶ�value tracking profile */
	const	float	fSpeedeps = 0.0001f;
	const	float	fLenProfile = fPosition[n-1] - fPosition[0];
	const	int		nCompl		= n + 2;
	const	int		nComplM1	= n + 1;

	int		nStep = 0;
	/* 1. Plausibility of data */
	if(fPointTwo < fPointOne || n < 2 || fSpeed[nStep] < fSpeedeps)
		return 0.0;

	int	iError = 0;
	while(++nStep < n)	{
		if(fPosition[nStep] < fPosition[nStep-1])		iError++;
		if(fSpeed[nStep] < fSpeedeps)	iError++;
	}
	if(iError > 0)
		return 0.0;

	/* 2. fill complete tracking profile */
	if((fTrackTime  = (float *) calloc(nCompl, sizeof( float ))) == NULL)
		return 0.0;
	if((fTrackingPosition  = (float *) calloc(nCompl, sizeof( float ))) == NULL)
		return 0.0;
	if((fTrackingSpeed  = (float *) calloc(nCompl, sizeof( float ))) == NULL)  // �˴�Ӧ����û����ȷ�滻 [8/12/2013 лǫ]
		return 0.0;

	/* determine ��Сvalue of complete profile */
	float fZero = fPosition[0] - fLenProfile;
	if(fPointOne < fZero) fZero = fPointOne;
	fTrackTime[0]	= 0.0f;
	fTrackingPosition[0]	= fZero;
	fTrackingSpeed[0] = fSpeed[0];
	/* fill in profile value */
	nStep = 0;
	int izM1 = -1;
	float txPrev = fTrackTime[0];
	float vxPrev = fSpeed[0];
	float pxPrev = fTrackingPosition[0];
	while(++nStep < nComplM1)	{
		izM1++;
		/* determine ʵ�� step value */
		fActualSpeed		= fSpeed[izM1];
		fActualPosition		= fPosition[izM1];
		fMean		= 0.5f * (vxPrev + fActualSpeed);
		fActualTime		= txPrev + (fActualPosition - pxPrev) / fMean;
		/* fill in to complete profile */
		fTrackTime[nStep]		= fActualTime;
		fTrackingPosition[nStep]		= fActualPosition;
		fTrackingSpeed[nStep]	= fActualSpeed;
		/* copy step value for next step */
		txPrev		= fActualTime;
		pxPrev		= fActualPosition;
		vxPrev		= fActualSpeed;
	}
	/* determine ��� value of complete profile */
	float fMax = fActualPosition + fLenProfile;
	if(fPointTwo > fMax) fMax = fPointTwo;
	fTrackTime[nComplM1]	= fActualTime + (fMax - fActualPosition) / fActualSpeed;
	fTrackingPosition[nComplM1]	= fMax;
	fTrackingSpeed[nComplM1]	= fActualSpeed;
	/* 3. calculate pass time of synchronization points */

	float	t1 = 0.0f;
	float	t2 = 0.0f;
	int iret1 = GetLinearInterpolation(fPointOne, nCompl, fTrackingPosition, fTrackTime, &t1);
	if(	(iret1 == -2) || (iret1 == -1) || (iret1 > nComplM1)){
		free(fTrackTime); free(fTrackingPosition);	free(fTrackingSpeed);
		return 0.0;
	}
	int iret2 = GetLinearInterpolation(fPointTwo, nCompl, fTrackingPosition, fTrackTime, &t2);
	if(	(iret2 == -2) || (iret2 == -1) || (iret2 > nComplM1)){
		free(fTrackTime); free(fTrackingPosition); free(fTrackingSpeed);
		return 0.0;
	}
	free(fTrackTime); free(fTrackingPosition);	free(fTrackingSpeed);
	return t2 - t1;
}

/// @����˵�� ������Ҫ˵��-���Ժ��� 
/*  modify function to limit value                  */
/*  - first ������ value is fLimitLow              */
/*  - highest ������ value is fLimitHigh           */
/*  - don'fTime change value if limit value outside    */
/*    ������ value                                */
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
int	ModifyLimits(const	int	n,	const float fLimitLow, const float fLimitHigh, float *x, float	*y)
{
	int	nVal	= n;
	const	int	nM1     = n - 1;
	const	int	iStart	= GetInterpolatedIntervalId(fLimitLow,  n, x);
	const	int	iEnde	= GetInterpolatedIntervalId(fLimitHigh, n, x);
	if((iStart == -1) && (iEnde == nM1))	
		return(nVal);
	if( x[0] > x[nM1])	
		return(nVal);

	int	nStep = 0;
	if(iStart > -1){
		x[0] = fLimitLow;
		GetLinearInterpolation(fLimitLow, n, x, y, &y[0]);
		nStep = 1;
	}
	int iCtr = iStart;
	while(++iCtr < (iEnde + 1)){
		x[nStep]   = x[iCtr];
		y[nStep++] = y[iCtr];
	}

	if(iEnde < nM1){
		x[nStep] = fLimitHigh;
		GetLinearInterpolation(fLimitHigh, n, x, y, &y[nStep]);
		iCtr = nStep;
		nStep++;
		while(++iCtr < n){
			x[iCtr] = 0.0f;
			y[iCtr] = 0.0f;
		}
	}
	return(nStep);
}


/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013  
float   Interpolation(const   int      n, const   float   fAbsz, const   float   *x, const   float   *y)
{
	if(n < 1)  
		return(0.0f);
	if(n == 1) 
		return(y[0]);

	float   fOrd;
	int     nStep  = 0;
	float   r;
	if(fAbsz < x[0])     {   /* linear extension */
		fOrd = y[0];
		r   = x[1] - x[0];
		if(fabs(r) < EPS)   return(y[0]);
		r = (fAbsz - x[0]) / r;
		fOrd     = (1.f- r) * y[0] + r * y[1];
		return(fOrd);
	}                   /* linear extension */
	if(fAbsz > x[n-1])   {   /* linear extension */
		fOrd = y[n-1];
		r   = x[n-1] - x[n-2];
		if(fabs(r) < EPS)   return(y[0]);
		r = (fAbsz - x[n-2]) / r;
		fOrd     = (1.f - r) * y[n-2] + r * y[n-1];
		return(fOrd);
	}                   /* linear extension */

	while(++nStep < n) {
		if(fAbsz < x[nStep])        break;
	}
	/* ensure use of last ��� (fAbsz == x[nStep = n -1] */
	if(nStep == n) nStep = n - 1;
	r       =       x[nStep] - x[nStep-1];
	if (fabs(r) < EPS)      {
		fOrd = .5f * (y[nStep] + y[nStep-1]);
		return(fOrd);
	}

	r = (fAbsz - x[nStep-1])/r;
	fOrd     = (1.f - r) * y[nStep-1] + r * y[nStep];

	return(fOrd);
}

/// @����˵�� �жϸ������ʹ�С��ȡ���д���
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
float MaxF(const	float	x1,const	float	x2)
{
	if(x1 < x2) return(x2);
	return(x1);
}

/// @����˵�� �жϸ������ʹ�С��ȡ����С��
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013
float MinF(const	float	x1,const	float	x2)
{
	if(x1 > x2) return(x2);
	return(x1);
}


/// @����˵�� ������Ҫ˵��-���Ժ��� 
/// @���� C1
/// @���� c2
/// ����value ����˵��
/// ���� Hanb
/// ���� 7/25/2013               ?????????????????????
int	AnalyseArrayFilter(const	int		n, const	float	*x, const	float	eps, float	*fMean,	float	*var, float	*fMin, float	*fMax, int		*nStart, int		*nEnd)
{
	*fMean = 0.0f;
	*var = 0.0f;
	*fMin = 0.0f;
	*fMax = 0.0f;
	*nStart = 0;
	*nEnd  = 0;


	float lastValue=0.0f;
	int	nResult = 0;	/* return value */
	int	nStep = -1;
	float	s1  = 0.0f;
	float	s2  = 0.0f;
	float	fValue = 0.0f;
	int nfront=0; 
	while(++nStep < n)	{				/* all elements */
		fValue = x[nStep];
		if(fabs(fValue) > eps ){	/* nonzero el. */
			if(nResult == 0){
				*fMin = fValue;
				*fMax = fValue;
			}
			nfront++;
			if(nfront>2) {   //����ǰ������ add by xie
				nResult++;
				*nEnd = nStep;
				s1  += fValue;
				s2  += fValue * fValue;
				if(fValue < *fMin) *fMin = fValue;
				if(fValue > *fMax) *fMax = fValue;
				lastValue=fValue;
			}
		}							/* nonzero el. */
		else{					/* check for last value */	
			if(nResult > 10)	
				break;
			else			
				*nStart = nStep+1;
		}							/* check for last value */	
	}								/* all elements */
	if(nResult > 0) {
		if (nResult>3)
			*fMean  =  (s1-lastValue)/(nResult-1);
		else
			*fMean  =  s1/nResult;
	}

	if(nResult > 1) 
		*var =  (float)(sqrt((s2 - s1*s1/nResult)/(nResult-1)));
	return(nResult);
}