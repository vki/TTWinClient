/******************************************************************************* 
 *  @file      updateDlg.cpp 2014\8\10 15:45:16 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"	
#include "updateDlg.h"
#include "util.h"
#include "launchInstall.h"
/******************************************************************************/
#define		WM_UPDATE_FEEDBACK			WM_USER + 1000

IMPLEMENT_DYNAMIC(CUpdateDlg, CBaseDialog)

CUpdateDlg::CUpdateDlg(CString csFilePath,CWnd* pParent /*=NULL*/)
: CBaseDialog(CUpdateDlg::IDD,this)
,m_csInstallFilePath(csFilePath)
{

}

CUpdateDlg::~CUpdateDlg()
{
}

void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_GIF, m_staGif);
	DDX_Control(pDX, IDC_STATIC_LINK, m_staticLink);
	DDX_Control(pDX, IDC_BTN_UPDATEOK, m_btnUpdateOk);
	DDX_Control(pDX, IDC_STATIC_INSTALL_INFO, m_stcInfo);
}

BEGIN_MESSAGE_MAP(CUpdateDlg, CBaseDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_UPDATEOK, &CUpdateDlg::OnBnClickedBtnUpdateok)
	ON_MESSAGE(WM_UPDATE_FEEDBACK,&CUpdateDlg::OnUpdateFeedback)
END_MESSAGE_MAP()

BOOL CUpdateDlg::OnInitDialog()
{
	__super::OnInitDialog();

	SetWindowText(_T("安装更新"));

	m_btnUpdateOk.LoadImage(IDB_BTN_NORMAL,IDB_BTN_NORMAL,IDB_BTN_NORMAL,IDB_BTN_DISABLE);

	CString path = GetAppPath() + _T("loading.gif");
	BOOL bRet = m_staGif.Load(path);
	if(bRet)
	{
		m_staGif.Draw();
		m_staGif.SetBkColor(COLOR_WND_BACKGROUND);
	}

	m_staticLink.ShowWindow(SW_HIDE);
	m_btnUpdateOk.ShowWindow(SW_HIDE);

	::CloseHandle((HANDLE)_beginthreadex(NULL,0,UpdateProc,this,0,NULL));

	return TRUE;
}

struct AFX_CTLCOLOR
{
	HWND hWnd;
	HDC hDC;
	UINT nCtlType;
};

HBRUSH CUpdateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor){
	if(pWnd->GetSafeHwnd() == m_staticLink.GetSafeHwnd())
	{
		struct AFX_CTLCOLOR ctl;
		ctl.hWnd = pWnd->GetSafeHwnd();
		ctl.nCtlType = CTLCOLOR_STATIC;
		ctl.hDC = pDC->GetSafeHdc();
		m_staticLink.SendMessage(WM_REFLECT_BASE+WM_CTLCOLOR,0,(LPARAM)&ctl);
	}
	return	CBaseDialog::OnCtlColor(pDC,pWnd,nCtlColor);
}

BOOL CUpdateDlg::RegisterPath(CString &csPath)
{	
	HKEY hKey;
	if(::RegOpenKey(HKEY_LOCAL_MACHINE,PRODUCT_DIR_REGKEY,&hKey) == ERROR_SUCCESS)
	{
		//CString csMark = _T("Installing");
		//csPath = csPath.Left(csPath.ReverseFind('\\')+1);
		//::RegSetValueEx(hKey,_T("Mark"),0,REG_SZ,(LPBYTE)(LPCTSTR)csMark,
		//	(DWORD)(csMark.GetLength() + 1)*sizeof(TCHAR));
		::RegSetValueEx(hKey,_T("Path"),0, REG_SZ,(LPBYTE)(LPCTSTR)csPath,
			(DWORD)(csPath.GetLength() + 1)*sizeof(TCHAR));
		::RegCloseKey(hKey);
	}
	return TRUE;

}

UINT CUpdateDlg::UpdateProc(LPVOID lparam)
{
	CUpdateDlg *pDlg = (CUpdateDlg*)lparam;
	if(pDlg == NULL || pDlg->m_hWnd == NULL)
		return 0;

	//将安装路径写入注册表，让安装程序读取
	CString csPath = GetParentAppPath();
#ifndef _DEBUG	
	pDlg->RegisterPath(csPath);
#endif

	CLaunchInstall luanch(pDlg->m_csInstallFilePath);
	bool bRet = luanch.Launch();

#ifndef _DEBUG	
	CString csStartExe = pDlg->m_csInstallFilePath;
	::DeleteFile(csStartExe);
#endif

	::PostMessage(pDlg->GetSafeHwnd(),WM_UPDATE_FEEDBACK,(WPARAM)bRet,NULL);

	return 0;
}

LRESULT CUpdateDlg::OnUpdateFeedback(WPARAM wparam,LPARAM lparam)
{
	BOOL bRet = (BOOL)wparam;

	if(bRet)
	{
		m_staticLink.m_link = _T("http://www.mogujie.com/");
		m_staticLink.SetWindowText(_T("更多精彩。尽在蘑菇街！http://www.mogujie.com/"));
		m_stcInfo.SetWindowText(_T("安装完成！请点击“确定”重新启动程序"));
		m_btnUpdateOk.SetWindowPos(NULL,265,39,64,27,SWP_NOZORDER);
	}
	else
	{
		m_staticLink.m_link = _T("http://mogujie.com/");
		m_staticLink.SetWindowText(_T("请至官方网站下载安装程序直接进行安装。"));
		m_stcInfo.SetWindowText(_T("安装失败！"));
		m_btnUpdateOk.SetWindowPos(NULL,265,39,64,27,SWP_NOZORDER);
	}

	m_staticLink.ShowWindow(SW_SHOW);
	m_staGif.ShowWindow(SW_HIDE);
	m_btnUpdateOk.EnableWindow(TRUE);
	m_btnUpdateOk.ShowWindow(SW_SHOW);
	Invalidate(TRUE);

	return S_OK;
}

void CUpdateDlg::OnBnClickedBtnUpdateok()
{
	CString csPath;
	csPath = CLaunchInstall::getLaunchPath();
	csPath += _T("\\bin\\teamtalk.exe");
	DWORD dwRet = (DWORD)::ShellExecute(NULL,_T("Open"),csPath,NULL,NULL,SW_SHOWMAXIMIZED);
	if(dwRet <= 32L)
	{
		AfxMessageBox(_T("启动TT失败!"),MB_OK | MB_ICONERROR);
		//OnUpdateFeedback(FALSE,NULL);
		//return;
	}

	ShowWindow(SW_HIDE);
	AfxPostQuitMessage(0);
}

BOOL CUpdateDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYUP
		|| pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_ESCAPE )
		{
			return FALSE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
/******************************************************************************/