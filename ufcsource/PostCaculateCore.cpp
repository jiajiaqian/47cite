/*! 
* Copyright (c) 2013, 东北大学轧制技术及连轧自动化国家重点实验室(RAL)
* All rights reserved
* 后计算核心
* FILE PostCaculateCore.cpp
* 说明 概述  
*  
* 详细说明
*  
* 版本 1.0 作者 Hanbing 日期 7/25/2013
* 
*/
#include "StdAfx.h"
#include "PostCaculateCore.h"
#include "../PreProcess.h"
#include <stdlib.h>
#include <cmath>
#include "SignalFormat.h"
#include "Mathematical.h"

CPostCaculateCore::CPostCaculateCore(void)
{
}

CPostCaculateCore::~CPostCaculateCore(void)
{
}

/// @函数说明 /* POSTPROCESS computation module FILE 
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int CPostCaculateCore::PostCoreProcessing
	(FILE *erg_f,
	FILE *st_err,
	const       int     isimula, 
	RALOUTDATA *pre_out,
	const       RALINDATA *ralInPut,
	const       RALCOEFFDATA *lay,
	RALMESDATA *mes,
	dbk_t   data_block[])
{
	int	iRet = 0; /* return value: = 0 O.K. */
	dbk_t 	  *DB1, *DB2, *DB3, *DB4, *DB5, *DB6, *DB7, *DB8;
	dbk_t     *DB9;			/* time telegram P5 */
	dbk_t     *DB10;	/* Temperature data P5 */
	dbk_t     *DB13, *DB14, *DB15, *DB16, *DB17, *DB18, *DB19, *DB20;

	float  t0_p1,  t0_sct, t0_fct, t0_p4;	/* zero time of each DB */
	float  t0_p2U,t0_p3U;

	//float  t0_p_top_ux1,t0_p_btm_ux1,t0_p_btm_ux2, t0_sut, t0_fut;	/* zero time of each DB */ // 暂时没用 [9/27/2013 谢谦]

	PHYSICAL      ver_par1;
	INPUTDATAPDI    pdiInput;
	COMPUTATIONAL compx;
	int			is_start;
	int           ixx, is;
	int			is_start_p1 = 0;
	int			is_start_p2U = 0;
	int			is_start_p3U = 0;
	int			is_start_p4 = 0;
	int			is_start_p5 = 0;
	int			is_start_p_top_ux1 = 0;
	int			is_start_p_btm_ux1 = 0;
	int			is_start_p_btm_ux2 = 0;

	int		iCheckPyroExit; /* 0: stay with P4, 1: replace with P5 */
	float     ti_p4Ref;
	float		Tm_p4Ref;
	int        i_middle = NODES_NUMBER/2;
	float     temp_diffx[NODES_NUMBER];
	float     sum1    = 0.0f;  /* Temperature error    */
	float     sum2    = 0.0f;
	float		diff_start;
	float		max_diff_start = 0.0f;
	int	    i_check_temp;

	/* -------------------------------------------------------------------------
	Statistical Data Array for FAT
	---------------------------------------------------------------------*/
	
	int	netseg;
	int	tolseg = 0; /*segments within tolerance */ 
	int	tolseg_p1 = 0;
	int	tolseg_p2 = 0;
	int	tolseg_p2L = 0;
	int	tolseg_p2U = 0;
	int	tolseg_p3L = 0;
	int	tolseg_p3U = 0;
	int	tolseg_p4 = 0;
	int	tolseg_p5 = 0;
	int	tolseg_p23 = 0;

	int	netsegx = 0; /* No. of segments for statistical use */
	int iStatusMasking;
	TiTemp	Seg;   	/* segment value			*/
	FATsegs	Seg_sv;	/* statistical value	*/

	int i_first_seg, i_last_seg, iyy,i;
	
	pdiInput = InitPostPDI(st_err,
		isimula, 
		&pre_out->PreResult,	
		data_block, 
		(const char*)(ralInPut->plateID)); 

	InitControlValues(&(pre_out->controlAcc), &(pdiInput.controlAcc)); //controlAcc 是在线和修正计算的控制参数结构体。 xq

	pdiInput.thick = ralInPut->thick;
	pdiInput.finishRollTemp = ralInPut->finishRollTemp;
	pdiInput.targetFinishCoolTemp = ralInPut->targetFinishCoolTemp;
	pdiInput.initFinishRollingTem = ralInPut->targetFinishCoolTemp;  // 谢谦 加入 2012-6-5
	pdiInput.pass_mode = ralInPut->pass_mode;
	pdiInput.activeSecType = ralInPut->activeSecType;
	pdiInput.activeFirstJet = ralInPut->activeFirstJet; // 谢谦 加入 2012-6-5 供自学习使用
	pdiInput.alloy_equC = (float)ralInPut->alloy_c + (float)ralInPut->alloy_mn/6.0f;
	pdiInput.iSimMode =ralInPut->iSimMode;
	//pdiInPut.cr = pdi_s->cr;
	pdiInput.FlowRate = 0;
	pdiInput.manualHC=ralInPut->manualHC;

	for(i=0;i<lay->nHeader;i++)
	{
		if(pre_out->nuzzleStatusTop[i] == 1){
			pdiInput.FlowRate = (int )mes->SetPtFlowTop[i]; /* Add Real FlowRate to Adaption Key  写入真实的水流密度*/
			break;
		}
	}
	/*	1.	开始分配 of Temperature over thickness */
	ixx = 0;  //错误判断变量

	ver_par1.waterTemp=lay->WaterTemp;  //add by xie
	if((pre_out->PreResult.alphaCoe>0.499)&&(pre_out->PreResult.alphaCoe<1.501))
		ver_par1.alpha_coe = pre_out->PreResult.alphaCoe;
	else
		ver_par1.alpha_coe = 1.0;

	ver_par1.adaptRatioFound=pre_out->PreResult.selfLearnAlphaCoe;   //add by xie 8-17

	CalculateStartDistributionOfTemperature(&ver_par1,&compx,&pdiInput,st_err,erg_f,temp_diffx);

	/* end of estimation from Temperature profile, start post_calculation for all segment data_block.*/
	DB1 = &data_block[1];	/* ti_p1 */
	DB2 = &data_block[2];	/* tm_p1 */
	DB3 = &data_block[3];	/* ti_p2U */
	DB4 = &data_block[4];	/* tm_p2U */
	DB5 = &data_block[5];	/* ti_sct */
	DB6 = &data_block[6];	/* ti_fct */
	DB7 = &data_block[7];	/* ti_p3U */

	/* check for 低 level / high level pyrometer */
	// 判断高温计类型 低温或高温 [9/26/2013 谢谦]
	if (pre_out->PreResult.finishCoolTargetTemperature > T_MIN_P4)
		DB8 = &data_block[8];
	else	
		DB8 = &data_block[9];

	if (pre_out->PreResult.finishCoolTargetTemperature > T_MIN_P5) {  
		DB9 = &data_block[10];	/* ti_p4  */
		DB10 = &data_block[11];/* tm_p4  */
	}
	else{	
		DB9 = &data_block[12];	/* ti_p4  */
		DB10 = &data_block[13];
	}

	DB13 = &data_block[13];	/* ti_p23 */   //注：data_block[13]已存p5Temperature
	DB14 = &data_block[14];	/* tm_p_top_Ux1 */

	DB15 = &data_block[15];	/* ti_p_btm_Ux1 */
	DB16 = &data_block[16];	/* tm_p_btm_Ux1 */
	DB17 = &data_block[17]; /* ti_sut */
	DB18 = &data_block[18];	/* ti_fut */
	DB19 = &data_block[19];	/* ti_p_btm_Ux2  */
	DB20 = &data_block[20];	/* tm_p_btm_Ux2  */     //应该用if语句，

	t0_p1  = (float) Tzero(DB1);
	t0_p2U  = (float) Tzero(DB3);
	t0_sct = (float) Tzero(DB5);
	t0_fct = (float) Tzero(DB6);
	t0_p3U  = (float) Tzero(DB7);
	t0_p4  = (float) Tzero(DB9);

	// 输出至post文件中[9/27/2013 谢谦]
	//FILE *fp;    
	//////更改路径到LogFILE夹  xiaoxiaoshine  20101227 ???????????????????????
	//if(( fp = fopen("Logs\\time\\time.txt","a"))== NULL)
	//	return 1;

	//fprintf(erg_f,"\n%s",ralInPut->plateID);
	fprintf(erg_f,"\n\n\t 冷却过程开始时间信息");
	fprintf(erg_f,"\n\t t0_p1=%f,t0_p2U=%f,t0_sct=%f,t0_fct=%f,t0_p3U=%f,t0_p4=%f",t0_p1,t0_p2U,t0_sct,t0_fct,t0_p3U,t0_p4);

	if (t0_p2U>50)   // 保护自学习 防止意外数据干扰 [11/18/2013 谢谦]
	{
		//fprintf(erg_f,"\n\n\t 空冷时间超过 50 不进行后计算自学习");
		//return -1;
	}

	//t0_p_top_ux1  = (float) Tzero(DB13);    // 暂时没用 [9/27/2013 谢谦]
	//t0_p_btm_ux1  = (float) Tzero(DB15);
	//t0_sut = (float) Tzero(DB17);
	//t0_fut = (float) Tzero(DB18);
	//t0_p_btm_ux2  = (float) Tzero(DB19);

	//确定数据的数量
	is_start_p1 =  CalculateStartSegmentInfo(&tolseg_p1, 
		CheckBit(1,pre_out->PreResult.iPyroBit), 
		D_TEMP_P1, 
		pdiInput.finishRollTemp,
		DB2); 

	is_start_p2U =  CalculateStartSegmentInfo(&tolseg_p2U,
		CheckBit(2,pre_out->PreResult.iPyroBit), 
		D_TEMP_P1, 
		pdiInput.finishRollTemp, 
		DB4); 

	is_start_p3U =  CalculateStartSegmentInfo(&tolseg_p3U,
		CheckBit(3,pre_out->PreResult.iPyroBit), 
		D_TEMP_P5, 
		pdiInput.targetFinishCoolTemp,
		DB8);
	is_start_p4 =  CalculateStartSegmentInfo(&tolseg_p4, 
		CheckBit(4,pre_out->PreResult.iPyroBit),
		D_TEMP_P5, 
		pdiInput.targetFinishCoolTemp,
		DB10);

	fprintf(erg_f,"\n\t is_start_p1 = %d,is_start_p2U = %d,is_start_p3U = %d,is_start_p4 = %d",
		is_start_p1,
		is_start_p2U,
		is_start_p3U,
		is_start_p4);
	fprintf(erg_f,"\n\t tolseg_p1   = %d,tolseg_p2U   = %d,tolseg_p3U   = %d,tolseg_p4   = %d",
		tolseg_p1,
		tolseg_p2U,
		tolseg_p3U,
		tolseg_p4);

	/* validate Temperature traces for later use */
	tolseg = ValidateTemprature(
		is_start_p1, 
		is_start_p2U, 
		is_start_p3U, 
		is_start_p4, 
		tolseg_p1,   
		tolseg_p2U,   
		tolseg_p3U,   
		tolseg_p4,
		&is_start);

	fprintf(erg_f,"\n\t is_start = %d,tolseg = %d",is_start,tolseg);  // 考虑把这些信息输出至每块钢的post 文件中 [9/26/2013 谢谦]

	if(tolseg < SEGMIN)  //如果数据有效长度小于4个，则输出没有后计算Temperature
		ReportUnpostTemperature(erg_f, 	tolseg_p1,  tolseg_p2U, tolseg_p3U,  tolseg_p4);


	netseg = 0;

	Seg_sv = InitStatisticalData(&ixx);
	i_first_seg =   OFFSET_SEG + is_start;  //此处的区段具体指哪得Temperature？还需看 xq 2012-2-7
	i_last_seg  =   i_first_seg + tolseg;
	iyy         =   i_last_seg -1;

	//mes->AveCoolTimeHead      = t0_fct - t0_sct;  // old [9/27/2013 谢谦]
	mes->AveCoolTimeHead      = t0_p3U - t0_p2U;   // 改为头部在冷却区的时间 [9/27/2013 谢谦]

	mes->AveCoolTimeRoot      = mes->AveCoolTimeHead;
	mes->AveCoolTimeRoot     += .01f*(DB5->data_word[iyy]-DB6->data_word[iyy]);
	//	mes->AveSpeed  = mes->NCoolBank * pre_out->PreResult.coolBankLen; old xie 8-21
	mes->AveSpeed  = pre_out->PreResult.headerNo * pre_out->PreResult.coolBankLen;  //add by xie 8-21 需注意 开启缝隙时集管数不对
	if((mes->AveCoolTimeHead > 0.0f) && (mes->AveCoolTimeRoot > 0.0f))
		mes->AveSpeed /= 0.5f * (mes->AveCoolTimeHead + mes->AveCoolTimeRoot);

	mes->AveSpeed=NUZZLE_NUMBER*2/mes->AveCoolTimeHead;  // 改为计算在冷却区内的时间 [9/27/2013 谢谦]

	fprintf(erg_f,"\n\t i_first_seg = %d,i_last_seg = %d,ave_speed calculated= %4.3f  ave_speed real= %4.3f ",i_first_seg,i_last_seg,mes->AveSpeed,lay->initSpeed+0.07);
	//fclose(fp);

	OutputTableHeader(erg_f,isimula, is_start, tolseg);

	pre_out->PreResult.aveSpeed=lay->initSpeed;  //add by 谢谦 8-8 给轧机二级发送数据
	
	//for (int temp1=7;temp1<NUZZLE_NUMBER;temp1++)  // 临时测试用 [9/27/2013 谢谦]
	//{
	//	pre_out->PreResult.realFlowUp[temp1]=200;
	//}
	
	//////////////////////////////////////////////////////////////////////////////////////////
	/* Main Loop over accepted segments */
	for (is=i_first_seg; is< i_last_seg; is++) 
	{	
		Seg.ti_p1  = .01f*DB1->data_word[is] + t0_p1;
		Seg.ti_p2U  = .01f*DB3->data_word[is] + t0_p2U;
		Seg.ti_sct = .01f*DB5->data_word[is] + t0_sct;
		Seg.ti_fct = .01f*DB6->data_word[is] + t0_fct;
		Seg.ti_p3U  = .01f*DB7->data_word[is] + t0_p3U;
		Seg.ti_p4  = .01f*DB9->data_word[is] + t0_p4;
		/* switch to higher Temperature */
		ti_p4Ref   = .01f*DB9->data_word[is] + t0_p4;

		//Seg.ti_p23 =  .01f*DB13->data_word[is] + t0_p_top_ux1;   // 暂时没有 [9/27/2013 谢谦]
		//Seg.ti_p2L =  .01f*DB15->data_word[is] + t0_p_btm_ux1;
		//Seg.ti_sut = .01f*DB17->data_word[is] + t0_sut;
		//Seg.ti_fut = .01f*DB18->data_word[is] + t0_fut;
		//Seg.ti_p3L =  .01f*DB19->data_word[is] + t0_p_btm_ux2;

		Seg.Tm_p1  = (float)DB2->data_word[is];
		Seg.Tm_p2U  = (float)DB4->data_word[is];
		if (Seg.Tm_p2U<300)
			Seg.Tm_p2U=pre_out->PreResult.afterAirCoolTemp;   //add by xie 8-24 防止 钢板太短 没有足够的Temperature
		Seg.Tm_p2L  = (float)DB16->data_word[is];//=600.0;////////////// wbxang 20100630 冷前下表面测温仪
		Seg.Tm_p23  = (float)DB14->data_word[is];//=650;////////////// wbxang 20100630 中间位置下表面测温仪
		Seg.Tm_p3U  = (float)DB8->data_word[is];
		Seg.Tm_p3L  = (float)DB20->data_word[is];//=600.0;
		Seg.Tm_p4  = (float)DB10->data_word[is];
		if(Seg.Tm_p4<400.0)
			Seg.Tm_p4 = pre_out->PreResult.aveTmP4;   //add by xie 8-28

		iCheckPyroExit = SelectPreferredMeasuringPoint(&pre_out->PreResult, 
			Seg.ti_p4 - Seg.ti_fct, 
			DB8->data_word[is], 
			ti_p4Ref - Seg.ti_fct, 
			DB10->data_word[is], 
			&Tm_p4Ref);

		if((iCheckPyroExit == 1)||(iCheckPyroExit == -1))	
		{
			Seg.ti_p4  = .01f*DB9->data_word[is] + t0_p4;
			Seg.Tm_p4  = Tm_p4Ref;
		}

		i_check_temp = CheckTemperature(Seg.Tm_p1, 
			Seg.Tm_p2U,
			pdiInput.finishRollTemp - 300.f,  //--------100.f to 300.f changed by xiaoxiaoshine 20110228--------//
			Seg.Tm_p4, 
			T_MIN_P4);

		if(i_check_temp == 0) 
			iRet +=100;

		//////////////////////////////
		if(i_check_temp ) { /* passed temp check */
			netseg ++;
			diff_start = pdiInput.finishRollTemp - Seg.Tm_p1;
			if (fabs(diff_start) > max_diff_start)
				max_diff_start= (float) fabs(diff_start);

			/* -------------------------------------------------------------------- */
			/* 2.			 air-cooling before water-cooling
			input  : measured Temperature at P2 and
			measured time for each segment group
			output : EN-Temperature-Distribution (NODES_NUMBER thickNodeNumber)
			for each segmentgroup
			---------------------------------------------------------------------	*/
			//前段空冷计算
			CheckDataPlausibility(&ver_par1,
				&compx,
				&pdiInput,
				st_err,
				erg_f,
				temp_diffx,
				Seg);
			Seg.Tc_sct = compx.calculationStopTemp;    /*  caloric mean Temperature 	*/
			Seg.air1   = compx.calculationStopTime;
			Seg.Tsc_sct = compx.startTempDistribution[i_middle];
			Seg.Tss_sct = compx.startTempDistribution[0];
			//add by xie 8-18
			i=0;

			while(++i < NODES_NUMBER) {  //谢谦加入 保存空冷后的Temperature 2012-5-16
				pre_out->PreResult.airEndTemp[i]=Seg.Tm_p2U; //modify by xie 8-22
				pre_out->PreResult.airEndAust[i]=compx.startAusteniteDistribution[i];
			}

			/*	3.1.	estimate calculation parameteres (PHYSICAL)		*/
			///层冷过程计算
			SetWaterCoolingParam(&ver_par1,&pdiInput,&pre_out->PreResult,	DB1); /* 06-sep o.k. */

			/* 3.2.	calculate Temperature drop ENTRY_ACC -> EXIT_ACC		*/
			/*
			CalculateWaterCooling(ver_par1,
			compx,
			pdiInPut,
			st_err,
			erg_f,
			Seg);
			*/

			//fprintf(erg_f,"\n 开冷wendu %f",compx->tx[compx->computedNumber-1]); 谢谦 测试 加入
			//if (1==pdi_s->offLineAdaptFlg)   // xie 9-6 自动选择离线或在线自学习

			if (2!=ralInPut->op_mode)	
				gPreprocess.CalcTempCurvePostOffLine(st_err,erg_f,&pdiInput,lay,pre_out,&ver_par1,&compx,mes->AveSpeed);	
			else
				gPreprocess.CalcTempCurvePost(st_err,erg_f,&pdiInput,lay,pre_out,&ver_par1,&compx);	
			
			CalculateSegmentData(&compx,&Seg);

			/* -----------------------------------------------------------------	*/
			/* 4.		 air-cooling after water-cooling
			input  : EX-Temperature-Distribution (NODES_NUMBER thickNodeNumber) and
			measured time for each segmentgroup
			output : Temperature-Distribution (NODES_NUMBER thickNodeNumber) at point P4
			for each segmentgroup
			time DIFFERENCE BETWEEN EXIT ACC AND MEASURING POINT P4/P5
			----------------------------------------------------------------- 	 */
			CalculateAirCoolMeasurePos(&ver_par1,
				&compx,
				&pdiInput,
				st_err,
				erg_f,
				Seg);

			/* ----------------------------------------------------------------	*/
			/* 5.  Calculate Real_FCT and 自适应value based on difference of
			calculated Tmean and measured Temp at P5
			------------------------------------------------------------  	*/
			EstimateAdaptonValues(&compx,
				is,
				&pdiInput,
				&Seg);

			sum1    +=  Seg.CRreal/Seg.CRcalc;

			if((netseg > SEGZX) && (netseg <  (tolseg-SEGZX)))
			{
				netsegx++;
				gSignalFormat.CalculateStatisticalValueForReport(&pdiInput,
					&Seg_sv,
					Seg, 
					netsegx);
			}

			OutputEachSegment(erg_f, 
				isimula,
				pdiInput.acc_mode,
				netseg, 
				Seg,
				is - OFFSET_SEG, 
				mes->fLengthSeg);
		} /* passed temp check */
	}     /*      loop over segment groups */

	if(netseg == 0)
	{
		CalculateStatisticalInfo(&pdiInput,&Seg_sv, 	mes);
	}
	
	//if (1==pdi_s->offLineAdaptFlg)   // xie 9-6 自动选择离线或在线自学习
	int offLineFlag=0;
	if (2!=ralInPut->op_mode)	
	{
		offLineFlag=1;   //add by xie 9-6
	} 
	else
	{
		offLineFlag=0;    //add by xie 9-6
	}
	
	ixx = gSignalFormat.StatisticalValueResult(&pre_out->PreResult,
		&Seg_sv,
		&pdiInput, 
		erg_f, 
		netsegx,
		offLineFlag); /* statistical res. */   //add by xie 8-21

	int nCout=0;
	float aveFlow=0.0f;
	fprintf(erg_f,"\n\n\t set avespeed = %4.3f , add by xie \n",pre_out->PreResult.aveSpeed);
	for (i=0;i<NUZZLE_NUMBER+GAP_NUZZLE_NUMBER;i++){
		if (pre_out->PreResult.realFlowUp[i] >30.0f){
			aveFlow+=pre_out->PreResult.realFlowUp[i];
			fprintf(erg_f,"\n\t 开启集管号 %d  ,   流量 %4.1f ",i+1,pre_out->PreResult.realFlowUp[i]);
			nCout++;
		}	
	}
	if (0!=nCout)
		aveFlow=aveFlow/nCout;

	if (1==offLineFlag)
	{

		fprintf(erg_f,"\n\t 进行了离线自学习 实际流量信息为:");
		fprintf(erg_f,"\n\t 开启集管数 %d  ,平均流量 %4.1f ",nCout,aveFlow);
	}
	else
	{
		fprintf(erg_f,"\n\t 进行了在线自学习 ");
		fprintf(erg_f,"\n\t 开启集管数 %d  ,平均流量 %4.1f ",nCout,aveFlow);
	}
	
	if(ixx == FALSE)
		iRet += 10000;
	ver_par1.isWriteAdaptValue = pre_out->PreResult.writeAdapt;

	iStatusMasking = gSignalFormat.CheckHeadTailMask(&pdiInput,
		lay,
		mes,
		pre_out, 
		data_block, 
		&Seg_sv, 
		st_err, 
		erg_f);

	//谢谦 测试屏蔽 2012-5-17
	//if((ixx == TRUE) && (((int)pdi_s->op_mode) == 2) && (((int)pdi_s->AdaptFlg) == 1) && (((int)pdi_s->CoolStatus) == 1)) 
	if ((ixx == TRUE) && (0!=aveFlow) && (((int)ralInPut->AdaptFlg) == 1)){
		//if (ralInPut->thick<0.05)   // 屏蔽 [3/12/2014 谢谦]
		{
			if (0 != ralInPut->acc_mode)
			{
				m_SelfLearnFunction.SuchWrite(erg_f, 
					&Seg_sv, 
					&pre_out->PreResult,
					&pdiInput,
					&ver_par1,
					Seg_sv.res[1],
					&pre_out->adapt);
			}

		}
		//else
		//fprintf(erg_f,"\n\t 没有进行自学习 厚度过大 ixx=%d , aveFlow= %5.1f , adaptFlg =%d",ixx,aveFlow,ralInPut->AdaptFlg);
	}
	else	
	{
		iRet += 10000;
		fprintf(erg_f,"\n\t 没有进行自学习 ixx=%d , aveFlow= %5.1f , adaptFlg =%d",ixx,aveFlow,ralInPut->AdaptFlg);
	}
		
	//add [1/13/2015 qian]
	if ((ixx == TRUE) && (0!=aveFlow) && (((int)ralInPut->AdaptFlg) == 1))
		m_SelfLearnFunction.AdaptationSLP_Write(
				&pdiInput,
				&pre_out->PreResult,
				&Seg_sv,
				&pre_out->SLP_Post,
				offLineFlag);
		
	mes->CoolCoeff[0] = (float)atof(ralInPut->plateID); /* todo if pltID is alphanumeric */
	mes->CoolCoeff[1] = ralInPut->thick;
	for (i=0;i<5;i++)
		mes->CoolCoeff[i+12]= (float) ver_par1.adaptKey[i];

	CopyInfoInSSAB(netsegx, &Seg_sv, mes);

	//AnalysisItems(erg_f,	netsegx, 	ralInPut, 	lay,pre_out, &Seg_sv, mes);  // 停止输出此信息 [8/16/2013 谢谦]

	//prn_add2(erg_f, mes);				// 停止输出此信息[8/15/2013 谢谦] 		

	return(iRet);
}

