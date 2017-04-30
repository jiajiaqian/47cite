#include "StdAfx.h"
#include "DataProc.h"


CDataProc::CDataProc(void)
{
}


CDataProc::~CDataProc(void)
{
}

// 根据参数2代表的前N列值，设置后M列值 [11/7/2013 hanb poorb@live.com]
void CDataProc::SetValue(CString strSec,CStringArray *pKeys, CStringArray *pValues)
{
	if(strSec.IsEmpty() || pKeys == NULL || pValues == NULL)
		return;

	std::map<string, OBJ>::iterator itr = m_Objs.find((LPCSTR)strSec);
	if(itr == m_Objs.end())
		return;
	for(std::vector<COLITEM>::iterator itrSub = itr->second._obj.begin(); itrSub != itr->second._obj.end(); itrSub++){
		int i = 0;

		for(std::vector<string>::iterator itrCol = itrSub->_Col.begin(); itrCol != itrSub->_Col.end() && i < pKeys->GetSize(); itrCol++, i++){
			if(itrCol->c_str() != pKeys->GetAt(i))
				break;
		}
		if(i == pKeys->GetSize()){		//说明找到了
			for(int i = 0; i < pValues->GetSize(); i++){
				itrSub->_Col.push_back((LPCSTR)pValues->GetAt(i));
			}
			if(itr->second.maxCol < itrSub->_Col.size())
				itr->second.maxCol = itrSub->_Col.size();
			break;
		}
	}
}

// 设置指定横纵坐标的值，设置后M列值 [11/7/2013 hanb poorb@live.com]
void CDataProc::SetValue(CString strSec, int nRow, int Col, CString strItem)
{
	if(strSec.IsEmpty() || nRow < 0 || Col < 0)
		return;

	std::map<string, OBJ>::iterator itr = m_Objs.find((LPCSTR)strSec);
	if(itr == m_Objs.end())
		return;

	if(itr->second._obj[nRow]._Col.size() > Col){
		itr->second._obj[nRow]._Col[Col] = strItem;
	}else{
		itr->second._obj[nRow]._Col.push_back((LPCSTR)strItem);
	}
}

// 根据前N列，获得后M列的值 [11/7/2013 hanb poorb@live.com]
void CDataProc::GetValue(CString strSec,CStringArray *pKeys, CStringArray *pValues,int *rowNum)
{
	int j=0;
	*rowNum=0;
	if(strSec.IsEmpty() || pKeys == NULL || pValues == NULL)
		return;

	std::map<string, OBJ>::iterator itr = m_Objs.find((LPCSTR)strSec);
	if(itr == m_Objs.end())
		return;
	for(std::vector<COLITEM>::iterator itrSub = itr->second._obj.begin(); itrSub != itr->second._obj.end(); itrSub++){
		int i = 0;
		j++;
		for(std::vector<string>::iterator itrCol = itrSub->_Col.begin(); itrCol != itrSub->_Col.end() && i < pKeys->GetSize(); itrCol++, i++){
			if(itrCol->c_str() != pKeys->GetAt(i))
				break;
		}
		if(i == pKeys->GetSize()){		//说明找到了
			pValues->RemoveAll();
			for(std::vector<string>::iterator itrCol = itrSub->_Col.begin(); itrCol != itrSub->_Col.end(); itrCol++){
				pValues->Add(itrCol->c_str());
			}
			*rowNum=j;
			return;
		}
	}
}

void CDataProc::LoadSingleConfig(CString strSinglePath, CString strTagName)
{
	if(strTagName.IsEmpty() || strSinglePath.IsEmpty())
		return;

	USES_CONVERSION;
	FILE *fp = fopen(strSinglePath, "rb");
	if(fp == NULL)
		return;

	OBJ _obj;
	char szLine[4096+1];

	int nMaxCol = 0;
	//填充包名
	_obj.strName = strTagName;
	int nAllCount = 0;
	while(fgets(szLine,4096,fp)){
		CString strInfo = szLine;
		strInfo.Trim();
		//变量名\t变量类型\t变量数量\t变量默认值\t变量测试值
		WORDDATA wdInfo(strInfo, ',');
		if(nMaxCol < wdInfo.GetSize())
			nMaxCol = wdInfo.GetSize();
		COLITEM col;
		for(int i = 0; i < wdInfo.GetSize(); i++){
			col._Col.push_back(wdInfo.GetItem(i));
		}
		_obj._obj.push_back(col);
	}
	_obj.maxCol = nMaxCol;
	m_Objs.insert(pair<string, OBJ>(strTagName, _obj));
}

//加载配置文件
void CDataProc::LoadIni(CString strIni)
{
	m_strPath = strIni;
	m_Objs.clear();
	CFileFind cf;
	strIni += "\\*";
	bool bIsFind = cf.FindFile(strIni);
	while(bIsFind){
		bIsFind = cf.FindNextFile();
		CString strFilePath = cf.GetFilePath();
		if(!cf.IsDots() && !cf.IsDirectory()){
			CString strFileName = cf.GetFileName();
			LoadSingleConfig(cf.GetFilePath(), cf.GetFileName());
		}
	}
	cf.Close();
}

void CDataProc::InitData(CString strPath)
{
	LoadIni(strPath);
}

//保存
void CDataProc::SaveObj()
{
	CString strFullPath;
	for(std::map<string, OBJ>::iterator itr = m_Objs.begin(); itr != m_Objs.end(); itr++)
	{
		strFullPath = m_strPath.c_str();
		strFullPath += "\\";
		strFullPath += itr->first.c_str();
		FILE *fp = fopen(strFullPath, "wb");

		for(std::vector<COLITEM>::iterator itrSub = itr->second._obj.begin(); itrSub != itr->second._obj.end(); itrSub++){
			CString strCol;
			for(std::vector<string>::iterator itrCol = itrSub->_Col.begin(); itrCol != itrSub->_Col.end(); itrCol++){
				strCol += itrCol->c_str();
				strCol += ",";
			}
			strCol.TrimRight(",");  // add [3/22/2014 xie]
			strCol += "\r\n";
			fputs(strCol, fp);
		}
		fclose(fp);
	}
}


void CDataProc::GetRangeValue(CString strSec,CStringArray *pKeys, CStringArray *pValues)
{
	std::map<string, OBJ>::iterator itr = m_Objs.find((LPCSTR)strSec);
	if(itr == m_Objs.end())
		return;
	for(std::vector<COLITEM>::iterator itrSub = itr->second._obj.begin(); itrSub != itr->second._obj.end(); itrSub++){
		int i = 0;
		std::vector<string>::iterator itrCol = itrSub->_Col.begin(); 
		std::vector<string>::iterator itrCol2 = itrSub->_Col.begin()+1;

		if((atof(itrCol->c_str())>= atof(pKeys->GetAt(i)))
			&&(atof(itrCol2->c_str())< atof(pKeys->GetAt(i))))
		{
			i=1;
		}

		if(1==i){		//说明找到了
			pValues->RemoveAll();
			for(std::vector<string>::iterator itrCol = itrSub->_Col.begin(); itrCol != itrSub->_Col.end(); itrCol++){
				pValues->Add(itrCol->c_str());
			}
			break;
		}
	}

}