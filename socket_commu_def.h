#ifndef   SOCKET_COMMU_H_FILE       //如果没有定义这个宏   
#define  SOCKET_COMMU_H_FILE       //定义这个宏


//PM L2 TO ACC L2 103
#pragma pack(1)
typedef struct GfcEXTL2AlvStatusMsg{
	int MessageLength; 
	int MessageID; 
	int Sequence; 
	int Spare; 
	int GfcAlvMsgHor; 
	int GfcAlvMsgMin; 
	int GfcAlvMsgSec; 
	int GfcAlvMsgDay; 
	int GfcAlvMsgMon; 
	int GfcAlvMsgYer;
}GfcEXTL2AlvStatusMsg, *PGfcEXTL2AlvStatusMsg;

//ACC L2  TO PM L2   204
#pragma pack(1)
typedef struct GfcL2ExtAlvStatusMsg{
	int GfcAlvMsgHor; 
	int GfcAlvMsgMin; 
	int GfcAlvMsgSec; 
	int GfcAlvMsgDay; 
	int GfcAlvMsgMon; 
	int GfcAlvMsgYer;
}GfcL2ExtAlvStatusMsg, *PGfcL2ExtAlvStatusMsg;

//ACC L2  TO PM L2   205

#pragma pack(1)
typedef struct tagheader{
	int nMsgLength;
	int nMsgID;
	int nSeqCounter;
	int nFlags;
}HEADER, *PHEADER;

//PM L2 TO ACC L2 101
#pragma pack(1)
typedef struct GfcEXTL2PdiData
{
	char GfcPieceId[20];
	char GfcPieceIdSpare[20];     //
	char GfcPltMatId[16];         //
	char GfcPltMatIdSpare[16];    //
	char GfcPltMatCode[32];       //
	float GfcSlabLen;		 //
	float GfcSlabWid;		 //
	float GfcSlabTck;		 //
	float GfcInterSlabTcr1Len;	 //
	float GfcInterSlabTcr1Wid;
	float GfcInterSlabTcr1Tck;
	float GfcInterSlabTcr2Len;
	float GfcInterSlabTcr2Wid;
	float GfcInterSlabTcr2Tck;
	float GfcPltLen;
	float GfcPltWid;
	float GfcPltTck;
	int   GfcIdTsk;
	int   GfcRollPassNumNow;
	int   GfcTcr1RollPassNumSum;
	int   GfcTcr2RollPassNumSum;
	int   GfcRollPassNumSum;
	int   GfcTime2ACCDQ;
	float GfcInterSlabTcr1ColTimCal;
	float GfcInterSlabTcr2ColTimCal;
	int   GfcPltAccMode;
	char  GfcPltIcFlg[1];
	int   GfcShpCde;
	int   GfcProdState;
	char  GfcDirLstPas[1];
	float taper_hd_thk;
	float taper_tl_thk;
	float taper_l1_len;
	float taper_l2_len;
	float taper_l3_len;
	float taper_l4_len;
	float taper_l5_len;
	float GfcPltDlyTimRol;
	float GfcSurTmpRolFin;
	float GfcAveTmpRolFin;
	float GfcCorTmpRolFin;
	float GfcPltCrDq;
	float GfcPltTmpDqStp;
	float GfcPltTmpAccSta;
	float GfcPltCrAcc;
	float GfcPltTmpAccStp;
	int   GfcPltHprActFlg;
	int   GfcPltPlActFlg;
	float GfcSlbTcr1TmpRolFin;
	float GfcSlbTcr1TmpSta;
	float GfcSlbTcr1TmpStp;
	float GfcSlbTcr2TmpRolFin;
	float GfcSlbTcr2TmpSta;
	float GfcSlbTcr2TmpStp;
	float GfcSlbSurTmpTcr1Fin;
	float GfcSlbCalTmpTcr1Fin;
	float GfcSlbCoreTmpTcr1Fin;
	float GfcSlbTcr1TmpAccSta;
	float GfcSlbTcr1TmpAccStp;
	float GfcSlbSurTmpTcr2Fin;
	float GfcSlbCalTmpTcr2Fin;
	float GfcSlbCoreTmpTcr2Fin;
	float GfcSlbTcr2TmpAccSta;
	float GfcSlbTcr2TmpAccStp;
	float GfcPltPplTimHead;
	float GfcPltPplSpdHead;
	float GfcPltPplLenHead;
	float GfcPltPplTimBody;
	float GfcPltPplSpdBody;
	float GfcPltPplLenBody;
	float GrnZAccStaHead1;
	float GrnZAccStaHead2;
	float GrnZAccStaHead3;
	float GrnZAccStaHead4;
	float GrnZAccStaHead5;
	float GrnZAccStaMdle;
	float GrnZAccStaTail;
	float DcmpZAccStaHead1;
	float DcmpZAccStaHead2;
	float DcmpZAccStaHead3;
	float DcmpZAccStaHead4;
	float DcmpZAccStaHead5;
	float DcmpZAccStaMdle;
	float DcmpZAccStaTail;
	float GfcPltMatCompC;
	float GfcPltMatCompMn;
	float GfcPltMatCompP;
	float GfcPltMatCompS;
	float GfcPltMatCompSi;
	float GfcPltMatCompCu;
	float GfcPltMatCompV;
	float GfcPltMatCompNb;
	float GfcPltMatCompCr;
	float GfcPltMatCompNi;
	float GfcPltMatCompMo;
	float GfcPltMatCompSn;
	float GfcPltMatCompN;
	float GfcPltMatCompB;
	float GfcPltMatCompAlTot;
	float GfcPltMatCompTi;
	float GfcPltMatCompCa;
	float GfcPltMatCompPb;
	float GfcPltMatCompAlSol;
	float GfcPltMatCompSb;
	float GfcPltMatCompZn;
	float GfcPltMatCompAs;
	float GfcPltMatCompCo;
	float GfcPltMatCompMg;
	float GfcPltMatCompZr;
	float GfcPltMatCompW;
	float GfcPltMatCompTa;
	float Spare1;
	float Spare2;
	float Spare3;
	float Spare4;
	float Spare5;
	float Spare6;
	float Spare7;
	float Spare8;
	float Spare9;
	float Spare10;
}GfcEXTL2PdiData, *PGfcEXTL2PdiData;
#pragma pack()

