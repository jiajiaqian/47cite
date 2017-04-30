
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "PCFrameWork.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

 // CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_MESSAGE(WM_UPDATEOUTPUT, &CMainFrame::OnUpdateoutputSystem)
	ON_MESSAGE(WM_UPDATEOUTPUTDB, &CMainFrame::OnUpdateoutputDBLine)

	ON_COMMAND(ID_MENU_SOCKET_CHECK, &CMainFrame::OnMenuSocketCheck)
	ON_COMMAND(ID_MENU_S7_CHECK, &CMainFrame::OnMenuS7Check)
	ON_COMMAND(ID_MODAO, &CMainFrame::OnModao)
	ON_COMMAND(ID_JINCHAO, &CMainFrame::OnJinchao)
	ON_COMMAND(ID_CHUCHAO, &CMainFrame::OnChuchao)
	ON_COMMAND(ID_FANHONG, &CMainFrame::OnFanhong)
	ON_COMMAND(ID_32803, &CMainFrame::onClearL2Track)
	ON_COMMAND(ID_32808, &CMainFrame::onUpDateSchFile)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFrameWndEx::EnableLoadDockState(FALSE);

	BOOL bNameValid;
	// 基于持久value设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);


	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}
	m_wndOutput.EnableDocking(CBRS_ALIGN_BOTTOM);
	DockPane(&m_wndOutput);



	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}
	m_wndOutput.ShowWindow(SW_SHOW);
	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}

LRESULT CMainFrame::OnUpdateoutputSystem(WPARAM wParam, LPARAM lParam)
{
	CString* pStr = (CString*)lParam;
	m_wndOutput.upDateOutPutWnd(pStr);
	return 0;
}

LRESULT CMainFrame::OnUpdateoutputDBLine(WPARAM wParam, LPARAM lParam)
{
	CString* pStr = (CString*)lParam;
	m_wndOutput.upDateOutPutWndDBLine(pStr);
	return 0;
}


/// @函数说明 Socket监控
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 8/5/2013
void CMainFrame::OnMenuSocketCheck()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	setTRB(4,32);
	SetEvent(pApp->L2Sys->thread[TYPE_SOCKETLINE].ThreadEvent[STATE_RUN]);
}


/// @函数说明 S7通讯监控
/// @参数 
/// @参数 c2
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 8/5/2013
void CMainFrame::OnMenuS7Check()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	setTRB(5,32);
	SetEvent(pApp->L2Sys->thread[TYPE_S7LINE].ThreadEvent[STATE_RUN]);
}


void CMainFrame::OnModao()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;

	 logger::LogDebug("system","UFCSignalProcess.cpp,TMEICToUFCL2ID1SignalProcess,手动末道次信号");

	//strcpy_s(ufcs->CSpdiAdcos,"insert into realpdi (PLATE_NUMBER) value ('2013-6-30')");
	//strcpy_s(ufcs->CSpdiAdcos,"select * from NG4700MES.GP_DQACCUTLIFW");
	//setTRB(6,1);//末3道次抛钢信号,触发预计算
	//SetEvent(theApp->L2Sys->thread[TYPE_DBLINE].ThreadEvent[STATE_RUN]);   // by Zed
	setTRB(7,1);//末3道次抛钢信号,触发预计算
	SetEvent(theApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);


}


void CMainFrame::OnJinchao()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;

	logger::LogDebug("system","UFCSignalProcess.cpp,TMEICToUFCL2ID1SignalProcess,手动进入超快冷信号");
	setTRB(7,3);//末3道次抛钢信号,触发预计算
	SetEvent(theApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);
}


void CMainFrame::OnChuchao()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;
	
	logger::LogDebug("system","UFCSignalProcess.cpp,TMEICToUFCL2ID1SignalProcess,手动出超快冷信号");

	setTRB(7,7);//末3道次抛钢信号,触发预计算
	SetEvent(theApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);
}


void CMainFrame::OnFanhong()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	UFCDefine *ufcs=theApp->ufcs; 
	S7Line1Define *s71=theApp->s71;

	logger::LogDebug("system","UFCSignalProcess.cpp,TMEICToUFCL2ID1SignalProcess,手动返红信号");
	/*strcpy_s(ufcs->m_coolStruct.TrkMes[0].plateID,"2013-8-16a");

	int i = 0;
	FILE *fpCool;
	char buff[100];
	char temp[PATH_MAX];
	char line[PATH_MAX];
	int  max_in = PATH_MAX - 1;

	sprintf(buff,"d:\\record_file\\2013-8-16a.txt");
	fpCool=fopen(buff,"r");
	if(fpCool==NULL)
		return;

	fgets(line, max_in, fpCool); 
	if (feof(fpCool)){
		fclose(fpCool);
		return;
	}	

	if (!strcmp(line,"")){
		fclose(fpCool);
		return;
	}
	else
		sscanf(line,"%s", &temp);

	fgets(line, max_in, fpCool); 
	sscanf(line,"%s%s%s%s%s%s%s%s%s%s%s%s",&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp,&temp);

	while ((!feof(fpCool)) && (i<MAX_MEASURE_VALUE-1)){
		fgets(line, max_in, fpCool); 
		sscanf(line,"%d%f%f%f%f%f%f%f%f%f%f%f",&ufcs->m_coolStruct.TrkMes[0].CurIndex[i],
			&ufcs->m_coolStruct.TrkMes[0].TimeSinceP1[i],
			&ufcs->m_coolStruct.TrkMes[0].Position[i],
			&ufcs->m_coolStruct.TrkMes[0].Speed[i],
			&ufcs->m_coolStruct.TrkMes[0].TempT0[i],
			&ufcs->m_coolStruct.TrkMes[0].TempTH1[i],
			&ufcs->m_coolStruct.TrkMes[0].TempT1[i],
			&ufcs->m_coolStruct.TrkMes[0].TempTH2[i],
			&ufcs->m_coolStruct.TrkMes[0].TempT2[i],
			&ufcs->m_coolStruct.TrkMes[0].TempT3[i],
			&ufcs->m_coolStruct.TrkMes[0].TempT4[i],
			&ufcs->m_coolStruct.TrkMes[0].TempTH2[i]
		);
		i ++ ;
	}

	ufcs->m_coolStruct.TrkMes[0].number=i;
	fclose(fpCool);*/
	
	setTRB(7,4);//末3道次抛钢信号,触发预计算
	SetEvent(theApp->L2Sys->thread[TYPE_TRACK].ThreadEvent[STATE_RUN]);



}


void CMainFrame::onClearL2Track()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	UFCDefine *ufcs=theApp->ufcs; 

	logger::LogDebug("system","清空L2跟踪 ");

	for(int i=0;i<AFT_COOL_QUE_NUM;i++)
	{
		memset(&ufcs->AFT_FINISH_ROLL[i],0,sizeof(TRACKDATARAL ));
	}

	for(int i=0;i<UND_COOL_QUE_NUM;i++)
	{
		memset(&ufcs->UND_LAMINAR_COOL[i],0,sizeof(TRACKDATARAL ));
	}
	for(int i=0;i<AFT_COOL_QUE_NUM;i++)
	{
		memset(&ufcs->AFT_LAMINAR_COOL[i],0,sizeof(TRACKDATARAL ));
	}

 }


void CMainFrame::onUpDateSchFile()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	CString passLocation="d:\\工艺规程文件";
	theApp->m_dataProc.InitData(passLocation);
	logger::LogDebug("system","更新sch文件");

}