
#include "stdafx.h"
#include "background.h"
#include <atlbase.h>

//构造函数和析构函数
Background::Background(void)
{
	m_pImgTitlebk = NULL;
	ImageFromIDResource(IDB_BK, "png", m_pImgTitlebk);
}
Background::~Background(void)
{
	if(m_pImgTitlebk != NULL)
		delete m_pImgTitlebk;
}
void Background::Draw(Graphics* graphics, int width, int height)//50,800
{
	RectF rectDest(0,0,width,height);
	graphics->DrawImage(m_pImgTitlebk,rectDest,0,0,width, height, Gdiplus::Unit::UnitPixel);
}

BOOL Background::ImageFromIDResource(UINT nID, LPCTSTR sTR, Image* &pImg)  
{  
 	HINSTANCE hInst = AfxGetResourceHandle();  
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type  
	if (!hRsrc)  
		return FALSE;  
	// load resource into memory  
	DWORD len = SizeofResource(hInst, hRsrc);  
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);  
	if (!lpRsrc)  
		return FALSE;  
	// Allocate global memory on which to create stream  
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);  
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);  
	memcpy(pmem,lpRsrc,len);  
	IStream* pstm;  
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);  
	// load from stream  
//	pImg=Gdiplus::Image::FromStream(pstm);  
	pImg = new Image(pstm);

	// free/release stuff  
	GlobalUnlock(m_hMem);  
	pstm->Release();  
	FreeResource(lpRsrc);  
	return TRUE;  
} 

void Background::DrawTime(Graphics* _g, int nX, int nY)
{
	Gdiplus::Font font(L"微软雅黑",16);
	SolidBrush brush0(Color::Black);
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	USES_CONVERSION;
	CString strTimer;
	strTimer.Format("%02d年%02d月%02d日 %02d:%02d:%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	Gdiplus::StringFormat sf;
	sf.SetAlignment(Gdiplus::StringAlignmentCenter);
	sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	RectF rectDest(nX-300, nY-50, 300, 50);
	_g->DrawImage(m_pImgTitlebk,rectDest,nX-300,nY-50,300, 50, Gdiplus::Unit::UnitPixel);
	_g->DrawString(A2W(strTimer), -1, &font, PointF(nX-300, nY-50), &brush0);

}
