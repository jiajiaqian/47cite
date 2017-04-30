/*! 
* Copyright (c) 2013, 东北大学轧制技术及连轧自动化国家重点实验室(RAL)
* All rights reserved
* Temperature场控制类
* FILE TemperatureField.cpp
* 说明 概述  
*  
* 详细说明
*  
* 版本 1.0 作者 Hanbing 日期 5/6/2013
* 
*/

#include "stdafx.h"
#include "TemperatureField.h"

CTemperatureField::CTemperatureField()
{
}

int CTemperatureField::CalculationTemperatureDistribution(	FILE 			*st_err,
	FILE 		*erg_f ,
	const		INPUTDATAPDI *pdi,
	COMPUTATIONAL *compx,
	const		PHYSICAL *ax,
	const		MATERIAL *matal)
{
	/* 	calculation of Temperature distribution        			*/
	FILE	*erg_graph = NULL;
	int   iz;
	int 	ixx = 0;    /* error parameter for memory reservation		*/
	int 	imatrix	= compx->matrixElementNumber;	/*	no. of matrix koeff.  	*/
	int		icolumn = compx->thickNodeNumber;		/*	no. of thickNodeNumber	      	*/
	int		iCenter = icolumn / 2;
	char	fx_name[255];
	char	*name_ex[2] = {"w", "a"};
	int	i_nx = 1;
	float	dt_zero = compx->detaTime;		/*	time increment	      	*/
	float	detaTime;			/*	actuel time step			*/
	float	dt_min = 0.0001f; /* lowest allowed time step      */
	float	timex; 		/*	time						*/
	float	stiff[100];		/*	modified system stiffness matrix	*/
	float	rr[100];		/*	upper matrix				*/
	float	yy[50];		/*	column for cholesky			*/
	float	force[50];		/*	system force column			*/
	float	temp_k[50];	/*	Temperature column at 实际 time step	*/
	float	temp_km1[50];	/*	Temperature column at previous step	*/
	//	float   temp_m[50];	/*	middle Temperature in predictor step	*/
	float   xx[15];		/*	value for output control			*/
	float   d_temp;		/*	output if 表面Temperature diiffers	*/
	/*	more than d_temp from previous output	*/
	float	temp_kph[50];	/*	Temperature (t(k-1) + t(k))/2		*/
	float	temp_pred[50];	/*	predictor Temperature			*/
	float	xgrad = 0.0f;
	int	izaehler = 0;   /*	index for time integration		*/
	int	i_it;		/*	number of iteration			*/
	int	anz_it = 10;	/*	maximum number of iteration cycles	*/
	int anz_dt = 1000;  /* 最大 allowed No. time increments */
	float	residual;	/*	estimated error				*/
	float	epsilon = .1f;	/*	accepted error in calculation = .1 K	*/
	float   calculationStopTime = compx->calculationStopTime;
	int	istop = 0;
	int	i_output = compx->femCalculationOutput;
	static	float	run_time;

	if(ax->coolPhaseNumber == 0)	
	{
		run_time = 0.0f;		/* set run time to zero 	*/
		i_nx = 0;				/* FILE extension = "w"		*/
	}

	xx[10] = run_time;
	compx->calculationTotalTime = run_time;	/* store 实际 time */

	strcpy_s(fx_name, pdi->plateID);
	strcat(fx_name, "_x.dat");

	if(i_output)  
	{
		if(i_nx == 0) 
			remove(fx_name);
		if((erg_graph = fopen(fx_name,name_ex[i_nx])) == NULL){
			fprintf(st_err,"\n\t ** not able to open: %s ***", fx_name);
			return -1;
			//exit(1);
		}
	}
	if(i_output > 1) 
		OutputComputational(erg_f, compx);

	iz = -1; 
	while(++iz < 15) 
		xx[iz] = 0.0f;
	/*	time integration algorithem		*/

	/*	start 参数 for Temperature distribution	*/

	d_temp = InitTemperatureDistribution(compx, matal,temp_k, temp_km1);
	if(pdi->taskID == PRE_CALC_SIM) 
		d_temp = 5.f;

	/* 纠正 initial distribution if temp < Ar3 */

	if(ax->coolPhaseNumber == 0)
	{
		/* calculate start distr. x */
		SystemMatrices(stiff, force, compx, temp_k, temp_k, 0.0f, ax, matal);
	}
	compx->detaTime = dt_zero;
	xx[1] = CalculationMeanTemperature( compx, compx->startTempDistribution);

	if(ixx > 0) 
	{
		fprintf(st_err,"\n\t *** not enough memory for allocation in: solver *** ");
		if(erg_graph != NULL) 
			fclose(erg_graph);
		return(1);
	}

	xx[2]	= xx[1];
	xx[3]	= temp_k[0];
	xx[4]	= xx[3];
	xx[5]	= xx[3];
	xx[11]	= temp_k[iCenter];
	xx[12] = xx[11];

	OutputFunc(i_output,erg_f, erg_graph, xx, compx, temp_k,0);
	timex = 0.0f;
	if(compx->testParameter == 2)
		dt_zero *= .1f;
	detaTime    = dt_zero;
	iz	= 1;

	while(iz == 1) 
	{	/*     loop over time increments	*/
		izaehler++;
		if(compx->testParameter == 1)
			detaTime = EstimationTemperatureDifference(detaTime, dt_zero, temp_km1, temp_k, izaehler, icolumn - 1);
		if( calculationStopTime > .001)  {
			if((timex + detaTime) > calculationStopTime) {
				detaTime = calculationStopTime - timex;
				istop = 1;
			}
		}
		timex += detaTime;
		xx[0]  = timex;
		xx[10] +=	detaTime;
		xx[6]  = xx[2];
		/*	initiate for iteration cycle				*/

		SetColumn(temp_kph, temp_km1, icolumn);
		SetColumn(temp_pred, temp_km1,icolumn);
		SetColumn(compx->TimeKpredAusteniteDistribution, compx->TimeKAusteniteDistribution,icolumn); 
		SetColumn(compx->TimeKpredRhoCp, compx->TimeKRhoCp,icolumn);

		i_it= -1;
		while(++i_it < anz_it)	{	/*	Iteration		*/
			/* system matrices					*/
			SystemMatrices(stiff, force, compx, temp_km1, temp_kph, detaTime, ax, matal);
			/*	solution of the linear algebraic system		*/
			SolveAlgebraicMatrix(stiff, rr, yy, force, temp_k, icolumn);
			residual = Norm2FindMax(temp_pred,  temp_k, icolumn);
			if(residual < epsilon)	
				break;
			ColCrNi1(temp_kph, temp_km1, temp_k, icolumn);
			SetColumn(temp_pred, temp_k, icolumn);
		} 	/*	Iteration		*/
		compx->calculationTotalTime += detaTime;
		compx->detaTime = detaTime;
		SetColumn(compx->TimeKRhoCp, compx->TimeKpredRhoCp, icolumn);
		SetColumn(compx->TimeKm1AusteniteDistribution, compx->TimeKAusteniteDistribution, icolumn);
		SetColumn(compx->TimeKAusteniteDistribution, compx->TimeKpredAusteniteDistribution, icolumn);
		/*	caloric mean tenperature			*/
		xx[7]	 = xx[2];
		xx[2]  = CalculationMeanTemperature( compx, temp_k);
		if(detaTime < dt_min)	
			xx[7] = 0.0f;
		else			
			xx[7]  = (xx[7] - xx[2])/detaTime;		/* local cooling rate	*/

		/*	output of calculated data			*/
		xx[4]	= temp_k[0];
		xx[12]	= temp_k[iCenter];
		/* output required, if surface temp.diff. exceeds limit d_temp */
		/* or Temperature in center line exeeds d_temp */
		if((	fabs(xx[4] - xx[3]) > d_temp)  || fabs(xx[12] - xx[11]) > d_temp){  
			OutputFunc(i_output,erg_f, erg_graph, xx, compx, temp_k, i_it);
		}
		iz	=  IntegrationTimeEnd(timex, xx[2], compx);
		if((istop == 1)||(iz!=1)) 
			break;
		if(izaehler > anz_dt) 
			break;
		SetColumn(temp_km1, temp_k, icolumn);

	}	/*	time steps		*/

	/* 	纠正ion if mean Temperature desired	*/
	if(compx->calculationStopTemp > .0001){
		xgrad = (compx->calculationStopTemp - xx[6])/(xx[2] - xx[6]);
		iz = -1;
		while(++iz < icolumn)
			temp_k[iz]  = temp_km1[iz] * (1.f - xgrad) + temp_k[iz] * xgrad;
		xx[0] = timex - detaTime + xgrad * detaTime;
		compx->calculationStopTime = xx[0];
		xx[10] +=xgrad * detaTime -detaTime;
	}

	compx->localCoolRate = xx[7]; /* local 冷却速度*/
	run_time = xx[10];
	SetColumn(compx->startTempDistribution, temp_k, icolumn);
	SetColumn(compx->startAusteniteDistribution, compx->TimeKAusteniteDistribution, icolumn);
	xx[2]  = CalculationMeanTemperature( compx, temp_k);
	compx->calculationStopTemp = xx[2];
	compx->calculationTotalTime       = run_time;

	OutputFunc(i_output, erg_f, erg_graph, xx, compx, temp_k, i_it);
	if(i_output) 
		fprintf(erg_f,"\n\t --- stop: %d steps %g seconds ---",
		izaehler, xx[0]);

	/* energy balance */
	if(i_output) 
		EnergyReport( erg_f, pdi, compx, xx);

	if(i_output)  
		fclose(erg_graph);
	return(ixx);
}




float  CTemperatureField::InitTemperatureDistribution( 	COMPUTATIONAL *compx,
	const	MATERIAL *matal,
	float  *temp_k,
	float  *temp_km1)
{
	float d_temp = 10.f;    /* 默认value for output max. TEMPERATURE_CURVE_NUMBER value are possible */
	int i_it, iz;
	float xgrad, timex, xx;
	int i_middle = compx->thickNodeNumber / 2;   /* index of node near centerline of the 板材 */
	int tempCurveNumber = TEMPERATURE_CURVE_NUMBER;                     /* number of points, which will be represent cooling curves */

	for(iz = 0; iz < compx->thickNodeNumber; iz++) {
		timex = compx->startTempDistribution[iz];
		i_it = (int) timex;
		temp_k[iz] 	= timex;
		temp_km1[iz] 	= timex;
		compx->TimeKAusteniteDistribution[iz]     = compx->startAusteniteDistribution[iz];
		compx->TimeKm1AusteniteDistribution[iz]   = compx->startAusteniteDistribution[iz];
		compx->TimeKpredAusteniteDistribution[iz]   = compx->startAusteniteDistribution[iz];
		xx = compx->TimeKAusteniteDistribution[iz] * .01f;
		xgrad  = (1.f- xx)* matal->para.rhoCpa[i_it];
		xgrad += xx*matal->para.rhoCpg[i_it];
		compx->TimeKRhoCp[iz] = xgrad;
		compx->TimeKm1RhoCp[iz] = xgrad;
		compx->TimeKpredRhoCp[iz] = xgrad;
		compx->TimeKEnthalpy[iz] = (1.f-xx) * matal->para.ferritEnthalpy[i_it] + xx *matal->para.austenitEnthalpy[i_it];
	}

	if(tempCurveNumber < 4) 
		tempCurveNumber = 4;
	if(compx->calculationStopTemp > 0.0f) 
		d_temp = (float)fabs(compx->startTempDistribution[i_middle] - compx->calculationStopTemp)/(TEMPERATURE_CURVE_NUMBER-3);
	compx->latentHeat = 0.0f;
	compx->meanCalculationValue = 0;
	compx->austenitePercentMean = 0.0f;
	compx->austenitePercentMeanNextStep = 0.0f;
	iz = -1; 
	while(++iz < 10) 
		compx->outputParameter[iz] = 0.0f;
	return(d_temp);

}

float CTemperatureField::EstimationTemperatureDifference( float detaTime, float dt_zero, float *temp_km1,
	float *temp_k, int izaehler, int icc)
{
	/* 	estimation of expected Temperature difference	*/
	float 	xmax;
	float	dt_eps = 0.001f; /* 最小allowed detaTime */

	float	delta_temp = 40.f;
	float	xdif_top;
	float	xdif_bot;
	float	zeta, zetaw = .0125f, kf = 10.f;
	float	dt_pred;	/*   predicted time increment		*/
	float   dt_pred_grad;	/*   estimated from 最大 gradient	*/
	float	dt_max = 20.f; /* 最大 allowed time increment */

	if(detaTime < dt_eps) 
		detaTime = dt_eps;
	dt_pred	  = detaTime;
	zeta 	 = zetaw * (float) izaehler;
	if (zeta < 1.f ) 
		dt_pred = dt_zero * ( 1.f + kf * (float)sqrt(zeta));
	xdif_bot = (float)fabs((temp_km1[0]   - temp_k[0])/detaTime);
	xdif_top = (float)fabs((temp_km1[icc] - temp_k[icc])/detaTime);

	xmax = xdif_top;	/*  xmax:  最大 Temperature gradient	*/
	if(xmax < xdif_bot) 
		xmax = xdif_bot;

	if(dt_pred > dt_max)	
		return(dt_max);
	if(xmax < dt_eps)		
		return(dt_pred);

	dt_pred_grad  = delta_temp / xmax;
	if(dt_pred_grad < dt_pred) 
		return (dt_pred_grad);
	return(dt_pred);
}

void CTemperatureField::OutputComputational( FILE *ffx, COMPUTATIONAL *xy)
{
	int 	iz = 0;
	fprintf(ffx,"\n\t Temperature distiribution at time = 0: \n");
	fprintf(ffx,"\n\t (node, temp.): ");
	for( iz = 0; iz < xy->thickNodeNumber; iz = iz + 5)
		fprintf(ffx," ( %d , %g) ", iz, xy->startTempDistribution[iz]);
	fprintf(ffx,"\n\t length of the elements: \n\t ( node, leng.): ");
	for( iz = 0; iz < xy->thickNodeNumber; iz = iz + 5)
		fprintf(ffx," ( %d , %g) ", iz, xy->elementLength[iz]);
	fprintf(ffx,"\n\t temp. identifier (0: given distribution");
	fprintf(ffx," 1: surface value) computeID = %d", xy->computeID);
	fprintf(ffx,"\n\t integration time 	     = %g s", xy->calculationStopTime);
	fprintf(ffx,"\n\t stop temp. for integration = %g grad C", xy->calculationStopTemp);
	fprintf(ffx,"\n\t time increment       detaTime    = %g s", xy->detaTime);
	fprintf(ffx,"\n\t integration algorithem     = %d points", xy->integra);
	fprintf(ffx,"\n\t identifier (0: nonsym.)    = %d", xy->isSymetric);
	fprintf(ffx,"\n\t matrix elements	     = %d \n", xy->matrixElementNumber);
}

int CTemperatureField::IntegrationTimeEnd( float timex, float time_mean, COMPUTATIONAL *compx)
{
	int	ient = 0;
	if(compx->calculationStopTime > .001 && timex > compx->calculationStopTime) return(ient);
	/* integration time */
	if(compx->calculationStopTemp >  .001 && time_mean < compx->calculationStopTemp) return(ient);
	/* stop Temperature */
	ient = 1;
	return(ient);
}

void   	CTemperatureField::OutputFunc(       int i_output,
	FILE *ffx,
	FILE *ffy,
	float *x,
	COMPUTATIONAL *sy,
	float *y,
	int i_it)
{
	float	cr = 0.0f;		/* actuel cooling rate	*/
	float	eps_time = .0001f;
	int	n1 = sy->thickNodeNumber - 1;
	int	n2 = (int) ( n1 / 2);
	float	sum = 0.0f;
	int     iz = -1;
	float rcpx = sy->rhoCpMeanValue * .1e-5f;
	float	hx_mean = 0.0f;	/* 平均value over thickness */
	static float	temp_alt;		/* caloric mean temp. of last step */

	if ((x[0] < eps_time) && (i_output > 0)) {
		sy->latentHeat = 0.0f;
		sy->meanCalculationValue = 1;
		fprintf(ffx,"\n\t --------------------------------------------------");
		fprintf(ffx,"--------------------------------------------------------");
		fprintf(ffx,"\n\t | time    |   T[0] |  T[%d] |   T[%d] |", n1, n2);
		fprintf(ffx,"  cr   |  crl  |  T mean|  T cal | it|rho_cp|X[0] |X[%d] |  Xm |",n2);
		fprintf(ffx," dL/detaTime|");
		fprintf(ffx,"\n\t -------------------------------------------------------");
		fprintf(ffx,"---------------------------------------------------");
	}
	if (x[0] > eps_time) {
		cr  =  (x[1] - x[2])/x[0];
		x[8] +=	 (x[2] - temp_alt)* sy->rhoCpMeanValue;;
	}
	sum = MiddleArit( sy,y);
	if(sy->outputParameter[0] > 1.e-5f) sy->latentHeat /= (sy->outputParameter[0]*sy->meanCalculationValue);
	else                      sy->latentHeat  = 0.0f;
	sy->latentHeat *= LATENT_HEAT_MULTIPLIER;
	sy->austenitePercentMean /= sy->meanCalculationValue;
	sy->austenitePercentMeanNextStep = sy->austenitePercentMean;

	if(i_output > 0) {    /* output detailed */
		fprintf(ffx,"\n\t | %7.3f | %6.1f | %6.1f | %6.1f |",x[0],y[0],y[n1],y[n2]);
		fprintf(ffx," %5.2f | %5.2f | %6.1f | %6.1f |%3d| %5.3f|%5.1f|%5.1f|%5.1f|%5.1f |",
			cr, x[7], sum, x[2],i_it, rcpx,sy->TimeKAusteniteDistribution[0], sy->TimeKAusteniteDistribution[n2], sy->austenitePercentMean,sy->latentHeat);

		/*	output on ASCII FILE if time > eps_time or cumm. time < eps_time */
		if(sy->femCalculationOutput)  {
			if(x[10] < eps_time)	{
				/*         ------------+------------+------------+------------+  */
				fprintf(ffy,"\n         time   total_time         T[0]       T[%3d]"
					"       T[%3d]", n1, n2);
				fprintf(ffy,  "           cr          crl       T_mean        T_cal"
					"       rho_cp         X[0]       X[%3d]           Xm",n2);
				fprintf(ffy,  "         H[0]       H[%3d]?     H_mean        dL/detaTime", n2);
			}

			if((x[0] > eps_time) || (x[10] <= eps_time)) {
				iz = -1; /* calculate 平均value of enthalpy over thickness */
				while(++iz < NODES_NUMBER) 
					hx_mean += sy->TimeKEnthalpy[iz];
				hx_mean /= NODES_NUMBER;


				fprintf(ffy,"\n %12.4f %12.4f %12.4f %12.4f %12.4f %12.4f %12.4f %12.4f %12.4f %12.4f",
					x[0], x[10], y[0], y[n1], y[n2], cr, x[7], sum, x[2],
					sy->rhoCpMeanValue);
				fprintf(ffy," %12.4f %12.4f %12.4f",  sy->TimeKAusteniteDistribution[0], sy->TimeKAusteniteDistribution[n2], sy->austenitePercentMean);
				fprintf(ffy," %12.4e %12.4e %12.4e %12.4f", 
					sy->TimeKEnthalpy[0], sy->TimeKEnthalpy[n2], hx_mean, sy->latentHeat);
			}
		}
	}						/* output detailed */
	x[3]	= x[4];			/* last output Temperature	*/
	x[11]	= x[12];

	temp_alt = x[2];
	sy->meanCalculationValue = 0;
	sy->latentHeat = 0.0f;
	sy->austenitePercentMean = 0.0f;
	sy->outputParameter[0] = 0.0f; /* reset time from last output */

	/* add output value for use in other programs refer: io_struc.h */

	if(x[0] < eps_time) {
		sy->computedNumber = -1;   	/* initialize output value */
		iz = -1;
		while(++iz < TEMPERATURE_CURVE_NUMBER) {
			sy->zx[iz]		= 0.0001f;
			sy->tx[iz]		= 0.0f;
			sy->cx[iz]		= 0.0f;
			sy->tx_cen[iz]	= 0.0f;
			sy->tx_top[iz]	= 0.0f;
			sy->tx_bot[iz]	= 0.0f;
		}  		/* initialize output value */
	}
	/* increment only, if space enough in files, otherwice overwrite last value */
	if(sy->computedNumber < (TEMPERATURE_CURVE_NUMBER-1)) 
		sy->computedNumber += 1;
	iz = sy->computedNumber;
	if(iz == 1) 
		sy->cx[0] = cr;   /* set value for initial time step */

	sy->zx[iz] = x[0];
	sy->tx[iz] = x[2];
	sy->cx[iz] = cr;

	sy->tx_cen[iz] = y[n2];
	sy->tx_top[iz] = y[n1];
	sy->tx_bot[iz] = y[0];

}


