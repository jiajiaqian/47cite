#include "stdafx.h"
#include "regulation.h"
#include <atlbase.h>

//相对于1916*1011的坐标值
#define  TAB_NUM		0
#define  TAB_ID			1
#define  TAB_POS		2
#define  TAB_STATE	3

#define  PART_NORMAL 52

#define  TAB_ROW_1		0
#define  TAB_ROW_1P	1
#define  TAB_ROW_2		2
#define  TAB_ROW_2P	3
#define  TAB_ROW_3		4
#define  TAB_ROW_3P	5
#define  TAB_ROW_4		6
#define  TAB_ROW_4P	7
#define  TAB_ROW_5		8
#define  TAB_ROW_5P	9

#define TAB_NUM_W 50
#define  TAB_ID_W 130
#define  TAB_POS_W 70
#define  TAB_STATE_W 111
#define  TAB_NORMAL_W 52

#define  TAB_ROW_H 28

int table_Col[] = {50,100,229,300,410,516,620,723, 827, 931, 1034,1138,1242,1345,1449,1553,1655,1760};

int table_row[] = {168,228,288,348,408};

//冷却类型
WCHAR chCoolType[][20] = {L"空冷", L"IC1水冷", L"IC2水冷", L"IC1&IC2水冷"};

//构造函数和析构函数
Regulation::Regulation()
{
	ImageFromIDResource(IDB_STRIP, "png", m_pImgStrp);
	
}
Regulation::~Regulation()
{
}
void Regulation::Draw(Graphics* graphics,int _startX, int _startY)//90,100
{
}

CSize Regulation::GetRealPos(int w, int h, int x, int y)
{
	CSize sizeRet;
	double dX = x/1920 * w * 1.0;
	double dY = y/1920 * h * 1.0;

	sizeRet.cx = (int)dX;
	sizeRet.cy = (int)dY;
	
	return sizeRet;
}