/// @函数说明 /*	reject segments for different Temperature criteria */
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::CheckTemperature(
	const	float	temp_p1,
	const	float	temp_p2,
	const	float	temp_p12_min,
	const	float	temp_p5,
	const	float	temp_p5_min)
{
	float	temp_p_max = temp_p1;
	if(temp_p_max < temp_p2)
		temp_p_max = temp_p2;
	/* reject , if no Temperature P1 or P2 is available */
	if(temp_p_max < temp_p12_min)
		return(0);
	if(temp_p_max < temp_p5)     
		return(0);
	if(temp_p5_min >temp_p5)    
		return(0);
	return(1);
}

/// @函数说明 函数简要说明-测试函数 
///* Output of measured data in 结果FILE  		*/
///* Remove function call in time - critical situation ! 	*/
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::OutputMeasuredResult(
	FILE	*erg_f,
	int		ioutput_flow,
	RALMESDATA *mes,
	RALCOEFFDATA *lay,
	RALOUTDATA   *pre_out)
{
	int	iz = -1;
	int i,j;

	fprintf(erg_f,"\n\n\n\n ______Output measured values_________________\n\n");
	fprintf(erg_f,"\n\t No.     time  Posit.   速度  TempP1 ");
	fprintf(erg_f," TempP2U  TempP3L  TempP3U  TempP4  TempSp1 TempSp2 TempSp3 TempSp4 TempSp5 TempSp6 TempSp7 TempSp8\n");
	while(++iz < mes->CurIndex)	
	{
		if (mes->Position[iz]>0)
		{
			if ((mes->TempP1[iz]>610)||(0!=mes->TempP2U[iz])||(0!=mes->TempP3U[iz])||(0!=mes->TempP4[iz]))
			{
				fprintf(erg_f,"\n\t %4d  %6.2f  %6.2f  %6.2f  %6.2f  %6.2f",
					iz, mes->TimeSinceP1[iz], mes->Position[iz],mes->Speed[iz],mes->TempP1[iz],mes->TempP2U[iz]);
				fprintf(erg_f,"  %6.2f  %6.2f  %6.2f  %6.2f   %6.2f  %6.2f  %6.2f  %6.2f  %6.2f  %6.2f  %6.2f",
					mes->TempP3L[iz],mes->TempP3U[iz],mes->TempP4[iz],mes->TempSp1[iz],mes->TempSp2[iz],mes->TempSp3[iz],mes->TempSp4[iz],mes->TempSp5[iz],mes->TempSp6[iz],mes->TempSp7[iz],mes->TempSp8[iz] );
			}
		}
	}

	if(ioutput_flow) 
	{ 	/* flow output only in 过程 version */
		fprintf(erg_f,"\n\n\n\n ______Output of measured flows_______________\n\n");
		fprintf(erg_f,"\n\n\tSyncronization points:"
			"\n\tHead/Tail        position");
		for(i=0;i<1;i++){ /*ANZ_HUB*/
			fprintf(erg_f,"\n");
			for(j=0;j< lay->nBls;j++)
			{
				fprintf(erg_f,"\t%5.2f/%5.2f[s]   %5.2f[m] %s\n",
					mes->fTimeBlsHead[i][j],
					mes->fTimeBlsTail[i][j],
					lay->fPosBls[j],
					lay->sNameBls[j]);
			}
		}
		fprintf(erg_f,"\n\tEdgeMasking: Ref./Act");
		for(i=0;i<lay->nEdge;i++){
			fprintf(erg_f,"\n\tNo.%i %5.2f[mm]",i,pre_out->edgePosition[i]);
		}

		fprintf(erg_f,"\n");
		/*                  ---+---+---1---+---2---+---3---+---4---+---5 */
		fprintf(erg_f,"\n\t No.    Z1Top   Z1Btm    Z2Top   Z2Btm    Z3Top   Z3Btm\n");
		iz = -1;
		while(++iz < NFLOW)	{
			fprintf(erg_f,"\n\t %4d  %6.2f  %6.2f  %6.2f  %6.2f  %6.2f  %6.2f", iz,
				mes->FlowZ1Top[iz],mes->FlowZ1Btm[iz],mes->FlowZ2Top[iz],
				mes->FlowZ2Btm[iz],mes->FlowZ3Top[iz],mes->FlowZ3Btm[iz]);
		}
		fprintf(erg_f,"\n");
	}	
	return(1);
}


