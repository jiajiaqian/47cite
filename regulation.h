

#pragma once
#include "PCFrameWork.h"
class Regulation
{
public:
	Regulation();
	~Regulation();
public:	
	Point pPosition;
	void Draw(Graphics* _g,int _startX,int _startY);//»æÖÆ
	void SetPlateInf(Graphics* graphics);

	CSize GetRealPos(int w, int h, int x, int y);

	BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Image* &pImg);

	Image *m_pImgStrp;
};