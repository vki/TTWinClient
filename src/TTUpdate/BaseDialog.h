/*******************************************************************************
 *  @file      BaseDialog.h 2014\8\10 15:41:05 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     自定义窗口基础实现
 ******************************************************************************/

#ifndef BASEDIALOG_6463B192_BE58_49A0_B189_C6A1A462C42D_H__
#define BASEDIALOG_6463B192_BE58_49A0_B189_C6A1A462C42D_H__

/******************************************************************************/

#define			COLOR_FRAME_BACKGROUND		RGB(126,154,176)
#define			COLOR_WND_BACKGROUND		RGB(235,244,250)

class CBaseDialog : public CDialog
{
	DECLARE_DYNAMIC(CBaseDialog)

public:
	CBaseDialog(UINT idd,CWnd* pParent = NULL);
	virtual ~CBaseDialog();

protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
	
	virtual BOOL OnInitDialog();
protected:
	BOOL	LoadImage();
	void	ReleaseImages();
protected:
	static CBrush		m_staticBrush;
	HBITMAP				m_hBgImg;
};
/******************************************************************************/
#endif// BASEDIALOG_6463B192_BE58_49A0_B189_C6A1A462C42D_H__
