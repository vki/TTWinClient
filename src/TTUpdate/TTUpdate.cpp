#include "stdafx.h"
#include "TTUpdate.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTTUpdateApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

#define		TT_COMMANDLINE_UPDATER		_T("/:?tt_updater")

CTTUpdateApp::CTTUpdateApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CTTUpdateApp theApp;

BOOL CTTUpdateApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();

#ifndef _DEBUG
	if (!DetectStartMark())
	{
		AfxMessageBox(_T("请用teamtalk.exe启动！"), MB_ICONINFORMATION);
		return FALSE;
	}
#endif

	CString csInstPath;
	if (!GetIntallFilePath(csInstPath) || !IsFileExist(csInstPath))
	{
		//AfxMessageBox(_T("安装文件不存在，请去官网下载"));
		//return FALSE;
	}

	m_pUpdateDlg = new CUpdateDlg(csInstPath);
	m_pMainWnd = m_pUpdateDlg;
	m_pUpdateDlg->Create(CUpdateDlg::IDD);
	m_pUpdateDlg->CenterWindow();
	m_pUpdateDlg->SetForegroundWindow();
	m_pUpdateDlg->ShowWindow(SW_NORMAL);

	return TRUE;

	return FALSE;
}

BOOL CTTUpdateApp::DetectStartMark()
{
	CString csCommline = ::GetCommandLine();
	csCommline.Trim();
	if (csCommline.Find(TT_COMMANDLINE_UPDATER) != -1)
		return TRUE;

	return FALSE;
}

BOOL CTTUpdateApp::GetIntallFilePath(CString& path)
{
	CString csCommline = ::GetCommandLine();
	CString csUpdaterMark = TT_COMMANDLINE_UPDATER;
	int StartIndex = csCommline.Find(csUpdaterMark);
	if (StartIndex < 0)
		return FALSE;
	path = csCommline.Mid(StartIndex + csUpdaterMark.GetLength()
		, csCommline.GetLength() - StartIndex - 1);

	return TRUE;
}

BOOL CTTUpdateApp::ExitInstance()
{
	if (m_pUpdateDlg)
	{
		m_pUpdateDlg->DestroyWindow();
		delete m_pUpdateDlg;
		m_pUpdateDlg = NULL;
	}
	return __super::ExitInstance();
}

