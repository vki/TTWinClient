 /*******************************************************************************
 *  @file      SessionLayout.h 2014\8\5 20:25:03 $
 *  @author    大佛<dafo@mogujie.com>
 *  @brief     
 ******************************************************************************/

#ifndef SESSIONLAYOUT_6BC9730E_47F6_4BCB_936D_AC034AA10DFF_H__
#define SESSIONLAYOUT_6BC9730E_47F6_4BCB_936D_AC034AA10DFF_H__

#include "DuiLib/UIlib.h"
#include "GlobalDefine.h"
#include <memory>

/******************************************************************************/
using namespace DuiLib;

/**
 * The class <code>SessionLayout</code> 
 *
 */
struct ST_picData
{
	UINT32	nPos;
	CString strLocalPicPath;
	CString strNetPicPath;
};

class MixedMsg
{
public:
	MixedMsg();
	BOOL SetNetWorkPicPath(IN CString strLocalPicPath,IN CString strNetPicPath);
	BOOL SucceedToGetAllNetWorkPic();
	CString makeMixedLocalMSG();
	CString makeMixedNetWorkMSG();
private:
	CString _getPicMsgByAddPicTeg(IN CString picPath);

public:
	CString												m_strTEXT;//文字
	std::vector<ST_picData>	m_picDataVec;//图片所在文字中的位置，图片的本地路径；图片的网络路径
private:
	UINT32												m_nSetNetWorkPathSuccTime;//成功获得的图片的次数
};

class MessageEntity;
class EmotionDialog;
class UIIMEdit;
class SessionLayout :public CHorizontalLayoutUI, public INotifyUI, public CWebBrowserEventHandler
{
public:
    /** @name Constructors and Destructor*/

    //@{
    /**
     * Constructor 
     */
	SessionLayout(const std::string& sId, CPaintManagerUI& paint_manager);
    /**
     * Destructor
     */
    virtual ~SessionLayout();
    //@}
public:
	virtual void DoInit();
	virtual void DoEvent(TEventUI& event);
	virtual void Notify(TNotifyUI& msg);
	void SendMsg();
	void UpdateRunTimeMsg();
	void UpdateSendMsgKey();
	void UpdateBottomLayout();
	void freshGroupMemberAvatar(IN const std::string& sID);		//刷新群成员的在线状态
	void freshAllGroupMemberAvatar();
	void onCallbackOperation(std::shared_ptr<void> param);//发送图片返回

	void DocmentComplete(IDispatch *pDisp, VARIANT *&url);//打开会话已经，等浏览器初始化完成，加载未读消息
	virtual HRESULT STDMETHODCALLTYPE TranslateUrl(
		/* [in] */ DWORD dwTranslate,
		/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
		/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
	virtual void NewWindow2(VARIANT_BOOL *&Cancel, BSTR bstrUrl);
private:
	CString _getGetContent();	//获取内容
	BOOL	_displayMsgToIE(IN MessageEntity msg, IN CString jsInterface);
	void	_updateGroupMembersList();//展示群成员

	//插入png和gif
	//int	GetObjectPos(CRichEditUI* pRichEdit);
	//bool	InsertAniSmiley(CRichEditUI* pRichEdit, long hwndHostWindow, BSTR bstrFileName, OLE_COLOR clr, INT cy);
	//HRESULT GetNewStorage(LPSTORAGE* ppStg);
	//void	_InsertPic(IN CString strFilePath);
	//void	GetObjectInfo(IRichEditOle *pIRichEditOle);
	//void	ReleaseAllGif();

	void	_AddGroupMemberToList(IN const std::string& sID);
private:
	void	OnWindowInitialized(TNotifyUI& msg);
private:
	BOOL	_displayUnreadMsg();
	void	_displayHistoryMsg(UInt32 nMsgCount);

	void	_sendReadAck(IN MessageEntity msg);	//发送已读确认
	
private:
	CPaintManagerUI&	m_paint_manager;

	CWebBrowserUI*		m_pWebBrowser;//聊天显示框
	UIIMEdit*			m_pInputRichEdit;

	CContainerUI*		m_pRightContainer;
	CListUI*			m_pSearchResultList;
	CListUI*			m_pGroupMemberList;

	CTextUI*			m_pSendDescription;	// ctrl+enter /enter

	CButtonUI*			m_pBtnSendMsg;
	CButtonUI*			m_pBtnClose;

	COptionUI*			m_pBtnEmotion;
	CButtonUI*			m_pBtnSendImage;
	COptionUI*			m_pBtnscreenshot;
	CButtonUI*			m_pBtnjisuanqi;
	CButtonUI*			m_pBtnshock;
	CButtonUI*			m_pBtnsendfile;
	CButtonUI*			m_pBtnadduser;//添加讨论组成员

	CHorizontalLayoutUI*	m_bottomLayout;

	std::string			m_sId;		//会话Id

	std::vector<MixedMsg>	m_SendingMixedMSGList;

	BOOL				m_bGroupSession;//作为输入状态判断用，群是不需要发状态的
};
/******************************************************************************/
#endif// SESSIONLAYOUT_6BC9730E_47F6_4BCB_936D_AC034AA10DFF_H__
