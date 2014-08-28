/******************************************************************************* 
 *  @file      BaseDialog.cpp 2014\8\10 15:41:53 $
 *  @author    ¿ìµ¶<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "BaseDialog.h"
/******************************************************************************/

CBrush		CBaseDialog::m_staticBrush;

IMPLEMENT_DYNAMIC(CBaseDialog, CDialog)

CBaseDialog::CBaseDialog(UINT idd,CWnd* pParent /*=NULL*/)
	: CDialog(idd, pParent)
	,m_hBgImg(NULL)
{
}

CBaseDialog::~CBaseDialog()
{
}

void CBaseDialog::ReleaseImages(){
	if(m_hBgImg){
		::DeleteObject(m_hBgImg);
	}
	m_hBgImg = NULL;
}

BEGIN_MESSAGE_MAP(CBaseDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CBaseDialog::OnInitDialog(){
	CDialog::OnInitDialog();
	/*ReleaseImages();
	HINSTANCE hResource = ::AfxGetResourceHandle();
	m_hBgImg = ::LoadBitmap(hResource,MAKEINTRESOURCE(IDB_BACKGROUND));*/
	return TRUE;
}

BOOL CBaseDialog::OnEraseBkgnd(CDC* pDC){
	CRect rectClient;
	GetClientRect(&rectClient);
	pDC->FillSolidRect(&rectClient,	COLOR_WND_BACKGROUND);	
	return TRUE;

}

void CBaseDialog::OnPaint(){
	CRect rectClient;
	GetClientRect(&rectClient);
	CPaintDC dc(this);

	/*CDC memDC;
	memDC.CreateCompatibleDC(NULL);
	HBITMAP *oldBitmap = (HBITMAP*)memDC.SelectObject(m_hBgImg);
	
	CBitmap *bitMapTemp = CBitmap::FromHandle(m_hBgImg);
	BITMAP bitmapInfo;
	bitMapTemp->GetBitmap(&bitmapInfo);
	dc.StretchBlt(0,0,3,rectClient.Height(),&memDC,0,0,
					1,bitmapInfo.bmHeight,SRCCOPY);

	dc.StretchBlt(rectClient.Width()-3,0,rectClient.Width(),rectClient.Height(),&memDC,0,0,
		1,bitmapInfo.bmHeight,SRCCOPY);*/

	dc.FrameRect(&rectClient, &CBrush(COLOR_FRAME_BACKGROUND));

//	memDC.SelectObject(oldBitmap);
//	DeleteObject(memDC);
}

HBRUSH CBaseDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor){
	if(nCtlColor == CTLCOLOR_STATIC){
		pDC->SetBkMode(TRANSPARENT);
		if(m_staticBrush.m_hObject == NULL){
			m_staticBrush.CreateSolidBrush(COLOR_WND_BACKGROUND);
		}
		return (HBRUSH)m_staticBrush.GetSafeHandle();
	}
	return CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
}
/******************************************************************************/