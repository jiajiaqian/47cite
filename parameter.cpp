#include "stdafx.h"
#include "parameter.h"

//���캯������������
Parameter::Parameter()
{
}
Parameter::~Parameter()
{
}
void Parameter::Draw(Graphics* graphics,int _startX, int _startY)
{
	pPosition.X=_startX;
	pPosition.Y=_startY;
	Pen pen(&SolidBrush(Color::Black));
	graphics->FillRectangle(&SolidBrush(Color::LightGray),35,485,250,310);
	graphics->FillRectangle(&SolidBrush(Color::Khaki),30,480,250,310);
	
	
	SolidBrush textBrush(Color::Black);
	Gdiplus::Font font(L"����",14);
	graphics->DrawString(L"�ְ���Ϣ", -1, &font,PointF(65,490), NULL, &textBrush);
	graphics->DrawString(L"ID�ţ�", -1, &font,PointF(30,525), NULL, &textBrush);
	graphics->DrawString(L"���֣�", -1, &font,PointF(30,550), NULL, &textBrush);
	graphics->DrawString(L"��ȣ�", -1, &font,PointF(30,575), NULL, &textBrush);
	graphics->DrawString(L"��ȣ�", -1, &font,PointF(30,600), NULL, &textBrush);
	graphics->DrawString(L"���ȣ�", -1, &font,PointF(30,625), NULL, &textBrush);
	graphics->DrawString(L"Ŀ����ȴ�ٶȣ�", -1, &font,PointF(30,650), NULL, &textBrush);
	graphics->DrawString(L"Ŀ������Temperature��", -1, &font,PointF(30,675), NULL, &textBrush);
	graphics->DrawString(L"Ŀ������Temperature��", -1, &font,PointF(30,700), NULL, &textBrush);

	graphics->FillRectangle(&SolidBrush(Color::LightGray),320,485,250,310);
	graphics->FillRectangle(&SolidBrush(Color::LightBlue),315,480,250,310);
	graphics->DrawString(L"ʵʱ����", -1, &font,PointF(400,490), NULL, &textBrush);

	graphics->FillRectangle(&SolidBrush(Color::LightGray),605,485,250,310);
	graphics->FillRectangle(&SolidBrush(Color::Khaki),600,480,250,310);
	graphics->DrawString(L"ͨѶ״̬", -1, &font,PointF(660,490), NULL, &textBrush);

	
	graphics->FillRectangle(&SolidBrush(Color::LightGray),895,485,250,310);
	graphics->FillRectangle(&SolidBrush(Color::LightBlue),890,480,250,310);
	graphics->DrawString(L"�߳�״̬", -1, &font,PointF(955,490), NULL, &textBrush);

	graphics->FillRectangle(&SolidBrush(Color::LightGray),1185,485,410,310);
	graphics->FillRectangle(&SolidBrush(Color::Khaki),1180,480,410,310);
	graphics->DrawString(L"��Ϣ����", -1, &font,PointF(1330,490), NULL, &textBrush);

	graphics->FillRectangle(&SolidBrush(Color::LightGray),1635,485,250,310);
	graphics->FillRectangle(&SolidBrush(Color::LightBlue),1630,480,250,310);
	graphics->DrawString(L"ϵͳ��Ϣ", -1, &font,PointF(1685,490), NULL, &textBrush);

}


