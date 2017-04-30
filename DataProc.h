#pragma once


#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

typedef struct _WORDDATA {
	_WORDDATA(LPCSTR szData, BYTE chDivChar)
	{
		SplitData(szData,chDivChar);
	};

	_WORDDATA(){
	}

	~_WORDDATA(){
		delete[] pszItemBuf;
		delete[] ppItem;
		nMaxItem = 0;
	};

	LPCSTR operator[] (int i){
		return GetItem(i);
	}

	int GetSize(){
		return nMaxItem;
	}

	void SplitData(LPCSTR szData, BYTE chDivChar)	{
		int nItemPos = 0;
		int nLen = 0;
		nMaxItem = 0;
		ppItem = NULL;
		pszItemBuf = NULL;
		if ( szData == NULL || chDivChar == '\0' )
			return;
		// 求字符串的长度和 chDivChar 的个数
		for(nLen = 0;szData[nLen]!='\0';nLen++)
		{
			if(szData[nLen] == chDivChar)
				nMaxItem++;
		}
		nMaxItem++;

		// 根据 字符串长度 和 chDivChar 的个数 申请内存
		ppItem = new char*[nMaxItem];
		pszItemBuf = new char[nLen+1];

		int iPos = 0;
		while( iPos < nLen + 1 )
		{
			ppItem[nItemPos] = &pszItemBuf[iPos];
			while( (BYTE)szData[iPos] != (BYTE)chDivChar && iPos < nLen )
			{
				pszItemBuf[iPos] = szData[iPos];
				iPos++;
			}
			pszItemBuf[iPos++] = '\0';
			nItemPos++;
		}
	}

	LPCSTR GetItem(int n)
	{
		if(n >= 0 && n < nMaxItem)
			return ppItem[n];
		return NULL;
	};

	int  nMaxItem;
private:
	char** ppItem;
	char* pszItemBuf;
}WORDDATA, *PWORDDATA;

typedef struct colItem{
	std::vector<string> _Col;
}COLITEM;

typedef struct obj{
	std::vector<COLITEM> _obj;
	int maxCol;
	string strName;
}OBJ;

//读取的数据文件格式为：
//  xxxx,xxxx,xxxx
//  yyyy,yyyy,yyyy

class CDataProc
{
public:
	CDataProc(void);
	~CDataProc(void);

	//根据前N列，设置后M列的值，数组形式
	void SetValue(CString strSec,CStringArray *pKeys, CStringArray *pValues);

	//根据前N列，设置指定坐标的值，
	void SetValue(CString strSec, int nRow, int Col, CString strItem);

	//获得指定Section中，前N列为pKeys序列的后M列值， pValues 为输出变量，用于返回整条记录的结果
	void GetValue(CString strSec,CStringArray *pKeys, CStringArray *pValues,int *rowNum);
	void GetRangeValue(CString strSec,CStringArray *pKeys, CStringArray *pValues);  // 选取合适的区间值 [11/12/2013 谢谦]

	//初始化数据，参数为指定目标的路径，指定路径后，该函数负责装载该函数下指定格式文件的数据值。
	void InitData(CString strPath);

	//保存操作，保存所有的记录
	void SaveObj();

private:
	void LoadSingleConfig(CString strSinglePath, CString strTagName);
	//加载配置文件
	void LoadIni(CString strIni);

	std::map<string, OBJ> m_Objs;
	string m_strPath;

};

