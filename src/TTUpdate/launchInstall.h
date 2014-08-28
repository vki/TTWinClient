/*******************************************************************************
 *  @file      launchInstall.h 2014\8\10 15:42:56 $
 *  @author    快刀<kuaidao@mogujie.com>
 *  @brief     
 ******************************************************************************/

#ifndef LAUNCHINSTALL_08AFC462_6E29_48E6_858D_CD58C27F17CD_H__
#define LAUNCHINSTALL_08AFC462_6E29_48E6_858D_CD58C27F17CD_H__
/******************************************************************************/

#define	PRODUCT_DIR_REGKEY		_T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\teamtalk.exe")

class CLaunchInstall
{
public:
	CLaunchInstall(CString &csPath);
	//启动安装TT
	bool Launch();

	bool launchInstExe(const CString& csStartExe);
	bool checkInstallSucc();
	bool KillChildsProcess();
	static CString getLaunchPath();

private:
	CString		m_csAppPath;
};
/******************************************************************************/
#endif// LAUNCHINSTALL_08AFC462_6E29_48E6_858D_CD58C27F17CD_H__