int CTemperatureField::EnergyReport( FILE    *erg_f,
	const  INPUTDATAPDI *pdi,
	const  COMPUTATIONAL *compx,
	float   *xx)

{  /*	i_output  of energy report	*/
	float   vol;
	int	iaus = NODES_NUMBER/2;

	fprintf(erg_f,"\n\t compx->startTempDistribution (0 and %d): %g %g",
		iaus, compx->startTempDistribution[0], compx->startTempDistribution[iaus]);
	fprintf(erg_f,"\n\t ------------------------------------------------------");
	vol = pdi->thick * pdi->width * pdi->length;
	fprintf(erg_f,"\n\t vol.: %6.2f m^3  mass: %6.2f t", vol, 7.9 *vol);
	xx[8] *= .001f;	/* cp in J/(kg*K)	*/
	fprintf(erg_f,"\n\n\t spec. energy diff.: %8g kJ/m^3 (%8g kcal/m^3)",
		xx[8], xx[8]*2.388e-1, vol);
	xx[9]= xx[8]*vol;
	vol = pdi->thick * pdi->width * pdi->length;
	fprintf(erg_f,"\n\t       energy diff.: %8g kJ     (%8g kcal)",
		xx[9], xx[9]*2.388e-1);
	if(xx[0] > 0.1)
		fprintf(erg_f,"\n\t mean heat output .: %8g MW     (%8g MW/m^2)",
		xx[9]*.001/xx[0],xx[9]*.001/(xx[0]*pdi->width*pdi->length));
	return(1);
}

void CTemperatureField::SetColumn(	float	*x_init,	/*	initiate	*/
	const	float	*x,		/*	with value	*/
	const	int	n)		/*	dimension	*/
{
	int	iz = -1;
	while(++iz < n)	
		x_init[iz] = x[iz];
}


void	CTemperatureField::ColCrNi1(	float	*x_kph,	/* halve time step	*/
	const	float	*TimeKm1AusteniteDistribution,	/* forgoing  k-1	*/
	const	float	*TimeKAusteniteDistribution,	/* predicted step k	*/
	const	int	n)	/* dimension		*/
{
	int 	iz = -1;
	while(++iz < n)	
		x_kph[iz] = .5f * (TimeKm1AusteniteDistribution[iz] + TimeKAusteniteDistribution[iz]);
}

float	CTemperatureField::Norm2FindMax(	const	float	*x,	/*	first column	*/
	const	float	*y,	/*	second column	*/
	const	int	n)	/*	dimension	*/
{	/*	calculate 最大 偏差	*/
	int	iz = -1;
	float	max = 0.0f;
	float	fdiff;

	while(++iz < n)	{
		fdiff 	= x[iz] - y[iz];
		if(fabs(fdiff) > max)	
			max = fdiff;
	}
	return(max);
}

float   CTemperatureField::CopyValue(
	const	int		 sw_mat,	/* material type */
	const  MATERIAL *matal,
	COMPUTATIONAL *sy,
	const   int     i_node,
	const   float   x_old,
	const   float   t_kph,  /* Temp. k + 1/2 in C */
	const   float   t_k)     /* Temp. k  in C */
{
	float   cp; /* value of cp */
	float   TimeKAusteniteDistribution = sy->TimeKAusteniteDistribution[i_node];
	float   xx;
	float   ord;
	int   it = (int)t_kph;
	float y_old;
	float   epst = .1e-5f; /* epsilon value for time step */
	float   d_temp = 100.f;   /* additional error window for search */

	if(sw_mat != 1)	{	/* one phase system */
		sy->TimeKEnthalpy[i_node] = 0.0;
		sy->relasedEnergy[i_node]  = 0.0;
		return(matal->para.rhoCpg[it]);
	}				/* one phase system */
	ord   = matal->para.ar3 - t_kph;

	if(ord < matal->para.austenitTransPercent.y_min - d_temp) { 
		xx = 100.f;
		if(xx > x_old) xx = x_old;
		sy->TimeKpredAusteniteDistribution[i_node] = xx;
		return(matal->para.rhoCpg[it]);
	}

	y_old = matal->para.ar3 - t_k;
	xx  = SearchXValue(&matal->para.austenitTransPercent, x_old, y_old, ord, sy->calculationTotalTime);

	xx *= .01f;
	cp  = (1.f-xx)*matal->para.rhoCpa[it] + xx *matal->para.rhoCpg[it];
	/* calculate xx for predetermined Temperature value in step k+1 */
	ord += t_k - t_kph;
	xx  = SearchXValue(&matal->para.austenitTransPercent, x_old, y_old, ord, sy->calculationTotalTime);
	sy->TimeKpredAusteniteDistribution[i_node] = xx; /* value in predictor time step k+1 */
	if(sy->calculationTotalTime < epst) {
		sy->TimeKm1AusteniteDistribution[i_node] = xx;
		sy->TimeKAusteniteDistribution[i_node] = xx;
	}

	xx *= .01f;      /* percent / 100 */
	sy->TimeKEnthalpy[i_node] = (1.f-xx)*matal->para.ferritEnthalpy[it] + xx *matal->para.austenitEnthalpy[it];
	sy->relasedEnergy[i_node]  =  matal->para.austenitEnthalpy[it] - matal->para.ferritEnthalpy[it];
	return(cp);
}

/* faster and replaces spline interpolant in each time step */

float   CTemperatureField::SearchXValue(    
	const   CURVE *cv,            /* curve */
	const   float   x_old,  /* old 横坐标value */
	const float     y_old,  /* old 纵坐标value */
	const float     y_new,  /* new 纵坐标value */
	const float		t_zykl) /* 实际 time in 冷却区 */
{
	float   x_new = x_old;
	int		iz = 100;
	/* int     iz  = (int)(x_old+1.f); *//* start search at highest index */
	float r;
	float	dy_iz = (float) fabs(100.f - x_old);
	int		i_interval;
	float	ya, yb;          /* limits of 间隔 */
	float   eps = .01f;      /* 最小posible 偏差 for austenite decomp. */
	float	epz = 100.f - eps;

	if(y_new < (cv->y_min-eps))  
		return(x_old);

	if((dy_iz > epz)&&(y_new > (cv->y_max+eps)))  return(0.0f);

	if(iz > 100) iz = 100;  /* check against 最大value */
	yb = cv->y[iz];
	while(--iz > -1)        {       /* search down to 低 value */
		ya = cv->y[iz];
		i_interval = CheckInterval(ya, yb, y_new);
		if(i_interval) break;
		yb = ya;
	}                                          /* search down to 低 value */
	if((iz < 1) && (i_interval == 0))     return(cv->x[0]);
	dy_iz = yb - ya;
	if(fabs(dy_iz) < eps) return(cv->x[iz+1]);
	r = (y_new - ya)/dy_iz;
	x_new = (1-r)*cv->x[iz] + r*cv->x[iz+1];

	if(x_new > 100.f) 
		return(100.f);
	if(x_new < 0.0f)  
		return(0.0f);
	if(x_new > x_old) 
		return(x_old); 
	return(x_new);
}

/* check if x is a value between xa and xb */
/* return 1: x is between xa and xb, 0: not */
int	CTemperatureField::CheckInterval(
	const	float	xa,  /* left boarder 间隔 */
	const	float	xb,  /* right border inteval  */
	const	float	x)   /* value */
{
	float	eps = 0.001f;
	if(( x > xa-eps) && ( x < xb + eps))	return(1);
	if(( x < xa+eps) && ( x > xb - eps))    return(1);
	return(0);
}

void    CTemperatureField::SystemMatrices(       float           *k_mod,  /* modified stiffness-matrix */
	float           *force,  /* modified force column     */
	COMPUTATIONAL *sy,
	const    float          *temp_k,        /* temp: step k               */
	const    float         *temp_kph,      /* temp: step k+1/2   */
	const    float          detaTime,
	const    PHYSICAL *ax,
	const    MATERIAL *matal)
{
	int     nodes1 = sy->thickNodeNumber - 1, iz = 0, iz1 = 2, izp1 = 1;
	int     it1,it2;
	float   lambda1, lambda2, c_eins, c_zwei;
	float   lm1, lm2, m_eins, m_zwei;
	float   sum1, sum2, r_null;
	float   t2, t3;
	float   xt1, xt2, xt3;
	float   k1, k2, k3;
	float   a0, a1, a2;
	float   xx1;
	float   d0, d1, d2;     /* enthalpy differences on thickNodeNumber */
	float     dx0, dx1, dx2;        /* Integral{v(dX(t)/detaTime)detaTime} on thickNodeNumber */
	float     x_null, x_eins,  x_zwei;      /* element 信息 page FEM/N/3 */
	float     xr_eins;
	float   xhil1, xhil2, ezw =  8.33333333e-02f;
	float     teta = 0.666666667f;
	float     dt_teta = teta * detaTime;
	float   dt_1m_teta = (1.f-teta)*detaTime;

	float   t_null;
	int     ischalter = ax->sw_air;
	if(ischalter == 10) ischalter = 0; /* non ferrous mat */

	t2      = temp_kph[0];
	t3      = temp_kph[1];
	xt2     = temp_k[0];
	xt3     = temp_k[1];
	it1     = (int) t2;
	it2     = (int) t3;
	lambda2 = matal->para.lambda[it2];
	lm1     = sy->elementLength[0];
	sum2    = (matal->para.lambda[it1] + lambda2) * .5f /lm1;
	c_eins  =   sum2 + matal->alpha.bot[it1];
	c_zwei  =  -sum2;

	d1     =  matal->para.austenitEnthalpy[it1]-matal->para.ferritEnthalpy[it1];
	d2     =  matal->para.austenitEnthalpy[it2]-matal->para.ferritEnthalpy[it2];

	a1      = CopyValue(ax->sw_mat, matal,sy,0, sy->TimeKAusteniteDistribution[0], t2, xt2); 
	sy->TimeKpredRhoCp[0] = a1;
	a2      = CopyValue(ax->sw_mat, matal,sy,1, sy->TimeKAusteniteDistribution[1], t3, xt3);
	sy->TimeKpredRhoCp[1] = a2;

	dx1 = d1 * .01f * (sy->TimeKAusteniteDistribution[0] - sy->TimeKpredAusteniteDistribution[0]);
	dx2 = d2 * .01f * (sy->TimeKAusteniteDistribution[1] - sy->TimeKpredAusteniteDistribution[1]);

	xhil2   = lm1 * ezw;
	m_eins  = xhil2 * (3.f * a1 + a2);
	m_zwei  = xhil2 * (a1 + a2);
	x_eins  = 4.f * xhil2;
	x_zwei  = 2.f * xhil2;
	xr_eins = LATENT_HEAT_MULTIPLIER * (x_eins * dx1 + x_zwei * dx2);

	k_mod[0] = m_eins + dt_teta * c_eins;
	k_mod[1] = m_zwei + dt_teta * c_zwei;


	k2      = m_eins - dt_1m_teta * c_eins;
	k3      = m_zwei - dt_1m_teta * c_zwei;

	t_null  = ax->waterTemp;
	if(ischalter == 0)      t_null = ax->t_umg;
	r_null  =  t_null * matal->alpha.bot[it1];
	force[0] = xr_eins + r_null * detaTime + k2 * xt2 + k3 * xt3;


	while( ++iz < nodes1)   {       /* inner thickNodeNumber  */
		izp1++;
		k1      = k3;
		xhil1   = xhil2;
		x_null  = x_zwei;
		a0      = a1;
		a1      = a2;
		d0             = d1;
		d1      = d2;
		dx0       = dx1;
		dx1       = dx2;
		sum1    = sum2;
		lambda1 = lambda2;
		t2      = t3;
		t3      = temp_kph[izp1];
		xt1     = xt2;
		xt2     = xt3;
		xt3     = temp_k[izp1];
		it1     = it2;
		it2     = (int) t3;
		lm2     = sy->elementLength[iz];
		a2      = CopyValue(ax->sw_mat, matal,sy,izp1, sy->TimeKAusteniteDistribution[izp1], t3, xt3);
		sy->TimeKpredRhoCp[izp1] = a2;
		d2     =  matal->para.austenitEnthalpy[it2]-matal->para.ferritEnthalpy[it2];
		dx2    = d2 * .01f * (sy->TimeKAusteniteDistribution[izp1] - sy->TimeKpredAusteniteDistribution[izp1]);
		xhil2   = lm2 * ezw;
		m_eins  =  xhil1 * (a0 + 3.f * a1) + xhil2 * (3.f * a1 + a2);
		m_zwei  =  xhil2 * ( a1 + a2);
		x_eins  =  4.f * (xhil1 + xhil2);
		x_zwei  =  2.f * xhil2;
		lambda2 = matal->para.lambda[it2];
		sum2    =  (lambda1 + lambda2) * .5f /lm2;
		c_eins  =  sum1 + sum2;
		c_zwei  = -sum2;

		/*      estimation of matrix elements   */

		k_mod[iz1++] =  m_eins + dt_teta * c_eins;
		k_mod[iz1++] =  m_zwei + dt_teta * c_zwei;

		/*      estimation of force column element      */

		k2        = m_eins - dt_1m_teta * c_eins;
		k3        = m_zwei - dt_1m_teta * c_zwei;

		force[iz]  = k1 * xt1 + k2 * xt2 + k3 * xt3;
		force[iz] += LATENT_HEAT_MULTIPLIER * (x_null * dx0 + x_eins * dx1 + x_zwei * dx2);
	}
	xhil1 = xhil2 * ( a1 + 3.f * a2);

	if (sy->isSymetric == 0) sum2 += matal->alpha.top[it2];
	k_mod[iz1] =  xhil1 + dt_teta * sum2;

	r_null = 0.0f;
	if(sy->isSymetric == 0 ) {
		it1  =  (int) temp_kph[nodes1];
		d1 = d2;
		r_null = t_null * matal->alpha.top[it1];
	}
	xx1     = LATENT_HEAT_MULTIPLIER * (x_zwei * dx1 + 4.f * xhil2 * dx2);
	force[nodes1] = xx1 + r_null * detaTime + k3 * xt2 + (xhil1 - dt_1m_teta * sum2) * xt3;
}

float CTemperatureField::CalculationMeanTemperature( COMPUTATIONAL *sy, float *y)
{
	/*      calculation of caloric mean Temperature         */
	/*      calculation of rhoCpMeanValue  */
	float   t_cal = 0.0f;          /* calculated caloric mean Temperature  */
	float   xhil;
	float   sum1 = 0.0f , sum2 = 0.0f, sum3 = 0.0f, sum4 = 0.0f, sum5 = 0.0f;
	float   sum6 = 0.0f;
	float   y1;
	float   y2 = y[0];
	float   w1, w2 = sy->TimeKRhoCp[0];
	float   x1,   x2   = sy->TimeKAusteniteDistribution[0];
	float     xm1 , xm2  = (sy->TimeKm1AusteniteDistribution[0]-x2) * sy->relasedEnergy[0];
	float     dhx1, dhx2 = sy->relasedEnergy[0];
	int     iz = -1;
	int       iz1;
	int     iende = sy->thickNodeNumber - 1;

	sy->meanCalculationValue +=1;
	sy->outputParameter[0] += sy->detaTime;  /* acummulated time */
	while( ++iz < iende) {
		y1      =   y2;
		w1      =   w2;
		x1      =   x2;
		xm1     =   xm2;
		dhx1    =   dhx2;
		iz1     = iz +1;
		y2      =   y[iz1];
		w2      =   sy->TimeKRhoCp[iz1];
		x2      =   sy->TimeKAusteniteDistribution[iz1];
		dhx2    =   sy->relasedEnergy[iz1];
		xm2     =   (sy->TimeKm1AusteniteDistribution[iz1]-x2) * dhx2;
		xhil    =   .5f * sy->elementLength[iz];
		sum1   +=   xhil  * (w1 + w2);
		sum2   +=   xhil  * ( w1 * y1 + w2 * y2);
		sum3   +=   2.f* xhil;
		sum4   +=   xhil * (x1 + x2);
		sum5   +=    xhil * (xm1 + xm2);
		sum6   +=    xhil * (dhx1 + dhx2);
	}
	if(sum1 < 0.001) 
		return(t_cal);
	t_cal = sum2/sum1;
	sy->rhoCpMeanValue = sum1/sum3;
	/* percent * .01 and use of MW insteed of W => 1.e-6 */
	sy->latentHeat        -= 1.e-8f*(sum4/sum3-sy->austenitePercentMeanNextStep)*sum6/sum3;
	sy->austenitePercentMean  += sum4/sum3;
	return(t_cal);
}

float CTemperatureField::MiddleArit(      const   COMPUTATIONAL *sy,
	const   float *y)
{
	float   sum = 0.0f;
	float   thick = 0.0f;
	float   xhil;
	float   y1;
	float   y2 = y[0];
	int     iz = -1;
	int     iende = sy->thickNodeNumber - 1;

	while( ++iz < iende) {
		y1      =   y2;
		y2      =   y[iz + 1];
		xhil    =  sy->elementLength[iz];
		thick  +=  xhil;
		sum    +=  xhil  * .5f *( y1 + y2);
	}
	if(iende < 2) return(sum);
	sum   /= thick;
	return(sum);
}


