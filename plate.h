

#pragma once
#include "PCFrameWork.h"
class Plate
{
public:
	Plate();
	~Plate();
public:	
	bool plate_inf;//�Ƿ���ʾ�ְ���Ϣ
	Point pPosition;
	void Move();//�ƶ�
	void Draw(Graphics* _g,int _startX,int _startY,char *plate_ID,int temp);//����
};