/*******************************************************************************
 *  @file      EmotionModule_Impl.h 2014\8\6 20:05:39 $
 *  @author    ¿ìµ¶<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#ifndef EMOTIONMODULE_IMPL_9432EBF5_DD5E_4C01_BF4B_A8D3E1E11CB9_H__
#define EMOTIONMODULE_IMPL_9432EBF5_DD5E_4C01_BF4B_A8D3E1E11CB9_H__

#include "Modules/IEmotionModule.h"
/******************************************************************************/
class EmotionDialog;

/**
 * The class <code>EmotionModule_Impl</code> 
 *
 */
class EmotionModule_Impl final : public module::IEmotionModule
{
public:
    /** @name Constructors and Destructor*/

    //@{
    /**
     * Constructor 
     */
    EmotionModule_Impl();
    /**
     * Destructor
     */
    virtual ~EmotionModule_Impl();
    //@}
	virtual void release();

public:
	virtual void showEmotionDialog(POINT pt);
	virtual BOOL getEmotionPathByText(IN CString csEmotion, OUT CString& csPath);
private:
	EmotionDialog*			m_emotionWindow;
};
/******************************************************************************/
#endif// EMOTIONMODULE_IMPL_9432EBF5_DD5E_4C01_BF4B_A8D3E1E11CB9_H__