//PM L2 TO ACC L2 102
#pragma pack(1)
typedef struct GfcEXTL2RealPdiData
{
	char GfcPieceId[20];		 //
	char GfcPieceIdSpare[20];     //
	char GfcPltMatId[16];         //
	char GfcPltMatIdSpare[16];    //
	char GfcPltMatCode[32];       //
	float GfcInterSlabTcr1Len;	 //
	float GfcInterSlabTcr1Wid;
	float GfcInterSlabTcr1Tck;
	float GfcInterSlabTcr2Len;
	float GfcInterSlabTcr2Wid;
	float GfcInterSlabTcr2Tck;
	float GfcPltLen;
	float GfcPltWid;
	float GfcPltTck;
	int   GfcPltRolMode;
	int   GfcDirLstPas;
	int   GfcRollPassNumNow;
	int   GfcRollPassNumRes;
	int   GfcRollPassNumSum;
	int   GfcTcr1RollPassNumSum;
	int   GfcTcr2RollPassNumSum;
	int   GfcPltAccReq;
	int   GfcPltAccMode;
	int   GfcPltIcMode;
	int   GfcPltIcBatch; 
	int   GfcPltRolNum;
	int   GfcShpCde;
	float GfcPltTmpSurMin;
	float GfcPltTapTck1;
	float GfcPltTapTck2;
	float GfcPltTapTck3;
	float GfcPltTapTck4;
	float GfcPltTapTck5;
	float GfcPltTapTck6;
	float GfcPltTapTck7;
	float GfcPltTapTck8;
	float GfcPltTapLen1;
	float GfcPltTapLen2;
	float GfcPltTapLen3;
	float GfcPltTapLen4;
	float GfcPltTapLen5;
	float GfcPltTapLen6;
	float GfcPltTapLen7;
	float GfcPltTapLen8;
	float GfcPltTmpColStaSurHead;
	float GfcPltTmpColStaCalHead;
	float GfcPltTmpColStaCoreHead;
	float GfcPltTmpColStaSurBody;
	float GfcPltTmpColStaCalBody;
	float GfcPltTmpColStaCoreBody;
	float GfcPltTmpColStaSurTail;
	float GfcPltTmpColStaCalTail;
	float GfcPltTmpColStaCoreTail;
	float GfcPltTcr1TmpColStaSurBody;
	float GfcPltTcr1TmpColStaCalBody;
	float GfcPltTcr1TmpColStaCoreBody;
	float GfcPltTcr2TmpColStaSurBody;
	float GfcPltTcr2TmpColStaCalBody;
	float GfcPltTcr2TmpColStaCoreBody;
	float GfcPltPplTimHead;
	float GfcPltPplSpdHead;
	float GfcPltPplLenHead;
	float GfcPltPplTimBody;
	float GfcPltPplSpdBody;
	float Spare1;
	float Spare2;
	float Spare3;
	float Spare4;
	float Spare5;
	float Spare6;
	float Spare7;
	float Spare8;
	float Spare9;
	float Spare10;
}GfcEXTL2RealPdiData, *PGfcEXTL2RealPdiData;
#pragma pack()

