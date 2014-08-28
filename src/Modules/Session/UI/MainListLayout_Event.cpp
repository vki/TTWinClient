/******************************************************************************* 
 *  @file      MainListLayout_Event.cpp 2014\8\11 14:16:22 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "Modules/IUserListModule.h"
#include "Modules/IGroupListModule.h"
#include "Modules/ISessionModule.h"
#include "Modules/ISysConfigModule.h"
#include "Modules/UI/MainListLayout.h"
#include "Modules/UI/UIEAUserTreelist.h"
#include "Modules/UI/UIGroupsTreelist.h"
#include "Modules/UI/UISessionList.h"
#include "Modules/UI/SessionDialog.h"
#include "utility/Multilingual.h"
#include "utility/utilStrCodeAPI.h"
#include "../SessionManager.h"
#include "../../Message/ReceiveMsgManage.h"
#include "../../Message/SendMsgManage.h"
#include "Modules/MessageEntity.h"
//#include "../../../teamtalk/UI/UIMenu.h"
/******************************************************************************/


void MainListLayout::OnUserlistModuleEvent(UInt16 moduleId, UInt32 keyId, MKO_TUPLE_PARAM mkoParam)
{
	if (module::KEY_USERLIST_UPDATE_DEPARTMENTLIST == keyId)
	{
		_LoadAllDepartment();
	}
	else if (module::KEY_USERLIST_UPDATE_RECENTLISTLIST == keyId)
	{
		_AddRecentUserListToUI();
	}
	else if (module::KEY_USERLIST_UPDATE_NEWUSESADDED == keyId)
	{
		//TODO:新的用户，或者一堆用户更新
	}
}

