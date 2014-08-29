/******************************************************************************* 
 *  @file      launchInstall.cpp 2014\8\10 15:43:42 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "LaunchInstall.h"
#include <Tlhelp32.h>
/******************************************************************************/

CLaunchInstall::CLaunchInstall(CString &csPath):
m_csAppPath(csPath)
{
	
}

bool CLaunchInstall::KillChildsProcess()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hSnapshot);
		return false;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hSnapshot,&pe32))
	{
		CloseHandle(hSnapshot);
		return false;
	}

	do{
		CString csProcName(pe32.szExeFile);
		//if(csProcName.CompareNoCase(INSTALL_EXE) == 0) todo...
		{
			DWORD dwProcessID = pe32.th32ProcessID;
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);
			::TerminateProcess(hProcess,0);
			CloseHandle(hProcess);
			continue;
		}

	}while(Process32Next(hSnapshot,&pe32));
	CloseHandle(hSnapshot);

	return true;
}

bool CLaunchInstall::launchInstExe(const CString& csStartExe)
{
	if(csStartExe.IsEmpty() || !::PathFileExists(csStartExe))
		return false;

	SHELLEXECUTEINFO shellExeInfo;
	ZeroMemory(&shellExeInfo,sizeof(shellExeInfo));
	shellExeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shellExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shellExeInfo.lpFile = csStartExe;
	shellExeInfo.lpVerb = _T("open");
	shellExeInfo.nShow = SW_SHOWMAXIMIZED;
	shellExeInfo.hwnd = NULL;
	shellExeInfo.lpParameters = _T("/S");
	shellExeInfo.lpDirectory = NULL;
	if(!::ShellExecuteEx(&shellExeInfo))
		return 0;
	if((DWORD) shellExeInfo.hInstApp <= 32L)
		return false;

	DWORD dwWaitResult;
	if(shellExeInfo.hProcess)
	{
		dwWaitResult = WaitForSingleObject(shellExeInfo.hProcess,120000);
	}

	if(dwWaitResult != WAIT_OBJECT_0)
	{
		if(shellExeInfo.hProcess)
		{
			KillChildsProcess();
		}
		return false;
	}
	return true;
}

bool CLaunchInstall::Launch()
{
	if(!launchInstExe(m_csAppPath))
		return false;
	
	if(checkInstallSucc())
		return true;

	return false;
}

bool CLaunchInstall::checkInstallSucc()
{
	HKEY hKEY;
	bool bInsted = false;
	CString valueName = _T("SilentMark");
	TCHAR value[MAX_PATH];
	DWORD size;

	if(::RegOpenKey(HKEY_LOCAL_MACHINE,PRODUCT_DIR_REGKEY,&hKEY)== ERROR_SUCCESS)
	{
		if(::RegQueryValueEx(hKEY,(LPCTSTR)valueName,NULL,NULL,(LPBYTE)value,&size)== ERROR_SUCCESS)
		{
			CString csValue(value);
			if(csValue.CompareNoCase(_T("succ")) == 0)
			{
				bInsted = true;
			}
		}
	}
	RegCloseKey(hKEY);

	//奇怪问题，在win7下，必须要执行两边，才行
	valueName = _T("SilentMark");
	if(::RegOpenKey(HKEY_LOCAL_MACHINE,PRODUCT_DIR_REGKEY,&hKEY)== ERROR_SUCCESS)
	{
		if(::RegQueryValueEx(hKEY,(LPCTSTR)valueName,NULL,NULL,(LPBYTE)value,&size)== ERROR_SUCCESS)
		{
			CString csValue(value);
			if(csValue.CompareNoCase(_T("succ")) == 0)
			{
				bInsted = true;
			}
		}
	}
	RegCloseKey(hKEY);

	if(::RegOpenKey(HKEY_LOCAL_MACHINE,PRODUCT_DIR_REGKEY,&hKEY) == ERROR_SUCCESS)
	{
		::RegDeleteValue(hKEY,(LPCWSTR)valueName);
		::RegCloseKey(hKEY);
	}

	return bInsted;
}

CString CLaunchInstall::getLaunchPath()
{
	CString path;
	HKEY hKey;
	if(::RegOpenKey(HKEY_LOCAL_MACHINE,PRODUCT_DIR_REGKEY,&hKey) != ERROR_SUCCESS)
		return path;
	DWORD datasize  = MAX_PATH*10;
	BYTE  szData[MAX_PATH*10];
	memset(szData,0,datasize);
	DWORD dType = REG_SZ;
	if(::RegQueryValueEx(hKey,_T("Path"),NULL,&dType,szData,&datasize) != ERROR_SUCCESS)
	{
		::RegCloseKey(hKey);
		return FALSE;
	}
	szData[datasize] = 0;
	path.Format(_T("%s"), szData);
	::RegCloseKey(hKey);

	return path;
}
/******************************************************************************/