//ACC L2 TO PM L2 2011
#pragma pack(1)
typedef struct GfcL2PresetAccResult_LP
{
	char    GfcPieceId[20];
	char    GfcPieceIdSpare[20];
	char    GfcPltMatId[16];
	char    GfcPltMatIdSpare[16];
	char    GfcPltMatCode[32];
	float   GfcResPresetSpd;
	int     GfcPltAccType;
	int     GfcPltCoolMode1Pass;
	float   Spare1;
	float   Spare2;
	float   Spare3;
	float   Spare4;
	float   Spare5;
	float   Spare6;
	float   Spare7;
	float   Spare8;
	float   Spare9;
	float   Spare10;  
}GfcL2PresetAccResult_LP, *PGfcL2PresetAccResult_LP;
#pragma pack()

//ACC L2 TO PM L2 2012
#pragma pack(1)
typedef struct GfcL2PresetAccResult_tcr1
{
	char    GfcPieceId[20];
	char    GfcPieceIdSpare[20];
	char    GfcPltMatId[16];
	char    GfcPltMatIdSpare[16];
	char    GfcPltMatCode[32];
	float   GfcResSlbTcr1PresetSpd;
	int     GfcSlbTcr1AccType;
	int     GfcSlbTcr1CoolMode1Pass;
	float   Spare1;
	float   Spare2;
	float   Spare3;
	float   Spare4;
	float   Spare5;
	float   Spare6;
	float   Spare7;
	float   Spare8;
	float   Spare9;
	float   Spare10;        	
}GfcL2PresetAccResult_tcr1, *PGfcL2PresetAccResult_tcr1;
#pragma pack()

//ACC L2 TO PM L2 2013
#pragma pack(1)
typedef struct GfcL2PresetAccResult_tcr2
{
	char    GfcPieceId[20];
	char    GfcPieceIdSpare[20];
	char    GfcPltMatId[16];
	char    GfcPltMatIdSpare[16];
	char    GfcPltMatCode[32];
	float   GfcResSlbTcr2PresetSpd;
	int     GfcSlbTcr2AccType;
	int     GfcSlbTcr2CoolMode1Pass;
	float   Spare1;
	float   Spare2;
	float   Spare3;
	float   Spare4;
	float   Spare5;
	float   Spare6;
	float   Spare7;
	float   Spare8;
	float   Spare9;
	float   Spare10;        
}GfcL2PresetAccResult_tcr2, *PGfcL2PresetAccResult_tcr2;
#pragma pack()

