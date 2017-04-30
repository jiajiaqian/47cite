#include "stdafx.h"
#include "Plate_Detail.h"

//构造函数和析构函数
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
	FontFamily fontFamily(L"宋体");
	Gdiplus::Font font(&fontFamily,14,FontStyleRegular,UnitPixel);
	if(plate_inf)
	{
		//画表格
		PointF pPosition1;
		PointF pPosition2;
		pPosition1.X=_startX+100;
		pPosition1.Y=_startY;
		pPosition2.X=pPosition1.X+300;//5列，每列50
		pPosition2.Y=pPosition1.Y;//5列，每列50
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
		pPosition2.X=pPosition1.X;//5列，每列50
		pPosition2.Y=pPosition1.Y+620;//5列，每列50
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

		//项目
		graphics->DrawString(L"项目",-1,&font,PointF(_startX+100, _startY+5),&brush);//板坯号
		graphics->DrawString(L"板坯号",-1,&font,PointF(_startX+100, _startY+25),&brush);//板坯号
		graphics->DrawString(L"坯料长",-1,&font,PointF(_startX+100, _startY+45),&brush);//板坯号
		graphics->DrawString(L"坯料宽",-1,&font,PointF(_startX+100, _startY+65),&brush);//板坯号
		graphics->DrawString(L"坯料厚",-1,&font,PointF(_startX+100, _startY+85),&brush);//板坯号
		graphics->DrawString(L"钢种",-1,&font,PointF(_startX+100, _startY+105),&brush);//板坯号
		graphics->DrawString(L"成品长",-1,&font,PointF(_startX+100, _startY+125),&brush);//板坯号
		graphics->DrawString(L"成品宽",-1,&font,PointF(_startX+100, _startY+145),&brush);//板坯号
		graphics->DrawString(L"成品厚",-1,&font,PointF(_startX+100, _startY+165),&brush);//板坯号
		graphics->DrawString(L"控轧1长",-1,&font,PointF(_startX+100, _startY+185),&brush);//板坯号
		graphics->DrawString(L"控轧1宽",-1,&font,PointF(_startX+100, _startY+205),&brush);//板坯号
		graphics->DrawString(L"控轧1厚",-1,&font,PointF(_startX+100, _startY+225),&brush);//板坯号
		graphics->DrawString(L"控轧2长",-1,&font,PointF(_startX+100, _startY+245),&brush);//板坯号
		graphics->DrawString(L"控轧2宽",-1,&font,PointF(_startX+100, _startY+265),&brush);//板坯号
		graphics->DrawString(L"控轧2厚",-1,&font,PointF(_startX+100, _startY+285),&brush);//板坯号
		graphics->DrawString(L"当前道次",-1,&font,PointF(_startX+100, _startY+305),&brush);//板坯号
		graphics->DrawString(L"控轧1道次数",-1,&font,PointF(_startX+100, _startY+325),&brush);//板坯号
		graphics->DrawString(L"控轧2道次数",-1,&font,PointF(_startX+100, _startY+345),&brush);//板坯号
		graphics->DrawString(L"淬火时间",-1,&font,PointF(_startX+100, _startY+365),&brush);//板坯号
		graphics->DrawString(L"控轧1时间",-1,&font,PointF(_startX+100, _startY+385),&brush);//板坯号
		graphics->DrawString(L"控轧2时间",-1,&font,PointF(_startX+100, _startY+405),&brush);//板坯号
		graphics->DrawString(L"ACC模式",-1,&font,PointF(_startX+100, _startY+425),&brush);//板坯号

		//
		graphics->DrawString(L"值",-1,&font,PointF(_startX+200, _startY+5),&brush);//板坯号
		graphics->DrawString(L"单位",-1,&font,PointF(_startX+350, _startY+5),&brush);//板坯号
	}
	//if (!hActive)
 //       return;
	////画一个方块
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
	//if(pRgn->IsVisible(point.x, point.y)) MessageBox(L"鼠标在区域内");

}