void CTemperatureField::SolveAlgebraicMatrix( float *k,   /*  stiffness matrix  bandwidth 3 symm. */
	float *r,   /*  right upper matrix                  */
	float *y,   /*  column forward substitution         */
	float *b,   /*  column right side                   */
	float *c,   /*  calculated value                   */
	int n)      /*  dimension of the problem            */
{
	float   rxx;
	register    int ixx = 0;
	register    int iz  = 0;
	int         iz1 = 0;
	int         n1 = n - 1;

	/* loesung des gleichungssystems k*c = b (k-symmetrisch tridiagonal) */

	/* dreieckszerlegung */
	rxx  = (float)sqrt(k[0]);
	r[0] = rxx;
	while(++iz < n)
	{
		rxx = k[++ixx]/rxx;
		r[ixx] = rxx;
		rxx = (float) sqrt(k[++ixx]-rxx*rxx);
		r[ixx] = rxx;
	}
	/* vorwaertseinsetzen */
	rxx = b[0]/r[0];
	y[0] = rxx;
	ixx = -1;
	iz  = 0;
	while(++iz < n){
		ixx +=2;
		rxx  =  (b[iz] - r[ixx]*rxx)/r[ixx+1];
		y[iz] = rxx;
	}
	/* rueckwaertseinsetzen */
	ixx = n1 + n1;
	rxx = y[n1]/r[ixx];
	c[n1] = rxx;
	iz    =  0;
	iz1   =  n1;
	while(++iz < n){
		ixx -= 2;
		rxx = (y[--iz1] - rxx*r[ixx+1])/r[ixx];
		c[iz1] = rxx;
	}
}

//part 2 boundry
void  CTemperatureField::SolveBoundary( FILE       *st_err,
	const		  PHYSICAL *xx,
	MATERIAL *yy,
	const		  INPUTDATAPDI *pdi)
{
	//CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	//UFCDefine *ufcs=theApp->ufcs; 

	int i_check_boundary=0;

	/*      at first:       allocation of MATERIAL in CalcMaterialAttribute      */
	if(xx->sw_mat == 1)	{ /* ferreous material */
		if(xx->sw_air == 0)
			AlphaAir(st_err, &yy->alpha, xx);
		/* air   cooling */
		if(xx->sw_air == 1){   //水冷阶段 sw_air 为 空冷 水冷 等的判断位 谢谦 2012-5-7
			if (ACC==pdi->acc_mode||BACKACC==pdi->acc_mode) 
			{   //acc 0.2kg 冷却模式  此处应建立4种换热系数读取程序
				if (1==pdi->working_stage)   //高密段
					i_check_boundary=AlphaACCJET(&yy->alpha, xx,pdi->thick);
				else if(2==pdi->working_stage)  //缝隙段
					i_check_boundary=AlphaACCSLIT(&yy->alpha, xx);
			} 
			else if(UFC==pdi->acc_mode) //ufc 0.2kg 冷却模式
			{
				if (1==pdi->working_stage)   //jet
					i_check_boundary=AlphaUFCJET(&yy->alpha, xx);
				else if(2==pdi->working_stage)   //slit
					i_check_boundary=AlphaUFCSLIT(&yy->alpha, xx);
			}
			
			float waterTempLevel[5]={0.0f,10.0f,24.0f,30.0f,50.0f};
			float waterFactorLevel[5]={1.8f,1.6f,1.2f,0.9f,0.4f};
			float waterFactor=0.0f;
			int nwater=sizeof(waterTempLevel)/sizeof(waterTempLevel[0]);
			waterFactor=GetLinearInterpolationFromAbscissa( waterTempLevel, waterFactorLevel, nwater,xx->waterTemp);

			float thickLevel[6]={0.005f,0.020f,0.035f,0.040f,0.060f,0.10f};
			float thickFactorLevel[6]={1.0f,1.2f,1.35f,1.5f,2.2f,2.8f};
			float thickFactor=0.0f;
			int nthick=sizeof(thickFactorLevel)/sizeof(thickFactorLevel[0]);
			thickFactor=GetLinearInterpolationFromAbscissa( thickLevel, thickFactorLevel, nthick,pdi->thick);

			// add [1/13/2015 qian]
			waterFactor=1.0f;
			thickFactor=1.0f;

			int i=0;
			for (i;i<1201;i++)
			{
				yy->alpha.top[i]=yy->alpha.top[i]*xx->alpha_coe*xx->adaptRatioFound*waterFactor*thickFactor*pdi->manualHC;  // 临时增大1.5倍 [11/5/2013 谢谦]
				yy->alpha.bot[i]=yy->alpha.bot[i]*xx->alpha_coe*xx->adaptRatioFound*waterFactor*thickFactor*pdi->manualHC;
			}


		}
		/* 水冷 */
		if(xx->sw_air == 2)  
			AlphaSprayCool(st_err, &yy->alpha, xx);
		/* spray cooling */
		if(xx->sw_air == 3)  
			AlphaFile(st_err,&yy->alpha, xx);
		/* read alpha - data from FILE */
		if(xx->sw_air == 6)   
			AlphaFileByDifferent(6, st_err,&yy->alpha, xx);
		/* read alpha - data from FILE */
		if(xx->sw_air == 7)   
			AlphaFileByDifferent(7, st_err, &yy->alpha, xx);
		/* read alpha - data from FILE */
		if(xx->sw_air == 8)   
			AlphaFileByDifferent(8, st_err, &yy->alpha, xx);
		/* read alpha - data from FILE */
		if(xx->sw_air == 5)
			AlphaStrip(&yy->alpha, xx, pdi);
	} /* ferrous material */

	if(xx->sw_mat == 2) 
	{ /* nonferreous material */
		if(xx->sw_air == 0)	
			AlphaAir(st_err, &yy->alpha, xx);
		if(xx->sw_air == 3) /* non ferreous material from FILE */
			AlphaFileNotMetal(st_err, &yy->alpha, &yy->para, xx);
		if(xx->sw_air == 6)   
			AlphaFileByDifferent(6, st_err, &yy->alpha, xx);
		/* read alpha - data from FILE */
		if(xx->sw_air == 1)
			alpha_calc_ne(st_err, &yy->alpha, &yy->para, xx);
	}	/* nonferreous material */

	switch(i_check_boundary)
	{
	case 0:
		break;
	case 1:
		fprintf(st_err,"\n\t  板材 ID: %s", pdi->plateID);
		fprintf(st_err,"\n\t  message:  %s", "not enough memory in alpha spray");
		fprintf(st_err,"\n\t  function: %s \n", "SolveBoundary");
		break;
	case 2:
		fprintf(st_err,"\n\t  板材 ID: %s", pdi->plateID);
		fprintf(st_err,"\n\t  message:  %s", "没有打开换热系数FILE");
		fprintf(st_err,"\n\t  function: %s \n", "SolveBoundary");
		break;
	case 3:
		fprintf(st_err,"\n\t  板材 ID: %s", pdi->plateID);
		fprintf(st_err,"\n\t  message:  %s", "设定流量超出换热系数范围");
		fprintf(st_err,"\n\t  function: %s \n", "SolveBoundary");
		break;
	}

	return;
}

void CTemperatureField::AlphaFile( FILE *st_err, ALPHAVALUES *alpha, const	PHYSICAL *xy)
{
	FILE            *ffx = NULL;
	int             ixx = 0;
	char          alpha_f[100];
	int             ianz;
	int             iz;
	float           *al_bot, *al_top;
	float				temp, alpha_top, alpha_bot;
	int  				maxi = xy->maxPossibleMillTemp;


	CombinFilePath("alpha_f.dat", xy->taskID, alpha_f);
	if((ffx = fopen(alpha_f,"r")) == NULL ){
		fprintf(st_err,"\n\t *** not able to open FILE: alpha_f.dat ***");
		return ;
	}
	fscanf(ffx,"%d", &ianz);
	if((al_bot = CALLOC(ianz * 2 , float)) == NULL) ixx++;
	if((al_top = CALLOC(ianz * 2 , float)) == NULL) ixx++;

	if(ixx != 0){
		free(alpha_f);   //谢谦 添加 2012-7-22 
		printf("\n\t *** not enough memory in AlphaFile() *** \n\n");
		return ;
	}

	for ( ixx = -1, iz  = 0; iz  < ianz; iz++)		  {
		fscanf(ffx,"%f%f%f", &temp, &alpha_top, &alpha_bot);
		ixx++;
		al_bot[ixx] = temp;
		al_top[ixx] = temp;
		ixx++;
		al_top[ixx] = alpha_top;
		al_bot[ixx] = alpha_bot;
	}
	SetAlphaLimits( xy, ianz,al_bot);
	SetAlphaLimits( xy, ianz,al_top);
	CalculateLinearInterpolation(alpha->bot, al_bot, ianz, maxi);
	CalculateLinearInterpolation(alpha->top, al_top, ianz, maxi);

	fclose(ffx);
	free(al_bot);
	free(al_top);

}

/*    data alpha value from different FILE Numbers               */
/*    id_file = 6: FILE alpha_f1.dat                              */
/*    id_file = 7: FILE alpha_f2.dat                              */
/*    id_file = 8: FILE alpha_f3.dat                              */
/*    else:        FILE alpha_f.dat                               */
void CTemperatureField::AlphaFileByDifferent(
	int	id_file,	/* id for integer FILE	*/
	FILE *st_err,	/* error output			*/
	ALPHAVALUES  *alpha,	/* alpha value			*/
	const	   PHYSICAL *xy)
{
	/*	read alpha value from FILE : alpha_f.dat	*/

	FILE            *ffx = NULL;
	int             ixx = 0;
	int             ianz;
	int             iz;
	float           *al_bot, *al_top;
	float				temp, alpha_top, alpha_bot;
	int  				maxi = xy->maxPossibleMillTemp;
	char	 alpha_f[100];
	char	 alpha_f_id[100];
	char	*alpha_file[] = {"alpha_f.dat",
		"alpha_f1.dat",	/* id_file = 6 */
		"alpha_f2.dat",	/* id_file = 7 */
		"alpha_f3.dat"};	/* id_file = 8 */
	strcpy_s(alpha_f_id, alpha_file[0]);
	if(id_file == 6) strcpy_s(alpha_f_id, alpha_file[1]);
	if(id_file == 7) strcpy_s(alpha_f_id, alpha_file[2]);
	if(id_file == 8) strcpy_s(alpha_f_id, alpha_file[3]);
	if(xy->sw_mat == 1)  CombinFilePath(alpha_f_id, xy->taskID, alpha_f);
	else	{
		strcpy_s(alpha_f, "../ne_dat/");
		strcat(alpha_f, alpha_f_id);
	}

	if((ffx = fopen(alpha_f,"r")) == NULL )
	{
		fprintf(st_err,"\n\t *** not able to open FILE: %s ***",
			alpha_f);
		return ;	

	}
	fscanf(ffx,"%d", &ianz);
	if((al_bot = CALLOC(ianz * 2 , float)) == NULL) ixx++;
	if((al_top = CALLOC(ianz * 2 , float)) == NULL) ixx++;
	if(ixx != 0){
		printf("\n\t *** not enough memory in alpha_file() *** \n\n");
		return ;
		//exit(1);
	}

	for ( ixx = -1, iz  = 0; iz  < ianz; iz++){
		fscanf(ffx,"%f%f%f", &temp, &alpha_top, &alpha_bot);
		ixx++;
		al_bot[ixx] = temp;
		al_top[ixx] = temp;
		ixx++;
		al_top[ixx] = alpha_top;
		al_bot[ixx] = alpha_bot;
	}
	SetAlphaLimits( xy, ianz,al_bot);
	SetAlphaLimits( xy, ianz,al_top);
	CalculateLinearInterpolation(alpha->bot, al_bot, ianz, maxi);
	CalculateLinearInterpolation(alpha->top, al_top, ianz, maxi);

	fclose(ffx);
	free(al_bot);
	free(al_top);
}

void CTemperatureField::AlphaFileNotMetal( FILE       *st_err,
	ALPHAVALUES *alpha,
	const			  MATERIALPARAM *ccv,
	const			  PHYSICAL *xy)
{
	/*	read alpha value for no ferrous material: 	*/
	/* file_name: 10: "mat"air.dat 15:"mat"wat.dat  */

	FILE            *ffx = NULL;
	int             ixx = 0;
	int             ianz;
	int             iz;
	char			alpha_f[100];
	float           *al_bot, *al_top;
	float				temp, alpha_top, alpha_bot;
	int  				maxi = xy->maxPossibleMillTemp;

	if(xy->sw_air == 0) strcpy_s(alpha_f, ccv->air_dat);
	if(xy->sw_air == 3) strcpy_s(alpha_f, ccv->wat_dat);
	if((ffx = fopen(alpha_f,"r")) == NULL ){
		fprintf(st_err,"\n\t *** not able to open FILE: %s ***", alpha_f);
		return ;
		//exit(1);
	}
	fscanf(ffx,"%d", &ianz);
	if((al_bot = CALLOC(ianz * 2 , float)) == NULL) ixx++;
	if((al_top = CALLOC(ianz * 2 , float)) == NULL) ixx++;
	if(ixx != 0){
		printf("\n\t *** not enough memory in AlphaFile() *** \n\n");
		return;
		//exit(1);
	}

	for ( ixx = -1, iz  = 0; iz  < ianz; iz++){
		fscanf(ffx,"%f%f%f", &temp, &alpha_top, &alpha_bot);
		ixx++;
		al_bot[ixx] = temp;
		al_top[ixx] = temp;
		ixx++;
		al_top[ixx] = alpha_top;
		al_bot[ixx] = alpha_bot;
	}
	SetAlphaLimits( xy, ianz,al_bot);
	SetAlphaLimits( xy, ianz,al_top);
	CalculateLinearInterpolation(alpha->bot, al_bot, ianz, maxi);
	CalculateLinearInterpolation(alpha->top, al_top, ianz, maxi);

	fclose(ffx);
	free(al_bot);
	free(al_top);
	return;
}


int	CTemperatureField::SetAlphaLimits( 	const	PHYSICAL *xy,
	const	int			ianz,
	float			*alpha)

{
	/*	alpha gespeichert in Form von Wertepaaren (temp, alpha)		*/
	/*	ianz - Anzahl der Wertepaare					*/

	int	ix = 2*(ianz-1);	/* Index highest Temperature value	*/
	int  	maxi  = xy->maxPossibleMillTemp;
	float	t_umg = xy->t_umg;
	float	t_max = (float) maxi;

	if(t_umg < 0.0f)	  		t_umg = 30.f;
	if(alpha[0]  > t_umg) 	alpha[0] 	= t_umg;
	if(alpha[ix] < t_max)	alpha[ix] 	= (float)maxi;
	return(1);
}

void	CTemperatureField::AlphaStrip(ALPHAVALUES *alpha,
	const			PHYSICAL *xy,
	const			INPUTDATAPDI *pdi)
{
	FILE	*air_dat = NULL;
	int	iz;
	float	fv;
	float	f1 = 1.f, f2 = .5f;
	float	r, xr;
	float	ft;
	float	thick, speed, speed_up;
	char	lay_name[100];

	int	ial = 5;
	float	alx[] = { 0.f, 3700.f,	500.f, 2035.f, 600.f,1700.f,
		900.f, 1600.f, 2000.f, 2210.f};

	CombinFilePath("layout3.dat",xy->taskID,lay_name);
	if((air_dat = fopen(lay_name,"r")) == NULL)	{
		printf("\n\t *** unable to open FILE: LAYOUT3.DAT ***");
		return;
	}
	while((fscanf(air_dat,"%f%f%f", &thick, &speed, &speed_up)) != EOF) {
		if(thick > 1000.f * pdi->thick)	break;
	}
	fclose(air_dat);

	/*	meas.dat erweitern bzw Schnitt zur aktuellen Wassertemperatur	*/
	r	= speed/25.f;
	ft = 1.5f - .0167f * xy->waterTemp;
	fv = (1.f-r) * f1 + r * f2;
	xr = ft * fv;
	for(iz = 1; iz < 2* ial; iz+=2)	alx[iz] *= xr;

	if(pdi->pl_mode == 1)
		AlphaAirTop(alpha->top ,xy);
	else
		CalculateLinearInterpolation(alpha->top, alx, ial, xy->maxPossibleMillTemp);
	CalculateLinearInterpolation(alpha->bot, alx, ial, xy->maxPossibleMillTemp);
}


void CTemperatureField::AlphaAir(FILE *st_err, ALPHAVALUES *alpha, const PHYSICAL *xy)
{
	float					air_anl = .7f;
	int             ixx = 0;
	int             iz;
	int             maxi, it_umg;
	int             ianz;   /* no. of points        */
	float           temp;
	float           *al_top;
	float           *al_bot;

	maxi = xy->maxPossibleMillTemp;
	ianz = maxi/100 + 2;

	if((al_top = CALLOC(ianz * 2 , float)) == NULL)   ixx++;
	if((al_bot = CALLOC(ianz * 2 , float)) == NULL)   ixx++;
	if(ixx != 0){
		fprintf(st_err,"\n\t *** allocation memory: AlphaAir *** \n");
		return ;
	}
	it_umg = (int) xy->t_umg;
	if(it_umg < 0)
		it_umg = 50;
	temp  = (float) it_umg;
	al_top[0] = (float) it_umg;
	al_top[1] = 0.0f;
	al_bot[0] = (float) it_umg;
	al_bot[1] = 0.0f;

	for ( ixx = 1, iz  = 1; iz  < ianz; iz++){
		ixx++;
		temp +=100;
		al_top[ixx] = temp;
		al_bot[ixx] = temp;
		ixx++;
		al_top[ixx] = air_anl * alphal( 0, xy->speed, temp, xy->t_umg, 0.68f)*2.0f;   // modify by xie 8-22   9-19
		al_bot[ixx] = air_anl * alphal( 1, xy->speed, temp, xy->t_umg, 0.68f)*2.0f;
	}
	CalculateLinearInterpolation(alpha->top, al_top, ianz, maxi);
	CalculateLinearInterpolation(alpha->bot, al_bot, ianz, maxi);
	free(al_top);
	free(al_bot);
}

float  CTemperatureField::alphal( int iseite, float v, float temp, float t_um, float grau)
{

	float	sb	=      5.67e-8f;
	float   fac, fax, f2, alpha;

	v = v * 0.14f;
	fac = 2.48f;

	if ( iseite == 1)
		fac = 1.31f;
	fac = fac * pow((temp - t_um),static_cast<float>(0.25));
	if( v > 5.f)
		f2 = 7.12f * pow(v, static_cast<float>(0.78));
	else
		f2 = 5.8f + 3.9f * v;
	alpha = (float)sqrt( fac * fac + f2 * f2);
	temp  += 273.0f;
	t_um  += 273.0f;
	fac    = temp * temp;
	fax    = t_um  * t_um;
	f2 = (fac + fax) * (fac -fax);
	f2 *= grau * sb / (temp - t_um);
	alpha += f2;

	return(alpha);
}

