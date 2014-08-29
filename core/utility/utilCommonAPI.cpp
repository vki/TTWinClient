/******************************************************************************* 
 *  @file      utilCommonAPI.cpp 2014\7\15 20:30:45 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief   
 ******************************************************************************/

#include "stdafx.h"
#include "utility/utilCommonAPI.h"
#include <shlwapi.h>
#include <shellapi.h>

/******************************************************************************/
NAMESPACE_BEGIN(util)

CString reparsePath(const CString& strFilePath)
{
	CString strTmp = strFilePath;

	strTmp.Replace(_T("/"), _T("\\"));
	strTmp.Replace(_T("//"), _T("\\"));

	// 如果非本地路径
	int nLen = strTmp.GetLength();
	if ((nLen > 2 && strTmp.Left(2).CompareNoCase(_T("\\\\")) == 0) || (nLen > 5 && (strTmp.Left(5).CompareNoCase(_T("http:")) == 0 || strTmp.Left(5).CompareNoCase(_T("file:")) == 0)) || (nLen > 4 && strTmp.Left(4).CompareNoCase(_T("ftp:")) == 0))
		return strTmp;

	strTmp.Replace(_T("\\\\"), _T("\\"));
	return strTmp;
}

CString getFileDirFromPath(const CString& strFilePath)
{
	CString strTmp = reparsePath(strFilePath);
	int index = strTmp.ReverseFind('\\');
	if (index == -1)
	{
		index = strTmp.ReverseFind(':');
	}
	return strTmp.Left(index + 1);
}

CString getAppPath()
{
	static CString g_sDllPath = _T("");

	if (g_sDllPath.IsEmpty())
	{
		TCHAR	buffer[MAX_PATH];
		ZeroMemory(buffer, sizeof(TCHAR)* MAX_PATH);
		HMODULE h = GetModuleHandle(NULL);
		::GetModuleFileName(h, buffer, MAX_PATH);
		g_sDllPath = getFileDirFromPath(CString(buffer));
	}
	return g_sDllPath;
}

CString getParentAppPath()
{
	static CString g_csParentAppPath = _T("");
	if (g_csParentAppPath.IsEmpty())
	{
		g_csParentAppPath = getAppPath();
		LPTSTR lpszPath = g_csParentAppPath.GetBuffer();
		::PathRemoveBackslash(lpszPath);
		g_csParentAppPath = getFileDirFromPath(g_csParentAppPath);
	}
	return g_csParentAppPath;
}

BOOL isFileExist(LPCTSTR lpcsFileName)
{
	return PathFileExists(lpcsFileName);
}

BOOL createAllDirectories(CString & csDir)
{
	if (csDir.Right(1) == _T("\\"))
	{
		csDir = csDir.Left(csDir.GetLength() - 1);
	}

	if (::GetFileAttributes(csDir) != INVALID_FILE_ATTRIBUTES)
	{
		return TRUE;
	}

	int nFound = csDir.ReverseFind('\\');
	createAllDirectories(csDir.Left(nFound));

	if (!::CreateDirectory(csDir, NULL))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL emptyFolder(const CString& csFolder, BOOL bDeleteSelf /*= FALSE*/)
{
	CFileFind tempFind;
	CString tmp;
	tmp.Format(_T("%s\\*.*"), csFolder);
	BOOL IsFinded = tempFind.FindFile(tmp);
	while (IsFinded)
	{
		IsFinded = tempFind.FindNextFile();

		if (!tempFind.IsDots())
		{
			CString& sFoundFileName = tempFind.GetFileName();

			if (tempFind.IsDirectory())
			{
				CString sTempDir;
				sTempDir.Format(_T("%s\\%s"), csFolder, sFoundFileName);
				emptyFolder(sTempDir, TRUE);
			}
			else
			{
				CString sTempFileName;
				sTempFileName.Format(_T("%s\\%s"), csFolder, sFoundFileName);
				DeleteFile(sTempFileName);
			}
		}
	}
	tempFind.Close();
	if (bDeleteSelf)
	{
		if (!RemoveDirectory(csFolder))
		{
			return FALSE;
		}
	}
	return TRUE;
}
CString getFileExtName(CString& strFile)
{
	CString strExt = _T("");
	LPTSTR pszExtName = (LPTSTR)_tcsrchr(strFile, _T('.'));
	if (pszExtName++)
	{
		strExt = pszExtName;
	}

	return strExt;
}

UInt32 hash_BKDR(const char* str)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc.
	unsigned int hash = 0;
	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

BOOL saveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName)
{
	HDC hDC;
	int iBits;
	WORD wBitCount = 24;
	DWORD dwPaletteSize = 0,
		dwBmBitsSize,
		dwDIBSize, dwWritten;
	BITMAP Bitmap;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;

	HANDLE fh = INVALID_HANDLE_VALUE;
	HGDIOBJ hPal = NULL, hOldPal = NULL;
	void* pData = NULL;
	BOOL bResult = FALSE;

	HDC hWndDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	hDC = ::CreateCompatibleDC(hWndDC);
	if (hDC)
		iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	DeleteDC(hWndDC);

	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 24;

	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	pData = malloc(dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));

	if (pData == NULL)
		goto leave;

	lpbi = (LPBITMAPINFOHEADER)pData;
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+dwPaletteSize,
		(LPBITMAPINFO)
		lpbi, DIB_RGB_COLORS);

	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpFileName, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		goto leave;

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER)
		+sizeof(BITMAPINFOHEADER)
		+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)
		+(DWORD)sizeof(BITMAPINFOHEADER)
		+dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);

	bResult = TRUE;

leave:
	if (pData != NULL)
	{
		free(pData);
	}

	if (fh != INVALID_HANDLE_VALUE)
		CloseHandle(fh);

	return bResult;

}

NAMESPACE_END(util)
/******************************************************************************/