/// @函数说明 函数简要说明-测试函数 
/// remove irrelevant measurements at the beginning of time  */
/// record (passes back to mill)                             */
/// keep relevant measurement (final pass through ACC)       */
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::InitMeasurements(
	FILE	*erg_f,
	int		ioutput_flow,
	RALMESDATA *mes,
	RALCOEFFDATA *lay,
	RALOUTDATA *pre_out)
{
	int	nRem = 0;	/* removed value */
	int nReverse = mes->CurIndex;
	int iStart   = nReverse;	/* start index relevant measurement */
	int iStartX  = 0; /* index with first increasing time */
	int iStartP1 = nReverse;
	int iStartP2 = nReverse;
	int iStartP3 = nReverse;
	int iEndeP1	 = 0;
	int iEndeP2	 = 0;
	int iEndeP3	 = 0;
	int iCheckPos1;
	int iCheckPos2;
	int iCheckPos3;
	int iz = -1;
	int index = -1;
	float	fEpsPos = EPSPOS;
	float	xMean, xVar, xMin, xMax;
	float	fDifTime;
	const	float	fEpsTime = 0.001f; /* 最小time difference [s] */

	/* check incresing time */
	iz = 0;
	while(++iz < nReverse) {
		fDifTime = mes->TimeSinceP1[iz] - mes->TimeSinceP1[iz-1];
		if(fDifTime < fEpsTime)
			iStartX += 1;
	}
	AnalyseArray(nReverse - iStartX, &mes->TempP1[iStartX], lay->fTempLowP1, 
		&xMean, &xVar, &xMin, &xMax, &iStartP1, &iEndeP1);
	iStartP1 += iStartX;
	iEndeP1  += iStartX;
	AnalyseArray(nReverse - iStartX, &mes->TempP2U[iStartX], lay->fTempLowP2U, 
		&xMean, &xVar, &xMin, &xMax, &iStartP2, &iEndeP2);
	iStartP2 += iStartX;
	iEndeP2  += iStartX;
	AnalyseArray(nReverse - iStartX, &mes->TempP23[iStartX],  lay->fTempLowP3U, 
		&xMean, &xVar, &xMin, &xMax, &iStartP3, &iEndeP3);
	iStartP3 += iStartX;
	iEndeP3  += iStartX;

	iCheckPos1 = GetValidValue(mes->Position[iStartP1], 
		lay->PosP1 - fEpsPos, lay->PosP1 + fEpsPos,  
		xMean, &xVar);
	iCheckPos2 = GetValidValue(mes->Position[iStartP2], 
		lay->PosP2U - fEpsPos, lay->PosP2U + fEpsPos,  
		xMean, &xVar);
	iCheckPos3 = GetValidValue(mes->Position[iStartP3], 
		lay->PosP3U - fEpsPos, lay->PosP3U + fEpsPos,  
		xMean, &xVar);
	if((iEndeP1 > iStartP1) && (iCheckPos1 == 0)) 
		iStart = iStartP1;
	if((iEndeP2 > iStartP2) && (iCheckPos2 == 0) && (iStartP2 < iStart)) 	
		iStart = iStartP2;
	if((iEndeP3 > iStartP3) && (iCheckPos3 == 0) && (iStartP3 < iStart)) 	
		iStart = iStartP3;
	if(iStart < 1)	
		return(nRem);
	if(iStart < 0)	
		return(nRem);
	
	iz = iStart - 1;

	while(++iz < mes->CurIndex){
		index++;
		mes->TimeSinceP1[index] = mes->TimeSinceP1[iz]; 
		mes->Speed[index]		= mes->Speed[iz]; 
		mes->Position[index]    = mes->Position[iz];
		mes->TempP1[index]		= mes->TempP1[iz];
		mes->TempP2L[index]		= mes->TempP2L[iz];
		mes->TempP2U[index]		= mes->TempP2U[iz];
		mes->TempP23[index]		= mes->TempP23[iz];
		mes->TempP4[index]		= mes->TempP4[iz];
		mes->TempP5[index]		= mes->TempP5[iz];
		mes->TempP6[index]		= mes->TempP6[iz];
	}
	mes->CurIndex = index;
	nRem = iStart;
	return(nRem);
}

