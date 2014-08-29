/*******************************************************************************
 *  @file      FileTransfer_Impl.h 2014\8\26 11:53:06 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#ifndef FILETRANSFER_IMPL_AB8D6DD0_25EF_4809_A857_9450EEA7CBFE_H__
#define FILETRANSFER_IMPL_AB8D6DD0_25EF_4809_A857_9450EEA7CBFE_H__

#include "Modules/IFileTransferModule.h"
/******************************************************************************/

/**
 * The class <code>FileTransfer_Impl</code> 
 *
 */
class FileTransferModule_Impl : public module::IFileTransferModule
{
public:
    /** @name Constructors and Destructor*/

    //@{
    /**
     * Constructor 
     */
    FileTransferModule_Impl();
    /**
     * Destructor
     */
    virtual ~FileTransferModule_Impl();
    //@}
	virtual void onPacket(std::auto_ptr<CImPdu> pdu);
	virtual void release();

private:
	/**@name 服务器端拆包*/
	//@{
	//@}

};
/******************************************************************************/
#endif// FILETRANSFER_IMPL_AB8D6DD0_25EF_4809_A857_9450EEA7CBFE_H__
