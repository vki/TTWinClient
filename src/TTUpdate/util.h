#pragma once

#ifndef __RVUPDATE_UTIL_H__
#define __RVUPDATE_UTIL_H__

extern  CString	GetFileDirFromPath(const CString &strFilePath);

extern	CString	GetAppPath();

extern	CString	GetParentAppPath();

extern  BOOL IsFileExist(const CString& csFileName);

#endif