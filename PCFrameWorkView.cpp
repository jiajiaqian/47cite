
// PCFrameWorkView.cpp : CPCFrameWorkView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "PCFrameWork.h"
#endif

#include "PCFrameWorkDoc.h"
#include "PCFrameWorkView.h"
#include "CMemDC_GDI.h"
#include <atlbase.h>
#include<windows.h> 
#include <iostream>
//#include "SocketLine1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPCFrameWorkView

IMPLEMENT_DYNCREATE(CPCFrameWorkView, CView)

BEGIN_MESSAGE_MAP(CPCFrameWorkView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPCFrameWorkView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
//	ON_WM_MOUSEMOVE()
ON_WM_RBUTTONDOWN()
ON_WM_ERASEBKGND()
ON_COMMAND(IDGfcL2PresetAccResult_LP, &CPCFrameWorkView::OnIDGfcL2PresetAccResult_LP)
ON_COMMAND(IDGfcL2PresetAccResult_tcr1, &CPCFrameWorkView::OnIDGfcL2PresetAccResult_tcr1)
ON_COMMAND(IDGfcL2PresetAccResult_tcr2, &CPCFrameWorkView::OnIDGfcL2PresetAccResult_tcr2)
ON_COMMAND(IDGfcL2ExtAccResult_LP, &CPCFrameWorkView::OnIDGfcL2ExtAccResult_LP)
ON_COMMAND(IDGfcL2ExtAccResult_tcr1, &CPCFrameWorkView::OnIDGfcL2ExtAccResult_tcr1)
ON_COMMAND(IDGfcL2ExtAccResult_tcr2, &CPCFrameWorkView::OnIDGfcL2ExtAccResult_tcr2)
ON_COMMAND(IDGfcL2ExtColTimeRes_tcr1, &CPCFrameWorkView::OnIDGfcL2ExtColTimeRes_tcr1)
ON_COMMAND(IDGfcL2ExtColTimeRes_tcr2, &CPCFrameWorkView::OnIDGfcL2ExtColTimeRes_tcr2)
ON_COMMAND(IDShowTrackDialog, &CPCFrameWorkView::OnShowtrackdialog)
ON_COMMAND(IDGfcL2ExtColTimeRes_LP, &CPCFrameWorkView::OnGfcL2ExtColTimeRes_LP)
ON_COMMAND(IDGfcL2ExtReq, &CPCFrameWorkView::OnGfcl2extreq)
ON_COMMAND(ID_GfcEXTL2PdiData, &CPCFrameWorkView::OnGfcextl2pdidata)
ON_COMMAND(ID_GfcEXTL2RealPdiData, &CPCFrameWorkView::OnGfcextl2realpdidata)
END_MESSAGE_MAP()

// CPCFrameWorkView 构造/析构

int errordat =0 ;

CPCFrameWorkView::CPCFrameWorkView()
{
	// TODO: 在此处添加构造代码
}

CPCFrameWorkView::~CPCFrameWorkView()
{
}

BOOL CPCFrameWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CPCFrameWorkView 绘制

void CPCFrameWorkView::OnDraw(CDC* pDC)
{
	CMemDC_GDI pDCGDI(pDC);   
	CPCFrameWorkDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	Graphics graph(pDCGDI->m_hDC);
	back.Draw(&graph, pDCGDI->GetWidth(),pDCGDI->GetHeight());

	//刷新线程信息
	regu.SetPlateInf(&graph);//刷新跟踪板坯信息

	CRect rect;
	GetClientRect(&rect);
	back.DrawTime(&graph, rect.Width(), rect.Height());

}

BOOL CPCFrameWorkView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CPCFrameWorkView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CPCFrameWorkView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CPCFrameWorkView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPCFrameWorkView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CPCFrameWorkView 诊断

#ifdef _DEBUG
void CPCFrameWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CPCFrameWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPCFrameWorkDoc* CPCFrameWorkView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPCFrameWorkDoc)));
	return (CPCFrameWorkDoc*)m_pDocument;
}
#endif //_DEBUG


// CPCFrameWorkView 消息处理程序


void CPCFrameWorkView::OnTimer(UINT_PTR nIDEvent)
{
	//刷新时间
	if(nIDEvent == 100){
		CRect rect;
		GetClientRect(&rect);
		CRect rectArea(rect.Width()-300, rect.Height()-50, rect.Width(), rect.Height());
		InvalidateRect(&rectArea);
	}
	CView::OnTimer(nIDEvent);
}


void CPCFrameWorkView::OnInitialUpdate()
{	
	SetTimer(100,1000,NULL);
	CView::OnInitialUpdate();
}


void CPCFrameWorkView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认value
	CView::OnLButtonDown(nFlags, point);
}


