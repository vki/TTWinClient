/******************************************************************************* 
 *  @file      EmotionModule_Impl.cpp 2014\8\6 20:07:17 $
 *  @author    ¿ìµ¶<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "EmotionModule_Impl.h"
#include "EmotionDialog.h"

/******************************************************************************/
namespace module
{
	module::IEmotionModule* getEmotionModule()
	{
		return (module::IEmotionModule*)logic::GetLogic()->getModule(MODULE_ID_EMOTION);
	}
}

// -----------------------------------------------------------------------------
//  EmotionModule_Impl: Public, Constructor

EmotionModule_Impl::EmotionModule_Impl()
:m_emotionWindow(0)
{

}

// -----------------------------------------------------------------------------
//  EmotionModule_Impl: Public, Destructor

EmotionModule_Impl::~EmotionModule_Impl()
{

}

void EmotionModule_Impl::showEmotionDialog(POINT pt)
{
	if (!m_emotionWindow)
	{
		m_emotionWindow = new EmotionDialog;
		m_emotionWindow->Create(NULL, _T("EmotionWnd")
			, UI_WNDSTYLE_FRAME | WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_STATICEDGE
			, 0, 0, 0, 0);
		m_emotionWindow->ShowWindow(pt, TRUE);
		return;
	}
	if (::IsWindowVisible(m_emotionWindow->GetHWND()))
		m_emotionWindow->ShowWindow(pt, FALSE);
	else
		m_emotionWindow->ShowWindow(pt, TRUE);
}

void EmotionModule_Impl::release()
{
	delete this;
}

BOOL EmotionModule_Impl::getEmotionPathByText(IN CString csEmotion, OUT CString& csPath)
{
	return FALSE;
}

/******************************************************************************/