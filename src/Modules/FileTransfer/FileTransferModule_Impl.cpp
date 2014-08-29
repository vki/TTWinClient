/******************************************************************************* 
 *  @file      FileTransfer_Impl.cpp 2014\8\26 11:53:09 $
 *  @author    ¿ìµ¶<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "FileTransferModule_Impl.h"

/******************************************************************************/

// -----------------------------------------------------------------------------
//  FileTransfer_Impl: Public, Constructor

FileTransferModule_Impl::FileTransferModule_Impl()
{

}

// -----------------------------------------------------------------------------
//  FileTransfer_Impl: Public, Destructor

FileTransferModule_Impl::~FileTransferModule_Impl()
{

}

void FileTransferModule_Impl::onPacket(std::auto_ptr<CImPdu> pdu)
{
}

void FileTransferModule_Impl::release()
{
	delete this;
}

/******************************************************************************/