void CTemperatureField::AlphaSprayCool( FILE *st_err, ALPHAVALUES *alpha, const	PHYSICAL *xy)
{

	FILE            *ffx = NULL;
	float           temp;
	int             it_umg;
	int             ixx = 0;
	char            zeile[200];
	char            *alpha_s;
	float           al[10], al_alt[10], t[20];
	int             max = 199, maxi, ianz;
	int             anz_temp, anz_flow, iz;
	float           flow_spray, flow_alt, max_flow, flow, r;
	float           *al_bot;


	flow_spray      = xy->topFlowRate;
	maxi            = xy->maxPossibleMillTemp;
	ianz = maxi/100 + 2;

	if((al_bot = CALLOC(ianz * 2 , float)) == NULL)
		ixx++;
	if(ixx != 0){
		printf("\n\t *** not enough memory in alpha spray *** \n\n");
		return ;
	}
	/*      spray cooling at top side       */
	alpha_s = CALLOC(100, char);
	CombinFilePath("alpha_s.dat", xy->taskID,alpha_s);
	if((ffx = fopen(alpha_s,"r")) == NULL ){
		fprintf(st_err,"\n\t *** not able to open FILE: alpha_s.dat ***");
		free(al_bot);   //谢谦 添加 2012-7-22 
		free(alpha_s);
		return ;
	}
	fgets(zeile,max,ffx);
	fscanf(ffx,"%d%d%f", &anz_temp, &anz_flow, &max_flow);
	if(flow_spray > max_flow)
	{
		free(al_bot);   //谢谦 添加 2012-7-22 
		free(alpha_s);
		fprintf(st_err,"\n\t *** required spray flow = %g > possible flow %g ***",
			flow_spray, max_flow);
		return;
	}
	fgets(zeile,max,ffx);
	fgets(zeile,max,ffx);
	sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &t[0],&t[2],&t[4],&t[6],
		&t[8],&t[10],&t[12],&t[14]);
	iz = -1;
	while(++iz < anz_temp)
		al_alt[iz] = 0.0f;
	flow_alt = 0.0f;
	while(fgets(zeile,max,ffx)){
		sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &al[0],&al[1],&al[2],&al[3],
			&al[4],&al[5],&al[6],&al[7]);
		if(flow > flow_spray)
			break;
		iz = -1;
		while(++iz < anz_temp)
			al_alt[iz] = al[iz];
		flow_alt = flow;
	}

	ixx = - 1;
	r = (flow - flow_spray)/(flow - flow_alt);
	iz = -1;
	while(++iz < anz_temp){
		ixx +=2;
		t[ixx] = al[iz] * ( 1.f - r) + al_alt[iz] * r;
	}

	fclose(ffx);

	CalculateLinearInterpolation(alpha->top, t, anz_temp, maxi);
	it_umg = (int) xy->t_umg;
	if(it_umg < 0)
		it_umg = 30;
	temp  = (float) it_umg;
	al_bot[0] = (float) it_umg;
	al_bot[1] = 0.0f;

	for ( ixx = 1, iz  = 1; iz  < ianz; iz++){
		ixx++;
		temp +=100;
		al_bot[ixx] = temp;
		ixx++;
		al_bot[ixx] = alphal( 1, xy->speed, temp, xy->t_umg, 0.68f);
	}
	CalculateLinearInterpolation(alpha->bot, al_bot, ianz, maxi);

	free(al_bot);
	free(alpha_s);
}

void CTemperatureField::AlphaAirTop( float *alpha, const	PHYSICAL *xy)
{
	int             iz;
	int					ixx;
	int             maxi, it_umg;
	int             ianz;   /* no. of points        */
	float           temp;
	float           al_top[100];

	maxi = xy->maxPossibleMillTemp;
	ianz = maxi/100 + 2;

	it_umg = (int) xy->t_umg;
	if(it_umg < 0)
		it_umg = 50;
	temp  = (float) it_umg;
	al_top[0] = (float) it_umg;
	al_top[1] = 0.0f;

	for ( ixx = 1, iz  = 1; iz  < ianz; iz++){
		ixx++;
		temp +=100.f;
		al_top[ixx] = temp;
		ixx++;
		al_top[ixx] = alphal( 0, xy->speed, temp, xy->t_umg, 0.68f);
	}
	CalculateLinearInterpolation(alpha, al_top, ianz, maxi);
}

//part 3 *CalcMaterialAttribute  物性参数计算
MATERIAL CTemperatureField::CalcMaterialAttribute( FILE *st_err, const	INPUTDATAPDI *pdi, PHYSICAL *xx)
{
	MATERIAL mat_box;
	int             ixx = 0, maxi = xx->maxPossibleMillTemp;
	if(xx->sw_mat == 1) 
		SteelMaterial(st_err, &mat_box.para, pdi, xx);
	else					  
		NotSteelMaterial(st_err, &mat_box.para, pdi, xx);
	return(mat_box);
}

void    CTemperatureField::SteelMaterial(FILE       *st_err,
	MATERIALPARAM *ccv,
	const		INPUTDATAPDI *pdi,
	PHYSICAL *xcc)
	/* estimation of lambda- and    */
{                        /* rho * cp-value              */
	int             icurvex;
	int             maxi;

	icurvex      = xcc->mat_id;   /* no. of lambda-curve  */
	maxi = xcc->maxPossibleMillTemp;
	CalcLambda(   st_err, ccv->lambda, pdi, maxi);
	ccv->ar3 = CalcRhoCx( st_err, xcc->sw_air, ccv ,pdi, xcc);
}

void CTemperatureField::NotSteelMaterial(FILE *st_err,
	MATERIALPARAM *ccv,
	const			  INPUTDATAPDI *pdi,
	const			  PHYSICAL *xcc)
{
	char  mat_name[100];
	char  *file_name[5];
	int	nf = 5;
	int iz = -1;
	int maxi;
	int  nx = 0; /* number of value per curve */
	float	xhil;
	float xx[100];
	float *value[5];

	maxi =xcc->maxPossibleMillTemp;

	ccv->ar3 = 650.f;
	while(++iz < nf) {
		if((file_name[iz] = CALLOC(50, char)) == NULL)nx = 1;
		if((value[iz] = CALLOC(maxi+1, float)) == NULL)nx = 1;
		if(nx)  {
			printf("\n\t not able to allocate :file_name + value ");
			fprintf(st_err,"\n\t error allocate :file_name + value %s",
				pdi->plateID);
			return;
			//exit(1);
		}
	}
	
	printf("\n\t Input of material type ");
	printf("\n\t available material types : ");
	printf("\n\t\t AlMg4,5Mn");
	printf("\n\t\t Copper_kx");
	printf("\n\t\t St37");
	printf("\n\t input of name  ?   :    ");
	scanf("%s", mat_name);
	printf("%s", mat_name);
	EstimationParamName(mat_name,file_name);
	iz = -1; while(++iz < nf) {
		printf("\n\t filename[%d] : %s", iz, file_name[iz]);
	}
	strcpy_s(ccv->rho_dat,file_name[0]);
	strcpy_s(ccv->cp_dat,file_name[1]);
	strcpy_s(ccv->lam_dat,file_name[2]);
	strcpy_s(ccv->air_dat,file_name[3]);
	strcpy_s(ccv->wat_dat,file_name[4]);

	iz = -1; while(++iz < 3)	{
		nx = LoadCurve(file_name[iz], xx);
		CalculateLinearInterpolation(value[iz], xx, nx, maxi);
		printf("\n\t val[%d][0] %g [%d][100] %g [%d][1200] %g",
			iz, value[iz][0],iz, value[iz][100],iz, value[iz][1200]);
	}

	iz = -1;
	while(++iz < maxi)	{
		xhil			   = value[0][iz] * value[1][iz];
		ccv->rhoCp[iz]  = xhil;
		ccv->rhoCpa[iz] = xhil;
		ccv->rhoCpg[iz] = xhil;
		ccv->ferritEnthalpy[iz]     = 0.0;
		ccv->austenitEnthalpy[iz]     = 0.0;
		ccv->lambda[iz]  = value[2][iz];
	}

	iz = -1; while(++iz < nf) {
		free(file_name[iz]);
		free(value[iz]);
	}
}

int CTemperatureField::LoadCurve(char  *f_name, /* name of FILE */
	float	*xx)     /* function value */
{
	FILE *trans = NULL;
	char line[100];
	int		max_line = 99;
	int  n = 0;
	int  iz = -2;
	if((trans = fopen(f_name,"r")) == NULL) return(n);
	fgets(line,max_line,trans);
	while(fgets(line,max_line,trans)!=NULL)	{
		iz +=2;
		n++;
		sscanf(line,"%f%f", &xx[iz], &xx[iz+1]);
	}
	fclose(trans);
	return(n);
}

int   CTemperatureField::EstimationParamName(char *mat_name, char **file_name)
{
	int ityp;
	char *fname[] = {"Al1_", "Cu1_", "St37_"};
	char *exten[] = {"rho.dat", "cp.dat","lam.dat", "air.dat", "wat.dat"};
	int iz = -1;

	ityp = 2; /* use only steel grades */

	while(++iz < 5)	{
		strcpy(file_name[iz],"../ne_dat/");
		strcat(file_name[iz],fname[ityp]);
		strcat(file_name[iz],exten[iz]);
	}
	return TRUE;
}

void CTemperatureField::CalcLambda(FILE *st_err, float *lxl, const	INPUTDATAPDI *pdi, const int tmaxi)
{
 
	/* ncurve von 1 bis 7 */
	int     index, n_formula, i = 0;
	int     anz_curves = 7;
	int		ncurve;
	int     IST = 24;
	int     ist2, iz = 0;
	float   xx[50];
	memset(xx,0,50);   //初始化 谢谦 2012-7-23
	float   lam4, r, rm1;

	float y[168]= {	55.5f, 53.8f, 50.8f ,47.7f ,40.0f, 37.5f, 26.2f,
		54.4f, 53.0f, 50.2f ,46.5f ,40.3f ,38.4f, 27.5f,
		53.0f ,52.0f, 49.0f ,45.4f ,40.3f ,38.8f ,28.8f,
		51.5f, 50.6f, 47.6f, 44.2f ,40.1f, 39.0f, 29.7f,
		49.9f, 49.2f, 46.0f, 43.0f ,39.7f, 38.9f ,30.5f,
		48.0f, 47.3f, 44.5f, 41.8f,39.1f, 38.4f ,30.9f,
		46.0f, 45.4f, 42.8f, 40.6f,38.5f, 37.7f ,31.0f,
		44.0f, 43.4f, 41.0f, 39.3f,37.5f, 37.0f ,31.0f,
		41.8f, 41.5f, 39.4f, 37.9f,36.4f, 36.1f ,30.8f,
		39.7f, 39.4f, 37.7f, 36.4f,35.0f, 35.0f ,30.5f,
		37.7f, 37.4f, 35.9f, 34.9f,33.7f, 34.0f ,30.0f,
		35.5f, 35.0f, 34.0f, 32.9f,31.6f, 33.0f ,29.0f,
		33.8f, 33.4f, 32.4f, 31.4f,30.0f, 31.2f ,28.1f,
		31.8f, 31.5f, 30.5f, 30.3f,28.4f, 29.5f ,27.3f,
		29.3f, 29.1f, 28.8f, 28.2f,27.4f, 28.2f ,26.5f,
		27.7f, 27.7f, 27.3f, 27.0f,26.3f, 27.0f ,25.9f,
		27.1f, 27.1f, 27.0f, 26.4f,25.9f, 26.4f ,25.9f,
		27.3f, 27.3f, 27.2f, 26.8f,26.3f, 26.8f ,26.3f,
		27.5f, 27.5f, 27.3f, 27.1f,26.7f, 27.1f ,26.7f,
		27.6f, 27.6f, 27.4f, 27.4f,27.0f, 27.4f ,27.0f,
		27.6f, 27.6f, 27.4f, 27.4f,27.0f, 27.4f ,27.0f,
		27.6f, 27.6f, 27.4f, 27.4f,27.0f, 27.4f ,27.0f,
		27.6f, 27.6f, 27.4f, 27.4f,27.0f, 27.4f ,27.0f,
		27.6f, 27.6f, 27.4f, 27.4f,27.0f, 27.4f ,27.0f};
	/* lambda werte */

	int t[24] = { 50,100,150,200,250,300,350,400,450,500,
		550,600,650,700,750,800,850,900,950,1000,
		1050,1100,1150,2000}; /* Temperaturwerte */


	ist2 = 2 * IST;

	/*      Bestimmung der gueltigen lambda Kurven und lineare      */
	/*      Interpolation zwischen diesen Werten                    */
	/*      estimation of lambda-curve      */

	n_formula = 0;
	if(pdi->alloy_cr > .06f && pdi->alloy_ni > .06f)  n_formula = 1;
	if( n_formula == 0     && pdi->alloy_si > .03f)  n_formula = 2;
	if( n_formula == 0     && pdi->alloy_si <= .03f) n_formula = 3;

	if(n_formula == 1)      {
		lam4  =  48.61f - 13.96f * pdi->alloy_c -11.79f * pdi->alloy_si;
		lam4 -=   3.48f * pdi->alloy_mn + 2.85f * pdi->alloy_cr;
		lam4 -=   1.18f * pdi->alloy_ni;
	}
	if(n_formula == 2)      {
		lam4  = 49.65f - 6.07f * pdi->alloy_c + 10.98f * pdi->alloy_si;
		lam4 -= 15.21f * pdi->alloy_mn;
	}
	if(n_formula == 3)
		lam4  = 49.f - 7.f * pdi->alloy_c - 10.f * pdi->alloy_mn;

	index = 7 * 7 - 1;
	while(++iz < 7) {
		if(y[++index] < lam4) break;
	}
	ncurve = iz;

	r       = (y[index] - lam4)/(y[index] - y[index-1]);
	rm1     = 1.f - r;

	index = ncurve - anz_curves - 1;
	iz = 0;
	while( i < ist2) {
		xx[i++] = (float) t[iz++];
		index   +=  anz_curves;
		if(ncurve == 1 || ncurve == 7) xx[i++] = y[index];
		else  xx[i++] = rm1 * y[index-1] + r * y[index];
	}

	for (iz=0;iz<50;iz++){
		xx[iz]=1.2f*xx[iz];        //add by xie 9-18 增大导热系数 原导热系数对于厚板明显过小
	}

	CalculateLinearInterpolation(lxl, xx, IST, tmaxi);

}

float CTemperatureField::CalcRhoCx(	FILE 	*st_err, /* error messages	*/
	const  int   c_type,
	MATERIALPARAM *ccv, /* cp - value */
	const	INPUTDATAPDI *pdi,	 /*PDI数据	*/
	PHYSICAL *xxn) /* 过程 参数	*/
{
	/* calculation of rho * cp - value for Temperature iz  */

	FILE            *ffx = NULL;   /* *ferg_rcp;   */
	int             iz = -1, ix = 0 , ianz;
	float           cc, cmn, ae1, ae3, grau_top, grau_bot;
	float           rhoc[30];
	float           tempx[30], cp[30];
	int              iorder[30];
	char            line[VALUE_PER_CURVE];

	memset(rhoc,0,30);
	memset(tempx,0,30);
	memset(cp,0,30);
	memset(iorder,0,30);
	/* 0. read data of specific heat capacity  */

	r_cp_file( pdi, xxn->taskID, xxn->datname);
	if(pdi->taskID == PRE_CALC_SIM)	{	/* only simulation */
		if ((ffx = fopen(xxn->datname,"r")) == NULL) {
			fprintf(st_err,"\n\t *** error by opening rho-data FILE %s ***",
				xxn->datname);
		}

		fgets(line,VALUE_PER_CURVE-1,ffx);    /* 1. read leading statement    */
		fscanf(ffx,"%f%f%s%s", &grau_top, &grau_bot, line, line);
		fscanf(ffx,"%f%f%s%s", &cc, &cmn, line, line);
		fscanf(ffx,"%f%f%s%s", &ae1, &ae3, line , line);
		fscanf(ffx,"%d",&ianz);

		while(++iz < ianz) fscanf(ffx,"%f%f", &tempx[iz], &cp[iz]);
		fclose(ffx);
		ianz ++;                /*      upper Temperature value */
		tempx[ianz-1] = 2000.f;
		cp[ianz-1]    = cp[ianz-2];
		ccv->ar3 = CalculateAr3TemperatureSAB(pdi, c_type);
		ae1_ae3(ae1, ae3,ccv->ar3, ianz, tempx);

	}	/* simulation PREPROCESS */

	/*      1.      纠正 for转变点 ar3    */
	ccv->ar3 = CalculateAr3TemperatureSAB(pdi, c_type);

	CalculateEnthalphy(pdi,c_type, ccv,xxn); /* 过程 参数	*/

	return(ccv->ar3);
}

/* -----------------calculate enthalphy--------------------------------*/

int	CTemperatureField::CalculateEnthalphy(	
	const	INPUTDATAPDI *pdi,
	const	int			c_type,
	MATERIALPARAM *ccv, /* cp - value */
	PHYSICAL *xxn) /* 过程 参数	*/
{
	char	line[101];
	char  basic_air[20], basic_wat[20];
	char  *basic_nb = "x_nb_1.dat";
	int	iz;
	float	xx[AUSTENITIC_PERCENT], yy[AUSTENITIC_PERCENT];
	CURVE air_trans,wat_trans, nb_trans;
	int	ianz;
	int	maxi = xxn->maxPossibleMillTemp;
	float CR = 5.0;
	EstimateBasicCoolingCurve(pdi, basic_air, basic_wat); /* ask for basic curve name */
	CombinFilePath(basic_air,PREPROCESS,line);       /* build name for FILE (path)  */
	air_trans =  init_curve_f(line, 0, pdi->alloy_c);  /* air transf.    */

	CombinFilePath(basic_wat,PREPROCESS,line);

	CR = pdi->cr;

	wat_trans =  init_curve_f(line, 0, CR);   /* water */

	if(pdi->taskID == PRE_CALC_SIM)	{
		ausg_curve(&air_trans, "x_trans1.dat", 0); /* Kontrolle */
		ausg_curve(&wat_trans, "x_trans1.dat", 1); /* Kontrolle */
	}
	/* 1. basic空冷 curve */
	AddValue(0, 0, AUSTENITIC_PERCENT, xx, yy, &air_trans);  /* add water constr. to air */
	/* 2. influence of niobium */
	if(pdi->alloy_nb > .01) {						/*  纠正ion for nb */
		CombinFilePath(basic_nb, PREPROCESS,line);
		nb_trans =  init_curve_f(line, 0, pdi->alloy_nb);
		if(pdi->taskID == PRE_CALC_SIM)
			ausg_curve(&nb_trans, "x_trans1.dat", 1); /* Kontrolle */
		AddValue(1, 0, AUSTENITIC_PERCENT, xx, yy, &nb_trans);

	}
	/* 3. multiply CR with 水冷 curves */
	if(c_type > 0) {
		iz = -1;
		while(++iz < wat_trans.n) 
			wat_trans.y[iz] *= CR;
		if(pdi->taskID == PRE_CALC_SIM)
			ausg_curve(&wat_trans, "x_trans1.dat", 1); /* Kontrolle */
		AddValue(1,0, AUSTENITIC_PERCENT, xx, yy, &wat_trans);
	}

	/* calculate one value of 奥氏体分解 per percent */
	ccv->austenitTransPercent = InitCurve(xx,yy, AUSTENITIC_PERCENT,1,0, pdi->alloy_c);
	ccv->ar1 = ccv->ar3 - ccv->austenitTransPercent.y[0];
	if(pdi->taskID == PRE_CALC_SIM)
		ausg_curve(&ccv->austenitTransPercent, "x_trans1.dat", 1); /* Kontrolle */

	/* calculate cp_alpha and cp_gamma */
	ianz = CalculateCP(xx,yy);

	CalcRhoCpx(xx,ianz);
	CalcRhoCpx(yy,ianz);
	CalculateLinearInterpolation(ccv->rhoCpa, xx, ianz, maxi);
	CalculateLinearInterpolation(ccv->rhoCpg, yy, ianz, maxi);
	/* calculate enthalpy - value */
	ianz = CalculateH(xx,yy);
	CalcRhoCpx(xx,ianz);
	CalcRhoCpx(yy,ianz);
	CalculateLinearInterpolation(ccv->ferritEnthalpy, xx, ianz, maxi);
	CalculateLinearInterpolation(ccv->austenitEnthalpy, yy, ianz, maxi);

	return(1);
}

