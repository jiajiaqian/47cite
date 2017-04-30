
// PCFrameWork.h : PCFrameWork 应用程序的主头文件
//
#pragma once
#include "DataProc.h"

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include <GDIplus.h>
using namespace Gdiplus; 
// CPCFrameWorkApp:
// 有关此类的实现，请参阅 PCFrameWork.cpp
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
	ULONG_PTR m_gdiplusToken; // ULONG PTR 为int64 类型

	SysDefine *L2Sys;//系统结构体指针
// 重写
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

	CDataProc  m_dataProc;  // 加入 [11/12/2013 谢谦]

	int timeHour; // add [1/13/2015 qian]
	int turning;  // add [1/13/2015 qian]


// 实现
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