//ACC L2 TO PM L2 2021
#pragma pack(1)
typedef struct GfcL2ExtAccResult_LP
{
	char    GfcPieceId[20];
	char    GfcPieceIdSpare[20];
	char    GfcPltMatId[16];
	char    GfcPltMatIdSpare[16];
	char    GfcPltMatCode[32]; 
	int	   GfcResAccReq;
	float GfcResPltTmpColStaSurHead; 
	float GfcResPltTmpColStaCalHead;
	float GfcResPltTmpColStaCorHead; 
	float GfcResPltTmpColStaSurBody; 
	float GfcResPltTmpColStaCalBody; 
	float GfcResPltTmpColStaCorBody; 
	float GfcResPltTmpColStaSurTail; 
	float GfcResPltTmpColStaCalTail; 
	float GfcResPltTmpColStaCorTail; 
	float GfcResPltTmpColStpSurHead; 
	float GfcResPltTmpColStpCalHead; 
	float GfcResPltTmpColStpCorHead; 
	float GfcResPltTmpColStpSurBody; 
	float GfcResPltTmpColStpCalBody; 
	float GfcResPltTmpColStpCorBody; 
	float GfcResPltTmpColStpSurTail;
	float GfcResPltTmpColStpCalTail; 
	float GfcResPltTmpColStpCorTail; 
	float GfcResColRatAvgCorDQ; 
	float GfcResColRatAvgCalDQ; 
	float GfcResColRatAvgSurDQ; 
	float GfcResColRatAvgCorACC; 
	float GfcResColRatAvgCalACC; 
	float GfcResColRatAvgSurACC; 
	float GfcResColTmpStaMilY; 
	float GfcResColTmpStaMilX; 
	float GfcResColTmpStaTckMea; 
	float GfcResColTmpStpAccX; 
	float GfcResColTmpStpHplY; 
	float GfcResTmpWat; 
	float GfcResPrsWat;
	char GfcResColStaDate[16];
	char GfcResColStpDate[16];
	float GfcResWatFlwAvgThd[24];  
	float GfcResWatFlwAvgBhd[24];  
	float GfcRefWatFlwRefTop[2]; 
	float GfcResWatFlwRefBot[2]; 
	float GfcResWatFlwAvgTop[2]; 
	float GfcResWatFlwAvgBot[2]; 
	float GfcResFrmPos[2]; 
	float GfcResColOpMode;
	int GfcResColErrSta;
	float GfcResHedMskTopFac;
	float GfcResHedMskTopLen; 
	float GfcResHedMskBotFac; 
	float GfcResHedMskBotLen; 
	float GfcResTalMskTopFac; 
	float GfcResTalMskTopLen; 
	float GfcResTalMskBotFac;
	float GfcResTalMskBotLen; 
	float GfcResHedRunTimRef; 
	float GfcResHedRunTimAct; 
	float GfcResHedRunTimAct2; 
	float GfcResTalRunTimRef; 
	float GfcResTalRunTimAct; 
	float GfcResTalRunTimAct2;
	float Spare[10];
}GfcL2ExtAccResult_LP, *PGfcL2ExtAccResult_LP;
#pragma pack()

//ACC L2 TO PM L2 2022
#pragma pack(1)
typedef struct GfcL2ExtAccResult_tcr1
{
	char    GfcPieceId[20];
	char    GfcPieceIdSpare[20];
	char    GfcPltMatId[16];
	char    GfcPltMatIdSpare[16];
	char    GfcPltMatCode[32];   //
	int     GfcResSlbAccReq; 
	float   GfcResSlbTcr1TmpColStpSurBody; 
	float   GfcResSlbTcr1TmpColStpCalBody; 
	float   GfcResSlbTcr1TmpColStpCorBody; 
	float   GfcResSlbTcr1TmpColStpSurTail; 
	float   GfcResSlbTcr1TmpColStpCalTail; 
	float   GfcResSlbTcr1TmpColStpCorTail; 
	float   GfcResSlbTcr1ColRatAvgCor; 
	float   GfcResSlbTcr1ColRatAvgCal; 
	float   GfcResSlbTcr1ColRatAvgSur; 
	float   GfcResSlbColTmpStaMilY; 
	float   GfcResSlbColTmpStaMilX; 
	float   GfcResSlbColTmpStaTckMea; 
	float   GfcResSlbColTmpStpAccX; 
	float   GfcResSlbColTmpStpHplY; 
	float   GfcResSlbTmpWat; 
	float   GfcResSlbPrsWat; 
	char 	GfcResSlbColStaDate[16]; 
	char 	GfcResSlbColStpDate[16]; 
	float   GfcResSlbWatFlwAvgThd[24];     
	float   GfcResSlbWatFlwAvgBhd[24];
	float   GfcRefSlbWatFlwRefTop[2];
	float   GfcResSlbWatFlwRefBot[2];
	float   GfcResSlbWatFlwAvgTop[2];
	float   GfcResSlbWatFlwAvgBot[2];
	float   GfcResSlbFrmPos[2];
	float   GfcResSlbColOpMode; 
	int     GfcResSlbColErrSta;
	float   GfcResSlbHedMskTopFac; 
	float   GfcResSlbHedMskTopLen; 
	float   GfcResSlbHedMskBotFac; 
	float   GfcResSlbHedMskBotLen; 
	float   GfcResSlbTalMskTopFac; 
	float   GfcResSlbTalMskTopLen; 
	float   GfcResSlbTalMskBotFac; 
	float   GfcResSlbTalMskBotLen; 
	float   GfcResSlbHedRunTimRef; 
	float   GfcResSlbHedRunTimAct; 
	float   GfcResSlbHedRunTimAct2; 
	float   GfcResSlbTalRunTimRef; 
	float   GfcResSlbTalRunTimAct; 
	float   GfcResSlbTalRunTimAct2;
	float   Spare[10];
}GfcL2ExtAccResult_tcr1, *PGfcL2ExtAccResult_tcr1;
#pragma pack()

