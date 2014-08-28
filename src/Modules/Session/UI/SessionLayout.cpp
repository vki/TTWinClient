/******************************************************************************* 
 *  @file      SessionLayout.cpp 2014\8\5 20:25:34 $
 *  @author    大佛<dafo@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "Modules/UI/SessionLayout.h"
#include <windows.h>
#include <shellapi.h>
#include <comutil.h>
#include "src/base/ImPduClient.h"
#include "src/base/ImPduGroup.h"
#include "json/writer.h"
#include "json/reader.h"
#include "utility/utilStrCodeAPI.h"
#include "Modules/MessageEntity.h"
#include "Modules/IMiscModule.h"
#include "Modules/IGroupListModule.h"
#include "Modules/ISysConfigModule.h"
#include "Modules/IMessageModule.h"
#include "Modules/ISessionModule.h"
#include "Modules/IUserListModule.h"
#include "TTLogic/ITcpClientModule.h"
#include "../Operation/SendImgHttpOperation.h"
#include "../../Message/SendMsgManage.h"
#include "../../Message/ReceiveMsgManage.h"
#include "../SessionManager.h"
#include "UIIMEdit.h"
/******************************************************************************/

// -----------------------------------------------------------------------------
//  SessionLayout: Public, Constructor


int CALLBACK GroupMemberListItemCompareFunc(UINT_PTR pa, UINT_PTR pb, UINT_PTR pUser)
{
	CListContainerElementUI* pListElement1 = (CListContainerElementUI*)pa;
	CListContainerElementUI* pListElement2 = (CListContainerElementUI*)pb;
	CDuiString node1 = pListElement1->GetUserData();
	CDuiString node2 = pListElement2->GetUserData();

	//获取会话的服务器时间

	CString s1 = node1;
	CString s2 = node2;
	if (s1.IsEmpty() || s2.IsEmpty())
	{
		return 0;
	}
	std::string sid1 = util::cStringToString(s1);
	std::string sid2 = util::cStringToString(s2);

	module::UserInfoEntity userInfo1;
	module::UserInfoEntity userInfo2;
	if (!(module::getUserListModule()->getUserInfoBySId(sid1, userInfo1)
		&& module::getUserListModule()->getUserInfoBySId(sid2, userInfo2)))
	{
		return 0;
	}

	if (userInfo1.onlineState == userInfo2.onlineState)
	{
		return 0;
	}

	int  nRes = userInfo1.onlineState < userInfo2.onlineState ? -1 : 1;
	return nRes;
}


SessionLayout::SessionLayout(const std::string& sId, CPaintManagerUI& paint_manager)
:m_pWebBrowser(nullptr)//聊天显示框
, m_pInputRichEdit(nullptr)
, m_pBtnSendMsg(nullptr)
, m_pBtnClose(nullptr)
, m_pBtnEmotion(nullptr)
, m_pBtnSendImage(nullptr)
, m_pBtnscreenshot(nullptr)
, m_pBtnjisuanqi(nullptr)
, m_pBtnshock(nullptr)
, m_pBtnsendfile(nullptr)
, m_pBtnadduser(nullptr)//添加讨论组成员
, m_sId(sId)
, m_paint_manager(paint_manager)
, m_bGroupSession(false)
, m_pSendDescription(nullptr)
, m_bottomLayout(nullptr)
{

}

// -----------------------------------------------------------------------------
//  SessionLayout: Public, Destructor

SessionLayout::~SessionLayout()
{
	m_pManager->RemoveNotifier(this);
	if (m_pInputRichEdit)
	{
		m_pInputRichEdit->ReleaseAllGif();
	}
}

void SessionLayout::DoInit()
{
	m_pManager->AddNotifier(this);
}



