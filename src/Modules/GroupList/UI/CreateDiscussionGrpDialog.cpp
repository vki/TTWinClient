/******************************************************************************* 
 *  @file      MakeGroupWnd.cpp 2014\7\24 17:42:25 $
 *  @author    大佛<dafo@mogujie.com>
 *  @brief   
 ******************************************************************************/

#include "stdafx.h"
#include "CreateDiscussionGrpDialog.h"
#include "TTLogic/ILogic.h"
#include "TTLogic/ITcpClientModule.h"
#include "Modules/IUserListModule.h"
#include "Modules/UI/SearchLayout.h"
#include "Modules/ISysConfigModule.h"
#include "ListGroupMembers.h"
#include "ListCreatFrom.h"
#include "src/base/ImPduGroup.h"


DUI_BEGIN_MESSAGE_MAP(CreateDiscussionGrpDialog, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_WINDOWINIT, OnPrepare)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMACTIVATE, OnItemActive)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	//DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMDBCLICK,OnItemDBClick)
DUI_END_MESSAGE_MAP()

/******************************************************************************/

// -----------------------------------------------------------------------------
//  MakeGroupWnd: Public, Constructor

CreateDiscussionGrpDialog::CreateDiscussionGrpDialog()
:m_pListCreatFrom(nullptr)
, m_pListGroupMembers(nullptr)
{

}

// -----------------------------------------------------------------------------
//  MakeGroupWnd: Public, Destructor

CreateDiscussionGrpDialog::~CreateDiscussionGrpDialog()
{

}

LPCTSTR CreateDiscussionGrpDialog::GetWindowClassName() const
{
	return _T("CreateDiscussionGrpDialog");
}

DuiLib::CDuiString CreateDiscussionGrpDialog::GetSkinFile()
{
	return  _T("CreateDiscussionGrpDialog\\CreateDiscussionGrpDialog.xml");
}

DuiLib::CDuiString CreateDiscussionGrpDialog::GetSkinFolder()
{
	return _T("");
}

CControlUI* CreateDiscussionGrpDialog::CreateControl(LPCTSTR pstrClass)
{
	if (0 == _tcsicmp(pstrClass, _T("SearchLayout")))
	{
		return new SearchLayout();
	}
	else if (_tcsicmp(pstrClass,_T("GroupMembersList")) == 0)
	{
		return new ListGroupMembers(m_PaintManager);
	}
	else if (_tcsicmp(pstrClass, _T("CreatFromList")) == 0)
	{
		return new ListCreatFrom(m_PaintManager);
	}
	return NULL;

}
void CreateDiscussionGrpDialog::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}


LRESULT CreateDiscussionGrpDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_NCLBUTTONDBLCLK != uMsg)//禁用双击标题栏最大化
	{
		return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
	}
	return 0;
}

