

#pragma once
#include "PCFrameWork.h"
class PlateDetail
{
public:
	PlateDetail();
	~PlateDetail();
public:	
	bool plate_inf;//�Ƿ���ʾ�ְ���Ϣ
	Point pPosition;
	void Draw(Graphics* _g,int _startX,int _startY);//����
};