/// @函数说明  copy additional output 信息 in SSAB - format 
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int CPostCaculateCore::CopyInfoInSSAB(           
							const	int	netseg, /* No. of segments passed */         
							FATsegs	*Seg_sv,	/* statistical value	*/
							RALMESDATA *mes)
{
	int iz = -1;
	int	nRep  = Seg_sv->seg; /* No. of report value */
	int nSend = NSEGSEND;	 /* 序号value to be send to database */
	float   min_flow = 10.f;
	float	temp_min = (float) T_MIN_P4;
	float	tempReport;
	/* Tss: Temperature sct: startcooling Temperature = Zon1Entry */ 
	mes->TempSurfCalcStartMin = Seg_sv->Tss_sct_sv.min;
	mes->TempSurfCalcStartMax = Seg_sv->Tss_sct_sv.max;
	mes->TempSurfCalcStartAve = Seg_sv->Tss_sct_sv.mean;

	mes->TempCentCalcStartMin = Seg_sv->Tsc_sct_sv.min; 
	mes->TempCentCalcStartMax = Seg_sv->Tsc_sct_sv.max; 
	mes->TempCentCalcStartAve = Seg_sv->Tsc_sct_sv.mean;
	mes->P2uAve               = Seg_sv->Tm_p2U_sv.mean;

	mes->TempSurfMeasStartMin = Seg_sv->Tm_p1_sv.min;
	mes->TempSurfMeasStartMax = Seg_sv->Tm_p1_sv.max;
	mes->TempSurfMeasStartAve = Seg_sv->Tm_p1_sv.mean;

	mes->TempSurfCalcStopMin  = Seg_sv->Tss_fct_sv.min;
	mes->TempSurfCalcStopMax  = Seg_sv->Tss_fct_sv.max;
	mes->TempSurfCalcStopAve  = Seg_sv->Tss_fct_sv.mean;

	mes->TempCentCalcStopMin  = Seg_sv->Tsc_fct_sv.min;
	mes->TempCentCalcStopMax  = Seg_sv->Tsc_fct_sv.max;
	mes->TempCentCalcStopAve  = Seg_sv->Tsc_fct_sv.mean;

	mes->TempSurfMeasStopStd  = Seg_sv->Tm_p4_sv.std;

	mes->TempSurfMeasStopMin  = Seg_sv->Tm_p4_sv.min;
	mes->TempSurfMeasStopMax  = Seg_sv->Tm_p4_sv.max;
	mes->TempSurfMeasStopAve  = Seg_sv->Tm_p4_sv.mean;

	/* CRsurf 冷却速度at survace (T_surface_start- T_surface_stop) cooling time */
	mes->CoolRateSurfCalcMin  = Seg_sv->CRsurf_sv.min;
	mes->CoolRateSurfCalcMax  = Seg_sv->CRsurf_sv.max;
	mes->CoolRateSurfCalcAve  = Seg_sv->CRsurf_sv.mean;

	/* CRsurf 冷却速度at survace (T_core_start- T_core_stop) cooling time */
	mes->CoolRateCentCalcAve  = Seg_sv->CRcore_sv.mean;
	mes->CoolRateSurfCalcMin  = Seg_sv->CRsurf_sv.min;
	mes->TempAveCalcStartAve  = Seg_sv->Tc_sct_sv.mean; 
	mes->TempAveCalcStopAve   = Seg_sv->Tr_fct_sv.mean;
	mes->CoolRateAveCalcAve   = Seg_sv->CRreal_sv.mean;

	AnalysisPyrometers(MAX_MEASURE_VALUE, temp_min, &(mes->IndPupP1),&(mes->IndDropP1),mes->TimeSinceP1, mes->TempP1);
	AnalysisPyrometers(MAX_MEASURE_VALUE, temp_min, &(mes->IndPupP2U),&(mes->IndDropP2U),mes->TimeSinceP1, mes->TempP2U);
	if(mes->IndDropP2U == 0)
		AnalysisPyrometers(MAX_MEASURE_VALUE, temp_min, &(mes->IndPupP2U),&(mes->IndDropP2U),mes->TimeSinceP1, mes->TempP2U);
	AnalysisPyrometers(MAX_MEASURE_VALUE, temp_min, &(mes->IndPupP3U),&(mes->IndDropP3U),mes->TimeSinceP1, mes->TempP3U);
	AnalysisPyrometers(MAX_MEASURE_VALUE, temp_min, &(mes->IndPupP4),&(mes->IndDropP4),	mes->TimeSinceP1, mes->TempP4);
	AnalysisPyrometers(MAX_MEASURE_VALUE, temp_min, &(mes->IndPupP5),&(mes->IndDropP5),	mes->TimeSinceP1, mes->TempP5);

	mes->AveFlow[0] = AnalysisFlow(MAX_MEASURE_VALUE, min_flow , mes->FlowZ1Top, mes->FlowZ1Btm);
	mes->AveFlow[1] = AnalysisFlow(MAX_MEASURE_VALUE, min_flow , mes->FlowZ2Top, mes->FlowZ2Btm);
	mes->AveFlow[2] = AnalysisFlow(MAX_MEASURE_VALUE, min_flow , mes->FlowZ3Top, mes->FlowZ3Btm);
	mes->CoolCoeff[2] = 0.0f;
	if(netseg > 0)
		mes->CoolCoeff[2] = Seg_sv->res[1];
	mes->CoolCoeff[3] = (float) mes->NCoolBank;
	mes->CoolCoeff[4] = mes->AveFlow[0];
	mes->CoolCoeff[5] = mes->AveFlow[1];
	mes->CoolCoeff[6] = mes->AveFlow[2];
	mes->CoolCoeff[7] = mes->P2uAve;
	mes->CoolCoeff[8] = Seg_sv->Tc_sct_sv.mean;
	mes->CoolCoeff[9] = Seg_sv->Tc_fct_sv.mean;
	mes->CoolCoeff[10]= mes->AveCoolTimeHead;
	mes->CoolCoeff[11]= mes->AveCoolTimeRoot;

	while(++iz < 200)
	{
		mes->CoolTempColStopCalSeg[iz] = 0.0f;
		mes->CoolRateAvgCalSeg[iz]     = 0.0f;
		mes->CoolTempColStartCalSeg[iz]= 0.0f;
	}

	if(nRep == 0) 
		nRep = NSEGREP;
	mes->SegCount= nRep;
	iz = -1;

	while(++iz < nRep)	
	{
		tempReport = Seg_sv->fTx_fct[iz];
		if(Seg_sv->fTc_fct[iz] < Seg_sv->fTx_fct[iz])
			tempReport = Seg_sv->fTx_fct[iz];
		mes->CoolTempColStopCalSeg[iz] = tempReport;
		mes->CoolRateAvgCalSeg[iz]     = Seg_sv->fCR_fct[iz];
		mes->CoolTempColStartCalSeg[iz] = Seg_sv->fTc_sct[iz];
	}
	RangeReplace(nRep, nSend, temp_min, mes->CoolTempColStopCalSeg);
	RangeReplace(nRep, nSend, 0.01f, mes->CoolRateAvgCalSeg);

	/* copy back mean value*/
	mes->CoolTempColStopCalSegMean = Seg_sv->fTx_fctMean;
	mes->CoolRateAvgCalSegMean     = Seg_sv->fCR_fctMean;

	mes->CoolTempColStopCalMinSeg = 9999;
	mes->CoolRateAvgCalMinSeg = 9999;

	for(iz=0;iz<nRep;iz++)
	{
		mes->CoolTempColStopCalMaxSeg = MAX(mes->CoolTempColStopCalMaxSeg,Seg_sv->fTx_fct[iz]);
		if(Seg_sv->fTx_fct[iz]>0.0f)
			mes->CoolTempColStopCalMinSeg = MIN(mes->CoolTempColStopCalMinSeg,Seg_sv->fTx_fct[iz]);

		mes->CoolRateAvgCalMaxSeg = MAX(mes->CoolRateAvgCalMaxSeg,Seg_sv->fCR_fct[iz]);
		if(Seg_sv->fTx_fct[iz]>0.0f)
			mes->CoolRateAvgCalMinSeg = MIN(mes->CoolRateAvgCalMinSeg,Seg_sv->fCR_fct[iz]);
	}
	return TRUE;
}

/// @函数说明 函数简要说明-测试函数 
/// cut value in middle range                         */
/// copy 1/3 of value on 头部 end                     */
/// copy 1/3 of value on 尾部 end                     */
/// remaining 1/3 on middle                            */
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::RangeReplace(
						const	int			nData,		/* complete data trace */
						const	int			nMax,		/* 最大 No. of report data */
						const	float		fEpsMin,	/* 低 limit */
						float		*fVal)		/* I/O: value */
{
	int	iz = -1;
	float	 xMean, xVar, xMin, xMax;
	float	 *fValTmp;
	float	 *fAbsTmp;
	float	 fIncrLength;
	float    fPos;
	float	 fZeta1, fZeta2;
	int		 iStart, iEnde;
	int		 nVal;
	int		 iPosTmp;
	int		 nRemain;

	int einDrittel;

	if((nMax < 1) || (nData < 1))
		return FALSE;
	if(nData < nMax)	
		return FALSE;

	AnalyseArray(nData, fVal, fEpsMin, &xMean, &xVar, &xMin, &xMax, &iStart, &iEnde);
	nVal = iEnde - iStart + 1;

	if(nVal < 2)
		return FALSE;
	if(nVal < nMax)
		return FALSE;
	einDrittel = (int) (nMax * 0.333f);
	if((fValTmp = (float *) calloc(nMax, sizeof(float))) == NULL) 
		return FALSE;

	if((fAbsTmp = (float *) calloc(nVal, sizeof(float))) == NULL) {
		free(fValTmp);
		return FALSE;
	}
	fIncrLength = 1.f / (nVal - 1.f);
	fPos = -fIncrLength;
	iz = -1;
	while(++iz < nVal) {
		fPos += fIncrLength;
		fAbsTmp[iz] = fPos;
	}
	iz = -1;
	while(++iz < einDrittel) {
		fValTmp[iz]				= fVal[iStart+iz];
		iPosTmp					= nVal - iz - 1;
		fValTmp[nMax-iz-1]		= fVal[iEnde - iz];
	}
	fZeta1	= fAbsTmp[einDrittel-1];
	fZeta2	= fAbsTmp[iPosTmp];
	nRemain = nMax - 2 * einDrittel + 1;
	if(nRemain < 1)	
		fIncrLength = 0.0f;
	else	
		fIncrLength = (fZeta2 - fZeta1) / nRemain;
	fPos = fZeta1;
	iz = -1;

	while(++iz < (nRemain - 1)){
		fPos                    += fIncrLength;
		fValTmp[einDrittel + iz] = Interpolation(nVal, fPos, fAbsTmp, &fVal[iStart]);
	}
	iz = -1;
	while(++iz < nMax)		
		fVal[iz] = fValTmp[iz];
	iz--;   
	while(++iz < nData)	
		fVal[iz] = 0.0f;

	free(fValTmp);
	free(fAbsTmp);
	return TRUE;
}

/// @函数说明 函数简要说明-测试函数 
/// analysis contractual items                         */
///  send to customer system                            */
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::prn_add2(FILE *erg_f, const RALMESDATA *mes)
{
	const	float	*pfTempStop = mes->CoolTempColStopCalSeg;
	const	float	*pfCr       = mes->CoolRateAvgCalSeg;
	int iz = -1;
	const	char	*sHead[] = {"Nr.","Temp_Stop", "Cr"};
	fprintf(erg_f, "\n\n\t Telegram Level II");
	fprintf(erg_f, "\n\t %3s %12s %12s", sHead[0], sHead[1], sHead[2]);
	while(++iz < NSEGSEND)
		fprintf(erg_f, "\n\t %3d %12.4f %12.4f", iz+1, pfTempStop[iz], pfCr[iz]);
	fprintf(erg_f,"\n");
	return TRUE;
}

