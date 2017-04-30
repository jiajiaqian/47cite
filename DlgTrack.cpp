// DlgTrack.cpp : 实现文件
//

#include "stdafx.h"
#include "PCFrameWork.h"
#include "DlgTrack.h"
#include "afxdialogex.h"


// CDlgTrack 对话框

IMPLEMENT_DYNAMIC(CDlgTrack, CDialogEx)

CDlgTrack::CDlgTrack(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTrack::IDD, pParent)
	, m_templeID(_T("test2013"))
	, m_templeThick(20)
	, m_templeTargetTemp(600)
	, m_targetStaTemp(800)
	, m_tempWidth(4000)
	, m_waterFactor(0)
	, m_manualHC(1)
	, m_selflearnChose(TRUE)
	, m_coolingrate(10)
	, m_length(19000)
	, m_ACCMODE(1)
	, m_watertemperature(20)
	, m_C(0.18)
	, m_Mn(1.4)
	, m_Ni(0.01)
	, m_Cr(0.01)
	, m_Si(0.2)
	, m_turning(0)
{

}

CDlgTrack::~CDlgTrack()
{
}

void CDlgTrack::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_TRACK, m_LstTrack);
	DDX_Control(pDX, IDC_LSTSENDTRACK, m_lstSendTrack);
	DDX_Text(pDX, IDC_EDIT1, m_templeID);
	DDV_MaxChars(pDX, m_templeID, 32);
	DDX_Text(pDX, IDC_EDIT2, m_templeThick);
	DDX_Text(pDX, IDC_EDIT3, m_templeTargetTemp);
	DDX_Text(pDX, IDC_EDIT4, m_targetStaTemp);
	DDX_Text(pDX, IDC_EDIT5, m_tempWidth);
	DDX_Text(pDX, IDC_EDIT6, m_waterFactor);
	DDX_Text(pDX, IDC_EDIT7, m_manualHC);
	DDV_MinMaxFloat(pDX, m_manualHC, 0.5, 2);
	DDX_Check(pDX, IDC_CHECK1, m_selflearnChose);
	DDX_Text(pDX, IDC_EDIT8, m_coolingrate);
	DDX_Text(pDX, IDC_EDIT9, m_length);
	DDX_Text(pDX, IDC_EDIT10, m_ACCMODE);
	DDX_Text(pDX, IDC_EDIT11, m_watertemperature);
	DDX_Text(pDX, IDC_EDIT12, m_C);
	DDX_Text(pDX, IDC_EDIT13, m_Si);
	DDX_Text(pDX, IDC_EDIT14, m_Mn);
	DDX_Text(pDX, IDC_EDIT16, m_Ni);
	DDX_Text(pDX, IDC_EDIT15, m_Cr);
	DDX_Text(pDX, IDC_EDIT17, m_turning);
	DDV_MinMaxInt(pDX, m_turning, 0, 4);
}


BEGIN_MESSAGE_MAP(CDlgTrack, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgTrack::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgTrack::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgTrack::OnBnClickedButton3)
END_MESSAGE_MAP()


// CDlgTrack 消息处理程序


