
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "OutputWnd.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	COutputWnd        m_wndOutput;

	CButton m_myButton; 
	CButton m_myButton_2; 
	CButton m_myButton_3; 
	CButton m_myButton_4; 

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	afx_msg LRESULT OnUpdateoutputSystem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateoutputDBLine(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnMenuSocketCheck();
	afx_msg void OnMenuS7Check();
	afx_msg void OnModao();
	afx_msg void OnJinchao();
	afx_msg void OnChuchao();
	afx_msg void OnFanhong();
	afx_msg void onClearL2Track();
	afx_msg void onUpDateSchFile();
};


