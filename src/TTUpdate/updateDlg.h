/*******************************************************************************
 *  @file      updateDlg.h 2014\8\10 15:44:18 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     强制更新窗口
 ******************************************************************************/

#ifndef UPDATEDLG_7DEF7D28_12AF_4729_8920_DE50DA58FD83_H__
#define UPDATEDLG_7DEF7D28_12AF_4729_8920_DE50DA58FD83_H__

#include "afxwin.h"
#include "resource.h"
#include "BaseDialog.h"
#include "controls/PictureEx.h"
#include "controls/StatLink.h"
#include "controls/ImageButton.h"
/******************************************************************************/

class CUpdateDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CUpdateDlg)

public:
	CUpdateDlg(CString csFilePath,CWnd* pParent = NULL);   // standard constructor
	virtual ~CUpdateDlg();
	enum { IDD = IDD_TTUPDATE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnUpdateok();
	afx_msg LRESULT OnUpdateFeedback(WPARAM wparam,LPARAM lparam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	static UINT __stdcall UpdateProc(LPVOID lparam);

	BOOL RegisterPath(CString &csPath);

public:
	CPictureEx			m_staGif;
	CStaticLink			m_staticLink;
	CImageButton		m_btnUpdateOk;
	CStatic				m_stcInfo;
    CString             m_csInstallFilePath;
};
/******************************************************************************/
#endif// UPDATEDLG_7DEF7D28_12AF_4729_8920_DE50DA58FD83_H__