void CPCFrameWorkView::OnRButtonDown(UINT nFlags, CPoint point)
{

}


BOOL CPCFrameWorkView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CPCFrameWorkView::OnIDGfcL2PresetAccResult_LP()
{
	GfcL2PresetAccResult_LP msg;
	SendGfcL2PresetAccResult_LP(&msg);
}


void CPCFrameWorkView::OnIDGfcL2PresetAccResult_tcr1()
{
	GfcL2PresetAccResult_tcr1 msg;
	SendGfcL2PresetAccResult_tcr1(&msg);
}


void CPCFrameWorkView::OnIDGfcL2PresetAccResult_tcr2()
{
	// TODO: 在此添加命令处理程序代码
	GfcL2PresetAccResult_tcr2 msg;
	SendGfcL2PresetAccResult_tcr2(&msg);
}


void CPCFrameWorkView::OnIDGfcL2ExtAccResult_LP()
{
	GfcL2ExtAccResult_LP msg;
	SendGfcL2ExtAccResult_LP(&msg);
}


void CPCFrameWorkView::OnIDGfcL2ExtAccResult_tcr1()
{
	GfcL2ExtAccResult_tcr1 msg;
	SendGfcL2ExtAccResult_tcr1(&msg);
}


void CPCFrameWorkView::OnIDGfcL2ExtAccResult_tcr2()
{
	GfcL2ExtAccResult_tcr2 msg;
	SendGfcL2ExtAccResult_tcr2(&msg);
}


void CPCFrameWorkView::OnIDGfcL2ExtColTimeRes_tcr1()
{
	GfcL2ExtColTimeRes_tcr1 msg;
	SendGfcL2ExtColTimeRes_tcr1(&msg);
}


void CPCFrameWorkView::OnIDGfcL2ExtColTimeRes_tcr2()
{
	GfcL2ExtColTimeRes_tcr2 msg;
	SendGfcL2ExtColTimeRes_tcr2(&msg);
}


void CPCFrameWorkView::OnShowtrackdialog()
{
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	theApp->m_DlgTrack.ShowWindow(SW_SHOW);
	theApp->m_DlgTrack.CenterWindow(NULL);
	// TODO: 在此添加命令处理程序代码
}


void CPCFrameWorkView::OnGfcL2ExtColTimeRes_LP()
{
	GfcL2ExtColTimeRes_LP msg;
	SendGfcL2ExtColTimeRes_LP(&msg);
}


void CPCFrameWorkView::OnGfcl2extreq()
{
	GfcL2ExtReq msg;
	SendGfcL2ExtReq(&msg);
}




