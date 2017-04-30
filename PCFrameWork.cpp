
// PCFrameWork.cpp : ����Ӧ�ó��������Ϊ��
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
	// ����FILE�ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CPCFrameWorkApp ����

CPCFrameWorkApp::CPCFrameWorkApp()
{
	m_bHiColorIcons = TRUE;

	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("PCFrameWork.AppID.NoVersion"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CPCFrameWorkApp ����

CPCFrameWorkApp theApp;


// CPCFrameWorkApp ��ʼ��

BOOL CPCFrameWorkApp::InitInstance()
{
	MiniDumper *pDumper = new MiniDumper(false);

// 	_CrtDumpMemoryLeaks();
// 	_CrtSetBreakAlloc(486);

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	CWinAppEx::InitInstance();

	//��ʼ����־���
	logger::Init();
	string strLog4Path = Global::GetApplicationBasePath();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ��FILE�Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI FILEѡ��(���� MRU)
	
	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);


	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	//���������ڴ�
	//SysDefine **tempL2Sys;
	//tempL2Sys=NULL;
	int i;
	L2Sys=NULL;
	createShareMemory(218,"L2Service");
	
	//���������ڴ�
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
	//�趨�����̵߳�����
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
	if(!createThreadEvent())//��ʼ��ϵͳ�����ڴ�
		return FALSE;

	//�������ԶԻ���
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

	//ָ�����ڴ�Ľṹ��ָ��


	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPCFrameWorkDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CPCFrameWorkView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	ufcs->waterFactorManual=0.0;
	ufcs->HeatCoefManual=1.0;


	// ������׼ shell ���DDE����FILE������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	
	CString passLocation="d:\\���չ���ļ�";
	m_dataProc.InitData(passLocation);
	logger::LogDebug("system","����sch�ļ�");

	ufcs->trk.learnFlg=1; // ��ʼ�� ������ѧϰ [11/30/2013 лǫ]


	return TRUE;
}

int CPCFrameWorkApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
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

	//������󲶻�
	MiniDumper::Release();

	return CWinAppEx::ExitInstance();
}

// CPCFrameWorkApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CPCFrameWorkApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CPCFrameWorkApp �Զ������/���淽��

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

// CPCFrameWorkApp ��Ϣ�������



