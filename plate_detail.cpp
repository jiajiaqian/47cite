#include "stdafx.h"
#include "Plate_Detail.h"

//���캯������������
PlateDetail::PlateDetail()
{
}
PlateDetail::~PlateDetail()
{
}
void PlateDetail::Draw(Graphics* graphics,int _startX, int _startY)
{
	pPosition.X=_startX;
	pPosition.Y=_startY;
	Pen pen(&SolidBrush(Color::Black));
	SolidBrush brush(Color(255,0,0,255));
	FontFamily fontFamily(L"����");
	Gdiplus::Font font(&fontFamily,14,FontStyleRegular,UnitPixel);
	if(plate_inf)
	{
		//�����
		PointF pPosition1;
		PointF pPosition2;
		pPosition1.X=_startX+100;
		pPosition1.Y=_startY;
		pPosition2.X=pPosition1.X+300;//5�У�ÿ��50
		pPosition2.Y=pPosition1.Y;//5�У�ÿ��50
		Pen line_pen(Color::Blue,1);
		for(int i=0;i<31;i++)
		{
			graphics->DrawLine(&line_pen,pPosition1,pPosition2);
			pPosition1.Y=pPosition1.Y+20;//
			pPosition2.Y=pPosition2.Y+20;//
		}
		graphics->DrawLine(&line_pen,pPosition1,pPosition2);

		pPosition1.X=_startX+100;
		pPosition1.Y=_startY;
		pPosition2.X=pPosition1.X;//5�У�ÿ��50
		pPosition2.Y=pPosition1.Y+620;//5�У�ÿ��50
		graphics->DrawLine(&line_pen,pPosition1,pPosition2);
		pPosition2.Y=pPosition1.Y+620;
		pPosition1.X=pPosition1.X+100;//
		pPosition2.X=pPosition2.X+100;//
		graphics->DrawLine(&line_pen,pPosition1,pPosition2);
		pPosition1.X=pPosition1.X+150;//
		pPosition2.X=pPosition2.X+150;//
		graphics->DrawLine(&line_pen,pPosition1,pPosition2);
		pPosition1.X=pPosition1.X+50;//
		pPosition2.X=pPosition2.X+50;//
		graphics->DrawLine(&line_pen,pPosition1,pPosition2);

		//��Ŀ
		graphics->DrawString(L"��Ŀ",-1,&font,PointF(_startX+100, _startY+5),&brush);//������
		graphics->DrawString(L"������",-1,&font,PointF(_startX+100, _startY+25),&brush);//������
		graphics->DrawString(L"���ϳ�",-1,&font,PointF(_startX+100, _startY+45),&brush);//������
		graphics->DrawString(L"���Ͽ�",-1,&font,PointF(_startX+100, _startY+65),&brush);//������
		graphics->DrawString(L"���Ϻ�",-1,&font,PointF(_startX+100, _startY+85),&brush);//������
		graphics->DrawString(L"����",-1,&font,PointF(_startX+100, _startY+105),&brush);//������
		graphics->DrawString(L"��Ʒ��",-1,&font,PointF(_startX+100, _startY+125),&brush);//������
		graphics->DrawString(L"��Ʒ��",-1,&font,PointF(_startX+100, _startY+145),&brush);//������
		graphics->DrawString(L"��Ʒ��",-1,&font,PointF(_startX+100, _startY+165),&brush);//������
		graphics->DrawString(L"����1��",-1,&font,PointF(_startX+100, _startY+185),&brush);//������
		graphics->DrawString(L"����1��",-1,&font,PointF(_startX+100, _startY+205),&brush);//������
		graphics->DrawString(L"����1��",-1,&font,PointF(_startX+100, _startY+225),&brush);//������
		graphics->DrawString(L"����2��",-1,&font,PointF(_startX+100, _startY+245),&brush);//������
		graphics->DrawString(L"����2��",-1,&font,PointF(_startX+100, _startY+265),&brush);//������
		graphics->DrawString(L"����2��",-1,&font,PointF(_startX+100, _startY+285),&brush);//������
		graphics->DrawString(L"��ǰ����",-1,&font,PointF(_startX+100, _startY+305),&brush);//������
		graphics->DrawString(L"����1������",-1,&font,PointF(_startX+100, _startY+325),&brush);//������
		graphics->DrawString(L"����2������",-1,&font,PointF(_startX+100, _startY+345),&brush);//������
		graphics->DrawString(L"���ʱ��",-1,&font,PointF(_startX+100, _startY+365),&brush);//������
		graphics->DrawString(L"����1ʱ��",-1,&font,PointF(_startX+100, _startY+385),&brush);//������
		graphics->DrawString(L"����2ʱ��",-1,&font,PointF(_startX+100, _startY+405),&brush);//������
		graphics->DrawString(L"ACCģʽ",-1,&font,PointF(_startX+100, _startY+425),&brush);//������

		//
		graphics->DrawString(L"ֵ",-1,&font,PointF(_startX+200, _startY+5),&brush);//������
		graphics->DrawString(L"��λ",-1,&font,PointF(_startX+350, _startY+5),&brush);//������
	}
	//if (!hActive)
 //       return;
	////��һ������
	//graphics->FillEllipse(&SolidBrush(Color::OrangeRed),pPosition.X,pPosition.Y,iDiameter,iDiameter);

	/*  Rect rt1(54, 107, 92, 77);
	  Rect rt2(168, 16, 92, 117);
	  Rect rt3(116, 70, 632, 150);

	  Region reg;
	  reg.MakeEmpty();

	  reg.Union(rt1);
	  reg.Union(rt2);
	  reg.Union(rt3);

	  Graphics g(pDC->GetSafeHdc());

	  SolidBrush brush(Color(128, 255, 0, 0));

	  g.FillRegion(&brush, &reg);*/
	//if(pRgn->IsVisible(point.x, point.y)) MessageBox(L"�����������");

}