CString SessionLayout::_getGetContent()
{
	CString strContent;

	CRichEditUI* pInputRichEdit = m_pInputRichEdit;
	if (nullptr == pInputRichEdit)
	{
		return _T("");
	}
	pInputRichEdit->SetFocus();

	strContent = pInputRichEdit->GetTextRange(0, pInputRichEdit->GetTextLength());
	if (strContent.IsEmpty()) return _T("");

	IRichEditOle *pRichEditOle = pInputRichEdit->GetRichEditOle();
	if (NULL == pRichEditOle)
	{
		return _T("");
	}
	UInt32 nImageCount = pRichEditOle->GetObjectCount();
	if (nImageCount == 0)//纯文字，直接发送消息
	{
		MessageEntity msg;
		msg.content = util::cStringToString(strContent);
		msg.sessionId = m_sId;
		msg.talkerSid = module::getSysConfigModule()->userID();
		msg.msgRenderType = MESSAGE_RENDERTYPE_TEXT;
		SessionEntity* pSessionInfo = SessionEntityManager::getInstance()->getSessionEntityBySId(m_sId);
		
		if (!pSessionInfo)
		{
			return _T("");
		}
		
		if (pSessionInfo->m_sessionType == SESSION_USERTYPE)
		{
			msg.msgType = MSG_TYPE_TEXT_P2P;
		}
		else
		{
			msg.msgType = MSG_TYPE_TEXT_GROUP;
		}

		msg.msgFromType = pSessionInfo->m_sessionType;	//sessionType和FromType定义一致
		msg.msgTime = module::getSessionModule()->getTime();
		SendMsgManage::getInstance()->pushSendingMsg(msg);

		//更新会话时间
		SessionEntity*  pSessionEntity = SessionEntityManager::getInstance()->getSessionEntityBySId(msg.sessionId);
		if (pSessionEntity)
		{
			pSessionEntity->m_updatedTime = msg.msgTime;
		}
		//主界面 消息内容，时间更新
		logic::GetLogic()->asynNotifyObserver(module::TAG_SESSION_TRAY_NEWMSGSEND, msg.sessionId);
	}
	else//图文混排
	{
		MixedMsg mixMsg;
		mixMsg.m_strTEXT = strContent;
		int nPosAdd = 0;
		for (int i = 0; i < pRichEditOle->GetObjectCount(); i++)
		{
			ST_picData picData;
			if (m_pInputRichEdit->GetPicPosAndPathbyOrder(i, picData.nPos,picData.strLocalPicPath))
			{
				mixMsg.m_picDataVec.push_back(picData);
				//有图片的话，需要上传
				SendImgParam param;
				param.csFilePath = picData.strLocalPicPath;
				SendImgHttpOperation* pOper = new SendImgHttpOperation(param
					, fastdelegate::MakeDelegate(this, &SessionLayout::onCallbackOperation));
				module::getHttpPoolModule()->pushHttpOperation(pOper);
			}
		}
		m_SendingMixedMSGList.push_back(mixMsg);
		strContent = mixMsg.makeMixedLocalMSG();
	}

	pInputRichEdit->SetText(_T(""));
	return strContent;
}

//int SessionLayout::GetObjectPos(CRichEditUI* pRichEdit)
//{
//	bool findObject = false;
//
//
//	IRichEditOle *pRichEditOle = pRichEdit->GetRichEditOle();
//	if (NULL == pRichEditOle)
//	{
//		return 0;
//	}
//
//	int nCount = pRichEditOle->GetObjectCount();
//	for (int i = nCount - 1; i >= 0; i--)
//	{
//		REOBJECT reobj = { 0 };
//		reobj.cbStruct = sizeof(REOBJECT);
//		pRichEditOle->GetObject(i, &reobj, REO_GETOBJ_POLEOBJ);
//		reobj.poleobj->Release();
//	}
//	return 0;
//}


STDAPI_(void) OleUIMetafilePictIconFree(HGLOBAL hMetaPict)
{
	LPMETAFILEPICT pMF;

	if (NULL == hMetaPict)
		return;

	pMF = (LPMETAFILEPICT)GlobalLock(hMetaPict);

	if (NULL != pMF)
	{
		if (NULL != pMF->hMF)
			DeleteMetaFile(pMF->hMF);
	}

	GlobalUnlock(hMetaPict);
	GlobalFree(hMetaPict);
	return;
}

