#include "stdafx.h"
#include "util.h"

CString ReparsePath(const CString &strFilePath)
{
	CString strTmp = strFilePath;

	strTmp.Replace(_T("/"),_T("\\"));
	strTmp.Replace(_T("//"),_T("\\"));

	// 如果非本地路径
	int nLen = strTmp.GetLength();
	if ((nLen>2 && strTmp.Left(2).CompareNoCase(_T("\\\\"))==0) || (nLen>5 && (strTmp.Left(5).CompareNoCase(_T("http:"))==0 || strTmp.Left(5).CompareNoCase(_T("file:"))==0)) || (nLen>4 && strTmp.Left(4).CompareNoCase(_T("ftp:"))==0))
		return strTmp;

	strTmp.Replace(_T("\\\\"),_T("\\"));
	return strTmp;
}

CString  GetFileDirFromPath(const CString &strFilePath)
{
	CString strTmp = ReparsePath(strFilePath);
	int index = strTmp.ReverseFind('\\');
	if (index == -1)
	{
		index = strTmp.ReverseFind(':');
	}
	return strTmp.Left(index+1);
}

CString   GetAppPath()
{
	static CString g_sDllPath = _T("");

	if(g_sDllPath.IsEmpty())
	{
		TCHAR	buffer[MAX_PATH];
		ZeroMemory(buffer, sizeof(TCHAR) * MAX_PATH);
		HMODULE h = GetModuleHandle(NULL);
		::GetModuleFileName(h, buffer, MAX_PATH);	
		g_sDllPath = GetFileDirFromPath(CString(buffer));
	}
	return g_sDllPath;
}

CString GetParentAppPath()
{
	static CString g_csParentAppPath = _T("");
	if(g_csParentAppPath.IsEmpty())
	{
		g_csParentAppPath = GetAppPath();
		LPTSTR lpszPath = g_csParentAppPath.GetBuffer();
		::PathRemoveBackslash(lpszPath);
		g_csParentAppPath = GetFileDirFromPath(g_csParentAppPath);
	}
	return g_csParentAppPath;
}

BOOL    IsFileExist(const CString& csFileName)
{
#ifdef _UNICODE
    return PathFileExists(csFileName);
#else
    int nLen = MultiByteToWideChar(CP_ACP, 0,csFileName, -1, NULL, NULL);
    LPWSTR lpszW = new WCHAR[nLen];
    MultiByteToWideChar(CP_ACP, 0, 
        csFileName, -1, lpszW, nLen);
    BOOL ret =  PathFileExistsW(lpszW);
    delete[] lpszW;
    return ret;
#endif
}
