
// PCFrameWork.h : PCFrameWork Ӧ�ó������ͷ�ļ�
//
#pragma once
#include "DataProc.h"

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include <GDIplus.h>
using namespace Gdiplus; 
// CPCFrameWorkApp:
// �йش����ʵ�֣������ PCFrameWork.cpp
//
#include "SAPI_S7.h"
#include "Socket_API.h"
#include "S7Line1.h"
#include "SocketLine1.h"
#include "Track.h"
#include "UFCModel.h"
#include "UFCSignalProcess.h"
#include "DataRecord.h"
#include "BaseCoolStruct.h"
#include "DlgTrack.h"

class CPCFrameWorkApp : public CWinAppEx
{
public:
	CPCFrameWorkApp();
	//
	ULONG_PTR m_gdiplusToken; // ULONG PTR Ϊint64 ����

	SysDefine *L2Sys;//ϵͳ�ṹ��ָ��
// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();


	HANDLE hShareMemory;
	PVOID pShareMemory;

	SocketLine1Define *sock1;
//	SocketMessageDefine *sockNew;
//	SocketMessageDefine *sockNew;
	S7Line1Define *s71;
	UFCDefine *ufcs;

	CDataProc  m_dataProc;  // ���� [11/12/2013 лǫ]

	int timeHour; // add [1/13/2015 qian]
	int turning;  // add [1/13/2015 qian]


// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	CDlgTrack m_DlgTrack;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPCFrameWorkApp theApp;
