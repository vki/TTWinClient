/******************************************************************************* 
 *  @file      UIEmotion.cpp 2014\7\16 16:29:00 $
 *  @author    大佛<dafo@mogujie.com>
 *  @brief   
 ******************************************************************************/

#include "stdafx.h"
#include "EmotionDialog.h"
#include "EmotionButton.h"

/******************************************************************************/

// -----------------------------------------------------------------------------
//  UIEmotion: Public, Constructor

EmotionDialog::EmotionDialog()
{

}

// -----------------------------------------------------------------------------
//  UIEmotion: Public, Destructor

EmotionDialog::~EmotionDialog()
{
	PostQuitMessage(0);
}


LPCTSTR EmotionDialog::GetWindowClassName() const
{
	return _T("UIEmotion");
}
CDuiString EmotionDialog::GetSkinFile()
{
	return  _T("EmotionDialog\\QSBFace.xml");
}

UINT EmotionDialog::GetClassStyle() const
{
	return UI_CLASSSTYLE_FRAME | CS_DBLCLKS;
}

CDuiString EmotionDialog::GetSkinFolder()
{
	return   _T("..\\data\\Emotion\\");

}

//UILIB_RESOURCETYPE UIEmotion::GetResourceType() 
//{
//	return UILIB_FILE;
//}

void EmotionDialog::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void EmotionDialog::Init()
{
}

void EmotionDialog::OnPrepare()
{


}

void EmotionDialog::Notify(TNotifyUI& msg)
{
	if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT))
	{
		OnPrepare();
	}
	else if (msg.sType == DUI_MSGTYPE_KILLFOCUS)
	{
		int n = 1; 
		n++;
	}
	else if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_CLICK))
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("prev_page")) == 0)  //上一页
		{
			CEmotionButton *pEmotionButton = static_cast<CEmotionButton*>(m_PaintManager.FindControl(_T("emotion_button")));
			if (pEmotionButton != NULL && pEmotionButton->GetCurrentPage() > 0)
			{
				pEmotionButton->SetPage(pEmotionButton->GetCurrentPage() - 1);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("next_page")) == 0)  //下一页
		{
			CEmotionButton *pEmotionButton = static_cast<CEmotionButton*>(m_PaintManager.FindControl(_T("emotion_button")));

			if (pEmotionButton != NULL && pEmotionButton->GetCurrentPage() < (pEmotionButton->GetPageCount() - 1))
			{
				pEmotionButton->SetPage(pEmotionButton->GetCurrentPage() + 1);
			}
		}
	}
	else
		__super::Notify(msg);
}


//LRESULT UIEmotion::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
//	if (wParam == SC_CLOSE)
//	{
//		::PostQuitMessage(0L);
//		bHandled = TRUE;
//		return 0;
//	}
//
//	BOOL bZoomed = ::IsZoomed(*this);
//	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
//	if (::IsZoomed(*this) != bZoomed) {
//		if (!bZoomed) {
//			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
//			if (pControl) pControl->SetVisible(false);
//			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
//			if (pControl) pControl->SetVisible(true);
//		}
//		else {
//			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
//			if (pControl) pControl->SetVisible(true);
//			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
//			if (pControl) pControl->SetVisible(false);
//		}
//	}
//	return lRes;
//}
LRESULT EmotionDialog::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}
CControlUI* EmotionDialog::CreateControl(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, _T("EmotionButton")) == 0)
		return new CEmotionButton;

	return NULL;
}


LONG EmotionDialog::GetStyle() 
{
	LONG styleValue = WindowImplBase::GetStyle();

	styleValue &= ~WS_MAXIMIZEBOX;
	styleValue &= ~WS_SIZEBOX;
	styleValue &= ~WS_CAPTION;

	return styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
}

LRESULT EmotionDialog::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return HTCLIENT; //这个窗体标题栏
}

void EmotionDialog::ShowWindow(IN POINT pt, bool bShow)
{
	CRect rc;
	GetWindowRect(m_hWnd,&rc);
	SetWindowPos(m_hWnd, NULL, pt.x, pt.y - rc.Height(), 0, 0, SWP_NOSIZE);
	__super::ShowWindow(bShow);
}

LRESULT EmotionDialog::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	return __super::OnKillFocus(uMsg,wParam,lParam,bHandled);
}

/******************************************************************************/