void CPCFrameWorkView::OnGfcextl2pdidata()
{
	GfcEXTL2PdiData block;
	CTime cTime = CTime::GetCurrentTime();
	strcpy(block.GfcPieceId, "2013-8-16a");
	strcpy(block.GfcPieceIdSpare, "MMMMMMMM2");
	strcpy(block.GfcPltMatId, "Q235C");
	strcpy(block.GfcPltMatIdSpare, "MMMMMMMM4");
	strcpy(block.GfcPltMatCode, "ATM_Q235C");
	block.GfcSlabLen = 1.0;
	block.GfcSlabWid = 1.0;
	block.GfcSlabTck = 1.0;
	block.GfcInterSlabTcr1Len = 1.0;
	block.GfcInterSlabTcr1Wid = 1.0;
	block.GfcInterSlabTcr1Tck = 1.0;
	block.GfcInterSlabTcr2Len = 1.1;
	block.GfcInterSlabTcr2Wid = 1.2;
	block.GfcInterSlabTcr2Tck = 1.3;
	block.GfcPltLen = 20000.0;
	block.GfcPltWid = 3000.0;
	block.GfcPltTck = 20.1;
	block.GfcIdTsk = 1;
	block.GfcRollPassNumNow = 1;
	block.GfcTcr1RollPassNumSum = 10;
	block.GfcTcr2RollPassNumSum = 15;
	block.GfcRollPassNumSum = 20;
	block.GfcTime2ACCDQ = 1;
	block.GfcInterSlabTcr1ColTimCal = 1.0;
	block.GfcInterSlabTcr2ColTimCal = 1.1;
	block.GfcPltAccMode = 1;

	block.GfcPltIcFlg[0] = '0';
	block.GfcShpCde = 1;
	block.GfcProdState = 1;
	block.GfcDirLstPas[0] = 'a';
	block.taper_hd_thk = 1.0;
	block.taper_tl_thk =1.0;
	block.taper_l1_len= 1.0;
	block.taper_l2_len= 1.0;
	block.taper_l3_len= 1.0;
	block.taper_l4_len= 1.0;
	block.taper_l5_len= 1.0;
	block.GfcPltDlyTimRol = 1;
	block.GfcSurTmpRolFin = 1.0;
	block.GfcAveTmpRolFin = 1.0;
	block.GfcCorTmpRolFin = 1.0;
	block.GfcPltCrDq = 1.0;
	block.GfcPltTmpDqStp= 1.0;
	block.GfcPltTmpAccSta      = 1.0;
	block.GfcPltCrAcc          = 1.0;
	block.GfcPltTmpAccStp      = 1.0;
	block.GfcPltHprActFlg      = 1.0;
	block.GfcPltPlActFlg       = 1.0;
	block.GfcSlbTcr1TmpRolFin  = 1.0;
	block.GfcSlbTcr1TmpSta     = 1.0;
	block.GfcSlbTcr1TmpStp     = 1.0;
	block.GfcSlbTcr2TmpRolFin  = 1.0;
	block.GfcSlbTcr2TmpSta     = 1.0;
	block.GfcSlbTcr2TmpStp     = 1.0;
	block.GfcSlbSurTmpTcr1Fin  = 1.0;
	block.GfcSlbCalTmpTcr1Fin  = 1.0;
	block.GfcSlbCoreTmpTcr1Fin = 1.0;
	block.GfcSlbTcr1TmpAccSta  = 1.0;
	block.GfcSlbTcr1TmpAccStp  = 1.0;
	block.GfcSlbSurTmpTcr2Fin  = 1.0;
	block.GfcSlbCalTmpTcr2Fin  = 1.0;
	block.GfcSlbCoreTmpTcr2Fin = 1.0;
	block.GfcSlbTcr2TmpAccSta = 1.0;
	block.GfcSlbTcr2TmpAccStp = 1.0;
	block.GfcPltPplTimHead = 1.0;
	block.GfcPltPplSpdHead = 1.0;
	block.GfcPltPplLenHead = 1.0;
	block.GfcPltPplTimBody = 1.0;
	block.GfcPltPplSpdBody = 1.0;
	block.GfcPltPplLenBody = 1.0;
	block.GrnZAccStaHead1 = 1.0;
	block.GrnZAccStaHead2 = 1.0;
	block.GrnZAccStaHead3 = 1.0;
	block.GrnZAccStaHead4 = 1.0;
	block.GrnZAccStaHead5 = 1.0;
	block.GrnZAccStaMdle = 1.0;
	block.GrnZAccStaTail = 1.0;
	block.DcmpZAccStaHead1 = 1.0;
	block.DcmpZAccStaHead2 = 1.0;
	block.DcmpZAccStaHead3 = 1.0;
	block.DcmpZAccStaHead4 = 1.0;
	block.DcmpZAccStaHead5 = 1.0;
	block.DcmpZAccStaMdle = 1.0;
	block.DcmpZAccStaTail = 1.0;
	block.GfcPltMatCompC = 1.0;
	block.GfcPltMatCompMn = 1.0;
	block.GfcPltMatCompP = 1.0;
	block.GfcPltMatCompS = 1.0;
	block.GfcPltMatCompSi = 1.0;
	block.GfcPltMatCompCu = 1.0;
	block.GfcPltMatCompV = 1.0;
	block.GfcPltMatCompNb = 1.0;
	block.GfcPltMatCompCr = 1.0;
	block.GfcPltMatCompNi = 1.0;
	block.GfcPltMatCompMo = 1.0;
	block.GfcPltMatCompSn = 1.0;
	block.GfcPltMatCompN = 1.0;
	block.GfcPltMatCompB = 1.0;
	block.GfcPltMatCompAlTot = 1.0;
	block.GfcPltMatCompTi = 1.0;
	block.GfcPltMatCompCa = 1.0;
	block.GfcPltMatCompPb = 1.0;
	block.GfcPltMatCompAlSol = 1.0;
	block.GfcPltMatCompSb = 1.0;
	block.GfcPltMatCompZn = 1.0;
	block.GfcPltMatCompAs = 1.0;
	block.GfcPltMatCompCo = 1.0;
	block.GfcPltMatCompMg = 1.0;
	block.GfcPltMatCompZr = 1.0;
	block.GfcPltMatCompW = 1.0;
	block.GfcPltMatCompTa = 1.0;
	block.Spare1 = 0;
	block.Spare2 = 0;
	block.Spare3 = 0;
	block.Spare4 = 0;
	block.Spare5 = 0;
	block.Spare6 = 0;
	block.Spare7 = 0;
	block.Spare8 = 0;
	block.Spare9 = 0;
	block.Spare10 = 0;      
	ProcessPackage(&block);
}

void CPCFrameWorkView::OnGfcextl2realpdidata()
{

}