/// @函数说明 analysis contractual items
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int CPostCaculateCore::AnalysisItems(
						FILE *erg_f,
						const	int	netseg, /* No. of segments passed */
						const   RALINDATA *ralInPut,
						const   RALCOEFFDATA *lay,
						const	RALOUTDATA *pre_out,
						FATsegs	*Seg_sv,	/* statistical value	*/
						const   RALMESDATA *mes)
{
	int i;
	int iStartP1 = 0;
	int iEndeP1	 = 0;
	int iCtrTempFail = 0;
	const	int iStatus = pre_out->PreResult.iPyroBit;
	int iHeaderFirst = 0;             
	int iHeaderLast  = lay->nHeader-1; 
	int	  iPosBlsEntry = mes->Tracking[mes->CurPass].iBlsEntry;
	float fPosBlsEntry = lay->fPosBls[iPosBlsEntry];
	int	  iPosBlsExit  = mes->Tracking[mes->CurPass].iBlsExit;
	float fPosBlsExit  = lay->fPosBls[iPosBlsExit];
	float fPosEntryAcc;
	float fPosExitAcc;
	float fLengthActiveAcc;
	float fLengthPlate = ralInPut->length;
	float fRunTimeHeadRef;
	float fRunTimeHeadAct;
	float fRunTimeTailRef;
	float fRunTimeTailAct;
	float fRunTimeDevHead;
	float fRunTimeDevTail;
	/* radar measurment BLS Entry BLS to Exit BLS 头部 */
	float fRunTimeHeadRadar;
	float fRunTimeHeadRadarRef;
	/* radar measurment BLS Entry BLS to Exit BLS 尾部 */
	float fRunTimeTailRadar;
	float fRunTimeTailRadarRef;
	float fRunTimeDev;
	float fRunTimeRef;
	float fRunTimeDevHeadRadar;
	float fRunTimeDevTailRadar;
	float fRunTimeDevRadar;
	float fRunTimeRefRadar;
	float fEpsSpd = 0.0f;
	int iz = -1;
	float	xMean, xVar, xMin, xMax;
	float	fTempSeg;
	float	fTempDevMax = 0.0f;
	float	fCr = ralInPut->cr;
	const	MODELCONTROL *controlAcc = &pre_out->controlAcc;
	/* allowed Temperature 偏差 by contract */
	const	float fTempDevEntryContr = controlAcc->fMinMaxTempEntry;
	float fTempDevExitContr  = controlAcc->fMinMaxTempExit;
	float fTempDevMeas;
	float fTempOutRange;
	float fTempExitMin;
	float fTempExitMax;
	float fTempDevTrack = 0.0f;

	for(i=0;i<lay->nHeader;i++){
		if(pre_out->nuzzleStatusTop[i] ==1 || pre_out->nuzzleStatusBottom[i] == 1 ){
			iHeaderFirst = i;
			break;
		}
	}
	for(i=iHeaderFirst;i<lay->nHeader;i++){
		if(pre_out->nuzzleStatusTop[i] == 0 || pre_out->nuzzleStatusBottom[i] == 0){
			iHeaderLast = i;
			break;
		}
	}

	fPosEntryAcc = lay->fPosHeader[iHeaderFirst];
	fPosExitAcc  = lay->fPosHeader[iHeaderLast] + lay->fLengthHeader[iHeaderLast];
	fLengthActiveAcc = fPosExitAcc - fPosEntryAcc;

	///////////////////////////////////////////////////////////////
	fRunTimeHeadRef = GetIntervalTimeSpeed(	fPosEntryAcc, 
		fPosExitAcc, 
		NSPDPROFILE, pre_out->speedPosition, pre_out->speed);
	fRunTimeHeadAct = GetIntervalTimeAvailable(	fPosEntryAcc, 
		fPosExitAcc, 
		mes->CurIndex, mes->TimeSinceP1, mes->Position);
	fRunTimeTailRef = GetIntervalTimeSpeed(	fPosEntryAcc + fLengthPlate, 
		fPosExitAcc  + fLengthPlate, 
		NSPDPROFILE, pre_out->speedPosition, pre_out->speed);
	fRunTimeTailAct = GetIntervalTimeAvailable(	fPosEntryAcc + fLengthPlate, 
		fPosExitAcc  + fLengthPlate,
		mes->CurIndex, mes->TimeSinceP1, mes->Position);
	fRunTimeDevHead = fRunTimeHeadRef - fRunTimeHeadAct;
	fRunTimeDevTail = fRunTimeTailRef - fRunTimeTailAct;
	/* radar measurment BLS Entry BLS to Exit BLS 头部 */
	fRunTimeHeadRadar    = mes->Tracking[0].TimeHeadPlt;
	fRunTimeHeadRadarRef = GetIntervalTimeSpeed(	fPosBlsEntry, 
		fPosBlsExit, 
		NSPDPROFILE, pre_out->speedPosition, pre_out->speed);
	/* radar measurment BLS Entry BLS to Exit BLS 尾部 */
	fRunTimeTailRadar = mes->Tracking[0].TimeTailPlt;
	fRunTimeTailRadarRef = GetIntervalTimeSpeed(	fPosBlsEntry + fLengthPlate, 
		fPosBlsExit  + fLengthPlate, 
		NSPDPROFILE, pre_out->speedPosition, pre_out->speed);
	fRunTimeDev = fRunTimeDevTail;
	fRunTimeRef = fRunTimeTailRef;
	fRunTimeDevHeadRadar = fRunTimeHeadRadarRef - fRunTimeHeadRadar;
	fRunTimeDevTailRadar = fRunTimeTailRadarRef - fRunTimeTailRadar;
	fRunTimeDevRadar = fRunTimeDevHeadRadar;
	fRunTimeRefRadar = fRunTimeHeadRadarRef;

	///////////////////////////////////////////////////////////////
	if((float)fabs(fRunTimeDevTail) < (float) fabs(fRunTimeDevHead)) {
		fRunTimeDev = fRunTimeDevHead;
		fRunTimeRef = fRunTimeHeadRef;
	}
	/* don't use because 尾部 遮蔽 radar is not working 
	if((float)fabs(fRunTimeDevTailRadar) > (float) fabs(fRunTimeDevHeadRadar)) 
	{
	fRunTimeDevRadar = fRunTimeDevTailRadar;
	fRunTimeRefRadar = fRunTimeTailRadarRef;
	} */
	if(fabs(fRunTimeDev) < fabs(fRunTimeDevRadar)) 
		fRunTimeDev = fRunTimeDevRadar;
	Seg_sv->fTempDevTempEntry = fTempDevEntryContr;
	Seg_sv->fTempDevSpd       = 0.0f;
	AnalyseArray(mes->CurIndex, mes->TempP1, TGFIMIN, 	&xMean, &xVar, &xMin, &xMax, &iStartP1, &iEndeP1);
	if(fCr < 0.01f)
		fCr = pre_out->PreResult.coolRateWaterProc; /* replace by model value if no PDI-value */
	if(fRunTimeRef > 0.01f) 
		fEpsSpd = (float)fabs(fRunTimeDev) / fRunTimeRef;
	/* 速度 偏差 */
	if(fEpsSpd > controlAcc->fAdaptDevSpd) 
		fTempDevTrack = fCr * (float) fabs(fRunTimeDev);
	fTempDevMeas  = xMax - xMin;
	fTempOutRange = (float) fabs(fTempDevMeas - fTempDevEntryContr);
	fprintf(erg_f, "\n\t Einlauf Temperatur Min: %8.2f 度 Max: %8.2f 度 OutRange: %8.2f K", 
		xMin, xMax, fTempOutRange);
	if(fTempOutRange > 0.0f)
	{
		Seg_sv->fTempDevTempEntry += controlAcc->fMinMaxTempExit * controlAcc->fFacDevTemp;
		fTempDevExitContr          = controlAcc->fMinMaxTempExit * controlAcc->fFacDevTemp;
	}
	fTempExitMin = ralInPut->targetFinishCoolTemp - fTempDevExitContr;
	fTempExitMax = ralInPut->targetFinishCoolTemp + fTempDevExitContr;

	fprintf(erg_f, "\n\n\t Allowed 偏差 Contract Entry Temperature: +/-%6.2f K", 
		fTempDevExitContr);
	fprintf(erg_f, " from: %6.2f 度 to:  %6.2f 度",  fTempExitMin, fTempExitMax);
	fprintf(erg_f, "\n\n\t Allowed 偏差 Contract Speed:   +/-%6.2f K", 
		fTempDevTrack);
	fprintf(erg_f, "\n\t Runtime 头部 within active ACC 参考: %6.2f sec Act:  %6.2f sec",
		fRunTimeHeadRef, fRunTimeHeadAct);
	fprintf(erg_f, "\n\t Runtime 尾部 within active ACC 参考: %6.2f sec Act:  %6.2f sec",
		fRunTimeTailRef, fRunTimeTailAct); 
	fprintf(erg_f, "\n\t 最大 偏差 Runtime caused by speed:%6.2f sec", fRunTimeDev);
	//fprintf(erg_f, "\n\t Percentage Error Speed:   %6.2f %", 
	//	fEpsSpd * 100.f);
	if(fabs(fRunTimeDevRadar) > fabs(fRunTimeDev)) 
		fTempDevTrack = fCr * (float) fabs(fRunTimeDevRadar);
	fprintf(erg_f, "\n\t Runtime 头部 from: %8s bis: %8s Ref.:  %6.2f sec Act:  %6.2f sec",
		lay->sNameBls[iPosBlsEntry],lay->sNameBls[iPosBlsExit],
		fRunTimeHeadRadarRef, fRunTimeHeadRadar);
	fprintf(erg_f, "\n\t Runtime 尾部 from: %8s bis: %8s Ref.:  %6.2f sec Act:  %6.2f sec",
		lay->sNameBls[iPosBlsEntry],lay->sNameBls[iPosBlsExit],
		fRunTimeTailRadarRef, fRunTimeTailRadar);
	fprintf(erg_f, "\n\t 最大 偏差 Measured (Radar):  %6.2f sec", fRunTimeDevRadar);
	fprintf(erg_f, "\n\t Additional Dev. Temp(Dev. * Cr = %g) = %6.2f K",
		fCr, fTempDevTrack);
	if(fEpsSpd > controlAcc->fAdaptDevSpd)
		fprintf(erg_f, " (Limit 速度 = %g exceeded)", controlAcc->fAdaptDevSpd);
	else	fprintf(erg_f, " (below Limit 速度 = %g)", controlAcc->fAdaptDevSpd);
	fTempExitMin -= fTempDevTrack;
	fTempExitMax += fTempDevTrack;
	fprintf(erg_f, "\n\t Total allowed range 停止冷却Temperature");
	fprintf(erg_f, " from: %6.2f 度 to:  %6.2f 度",  fTempExitMin, fTempExitMax);
	iz = -1;
	while(++iz < netseg){
		fTempSeg = Seg_sv->fTx_fct[iz];
		if(fabs(ralInPut->targetFinishCoolTemp - fTempSeg) > fabs(fTempDevMax)) 
			fTempDevMax =  ralInPut->targetFinishCoolTemp - fTempSeg;
		if(	(fTempSeg < fTempExitMin) ||
			(fTempSeg > fTempExitMax)) iCtrTempFail++;
	}
	fprintf(erg_f,"\n\t Segments outside Range: %d", iCtrTempFail);
	if(iCtrTempFail == 0) fprintf(erg_f," (Temperature Guarantee passed)\n");
	fprintf(erg_f,"\n\t 最大 偏差 Stop Temperature: %6.2f K", fTempDevMax); 
	return TRUE;
}



/// @函数说明 运行时通过分析测温仪
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/27/2013
int     CPostCaculateCore::AnalysisPyrometers(
	const   int     n,      /* 序号value   */
	const   float   min_val, /*最小value  */
	int   *val1,  /* first value     */
	int   *val2,  /* second value    */
	const   float   *timex, /*time 信息 */
	const   float   *valex) /*value 信息 */
{
	int     ifound = 0;       /* found no value */
	int     iz = -1;
	float	wert1 = 0.0f;

	*val1 = 0;
	*val2 = 0;

	while(++iz < n) {
		if(valex[iz] > min_val)
			break;
	}
	if(iz < (n-1))  {
		*val1  = (int)timex[iz];
		wert1  = valex[iz];
	}
	iz--;
	ifound = iz;
	while(++iz < n) {
		if(valex[iz] < min_val) 
			break;
	}
	ifound = iz - ifound;
	if(wert1 > min_val) 
		*val2  =(int) timex[iz-1];

	return(ifound);
}

float     CPostCaculateCore::AnalysisFlow(
	const   int     n,      /* 序号value   */
	const   float   min_val, /*最小value  */
	const   float   *f_top, /* top flow */
	const   float   *f_bot)
{
	int     iz = -1;
	int     n_val = 0;
	float   sum1 = 0.0f;

	while(++iz < n) {
		if((f_top[iz] > min_val)||(f_bot[iz] > min_val)){
			n_val++;
			sum1 += f_top[iz] + f_bot[iz];
		}
	}
	if(n_val > 0) 
		sum1 /= n_val;
	return(sum1);
}