//bool SessionLayout::InsertAniSmiley(CRichEditUI* pRichEdit, long hwndHostWindow, BSTR bstrFileName, OLE_COLOR clr, INT cy)
//{
//	GifSmiley::IGifSmileyCtrl* lpAnimator = nullptr;
//	::CoCreateInstance(GifSmiley::CLSID_CGifSmileyCtrl, NULL, CLSCTX_INPROC, GifSmiley::IID_IGifSmileyCtrl, (LPVOID*)&lpAnimator);
//
//	if (lpAnimator == NULL)
//	{
//		return false;
//	}
//
//	COLORREF backColor = (COLORREF)(clr);
//	HWND hwnd = (HWND)(hwndHostWindow);
//	HRESULT hr;
//
//
//	IRichEditOle *pRichEditOle = pRichEdit->GetRichEditOle();
//	if (NULL == pRichEditOle)
//	{
//		return false;
//	}
//
//	LPSTORAGE lpStorage = NULL;
//	LPOLEOBJECT	lpObject = NULL;
//	LPLOCKBYTES lpLockBytes = NULL;
//	LPOLECLIENTSITE lpClientSite = NULL;
//	BSTR path = NULL;
//
//	//Create lockbytes
//	hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
//	if (FAILED(hr))
//	{
//		return	 false;
//	}
//	//use lockbytes to create storage
//	SCODE sc = ::StgCreateDocfileOnILockBytes(lpLockBytes, STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &lpStorage);
//	if (sc != S_OK)
//	{
//		lpLockBytes->Release();
//		return false;
//	}
//
//	// retrieve OLE interface for richedit   and  Get site
//	pRichEditOle->GetClientSite(&lpClientSite);
//
//	try
//	{
//		//COM operation need BSTR, so get a BSTR
//		path = bstrFileName;
//
//		//Load the gif
//		lpAnimator->LoadFromFileSized(path, cy);
//
//		//Set back color
//		OLE_COLOR oleBackColor = (OLE_COLOR)backColor;
//		lpAnimator->put_BackColor(oleBackColor);
//
//
//		//get the IOleObject
//		hr = lpAnimator->QueryInterface(IID_IOleObject, (void**)&lpObject);
//		if (FAILED(hr))
//		{
//			return	 false;
//		}
//
//		//Set it to be inserted
//		OleSetContainedObject(lpObject, TRUE);
//
//		//to insert into richedit, you need a struct of REOBJECT
//		REOBJECT reobject;
//		ZeroMemory(&reobject, sizeof(REOBJECT));
//
//		reobject.cbStruct = sizeof(REOBJECT);
//
//		CLSID clsid;
//		hr = lpObject->GetUserClassID(&clsid);
//
//
//		//set clsid
//		reobject.clsid = clsid;
//		//can be selected
//		reobject.cp = REO_CP_SELECTION;
//		//content, but not static
//		reobject.dvaspect = DVASPECT_CONTENT;
//		//goes in the same line of text line
//		reobject.dwFlags = REO_BELOWBASELINE;
//		//reobject.dwUser = (DWORD)myObject;
//
//		//the very object
//		reobject.poleobj = lpObject;
//		//client site contain the object
//		reobject.polesite = lpClientSite;
//		//the storage 
//		reobject.pstg = lpStorage;
//
//		SIZEL sizel = { 0 };
//		reobject.sizel = sizel;
//
//
//		LPOLECLIENTSITE lpObjectClientSite = NULL;
//		hr = lpObject->GetClientSite(&lpObjectClientSite);
//		if (FAILED(hr) || lpObjectClientSite == NULL)
//			lpObject->SetClientSite(lpClientSite);
//
//		pRichEditOle->InsertObject(&reobject);
//		//redraw the window to show animation
//		::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
//
//		if (lpClientSite)
//		{
//			lpClientSite->Release();
//			lpClientSite = NULL;
//		}
//		if (lpObject)
//		{
//			lpObject->Release();
//			lpObject = NULL;
//		}
//		if (lpLockBytes)
//		{
//			lpLockBytes->Release();
//			lpLockBytes = nullptr;
//		}
//		if (lpStorage)
//		{
//			lpStorage->Release();
//			lpStorage = NULL;
//		}
//
//		if (lpAnimator)
//		{
//			lpAnimator->Release();
//			lpAnimator = NULL;
//		}
//		if (pRichEditOle)
//		{
//			pRichEditOle->Release();
//		}
//		
//
//	}
//	catch (...)
//	{
//		if (lpClientSite)
//		{
//			lpClientSite->Release();
//			lpClientSite = NULL;
//		}
//		if (lpObject)
//		{
//			lpObject->Release();
//			lpObject = NULL;
//		}
//		if (lpStorage)
//		{
//			lpStorage->Release();
//			lpStorage = NULL;
//		}
//
//		if (lpAnimator)
//		{
//			lpAnimator->Release();
//			lpAnimator = NULL;
//		}
//
//		return false;
//	}
//	return true;
//}
//


