
#include "stdafx.h"
#include "threadInf.h"



//���캯������������
ThreadInf::ThreadInf(void)
{
}
ThreadInf::~ThreadInf(void)
{
}
void ThreadInf::Draw(Graphics* graphics,int _startX, int _startY)//10,100
{
	//�Ȼ�һ���
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp*)AfxGetApp();
	graphics->FillRectangle(&SolidBrush(Color::LightGray),15,105,165,360);//����
	graphics->FillRectangle(&SolidBrush(Color::LightBlue),10,100,165,360);//����
	pPosition.X=_startX;
	pPosition.Y=_startY;
	Pen pen(&SolidBrush(Color::Black));
	//graphics->DrawRectangle(&pen,_startX,_startY,160,360);//��Ⱥ͸߶��ݶ�
	FontFamily fontFamily(L"����");
	FontFamily fontFamily1(L"Times New Roman");
	Gdiplus::Font font(&fontFamily,16,FontStyleBold,UnitPixel);
	Gdiplus::Font font1(&fontFamily1,14,FontStyleRegular,UnitPixel);
	SolidBrush brush(Color(255,0,0,0));
	graphics->DrawString(L"�߳���",-1,&font,PointF(_startX+5, _startY+20),&brush);
	graphics->DrawString(L"״̬",-1,&font,PointF(_startX+125, _startY+20),&brush);
	_startY=_startY+20;
	WCHAR szWchar[20]; 
	
	for(int i=0;i<pApp->L2Sys->ThreadNum;i++)
	{
		MultiByteToWideChar(CP_ACP,   0,   pApp->L2Sys->thread[i].ThreadName,   -1,   szWchar,   sizeof(szWchar)); 
		graphics->DrawString(szWchar,-1,&font1,PointF(_startX+5, _startY+25*i+40),&brush);
 		graphics->DrawRectangle(&pen,_startX+135,_startY+25*i+40,12,12);//��Ⱥ͸߶��ݶ�
	}
}
void ThreadInf::SetStatus(Graphics* graphics)
{
	CPCFrameWorkApp * pApp;

	pApp=(CPCFrameWorkApp *)AfxGetApp();
	int i;
	for(i=0;i<pApp->L2Sys->ThreadNum;i++)
	{
		switch(pApp->L2Sys->thread[i].status)
		{
		case 0:
			graphics->FillRectangle(&SolidBrush(Color::Red),pPosition.X+135,pPosition.Y+25*i+60,12,12);//��Ⱥ͸߶��ݶ�
			break;
		case 1:
			graphics->FillRectangle(&SolidBrush(Color::Green),pPosition.X+135,pPosition.Y+25*i+60,12,12);//��Ⱥ͸߶��ݶ�
			break;
		case 2:
			graphics->FillRectangle(&SolidBrush(Color::Blue),pPosition.X+135,pPosition.Y+25*i+60,12,12);//��Ⱥ͸߶��ݶ�
			break;
		default:
			break;
			
		}
	}
}
void ThreadInf::SetButton(Graphics* graphics,int buttonIndex,Region *threadRegion)
{
	//graphics->threadRegion
	//graphics->DrawRectangle(&pen,pPosition.X+135,pPosition.Y+20*(buttonIndex-1)+5,12,12);//��Ⱥ͸߶��ݶ�
}