//	Calculate statistical 信息 if no segment passed  
/// @函数说明 如果没有段通过计算统计信息
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/27/2013
int	CPostCaculateCore::CalculateStatisticalInfo(
	const		INPUTDATAPDI *pdiInPut,
	FATsegs		*Seg_sv,	/* statistical value	*/
	RALMESDATA *mes)
{
	int	iz = -1;
	int	i_first; /* first index with temp > T_MIN_P4 */ 
	int	i_last;  /* last index with temp > T_MIN_P4  */ 
	float	temp_min = (float)T_MIN_P4;
	float	alpha = 0.3f;
	float	fMean = 0.0f;
	//	float	fTempP5;
	float   fTempP4;
	float	fTempTrace;
	int	n_found;
	int	nMax = 19;
	GetFirstAndLastPos(&i_first, &i_last, MAX_MEASURE_VALUE, temp_min, mes->TempP1);
	EstimateTemperatureProfileStatisticalValues(&Seg_sv->Tm_p1_sv, i_first, i_last, mes->TempP1);
	n_found = GetFirstAndLastPos(&i_first, &i_last, MAX_MEASURE_VALUE, temp_min, mes->TempP2U);
	EstimateTemperatureProfileStatisticalValues(&Seg_sv->Tm_p2U_sv, i_first, i_last, mes->TempP2U);

	if(n_found < 2)	
	{
		GetFirstAndLastPos(&i_first, &i_last, MAX_MEASURE_VALUE, temp_min, mes->TempP2U);
		EstimateTemperatureProfileStatisticalValues(&Seg_sv->Tm_p2U_sv, i_first, i_last, mes->TempP2U);
	}

	GetFirstAndLastPos(&i_first, &i_last, MAX_MEASURE_VALUE, temp_min, mes->TempP4);
	EstimateTemperatureProfileStatisticalValues(&Seg_sv->Tm_p4_sv, i_first, i_last, mes->TempP4);
	n_found = i_last - i_first + 1;

	if(n_found < nMax) 
		nMax = n_found;
	iz = -1;

	while(++iz < nMax)	
	{
		fTempP4 = mes->TempP4[i_first+iz];
		if(fTempP4 < pdiInPut->targetFinishCoolTemp)	
		{
			fTempTrace  = (1.f - alpha) * fTempP4;
			fTempTrace += alpha * pdiInPut->targetFinishCoolTemp;
		}
		else	
			fTempTrace = fTempP4;
		Seg_sv->fTx_fct[iz] = fTempTrace;
		fMean += fTempTrace;
	}
	if(iz > 0)	
		fMean /= (iz);
	Seg_sv->fTx_fctMean = fMean;

	return(1);
}

/// @函数说明 estimate first and last index  ???????????????
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::GetFirstAndLastPos(
	int	*i_first, /* O: first index > eps  */
	int	*i_last,  /* O: last  index > eps  */
	const	int	n,	  /* No. of elements array */
	const	float	eps,	  /* 最小 accepted value */
	const	float	*x)
{
	int	n_found = 0; /* found elements */
	int	iz = -1;
	while(++iz < n)	{
		if(x[iz] > eps)	break;
	}
	*i_first = iz;
	iz--;
	while(++iz < n)	{
		if(x[iz] < eps)	break;
	}
	*i_last  = iz;
	n_found = iz - *i_first +1;
	return(n_found);
}

/// @函数说明 estimate statistical value for Temperature profile 
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::EstimateTemperatureProfileStatisticalValues(
	s_val_t	*s,
	const	int	i_first, 
	const	int	i_last, 
	const	float	*x)
{
	int	iz = i_first-1;
	int	n_anz = 0;

	while(++iz < i_last){
		n_anz++;
		ResetSVal(s, n_anz, x[iz], 3);
	}
	SetSVal(s, n_anz);
	return(n_anz);
}


/// @函数说明 fill in PDI - 信息 from first telegram
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
INPUTDATAPDI CPostCaculateCore::InitPostPDI(        FILE	*st_err,
								const  int     isimula,
								PRERESULT *PreResult,
								dbk_t 	data_block[],
								const char* plate_id)		
{
	INPUTDATAPDI pdiInPut;
	pdiInPut.taskID = POSTPROCESS;

	char            dbstry[100];
	char		*name_vpb = "ergout.dat";
	char           plate_nr[32];
	float          fAlloyMult = .0001f;
	int	iz = -1; /* index for alloy multiplier */
	int	exponentx;

	sprintf(plate_nr, "%s",plate_id);

	strcpy_s(pdiInPut.plateID,plate_nr);

	CombinFileName(plate_nr,pdiInPut.taskID,pdiInPut.file_nam);

	pdiInPut.isHeadFlated=PreResult->isHeadFlated;  

	pdiInPut.thick = PreResult->plateThick * 0.001f;
	pdiInPut.width = PreResult->plateWidth * 0.001f;
	pdiInPut.length =PreResult->plateLength;
	pdiInPut.finishRollTemp = PreResult->finishRollTemperature;
	pdiInPut.targetFinishCoolTemp = PreResult->finishCoolTargetTemperature;
	pdiInPut.cr = PreResult->coolRate;
	pdiInPut.acc_mode = PreResult->coolTypeMode;

	pdiInPut.SpeedEXP = PreResult->speedEXP;
	pdiInPut.AcclerEXP = PreResult->acclerEXP;

	if(pdiInPut.acc_mode > 4)	/* only for tests with the acc 	*/
		PreResult->coolTypeMode = 0;	

	/* changing const int *in	*/
	strcpy_s(pdiInPut.matID,PreResult->MatId);
	strcpy_s(pdiInPut.slabGrade,PreResult->slabGrade);

	if(PreResult->alloyExponent > 0) {             	/* alloy multiplier if setted */
		fAlloyMult = 1.f;
		exponentx = PreResult->alloyExponent;
		while(++iz < exponentx)	fAlloyMult *= 0.1f;
	}								/* alloy multiplier if setted */
	pdiInPut.alloy_c = PreResult->alloyC;
	pdiInPut.alloy_cr =PreResult->alloyCr;
	pdiInPut.alloy_cu =PreResult->alloyCu;
	pdiInPut.alloy_mn =PreResult->alloyMn;
	pdiInPut.alloy_mo =PreResult->alloyMo;
	pdiInPut.alloy_ni =PreResult->alloyNi;
	pdiInPut.alloy_si =PreResult->alloySi;
	/* Extension to elements> Titannium, Niobium, Boron, Vanadium      */
	pdiInPut.alloy_ti =PreResult->alloyTi;
	pdiInPut.alloy_nb =PreResult->alloyNb;
	pdiInPut.alloy_b  =PreResult->alloyB;
	pdiInPut.alloy_v  =PreResult->alloyV;
	pdiInPut.pl_mode = PreResult->preLevelerMode;

	/* keep the name task dependent in principle */
	if (pdiInPut.acc_mode > 0){
		if(isimula < 2){
			CombinFileName(plate_nr,pdiInPut.taskID,dbstry);
			CombinFilePath(dbstry,POSTPROCESS,pdiInPut.file_nam);
			/*	strcat(dbstry,".dat");  */
		}else{
			strcpy_s(dbstry, name_vpb);
			CombinFilePath(name_vpb,POSTPROCESS,dbstry);
			strcpy_s(pdiInPut.file_nam, dbstry);
		}
	}

	if (pdiInPut.acc_mode == 0){
		if(isimula < 2){
			CombinFileName(plate_nr,pdiInPut.taskID,dbstry);
			CombinFilePath(dbstry,POSTPROCESS,pdiInPut.file_nam);
		}
		else{
			strcpy_s(dbstry, name_vpb);
			CombinFilePath(name_vpb,POSTPROCESS,dbstry);
			strcpy_s(pdiInPut.file_nam, dbstry);
		}
	}
	return(pdiInPut);
}

/// @函数说明 calculate start segment and segments within tolerance
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::CalculateStartSegmentInfo(		int		    *tolseg,
	const	int			iStatusPyro,
	const	float		dtemp_max,
	const   float		target_temp,
	const	dbk_t 		*db_temp)
{
	/* estimate the number of start segments to perform post_calculation */
	int start_seg = 0;   /* default : beginning analysis from first segment */
	int d_temp  = (int) dtemp_max;  /* 最大 allowed Temperature 偏差 */
	int iz = -1;
	int t_low;   /* lowest allowed Temperature */
	int n_tol = 0; /* segments within tolerance */
	int	temp_min = T_MIN_P4;
	t_low = (int) target_temp - d_temp;
	if(t_low < temp_min) 
		t_low = temp_min;

	while(++iz < N)	
	{
		if(db_temp->data_word[OFFSET_SEG+iz] > t_low) 
			break;
	}

	start_seg = iz;  /* in error case -1 */
	--iz;

	while(++iz < N)	
	{
		if(db_temp->data_word[OFFSET_SEG+iz] < t_low) 
			break;
		n_tol++;
	}
	*tolseg = n_tol;
	return(start_seg);
}

/// @函数说明 calculate 开始分配 of Temperature over thickness for the position P1  and P2     
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::CalculateStartDistributionOfTemperature(	PHYSICAL 	*ver_par1,
							COMPUTATIONAL *compx,
							INPUTDATAPDI *pdiInPut,
							FILE		*st_err,
							FILE		*erg_f,
							float		*temp_diffx)
{
	/*	estimate 开始分配 of Temperature over thickness	*/
	/* in front of空冷						*/
	float		t_rand;
	MATERIAL 	matal;
	int		iz;
	int		ixx;

	ver_par1->sw_air = 0;
	ver_par1->sw_mat = 1;
	ver_par1->maxPossibleMillTemp = MAXTEMP;
	ver_par1->speed  = PLATE_SPEED;
	//ver_par1->taskID   = POSTPROCESS;
	ver_par1->taskID   = PREPROCESS;  //  [11/15/2013 谢谦]

	/* estimation of Temperature profile along 板材 length		*/
	ver_par1->actualStopTime     =       0.0f;
	ver_par1->actualStartTemp     = pdiInPut->finishRollTemp + 150.0f;
	compx->calculationStopTemp   = pdiInPut->finishRollTemp - 20.0f;
	ver_par1->sw_air = 0;

	ver_par1->t_umg = 23.f;
	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
	m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal,pdiInPut);

	compx->computeID    = 1;       /* only surface-Temperature    */
	ComputationalParameters( compx, pdiInPut, ver_par1);
	ixx = m_TemperatureField.CalculationTemperatureDistribution(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);
 
	iz = -1;
	t_rand= compx->startTempDistribution[0];	/* 表面Temperature	*/

	while(++iz< NODES_NUMBER)  
		temp_diffx[iz] = compx->startTempDistribution[iz] - t_rand;

	return(ixx);
}

/// @函数说明 calculate空冷 from P1 to ENTRY_ACC  - check data plausibility measured value
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::CheckDataPlausibility(PHYSICAL *ver_par1,
								COMPUTATIONAL *compx,
								INPUTDATAPDI *pdiInPut,
								FILE		*st_err,
								FILE		*erg_f,
								float		*temp_diffx,
								const		TiTemp		Seg)
{
	/*	calculate first 冷却区 (pyrometer to Entry ACC	*/
	MATERIAL matal;
	float		temp_node;	/* Temperature on node */
	float		ord;			/* scaled Temperature for X */
	float		xx;	/* 奥氏体分解 [%] */
	float		fTempP1;
	float		fTempP2U;
	int			iRelayP1;
	int			iRelayP2U;
	int		iz;
	int		ixx;

	const	float	fTempDefault = pdiInPut->finishRollTemp;
	const	float	fTempEps   = D_TEMP_P1;

	iRelayP1 = GetValidValue(Seg.Tm_p1, fTempDefault - fTempEps, 
		fTempDefault + fTempEps,
		fTempDefault, &fTempP1);
	iRelayP2U = GetValidValue(Seg.Tm_p2U, fTempDefault - fTempEps, 
		fTempDefault + fTempEps,
		fTempDefault, &fTempP2U);

	ver_par1->sw_air = 0;
	ver_par1->t_umg = 23.f;
	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
	m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal,pdiInPut);

	/* time between P1 - ENTRY COOLING AREA */
	/////////////////////////////////////
	//////add by hwf for different cooling type 

	ver_par1->actualStopTime = Seg.ti_sct - Seg.ti_p1;
	ver_par1->actualStartTemp = Seg.Tm_p1;

	if(iRelayP1 != 0){ /* P1 data not plausible */
		ver_par1->actualStopTime = Seg.ti_sct - Seg.ti_p2U;
		ver_par1->actualStartTemp = Seg.Tm_p2U;
	}

	/* no plausible data entry Temperature */
	if((iRelayP1 != 0) && (iRelayP2U != 0))	
		ver_par1->actualStartTemp = fTempDefault;

	if(ver_par1->actualStopTime < 0.5f) 
		ver_par1->actualStopTime = 0.5f;
	compx->computeID = 0;            /* only surface-Temperature    */
	iz = -1;

	while(++iz< NODES_NUMBER){
		temp_node			  = ver_par1->actualStartTemp + temp_diffx[iz];
		compx->startTempDistribution[iz] = temp_node;
		xx = -1.f;
		if(temp_node < matal.para.ar1) 
			xx = 0.0f;
		if(temp_node > matal.para.ar3) 
			xx = 100.0f;

		if(xx < 0.0f){
			ord = matal.para.ar3 - temp_node;
			xx  = m_TemperatureField.SearchAbsz(&matal.para.austenitTransPercent, 100.f, ord);
		}
		compx->startAusteniteDistribution[iz] = xx;
	}
	ComputationalParameters( compx, pdiInPut, ver_par1);
	compx->calculationStopTemp = 0.0f;            /* = 0 , because calculationStopTime <> 0 */
	ixx = m_TemperatureField.CalculationTemperatureDistribution(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);

	return(ixx);
}

