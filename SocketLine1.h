#ifndef   SOCKETLINE1_H_FILE       //如果没有定义这个宏   
#define  SOCKETLINE1_H_FILE       //定义这个宏

struct SocketLine1Define
{
	short RecieveBufferFlag[8];//接收到的缓冲区索引
	short ProcessRecieveBufferFlag[8];//已经处理的缓冲区索引
	GfcEXTL2PdiData r1[32];
	GfcEXTL2PdiData r1ontime;
	GfcEXTL2PdiData r1Camera[64];
	GfcEXTL2RealPdiData r2[32];
	GfcEXTL2PdiData r2ontime;
	GfcEXTL2RealPdiData r2Camera[64];
	short SendBufferFlag[16];
	short ProcessSendBufferFlag[16];
	GfcL2PresetAccResult_LP s1[32];
	GfcL2PresetAccResult_LP s1Camera[64];
	GfcL2PresetAccResult_tcr1 s2[32];
	GfcL2PresetAccResult_tcr1 s2Camera[64];
	GfcL2PresetAccResult_tcr2 s3[32];
	GfcL2PresetAccResult_tcr2 s3Camera[64];
	GfcL2ExtAccResult_LP s4[32];
	GfcL2ExtAccResult_LP s4Camera[64];
	GfcL2ExtAccResult_tcr1 s5[32];
	GfcL2ExtAccResult_tcr1 s5Camera[64];
	GfcL2ExtAccResult_tcr2 s6[32];
	GfcL2ExtAccResult_tcr2 s6Camera[64];
	GfcL2ExtColTimeRes_LP s7[32];
	GfcL2ExtColTimeRes_LP s7Camera[64];
	GfcL2ExtColTimeRes_tcr1 s8[32];
	GfcL2ExtColTimeRes_tcr1 s8Camera[64];
	GfcL2ExtColTimeRes_tcr2 s9[32];
	GfcL2ExtColTimeRes_tcr2 s9Camera[64];
	GfcL2ExtReq s10[32];
	GfcL2ExtReq s10Camera[64];

	GfcL2PresetAccResult_LP send2011;
	GfcL2PresetAccResult_tcr1 send2012;
	GfcL2PresetAccResult_tcr2 send2013;
	GfcL2ExtAccResult_LP send2021;
	GfcL2ExtAccResult_tcr1 send2022;
	GfcL2ExtAccResult_tcr2 send2023;
	GfcL2ExtColTimeRes_LP send2031;
	GfcL2ExtColTimeRes_tcr1 send2032;
	GfcL2ExtColTimeRes_tcr2 send2033;
};

void ProcessPackage(LPVOID lpInfo);

//ACC L2 TO PM L2 2011
void SendGfcL2PresetAccResult_LP(PGfcL2PresetAccResult_LP pMsg);
//ACC L2 TO PM L2 2012
void SendGfcL2PresetAccResult_tcr1(PGfcL2PresetAccResult_tcr1 pMsg);
//ACC L2 TO PM L2 2013
void SendGfcL2PresetAccResult_tcr2(PGfcL2PresetAccResult_tcr2 pMsg);
//ACC L2 TO PM L2 2021
void SendGfcL2ExtAccResult_LP(PGfcL2ExtAccResult_LP pMsg);
//ACC L2 TO PM L2 2022
void SendGfcL2ExtAccResult_tcr1(PGfcL2ExtAccResult_tcr1 pMsg);
//ACC L2 TO PM L2 2023
void SendGfcL2ExtAccResult_tcr2(PGfcL2ExtAccResult_tcr2 pMsg);

void SendGfcL2ExtColTimeRes_LP(PGfcL2ExtColTimeRes_LP pMsg);
//ACC L2 TO PM L2 2032
void SendGfcL2ExtColTimeRes_tcr1(PGfcL2ExtColTimeRes_tcr1 pMsg);
//ACC L2 TO PM L2 2033
void SendGfcL2ExtColTimeRes_tcr2(PGfcL2ExtColTimeRes_tcr2 pMsg);
//ACC L2 TO PM L2 205
void SendGfcL2ExtReq(PGfcL2ExtReq msg);

#endif