//HRESULT SessionLayout::GetNewStorage(LPSTORAGE* ppStg)
//{
//
//	if (!ppStg)
//		return E_INVALIDARG;
//
//	*ppStg = NULL;
//
//	//
//	// We need to create a new storage for an object to occupy.  We're going
//	// to do this the easy way and just create a storage on an HGLOBAL and let
//	// OLE do the management.  When it comes to saving things we'll just let
//	// the RichEdit control do the work.  Keep in mind this is not efficient, 
//	// but this program is just for demonstration.
//	//
//
//	LPLOCKBYTES pLockBytes;
//	HRESULT hr = CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);
//	if (FAILED(hr))
//		return hr;
//
//	hr = StgCreateDocfileOnILockBytes(pLockBytes,
//		STGM_SHARE_EXCLUSIVE | STGM_CREATE |
//		STGM_READWRITE,
//		0,
//		ppStg);
//	pLockBytes->Release();
//	return (hr);
//}

//void SessionLayout::_InsertPic(IN CString strFilePath)
//{
//	CRichEditUI* pInputRichEdit = m_pInputRichEdit;
//	if (nullptr == pInputRichEdit)
//	{
//		return;
//	}
//
//	InsertAniSmiley(pInputRichEdit, (long)m_pManager->GetPaintWindow()
//		, strFilePath.GetBuffer(), ::GetSysColor(COLOR_WINDOW), 0);
//}



//void SessionLayout::GetObjectInfo(IRichEditOle *pIRichEditOle)
//{
//	long count = pIRichEditOle->GetObjectCount();
//	if (count)
//	{
//		REOBJECT reobj = { 0 };
//		reobj.cbStruct = sizeof(REOBJECT);
//		pIRichEditOle->GetObject(0, &reobj, REO_GETOBJ_POLEOBJ);
//		GifSmiley::IGifSmileyCtrl* lpAnimator;
//		HRESULT hr = reobj.poleobj->QueryInterface(GifSmiley::IID_IGifSmileyCtrl, (void**)&lpAnimator);
//		if (SUCCEEDED(hr))
//		{
//			BSTR*  fileName = nullptr;
//			hr = lpAnimator->FileName(fileName);
//		}
//		reobj.poleobj->Release();
//	}
//}

//void SessionLayout::ReleaseAllGif()
//{
//	CRichEditUI* pRichEdit = m_pInputRichEdit;
//	IRichEditOle *pRichEditOle = pRichEdit->GetRichEditOle();
//	if (NULL == pRichEditOle)
//	{
//		return;
//	}
//
//	for (int i = 0; i < pRichEditOle->GetObjectCount(); i++)
//	{
//		REOBJECT reobj = { 0 };
//		reobj.cbStruct = sizeof(REOBJECT);
//		pRichEditOle->GetObject(i, &reobj, REO_GETOBJ_POLEOBJ);
//		GifSmiley::IGifSmileyCtrl* lpAnimator;
//		HRESULT hr = reobj.poleobj->QueryInterface(GifSmiley::IID_IGifSmileyCtrl, (void**)&lpAnimator);
//		if (SUCCEEDED(hr))
//		{
//			BSTR filename;
//			lpAnimator->FileName(&filename);
//			lpAnimator->FreeImage();
//			lpAnimator->Release();
//		}
//		reobj.poleobj->Release();
//	}
//}

