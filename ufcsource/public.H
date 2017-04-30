//本函数包含路径函数 及bit函数
#pragma once
#include "../BaseCoolStruct.h"
#include "Mathematical.h"
#include <cstring>

int FillPDIStruct(INPUTDATAPDI *pdi,RALINDATA *ralInPut,RALCOEFFDATA *lay,PRERESULT *PreResult,	const int ioutp);

int	GetPlateTaskFileName(const  char *plate_id,	const  int  taskID,	char *fn);

int InitControlValues(MODELCONTROL *x);

int InitPostControlValues(const char *sFName, MODELCONTROL *x);

int InitControlValues(const MODELCONTROL *x, MODELCONTROL *y);

int	PreparationOutput(RALINDATA *ralInPut, RALOUTDATA *pre_out, FILE *st_err);

void CalculateLinearInterpolation(float *a, float *x, int anz, int maxi);

//获得线性插value
float   GetLinearInterpolationFromAbscissa(const   float   *x,const   float   *y,const   int n,	const   float   absz);

int	GetFirstNonzeroElement(const	int		n,	const	float	fEps,	const	float	*fVal);
int	GetLastNonzeroElement(const	int		n,	const	float	fEps,	const	float	*fVal);
int swapInt(int *ix1, int *ix2);
int	GetMaxValuePos(	const	int		n,	const	float	fEps,	const	float	*fVal);
int	GetMinValuePos(	const	int		n,	const	float	fEps,	const	float	*fVal);
long int	LimitFileSize(const	long int	lSizeMax, const	char *sFileName, const	char		*sFilePref);
void ComputationalParameters(COMPUTATIONAL *compx, const INPUTDATAPDI *pdiInPut, const PHYSICAL *ax);
int     CalculationElementSizes(COMPUTATIONAL *compx, const   int   iv, const   float   l);
int     netx1(const int isSymetric,   /* 0: non symm, 1: symm */
	const int       iele, /* number of elements   */
	const   float   l,              /* length of area             */
	const float p,          /* exponent             */
	float *y);   /* element lengthes     */

void	CombinFilePath(const	char	*f_name, const int taskID, char	*pf_name);

int CheckBit(const int nPos,int nVal);
int SetBit(const int nPos, int nVal);
char* BitToString(const int nVal, int nSize, char	*sStr);
char*	BitToStatusComment(const	int	nVal, int nSize);

