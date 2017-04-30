

#pragma once
#include "PCFrameWork.h"
/////////////////该类暂时未使用
class Parameter
{
public:
	Parameter();
	~Parameter();
public:	
	Point pPosition;
	void Draw(Graphics* _g,int _startX,int _startY);//绘制
};