void SessionLayout::OnWindowInitialized(TNotifyUI& msg)
{
	m_pWebBrowser = dynamic_cast<CWebBrowserUI*>(m_pManager->FindSubControlByName(this, _T("ie")));
	if (m_pWebBrowser) {
		m_pWebBrowser->SetWebBrowserEventHandler(this);
		//m_pWebBrowser->Navigate2(CPaintManagerUI::GetInstancePath() + _T("..\\data\\module\\module-content.html"));//大佛：调用两遍去3D边框
		m_pWebBrowser->Navigate2(CPaintManagerUI::GetInstancePath() + _T("..\\data\\module\\module-content.html"));
	}

	m_pInputRichEdit = dynamic_cast <UIIMEdit*>(m_pManager->FindSubControlByName(this, _T("input_richedit")));
	m_pBtnSendMsg = dynamic_cast <CButtonUI*>(m_pManager->FindSubControlByName(this, _T("sendbtn")));
	m_pBtnClose = dynamic_cast <CButtonUI*>(m_pManager->FindSubControlByName(this, _T("closebtn")));

	m_pBtnEmotion = dynamic_cast <COptionUI*>(m_pManager->FindSubControlByName(this, _T("emotionbtn")));
	m_pBtnSendImage = dynamic_cast <CButtonUI*>(m_pManager->FindSubControlByName(this, _T("imagebtn")));
	m_pBtnscreenshot = dynamic_cast <COptionUI*>(m_pManager->FindSubControlByName(this, _T("screenshotbtn")));
	m_pBtnjisuanqi = dynamic_cast <CButtonUI*>(m_pManager->FindSubControlByName(this, _T("jisuanqibtn")));
	m_pBtnshock = dynamic_cast <CButtonUI*>(m_pManager->FindSubControlByName(this, _T("shockbtn")));
	m_pBtnsendfile = dynamic_cast <CButtonUI*>(m_pManager->FindSubControlByName(this, _T("sendfilebtn")));
	m_pBtnadduser = dynamic_cast <CButtonUI*>(m_pManager->FindSubControlByName(this, _T("adduserbtn")));
	m_pSendDescription = dynamic_cast <CTextUI*>(m_pManager->FindSubControlByName(this, _T("senddescription")));
	m_bottomLayout = dynamic_cast<CHorizontalLayoutUI*>(m_pManager->FindSubControlByName(this, _T("bottomLayout")));
	SessionEntity* pSessionInfo = SessionEntityManager::getInstance()->getSessionEntityBySId(m_sId);
	if (!pSessionInfo)
	{
		return;
	}
	if (SESSION_GROUPTYPE == pSessionInfo->m_sessionType)
	{
		m_pBtnshock->SetVisible(false);
		m_pBtnsendfile->SetVisible(false);
		m_bGroupSession = TRUE;
		m_pSearchResultList = (CListUI*)m_pManager->FindSubControlByName(this, _T("SearchResults"));
		m_pGroupMemberList = (CListUI*)m_pManager->FindSubControlByName(this, _T("GroupMembers"));
		m_pRightContainer = (CContainerUI*)m_pManager->FindSubControlByName(this, _T("right_part"));
		//m_pGroupMemberList->SetItemNormalHeight(40);
		//m_pGroupMemberList->SetIitemXmlFile(_T("SessionDialog\\groupMembersListItem.xml"));
		_updateGroupMembersList();
		m_pRightContainer->SetVisible(true);
	}

	UpdateSendMsgKey();
}

void SessionLayout::onCallbackOperation(std::shared_ptr<void> param)
{
	SendImgParam* pParam = (SendImgParam*)param.get();
	if (pParam == nullptr)
	{
		return;
	}
	if (SendImgParam::SENDIMG_OK == pParam->m_result)
	{
		for (auto mixedMsgIt = m_SendingMixedMSGList.begin(); mixedMsgIt != m_SendingMixedMSGList.end(); mixedMsgIt++)
		{
			for (auto picdata : mixedMsgIt->m_picDataVec)
			{
				if (picdata.strLocalPicPath == pParam->csFilePath)
				{
					mixedMsgIt->SetNetWorkPicPath(pParam->csFilePath, util::stringToCString(pParam->m_pathUrl));
					if (mixedMsgIt->SucceedToGetAllNetWorkPic())
					{
						MessageEntity msg;
						msg.content = util::cStringToString(mixedMsgIt->makeMixedNetWorkMSG());
						msg.sessionId = m_sId;
						msg.talkerSid = module::getSysConfigModule()->userID();
						
						SessionEntity* pSessionInfo = SessionEntityManager::getInstance()->getSessionEntityBySId(m_sId);
						
						if (!pSessionInfo)
						{
							return;
						}
						if (pSessionInfo->m_sessionType == SESSION_USERTYPE)
						{
							msg.msgType = MSG_TYPE_TEXT_P2P;
						}
						else
						{
							msg.msgType = MSG_TYPE_TEXT_GROUP;
						}
						msg.msgFromType = pSessionInfo->m_sessionType;	//sessionType和FromType定义一致
						msg.msgTime = module::getSessionModule()->getTime();
						SendMsgManage::getInstance()->pushSendingMsg(msg);
						m_SendingMixedMSGList.erase(mixedMsgIt);

						//更新会话时间
						SessionEntity*  pSessionEntity = SessionEntityManager::getInstance()->getSessionEntityBySId(msg.sessionId);
						if (pSessionEntity)
						{
							pSessionEntity->m_updatedTime = msg.msgTime; 
						}
						//主界面 消息内容，时间更新
						logic::GetLogic()->asynNotifyObserver(module::TAG_SESSION_TRAY_NEWMSGSEND, msg.sessionId);
						return;
					}
				}
			}
		}
	}
}

