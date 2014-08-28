/*******************************************************************************
 *  @file      IFileTransfer.h 2014\8\26 11:51:55 $
 *  @author    ¿ìµ¶<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#ifndef IFILETRANSFER_425BDB8D_221E_4952_93C5_4362AF5217CB_H__
#define IFILETRANSFER_425BDB8D_221E_4952_93C5_4362AF5217CB_H__

#include "GlobalDefine.h"
#include "TTLogic/IModule.h"
#include "Modules/ModuleDll.h"
/******************************************************************************/
NAMESPACE_BEGIN(module)

/**
 * The class <code>IFileTransfer</code> 
 *
 */
class IFileTransferModule : public logic::IPduAsyncSocketModule
{
public:
    /** @name Constructors and Destructor*/

    //@{
    /**
     * Constructor 
     */
	IFileTransferModule()
	{
		m_moduleId = MODULE_ID_FILETRANSFER;
	}
    //@}
};

NAMESPACE_END(module)
/******************************************************************************/
#endif// IFILETRANSFER_425BDB8D_221E_4952_93C5_4362AF5217CB_H__
