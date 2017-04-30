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
		// ���ַ����ĳ��Ⱥ� chDivChar �ĸ���
		for(nLen = 0;szData[nLen]!='\0';nLen++)
		{
			if(szData[nLen] == chDivChar)
				nMaxItem++;
		}
		nMaxItem++;

		// ���� �ַ������� �� chDivChar �ĸ��� �����ڴ�
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

//��ȡ�������ļ���ʽΪ��
//  xxxx,xxxx,xxxx
//  yyyy,yyyy,yyyy

class CDataProc
{
public:
	CDataProc(void);
	~CDataProc(void);

	//����ǰN�У����ú�M�е�ֵ��������ʽ
	void SetValue(CString strSec,CStringArray *pKeys, CStringArray *pValues);

	//����ǰN�У�����ָ�������ֵ��
	void SetValue(CString strSec, int nRow, int Col, CString strItem);

	//���ָ��Section�У�ǰN��ΪpKeys���еĺ�M��ֵ�� pValues Ϊ������������ڷ���������¼�Ľ��
	void GetValue(CString strSec,CStringArray *pKeys, CStringArray *pValues,int *rowNum);
	void GetRangeValue(CString strSec,CStringArray *pKeys, CStringArray *pValues);  // ѡȡ���ʵ�����ֵ [11/12/2013 лǫ]

	//��ʼ�����ݣ�����Ϊָ��Ŀ���·����ָ��·���󣬸ú�������װ�ظú�����ָ����ʽ�ļ�������ֵ��
	void InitData(CString strPath);

	//����������������еļ�¼
	void SaveObj();

private:
	void LoadSingleConfig(CString strSinglePath, CString strTagName);
	//���������ļ�
	void LoadIni(CString strIni);

	std::map<string, OBJ> m_Objs;
	string m_strPath;

};