void CreateDiscussionGrpDialog::OnPrepare(TNotifyUI& msg)
{
	m_pListCreatFrom = static_cast<ListCreatFrom*>(m_PaintManager.FindControl(_T("CreatFromList")));
	m_pListGroupMembers = static_cast<ListGroupMembers*>(m_PaintManager.FindControl(_T("GroupMembersList")));
	m_editGroupName = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editGroupName")));
	m_TextaddNums = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("TextaddNums")));
	const module::DepartmentVec vecDeparments
		= module::getUserListModule()->getAllDepartments();
	for (module::DepartmentEntity depart : vecDeparments)
	{
		IMListItemInfo item;
		item.id = util::stringToCString(depart.dId);
		item.folder = true;
		item.empty = false;
		item.nickName = depart.title;
		Node* root_parent = m_pListCreatFrom->AddNode(item, NULL);

		for (std::string uId : depart.members)
		{
			module::UserInfoEntity user;
			if (module::getUserListModule()->getUserInfoBySId(uId, user))
			{
				item.id = util::stringToCString(uId);
				item.folder = false;
				item.avatarPath = util::stringToCString(user.getAvatarPath());
				item.nickName = user.getRealName();
				item.description = _T("这里显示描述信息");
				m_pListCreatFrom->AddNode(item, root_parent);
			}
		}
	}

	std::string mySid = module::getSysConfigModule()->userID();
	module::UserInfoEntity myInfo;
	if (module::getUserListModule()->getUserInfoBySId(mySid, myInfo))
	{
		IMListItemInfo item;
		item.id = util::stringToCString(mySid);
		item.avatarPath = util::stringToCString(myInfo.getAvatarPath());
		item.nickName = myInfo.getRealName();
		item.folder = false;
		item.empty = false;
		item.description = _T("我自己");
		m_pListGroupMembers->AddNode(item);
	}


	//CGroupsTreelistUI* pGroupsList = static_cast<CGroupsTreelistUI*>(m_PaintManager.FindControl(_T("AllUsersList")));
	//if (pGroupsList != NULL)
	//{
	//	if (pGroupsList->GetCount() > 0)
	//		pGroupsList->RemoveAll();

	//	GroupsListItemInfo item;

	//	item.folder = true;
	//	item.empty = false;
	//	item.nickName = _T("SWAT");

	//	Node* root_parent = pGroupsList->AddNode(item, NULL);

	//	item.folder = false;
	//	item.avatarPath = _T("duilib.png");
	//	item.nickName = _T("大佛");
	//	item.description = _T("153787916");
	//	pGroupsList->AddNode(item, root_parent);

	//	item.folder = false;
	//	item.avatarPath = _T("groups.png");
	//	item.nickName = _T("快刀");
	//	item.description = _T("79145400");
	//	pGroupsList->AddNode(item, root_parent);


	//	item.id = _T("3");
	//	item.folder = true;
	//	item.empty = false;
	//	item.nickName = _T("客服");
	//	Node* root_parent2 = pGroupsList->AddNode(item, NULL);

	//	item.folder = false;
	//	item.avatarPath = _T("groups.png");
	//	item.nickName = _T("提莫");
	//	item.description = _T("79145400");
	//	pGroupsList->AddNode(item, root_parent2);

	//	for (int n = 0; n < 40; ++n)
	//	{
	//		item.folder = false;
	//		item.avatarPath = _T("groups.png");
	//		item.nickName = _T("荡荡");
	//		item.description = _T("79145400");
	//		pGroupsList->AddNode(item, root_parent2);
	//	}

	//}

	//CUISessionList* pRecentlyList = static_cast<CUISessionList*>(m_PaintManager.FindControl(_T("AddedUsersList")));
	//if (pRecentlyList != NULL)
	//{
	//	if (pRecentlyList->GetCount() > 0)
	//		pRecentlyList->RemoveAll();

	//	SessionListItemInfo item;

	//	srand((UINT)time(0));
	//	item.Time = rand() % 200;
	//	item.folder = false;
	//	item.empty = false;
	//	item.logo = _T("duilib.png");
	//	item.nick_name = _T("子烨");
	//	item.description.Format(_T("%d"), item.Time);

	//	pRecentlyList->AddNode(item, NULL);


	//	item.Time = rand() % 200;
	//	item.logo = _T("duilib.png");
	//	item.nick_name = _T("麦克");
	//	//item.description = _T("大帅哥");
	//	item.description.Format(_T("%d"), item.Time);
	//	pRecentlyList->AddNode(item, NULL);


	//	item.Time = rand() % 200;
	//	item.logo = _T("duilib.png");
	//	item.nick_name = _T("四叶");
	//	//item.description = _T("闷骚的少年");
	//	item.description.Format(_T("%d"), item.Time);
	//	pRecentlyList->AddNode(item, NULL, 2);

	//	item.Time = rand() % 200;
	//	item.logo = _T("duilib.png");
	//	item.nick_name = _T("独嘉");
	//	//item.description = _T("风一般的少年");
	//	item.description.Format(_T("%d"), item.Time);
	//	pRecentlyList->AddNode(item, NULL);

	//	item.Time = rand() % 200;
	//	item.logo = _T("duilib.png");
	//	item.nick_name = _T("四叶");
	//	//item.description = _T("闷骚的少年");
	//	item.description.Format(_T("%d"), item.Time);
	//	pRecentlyList->AddNode(item, NULL);

	//	item.Time = rand() % 200;
	//	item.logo = _T("duilib.png");
	//	item.nick_name = _T("千凡");
	//	//item.description = _T("养猫的孩纸");
	//	item.description.Format(_T("%d"), item.Time);
	//	pRecentlyList->AddNode(item, NULL, 3);

	//	pRecentlyList->sort();
	//}
}

