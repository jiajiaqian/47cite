

#pragma once
#include "PCFrameWork.h"
class Background
{
public:
	Background(void);
	~Background(void);
public:
	void Draw(Graphics* _g, int width, int height);//»æÖÆ


	void Background::DrawTime(Graphics* _g, int nX, int nY);
	BOOL ImageFromIDResource(UINT nID, LPCTSTR sTR, Image *&pImg);

	Image *m_pImgTitle;
	Image *m_pImgTitlebk;
};