int	CTemperatureField::EstimateBasicCoolingCurve(
	const	INPUTDATAPDI *pdi,
	char *air_fname,
	char *wat_fname)
{
	/* estimate basic空冷 curve */
	char *str_1 = "x_air_";
	char *str_2 = "x_wat_";

	float c_cont = pdi->alloy_c;
	strcpy(air_fname, str_1);
	strcpy(wat_fname, str_2);
	if(c_cont < .11){
		strcat(air_fname, "1.dat");
		strcat(wat_fname, "1.dat");
		return(1);
	}
	if((c_cont >=.11)&&(c_cont < .3)) {
		strcat(air_fname, "2.dat");
		strcat(wat_fname, "2.dat");
		return(2);
	}
	strcat(air_fname, "3.dat");
	strcat(wat_fname, "3.dat");
	return(3);
}

int	CTemperatureField::CalculateCP(
	float   *x,  /* cp_alpha */
	float   *y)  /* cp_gamma */
{
	int ianz = 0;
	int iwert = 0;
	float	temp = 50.f;
	float	dtemp = 50.f;
	x[iwert] = 1.f;
	y[iwert] = 1.f;
	x[++iwert] = 470.f;  /* spec. heat cap. -10 C (Richter 1973) */
	y[iwert]   = CalcCarbenGam(1.f);
	ianz++;
	for(;;)	{  /* use curve */
		temp +=dtemp;
		if(temp > MAXTEMP) break;
		x[++iwert] = temp;
		y[iwert]   = temp;
		x[++iwert] = CalcCarbenAl(temp);
		y[iwert]   = CalcCarbenGam(temp);
		ianz++;
	}	  /* use curve */
	return(ianz);
}

int	CTemperatureField::CalculateH(
	float   *x,  /* h_alpha */
	float   *y)  /* h_gamma */
{
	int ianz = 0;
	int iwert = 0;
	float	temp = 50.f;
	float	dtemp = 50.f;
	x[iwert] = 1.f;
	y[iwert] = 1.f;
	x[++iwert] = CalcHAl(1.f);
	y[iwert]   = CalcHGam(1.f);
	ianz++;
	for(;;)	{  /* use curve */
		temp +=dtemp;
		if(temp > MAXTEMP) break;
		x[++iwert] = temp;
		y[iwert]   = temp;
		x[++iwert] = CalcHAl(temp);
		y[iwert]   = CalcHGam(temp);
		ianz++;
	}	  /* use curve */
	return(ianz);
}

float	CTemperatureField::CalcHGam(	const float temp)
{
	float	h_gam;
	float bm1 = 1.221e5f;
	float b[]={505.f, 0.1f};
	int n_poly = 1; /* polynom first grade for cp */
	h_gam = CalculateIntegalPoly(temp, n_poly, bm1,b);
	return(h_gam);
}

float	CTemperatureField::CalcHAl(	const float t)
{
	float	h_al;
	int   n_poly = 3; /* grade of polynom cp */
	float	am1 = -2.452e4f;
	float	a[]= { 422.f,0.931f,-2.142e-3f,2.64e-6f};
	h_al = CalculateIntegalPoly(t, n_poly, am1, a);
	return(h_al);
}

float CTemperatureField::CalculateIntegalPoly(	const float x,
	const int order,
	const float am1,
	const float *a)
{
	float int_fx = am1;
	int iz = 0;
	float xx = 1.f;
	while(++iz <= (order+1)) {
		xx *= x;
		int_fx += a[iz-1]*xx /(float)iz;
	}
	return(int_fx);
}


float	CTemperatureField::CalcCarbenGam(	const float temp)
{
	float	c_gam;
	c_gam = 505.f + .1f*temp;
	return(c_gam);
}

float	CTemperatureField::CalcCarbenAl(	const float t)
{
	
	//  [8/10/2013 谢谦]低 alloy carbon 100 C < T < Ar3 
	float	c_al;
	c_al = 422.f+t*(.931f+t*(-.002143f+t*2.64e-6f));
	return(c_al);
}

int    CTemperatureField:: CalcRhoCpx(  float   *rhoc, const int ianz)
{
	int i, iz;
	float temp, wert;

////////?????????????????????????????
	for ( i= -1, iz = 0; iz <= ianz; iz++)  
	{

		i +=2;
		temp =  rhoc[i-1];
		/*	wert = 1000. * (7850. - .3182 * temp);  */
		wert = (7850.f - .3182f * temp);
		/* (rho * c)- value */
		rhoc[i] *= wert;
		
	}
	return(1);
}

/* calculate Ar3 -Temperature on request of SSAB */

float   CTemperatureField::CalculateAr3TemperatureSAB(   const INPUTDATAPDI *pdi,
	const int     c_type)
{
	float ar3;                      /* Temperature of upper transformation */
	float xmax = .001f;              /* avoid zero alloy from PDI           */
	float xhil;
	float crx = pdi->cr;
	int	middle = NODES_NUMBER/2;
	int i_ende = NODES_NUMBER-1;

	float	grain_size = 0.33f * (2.f * pdi->grain_size[middle] + pdi->grain_size[0]);
	float	aust_decomp= 0.33f * (2.f * pdi->aust_start[middle] + pdi->aust_start[0]) * .01f;

	if(c_type == 0) {
		crx = 4.93535e-4f*pdi->finishRollTemp;
		crx = (float)pow(crx,4.f)/pdi->thick;
		if(crx < .4f ) crx = .401f;
		xhil = 310.f * pdi->alloy_c + 80.f * pdi->alloy_mn;
		if ( xhil > .001f) xmax = xhil;
		ar3 = 910.f - xhil;
		ar3 -= 20.f * pdi->alloy_cu + 15.f * pdi->alloy_cr;
		ar3 -= 55.f * pdi->alloy_ni + 80.f * pdi->alloy_mo;
		ar3 += (4.5f * (float)log10(xmax) - 11.7f) * (crx - .4f);
	}
	else{
		if(grain_size < 20.f) grain_size = 20.f;
		if(grain_size > 80.f) grain_size = 60.f;
		if((aust_decomp < 0.5f)||(aust_decomp > 0.99f)) aust_decomp = 1.f;

		ar3  = 757.72f - 637.4f * pdi->alloy_nb -2.8901f * crx;
		ar3 += 7.34f * aust_decomp - 0.36755f * grain_size;

	}
	return(ar3);
}

/* -------------------shifting indices ------------ ------------------*/

int  CTemperatureField::ae1_ae3(   const float ae1,
	const float ae3,
	const float ar3,
	const int   ianz,
	float *tempx)
{
	int iz, nae3, nae1;
	float td;

	iz = -1;
	while(++iz < ianz)      {
		if(tempx[iz + 1] >  ae1)        {
			nae1 = iz;
			break;
		}
	}
	iz = -1;
	while(++iz < ianz)      {
		if(tempx[iz + 1] >  ae3)        {
			nae3 = iz;
			break;
		}
	}

	td      = ae3 - ar3;

	for ( iz = nae1 +1 ; iz <= nae3; iz++) tempx[iz] -= td;
	if((nae3 + 2) < ianz) tempx[nae3 +1] = ar3+1.f; /* check against limit   */
	return(1);
}


CURVE CTemperatureField::InitCurve(  const  float *x,
	const  float *y,
	const  int    n,
	const  int    init, /* 0: zero init 1: init */
	const  int    iabl, /* no. of derivatives */
	const  float  c_id)
{
	CURVE cx;
	int      ixx = 0;
	int      iz = -1;
	float		yw[3];
	float absz, ord;

	cx.n = n;
	cx.c_id = 0.0f;
	cx.iabl = iabl;
	cx.x_min =  FLT_MAX;
	cx.x_max = -FLT_MAX;
	cx.y_min =  FLT_MAX;
	cx.y_max = -FLT_MAX;

	if(init == 0) 
		return(cx);
	while(++iz < n)	{
		absz = x[iz];
		ord  = y[iz];
		cx.x[iz] = absz;
		cx.y[iz] = ord;
		if(ord > cx.y_max) 
		{
			cx.x_max = absz;
			cx.y_max = ord;
		}
		if(ord < cx.y_min) 
		{
			cx.x_min = absz;
			cx.y_min = ord;
		}
	}
	cx.c_id = c_id; /* identification value for curve */
	if(iabl > 0) {
		iz = -1;
		while(++iz < n)	{
			spline_1(&cx,2,x[iz],yw);
			cx.yp1[iz] = yw[1];
			if(iabl > 1)
				cx.yp2[iz] = yw[2];
		}
	}
	return(cx);
}

CURVE CTemperatureField::init_curve_f(
	const  char *f_name,
	const  int    iabl, /* no. of derivatives */
	const  float  c_id)
{
	CURVE cx;
	int      ixx = 0;
	int      iz =  -1;
	int		n = 0;
	float		yw[3];
	float    absz, ord;
	FILE		*dat_f;

	cx.n = n;
	cx.c_id = 0.0f;
	cx.iabl = iabl;
	cx.x_min =  FLT_MAX;
	cx.x_max = -FLT_MAX;
	cx.y_min =  FLT_MAX;
	cx.y_max = -FLT_MAX;

	if((dat_f = fopen(f_name, "r")) == NULL) {
		printf("\n\t *** not able to open : %s", f_name);
		return(cx);
	}
	while((fscanf(dat_f,"%f%f", &absz, &ord))!= EOF)
		n++;
	rewind(dat_f);

	while(++iz < n)	{
		fscanf(dat_f,"%f%f", &absz, &ord);
		cx.x[iz] = absz;
		cx.y[iz] = ord;
		if(ord > cx.y_max) {
			cx.x_max = absz;
			cx.y_max = ord;
		}
		if(ord < cx.y_min) {
			cx.x_min = absz;
			cx.y_min = ord;
		}
	}
	cx.c_id = c_id; /* identification value for curve */
	if(iabl > 0) {
		iz = -1;
		while(++iz < n)	{
			spline_1(&cx,2,cx.x[iz],yw);
			cx.yp1[iz] = yw[1];
			if(iabl > 1)
				cx.yp2[iz] = yw[2];
		}
	}
	fclose(dat_f);
	return(cx);
}

/* add value per int to float arrays */

int  CTemperatureField::AddValue(
	const int   coolPhaseNumber,
	const int   ianf,
	const int   iende,
	float *xx, /* absz. value */
	float *yy, /* ord. value */
	const CURVE *cx)
{
	int iz = ianf -1;
	float yw[3];
	if (coolPhaseNumber == 0) { /* initialization */
		while(++iz < iende) {
			xx[iz] = (float) iz;
			yy[iz] = 0.0f;
		}
	}                  /* initialization */
	iz = ianf - 1;
	while(++iz < iende) {
		spline_1(cx,2, xx[iz], yw);
		yy[iz] += yw[0];
	}
	return(1);
}

float	CTemperatureField::SearchAbsz(	const CURVE *cx,
	const float   x_old, /* old Absz. value */
	const float   y_new)
{
	float x_new;  /* return value */
	float yw[3];
	float y_x_old;
	float yp1_x_old; /* 一阶导数 at x_old */
	int ix1, ix2, ix;
	int iint; /* estimated number of intervall */
	int istart; /* start index for intervall */

	if(y_new < cx->y_min) return(cx->x_min);
	if(y_new > cx->y_max) return(cx->x_max);
	spline_1(cx, 2, x_old, yw);
	y_x_old = yw[0];
	yp1_x_old = yw[1];
	if(yp1_x_old < 0.0) ix1 = -1;
	else                ix1 =  1;
	if(y_x_old < y_new) ix2 =  1;
	else                ix2 = -1;
	ix = ix1 * ix2;
	istart = intervall_no(cx, x_old);//此函数名疑是错别字 间隔
	iint   = intervall_id(cx->n, ix, istart, cx->y, y_new);
	x_new  = InterpolateAbszizzeY(cx, iint,y_new);
	return(x_new);
}

float CTemperatureField::InterpolateAbszizzeY(
	const	CURVE *cx,
	const	int iint,
	const float	y_new)
{
	/* interpolate abszizze value for y_new */
	/* intervall number is well known */
	float x_new = cx->x[iint];
	float yy1 = cx->y[iint];
	float yy2 = cx->y[iint+1];
	float r;
	float eps = .1e-23f;
	float dyy = yy2 - yy1;
	if(fabs(dyy) < eps ) 
		return(x_new);
	r = (y_new - yy1)/dyy;
	x_new = (1.f-r)* x_new + r * cx->x[iint+1];
	return(x_new);
}

int CTemperatureField::intervall_id(
	const int   n,
	const int   ix,   /* direction of search */
	const int   istart,
	const float *y,	/* FILE */
	const float ys)
{
	int ip;
	if(ix > 0) { /* positive search direction first */
		ip = SearchFirstPos(n,istart,y,ys);
		if(ip < 0)  ip = SearchFirstNegative(istart,y,ys);
		return(ip);
	}
	else			{ /* negative search direction first */
		ip = SearchFirstNegative(istart,y,ys);
		if(ip < 0)  ip = SearchFirstPos(n,istart,y,ys);
		return(ip);
	}
}

int CTemperatureField::SearchFirstPos(
	const int   n,
	const int   istart,
	const float *y,	/* FILE */
	const float ys)
{
	int ip;
	float ys_diff = ys - y[istart];
	float yi_diff;
	float eps = .1e-23f;

	/* search in positive direction */
	for(ip = istart; ip < n-1; ip++) {
		yi_diff = ys - y[ip+1];
		if((yi_diff * ys_diff) < eps) return(ip);
	}
	return(-1);
}

int CTemperatureField::SearchFirstNegative(
	const int   istart,
	const float *y,	/* FILE */
	const float ys)
{
	int ip;
	float ys_diff = ys - y[istart+1];
	float yi_diff;
	float eps = .1e-23f;
	if(istart == 0) return(0);
	/* search in positive direction */
	for(ip = istart ; ip > -1; ip--) {
		yi_diff = ys - y[ip];
		if((yi_diff * ys_diff) < eps) return(ip);
	}
	return(-1);
}

int CTemperatureField::intervall_no( const CURVE *cx,
	const float absz)
{
	int iz = 0;
	if(absz > cx->x[cx->n-1]) return(-2);
	if(absz < cx->x[0]) 		  return(-1);
	while(++iz < cx->n)	{
		if(absz < cx->x[iz]) return(iz-1);
	}
	return(cx->n-2);
}

int	CTemperatureField::ausg_curve(CURVE *x,
	char *name,
	int  icall)
{
	FILE *ay;
	char *attr[] = {"w", "a"};
	int	iz = -1;
	if(icall == 0) remove(name);

	if ((ay = fopen(name,attr[icall]))==NULL)   //谢谦 2012-7-6 增加保护
		return 1;

	if(icall == 1) 
		fprintf(ay,"\n\n\t");
	fprintf(ay," curve id : %g   n: %d iabl: %d", x->c_id, x->n, x->iabl);
	fprintf(ay,"\n\t x_min %g   y_min %g    x_max %g  y_max %g",
		x->x_min, x->y_min, x->x_max, x->y_max);
	while(++iz < x->n) {
		fprintf(ay,"\n\t %5d %10g %10g", iz, x->x[iz], x->y[iz]);
		if(x->iabl > 0 ) fprintf(ay," %10g", x->yp1[iz]);
		if(x->iabl > 1 ) fprintf(ay," %10g", x->yp2[iz]);
	}
	fclose(ay);
	return(1);
}


/* 1.2.	calculate for individual 横坐标*/

int	CTemperatureField::spline_1	(	const	CURVE *cx,	/*	curve value */
	const	int	ix,		/*	value +/- ix are incorp. */
	const	float	x,			/*	absz. for interpolation */
	float	*y)		/*	插value value y[0] */
	/* first 偏差	  y[1] */
{
	spline_t	   spl;

	if(x < cx->x[0]) {
		y[0] = cx->y[0];
		y[1] = 0.0f;
		y[2] = 0.0f;
		return(-1);
	}
	if(x > cx->x[cx->n-1]) {
		y[0] = cx->y[cx->n-1];
		y[1] = 0.0f;
		y[2] = 0.0f;
		return(1);
	}

	spl = InitSpline( cx->n, ix, 0);
	spl.i = EstimateStartIndex(x,cx->n,cx->x);  /*	estimate start index */
	coef_spline(cx,&spl);
	y[0] = CalculateSplineValue(&spl,cx->x,x);
	y[1] = CalculateFirstDerivative(&spl,cx->x,x);
	y[2] = CalculateSecondDerivative(&spl,cx->x,x);

	return TRUE;
}

int	CTemperatureField::spline_2	(	const	CURVE *cx,	/*	curve value */
	const	int	ix,		/*	value +/- ix are incorp. */
	const	int	iend,		/*	end for interpolation */
	float	*y,		/*	插value value  */
	float *yp1,		/* first 偏差	   */
	float *yp2)		/* sec 偏差	   */

{
	spline_t  spl;
	int iz;
	float absz;
	float eps = .1e-12f;

	spl = InitSpline( cx->n, ix, 0);

	iz = -1;
	while(++iz <= iend) {
		if(((float)iz) > (cx->x[0]-eps)) break;
		y[iz] = cx->y[0];
		yp1[iz] = 0.0f;
		yp2[iz] = 0.0f;
	}

	if(iz > -1) iz--;
	while(++iz <= iend)	{
		absz = (float)iz;
		if(absz > (cx->x[cx->n-1]+eps)) break;
		spl.i = EstimateStartIndex(absz,cx->n,cx->x);  /*	estimate start index */
		coef_spline(cx,&spl);
		y[iz] = CalculateSplineValue(&spl,cx->x,absz);
		yp1[iz] = CalculateFirstDerivative(&spl,cx->x,absz);
		yp2[iz] = CalculateSecondDerivative(&spl,cx->x,absz);
	}

	if(iz > -1) iz--;
	while(++iz <= iend) 
	{
		y[iz] = cx->y[cx->n-1];
		yp1[iz] = 0.0f;
		yp2[iz] = 0.0f;
	}
	return(1);
}

