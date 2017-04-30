#ifndef   TRACK_H_FILE       //如果没有定义这个宏   
#define  TRACK_H_FILE       //定义这个宏

//////////////////////////////////////////////////////////
//extern bool SelfLearnChoose;
extern bool SendDataToHMI;
extern bool offLineSelfLearn;
extern int autozip_flag;
extern int     mode_tag;    //model return

#include "PCFramework.h"
#include "ufcsource/Infoinit.h"
#include "ufcsource/public.H"

void AlarmPrint(int error_flg,char *message, double par1,double par2);
void Auto_ModelPreCal(); 
void AutoCalcTimeInFurnance();
void Auto_ModelFrtModiCal();
void Auto_TrackPltEnterCZone();
void Auto_TrackPltOutCZone();
void Auto_TrackEndTreat();
void ProcessReset();
void AccidentDispose();

char* removespace(char *text);

void SendScheduleToPLC(float sideWater);
void SendScheduleToPLCDefult();

void TRK_LPT_HMI_ACC();
void TRK_HMI_NOACC();
void TRK_LPT_HMI_EXP();
void DisplayData();

void SendSpeedToPLC() ;

void SendScheduleToPLCTrigger();
void SendSpeedToPLCTrigger();
void SendPDIToPLTrigger();
float GetAlphaCoeWatTemp(float fWatTempInput);
void ProcessReset();

void PDI_Prepare(PGfcEXTL2PdiData pInfo);

void ReadMeaDataFile(TRACKDATARAL *trackPreRal);

void DataRealRecord();

void Auto_SendPdiToPLCWithParam(PGfcEXTL2PdiData pInfo);

void Auto_SendPdiToPLC();

bool TransMESDateBase(PRERESULT *PreResult);
bool TransMESDateBaseTable2(PRERESULT *PreResult);

void ManualChangeAcce();

void WaterRatioStudy(PRERESULT preResult); // add [4/16/2014 谢谦]

#endif