BOOL CDlgTrack::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	DWORD dStyle = m_LstTrack.GetExtendedStyle();
	m_LstTrack.SetExtendedStyle(dStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_LstTrack.InsertColumn(0,_T("属性"),LVCFMT_LEFT,200);
	m_LstTrack.InsertColumn(1,_T("值"),LVCFMT_LEFT,600);

	for(int i = 0; i < 150; i++){
		m_LstTrack.InsertItem(i,"");
	}

	dStyle = m_lstSendTrack.GetExtendedStyle();
	m_lstSendTrack.SetExtendedStyle(dStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_lstSendTrack.InsertColumn(0,_T("属性"),LVCFMT_LEFT,200);
	m_lstSendTrack.InsertColumn(1,_T("值"),LVCFMT_LEFT,600);
	for(int i = 0; i < 150; i++){
		m_lstSendTrack.InsertItem(i,"");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

CString CDlgTrack::FloatToString(float f){

	CString strFormat;
	strFormat.Format("%f", f);

	return strFormat;
}

CString CDlgTrack::ChartoString(char sz)
{
	CString strFormat;
	strFormat.Format("%c", sz);

	return strFormat;
}

CString CDlgTrack::IntToString(int d){

	CString strFormat;
	strFormat.Format("%d", d);

	return strFormat;
}


void CDlgTrack::LoadModel(int nType)
{
	 CString strPath = Global::GetApplicationBasePath();
	 strPath += "Socket_Line1\\";
	 strPath += IntToString(nType);
	 strPath += ".txt";

	 FILE * fp = fopen(strPath.GetBuffer(0), "r");
	 char szLine[250];
	 CString strInfo;
	 CString strType;
	 CString strParam;
	 CString strCount;

	 PARAMS pms;
	 int nStep = 0;
	 while(fgets(szLine, 250, fp)){
		 strInfo = szLine;
		 strInfo.Trim();
		 strInfo.Trim('\t');
	
		 int nPos = strInfo.Find(' ');
		 if(nPos <= 0)
			continue;

		 strType = strInfo.Mid(0,nPos);
		 strInfo = strInfo.Mid(nPos+1);
		 strInfo.Trim();
		 strInfo.Trim('\t');
		 nPos = strInfo.Find(' ');
		 if(nPos > 0){
			 strParam = strInfo.Mid(0, nPos);
			 strInfo = strInfo.Mid(nPos+1);
			 strInfo.Trim();
			 strInfo.Trim('\t');
			 if(strInfo.IsEmpty())
				 strCount = "1";
			 else
				 strCount = strInfo;
		 }else{
			 strParam = strInfo;
			 strCount = "1";
		 }

		 PARAM par;
		 ZeroMemory(&par,sizeof(PARAM));
		 strncpy(par.szType, strType, min(strType.GetLength(), 20));
		 strncpy(par.szParam, strParam, min(strParam.GetLength(), 250));
		 par.nCount = atoi(strCount);
		 par.nStep = nStep;
		 nStep += GetStep(par.szType) * par.nCount;
		 par.nSubStep = GetStep(par.szType) * par.nCount;
		 pms.vecParams.push_back(par);
	 }
	m_mapModel.insert(pair<string,PARAMS>(IntToString(nType),pms));
}

int CDlgTrack::GetStep(CString strType)
{
	strType.MakeLower();
	if(strType == "char")
		return 1;
	else if(strType == "float")
		return 4;
	else if(strType == "int")
		return 4;
	else if(strType == "short")
		return 2;
	return 0;
}

CString CDlgTrack::ShortToString(short s)
{
	CString strParam;
	strParam.Format("%d", s);
	return strParam;
}

CString CDlgTrack::GetParamValue(const char *szType, int nCount, LPVOID lpInfo)
{
	string strType = szType;
	if(strType == "char")
	{
		CString strInfo;
		char szInfo[200];
		memcpy(szInfo, lpInfo, sizeof(char)*nCount);
		strInfo.Format("%s", lpInfo);
		return strInfo;
	}else if(strType == "int"){
		int nBuf;
		memcpy(&nBuf,lpInfo, sizeof(int));
		return IntToString(nBuf);
	}else if(strType == "float"){
		float fBuf;
		memcpy(&fBuf,lpInfo, sizeof(float));
		return FloatToString(fBuf);
	}else if(strType == "short"){
		short sbuf;
		memcpy(&sbuf,lpInfo, sizeof(short));
		return ShortToString(sbuf);
	}
	return "";
}

void CDlgTrack::ShowSendList( int nType, LPVOID lpInfo)
{
//	m_LstTrack.RemoveAllGroups();
	std::map<string,PARAMS>::iterator itrParams = m_mapModel.find(IntToString(nType).GetBuffer(0));
	if(itrParams == m_mapModel.end()){
		LoadModel(nType);
		itrParams = m_mapModel.find(IntToString(nType).GetBuffer(0));
		if(itrParams == m_mapModel.end())
			return;
	}
	PARAMS *pMsg = &itrParams->second;
	int nIndex = 0;
	for(vector<PARAM>::iterator itr = pMsg->vecParams.begin(); itr != pMsg->vecParams.end(); itr++, nIndex++)
	{
		if(itr->nCount > 1 && strcmp(itr->szType, "char") != 0){
			CString strParam = itr->szParam;
			int nPartStep = 0;
			for(int i = 0; i < itr->nCount; i++)
			{
				strParam.Format("%s [%d]", itr->szParam, i);
				nPartStep = itr->nSubStep / itr->nCount * i;
				m_lstSendTrack.SetItemText(nIndex, 0, strParam);
				m_lstSendTrack.SetItemText(nIndex, 1, GetParamValue(itr->szType, itr->nCount, (char *)lpInfo+itr->nStep+nPartStep));
				nIndex++;
			}
		}else{
			m_lstSendTrack.SetItemText(nIndex, 0, itr->szParam);
			m_lstSendTrack.SetItemText(nIndex, 1, GetParamValue(itr->szType, itr->nCount, (char *)lpInfo+itr->nStep));
		}
	}
	for(int i = nIndex; i < 150; i++)
	{
		m_LstTrack.SetItemText(nIndex, 0, "");
		m_LstTrack.SetItemText(nIndex, 1, "");
	}
}

void CDlgTrack::ShowRecvList( int nType, LPVOID lpInfo)
{
	std::map<string,PARAMS>::iterator itrParams = m_mapModel.find(IntToString(nType).GetBuffer(0));
	if(itrParams == m_mapModel.end()){
		LoadModel(nType);
		itrParams = m_mapModel.find(IntToString(nType).GetBuffer(0));
		if(itrParams == m_mapModel.end())
			return;
	}

	PARAMS *pMsg = &itrParams->second;
	int nIndex = 0;
	for(vector<PARAM>::iterator itr = pMsg->vecParams.begin(); itr != pMsg->vecParams.end(); itr++, nIndex++)
	{
		if(itr->nCount > 1 && strcmp(itr->szType, "char") != 0){
			CString strParam = itr->szParam;
			int nPartStep = 0;
			for(int i = 0; i < itr->nCount; i++)
			{
				strParam.Format("%s [%d]", itr->szParam, i);
				nPartStep = itr->nSubStep / itr->nCount * i;
				m_LstTrack.SetItemText(nIndex, 0, strParam);
				m_LstTrack.SetItemText(nIndex, 1, GetParamValue(itr->szType, itr->nCount, (char *)lpInfo+itr->nStep+nPartStep));
				nIndex++;
			}
		}else{
			m_LstTrack.SetItemText(nIndex, 0, itr->szParam);
			m_LstTrack.SetItemText(nIndex, 1, GetParamValue(itr->szType, itr->nCount, (char *)lpInfo+itr->nStep));
		}
	}
	for(int i = nIndex; i < 150; i++)
	{
		m_LstTrack.SetItemText(nIndex, 0, "");
		m_LstTrack.SetItemText(nIndex, 1, "");
	}
}

void CDlgTrack::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 
	SocketLine1Define *sock1=theApp->sock1;
	S7Line1Define *s71=theApp->s71;

	UpdateData(TRUE);

	strcpy_s(s71->s1[0].Trk_ID,m_templeID);
	strcpy_s(s71->s1[0].Steel_type,"Q235B");

	s71->s1[0].Plate_length=m_length;
	s71->s1[0].Plate_width=m_tempWidth;
	s71->s1[0].Plate_thick=m_templeThick;
	s71->s1[0].tar_rbt=m_templeTargetTemp;
	s71->s1[0].tar_cr_1=m_coolingrate;
	ufcs->FINISH_ROLL[0].RalCoeff.WaterTemp=m_waterFactor;

	setTRB(5,1);
	SetEvent(theApp->L2Sys->thread[4].ThreadEvent[0]);
	logger::LogDebug("system","send schedule to PLC S1");
	
	strcpy(ufcs->FINISH_ROLL[0].RalPdi.plateID,m_templeID);
	//strcpy(ufcs->FINISH_ROLL.RalPdi.plateID,"1330647004");
	strcpy(ufcs->FINISH_ROLL[0].RalPdi.mat_id,"A516M485/A516Gr7");
	ufcs->FINISH_ROLL[0].RalPdi.length=m_length/1000;
	ufcs->FINISH_ROLL[0].RalPdi.width=m_tempWidth/1000;
	ufcs->FINISH_ROLL[0].RalPdi.thick=m_templeThick/1000;
	ufcs->FINISH_ROLL[0].RalPdi.targetFinishCoolTemp=m_templeTargetTemp;	
	ufcs->FINISH_ROLL[0].RalPdi.cr=m_coolingrate;
	ufcs->FINISH_ROLL[0].RalPdi.finishRollTemp=m_targetStaTemp;
	ufcs->FINISH_ROLL[0].RalPdi.acc_request =1;
	ufcs->FINISH_ROLL[0].RalPdi.op_mode = 2;
	ufcs->FINISH_ROLL[0].RalPdi.acc_mode = m_ACCMODE;
	ufcs->FINISH_ROLL[0].RalPdi.alloy_c = m_C;
	ufcs->FINISH_ROLL[0].RalPdi.alloy_si = m_Si;
	ufcs->FINISH_ROLL[0].RalPdi.alloy_mn = m_Mn;
	ufcs->FINISH_ROLL[0].RalPdi.alloy_ni = m_Ni;
	ufcs->FINISH_ROLL[0].RalPdi.alloy_cr = m_Cr;
	ufcs->FINISH_ROLL[0].RalCoeff.WaterTemp=m_waterFactor;
	

}


void CDlgTrack::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

	UFCDefine *ufcs=theApp->ufcs; 

	UpdateData(TRUE);

	ufcs->waterFactorManual=m_waterFactor;
	ufcs->HeatCoefManual=m_manualHC;
	ufcs->trk.learnFlg=m_selflearnChose;

	
}


void CDlgTrack::OnBnClickedButton3()
{
	// add [1/14/2015 谢谦]
	CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();
	UpdateData(TRUE);
	theApp->turning=m_turning;
	
	// TODO: 在此添加控件通知处理程序代码
}
