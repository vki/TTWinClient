/******************************************************************************* 
 *  @file      SessionLayout_Function.cpp 2014\8\15 13:26:01 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "Modules/MessageEntity.h"
#include "Modules/UI/SessionLayout.h"
#include "Modules/ISysConfigModule.h"
#include "Modules/ISessionModule.h"
#include "Modules/IUserListModule.h"
#include "Modules/IUserListModule.h"
#include "utility/Multilingual.h"
#include "utility/utilStrCodeAPI.h"
#include "TTLogic/ITcpClientModule.h"
#include "../../Message/ReceiveMsgManage.h"
#include "json/reader.h"
#include "json/writer.h"
#include "UIIMEdit.h"
/******************************************************************************/
namespace
{
	const CString CS_SPLIT_CODE_START = _T("&$#@~^@[{:");
	const CString CS_SPLIT_CODE_END = _T(":}]&$~@#@");
}
void SessionLayout::SendMsg()
{
	MessageEntity msg;
	module::UserInfoEntity myInfo;
	module::getUserListModule()->getMyInfo(myInfo);
	UInt8 netState = logic::getTcpClientModule()->getTcpClientNetState();
	if (logic::TCPCLIENT_STATE_OK == netState && USER_STATUS_OFFLINE != myInfo.onlineState)
	{
		msg.msgType = MSG_TYPE_TEXT_P2P;
		msg.talkerSid = module::getSysConfigModule()->userID();
		msg.sessionId = m_sId;
		msg.msgRenderType = MESSAGE_RENDERTYPE_TEXT;
		msg.msgStatusType = MESSAGE_TYPE_RUNTIME;
		CString strContent = _getGetContent();	//如果是图片，在这里会发送图片
		if (strContent.IsEmpty())
		{
			return;
		}
		msg.content = util::cStringToString(strContent);
		msg.msgTime = module::getSessionModule()->getTime();
		_displayMsgToIE(msg, _T("sendMessage"));
	}
	else
	{
		MessageEntity msg;
		msg.content = util::cStringToString(util::getMultilingual()->getStringViaID(_T("STRID_SESSIONMODULE_OFFLINE_SENDMSG_TIP")));
		msg.sessionId = m_sId;
		msg.talkerSid = module::getSysConfigModule()->userID();
		msg.msgRenderType = MESSAGE_RENDERTYPE_SYSTEMTIPS;
		ReceiveMsgManage::getInstance()->pushMessageBySId(m_sId, msg);
		logic::GetLogic()->asynNotifyObserver(module::KEY_SESSION_NEWMESSAGE, msg.sessionId);	//发送消息太快
	}
	
}

BOOL SessionLayout::_displayMsgToIE(IN MessageEntity msg ,IN CString jsInterface)
{
	module::UserInfoEntity userInfo;
	if (!module::getUserListModule()->getUserInfoBySId(msg.talkerSid, userInfo))
	{
		return FALSE;
	}

	Json::Value root;
	root["name"] = util::cStringToString(userInfo.getRealName());
	root["avatar"] = userInfo.getAvatarPathWithoutOnlineState();
	root["msgtype"] = msg.msgRenderType;
	root["uuid"] = msg.talkerSid;
	CString csContent = util::stringToCString(msg.content);
	ReceiveMsgManage::getInstance()->parseContent(csContent, FALSE, GetWidth(), !msg.isMySendMsg());
	root["content"] = util::cStringToString(csContent);
	if (msg.isMySendMsg())
	{
		root["mtype"] = "me";
	}
	else
	{
		root["mtype"] = "other";
	}
	CTime timeData(msg.msgTime);
	root["time"] = util::cStringToString(timeData.Format(_T("%Y-%m-%d %H:%M:%S")));
	Json::StyledWriter styleWrite;
	std::string record = styleWrite.write(root);
	Json::Reader jsonRead;
	Json::Value rootRead;
	CString jsData = _T("[]");
	if (!jsonRead.parse(record, rootRead) || rootRead.isNull())
	{
		CString csError = util::stringToCString(record, CP_UTF8);
		APP_LOG(LOG_INFO, TRUE, _T("json parse error:%s"), csError);
		jsData = _T("[]");
		return FALSE;
	}
	else
		jsData = util::stringToCString(record, CP_UTF8);
	//调用页面的JS代码
	if (m_pWebBrowser)
	{
		VARIANT VarResult;
		m_pWebBrowser->CallJScript(jsInterface.GetBuffer(), jsData.GetBuffer(), &VarResult);
		jsData.ReleaseBuffer();
	}
	return TRUE;
}

CString MixedMsg::_getPicMsgByAddPicTeg(IN CString picPath)
{
	CString strAddedMSG;
	strAddedMSG += CS_SPLIT_CODE_START;
	strAddedMSG += picPath;
	strAddedMSG += CS_SPLIT_CODE_END;
	return strAddedMSG;
}
/******************************************************************************/