/*		2.		modules of next order */
spline_t	  CTemperatureField::InitSpline(	const	int	n,	/* number of function value */
	const	int	i,	/*	required order of spline */
	/* +/- i absz. value */
	const	int	izw)
{
	/*	initialization of spline */
	int		imax = n/2;
	spline_t	  xx;

	xx.i = 1;
	xx.n = n;
	xx.a = 0.0f;
	xx.b = 0.0f;
	xx.c = 0.0f;
	xx.d = 0.0f;
	xx.ix = i;
	if(i > imax)	
		xx.ix = imax;
	xx.izw = izw;
	if(n <  2)					xx.ix = 0;
	if((n == 2)||(n == 3))	xx.ix = 1;
	xx.ir 	=	2 * xx.ix - 1;			/*	2 bis N-1  xx.ix */
	xx.imat	=	3 * (xx.ir-1) +1;		/* 3 * Vektorl鋘ge  3 Diagonalen */
	xx.ibw = 2;
	memset(xx.mat,0,sizeof(xx.mat));
	memset(xx.rb,0,sizeof(xx.rb));
	memset(xx.y2,0,sizeof(xx.y2));
	return(xx);
}

float	CTemperatureField::CalculateSplineValue(	const	spline_t	*spl,	/*	pointer to spline */
	const	float		*xx,	/*	横坐标value */
	const	float		x)		/*	横坐标value */
{
	/*	calculate spline value */
	float	px;
	float	dx = x - xx[spl->i];
	px 	= spl->a + dx*(spl->b + dx*(spl->c+dx*spl->d));
	return(px);
}

float	CTemperatureField::CalculateFirstDerivative(	const	spline_t	*spl,	/*	pointer to spline */
	const	float		*xx,	/*	横坐标value */
	const	float		x)		/*	横坐标value */
{
	/*	calculate 一阶导数 of spline  */
	float px1;
	float	dx = x - xx[spl->i];
	px1	= spl->b + (2.f* spl->c + 3.f* spl->d *dx)*dx;
	return(px1);
}

float	CTemperatureField::CalculateSecondDerivative(	const	spline_t	*spl,	/*	pointer to spline */
	const	float		*xx,	/*	横坐标value */
	const	float		x)		/*	横坐标value */
{
	/*	calculate 二阶导数 of spline  */
	float px2;
	float	dx = x - xx[spl->i];
	px2	= 2.f* spl->c + 6.f* spl->d *dx;
	return(px2);
}

int	CTemperatureField::EstimateStartIndex(	const	float	x,
	const	int	imax,		/*	maximum index */
	const	float	*xx)		/*	横坐标value */
{
	/*	estimate index i with xx[i] < x &&	xx[i+1] > x */
	/* lowest index: 0 highest index: n-2 */
	int	iz = -1;
	if(xx[0]>x)	return(0);
	while(++iz < imax)		if(xx[iz]>x)	return(iz-1);
	return(imax-2);
}

int	CTemperatureField::coef_spline(	const	CURVE *cx,	/*	curve     */
	spline_t *spl)	/*	spline    */
{
	int	iret	=	TRUE;
	int	n		=	spl->n;
	float	hi;
	float	fac;
	float	eps = .1e-08f;
	int	iex = spl->i;
	int	ix;		/*	Index 1. Wertepaar f den Spline	*/
	int	iex0, iex1;
	float	y2;  		/*	left value sec. dev (on y[i])			*/
	float y2p1;		/*	right value sec. dev (on y[i+1])		*/


	spl->b = 0.0f;
	spl->c = 0.0f;
	spl->d = 0.0f;
	spl->a = cx->y[iex];		/*	constant interpolation */
	if(spl->ix == 0)	return FALSE;
	iex0	=	iex;
	iex1	=	iex+1;

	hi =	cx->x[iex1]-cx->x[iex0];
	if(fabs(hi) < eps)	spl->b = .5f*(cx->y[iex1]+cx->y[iex0]);
	else						spl->b = (cx->y[iex1]-cx->y[iex0])/hi; /* linear */
	if(spl->ix == 1)	return TRUE;
	ix	  =	IntervalIndexStart(spl);	/*	ersten Index f黵 Spline ermitteln */
	iret = CalculateMat( &(cx->x[ix]), &(cx->y[ix]), spl);

	if(spl->izw )	{
		outpx(spl->mat,   spl->imat-1, "Matrix f splines");
		outpx(spl->rb,    spl->ir-1, "Vektor f splines");
	}
	if(iret < 1)	fehler_id(0,"error in CalculateMat()");
	iret = vealb(spl->mat, 1.e18f, spl->ir-1 , spl->ibw);
	if(spl->izw )	{
		outpx(spl->mat,   spl->imat-1, "Matrix nach VEALB");
	}
	if(iret < 1)	fehler_id(0,"error in vealb()");
	iret = voruva(spl->mat, spl->rb, &(spl->y2[ix]),
		spl->ibw, spl->ir-1,1 , spl->ir-1);
	if(spl->izw )	{
		outpx(&(spl->y2[ix]), spl->ir-1, "Y2");
	}
	if(iret < 1)	fehler_id(0,"error in voruva()");
	/*	second 偏差 at boundaries */
	if(spl->i == 0)				{
		fac=(cx->x[2]-cx->x[1])/(cx->x[1]-cx->x[0]);
		spl->y2[0] = spl->y2[1]+ fac*(spl->y2[1]-spl->y2[2]);
	}
	if(spl->i == spl->n - 1)	{
		fac=(cx->x[n-1]-cx->x[n-2])/(cx->x[n-2]-cx->x[n-3]);
		spl->y2[n-1] = spl->y2[n-2]+ fac*(spl->y2[n-2]-spl->y2[n-3]);
	}
	y2			=spl->y2[iex0];	/*	left value sec. 偏差 		*/
	y2p1  	=spl->y2[iex1];   /*	right value sec. 偏差 	*/
	spl->c   =0.5f*y2;
	spl->d	=(y2p1-y2)/(6.f*hi);
	spl->b	=(cx->y[iex1]-cx->y[iex0])/hi-hi*(y2p1+y2*2.f)/6.f;
	return(iret);
}

int	CTemperatureField::CalculateMat(	const float		*xi,
	const	float		*yi,
	spline_t	*spl)
{
	int	ir			=	0;				/*	Index of column */
	int	imat		=	0;		    	/*	Index of matrix */
	int	n			=	spl->ir;		/* bis N-1 interpolieren 	*/
	int	i			=  0;				/* ab 2 interpolieren		*/
	float	hi, him1, h1, h12;

	hi 		=	xi[2]  -	xi[1];
	him1		=	xi[1]	 -	xi[0];
	h1			=	him1;
	h12		=	h1/hi;

	/* set initial value for solver */
	spl->rb[0] = 1.f;  /* one right side in matrix */
	spl->mat[0] =3.f*(float)(spl->ir-1);

	while(++i < n)	{
		him1					=	hi; 	/*	xi[i]  -xi[i-1]; */
		hi						=	xi[i+1]-xi[i];
		ir++;
		spl->rb[ir]		=	6.f*((yi[i+1]-yi[i])/hi - (yi[i]-yi[i-1])/him1);
		imat++;
		spl->mat[imat]	=	him1;
		imat++;
		spl->mat[imat]	=	2.f*(him1+hi);
		imat++;
		spl->mat[imat]	=	hi;
	}
	/*	first row */
	spl->mat[1]			= 0.0f;
	/* will be modified only for "not at knot"   */
	spl->mat[2]			+= h1*(1.f+h12);
	spl->mat[3]    	-= h1;
	/*	last row */
	spl->mat[imat]		= 0.0f;
	/* will be modified only for "not at knot" */
	--imat;
	spl->mat[imat]	+=	hi*(1.f+hi/him1);
	--imat;
	spl->mat[imat]	-= hi*hi/him1;
	return TRUE;
}


int	CTemperatureField::IntervalIndexStart(const	spline_t	*spl)
{
	int	iende		=  MIN(spl->i + spl->ix, spl->n-1);
	int	istart   =	MAX(spl->i - spl->ix, 0);
	if(istart == 0)			iende 	= 2 * spl->ix;
	if(iende  == spl->n) 	istart 	= iende - 2*spl->ix;
	return(istart);
}

int CTemperatureField::vealb(				float	*a,
	const	float	elmax,
	const	int	n,
	const	int	ibw)
{
	const	float	elmin= 1.f/elmax;
	const	int 	ib=ibw*2-1;
	const	int	ib1 = ib-1;
	const	int 	ibw1=ibw-1;
	const	int	irue = ibw1*ib;
	int			i,ii,iii = 1,ient,ij,ij0,iu,ji,ji0,jj, kk;
	float			ac1, s, sel, sel1, sel2, sel3;
	if(ib == 1)	{
		ac1=a[1];
		if(fabs(ac1) > elmin) return TRUE;
	}
	while(++iii <= n) 	{						/* 	Loop 1 */
		i=iii*ib-ibw1;
		ii=i-ibw1;
		jj=i-irue;
		if(iii < ibw)	{								/*		Test ibw */
			ii=i-iii+1 ;
			jj=ibw;
		}													/*		Test ibw */
		sel=a[ii]*elmin;
		sel1=a[jj];
		if(fabs(sel) > fabs(sel1)) return FALSE;
		a[ii] /= sel1;
		ij0=i-1;
		iu=ii+1;
		if(iii != 2)						{			/*		Test ag. 2 */
			ij = iu-1; while(++ij <= ij0)	{        /*		Loop 2 */
				jj++;
				kk=jj ;
				ji0=ij-1;
				s=a[ij];
				ji = ii-1; while(++ji <= ji0)	{			/*		Loop 3 */
					s -=a[ji]*a[kk];
					kk +=ib1;
				}													/*		Loop 3 */
				sel2=s*elmin;
				sel3=a[kk];
				if(fabs(sel2) > fabs(sel3)) return FALSE;
				a[ij]=s/sel3;
			}              								/*		Loop 2  */
		}													/*		Test ag. 2 */
		ji0=i+ibw-2;
		jj++;
		iu=1 ;
		if(jj > ib) iu=ib;
		ij = i-1; while(++ij <= ji0)		{		/*		Loop 2 */
			kk=ii;
			s=a[ij];
			for(ji=jj; ji <= ij0; ji +=ib1)	{  	/*		Loop 3 */
				s -= a[ji]*a[kk];
				kk++;
			}													/*		Loop 3 */
			a[ij]=s;
			ient	= jj - ib;
			if(ient == 0)		{
				iu = ib;
				ii++;
				jj+=iu;
			}
			if(ient >  0)  	{
				ii++;
				jj+=iu;
			}
			if(ient <  0)		jj +=iu;
		}													/*		Loop 2 */
	}                                      /*		Loop 1 */
	return TRUE;
}

int	CTemperatureField::voruva(		const	float	*a,
	const	float	*b,
	float	*x,
	const	int	ibw,
	const	int	m,
	const	int	n,
	const	int	mb)
{
	/*
	vorwaerts und rueckwaertseinsetzen fuer verketteten algorithmus;
	mit bandmatrix [nur fuer vealb verwenden];
	*/
	const	int	ib=ibw*2-1;
	const	int	mbn=mb*n;
	int			idia=ibw+ib;
	const	int	ibw1 = ibw -1;
	int			i,ii,ia, iaa, iug, j, jaa;
	float			s;

	if(ibw == 1) 	{								/*	Test ibw == 1 */
		i = 0; while(++i <=m)		{
			for(j=i; j <= mbn; j+=mb)	{
				x[j]=b[j]/a[i];
			}
		}
		return TRUE;
	}													/*	Test ibw == 1 */

	for(i=1; i <= mbn; i+=mb)	{				/*		Loop 1  A */
		x[i]=b[i];
	}                            				/*		Loop 1  E */
	i = 1; while(++i <= m)		{				/*		Loop x1 */
		ia=i-ibw1;
		if(ia < 1) ia=1;
		iaa=idia-i+ia;
		iug=i-1;
		for(ii=iug; ii <= mbn; ii+=mb)	{		/*		Loop 2 */
			jaa=iaa;
			s=b[ii+1];
			j = ia-1; while(++j <= ii)		{			/*		Loop 3 */
				s -= a[jaa]*x[j];
				jaa++;
			}													/*		Loop 3 */
			x[ii+1]=s;
			ia   +=mb;
		}													/*		Loop 2 */
		idia +=ib;
	}													/*		Loop x1 */
	idia -=ib;
	s= 1.f/a[idia];
	for(i=m; i <= mbn; i+=mb)	{				/*		Loop 1 */
		x[i] *=s;
	}													/*		Loop 1 */
	i=0; while(++i < m)			{				/*		Loop y1 */
		idia -=ib;
		ia=m-i+1;
		iug=ia+ibw1-1;
		if(i < ibw) iug=m;
		for(ii=iug; ii <= mbn; ii+=mb)	{		/*		Loop 2 */
			jaa=idia+1;
			s=x[ia-1];
			j=ia-1; while(++j <= ii)			{		/*		Loop 3 */
				s -= a[jaa]*x[j];
				jaa++;
			}													/*		Loop 3 */
			x[ia-1]=s/a[idia];
			ia +=mb;
		}													/*		Loop 2 */
	}													/*		Loop y1 */
	return TRUE;
}



void	CTemperatureField::fehler_id(      int	igroup,		/* Fehlergruppe */
	const	char	*melde)	/*	Botschaft */
{
	time_t	zeitx;
	FILE		*st_err;
	char		*group[]	={ "open one FILE",    		/*	0 */
		"memory allocation",	
		"numerical error",
		"unknown reason"
	};
	int		f_max = 3;	/*	maximum number of error */

	time(&zeitx);

	if((st_err = fopen("st_err.dat","a")) == NULL)	{
		printf("\n\t *** not able to open ST_ERR.DAT ***");
		return ;
		//exit(1);
	}

	if(igroup > f_max)	igroup = f_max;

	printf("\n\t %s \t error group %3d : %s",
		ctime(&zeitx), igroup, group[igroup]);
	printf("\n\t %s \n\n", melde);
	fprintf(st_err,"\n\t %s \t error group %3d : %s",
		ctime(&zeitx), igroup, group[igroup]);
	fprintf(st_err,"\n\t %s \n\n", melde);
	fclose(st_err);
	return ;//exit(1);	/*		Fehlerabbruchkriterium sp鋞er modifizierbar */
}

void CTemperatureField::alpha_calc_ne( FILE       *st_err,
	ALPHAVALUES *alpha,
	const				  MATERIALPARAM *ccv,
	const				  PHYSICAL *xy)
{
	/*	read alpha value for no ferrous material: 	*/
	int	ianz = 6;
	float	al_top[15];
	float al_bot[15];
	int ixx = 0;
	int	maxi = xy->maxPossibleMillTemp;

	alpha_ne_wat(0,al_top,ccv,xy);
	alpha_ne_wat(1,al_bot,ccv,xy);
	SetAlphaLimits( xy, ianz,al_bot);
	SetAlphaLimits( xy, ianz,al_top);
	CalculateLinearInterpolation(alpha->bot, al_bot, ianz, maxi);
	CalculateLinearInterpolation(alpha->top, al_top, ianz, maxi);

	return;
}


int CTemperatureField::alpha_ne_wat( int        seite,
	float     *val,
	const			  MATERIALPARAM *ccv,
	const			  PHYSICAL *xy)
{
	float	temp;
	float	al_max;
	float	flow;
	float	t_le;	/* Leidenfrost point */
	float t_bo; /* BurnoutTemperature */
	float	air_anl = .7f;
	int	iz;

	if(seite == 0) flow = xy->topFlowRate;
	else				flow = xy->bottomFlowRate;
	al_max = 1530.f * (float)pow(flow,static_cast<float>(0.48)); /* /1/ S. 44 */
	val[0] = -20.f;
	val[1] = al_max;
	t_le  = 190.f*(float)pow(flow, static_cast<float>(0.09));   /* /1/ S. 37 */
	t_bo  = 83.f *(float)pow(flow, static_cast<float>(0.11));   /* /1/ S. 39 */
	val[2] = t_bo;
	val[3] = al_max;
	val[4] = t_le;
	val[5] = 1.82f * flow+198.f;
	val[6] = (float)(xy->maxPossibleMillTemp);
	val[7] = val[5];
	for(iz = 1; iz < 8; iz +=2)	{
		temp = val[iz-1];
		if(temp < 100.f) temp = 20.f;
		val[iz] += air_anl * alphal( seite, xy->speed, temp, xy->t_umg, 0.68f);
	}
	return(1);
}

void CTemperatureField::outpx( float *x, int n, char *str)
{
	int iz,ixx,ixy;
	printf("\n");
	for(iz = 0; iz <80;iz++)
		printf("_");
	printf("\n\t __________%s____________ \n", str);
	ixy = -1;
	for(;;){

		if(ixy > n)
			break;
		ixx = ixy+1;
		ixy +=4;
		if(ixy > n)
			break;
		printf("\n Element %d bis %d:", ixx, ixy);
		for(iz = ixx; iz <= ixy;iz++)
			printf(" %g", x[iz]);
	}
	if(ixx <= n)
	{
		printf("\n Element %d bis %d:", ixx, n);
		for(iz = ixx; iz <= n;iz++)
			printf(" %g", x[iz]);
	}
	printf("\n");
	for(iz = 0; iz <80;iz++)
		printf("_");
}


void   CTemperatureField::r_cp_file( const	INPUTDATAPDI *pdi, const int taskID,  char *file_name)
{
	int     index           =       0;
	char    *xx_datei       =       {"rcp0"};
	char    *ende_txt       =       {".dat"};
	char    datstart[20];

	int     ianz    = 17;           /*      no. of files = dimension of para */

	float   para[]  =       {				  23.f,    .298f,   .09f,    1.25f,
		26.f,    .2467f,  .13f,    .7f,
		36.f,    .37f,    .12f,    1.5f,
		45.f,    .4217f,  .18f,    1.45f,
		51.f,    .337f,   .12f,    1.3f,
		54.f,    .375f,   .15f,    1.35f,
		58.f,    .4116f,  .17f,    1.45f,
		59.f,    .325f,   .1f,     1.35f,
		62.f,    .29f,    .09f,    1.2f,
		63.f,    .2633f,  .08f,    1.1f,
		65.f,    .3333f,  .1f,     1.4f,
		68.f,    .283f,   .1f,     1.1f,
		76.f,    .3333f,  .1f,     1.4f,
		85.f,    .358f,   .1f,     1.55f,
		87.f,    .295f,   .1f,     1.35f,
		92.f,    .075f,   .07f,    .3f,
		97.f,    .35f,    .1f,     1.5f};

	char  *str_para[]       =       { "23" ,"26", "36", "45", "51", "54", "58",
		"59", "62", "63", "65", "68", "76", "85",
		"87", "92", "97"};

	float   x_min   =       12000.f;
	float   ceq     =       pdi->alloy_c + pdi->alloy_mn * .1666667f;
	float   norm;
	int     iz      = -1;
	int     ixx     = -4;

	while(++iz < ianz)      {
		ixx += 4;
		norm    = (float)fabs(para[ixx+1] - ceq);              /* 偏差 ceq */
		norm   += (float)fabs(para[ixx+2] - pdi->alloy_c);     /* 偏差 C   */
		norm   += (float)fabs(para[ixx+3] - pdi->alloy_mn);    /* 偏差 Mn  */
		if( norm < x_min) {
			index   = iz;
			x_min   = norm;
		}
	}

	strcpy_s(datstart, xx_datei);
	strcat(datstart, str_para[index]);
	strcat(datstart, ende_txt);
	CombinFilePath(datstart, taskID, file_name); 
	/*	printf("\n\t estimated file_name: %s", file_name);	*/
}



