 /*******************************************************************************
 *  @file      UIIMEdit.h 2014\8\19 13:18:10 $
 *  @author    ´ó·ð<dafo@mogujie.com>
 *  @brief     
 ******************************************************************************/

#ifndef UIIMEDIT_3DC89058_72C4_4225_A6EB_0F39D182913F_H__
#define UIIMEDIT_3DC89058_72C4_4225_A6EB_0F39D182913F_H__


#include "DuiLib/UIlib.h"
using namespace DuiLib;
#include "GlobalDefine.h"


#define UIIMEdit_MSGTYPE_TEXTCHANGED (_T("UIIMEdit_TEXT_Changed"))


/******************************************************************************/

/**
 * The class <code>UIIMEdit</code> 
 *
 */

class UIIMEdit :public CRichEditUI
{
public:
    /** @name Constructors and Destructor*/

    //@{
    /**
     * Constructor 
     */
    UIIMEdit();
    /**
     * Destructor
     */
    ~UIIMEdit();
    //@}

	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual LPCTSTR GetClass() const;
	virtual bool OnTxViewChanged();

public:
	void InsertBitmap(CString pBmpFile);
	void InsertBitmap(IN HBITMAP hBitmap);
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	BOOL WantCtrlEnterBySysSetting();

public:
	int	GetObjectPos();
	bool InsertAniSmiley(BSTR bstrFileName);
	HRESULT GetNewStorage(LPSTORAGE* ppStg);
	void	GetObjectInfo(IRichEditOle *pIRichEditOle);
	void ReleaseAllGif();
	bool GetPicPosAndPathbyOrder(IN UInt32 nOrder,OUT UInt32& nPos,OUT CString& path );
private:
	void _ImEditPaste();
	BOOL _SaveFile(IN HBITMAP hbitmap, OUT CString& strFilePath);
	HBITMAP _LoadAnImage(IN CString filePath);
	
private:
	CString                     m_strImagePath;
};
/******************************************************************************/
#endif// UIIMEDIT_3DC89058_72C4_4225_A6EB_0F39D182913F_H__
