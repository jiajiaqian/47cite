#pragma once
#include "afxcmn.h"
#include <map>
#include <vector>

typedef struct tagParam
{
	char szType[20];
	char szParam[200];
	int nCount;
	int nStep;
	int nSubStep;
}PARAM;

typedef struct tagParams
{
	vector<PARAM> vecParams;
}PARAMS;

class CDlgTrack : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTrack)

public:
	CDlgTrack(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTrack();

// 对话框数据
	enum { IDD = IDD_DLGTRACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_LstTrack;
	virtual BOOL OnInitDialog();

	CString GetParamValue(const char *ppar, int nCount, LPVOID lpInfo);

	CString ShortToString(short s);
	CString FloatToString(float f);
	CString IntToString(int d);
	CString ChartoString(char sz);

	void ShowSendList( int nType, LPVOID lpInfo);
	void ShowRecvList( int nType, LPVOID lpInfo);



	CListCtrl m_lstSendTrack;

	void LoadModel(int nType);
	
	int GetStep(CString strType);

	std::map<string, PARAMS> m_mapModel;
	CString m_templeID;
	float m_templeThick;
	float m_templeTargetTemp;
	afx_msg void OnBnClickedButton1();
	float m_targetStaTemp;
	float m_tempWidth;
	float m_waterFactor;
	afx_msg void OnBnClickedButton2();
	float m_manualHC;
	BOOL m_selflearnChose;
	float m_coolingrate;
	float m_length;
	int m_ACCMODE;
	float m_watertemperature;
	float m_C;
	float m_Si;
	float m_Mn;
	float m_Ni;
	float m_Cr;
	int m_turning;
	afx_msg void OnBnClickedButton3();
};
