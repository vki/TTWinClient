/*******************************************************************************
 *  @file      utilAPI.h 2014\7\15 20:26:18 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief   
 ******************************************************************************/

#ifndef UTILAPI_AF8EB6AC_116D_4D74_8AFA_0C5E7BF6FFEB_H__
#define UTILAPI_AF8EB6AC_116D_4D74_8AFA_0C5E7BF6FFEB_H__

#include "utility/utilityDll.h"
#include "utility/log.h"
#include "GlobalDefine.h"
/******************************************************************************/
NAMESPACE_BEGIN(util)

//APP相关
UTILITY_API CString		getAppPath();
UTILITY_API CString		getParentAppPath();

//文件相关
UTILITY_API BOOL		isFileExist(LPCTSTR lpcsFileName);
UTILITY_API BOOL		createAllDirectories(CString & csDir);
UTILITY_API BOOL		emptyFolder(const CString& csFolder, BOOL bDeleteSelf = FALSE);
UTILITY_API CString		getFileExtName(CString& strFile);

//hash函数相关
UTILITY_API UInt32		hash_BKDR(const char* str);

//log
#define APP_LOG	util::Log::getInstance()->logging

UTILITY_API BOOL saveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName);

NAMESPACE_END(util)
/******************************************************************************/
#endif// UTILAPI_af8eb6ac-116d-4d74-8afa-0c5e7bf6ffeb_H__
