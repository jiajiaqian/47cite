

#pragma once
#include "PCFrameWork.h"
class Plate
{
public:
	Plate();
	~Plate();
public:	
	bool plate_inf;//是否显示钢板信息
	Point pPosition;
	void Move();//移动
	void Draw(Graphics* _g,int _startX,int _startY,char *plate_ID,int temp);//绘制
};