int CTemperatureField::TimeEnd( float timex, float time_mean, COMPUTATIONAL *compx)
{
	int	ient = 0;
	if(compx->calculationStopTime > .001 && timex > compx->calculationStopTime) return(ient);
	/* integration time */
	ient = 1;
	return(ient);
}

int CTemperatureField::TemperatureEnd( float timex, float time_mean, COMPUTATIONAL *compx)
{
	int	ient = 0;

	if(compx->calculationStopTemp >  .001 && time_mean < compx->calculationStopTemp) return(ient);
	/* stop Temperature */

	ient = 1;
	return(ient);
}

int CTemperatureField::TimeSolver(	FILE 			*st_err,
	FILE 		*erg_f ,
	const		INPUTDATAPDI *pdi,
	COMPUTATIONAL *compx,
	const		PHYSICAL *ax,
	const		MATERIAL 	*matal)
{
	/* 	calculation of Temperature distribution        			*/

	FILE	*erg_graph = NULL;
	int   iz;
	int 	ixx = 0;    /* error parameter for memory reservation		*/
	int 	imatrix	= compx->matrixElementNumber;	/*	no. of matrix koeff.  	*/
	int		icolumn = compx->thickNodeNumber;		/*	no. of thickNodeNumber	      	*/
	int		iCenter = icolumn / 2;
	char	fx_name[255];
	char	*name_ex[2] = { "w", "a"};
	int	i_nx = 1;
	/*	output-FILE for calculation results	*/
	float	dt_zero = compx->detaTime;		/*	time increment	      	*/
	float	detaTime;			/*	actuel time step			*/
	float	dt_min = 0.0001f; /* lowest allowed time step      */
	float	timex; 		/*	time						*/
	float	stiff[100];		/*	modified system stiffness matrix	*/
	float	rr[100];		/*	upper matrix				*/
	float	yy[50];		/*	column for cholesky			*/
	float	force[50];		/*	system force column			*/
	float	temp_k[50];	/*	Temperature column at 实际 time step	*/
	float	temp_km1[50];	/*	Temperature column at previous step	*/
	//	float   temp_m[50];	/*	middle Temperature in predictor step	*/
	float   xx[15];		/*	value for output control			*/
	float   d_temp;		/*	output if 表面Temperature diiffers	*/
	/*	more than d_temp from previous output	*/
	float	temp_kph[50];	/*	Temperature (t(k-1) + t(k))/2		*/
	float	temp_pred[50];	/*	predictor Temperature			*/
	float	xgrad = 0.0f;
	int	izaehler = 0;   /*	index for time integration		*/
	int	i_it;		/*	number of iteration			*/
	int	anz_it = 10;	/*	maximum number of iteration cycles	*/
	int anz_dt = 1000;  /* 最大 allowed No. time increments */
	float	residual;	/*	estimated error				*/
	float	epsilon = .1f;	/*	accepted error in calculation = .1 K	*/
	float   calculationStopTime = compx->calculationStopTime;
	int	istop = 0;
	int	i_output = compx->femCalculationOutput;
	static	float	run_time;

	if(ax->coolPhaseNumber == 0)	{
		run_time = 0.0f;		/* set run time to zero 	*/  // 谢谦 实验性屏蔽 空冷时 time不置0
		i_nx = 0;				/* FILE extension = "w"		*/
	}

	xx[10] = run_time;
	compx->calculationTotalTime = run_time;	/* store 实际 time */

	strcpy_s(fx_name, pdi->plateID);
	strcat(fx_name, "_x.dat");

	if(i_output)  
	{
		if(i_nx == 0) 
			remove(fx_name);
		if((erg_graph = fopen(fx_name,name_ex[i_nx])) == NULL)	
		{
			fprintf(st_err,"\n\t ** not able to open: %s ***", fx_name);
			return -1;
			//exit(1);
		}
	}

	if(i_output > 1) 
		OutputComputational(erg_f, compx);

	iz = -1; 
	while(++iz < 15) 
		xx[iz] = 0.0f;
	/*	time integration algorithem		*/

	/*	start 参数 for Temperature distribution	*/
	d_temp = InitTemperatureDistribution(compx, matal,temp_k, temp_km1);

	//d_temp = 1;  //谢谦 增大solver 输出的频率 只要一个time步内温差大于1 就输出
	if(pdi->taskID == PRE_CALC_SIM) 
		d_temp = 5.f;

	/* 纠正 initial distribution if temp < Ar3 */

	if(ax->coolPhaseNumber == 0) 
	{
		/* calculate start distr. x */
		SystemMatrices(stiff, force, compx, temp_k, temp_k, 0.0f, ax, matal);
	}

	compx->detaTime = dt_zero;    //初始的detaTime

	xx[1] = CalculationMeanTemperature( compx, compx->startTempDistribution);  //计算出平均Temperature 并计算出ROCP等value

	if(ixx > 0) {
		fprintf(st_err,"\n\t *** not enough memory for allocation in: solver *** ");
		if(erg_graph != NULL) 
			fclose(erg_graph);
		return(1);
	}

	xx[2]	= xx[1];
	xx[3]	= temp_k[0];
	xx[4]	= xx[3];
	xx[5]	= xx[3];
	xx[11]	= temp_k[iCenter];
	xx[12] = xx[11];

	//xx[0]=0.01;     // 谢谦加入 区别用 2012-5-4 不适合添加
	OutputFunc(i_output,erg_f, erg_graph, xx, compx, temp_k,0);

	timex = 0.0f;
	if(compx->testParameter == 2)
		dt_zero *= .1f;
	detaTime    = dt_zero;
	iz	= 1;

	while(iz == 1) {	/*     loop over time increments	*/
		izaehler++;

		if(compx->testParameter == 1)
			// 一般空冷时初始条件 会令itest = 1 谢谦 现有问题是 此函数返回的冷却time过长 在一组集管的长度内 不符合精度
			detaTime = 0.5f*EstimationTemperatureDifference(detaTime, dt_zero, temp_km1, temp_k, izaehler, icolumn - 1);       //谢谦进行修改 减小time间隔

		if( calculationStopTime > .001)  {
			if((timex + detaTime) > calculationStopTime) {
				detaTime = calculationStopTime - timex;
				istop = 1;
			}
		}

		timex += detaTime;
		xx[0]  = timex;
		xx[10] +=	detaTime;
		xx[6]  = xx[2];

		/*	initiate for iteration cycle				*/

		SetColumn(temp_kph, temp_km1, icolumn);
		SetColumn(temp_pred, temp_km1,icolumn);
		SetColumn(compx->TimeKpredAusteniteDistribution, compx->TimeKAusteniteDistribution,icolumn); 
		SetColumn(compx->TimeKpredRhoCp, compx->TimeKRhoCp,icolumn);

		i_it= -1;
		while(++i_it < anz_it)	{	/*	Iteration		*/
			/* system matrices					*/
			SystemMatrices(stiff, force, compx, temp_km1, temp_kph, detaTime, ax, matal);
			/*	solution of the linear algebraic system		*/
			SolveAlgebraicMatrix(stiff, rr, yy, force, temp_k, icolumn);
			residual = Norm2FindMax(temp_pred,  temp_k, icolumn);
			if(residual < epsilon)	
				break;
			ColCrNi1(temp_kph, temp_km1, temp_k, icolumn);
			SetColumn(temp_pred, temp_k, icolumn);
		} 	/*	Iteration		*/

		compx->calculationTotalTime += detaTime;
		compx->detaTime = detaTime;
		SetColumn(compx->TimeKRhoCp, compx->TimeKpredRhoCp, icolumn);
		SetColumn(compx->TimeKm1AusteniteDistribution, compx->TimeKAusteniteDistribution, icolumn); 
		SetColumn(compx->TimeKAusteniteDistribution, compx->TimeKpredAusteniteDistribution, icolumn);
		/*	caloric mean tenperature			*/
		xx[7]	 = xx[2];
		xx[2]  = CalculationMeanTemperature( compx, temp_k);

		if(detaTime < dt_min)	
			xx[7] = 0.0f;
		else			
			xx[7]  = (xx[7] - xx[2])/detaTime;		/* local cooling rate	*/

		/*	output of calculated data			*/
		xx[4]	= temp_k[0];
		xx[12]	= temp_k[iCenter];
		/* output required, if surface temp.diff. exceeds limit d_temp */
		/* or Temperature in center line exeeds d_temp */
		if((	fabs(xx[4] - xx[3]) > d_temp)  || 
			fabs(xx[12] - xx[11]) > d_temp)	{  
				OutputFunc(i_output,erg_f, erg_graph, xx, compx, temp_k, i_it);
		}
		iz	=  TimeEnd(timex, xx[2], compx);
		if((istop == 1)||(iz!=1)) 
			break;
		if(izaehler > anz_dt) 
			break;
		SetColumn(temp_km1, temp_k, icolumn);

	}	/*	time steps		*/

	compx->localCoolRate = xx[7]; /* local 冷却速度*/
	run_time = xx[10];
	SetColumn(compx->startTempDistribution, temp_k, icolumn);
	SetColumn(compx->startAusteniteDistribution, compx->TimeKAusteniteDistribution, icolumn);

	xx[2]  = CalculationMeanTemperature( compx, temp_k);
	compx->calculationTotalTime       = run_time;

	OutputFunc(i_output, erg_f, erg_graph, xx, compx, temp_k, i_it);
	if(i_output) 
		fprintf(erg_f,"\n\t --- stop: %d steps %g seconds ---",izaehler, xx[0]);

	/* energy balance */
	if(i_output) 
		EnergyReport( erg_f, pdi, compx, xx);

	if(i_output)  
		fclose(erg_graph);
	return(ixx);
}

int CTemperatureField::Temperatureolver(	FILE 			*st_err,
	FILE 		*erg_f ,
	const		INPUTDATAPDI *pdi,
	COMPUTATIONAL *compx,
	const		PHYSICAL *ax,
	const		MATERIAL 	*matal)
{
	/* 	calculation of Temperature distribution        			*/

	FILE	*erg_graph = NULL;
	int   iz;
	int 	ixx = 0;    /* error parameter for memory reservation		*/
	int 	imatrix	= compx->matrixElementNumber;	/*	no. of matrix koeff.  	*/
	int		icolumn = compx->thickNodeNumber;		/*	no. of thickNodeNumber	      	*/
	int		iCenter = icolumn / 2;
	char	fx_name[255];
	char	*name_ex[2] = { "w", "a"};
	int	i_nx = 1;

	/*	output-FILE for calculation results	*/
	float	dt_zero = compx->detaTime;		/*	time increment	      	*/
	float	detaTime;			/*	actuel time step			*/
	float	dt_min = 0.0001f; /* lowest allowed time step      */
	float	timex; 		/*	time						*/
	float	stiff[100];		/*	modified system stiffness matrix	*/
	float	rr[100];		/*	upper matrix				*/
	float	yy[50];		/*	column for cholesky			*/
	float	force[50];		/*	system force column			*/
	float	temp_k[50];	/*	Temperature column at 实际 time step	*/
	float	temp_km1[50];	/*	Temperature column at previous step	*/
	//	float   temp_m[50];	/*	middle Temperature in predictor step	*/
	float   xx[15];		/*	value for output control			*/
	float   d_temp;		/*	output if 表面Temperature diiffers	*/
	/*	more than d_temp from previous output	*/
	float	temp_kph[50];	/*	Temperature (t(k-1) + t(k))/2		*/
	float	temp_pred[50];	/*	predictor Temperature			*/
	float	xgrad = 0.0f;
	int	izaehler = 0;   /*	index for time integration		*/
	int	i_it;		/*	number of iteration			*/
	int	anz_it = 10;	/*	maximum number of iteration cycles	*/
	int anz_dt = 1000;  /* 最大 allowed No. time increments */
	float	residual;	/*	estimated error				*/
	float	epsilon = .1f;	/*	accepted error in calculation = .1 K	*/
	float   calculationStopTime = compx->calculationStopTime;
	int	istop = 0;
	int	i_output = compx->femCalculationOutput;
	static	float	run_time;

	if(ax->coolPhaseNumber == 0)	{
		run_time = 0.0f;		/* set run time to zero 	*/
		i_nx = 0;				/* FILE extension = "w"		*/
	}

	xx[10] = run_time;
	compx->calculationTotalTime = run_time;	/* store 实际 time */

	strcpy_s(fx_name, pdi->plateID);
	strcat(fx_name, "_x.dat");

	if(i_output)  {
		if(i_nx == 0) 
			remove(fx_name);
		if((erg_graph = fopen(fx_name,name_ex[i_nx])) == NULL)	{

			fprintf(st_err,"\n\t ** not able to open: %s ***", fx_name);
			return -1;
		}
	}
	if(i_output > 1) 
		OutputComputational(erg_f, compx);

	iz = -1; 
	while(++iz < 15) 
		xx[iz] = 0.0f;
	/*	time integration algorithem		*/

	/*	start 参数 for Temperature distribution	*/

	d_temp = InitTemperatureDistribution(compx, matal,temp_k, temp_km1);
	if(pdi->taskID == PRE_CALC_SIM) 
		d_temp = 5.f;

	/* 纠正 initial distribution if temp < Ar3 */

	if(ax->coolPhaseNumber == 0) {
		SystemMatrices(stiff, force, compx, temp_k, temp_k, 0.0f, ax, matal);
	}
	compx->detaTime = dt_zero;
	xx[1] = CalculationMeanTemperature( compx, compx->startTempDistribution);

	if(ixx > 0) {
		fprintf(st_err,"\n\t *** not enough memory for allocation in: solver *** ");
		if(erg_graph != NULL) 
			fclose(erg_graph);
		return(1);
	}

	xx[2]	= xx[1];
	xx[3]	= temp_k[0];
	xx[4]	= xx[3];
	xx[5]	= xx[3];
	xx[11]	= temp_k[iCenter];
	xx[12] = xx[11];

	OutputFunc(i_output,erg_f, erg_graph, xx, compx, temp_k,0);
	timex = 0.0f;
	if(compx->testParameter == 2)
		dt_zero *= .1f;
	detaTime    = dt_zero;
	iz	= 1;

	while(iz == 1) {	/*     loop over time increments	*/
		izaehler++;

		if(compx->testParameter == 1)
			detaTime = EstimationTemperatureDifference(detaTime, dt_zero, temp_km1, temp_k, izaehler, icolumn - 1);

		timex += detaTime;
		xx[0]  = timex;
		xx[10] +=	detaTime;
		xx[6]  = xx[2];

		/*	initiate for iteration cycle				*/

		SetColumn(temp_kph, temp_km1, icolumn);
		SetColumn(temp_pred, temp_km1,icolumn);
		SetColumn(compx->TimeKpredAusteniteDistribution, compx->TimeKAusteniteDistribution,icolumn); 
		SetColumn(compx->TimeKpredRhoCp, compx->TimeKRhoCp,icolumn);

		i_it= -1;
		while(++i_it < anz_it)	{	/*	Iteration		*/
			/* system matrices					*/
			SystemMatrices(stiff, force, compx, temp_km1, temp_kph, detaTime, ax, matal);
			/*	solution of the linear algebraic system		*/
			SolveAlgebraicMatrix(stiff, rr, yy, force, temp_k, icolumn);
			residual = Norm2FindMax(temp_pred,  temp_k, icolumn);
			if(residual < epsilon)	
				break;
			ColCrNi1(temp_kph, temp_km1, temp_k, icolumn);
			SetColumn(temp_pred, temp_k, icolumn);
		} 	/*	Iteration		*/

		compx->calculationTotalTime += detaTime;
		compx->detaTime = detaTime;
		SetColumn(compx->TimeKRhoCp, compx->TimeKpredRhoCp, icolumn);
		SetColumn(compx->TimeKm1AusteniteDistribution, compx->TimeKAusteniteDistribution, icolumn);
		SetColumn(compx->TimeKAusteniteDistribution, compx->TimeKpredAusteniteDistribution, icolumn);

		/*	caloric mean tenperature			*/
		xx[7]	 = xx[2];
		xx[2]  = CalculationMeanTemperature( compx, temp_k);
		if(detaTime < dt_min)	
			xx[7] = 0.0f;
		else			
			xx[7]  = (xx[7] - xx[2])/detaTime;		/* local cooling rate	*/

		/*	output of calculated data			*/
		xx[4]	= temp_k[0];
		xx[12]	= temp_k[iCenter];

		/* output required, if surface temp.diff. exceeds limit d_temp */
		/* or Temperature in center line exeeds d_temp */
		if((	fabs(xx[4] - xx[3]) > d_temp)  || 
			fabs(xx[12] - xx[11]) > d_temp)	{  
				OutputFunc(i_output,erg_f, erg_graph, xx, compx, temp_k, i_it);
		}

		iz	=  TemperatureEnd(timex, xx[2], compx);

		if((istop == 1)||(iz!=1)) 
			break;
		if(izaehler > anz_dt) 
			break;

		SetColumn(temp_km1, temp_k, icolumn);

	}	/*	time steps		*/

	/* 	纠正ion if mean Temperature desired	*/
	if(compx->calculationStopTemp > .0001){
		xgrad = (compx->calculationStopTemp - xx[6])/(xx[2] - xx[6]);
		iz = -1;
		while(++iz < icolumn)
			temp_k[iz]  = temp_km1[iz] * (1.f - xgrad) + temp_k[iz] * xgrad;
		xx[0] = timex - detaTime + xgrad * detaTime;
		compx->calculationStopTime = xx[0];
		xx[10] +=xgrad * detaTime -detaTime;
	}

	compx->localCoolRate = xx[7]; /* local 冷却速度*/
	run_time = xx[10];
	SetColumn(compx->startTempDistribution, temp_k, icolumn);
	SetColumn(compx->startAusteniteDistribution, compx->TimeKAusteniteDistribution, icolumn);
	xx[2]  = CalculationMeanTemperature( compx, temp_k);
	compx->calculationStopTemp = xx[2];
	compx->calculationTotalTime       = run_time;

	OutputFunc(i_output, erg_f, erg_graph, xx, compx, temp_k, i_it);

	if(i_output) 
		fprintf(erg_f,"\n\t --- stop: %d steps %g seconds ---",izaehler, xx[0]);

	/* energy balance */
	if(i_output) 
		EnergyReport( erg_f, pdi, compx, xx);

	if(i_output)  
		fclose(erg_graph);
	return(ixx);
}


