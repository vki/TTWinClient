/*******************************************************************************
 *  @file      IEmotionModule.h 2014\8\6 20:03:22 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     表情管理模块
 ******************************************************************************/

#ifndef IEMOTIONMODULE_8E431B43_8F05_4934_8B5B_BB839730F3A6_H__
#define IEMOTIONMODULE_8E431B43_8F05_4934_8B5B_BB839730F3A6_H__

#include "GlobalDefine.h"
#include "TTLogic/IModule.h"
#include "Modules/ModuleDll.h"
/******************************************************************************/
NAMESPACE_BEGIN(module)

/**
 * The class <code>表情管理模块</code> 
 *
 */
class MODULE_API IEmotionModule : public logic::IModule
{
public:
    /** @name Constructors and Destructor*/

    //@{
    /**
     * Constructor 
     */
	IEmotionModule()
	{
		m_moduleId = MODULE_ID_EMOTION;
	}
    //@}

public:
	/**
	 * 显示表情窗口
	 *
	 * @param   POINT pt 显示的位置
	 * @return  void
	 * @exception there is no any exception to throw.
	 */
	virtual void showEmotionDialog(POINT pt) = 0;
	virtual BOOL getEmotionPathByText(IN CString csEmotion, OUT CString& csPath) = 0;
};

MODULE_API IEmotionModule* getEmotionModule();

NAMESPACE_END(module)
/******************************************************************************/
#endif// IEMOTIONMODULE_8E431B43_8F05_4934_8B5B_BB839730F3A6_H__
