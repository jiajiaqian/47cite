

#pragma once
#include "PCFrameWork.h"
/////////////////������ʱδʹ��
class Parameter
{
public:
	Parameter();
	~Parameter();
public:	
	Point pPosition;
	void Draw(Graphics* _g,int _startX,int _startY);//����
};