//填充表格数据
void Regulation::SetPlateInf(Graphics* graphics)
{
	USES_CONVERSION;

	//首先写入的是索引号
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	char temp[10];
	FontFamily fontFamily(L"微软雅黑");
	Pen pen(&SolidBrush(Color::Black));
	Gdiplus::Font font(&fontFamily,16,FontStyleRegular,UnitPixel);
	SolidBrush brush(Color::Navy);
	//刷新规程信息
	UFCDefine *ufcs;
	ufcs=pApp->ufcs;
	
	
	//graphics->DrawLine(&pen,2,2,10,10);

	Gdiplus::StringFormat sf;
	sf.SetAlignment(Gdiplus::StringAlignmentCenter);
	sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	int i=0;

	//RectF rectID(100,500, TAB_ID_W, TAB_ROW_H*2);
	//if(strcmp(ufcs->FINISH_ROLL[0].RalPdi.plateID,"")!=0)
	//{
	//graphics->DrawString(A2W( ufcs->FINISH_ROLL[0].RalPdi.plateID),-1,&font, rectID, &sf, &brush);//板坯号
	//}

	for (i=0;i<4;i++)  // add [12/23/2013 谢谦]
	{
		if(strcmp(ufcs->FINISH_ROLL[i].RalPdi.plateID,"")!=0){
			RectF rectID(100+20,500+TAB_ROW_H*i, TAB_ID_W, TAB_ROW_H*2);
			graphics->DrawString(A2W( ufcs->FINISH_ROLL[i].RalPdi.plateID),-1,&font, rectID, &sf, &brush);//板坯号
		}

	}
	

	for (i=0;i<3;i++)
	{
		if(strcmp(ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID,"")!=0){
			RectF rectID(100+TAB_ID_W+20,500+TAB_ROW_H*i, TAB_ID_W, TAB_ROW_H*2);
			graphics->DrawString(A2W( ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID),-1,&font, rectID, &sf, &brush);//板坯号
		}

	}
	
	for (i=0;i<3; i++)
	{
		if(strcmp(ufcs->UND_LAMINAR_COOL[i].RalPdi.plateID,"")!=0){
					RectF rectID(100+TAB_ID_W*2+20,500+TAB_ROW_H*i, TAB_ID_W, TAB_ROW_H*2);
					graphics->DrawString(A2W( ufcs->UND_LAMINAR_COOL[i].RalPdi.plateID),-1,&font, rectID, &sf, &brush);//板坯号
		}

	}

	for (i=0;i<3;i++)
	{
		if(strcmp(ufcs->UND_LAMINAR_COOL[i].RalPdi.plateID,"")!=0){
			RectF rectID(100+TAB_ID_W*3+20,500+TAB_ROW_H*i, TAB_ID_W, TAB_ROW_H*2);
			graphics->DrawString(A2W( ufcs->AFT_LAMINAR_COOL[i].RalPdi.plateID),-1,&font, rectID, &sf, &brush);//板坯号
		}
	}
	
	int plate_num = 0;
	/*TRACKDATARAL FINISH_ROLL[FIN_QUE_NUM];	TRACKDATARAL AFT_FINISH_ROLL[AFT_FIN_QUE_NUM];	TRACKDATARAL UND_LAMINAR_COOL[UND_COOL_QUE_NUM];	TRACKDATARAL AFT_LAMINAR_COOL[AFT_COOL_QUE_NUM];*/


	wchar_t szNum[10];
	for(int i=0;i<1;i++){
		ufcs->FINISH_ROLL[0].RalPdi.PltIcFlg[0] = '0';
		if(strcmp(ufcs->FINISH_ROLL[0].RalPdi.plateID,"")!=0){

			swprintf(szNum, L"%d", plate_num);
			RectF rectNum(table_Col[TAB_NUM], table_row[plate_num], TAB_NUM_W, TAB_ROW_H*2);
			graphics->DrawString(szNum,-1,&font,  rectNum, &sf, &brush);//序号
			RectF rectID(table_Col[TAB_ID],table_row[plate_num], TAB_ID_W, TAB_ROW_H*2);
			graphics->DrawString(A2W( ufcs->FINISH_ROLL[0].RalPdi.plateID),-1,&font, rectID, &sf, &brush);//板坯号
			RectF rectPOS(table_Col[TAB_POS],table_row[plate_num], TAB_POS_W, TAB_ROW_H*2);
			graphics->DrawString(L"轧前",-1,&font, rectPOS, &sf, &brush);
			RectF rectState(table_Col[TAB_STATE],table_row[plate_num], TAB_STATE_W, TAB_ROW_H*2);
		//	graphics->DrawString(chCoolType[ufcs->AFT_FINISH_ROLL[i].RalPdi.PltIcFlg],-1,&font,rectState, &sf, &brush);
			RectF rectStrip(50,560,200,20);
			graphics->DrawImage(m_pImgStrp, rectStrip);
			graphics->DrawString(A2W(ufcs->FINISH_ROLL[0].RalPdi.plateID),-1,&font,rectStrip, &sf, &brush);//显示 钢板
		}
	}
		
	for(int i=0;i<AFT_FIN_QUE_NUM && plate_num < 5;i++){
		/*int CFlowup[14]={200,200,100,100,200,200,200,200,200,200,200,200,100,100};
		int LFowup[14] = {200,200,100,100,200,200,200,200,200,200,200,200,100,100};
		float Cratio[14]={1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5};
		float Lratio[14]={1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8,1.8};*///test
		int CFlowup[14],LFlowup[14];
		float Cratio[14],Lratio[14];
		for(int h=0;h<14;h++)
		{
			CFlowup[h] = ufcs->scr.setpoints.qtop[h];
			LFlowup[h] = ufcs->scr.setpoints.qtop[h];
			Cratio[h] = ufcs->scr.setpoints.w_ratio[h];
			Lratio[h] = ufcs->scr.setpoints.w_ratio[h];

		}
		
//		ufcs->AFT_FINISH_ROLL[0].RalPdi.PltIcFlg = 1;//test

		if(strcmp(ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID,"")!=0){
			plate_num++;
			itoa(plate_num,temp,10);

			RectF rectNum(table_Col[TAB_NUM], table_row[plate_num], TAB_NUM_W, TAB_ROW_H*2);

			graphics->DrawString(A2W(temp),-1,&font,  rectNum, &sf, &brush);//序号
			RectF rectID(table_Col[TAB_ID],table_row[plate_num], TAB_ID_W, TAB_ROW_H*2);
			graphics->DrawString(A2W( ufcs->FINISH_ROLL[0].RalPdi.plateID),-1,&font, rectID, &sf, &brush);//板坯号
			RectF rectPOS(table_Col[TAB_POS],table_row[plate_num], TAB_POS_W, TAB_ROW_H*2);
			graphics->DrawString(L"轧后",-1,&font, rectPOS, &sf, &brush);
			RectF rectState(table_Col[TAB_STATE],table_row[plate_num], TAB_STATE_W, TAB_ROW_H*2);
			//graphics->DrawString(chCoolType[ufcs->AFT_FINISH_ROLL[i].RalPdi.PltIcFlg],-1,&font,rectState, &sf, &brush);
			
			RectF rectStrip(50,560,200,20);
			graphics->DrawImage(m_pImgStrp, rectStrip);
			graphics->DrawString(A2W(ufcs->AFT_FINISH_ROLL[i].RalPdi.plateID),-1,&font, rectStrip, &sf,&brush);

			wchar_t str[20];
			for (int j=0; j<14; j++){
				swprintf(str, L"%d", CFlowup[j]);				
				RectF rectCF(table_Col[j+4], table_row[plate_num], TAB_NORMAL_W, TAB_ROW_H);
				graphics->DrawString(str,-1,&font, rectCF, &sf, &brush);

				swprintf(str, L"%d", LFlowup[j]);				
				RectF rectLF(table_Col[j+4]+PART_NORMAL, table_row[plate_num], TAB_NORMAL_W, TAB_ROW_H);
				graphics->DrawString(str,-1,&font, rectLF, &sf, &brush);

				swprintf(str, L"%.1f", Cratio[j]);				
				RectF rectCR(table_Col[j+4], table_row[plate_num]+30, TAB_NORMAL_W, TAB_ROW_H);
				graphics->DrawString(str,-1, &font, rectCR, &sf,&brush);

				swprintf(str, L"%.1f", Cratio[j]);				
				RectF rectLR(table_Col[j+4]+PART_NORMAL, table_row[plate_num]+30, TAB_NORMAL_W, TAB_ROW_H);
				graphics->DrawString(str, -1,&font, rectLR,  &sf, &brush);
			}
		}
	}

		for(int j = 0; j < UND_COOL_QUE_NUM && plate_num < 5; j++){
			if(strcmp(ufcs->UND_LAMINAR_COOL[j].RalPdi.plateID,"")!=0){
				plate_num++;
				itoa(plate_num,temp,10);
				RectF rectNum(table_Col[TAB_NUM], table_row[plate_num], TAB_NUM_W, TAB_ROW_H*2);
				graphics->DrawString(A2W(temp),-1,&font, rectNum, &sf, &brush);
				RectF rectID(table_Col[TAB_ID],table_row[plate_num], TAB_ID_W, TAB_ROW_H*2);
				graphics->DrawString(A2W(ufcs->UND_LAMINAR_COOL[j].RalPdi.plateID),-1,&font,rectID, &sf, &brush);
				RectF rectStrip(1180,560 + 60*j,200,20);
				graphics->DrawImage(m_pImgStrp, rectStrip);
				graphics->DrawString(A2W(ufcs->UND_LAMINAR_COOL[j].RalPdi.plateID),-1,&font, rectStrip, &sf,&brush);
			}
		}
	

		for(int j = 0; j < 3 && plate_num < 5; j++){
			if(strcmp(ufcs->AFT_LAMINAR_COOL[j].RalPdi.plateID,"")!=0){
				plate_num++;
				itoa(plate_num,temp,10);
				RectF rectNum(table_Col[TAB_NUM], table_row[plate_num], TAB_NUM_W, TAB_ROW_H*2);
				graphics->DrawString(A2W(temp),-1,&font, rectNum, &sf, &brush);
				RectF rectID(table_Col[TAB_ID],table_row[plate_num], TAB_ID_W, TAB_ROW_H*2);
				graphics->DrawString(A2W(ufcs->AFT_LAMINAR_COOL[j].RalPdi.plateID),-1,&font,rectID, &sf, &brush);
				RectF rectStrip(1550,560 + 60*j,200,20);
				graphics->DrawImage(m_pImgStrp, rectStrip);
				graphics->DrawString(A2W(ufcs->AFT_LAMINAR_COOL[j].RalPdi.plateID),-1,&font,rectStrip, &sf, &brush);
			}
		}
	

}



BOOL Regulation::ImageFromIDResource(UINT nID, LPCTSTR sTR, Image* &pImg)  
{  
	HINSTANCE hInst = AfxGetResourceHandle();  
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type  
	if (!hRsrc)  
		return FALSE;  
	// load resource into memory  
	DWORD len = SizeofResource(hInst, hRsrc);  
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);  
	if (!lpRsrc)  
		return FALSE;  
	// Allocate global memory on which to create stream  
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);  
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);  
	memcpy(pmem,lpRsrc,len);  
	IStream* pstm;  
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);  
	// load from stream  
	//	pImg=Gdiplus::Image::FromStream(pstm);  
	pImg = new Image(pstm);

	// free/release stuff  
	GlobalUnlock(m_hMem);  
	pstm->Release();  
	FreeResource(lpRsrc);  
	return TRUE;  
} 