BOOL SessionLayout::_displayUnreadMsg()
{
	SessionChatMsg_List msgList;
	if (!ReceiveMsgManage::getInstance()->popAllMessageBySId(m_sId, msgList))
	{
		//没有未读消息
		return FALSE;
	}

	for (auto MessageInfo : msgList)
	{
		_displayMsgToIE(MessageInfo, _T("sendMessage"));
		//给接收到的消息增加offset计数
		module::getMessageModule()->countMsgOffset(MessageInfo.talkerSid, 1);
	}

	//保存到历史消息中
	module::getMessageModule()->sqlBatchInsertHistoryMsg(msgList);

	//发送已读确认 todo...
	auto msg = msgList.front();
	_sendReadAck(msg);
	return TRUE;
}

void SessionLayout::_sendReadAck(IN MessageEntity msg)
{
	logic::GetLogic()->pushBackOperationWithLambda(
		[=]()
	{
		MessageEntity sendingmsgTemp = msg;
		std::string OriginSessionId = sendingmsgTemp.getOriginSessionId();

		if (MESSAGETYPE_FROM_FRIEND == sendingmsgTemp.msgFromType)
		{
			CImPduClientMsgReadAck pduMsgData(OriginSessionId.c_str());
			logic::getTcpClientModule()->sendPacket(&pduMsgData);
		}
		else if (MESSAGETYPE_FROM_GROUP == sendingmsgTemp.msgFromType)
		{
			CImPduClientGroupMsgReadAck pduMsgData(OriginSessionId.c_str());
			logic::getTcpClientModule()->sendPacket(&pduMsgData);
		}
	}
	);
}

void SessionLayout::DocmentComplete(IDispatch *pDisp, VARIANT *&url)
{
	logic::GetLogic()->asynFireUIEventWithLambda(
		[=]()
	{
		if (!_displayUnreadMsg())
		{
			_displayHistoryMsg(20);
		}
	}
	);
	
}

void SessionLayout::UpdateRunTimeMsg()
{
	_displayUnreadMsg();
}

void SessionLayout::_updateGroupMembersList()
{
	if (!m_pRightContainer
		|| !m_pGroupMemberList)
	{
		return;
	}
	module::UserInfoEntityVec groupUserVec;
	if (!module::getGroupListModule()->getGroupUserVecBySId(m_sId, groupUserVec))
	{
		return;
	}

	for (std::string sid : groupUserVec)
	{
		_AddGroupMemberToList(sid);
	}
	m_pGroupMemberList->SortItems(GroupMemberListItemCompareFunc, 0);
}

void SessionLayout::_AddGroupMemberToList(IN const std::string& sID)
{
	module::UserInfoEntity userInfo;
	if (!module::getUserListModule()->getUserInfoBySId(sID, userInfo))
	{
		APP_LOG(LOG_ERROR, _T("SessionLayout::_updateGroupMembersList(),can't find the userInfo"));
		return;
	}
	CDialogBuilder dlgBuilder;
	CListContainerElementUI* pListElement = (CListContainerElementUI*)dlgBuilder.Create(_T("SessionDialog\\groupMembersListItem.xml"), (UINT)0, NULL, &m_paint_manager);
	if (!pListElement)
	{
		APP_LOG(LOG_ERROR, _T("群item创建失败"));
		return;
	}
	CButtonUI* pLogo = static_cast<CButtonUI*>(pListElement->FindSubControl(_T("AvatarInfo")));
	if (!pLogo)
	{
		return;
	}
	pLogo->SetBkImage(util::stringToCString(userInfo.getAvatarPath()));

	CLabelUI* pNameLable = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("nickname")));
	if (!pNameLable)
	{
		return;
	}
	pNameLable->SetText(userInfo.getRealName());
	pListElement->SetUserData(util::stringToCString(userInfo.sId));

	m_pGroupMemberList->Add(pListElement);

}