//ACC L2 TO PM L2 2023
#pragma pack(1)
typedef struct GfcL2ExtAccResult_tcr2
{
	char    GfcPieceId[20];
	char    GfcPieceIdSpare[20];
	char    GfcPltMatId[16];
	char    GfcPltMatIdSpare[16];
	char    GfcPltMatCode[32];
	int	    GfcResSlbAccReq; 
	float   GfcResSlbTcr2TmpColStpSurBody; 
	float   GfcResSlbTcr2TmpColStpCalBody; 
	float   GfcResSlbTcr2TmpColStpCorBody; 
	float   GfcResSlbTcr2TmpColStpSurTail; 
	float   GfcResSlbTcr2TmpColStpCalTail; 
	float   GfcResSlbTcr2TmpColStpCorTail; 
	float   GfcResSlbTcr2ColRatAvgCor; 
	float   GfcResSlbTcr2ColRatAvgCal; 
	float   GfcResSlbTcr2ColRatAvgSur; 
	float   GfcResSlbColTmpStaMilY; 
	float   GfcResSlbColTmpStaMilX; 
	float   GfcResSlbColTmpStaTckMea; 
	float   GfcResSlbColTmpStpAccX; 
	float   GfcResSlbColTmpStpHplY; 
	float   GfcResSlbTmpWat; 
	float   GfcResSlbPrsWat; 
	char 	GfcResSlbColStaDate[16]; 
	char 	GfcResSlbColStpDate[16]; 
	float   GfcResSlbWatFlwAvgThd[24];
	float   GfcResSlbWatFlwAvgBhd[24];
	float   GfcRefSlbWatFlwRefTop[2];
	float   GfcResSlbWatFlwRefBot[2];
	float   GfcResSlbWatFlwAvgTop[2];
	float   GfcResSlbWatFlwAvgBot[2];
	float   GfcResSlbFrmPos[2];
	float   GfcResSlbColOpMode; 
	int     GfcResSlbColErrSta;
	float   GfcResSlbHedMskTopFac; 
	float   GfcResSlbHedMskTopLen; 
	float   GfcResSlbHedMskBotFac; 
	float   GfcResSlbHedMskBotLen; 
	float   GfcResSlbTalMskTopFac; 
	float   GfcResSlbTalMskTopLen; 
	float   GfcResSlbTalMskBotFac; 
	float   GfcResSlbTalMskBotLen; 
	float   GfcResSlbHedRunTimRef; 
	float   GfcResSlbHedRunTimAct; 
	float   GfcResSlbHedRunTimAct2; 
	float   GfcResSlbTalRunTimRef; 
	float   GfcResSlbTalRunTimAct; 
	float   GfcResSlbTalRunTimAct2;
	float   Spare[10];
}GfcL2ExtAccResult_tcr2, *PGfcL2ExtAccResult_tcr2;
#pragma pack()

//ACC L2 TO PM L2 2031
// 203-1 - GfcL2ExtColTimeRes (LP)
#pragma pack(1)
typedef struct GfcL2ExtColTimeRes_LP
{
	char GfcPieceId[20];
	float	GfcResFnlColTimWat;
	float	GfcResFnlCr;
	float	GfcResFnlAveTmp;
	float	GfcResFnlSufTmp;
	float	GfcResFnlColTimAir;
	float	Spare[10];
}GfcL2ExtColTimeRes_LP, *PGfcL2ExtColTimeRes_LP;
#pragma pack()

