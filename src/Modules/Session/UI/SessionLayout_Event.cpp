/******************************************************************************* 
 *  @file      SessionLayout_Event.cpp 2014\8\15 13:03:04 $
 *  @author    大佛<dafo@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "Modules/UI/SessionLayout.h"
#include "Modules/IEmotionModule.h"
#include "Modules/IMiscModule.h"
#include "Modules/IGroupListModule.h"
#include "Modules/ISessionModule.h"
#include "Modules/IP2PCmdModule.h"
#include "Modules/ISysConfigModule.h"
#include "../../Message/ReceiveMsgManage.h"
#include "utility/Multilingual.h"
#include "utility/utilStrCodeAPI.h"
#include "UIIMEdit.h"

#define  TIMER_CHECK_WRITING 1

/******************************************************************************/

void SessionLayout::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0 )
	{
		OnWindowInitialized(msg);
	}
	else if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender == m_pBtnSendMsg)
		{
			SendMsg();
		}
		else if (m_pBtnClose == msg.pSender)
		{
		}
		else if (msg.pSender == m_pBtnEmotion)
		{
			//表情先不展示
			POINT pt = { 0 };
			CDuiRect rcEmotionBtn = msg.pSender->GetPos();
			CDuiRect rcWindow;
			GetWindowRect(m_pManager->GetPaintWindow(), &rcWindow);

			pt.y = rcWindow.top + rcEmotionBtn.top;
			pt.x = rcWindow.left + rcEmotionBtn.left;
			//m_emotion_list_window.SelectEmotion(pt);

			module::getEmotionModule()->showEmotionDialog(pt);
			//if (0 == m_emotionWindow)
			//{
			//	module::getEmotionModule()->showEmotionDialog(pt);
			//}
			//else if (::IsWindowVisible(m_emotionWindow->GetHWND()))
			//{
			//	m_emotionWindow->ShowWindow(pt,FALSE);
			//}
			//else
			//{
			//	m_emotionWindow->ShowWindow(pt,TRUE);
			//}
		}
		else if (msg.pSender == m_pBtnSendImage)
		{
			if (m_pInputRichEdit)
			{
				m_pInputRichEdit->InsertBitmap(CString(CPaintManagerUI::GetResourcePath()) + _T("..\\data\\Miss.bmp"));
				//CString strPath = CString(CPaintManagerUI::GetResourcePath()) + _T("..\\data\\Miss.bmp");
				//m_pInputRichEdit->InsertAniSmiley(strPath.GetBuffer());
			}
			//_InsertPic(CString(CPaintManagerUI::GetResourcePath()) + _T("..\\data\\Miss.bmp"));//_T("MainDialog\\default2.png"));
			////插入图片
			//CBitmap bmp;
			//CString strBitmapPath = CPaintManagerUI::GetInstancePath() + _T("..\\data\\Miss.bmp");

			//HBITMAP bitmap;
			//bitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(), strBitmapPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			//bmp.DeleteObject();
			//if (!bmp.Attach(bitmap))
			//{
			//	return;
			//}
			//if (bitmap)
			//{
			//	CImageDataObject::InsertBitmap(m_pRichEditOle, bitmap);
			//}
		}
		else if (msg.pSender == m_pBtnscreenshot)
		{
		}
		else if (msg.pSender == m_pBtnjisuanqi)
		{
			if (m_pInputRichEdit)
			{
				CString strPath = CString(CPaintManagerUI::GetResourcePath()) + _T("..\\data\\Emotion\\Face\\2.gif");
				m_pInputRichEdit->InsertAniSmiley(strPath.GetBuffer());
			}
		}
		else if (msg.pSender == m_pBtnshock)
		{
			CString csTip = util::getMultilingual()->getStringViaID(_T("STRID_SESSIONMODULE_SHAKEWINDOW_SEND_TIP"));
			MessageEntity msg;
			msg.content = util::cStringToString(csTip);
			msg.sessionId = m_sId;
			msg.talkerSid = module::getSysConfigModule()->userID();
			msg.msgRenderType = MESSAGE_RENDERTYPE_SYSTEMTIPS;
			ReceiveMsgManage::getInstance()->pushMessageBySId(msg.sessionId, msg);
			logic::GetLogic()->asynNotifyObserver(module::KEY_SESSION_NEWMESSAGE, msg.sessionId);	//收到屏幕抖动消息提示

			logic::GetLogic()->asynNotifyObserver(module::KEY_SESSION_SHAKEWINDOW_MSG, m_sId);
			module::getP2PCmdModule()->tcpSendShakeWindowCMD(m_sId);
		}
		else if (msg.pSender == m_pBtnsendfile)
		{
		}
		else if (msg.pSender == m_pBtnadduser)
		{
			module::getGroupListModule()->onCreateDiscussionGrpDialog();
		}
	}
	else if (msg.sType == _T("return"))
	{
		if (msg.pSender == m_pInputRichEdit)
		{
			SendMsg();
		}
	}
	else if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_ITEMACTIVATE))
	{
		if (msg.pSender->GetName() == _T("ListGroupMembersItem"))
		{
			CListContainerElementUI* pListElement = static_cast<CListContainerElementUI*>(msg.pSender);
			if (!pListElement->GetUserData().IsEmpty())
			{
				std::string sid = util::cStringToString(CString(pListElement->GetUserData()));
				logic::GetLogic()->asynNotifyObserver(module::KEY_SESSION_OPENNEWSESSION, sid);//通知主窗口创建会话
			}
		}
	}
	else if (0 == _tcsicmp(msg.sType, UIIMEdit_MSGTYPE_TEXTCHANGED))
	{
		if (msg.pSender->GetName() == _T("input_richedit"))
		{
			if (!m_bGroupSession)
			{
				module::getP2PCmdModule()->tcpSendWritingCMD(m_sId, TRUE);
				m_pManager->KillTimer(this, TIMER_CHECK_WRITING);
				m_pManager->SetTimer(this, TIMER_CHECK_WRITING, 5000);
			}
		}
	}
	else if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_KILLFOCUS))
	{
		if (!m_bGroupSession)
		{
			m_pManager->KillTimer(this, TIMER_CHECK_WRITING);
			module::getP2PCmdModule()->tcpSendWritingCMD(m_sId, FALSE);
		}
	}
	else if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_MENU))
	{
		int n = 100;
		n++;
	}
}
void SessionLayout::DoEvent(TEventUI& event)
{
	if (event.Type == UIEVENT_TIMER  )
	{
		if (event.pSender == this && !m_bGroupSession && TIMER_CHECK_WRITING == event.wParam)
		{
			module::getP2PCmdModule()->tcpSendWritingCMD(m_sId, FALSE);
		}
	}
	else if (event.Type == UIEVENT_CONTEXTMENU)
	{
		int	n = 100;
		n++;
	}
}
/******************************************************************************/