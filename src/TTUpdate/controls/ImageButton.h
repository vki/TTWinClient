/*******************************************************************************
 *  @file      ImageButton.h 2014\8\10 15:36:33 $
 *  @author    ¿ìµ¶<kuaidao@mogujie.com>
 *  @brief     ÌùÍ¼°´Å¥¿Ø¼þ
 ******************************************************************************/

#ifndef IMAGEBUTTON_10F8BE6E_3DDF_4FC3_8F57_449B807A612A_H__
#define IMAGEBUTTON_10F8BE6E_3DDF_4FC3_8F57_449B807A612A_H__

#include "afxwin.h"
#include "../stdafx.h"
/******************************************************************************/

class CImageButton : public CButton {
public:
	CImageButton(void);
	~CImageButton(void);

	typedef enum  {
		NORMAL = 0,
		COVERED,
		PRESSED,
		DISABLED
	} ButtonState ;

	DECLARE_DYNAMIC(CImageButton)

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT		OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT		OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT		OnEraseBkgnd(CDC* pDC);

	virtual	void		ReleaseImages();
	virtual	void		DrawFrame(CDC * pDC, CRect & Rect);
	virtual	void		DrawBackgroud(CDC * pDC, CRect & Rect, ButtonState state);
	virtual	void		PreSubclassWindow();

public:
	BOOL			LoadImage(UINT nNormalId, UINT nCoverdId = 0, UINT nPressedId = 0,UINT nDisableId = 0);
	void			FitToImage();
	void			SetTransColor(DWORD dwColor) {	m_dwTransColor = dwColor; }
	void			SetFrameColor(DWORD dwColor) {	m_dwFrameColor = dwColor; }
	void			SetHotTrack(BOOL bHotTrack ) {	m_bIsHotTrack = bHotTrack; }
	void			SetMouseShowHand(BOOL b){m_bMouseShowHand = b;}
	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct );

protected:
	HBITMAP			m_hImgNormal;
	HBITMAP			m_hImgCovered;
	HBITMAP			m_hImgPressed;
	HBITMAP			m_hImgDisable;

	DWORD			m_dwTransColor;
	DWORD			m_dwFrameColor;
	
	BOOL			m_bIsHotTrack;
	BOOL			m_bIsTracking;
	BOOL			m_bMouseCovered;
	BOOL			m_bMouseShowHand;
private:
	 HCURSOR		m_hOldCursor;
};
/******************************************************************************/
#endif// IMAGEBUTTON_10f8be6e-3ddf-4fc3-8f57-449b807a612a_H__