int CTemperatureField::AlphaACCJET( ALPHAVALUES *alpha, const PHYSICAL *xy,float thickness) /* 参数   */
{

	FILE            *ffx = NULL;
	int             ixx = 0;
	char            zeile[200];
	char            alpha_ufc[100];
	float           al[10], al_alt[10], t[20];
	int             max = 199, maxi, ianz;
	int             anz_temp, anz_flow, iz;
	float           flow_spray, flow_alt, max_flow, flow, r;

	flow_spray      = xy->topFlowRate;
	maxi            = xy->maxPossibleMillTemp;
	ianz = maxi/100 + 2;

	if(ixx != 0)
	{
		printf("\n\t *** not enough memory in alpha spray *** \n\n");
		return 1;
		//exit(1);            //原程序之间退出 2012-5-10 谢谦修改
	}

	CombinFilePath("AlphaACCJET.dat", xy->taskID,alpha_ufc);

	if((ffx = fopen(alpha_ufc,"r")) == NULL )
		return 2;

	fgets(zeile,max,ffx);
	fscanf(ffx,"%d%d%f", &anz_temp, &anz_flow, &max_flow);  //fscanf 有bug 遇见空格结束
	if(flow_spray > max_flow)
	{
		fclose(ffx);    //谢谦 添加 2012-7-22 
		return 3;
	}

	fgets(zeile,max,ffx);
	fgets(zeile,max,ffx);    

	sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &t[0],&t[2],&t[4],&t[6],
		&t[8],&t[10],&t[12],&t[14]);    //t 为Temperature
	iz = -1;
	while(++iz < anz_temp)
		al_alt[iz] = 0.0f;
	flow_alt = 0.0f;

	while(fgets(zeile,max,ffx)){
		sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &al[0],&al[1],&al[2],&al[3],
			&al[4],&al[5],&al[6],&al[7]);
		if(flow > flow_spray)
			break;
		iz = -1;
		while(++iz < anz_temp)
			al_alt[iz] = al[iz];
		flow_alt = flow;
	}

	ixx = - 1;
	r = (flow - flow_spray)/(flow - flow_alt);
	iz = -1;
	while(++iz < anz_temp){
		ixx +=2;
		t[ixx] = al[iz] * ( 1.f - r) + al_alt[iz] * r;
	}

	fclose(ffx);

	CalculateLinearInterpolation(alpha->top, t, anz_temp, maxi);
	CalculateLinearInterpolation(alpha->bot, t, anz_temp, maxi);

	return 0;
}

int CTemperatureField::AlphaACCSLIT( ALPHAVALUES *alpha, const PHYSICAL *xy) /* 参数   */
{
	FILE            *ffx = NULL;
	int             ixx = 0;
	char            zeile[200];
	char            alpha_ufc[100];
	float           al[10], al_alt[10], t[20];
	int             max = 199, maxi, ianz;
	int             anz_temp, anz_flow, iz;
	float           flow_spray, flow_alt, max_flow, flow, r;


	flow_spray      = xy->topFlowRate;
	maxi            = xy->maxPossibleMillTemp;
	ianz = maxi/100 + 2;

	CombinFilePath("AlphaACCSLIT.dat", xy->taskID,alpha_ufc);

	if((ffx = fopen(alpha_ufc,"r")) == NULL ){
		return 2;
	}

	fgets(zeile,max,ffx);
	fscanf(ffx,"%d%d%f", &anz_temp, &anz_flow, &max_flow);  //fscanf 有bug 遇见空格结束
	if(flow_spray > max_flow){
		fclose(ffx);
		return 3;
	}

	fgets(zeile,max,ffx);
	fgets(zeile,max,ffx);    

	sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &t[0],&t[2],&t[4],&t[6],
		&t[8],&t[10],&t[12],&t[14]);    //t 为Temperature
	iz = -1;
	while(++iz < anz_temp)
		al_alt[iz] = 0.0f;
	flow_alt = 0.0f;

	while(fgets(zeile,max,ffx)){
		sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &al[0],&al[1],&al[2],&al[3],
			&al[4],&al[5],&al[6],&al[7]);
		if(flow > flow_spray)
			break;
		iz = -1;
		while(++iz < anz_temp)
			al_alt[iz] = al[iz];
		flow_alt = flow;
	}

	ixx = - 1;
	r = (flow - flow_spray)/(flow - flow_alt);
	iz = -1;
	while(++iz < anz_temp){
		ixx +=2;
		t[ixx] = al[iz] * ( 1.f - r) + al_alt[iz] * r;
	}

	fclose(ffx);

	CalculateLinearInterpolation(alpha->top, t, anz_temp, maxi);  
	/////////////////////////// change by wbxang 20100629 ////////////////////////////////////
	CalculateLinearInterpolation(alpha->bot, t, anz_temp, maxi);  //??????

	return 0;
}


int CTemperatureField::AlphaUFCSLIT( ALPHAVALUES *alpha, const PHYSICAL *xy) /* 参数   */
{

	FILE            *ffx = NULL;
	int             ixx = 0;
	char            zeile[200];
	char           alpha_ufc[100];
	float           al[10], al_alt[10], t[20];
	int             max = 199, maxi, ianz;
	int             anz_temp, anz_flow, iz;
	float           flow_spray, flow_alt, max_flow, flow, r;

	flow_spray      = xy->topFlowRate;
	maxi            = xy->maxPossibleMillTemp;
	ianz = maxi/100 + 2;


	CombinFilePath("AlphaUFCSLIT.dat", xy->taskID,alpha_ufc);

	if((ffx = fopen(alpha_ufc,"r")) == NULL )
		return 2;

	fgets(zeile,max,ffx);
	fscanf(ffx,"%d%d%f", &anz_temp, &anz_flow, &max_flow);  //fscanf 有bug 遇见空格结束
	if(flow_spray > max_flow){
		fclose(ffx);
		return 3;
	}

	fgets(zeile,max,ffx);
	fgets(zeile,max,ffx);    

	sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &t[0],&t[2],&t[4],&t[6],
		&t[8],&t[10],&t[12],&t[14]);    //t 为Temperature
	iz = -1;
	while(++iz < anz_temp)
		al_alt[iz] = 0.0f;
	flow_alt = 0.0f;

	while(fgets(zeile,max,ffx)){
		sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &al[0],&al[1],&al[2],&al[3],
			&al[4],&al[5],&al[6],&al[7]);
		if(flow > flow_spray)
			break;
		iz = -1;
		while(++iz < anz_temp)
			al_alt[iz] = al[iz];
		flow_alt = flow;
	}

	ixx = - 1;
	r = (flow - flow_spray)/(flow - flow_alt);
	iz = -1;
	while(++iz < anz_temp){
		ixx +=2;
		t[ixx] = al[iz] * ( 1.f - r) + al_alt[iz] * r;
	}

	fclose(ffx);
	CalculateLinearInterpolation(alpha->top, t, anz_temp, maxi);
	CalculateLinearInterpolation(alpha->bot, t, anz_temp, maxi);
	return 0;
}

int CTemperatureField::AlphaUFCJET( ALPHAVALUES *alpha, const PHYSICAL *xy) /* 参数   */
{
	FILE            *ffx = NULL;
	int             ixx = 0;
	char            zeile[200];
	char            alpha_ufc[100];
	float           al[10], al_alt[10], t[20];
	int             max = 199, maxi, ianz;
	int             anz_temp, anz_flow, iz;
	float           flow_spray, flow_alt, max_flow, flow, r;

	flow_spray      = xy->topFlowRate;
	maxi            = xy->maxPossibleMillTemp;
	ianz = maxi/100 + 2;

	if(ixx != 0){
		printf("\n\t *** not enough memory in alpha spray *** \n\n");
		return 1;
	}

	CombinFilePath("AlphaUFCJET.dat", xy->taskID,alpha_ufc);

	if((ffx = fopen(alpha_ufc,"r")) == NULL )
		return 2;

	fgets(zeile,max,ffx);
	fscanf(ffx,"%d%d%f", &anz_temp, &anz_flow, &max_flow);  //fscanf 有bug 遇见空格结束
	if(flow_spray > max_flow){
		fclose(ffx);
		return 3;
	}

	fgets(zeile,max,ffx);
	fgets(zeile,max,ffx);    

	sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &t[0],&t[2],&t[4],&t[6],
		&t[8],&t[10],&t[12],&t[14]);    //t 为Temperature
	iz = -1;
	while(++iz < anz_temp)
		al_alt[iz] = 0.0f;
	flow_alt = 0.0f;

	while(fgets(zeile,max,ffx)){
		sscanf(zeile,"%f%f%f%f%f%f%f%f%f", &flow, &al[0],&al[1],&al[2],&al[3],
			&al[4],&al[5],&al[6],&al[7]);
		if(flow > flow_spray)
			break;
		iz = -1;
		while(++iz < anz_temp)
			al_alt[iz] = al[iz];
		flow_alt = flow;
	}

	ixx = - 1;
	r = (flow - flow_spray)/(flow - flow_alt);
	iz = -1;
	while(++iz < anz_temp){
		ixx +=2;
		t[ixx] = al[iz] * ( 1.f - r) + al_alt[iz] * r;
	}

	fclose(ffx);

	CalculateLinearInterpolation(alpha->top, t, anz_temp, maxi);

	CalculateLinearInterpolation(alpha->bot, t, anz_temp, maxi);

	return 0;
}

int CTemperatureField::TimeSolverNoFirstOutPut(	FILE 			*st_err,
	FILE 		*erg_f ,
	const		INPUTDATAPDI *pdi,
	COMPUTATIONAL *compx,
	const		PHYSICAL *ax,
	const		MATERIAL 	*matal)
{
	/* 	calculation of Temperature distribution        			*/

	FILE	*erg_graph = NULL;
	int   iz;
	int 	ixx = 0;    /* error parameter for memory reservation		*/
	int 	imatrix	= compx->matrixElementNumber;	/*	no. of matrix koeff.  	*/
	int		icolumn = compx->thickNodeNumber;		/*	no. of thickNodeNumber	      	*/
	int		iCenter = icolumn / 2;
	char	fx_name[255];
	char	*name_ex[2] = { "w", "a"};
	int	i_nx = 1;
	/*	output-FILE for calculation results	*/
	float	dt_zero = compx->detaTime;		/*	time increment	      	*/
	float	detaTime;			/*	actuel time step			*/
	float	dt_min = 0.0001f; /* lowest allowed time step      */
	float	timex; 		/*	time						*/
	float	stiff[100];		/*	modified system stiffness matrix	*/
	float	rr[100];		/*	upper matrix				*/
	float	yy[50];		/*	column for cholesky			*/
	float	force[50];		/*	system force column			*/
	float	temp_k[50];	/*	Temperature column at 实际 time step	*/
	float	temp_km1[50];	/*	Temperature column at previous step	*/
	//	float   temp_m[50];	/*	middle Temperature in predictor step	*/
	float   xx[15];		/*	value for output control			*/
	float   d_temp;		/*	output if 表面Temperature diiffers	*/
	/*	more than d_temp from previous output	*/
	float	temp_kph[50];	/*	Temperature (t(k-1) + t(k))/2		*/
	float	temp_pred[50];	/*	predictor Temperature			*/
	float	xgrad = 0.0f;
	int	izaehler = 0;   /*	index for time integration		*/
	int	i_it;		/*	number of iteration			*/
	int	anz_it = 10;	/*	maximum number of iteration cycles	*/
	int anz_dt = 1000;  /* 最大 allowed No. time increments */
	float	residual;	/*	estimated error				*/
	float	epsilon = .1f;	/*	accepted error in calculation = .1 K	*/
	float   calculationStopTime = compx->calculationStopTime;
	int	istop = 0;
	int	i_output = compx->femCalculationOutput;
	static	float	run_time;

	if(ax->coolPhaseNumber == 0)	{
		run_time = 0.0f;		/* set run time to zero 	*/  // 谢谦 实验性屏蔽 空冷时 time不置0
		i_nx = 0;				/* FILE extension = "w"		*/
	}

	xx[10] = run_time;
	compx->calculationTotalTime = run_time;	/* store 实际 time */

	strcpy_s(fx_name, pdi->plateID);
	strcat(fx_name, "_x.dat");

	if(i_output)  {
		if(i_nx == 0) 
			remove(fx_name);
		if((erg_graph = fopen(fx_name,name_ex[i_nx])) == NULL)	{
			fprintf(st_err,"\n\t ** not able to open: %s ***", fx_name);
			return -1;
		}
	}

	if(i_output > 1) 
		OutputComputational(erg_f, compx);

	iz = -1; 
	while(++iz < 15) 
		xx[iz] = 0.0f;
	/*	time integration algorithem		*/

	/*	start 参数 for Temperature distribution	*/

	d_temp = InitTemperatureDistribution(compx, matal,temp_k, temp_km1);
	//d_temp = 1;  //谢谦 增大solver 输出的频率 只要一个time步内温差大于1 就输出
	if(pdi->taskID == PRE_CALC_SIM) 
		d_temp = 5.f;

	if(ax->coolPhaseNumber == 0) {
		/* calculate start distr. x */
		SystemMatrices(stiff, force, compx, temp_k, temp_k, 0.0f, ax, matal);
	}

	compx->detaTime = dt_zero;    //初始的detaTime

	xx[1] = CalculationMeanTemperature( compx, compx->startTempDistribution);  //计算出平均Temperature 并计算出ROCP等value

	if(ixx > 0) {
		fprintf(st_err,"\n\t *** not enough memory for allocation in: solver *** ");
		if(erg_graph != NULL) 
			fclose(erg_graph);
		return(1);
	}

	xx[2]	= xx[1];
	xx[3]	= temp_k[0];
	xx[4]	= xx[3];
	xx[5]	= xx[3];
	xx[11]	= temp_k[iCenter];
	xx[12] = xx[11];

	if(xx[0] < 0.001) {
		compx->computedNumber = -1;   	/* initialize output value */
		iz = -1;
		while(++iz < TEMPERATURE_CURVE_NUMBER) {
			compx->zx[iz]		= 0.0001f;
			compx->tx[iz]		= 0.0f;
			compx->cx[iz]		= 0.0f;
			compx->tx_cen[iz]	= 0.0f;
			compx->tx_top[iz]	= 0.0f;
			compx->tx_bot[iz]	= 0.0f;
		}  		/* initialize output value */
	}


	timex = 0.0f;
	if(compx->testParameter == 2)
		dt_zero *= .1f;
	detaTime    = dt_zero;
	iz	= 1;

	while(iz == 1) {	/*     loop over time increments	*/
		izaehler++;

		if(compx->testParameter == 1)
			//detaTime = dt_est(detaTime, dt_zero, temp_km1, temp_k, izaehler, icolumn - 1);        //原程序
			// 一般空冷时初始条件 会令itest = 1 谢谦 现有问题是 此函数返回的冷却time过长 在一组集管的长度内 不符合精度
			detaTime = 0.5f*EstimationTemperatureDifference(detaTime, dt_zero, temp_km1, temp_k, izaehler, icolumn - 1);       //谢谦进行修改 减小time间隔

		if( calculationStopTime > .001)  {
			if((timex + detaTime) > calculationStopTime) {
				detaTime = calculationStopTime - timex;
				istop = 1;
			}
		}

		timex += detaTime;
		xx[0]  = timex;
		xx[10] +=	detaTime;
		xx[6]  = xx[2];

		/*	initiate for iteration cycle				*/

		SetColumn(temp_kph, temp_km1, icolumn);
		SetColumn(temp_pred, temp_km1,icolumn);
		SetColumn(compx->TimeKpredAusteniteDistribution, compx->TimeKAusteniteDistribution,icolumn); 
		SetColumn(compx->TimeKpredRhoCp, compx->TimeKRhoCp,icolumn);

		i_it= -1;
		while(++i_it < anz_it)	{	/*	Iteration		*/
			/* system matrices					*/
			SystemMatrices(stiff, force, compx, temp_km1, temp_kph, detaTime, ax, matal);
			/*	solution of the linear algebraic system		*/
			SolveAlgebraicMatrix(stiff, rr, yy, force, temp_k, icolumn);
			residual = Norm2FindMax(temp_pred,  temp_k, icolumn);
			if(residual < epsilon)	
				break;
			ColCrNi1(temp_kph, temp_km1, temp_k, icolumn);
			SetColumn(temp_pred, temp_k, icolumn);
		} 	/*	Iteration		*/

		compx->calculationTotalTime += detaTime;
		compx->detaTime = detaTime;
		SetColumn(compx->TimeKRhoCp, compx->TimeKpredRhoCp, icolumn);
		SetColumn(compx->TimeKm1AusteniteDistribution, compx->TimeKAusteniteDistribution, icolumn);
		SetColumn(compx->TimeKAusteniteDistribution, compx->TimeKpredAusteniteDistribution, icolumn);
		/*	caloric mean tenperature			*/
		xx[7]	 = xx[2];
		xx[2]  = CalculationMeanTemperature( compx, temp_k);

		if(detaTime < dt_min)	
			xx[7] = 0.0f;
		else			
			xx[7]  = (xx[7] - xx[2])/detaTime;		/* local cooling rate	*/

		/*	output of calculated data			*/
		xx[4]	= temp_k[0];
		xx[12]	= temp_k[iCenter];
		/* output required, if surface temp.diff. exceeds limit d_temp */
		/* or Temperature in center line exeeds d_temp */
		if((	fabs(xx[4] - xx[3]) > d_temp)  || fabs(xx[12] - xx[11]) > d_temp)  
			OutputFunc(i_output,erg_f, erg_graph, xx, compx, temp_k, i_it);

		iz	=  TimeEnd(timex, xx[2], compx);
		if((istop == 1)||(iz!=1)) 
			break;
		if(izaehler > anz_dt) 
			break;
		SetColumn(temp_km1, temp_k, icolumn);

	}	/*	time steps		*/

	compx->localCoolRate = xx[7]; /* local 冷却速度*/
	run_time = xx[10];
	SetColumn(compx->startTempDistribution, temp_k, icolumn);
	SetColumn(compx->startAusteniteDistribution, compx->TimeKAusteniteDistribution, icolumn);

	xx[2]  = CalculationMeanTemperature( compx, temp_k);
	compx->calculationTotalTime    = run_time;

	OutputFunc(i_output, erg_f, erg_graph, xx, compx, temp_k, i_it);
	if(i_output) 
		fprintf(erg_f,"\n\t --- stop: %d steps %g seconds ---",izaehler, xx[0]);

	/* energy balance */
	if(i_output) 
		EnergyReport( erg_f, pdi, compx, xx);

	if(i_output)  
		fclose(erg_graph);
	return(ixx);
}