void MainListLayout::OnGrouplistModuleEvent(UInt16 moduleId, UInt32 keyId, MKO_TUPLE_PARAM mkoParam)
{
	if (module::KEY_GROUPLIST_UPDATE_GROUPLIST == keyId)
	{
		_AddGroupList();
	}
	else if (module::KEY_GROUPLIST_UPDATE_GROUPDISCUSSLIST == keyId)
	{
		_AddDiscussGroupList();
	}
	else if (module::KEY_GROUPLIST_UPDATE_RECENTGROUPLIST == keyId)
	{
		_AddRecentGroupListToUI();
	}
	else if (module::KEY_GROUPLIST_UPDATE_NEWGROUPADDED == keyId)
	{		
		_NewGroupAdded(std::get<MKO_STRING>(mkoParam));
	}
	else if (module::KEY_GROUPLIST_UPDATE_CREATNEWGROUP == keyId)//新讨论组创建
	{
		_CreatNewDiscussGroupRes(std::get<MKO_STRING>(mkoParam));
	}
}
void MainListLayout::OnSessionModuleEvent(UInt16 moduleId, UInt32 keyId, MKO_TUPLE_PARAM mkoParam)
{
	if (module::KEY_SESSION_NEWMESSAGE == keyId)
	{
		_NewMsgUpdate(std::get<MKO_STRING>(mkoParam));
	}
	else if (module::KEY_SESSION_OPENNEWSESSION == keyId)
	{
		std::string sId = std::get<MKO_STRING>(mkoParam);
		if (module::getSysConfigModule()->userID() != sId)
		{
			SessionEntityManager::getInstance()->createSessionEntity(sId);
			SessionDialogManager::getInstance()->openSessionDialog(sId);
			m_UIRecentConnectedList->ClearItemMsgCount(sId);//清除显示的未读计数
		}
		else
		{
			APP_LOG(LOG_DEBUG, _T("点击的是自己"));
		}

	}
	else if (module::KEY_SESSION_SENDMSG_TOOFAST == keyId)
	{
		std::string& sId = std::get<MKO_STRING>(mkoParam);
		SessionDialog* pSessionDialog = SessionDialogManager::getInstance()->findSessionDialogBySId(sId);
		if (pSessionDialog)
		{
			MessageEntity msg;
			msg.content = util::cStringToString(util::getMultilingual()->getStringViaID(_T("STRID_SESSIONMODULE_SENDMSG_TOOFAST")));
			msg.sessionId = sId;
			msg.talkerSid = module::getSysConfigModule()->userID(); 
			msg.msgRenderType = MESSAGE_RENDERTYPE_SYSTEMTIPS;
			ReceiveMsgManage::getInstance()->pushMessageBySId(sId, msg);
			logic::GetLogic()->asynNotifyObserver(module::KEY_SESSION_NEWMESSAGE, msg.sessionId);	//发送消息太快
		}
	}
	else if (module::KEY_SESSION_SENDMSG_FAILED == keyId)
	{
		SendingMsgList FailedMsgList;
		SendMsgManage::getInstance()->getSendFailedMsgs(FailedMsgList);
		CString csErrorTipFormat = util::getMultilingual()->getStringViaID(_T("STRID_SESSIONMODULE_SENDMSG_FAIL"));
		for (SendingMsg failedmsg:FailedMsgList)
		{
			CString csContent = util::stringToCString(failedmsg.msg.content);
			CString csErrorTip;
			MessageEntity msg;
			csErrorTip.Format(csErrorTipFormat, csContent);
			msg.content = util::cStringToString(csErrorTip);
			msg.sessionId = failedmsg.msg.sessionId;
			msg.talkerSid = module::getSysConfigModule()->userID();
			msg.msgRenderType = MESSAGE_RENDERTYPE_SYSTEMTIPS;
			ReceiveMsgManage::getInstance()->pushMessageBySId(msg.sessionId, msg);
			logic::GetLogic()->asynNotifyObserver(module::KEY_SESSION_NEWMESSAGE, msg.sessionId);	//发送消息太快
		}
	}
	else if (module::TAG_SESSION_TRAY_NEWMSGSEND == keyId)
	{
		std::string& sId = std::get<MKO_STRING>(mkoParam);
		if (!m_UIRecentConnectedList->IsExistSId(sId))
			m_UIRecentConnectedList->AddNode(sId);
		m_UIRecentConnectedList->UpdateItemBySId(sId);
		m_UIRecentConnectedList->sort();
	}
}
void MainListLayout::OnSysConfigModuleEvent(UInt16 moduleId, UInt32 keyId, MKO_TUPLE_PARAM mkoParam)
{
	if (module::KEY_SYSCONFIG_UPDATED == keyId)
	{
		module::TTConfig* pTTConfig = module::getSysConfigModule()->getSystemConfig();
		if (pTTConfig)
		{
			return;
		}
		if (pTTConfig->sysBaseFlag & module::BASE_FLAG_TOPMOST)
			::SetWindowPos(m_pManager->GetPaintWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			::SetWindowPos(m_pManager->GetPaintWindow(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

}
void MainListLayout::_creatSessionDialog(IN UIIMList* pList, IN CControlUI* pMsgSender)
{
	if (nullptr == pList || pMsgSender == nullptr)
	{
		return;
	}
	if (-1 != pList->GetItemIndex(pMsgSender)
		&& 0 == _tcsicmp(pMsgSender->GetClass(), _T("ListContainerElementUI")))
	{
		Node* node = (Node*)pMsgSender->GetTag();
		if (!pList->CanExpand(node))
		{
			CString csId = node->data().sId;
			if (csId.IsEmpty())
				return;
			std::string sId = util::cStringToString(csId);
			SessionEntityManager::getInstance()->createSessionEntity(sId);
			SessionDialogManager::getInstance()->openSessionDialog(sId);

			m_UIRecentConnectedList->ClearItemMsgCount(sId);//清除显示的未读计数
			//停止托盘闪烁
			logic::GetLogic()->asynNotifyObserver(module::TAG_SESSION_TRAY_STOPEMOT);
		}
	}
}

void MainListLayout::Notify(TNotifyUI& msg)
{
	if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_ITEMCLICK))
	{
		if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			if (m_Tab)
			{
				if (0 == m_Tab->GetCurSel() && m_EAuserTreelist)
				{
					Node* node = (Node*)msg.pSender->GetTag();

					if (m_EAuserTreelist->CanExpand(node))
					{
						m_EAuserTreelist->SetChildVisible(node, !node->data().child_visible_);
					}
				}
				else if (1 == m_Tab->GetCurSel() && m_GroupList)
				{
					Node* node = (Node*)msg.pSender->GetTag();

					if (m_GroupList->CanExpand(node))
					{
						m_GroupList->SetChildVisible(node, !node->data().child_visible_);
					}
				}

			}
		}
	}
	else if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_SELECTCHANGED))
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("friendbtn")) == 0)
		{
			if (m_Tab && m_Tab->GetCurSel() != 0)
			{
				m_Tab->SelectItem(0);
			
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("groupbtn")) == 0)
		{
			if (m_Tab && m_Tab->GetCurSel() != 1)
			{
				m_Tab->SelectItem(1);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("RecentlyListbtn")) == 0)
		{
			if (m_Tab && m_Tab->GetCurSel() != 2)
			{
				m_Tab->SelectItem(2);
			}
		}
		test();
	}
	else if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_ITEMACTIVATE))
	{
		if (m_Tab->GetCurSel() == 0)
		{
			_creatSessionDialog(m_EAuserTreelist, msg.pSender);
		}
		else if (1 == m_Tab->GetCurSel())
		{
			_creatSessionDialog(m_GroupList, msg.pSender);
		}
		else if (m_Tab->GetCurSel() == 2)
		{
			_creatSessionDialog(m_UIRecentConnectedList, msg.pSender);
		}
	}
	else if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_CLICK))
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("searchbtn")) == 0)
		{

		}
	}
	else if (0 == _tcsicmp(msg.sType, DUI_MSGTYPE_MENU))
	{
		//CMenuWnd* pMenu = new CMenuWnd(NULL);
		//DuiLib::CPoint point = msg.ptMouse;
		//ClientToScreen(m_pManager->GetPaintWindow(), &point);
		//STRINGorID xml(_T("menu\\lineStatus.xml"));
		//pMenu->Init(reinterpret_cast<CMenuElementUI*>(this), xml, _T("xml"), point);
	}
}
void MainListLayout::DoEvent(TEventUI& event)
{
	if (event.Type == UIEVENT_TIMER)
	{
	}
}


/******************************************************************************/