void CreateDiscussionGrpDialog::OnItemActive(TNotifyUI& msg)
{
	if (0 == _tcsicmp(msg.pSender->GetClass(),_T("ListContainerElementUI")))
	{
		if (0 == _tcsicmp(msg.pSender->GetName(), _T("ListCreatFromItem")))
		{
			Node* node = (Node*)msg.pSender->GetTag();
			if (!m_pListCreatFrom->CanExpand(node))
			{
				CString csId = node->data().sId;
				if (csId.IsEmpty())
					return;
				std::string sId = util::cStringToString(csId);
				
				IMListItemInfo addItem;
				module::UserInfoEntity user;
				if (module::getUserListModule()->getUserInfoBySId(sId, user))
				{
					addItem.id = util::stringToCString(sId);
					addItem.folder = false;
					addItem.empty = false;
					addItem.avatarPath = util::stringToCString(user.getAvatarPath());
					addItem.nickName = user.getRealName();
					addItem.description = _T("描述");
					if (!m_pListGroupMembers->IsExistSId(sId))
					{
						m_pListGroupMembers->AddNode(addItem);
					}
				}
			}

			if (m_TextaddNums)
			{
				CString strFormat;
				strFormat.Format(_T("(%d/50)"), m_pListGroupMembers->GetCount());
				m_TextaddNums->SetText(strFormat);
			}
		}
		else if (msg.pSender->GetName() == _T("ListCreatFromItem"))
		{
		}
	}
}

void CreateDiscussionGrpDialog::OnItemClick(TNotifyUI& msg)
{
	CDuiString cname = msg.pSender->GetClass();
	CDuiString name = msg.pSender->GetName();
	if (0 == _tcsicmp(msg.pSender->GetClass(),_T("ListContainerElementUI")))
	{
		if (0 == _tcsicmp(msg.pSender->GetName(),_T("ListCreatFromItem")))
		{
			Node* node = (Node*)msg.pSender->GetTag();

			if (m_pListCreatFrom->CanExpand(node))
			{
				m_pListCreatFrom->SetChildVisible(node, !node->data().child_visible_);
			}
		}
	}
}

void CreateDiscussionGrpDialog::OnClick(TNotifyUI& msg)
{
	__super::OnClick(msg);
	if (0 == _tcsicmp(msg.pSender->GetName(), _T("okbtn"))
		&& m_pListCreatFrom && m_pListGroupMembers)
	{
		CDuiString groupName = m_editGroupName->GetText();
		if (m_pListGroupMembers->GetCount()!= 0
			&& !groupName.IsEmpty())
		{
			std::vector<std::string> vecMembers = m_pListGroupMembers->GetAllItemsSId();
			std::string strGroupName = util::cStringToString(CString(groupName));

			logic::GetLogic()->pushBackOperationWithLambda(		//创建群
				[=]()
			{
				list<user_id_t> listSid;
				user_id_t item;
				for (UInt32 i = 0; i < vecMembers.size(); ++i)
				{
					item.id_len = vecMembers[i].length();
					item.id_url = (char*)vecMembers[i].c_str();
					listSid.push_back(item);
				}

				CImPduClientGroupCreateTmpGroupRequest pduMsgData(strGroupName.c_str(), 0, &listSid);
				logic::getTcpClientModule()->sendPacket(&pduMsgData);
			}
			);

			Close(IDCANCEL);
		}
		
	}
}

//void CreateDiscussionGrpDialog::OnItemDBClick(TNotifyUI& msg)
//{
//	CDuiString cname = msg.pSender->GetClass();
//	CDuiString name = msg.pSender->GetName();
//}

/******************************************************************************/