HRESULT STDMETHODCALLTYPE SessionLayout::TranslateUrl( /* [in] */ DWORD dwTranslate, /* [in] */ OLECHAR __RPC_FAR *pchURLIn, /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
{
	*ppchURLOut = 0;

	//历史消息内容
	CString csUrl = pchURLIn;
	if (csUrl.Find(_T("moguim/:history")) > -1)//显示历史消息
	{
		_displayHistoryMsg(20);
	}
	else if (csUrl.Find(_T("moguim/:playvoice")) > -1)//播放语音文件
	{
		int npos = csUrl.Find(_T("?"));
		if (-1 != npos)
		{
			////先停掉前面一个的播放动画
			//AudioMessageMananger::getInstance()->popPlayingAudioMsg();

			////播放当前选择的声音
			//string sAudioID = util::cStringToString(csUrl.Mid(npos + 1, csUrl.GetLength() - npos), CP_UTF8);
			//UserListViewItem* pSelItem = (UserListViewItem*)GetCurrentUserList()->GetFocusItem();
			//AudioMessageMananger::getInstance()->playAudioMsgByAudioSid(pSelItem->m_sId, sAudioID);

			////由于播放同一个文件两次，在第二次的时候，gif动画就不出来了，这个时候需要手动再调一下
			//if (pSelItem && pSelItem->m_pSessionChat)
			//{
			//	pSelItem->m_pSessionChat->StartPlayingAnimate(sAudioID);
			//}
		}
	}
	return S_OK;
}

void SessionLayout::NewWindow2(VARIANT_BOOL *&Cancel, BSTR bstrUrl)
{
	*Cancel = VARIANT_TRUE;
}

void SessionLayout::_displayHistoryMsg(IN UInt32 nMsgCount)
{
	std::vector<MessageEntity> msgList;
	if (module::getMessageModule()->sqlGetHistoryMsg(m_sId, nMsgCount, msgList))
	{
		//给接收到的消息增加offset计数
		module::getMessageModule()->countMsgOffset(m_sId, msgList.size());
	}
	

	if (msgList.empty())
	{
		//MessageEntity msg;
		//msg.content = util::cStringToString(_T("没有历史消息了"));
		//msg.sessionId = m_sId;
		//msg.talkerSid = module::getSysConfigModule()->userID();
		//msg.msgRenderType = MESSAGE_RENDERTYPE_SYSTEMTIPS;
		//ReceiveMsgManage::getInstance()->pushMessageBySId(msg.sessionId, msg);
		//logic::GetLogic()->asynNotifyObserver(module::KEY_SESSION_NEWMESSAGE, msg.sessionId);
		return;
	}

	Json::Value root;
	for (std::vector<MessageEntity>::reverse_iterator itMsg = msgList.rbegin();
		itMsg != msgList.rend();++itMsg)
	{
		module::UserInfoEntity userInfo;
		if (!module::getUserListModule()->getUserInfoBySId(itMsg->talkerSid, userInfo))
		{
			continue;
		}

		//组装json data
		Json::Value msgItem;
		msgItem["name"] = util::cStringToString(userInfo.getRealName());
		msgItem["avatar"] = userInfo.getAvatarPathWithoutOnlineState();
		if (itMsg->isMySendMsg())
		{
			msgItem["mtype"] = "me";
		}
		else
		{
			msgItem["mtype"] = "other";
		}
		CTime time(itMsg->msgTime);
		msgItem["time"] = util::cStringToString(time.Format(_T("%Y-%m-%d %H:%M:%S")));
		msgItem["uuid"] = itMsg->talkerSid;
		msgItem["msgtype"] = itMsg->msgRenderType;
		CString& csContent = util::stringToCString(itMsg->content);
		ReceiveMsgManage::getInstance()->parseContent(csContent, FALSE, GetWidth(), !itMsg->isMySendMsg());
		std::string content = util::cStringToString(csContent, CP_UTF8);
		msgItem["content"] = content;
		root.append(msgItem);
	}

	Json::StyledWriter styleWrite;
	std::string record = styleWrite.write(root);
	CString jsData = _T("[]");
	Json::Reader jsonRead;
	Json::Value rootRead;
	if (!jsonRead.parse(record, rootRead) || rootRead.isNull())
	{
		CString csError = util::stringToCString(record, CP_UTF8);
		APP_LOG(LOG_ERROR, _T("history is null or json parse error:%s"), csError);
		jsData = _T("[]");
	}
	else
		jsData = util::stringToCString(record);

	//调用js
	CStringArray param;
	param.Add(jsData);
	CComVariant result;
	BOOL bRet = m_pWebBrowser->CallJScript(_T("historyMessage"), param.GetData()->GetBuffer(), &result);
	if (!bRet)
		APP_LOG(LOG_ERROR, _T("DisplayToMsg CallJScript failed,%s"), jsData);
	
}

void SessionLayout::UpdateSendMsgKey()
{
	if (!m_pInputRichEdit)
	{
		return;
	}
	BOOL bWantCtrlEnter = m_pInputRichEdit->WantCtrlEnterBySysSetting();
	m_pInputRichEdit->SetWantReturn(!bWantCtrlEnter);
	if (bWantCtrlEnter)
	{
		m_pSendDescription->SetText(_T("按Ctrl + Enter发送"));
	}
	else
		m_pSendDescription->SetText(_T("Enter发送"));

	m_bottomLayout->NeedUpdate();
}

void SessionLayout::freshGroupMemberAvatar(IN const std::string& sID)
{
	module::UserInfoEntity userInfo;
	if (!module::getUserListModule()->getUserInfoBySId(sID, userInfo))
	{
		APP_LOG(LOG_ERROR, _T("SessionLayout::_updateGroupMembersList(),can't find the userInfo"));
		return;
	}

	if (!m_pGroupMemberList)
	{
		return;
	}
	for (int n = 0; n < m_pGroupMemberList->GetCount(); n++)
	{
		CListContainerElementUI* pItem = (CListContainerElementUI*)m_pGroupMemberList->GetItemAt(n);
		if (pItem)
		{
			CDuiString userData = pItem->GetUserData();
			if (!userData.IsEmpty())
			{
				std::string sid = util::cStringToString(CString(userData));
				if (sid == sID)
				{
					CButtonUI* btnAvatarInfo = (CButtonUI*)pItem->FindSubControl(_T("AvatarInfo"));
					if (btnAvatarInfo)
					{
						btnAvatarInfo->SetBkImage(util::stringToCString(userInfo.getAvatarPath()));
					}
				}
			}
		}
	}
	m_pGroupMemberList->SortItems(GroupMemberListItemCompareFunc, 0);
}

void SessionLayout::freshAllGroupMemberAvatar()
{
	if (!m_pGroupMemberList)
	{
		return;
	}
	for (int n = 0; n < m_pGroupMemberList->GetCount(); n++)
	{
		CListContainerElementUI* pItem = (CListContainerElementUI*)m_pGroupMemberList->GetItemAt(n);
		if (pItem)
		{
			CDuiString userData = pItem->GetUserData();
			if (!userData.IsEmpty())
			{
				std::string sid = util::cStringToString(CString(userData));
				module::UserInfoEntity userInfo;
				if (!module::getUserListModule()->getUserInfoBySId(sid, userInfo))
				{
					APP_LOG(LOG_ERROR, _T("SessionLayout::_updateGroupMembersList(),can't find the userInfo"));
					continue;;
				}
				CButtonUI* btnAvatarInfo = (CButtonUI*)pItem->FindSubControl(_T("AvatarInfo"));
				if (btnAvatarInfo)
				{
					btnAvatarInfo->SetBkImage(util::stringToCString(userInfo.getAvatarPath()));
				}
			}
		}
	}
	m_pGroupMemberList->SortItems(GroupMemberListItemCompareFunc, 0);
}

void SessionLayout::UpdateBottomLayout()
{
	if (m_bottomLayout)
	{
		NeedUpdate();
	}
}

/******************************************************************************/

MixedMsg::MixedMsg()
:m_nSetNetWorkPathSuccTime(0)
{

}

BOOL MixedMsg::SetNetWorkPicPath(IN CString strLocalPicPath, IN CString strNetPicPath)
{
	for (auto& picData : m_picDataVec)
	{
		if (picData.strLocalPicPath == strLocalPicPath)
		{
			picData.strNetPicPath = strNetPicPath;
			m_nSetNetWorkPathSuccTime++;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL MixedMsg::SucceedToGetAllNetWorkPic()
{
	return m_nSetNetWorkPathSuccTime == m_picDataVec.size();
}

CString MixedMsg::makeMixedLocalMSG()
{
	CString msg = m_strTEXT;
	int nPosAdd = 0;
	for (auto picData : m_picDataVec)
	{
		CString strPic = _getPicMsgByAddPicTeg(picData.strLocalPicPath);
		msg.Insert(nPosAdd + picData.nPos, strPic.GetBuffer());
		strPic.ReleaseBuffer();
		nPosAdd += strPic.GetLength();
	}
	return msg;
}

CString MixedMsg::makeMixedNetWorkMSG()
{
	CString msg = m_strTEXT;
	int nPosAdd = 0;
	for (auto picData : m_picDataVec)
	{
		CString strPic = _getPicMsgByAddPicTeg(picData.strNetPicPath);
		msg.Insert(nPosAdd + picData.nPos, strPic.GetBuffer());
		strPic.ReleaseBuffer();
		nPosAdd += strPic.GetLength();
	}
	return msg;
}