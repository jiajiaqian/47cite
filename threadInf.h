

#pragma once
#include "PCFrameWork.h"
class ThreadInf
{
public:
	ThreadInf(void);
	~ThreadInf(void);
public:
	bool request_inf;//�Ƿ���ʾ������������Ϣ
	int  thread_ID;//��ʾ������������Ϣʱ����Ҫ���߳�ID
	Point pPosition;
	void Draw(Graphics* _g,int _startX, int _startY);//����
	void SetButton(Graphics* graphics,int buttonIndex,Region *threadRegion);
	void SetStatus(Graphics* graphics);
	
};