/// @函数说明 set 参数 for 水冷
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::SetWaterCoolingParam(		PHYSICAL *ver_par1,
							INPUTDATAPDI *pdiInPut,
							const	PRERESULT *PreResult,
							const	dbk_t		*DB1)
{
	float	    area = 1.0f;
	int		iz = -1;
	int		iFlwTop = 0;	/* flow first active top    header [ltr/min] */
	int		iFlwBtm = 0;	/* flow first active bottom header [ltr/min] */
	int		iEps = 0;

	ver_par1->sw_air = 1;

	/* RR	pdiInPut->cr  = .1 * DB1->data_word[17]; keep original value */ 
	/* CR      */
	//changed by wbxang 20120516 
	ver_par1->waterTemp = PreResult->tempWater;  /* changed for RR */
	if((PreResult->coolBankLen>0) && (PreResult->coolWidth>0)) 
		area = PreResult->coolBankLen * PreResult->coolWidth;
	//area = 1.e-6f * PreResult->coolBankLen * PreResult->coolWidth;

	/* estimate flow of first active header unit */
	/* first header might be switched off by used 冷却模式 */
	iz = -1;
	while(++iz < (PreResult->iHeaderLast + 1))	{
		iFlwTop = (int)PreResult->upNuzzleNumber[iz];
		if(iFlwTop > iEps) 
			break;
	}

	iz = -1;
	while(++iz < (PreResult->iHeaderLast + 1))	{
		iFlwBtm = (int)PreResult->downNuzzleNumber[iz];
		if(iFlwBtm > iEps) 
			break;
	}
	ver_par1->topFlowRate = (float) iFlwTop /area;
	ver_par1->bottomFlowRate = (float) iFlwBtm /area;

	if((iFlwTop == 0) && (iFlwBtm == 0)) {
		ver_par1->sw_air = 0;
		return FALSE;
	}

	return TRUE;
}

/// @函数说明 Calculate 水冷 ENTRY_ACC => EXIT_ACC 
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::CalculateWaterCooling(PHYSICAL *ver_par1,
						COMPUTATIONAL		*compx,
						INPUTDATAPDI *pdiInPut,
						FILE		*st_err,
						FILE		*erg_f,
						const	TiTemp		Seg)
{
	/* 3.2.	水冷 in POSTPROCESS		*/
	MATERIAL matal;
	int		ixx;
	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
	pdiInPut->working_stage=1;
	m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal,pdiInPut);
	ver_par1->actualStopTime    = Seg.ti_fct - Seg.ti_sct;
	compx->calculationStopTime = ver_par1->actualStopTime;
	compx->computeID    = 0;        /* Temperature-distribution (NODES_NUMBER thickNodeNumber) */
	ComputationalParameters(compx, pdiInPut, ver_par1);
	compx->calculationStopTemp = 0.0f;              /* = 0 , because calculationStopTime <> 0 */
	ixx = m_TemperatureField.CalculationTemperatureDistribution(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);

	return(ixx);
}

/// @函数说明 calculate Segment data for position EXIT_ACC 
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::CalculateSegmentData(const	COMPUTATIONAL 	*compx, TiTemp	*Seg)
{
	float	adapt_calc;
	int     i_middle = NODES_NUMBER/2;
	float   T_core =   compx->startTempDistribution[i_middle];
	float   T_surf =   compx->startTempDistribution[0];

	Seg->Tc_fct = compx->calculationStopTemp;
	adapt_calc = (Seg->Tc_sct - Seg->Tc_fct)/compx->calculationStopTime;
	Seg->CRcalc = adapt_calc;
	Seg->ti_cool = compx->calculationStopTime;
	Seg->CRcore = (Seg->Tsc_sct - T_core)/compx->calculationStopTime;
	Seg->CRsurf = (Seg->Tss_sct - T_surf)/compx->calculationStopTime;
	Seg->Tss_fct = T_surf;
	Seg->Tsc_fct = T_core;

	return(1);
}

/// @函数说明 Calculate空冷 EXIT_ACC to measuring position
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::CalculateAirCoolMeasurePos(	PHYSICAL *ver_par1,
	COMPUTATIONAL *compx,
	INPUTDATAPDI *pdiInPut,
	FILE		*st_err,
	FILE		*erg_f,
	const	TiTemp		Seg)
{
	/*	4.	air cooling after watrer cooling	*/
	MATERIAL matal;
	int		ixx;

	ver_par1->sw_air = 0; /* air-cooling + Temperature-distributiondes */
	ver_par1->t_umg  = 23.f;
	matal = m_TemperatureField.CalcMaterialAttribute(st_err, pdiInPut, ver_par1);
	m_TemperatureField.SolveBoundary(st_err, ver_par1, &matal,pdiInPut);
	ver_par1->actualStopTime     = Seg.ti_p4 - Seg.ti_fct;
	compx->calculationStopTime   = ver_par1->actualStopTime;
	compx->computeID      = 0;                /* Temperature NODES_NUMBER thickNodeNumber */
	ComputationalParameters(compx, pdiInPut, ver_par1);
	compx->calculationStopTemp = 0.0f;              /* = 0 , because calculationStopTime <> 0 */
	ixx = m_TemperatureField.CalculationTemperatureDistribution(st_err, erg_f, pdiInPut, compx, ver_par1, &matal);

	return(ixx);
}

/// @函数说明  Estimate value for Adaption  
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/25/2013
int	CPostCaculateCore::EstimateAdaptonValues(	const		COMPUTATIONAL *compx,
	const		int			is,
	const		INPUTDATAPDI *pdiInPut,
	TiTemp		*Seg)
{
	float	temp_diff;
	/* Temperature difference mean - surface on Exit Pyrometer */
	float	temp_diff_p4;
	float adapt_real;
	float	diff_stop;
	if(is==OFFSET_SEG)	
		Seg->max_diff_stop = 0.0f;
	Seg->Tc_p4	= compx->calculationStopTemp;
	Seg->air2		= compx->calculationStopTime;
	Seg->Tb_p4	= compx->startTempDistribution[compx->thickNodeNumber-1];
	temp_diff		= Seg->Tm_p4 - Seg->Tb_p4;
	Seg->Tr_fct   = Seg->Tc_fct + temp_diff;

	if(Seg->Tr_fct < Seg->Tm_p4){
		temp_diff_p4 = compx->calculationStopTemp - compx->startTempDistribution[compx->thickNodeNumber-1];
		Seg->Tr_fct  = Seg->Tm_p4 + temp_diff_p4;
	}

	diff_stop    =  Seg->Tr_fct - pdiInPut->targetFinishCoolTemp;
	if(fabs(diff_stop)>Seg->max_diff_stop)
		Seg->max_diff_stop = (float)fabs(diff_stop);

	adapt_real = 	  Seg->CRcalc  - temp_diff/Seg->ti_cool;
	Seg->CRreal = adapt_real;
	return(1);
}



/// @函数说明 输出后计算的表头信息
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/27/2013
int	CPostCaculateCore::OutputTableHeader(	FILE *SEGdat, 
	int	isimula, 
	int is_start, 
	int tolseg)
{
	if(isimula)	
	{
		printf("\n\t Segments within tolerance : %d Start Segment: %d \n", tolseg, is_start);
		printf("Start Calculation of Water_Cooling for each segment\n");
		printf("     SEG Tm_p1 Tm_p2 Tc_sct Tc_fct Tm_p4 Tb_p4 Tr_fct CRcal CRact\n");
	}

	fprintf(SEGdat, "\n\n\n\t Segments within tolerance : %d Start Segment: %d", tolseg, is_start);

	fprintf(SEGdat,
		"\n\n  SEG Tm_p1 Tm_p2 Tc_sct Tc_fct Tm_p4 Tb_p4 Tr_fct CRcal CRact");
	fprintf(SEGdat," t_l1 t_AC  t_l2   Pos\n");
	return(1);
}


/// @函数说明 输出各段的计算数据
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/27/2013
int	CPostCaculateCore::OutputEachSegment(  
	FILE   *SEGdat,
	const  int    isimula,
	const  int    iMode, 
	const  int    netseg,
	const  TiTemp Seg,
	const  int    iSeg,        /* 实际 segment No. */
	const  float  fLengthSeg)  /* segment length */
{
	const	char sFormat[] = "  %3d %5.1f %5.1f %6.1f %6.1f %5.1f %5.1f %6.1f"
		" %4.1f %4.1f %5.1f %5.1f %5.1f %5.1f\n";
	const	float fPos = (iSeg   + 0.5f) * fLengthSeg;
	/* const	float fPos = (netseg - 0.5f) * fLengthSeg; */
	if(iMode == IC) /* modify to surface value */
		fprintf(SEGdat, sFormat, 
		netseg, Seg.Tm_p1, Seg.Tm_p2U, Seg.Tc_sct, Seg.Tc_fct, Seg.Tm_p4,
		Seg.Tb_p4, Seg.Tm_p4, Seg.CRcalc, Seg.CRsurf,
		Seg.air1, Seg.ti_cool, Seg.air2, fPos);
	else
		fprintf(SEGdat, sFormat, 
		netseg, Seg.Tm_p1, Seg.Tm_p2U, Seg.Tc_sct, Seg.Tc_fct, Seg.Tm_p4,
		Seg.Tb_p4, Seg.Tr_fct, Seg.CRcalc, Seg.CRreal,
		Seg.air1, Seg.ti_cool, Seg.air2, fPos);
	if(isimula)
		printf("%3d %5.1f %5.1f %6.1f %6.1f %5.1f %5.1f %6.1f %5.2f %5.2f\n",
		netseg, Seg.Tm_p1, Seg.Tm_p2U, Seg.Tc_sct, Seg.Tc_fct, Seg.Tm_p4,
		Seg.Tb_p4, Seg.Tr_fct, Seg.CRcalc, Seg.CRreal);
	return(1);
}



/// @函数说明 选择首选测量点
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/27/2013
int	CPostCaculateCore::SelectPreferredMeasuringPoint(
	const	PRERESULT *PreResult,
	const	float	fTimeP4,
	const	int		iTempP4,
	const	float	fTimeP5,
	const	int		iTempP5,
	float	*fTemp)
{
	int	iMod = 0;
	const	float	fThick = PreResult->plateThick * 0.001f;
	const	float	fTempStop = PreResult->finishCoolTargetTemperature;
	const	float	fDTempMeanP4 = dxx(fTempStop, fThick, fTimeP4); 
	const	float	fDTempMeanP5 = dxx(fTempStop, fThick, fTimeP5);
	const   float   fTempExP4 = iTempP4 + fDTempMeanP4;
	const   float   fTempExP5 = iTempP5 + fDTempMeanP5;
	const	float	fDTempPdiP4 = (float)fabs(fTempStop - fTempExP4);
	const	float	fDTempPdiP5 = (float)fabs(fTempStop - fTempExP5);
	int		iCheck = 0;
	const	int		iStatusP4 = CheckBit(4, PreResult->iPyroBit);
	const	int		iStatusP5 = CheckBit(5, PreResult->iPyroBit);
	*fTemp = 0.0f;
	if((iStatusP4 == 0) &&  (iStatusP5 == 0)) return(-1);
	*fTemp = (float)iTempP4;
	if(iStatusP5 == 0) return(iMod);
	if((fDTempPdiP5 < fDTempPdiP4) || (iStatusP4 == 0)) {
		*fTemp = (float) iTempP5;
		iCheck = 1;
		iMod = 1;
	}
	if(fThick > (0.001f * THICK_ADAPT_P5))	{
		if(iTempP5 > iTempP4) {
			*fTemp = (float) iTempP5;
			return(1);
		}
	}
	return (iMod);
}


