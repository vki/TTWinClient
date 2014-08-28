/*******************************************************************************
 *  @file      MakeGroupWnd.h 2014\7\24 17:42:15 $
 *  @author    大佛<dafo@mogujie.com>
 *  @brief   
 ******************************************************************************/

#ifndef MAKEGROUPWND_537FB18B_C75C_445C_805F_C2624BCEB298_H__
#define MAKEGROUPWND_537FB18B_C75C_445C_805F_C2624BCEB298_H__

#include "DuiLib/UIlib.h"
/******************************************************************************/
using namespace DuiLib;

/**
 * The class <code>CreateDiscussionGrpDialog</code> 
 *
 */
class ListCreatFrom;
class ListGroupMembers;
class CreateDiscussionGrpDialog : public WindowImplBase
{
public:
    /** @name Constructors and Destructor*/

    //@{
    /**
     * Constructor 
     */
    CreateDiscussionGrpDialog();
    /**
     * Destructor
     */
    virtual ~CreateDiscussionGrpDialog();
    //@}
	DUI_DECLARE_MESSAGE_MAP()
public:
	LPCTSTR GetWindowClassName() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	void OnPrepare(TNotifyUI& msg);
	void OnItemActive(TNotifyUI& msg);
	void OnItemClick(TNotifyUI& msg);
	void OnClick(TNotifyUI& msg);
	//void OnItemDBClick(TNotifyUI& msg);
private:
	ListCreatFrom*			m_pListCreatFrom;		//讨论组数据源列表	左
	ListGroupMembers*		m_pListGroupMembers;	//讨论组结果列表	右
	CEditUI*				m_editGroupName;
	CTextUI*				m_TextaddNums;			//已经加了多少人
};
/******************************************************************************/
#endif// MAKEGROUPWND_537FB18B_C75C_445C_805F_C2624BCEB298_H__
