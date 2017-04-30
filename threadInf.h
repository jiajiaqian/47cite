

#pragma once
#include "PCFrameWork.h"
class ThreadInf
{
public:
	ThreadInf(void);
	~ThreadInf(void);
public:
	bool request_inf;//是否显示任务请求字信息
	int  thread_ID;//显示任务请求字信息时所需要的线程ID
	Point pPosition;
	void Draw(Graphics* _g,int _startX, int _startY);//绘制
	void SetButton(Graphics* graphics,int buttonIndex,Region *threadRegion);
	void SetStatus(Graphics* graphics);
	
};