//ACC L2 TO PM L2 2032
#pragma pack(1)
typedef struct GfcL2ExtColTimeRes_tcr1
{
	char    GfcPieceId[20];
	float   GfcResInterSlabTcr1ColTimAir; 
	float   GfcResInterSlabTcr1ColTimWat; 
	float   GfcResTcr1ColTimAir; 
	float   GfcResTcr1ColTimWat; 
	int     GfcResTcr1ColTimSta; 
	float   GfcResTcr1PresetSpd; 
	int     GfcPltTcr1AccType; 
	int     GfcPltTcr1CoolMode1Pass;       
	float GfcResTcr1Cr;
	float GfcResTcr1AveTmp;
	float GfcResTcr1SufTmp;
	float Spare[10];
}GfcL2ExtColTimeRes_tcr1, *PGfcL2ExtColTimeRes_tcr1;
#pragma pack()

//ACC L2 TO PM L2 2033
#pragma pack(1)
typedef struct GfcL2ExtColTimeRes_tcr2
{
	char    GfcPieceId[20];
	float   GfcResInterSlabTcr2ColTimAir; 
	float   GfcResInterSlabTcr2ColTimWat; 
	float   GfcResTcr2ColTimAir; 
	float   GfcResTcr2ColTimWat; 
	int     GfcResTcr2ColTimSta; 
	float   GfcResTcr2PresetSpd; 
	int     GfcPltTcr2AccType; 
	int     GfcPltTcr2CoolMode1Pass;       
	float   GfcResTcr2Cr;
	float   GfcResTcr2AveTmp;
	float   GfcResTcr2SufTmp;
	float   Spare[10];
}GfcL2ExtColTimeRes_tcr2, *PGfcL2ExtColTimeRes_tcr2;
#pragma pack()

#pragma pack(1)
typedef struct GfcL2ExtReq
{
	char    GfcPieceId[20];
	char	GfcPieceidSpare[20];
	char	GfcPltMatId[16];
	char	GfcPltMatIdSpare[16];
	char	GfcPltmatCode[32];
	float	Spare[10];
}GfcL2ExtReq, *PGfcL2ExtReq;
#pragma pack()

#pragma pack(1)
enum SenderType{
	enum_GfcL2PresetAccResult_LP =2011, 
	enum_GfcL2PresetAccResult_tcr1 = 2012,
	enum_GfcL2PresetAccResult_tcr2	= 2013,
	enum_GfcL2ExtAccResult_LP = 2021,
	enum_GfcL2ExtAccResult_tcr1 = 2022, 
	enum_GfcL2ExtAccResult_tcr2 = 2023,
	enum_GfcL2ExtColTimeRes_LP= 2031,
	enum_GfcL2ExtColTimeRes_tcr1 = 2032, 
	enum_GfcL2ExtColTimeRes_tcr2 = 2033,
	enum_GfcL2ExtReq = 205
};
#pragma pack()

typedef struct tagSenderPackage
{
	HEADER header;
	union{
		GfcL2PresetAccResult_LP parlp;
		GfcL2PresetAccResult_tcr1 partcr1;
		GfcL2PresetAccResult_tcr2 partcr2;
		GfcL2ExtAccResult_LP earlp;
		GfcL2ExtAccResult_tcr1 eartcr1;
		GfcL2ExtAccResult_tcr2 eartcr2;
		GfcL2ExtColTimeRes_LP ectlp;
		GfcL2ExtColTimeRes_tcr1 ecttcr1;
		GfcL2ExtColTimeRes_tcr2 ecttcr2;
		GfcL2ExtReq er;
	};
	int nType;
}SENDPACKAGE, *PSENDPACKAGE;
#pragma pack()



#pragma pack(1)
typedef struct SocketMessage
{
	GfcEXTL2PdiData recive101;
	GfcEXTL2RealPdiData recive102;
	GfcL2PresetAccResult_LP send2011;
	GfcL2PresetAccResult_tcr1 send2012;
	GfcL2PresetAccResult_tcr2 send2013;
	GfcL2ExtAccResult_LP send2021;
	GfcL2ExtAccResult_tcr1 send2022;
	GfcL2ExtAccResult_tcr2 send2023;
	GfcL2ExtColTimeRes_LP send2031;
	GfcL2ExtColTimeRes_tcr1 send2032;
	GfcL2ExtColTimeRes_tcr2 send2033;
}SocketMessageDefine;
#pragma pack()


#endif