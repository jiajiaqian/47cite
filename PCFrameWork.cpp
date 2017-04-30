
// PCFrameWork.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "PCFrameWork.h"
#include "MainFrm.h"

#include "PCFrameWorkDoc.h"
#include "PCFrameWorkView.h"
#include "MiniDump.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned __stdcall S7Line1          (void* pv);
unsigned __stdcall SocketLine1      (void* pv);
unsigned __stdcall DBLine1			(void* pv);
unsigned __stdcall Track			(void* pv);
unsigned __stdcall UFCModel			(void* pv);
unsigned __stdcall UFCSignalProcess (void* pv);
unsigned __stdcall UFCDataRecord    (void* pv);
// CPCFrameWorkApp

BEGIN_MESSAGE_MAP(CPCFrameWorkApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CPCFrameWorkApp::OnAppAbout)
	// 基于FILE的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CPCFrameWorkApp 构造

CPCFrameWorkApp::CPCFrameWorkApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("PCFrameWork.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CPCFrameWorkApp 对象

CPCFrameWorkApp theApp;


// CPCFrameWorkApp 初始化

BOOL CPCFrameWorkApp::InitInstance()
{
	MiniDumper *pDumper = new MiniDumper(false);

// 	_CrtDumpMemoryLeaks();
// 	_CrtSetBreakAlloc(486);

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	CWinAppEx::InitInstance();

	//初始化日志输出
	logger::Init();
	string strLog4Path = Global::GetApplicationBasePath();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行FILE的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI FILE选项(包括 MRU)
	
	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);


	// 注册应用程序的文档模板。文档模板
	//创建共享内存
	//SysDefine **tempL2Sys;
	//tempL2Sys=NULL;
	int i;
	L2Sys=NULL;
	createShareMemory(218,"L2Service");
	
	//创建共享内存
	hShareMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,218*1024*1024, (LPCSTR)"L2Service");
	if (hShareMemory == NULL) 
	{
		return false;
	}
	pShareMemory=MapViewOfFile(hShareMemory, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
	L2Sys=(SysDefine *)pShareMemory;
	sock1=(SocketLine1Define *)((char *)(pShareMemory)+29*1024*1024);
	s71=(S7Line1Define *)((char *)(pShareMemory)+30*1024*1024);
	ufcs=(UFCDefine *)((char *)(pShareMemory)+31*1024*1024);

//	L2Sys=*tempL2Sys;
	//设定各个线程的内容
	CString strLogPath = Global::GetApplicationBasePath();
	strLogPath += "L2LOG";
	setSaveDir((LPSTR)strLogPath.GetBuffer());
	strcpy(L2Sys->ProjectName,"NG4700L2");
	L2Sys->ThreadNum=10;
	initSysService();
	SetCameraSize(5,64);
	startSysService();
	L2Sys->thread[TYPE_SOCKETLINE].ThreadID=4;
	strcpy(L2Sys->thread[TYPE_SOCKETLINE].ThreadName,"SocketLine1");
	L2Sys->thread[TYPE_S7LINE].ThreadID=5;
	strcpy(L2Sys->thread[TYPE_S7LINE].ThreadName,"S7Line1");
	L2Sys->thread[TYPE_DBLINE].ThreadID=6;
	strcpy(L2Sys->thread[TYPE_DBLINE].ThreadName,"DBLine1");
	L2Sys->thread[TYPE_TRACK].ThreadID=7;
	strcpy(L2Sys->thread[TYPE_TRACK].ThreadName,"Track");
	L2Sys->thread[TYPE_UFCMODEL].ThreadID=8;
	strcpy(L2Sys->thread[TYPE_UFCMODEL].ThreadName,"UFCModel");
	L2Sys->thread[TYPE_UFCSIGNALPROCESS].ThreadID=9;
	strcpy(L2Sys->thread[TYPE_UFCSIGNALPROCESS].ThreadName,"UFCSignalProcess");
	L2Sys->thread[TYPE_UFCDATARECORD].ThreadID=10;
	strcpy(L2Sys->thread[TYPE_UFCDATARECORD].ThreadName,"UFCDataRecord");
	
	PTHREAD_START TS[7];
	TS[0]=SocketLine1;
	TS[1]=S7Line1;
	TS[2]=DBLine1;
	TS[3]=Track;
	TS[4]=UFCModel;
	TS[5]=UFCSignalProcess;
	TS[6]=UFCDataRecord;
	if(!createThreadEvent())//初始化系统共享内存
		return FALSE;

	//创建调试对话框
	m_DlgTrack.Create(IDD_DLGTRACK);

	for(i=3;i<10;i++)
	{
		CString strInfo;
		strInfo.Format("%d\n", i);
		OutputDebugString(strInfo);
		beginthread(i+1,TS[i-3]);
//		WaitForSingleObject(L2Sys->thread[i].ThreadEvent[STATE_RUN], INFINITE);
//		SetEvent(L2Sys->thread[i].ThreadEvent[STATE_RUN]);
//		Sleep(100);
	}

	//指向共享内存的结构体指针


	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPCFrameWorkDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CPCFrameWorkView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	ufcs->waterFactorManual=0.0;
	ufcs->HeatCoefManual=1.0;


	// 分析标准 shell 命令、DDE、打开FILE操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	
	CString passLocation="d:\\工艺规程文件";
	m_dataProc.InitData(passLocation);
	logger::LogDebug("system","更新sch文件");

	ufcs->trk.learnFlg=1; // 初始化 开启自学习 [11/30/2013 谢谦]


	return TRUE;
}

int CPCFrameWorkApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	int i;

//	ExitRecordProcess();

	CString strInfo;
	for(i=0;i<10;i++)
	{	
		SetEvent(L2Sys->thread[i].ThreadEvent[STATE_STOP]);
		Sleep(200);
		WaitForSingleObject(L2Sys->thread[i].ThreadEvent[STATE_STOP], INFINITE);

		strInfo.Format("Close %d \n", i);
		OutputDebugString(strInfo);
	}

	Sleep(2000);
	closeThreadEvent();
	closeShareMemory();
	AfxOleTerm(FALSE);

	OutputDebugString(_T("test\n"));

	GdiplusShutdown(m_gdiplusToken);

	 logger::Uninit();

	//清理错误捕获
	MiniDumper::Release();

	return CWinAppEx::ExitInstance();
}

// CPCFrameWorkApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CPCFrameWorkApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CPCFrameWorkApp 自定义加载/保存方法

void CPCFrameWorkApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CPCFrameWorkApp::LoadCustomState()
{
}

void CPCFrameWorkApp::SaveCustomState()
{
}

// CPCFrameWorkApp 消息处理程序



