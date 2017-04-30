
// PCFrameWorkView.h : CPCFrameWorkView ��Ľӿ�
//

#pragma once
#include "background.h"
#include "plate.h"
#include "threadInf.h"
#include "plate_detail.h"
#include "regulation.h"
#include "parameter.h"
#include "PCFrameWorkDoc.h"

class CPCFrameWorkView : public CView
{
protected: // �������л�����
	CPCFrameWorkView();
	DECLARE_DYNCREATE(CPCFrameWorkView)

// ����
public:
	CPCFrameWorkDoc* GetDocument() const;

// ����
public:

	ThreadInf th;
	Background back;
	Regulation regu;
	Parameter par;
	Region thread[10]; 


// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CPCFrameWorkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	//afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnIDGfcL2PresetAccResult_LP();
	afx_msg void OnIDGfcL2PresetAccResult_tcr1();
	afx_msg void OnIDGfcL2PresetAccResult_tcr2();
	afx_msg void OnIDGfcL2ExtAccResult_LP();
	afx_msg void OnIDGfcL2ExtAccResult_tcr1();
	afx_msg void OnIDGfcL2ExtAccResult_tcr2();
	afx_msg void OnIDGfcL2ExtColTimeRes_tcr1();
	afx_msg void OnIDGfcL2ExtColTimeRes_tcr2();
	afx_msg void OnShowtrackdialog();
	afx_msg void OnGfcL2ExtColTimeRes_LP();
	afx_msg void OnGfcl2extreq();
	afx_msg void OnGfcextl2pdidata();
	afx_msg void OnGfcextl2realpdidata();
};

#ifndef _DEBUG  // PCFrameWorkView.cpp �еĵ��԰汾
inline CPCFrameWorkDoc* CPCFrameWorkView::GetDocument() const
   { return reinterpret_cast<CPCFrameWorkDoc*>(m_pDocument); }
#endif

