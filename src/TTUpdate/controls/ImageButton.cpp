/******************************************************************************* 
 *  @file      ImageButton.cpp 2014\8\10 15:37:20 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "ImageButton.h"
#include "../BaseDialog.h"
/******************************************************************************/
CImageButton::CImageButton(void):
	m_hImgNormal(NULL)
	, m_hImgPressed(NULL)
	, m_hImgCovered(NULL)
	, m_bIsHotTrack(TRUE)
	, m_bIsTracking(FALSE)
	, m_bMouseCovered(FALSE)
	, m_dwTransColor(RGB(255,0,255))		// red is transparent
	, m_dwFrameColor(RGB(255,255,255)) 
	, m_bMouseShowHand(TRUE)
	,m_hImgDisable(NULL){
}

CImageButton::~CImageButton(void) {
	ReleaseImages();
}

IMPLEMENT_DYNAMIC(CImageButton, CButton)

BEGIN_MESSAGE_MAP(CImageButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()


void CImageButton::ReleaseImages() {
	if (NULL != m_hImgCovered) {
		::DeleteObject(m_hImgCovered);
	}
	m_hImgCovered = NULL;

	if (NULL != m_hImgNormal) {
		::DeleteObject(m_hImgNormal);
	}
	m_hImgNormal = NULL;
	
	if (NULL != m_hImgPressed) {
		::DeleteObject(m_hImgPressed);
	}
	m_hImgPressed = NULL;
	
	if(NULL != m_hImgDisable){
		::DeleteObject(m_hImgDisable);
	}
	m_hImgDisable = NULL;
}
BOOL CImageButton::LoadImage(UINT nNormalId, UINT nCoveredId, UINT nPressedId,UINT nDisableId) {
	HINSTANCE	hResource = ::AfxGetResourceHandle();
	ReleaseImages();

	m_hImgNormal = ::LoadBitmap(hResource, MAKEINTRESOURCE(nNormalId));
	m_hImgCovered = ::LoadBitmap(hResource, MAKEINTRESOURCE(nCoveredId));
	m_hImgPressed = ::LoadBitmap(hResource, MAKEINTRESOURCE(nPressedId));
	m_hImgDisable = ::LoadBitmap(hResource,MAKEINTRESOURCE(nDisableId));	

	if (NULL == m_hImgNormal) {
		return FALSE;
	}
	if (NULL == m_hImgCovered) {
		m_hImgCovered = m_hImgNormal;
	}
	if ( NULL == m_hImgPressed ) {
		m_hImgPressed = m_hImgNormal;
	}

	if(NULL == m_hImgDisable){
		m_hImgDisable = m_hImgNormal;
	}
	FitToImage();
	return TRUE;
}
void CImageButton::FitToImage() {
	if ( NULL != m_hImgNormal ) { 
		// 设置按钮大小为图片大小
		CBitmap	* pBmp = CBitmap::FromHandle(m_hImgNormal);
		if (NULL != pBmp) {
			BITMAP	BmpInfo;
			pBmp->GetBitmap(&BmpInfo);
			SetWindowPos(0, 0, 0, BmpInfo.bmWidth, BmpInfo.bmHeight, SWP_NOMOVE|SWP_NOZORDER);
		}
	}
}
void CImageButton::OnMouseMove(UINT nFlags, CPoint point) {
	if (!m_bIsTracking) {
		TRACKMOUSEEVENT	tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bIsTracking = (FALSE != _TrackMouseEvent(&tme));
	}

	if (m_bIsHotTrack && m_bMouseShowHand) {
		// 使得鼠标变成手行
		if (MAKEINTRESOURCE(::GetCursor()) != IDC_HAND) {
			m_hOldCursor = ::GetCursor();
			::SetCursor(::LoadCursor(NULL, IDC_HAND));
		}
	}
	CButton::OnMouseMove(nFlags, point);
}
LRESULT	CImageButton::OnMouseLeave(WPARAM wParam, LPARAM lParam) {
	m_bMouseCovered = FALSE;
	m_bIsTracking = FALSE;

	// not erase background
	InvalidateRect(NULL, FALSE);

	if (m_bIsHotTrack) {
		// 获取原有鼠标
		if (::GetCursor() != m_hOldCursor) {
			::SetCursor(m_hOldCursor);
		}
	}
	return NULL;
}
LRESULT	CImageButton::OnMouseHover(WPARAM wParam, LPARAM lParam) {
	m_bMouseCovered = TRUE;

	// erase background
	InvalidateRect(NULL);

	return NULL;
}
void CImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	CDC *	pDC = GetDC();
	UINT	nState = lpDrawItemStruct->itemState;
	CRect	Rect = lpDrawItemStruct->rcItem;

	// draw back ground
	if (0 != (nState & ODS_SELECTED)) {
		DrawBackgroud(pDC, Rect, PRESSED);
	} else if (0 != (nState &  ODS_DISABLED)) {
		DrawBackgroud(pDC, Rect, DISABLED);
	} else if (m_bMouseCovered) {
		DrawBackgroud(pDC, Rect, COVERED);
	} else {
		DrawBackgroud(pDC, Rect, NORMAL);
	}

	// draw frame border
//	DrawFrame(pDC, Rect);

	ReleaseDC(pDC);
}

void CImageButton::PreSubclassWindow() {
	CButton::PreSubclassWindow();
	ModifyStyle(BS_DEFPUSHBUTTON, BS_OWNERDRAW);
}
void CImageButton::DrawBackgroud(CDC * pDC, CRect & Rect, ButtonState state) {
	HBITMAP	hCurBmp = NULL;

	switch(state) {
	case NORMAL:
		hCurBmp = m_hImgNormal;
		break;
	case COVERED:
		hCurBmp = m_hImgCovered;
		break;
	case PRESSED:
		hCurBmp = m_hImgPressed;
		break;
	case DISABLED:
		hCurBmp = m_hImgDisable;
	}

	if (NULL != hCurBmp) {
		CDC		BackgourdDC;
		HBITMAP	hOldBitMap = NULL;

		BackgourdDC.CreateCompatibleDC(NULL);
		hOldBitMap = (HBITMAP)BackgourdDC.SelectObject(hCurBmp);

		BITMAP	BitMap;
		CBitmap::FromHandle(hCurBmp)->GetBitmap(&BitMap);
		// 将BackgourdDC中的图片拷贝到pDC上
		pDC->TransparentBlt(Rect.left, Rect.top, Rect.Width(), Rect.Height(),
			&BackgourdDC, 0, 0, BitMap.bmWidth, BitMap.bmHeight,  m_dwTransColor);

		BackgourdDC.SelectObject(hOldBitMap);
	}else{
		pDC->FillSolidRect(&Rect,COLOR_WND_BACKGROUND);
	}
}
void CImageButton::DrawFrame(CDC * pDC, CRect & Rect) {
	CRgn		rgn;
	rgn.CreateRoundRectRgn(Rect.left-1, Rect.top-1, Rect.right+2, Rect.bottom+2, 2, 1);
	// draw frame border
	pDC->FrameRgn(&rgn, &CBrush(m_dwFrameColor), 1, 1);

	rgn.DeleteObject();
}

LRESULT	CImageButton::OnEraseBkgnd(CDC* pDC){
	return TRUE;
}
/******************************************************************************/
