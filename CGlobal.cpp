#include "stdafx.h"
#include "CGlobal.h"

// 获得应用程序所在目录 [7/17/2013 hanb poorb@live.com]
CString Global::GetApplicationBasePath()
{
	CString strPath;
	TCHAR szPathBuf[_MAX_PATH];
	GetModuleFileName(NULL, szPathBuf, _MAX_PATH);
	if(_tcslen(szPathBuf) != 0)
		strPath = szPathBuf;

	if(strPath.GetLength() > 0){
		if(strPath[0] == '"')
			strPath = strPath.Right(strPath.GetLength()-1);

		int nLast = strPath.GetLength() - 1;

		while(nLast > 0 && strPath[nLast] != '\\')
			nLast--;

		strPath = strPath.Left(nLast);
	}
	strPath += "\\";

	return strPath;
}

