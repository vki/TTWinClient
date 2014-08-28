#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"
#include "updateDlg.h"

class CTTUpdateApp : public CWinApp
{
public:
	CTTUpdateApp();

public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	BOOL DetectStartMark();
	BOOL GetIntallFilePath(CString& path);

	DECLARE_MESSAGE_MAP()

private:
	CUpdateDlg*         m_pUpdateDlg;
};

extern CTTUpdateApp theApp;