/// @函数说明 函数简要说明-测试函数 
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/27/2013
float	 CPostCaculateCore::dxx(	
	const float	temp,
	const float	thick,
	const float	timex)
{
	float	xhil	=	5.62522e-5f * temp;
	float	xhil1;
	float	epsThick = 0.0001f;
	float	delta_temp;
	if(thick < epsThick) 
		return(0.0f);
	xhil1 =  .33333333333f * (float) pow(xhil, (float)3.) * timex/thick;
	delta_temp = temp * xhil1 * ( 1.f - .66666f * xhil1);
	return(delta_temp);
}


/// @函数说明select 最大 length and relaiable Temperature trace 
/// @参数 
/// @参数
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 8/6/2013
int	CPostCaculateCore::ValidateTemprature(
	const	int		iStartP1,	/* start index P1 Entry */
	const	int		iStartP2,	/* start index P2 Entry */
	const	int		iStartP4,	/* start index P4 Exit  */
	const	int		iStartP5,	/* start index P5 Exit  */
	const	int		nTolP1,	    /* tolerable segments P1 */
	const	int		nTolP2,	    /* tolerable segments P2 */
	const	int		nTolP4,	    /* tolerable segments P4 */
	const	int		nTolP5,	    /* tolerable segments P5 */
	int		*iStart)	/* start index */
{

	/* set value to default */

	int	is_startEntry = iStartP1;
	int	is_startExit  = iStartP4;
	int is_start;
	int nTolSegEntry  = nTolP1;
	int nTolSegExit   = nTolP4;
	int nTolSeg;

	/* select lowest start index Entry Pyrometer */
	if(is_startEntry > iStartP2) 
		is_startEntry = iStartP2;
	is_start = is_startEntry;
	/* select lowest start index Exit Pyrometer */
	if(is_startExit  > iStartP5) 
		is_startExit  = iStartP5;

	/* select highest index for complete Temperature trace */
	if(is_start < is_startExit) 
		is_start = is_startExit;

	if(nTolSegEntry < nTolP2) 
		nTolSegEntry = nTolP2;

	nTolSeg = nTolSegEntry;
	if(nTolSegExit  < nTolP5) 
		nTolSegExit  = nTolP5;
	if(nTolSeg > nTolSegExit) 
		nTolSeg = nTolSegExit;
	if(nTolSeg < SEGMIN) 
	{
		nTolSeg = 0;
	}
	*iStart = is_start;
	return(nTolSeg);
}

/// @函数说明 report reason for no postcalc Temperature 
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/27/2013
int	CPostCaculateCore::ReportUnpostTemperature(
	FILE	*erg_f,
	const	int		nTolP1,	    /* tolerable segments P1 */
	const	int		nTolP2,	    /* tolerable segments P2 */
	const	int		nTolP4,	    /* tolerable segments P4 */
	const	int		nTolP5)    /* tolerable segments P5 */
{
	fprintf(erg_f, "\n\n\t 因为有效数据点太少,没有进行后计算");
	fprintf(erg_f, "\n\t Entry Pyrometer 1: %d", nTolP1);
	fprintf(erg_f, "\n\t Entry Pyrometer 2: %d", nTolP2);
	fprintf(erg_f, "\n\t Exit  Pyrometer 4: %d", nTolP4);
	fprintf(erg_f, "\n\t Exit  Pyrometer 5: %d", nTolP5);

	return TRUE;
}

/// @函数说明 initialize control value adaption and online
/// @参数 
/// @参数 
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 8/6/2013
int CPostCaculateCore::InitControlValues(const MODELCONTROL *x, MODELCONTROL *y)
{
	*y = *x;

	return TRUE;

	/* 控制参数 online */
	y->bSmoothTempEntry     = x->bSmoothTempEntry;
	y->bSmoothTempExit      = x->bSmoothTempExit;
	y->bCorTempAct          = x->bCorTempAct;
	y->bCorSpdAct           = x->bCorSpdAct;
	y->fCorDevSpd           = x->fCorDevSpd;     
	y->bCorSpdSynchro       = x->bCorSpdSynchro;
	y->fCorTimeSynchro      = x->fCorTimeSynchro;
	y->fCorPosSynchro       = x->fCorPosSynchro;
	y->iBlsSynchro          = x->iBlsSynchro;
	y->fGainDevSpd          = x->fGainDevSpd; 
	y->fGainDevTemp         = x->fGainDevTemp;
	y->fDistSafety          = x->fDistSafety;
	/* 控制参数 adaption */
	y->iAdaptLevel          = x->iAdaptLevel;   
	y->nAdaptPlates         = x->nAdaptPlates;    


	y->bAdaptHeadRead       = x->bAdaptHeadRead;
	y->bAdaptTailRead       = x->bAdaptTailRead;
	y->bAdaptHeadWrite      = x->bAdaptHeadWrite;
	y->bAdaptTailWrite      = x->bAdaptTailWrite;
	y->fAdaptDifTime        = x->fAdaptDifTime;
	y->fAdaptLimitMin       = x->fAdaptLimitMin;
	y->fAdaptLimitMax       = x->fAdaptLimitMax;

	y->fAdaptStepSize       = x->fAdaptStepSize;
	y->fAdaptStepSizeTmp    = x->fAdaptStepSizeTmp;



	y->fAdaptLimitHeadMin   = x->fAdaptLimitHeadMin;
	y->fAdaptLimitHeadMax   = x->fAdaptLimitHeadMax;
	y->fAdaptLimitTailMin   = x->fAdaptLimitTailMin;
	y->fAdaptLimitTailMax   = x->fAdaptLimitTailMax;
	y->fAdaptAddFlwHead     = x->fAdaptAddFlwHead;
	y->fAdaptAddLenHead     = x->fAdaptAddLenHead;
	y->fAdaptAddFlwTail     = x->fAdaptAddFlwTail;
	y->fAdaptAddLenTail     = x->fAdaptAddLenTail;
	/* contractual 参数 */
	y->fMinMaxTempEntry     = x->fMinMaxTempEntry;
	y->fMinMaxTempExit      = x->fMinMaxTempExit;
	y->fFacDevTemp          = x->fFacDevTemp;
	y->fAdaptDevSpd         = x->fAdaptDevSpd;  
	y->fFacDevSpd           = x->fFacDevSpd;
	/* read 冷却模式 */
	y->iDimFlw              = x->iDimFlw;
	y->iDimEdge             = x->iDimEdge;
	y->iDimLenMask          = x->iDimLenMask;
	return TRUE;
}



/// @函数说明 initalize statistical data
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/27/2013
FATsegs  CPostCaculateCore::InitStatisticalData(int *iret)
{
	int	ixx = 0;
	FATsegs	xx;
	xx.Tm_p1_sv = InitSVal(1);ixx++;
	xx.Tm_p2U_sv = InitSVal(1);ixx++;
	xx.Tm_p3U_sv = InitSVal(1);ixx++;
	xx.Tm_p4_sv = InitSVal(1);ixx++;
	xx.Tb_p4_sv = InitSVal(1);ixx++;
	xx.Tm_p5_sv = InitSVal(1);ixx++;
	xx.Tb_p5_sv = InitSVal(1);ixx++;
	xx.Tc_sct_sv = InitSVal(1);ixx++;
	xx.air1_sv = InitSVal(1);ixx++;
	xx.air2_sv = InitSVal(1);ixx++;
	xx.ti_cool_sv = InitSVal(1); ixx++;
	xx.Tc_fct_sv = InitSVal(1); ixx++;
	xx.Tr_fct_sv = InitSVal(1); ixx++;
	xx.Tx_fct_sv = InitSVal(1); ixx++;
	xx.CRcalc_sv = InitSVal(1); ixx++;
	xx.CRreal_sv = InitSVal(1); ixx++;
	xx.CRcore_sv = InitSVal(1); ixx++;
	xx.CRsurf_sv = InitSVal(1); ixx++;
	xx.Tss_sct_sv = InitSVal(1); ixx++;
	xx.Tsc_sct_sv = InitSVal(1); ixx++;
	xx.Tss_fct_sv = InitSVal(1); ixx++;
	xx.Tsc_fct_sv = InitSVal(1); ixx++;
	*iret = ixx;
	ixx = -1;
	while(++ixx < NUMBER_OF_POST_RESULT) 
		xx.res[ixx] = 0.0f;
	ixx = -1;
	while(++ixx < N) 
	{
		xx.fti_cool[ixx] = 0.0f;
		xx.fTc_sct[ixx] = 0.0f;
		xx.fTc_fct[ixx] = 0.0f;
		xx.fTx_fct[ixx] = 0.0f;
		xx.fCR_fct[ixx] = 0.0f;
	}
	return(xx);
}

s_val_t	 CPostCaculateCore::InitSVal(	const	int inr)
{
	s_val_t	xx;

	xx.n   	 = 0;
	xx.max   = 0.0f;
	xx.min   = 0.0f;
	xx.mean  = 0.0f;
	xx.s1	 = 0.0f;
	xx.std   = 0.0f;
	xx.s2	 = 0.0f;
	return(xx);
}



int	CPostCaculateCore::ResetSVal(s_val_t	*s,	/*	statistical value	*/
	const	int		iv,	/* = 1: first call		*/
	const	float		val,	/*	value		*/
	const	int		ime)	/* method of calcul.		*/
{
	/*
	iv = 0:	set initial value for structure s_val_t
	ime = 0:	nothing to calculate
	1:	max and最小value
	2:	平均value
	3:	标准差
	*/
	if(ime < 1 )
		return FALSE;
	if(iv == 1)	
	{	/* set initial value for statistal value	*/
		s->n    =  0;
		s->max  =  FLT_MIN;
		s->min  =  FLT_MAX;
		s->mean =  0.0f;
		s->s1	=  0.0f;
		s->std  =  0.0f;
		s->s2	=  0.0f;
	}

	if(val > s->max)	
		s->max = val;
	if(val < s->min)	
		s->min = val;
	s->n++;
	if(ime < 2)		
		return TRUE;
	/*	subsequent calls	*/
	s->s1 += val;	/*	new 平均value	*/

	if(ime < 3)		
		return TRUE;
	s->s2 += val * val;
	return TRUE;
}


int	CPostCaculateCore::SetSVal(	s_val_t	*s,	/*	statistical value	*/
	const	int		iv)	/* = 0: first call		*/
{
	int	ivx = s->n;
	float std2;
	const float fEpsStdMin = 0.1e-4f;
	s->mean = 0.0f;
	s->std  = 0.0f;
	if(ivx != iv)
		return FALSE;
	if(ivx < 1)	
		return FALSE;
	s->mean = s->s1/ivx;
	if(ivx < 2)
		return TRUE;
	std2 = (s->s2 - ivx * s->mean * s->mean)/(ivx-1);
	if((float) std2 < fEpsStdMin)	
		s->std = 0.0f;
	else							
		s->std = (float)sqrt(std2);
	return TRUE;
}

/// @函数说明 组合文件名称
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 7/27/2013
int CPostCaculateCore::CombinFileName(const char *plate_id,
	const int  taskID,
	char *fn)
{	int ibuf = 0; 
	strcpy(fn,plate_id);
	strcat(fn,".dat");
	return(1);
}

