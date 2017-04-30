

#pragma once
#include "PCFrameWork.h"
class PlateDetail
{
public:
	PlateDetail();
	~PlateDetail();
public:	
	bool plate_inf;//是否显示钢板信息
	Point pPosition;
	void Draw(Graphics* _g,int _startX,int _startY);//绘制
};