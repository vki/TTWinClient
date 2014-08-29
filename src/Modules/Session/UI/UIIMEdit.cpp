/******************************************************************************* 
 *  @file      UIIMEdit.cpp 2014\8\19 13:20:13 $
 *  @author    大佛<dafo@mogujie.com>
 *  @brief     
 ******************************************************************************/

#include "stdafx.h"
#include "UIIMEdit.h"
#include "Modules/IMiscModule.h"
#include "Modules/ICaptureModule.h"
#include "Modules/IP2PCmdModule.h"
#include "Modules/ISysConfigModule.h"
#include "GifSmiley/GifSmiley.h"
#include "cxImage/cxImage/ximage.h"
/******************************************************************************/

// -----------------------------------------------------------------------------
//  UIIMEdit: Public, Constructor

UIIMEdit::UIIMEdit()
{
}

// -----------------------------------------------------------------------------
//  UIIMEdit: Public, Destructor

UIIMEdit::~UIIMEdit()
{
	
}

LPVOID UIIMEdit::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, _T("UIIMEdit")) == 0) return static_cast<UIIMEdit*>(this);
	return __super::GetInterface(pstrName);
}

LPCTSTR UIIMEdit::GetClass() const
{
	return _T("UIIMEdit");
}



void UIIMEdit::_ImEditPaste()
{
	BOOL bHadPic = FALSE;
	if (::OpenClipboard(::GetDesktopWindow()))
	{
		if (IsClipboardFormatAvailable(CF_BITMAP))
		{
			HBITMAP handle = (HBITMAP)::GetClipboardData(CF_BITMAP);
			if (handle)
			{
				_SaveFile(handle, m_strImagePath);
				InsertAniSmiley(m_strImagePath.GetBuffer());
				//InsertBitmap(handle);发送内容的时候，找不到图片位置
				//InsertBitmap(m_strImagePath);
				bHadPic = TRUE;
			}
		}
		CloseClipboard();
	}
	if (!bHadPic)
	{
		Paste();
	}
}

BOOL UIIMEdit::_SaveFile(IN HBITMAP hbitmap, OUT CString& strFilePath)
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	CString strTime;
	strTime.Format(_T("%d-%02d-%02d-%02d%02d%02d-%03d"), st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);
	util::createAllDirectories(module::getMiscModule()->getUserTempDir());
	CString csDstFileName = module::getMiscModule()->getUserTempDir() + strTime;
	//CString csExtName = _T(".bmp");
	CString csExtName = _T(".jpg");

	if (module::getCaptureModule()->saveToFile(hbitmap, csDstFileName + csExtName))
	{
		strFilePath = csDstFileName + csExtName;
		return TRUE;
	}
	else
	{
		//保存图片失败
	}
	return FALSE;
}
void UIIMEdit::InsertBitmap(CString pBmpFile)
{
	//CxImage cximage;
	//m_strImagePath = pBmpFile;
	//if (-1 != pBmpFile.Find(_T(".bmp")))
	//{
	//	cximage.Load(pBmpFile, 1);
	//}
	////else if (-1 != pBmpFile.Find(_T(".jpg")))
	////{
	////	cximage.Load(pBmpFile, CXIMAGE_FORMAT_JPG);
	////}
	////else if (-1 != pBmpFile.Find(_T(".png")))
	////{
	////	cximage.Load(pBmpFile, CXIMAGE_FORMAT_PNG);
	////}
	//if (!cximage.IsValid())
	//{
	//	return;
	//}
	//HBITMAP hBitMap = cximage.MakeBitmap(m_pManager->GetPaintDC());
	
	HBITMAP hBitMap = 0;
	m_strImagePath = pBmpFile;
	if (-1 != pBmpFile.Find(_T(".bmp")))
	{
		hBitMap = (HBITMAP)::LoadImage(NULL,
			pBmpFile,
			IMAGE_BITMAP,
			0,
			0,
			LR_LOADFROMFILE);
	}
	else
	{
		hBitMap = _LoadAnImage(pBmpFile);
	}

	InsertBitmap(hBitMap);
}
void UIIMEdit::InsertBitmap(IN HBITMAP hBitmap)
{
	try
	{
		////创建输入数据源
		IStorage *pStorage;
		///分配内存
		LPLOCKBYTES lpLockBytes = NULL;
		SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
		if (sc != S_OK)
			AfxThrowOleException(sc);
		ASSERT(lpLockBytes != NULL);

		sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
			STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &pStorage);
		if (sc != S_OK)
		{
			VERIFY(lpLockBytes->Release() == 0);
			lpLockBytes = NULL;
			AfxThrowOleException(sc);
		}
		ASSERT(pStorage != NULL);

		STGMEDIUM stgm;
		stgm.tymed = TYMED_GDI;	   // Storage medium = HBITMAP handle
		stgm.hBitmap = hBitmap;
		stgm.pUnkForRelease = NULL; // Use ReleaseStgMedium
		COleDataSource *pDataSource = new COleDataSource;
		pDataSource->CacheData(CF_BITMAP, &stgm);
		LPDATAOBJECT lpDataObject =
			(LPDATAOBJECT)pDataSource->GetInterface(&IID_IDataObject);

		///获取RichEdit的OLEClientSite
		LPOLECLIENTSITE lpClientSite;
		GetRichEditOle()->GetClientSite(&lpClientSite);


		//创建OLE对象
		FORMATETC fm;
		fm.cfFormat = CF_BITMAP;    // Clipboard format = CF_BITMAP
		fm.ptd = NULL;              // Target Device = Screen
		fm.dwAspect = DVASPECT_CONTENT; // Level of detail = Full content
		fm.lindex = -1;         // Index = Not applicaple
		fm.tymed = TYMED_GDI;
		CComPtr<IOleObject> pOleObject;
		sc = OleCreateStaticFromData(lpDataObject, IID_IOleObject, OLERENDER_FORMAT,
			&fm, lpClientSite, pStorage, (void **)&pOleObject);
		if (sc != S_OK)
			AfxThrowOleException(sc);

		///插入OLE对象

		REOBJECT reobject;
		ZeroMemory(&reobject, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);

		CLSID clsid;
		sc = pOleObject->GetUserClassID(&clsid);
		if (sc != S_OK)
			AfxThrowOleException(sc);

		reobject.clsid = clsid;
		reobject.cp = REO_CP_SELECTION;
		reobject.dvaspect = DVASPECT_CONTENT;
		reobject.poleobj = pOleObject;
		reobject.polesite = lpClientSite;
		reobject.pstg = pStorage;

		HRESULT hr = m_pRichEditOle->InsertObject(&reobject);
		delete pDataSource;
		pDataSource = 0;
		lpClientSite->Release();
		pStorage->Release();
		lpLockBytes->Release();
		m_pRichEditOle->Release();
	}
	catch (...)
	{
		int i = 0;
		i++;
	}
}

HBITMAP UIIMEdit::_LoadAnImage(IN CString filePath)
{
	HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //从指定的路径szImagePath中读取文件句柄
	DWORD dwFileSize = GetFileSize(hFile, NULL); //获得图片文件的大小，用来分配全局内存
	HGLOBAL hImageMemory = GlobalAlloc(GMEM_MOVEABLE, dwFileSize); //给图片分配全局内存
	void *pImageMemory = GlobalLock(hImageMemory); //锁定内存
	DWORD dwReadedSize; //保存实际读取的文件大小
	ReadFile(hFile, pImageMemory, dwFileSize, &dwReadedSize, NULL); //读取图片到全局内存当中
	GlobalUnlock(hImageMemory); //解锁内存
	CloseHandle(hFile); //关闭文件句柄

	HRESULT hr = NULL;
	IStream *pIStream = NULL;//创建一个IStream接口指针，用来保存图片流
	IPicture *pIPicture = NULL;//创建一个IPicture接口指针，表示图片对象

	hr = CreateStreamOnHGlobal(hImageMemory, false, &pIStream); //用全局内存初使化IStream接口指针
	ASSERT(SUCCEEDED(hr));

	hr = OleLoadPicture(pIStream, 0, false, IID_IPicture, (LPVOID*)&(pIPicture));//用OleLoadPicture获得IPicture接口指针
	ASSERT(SUCCEEDED(hr));

	HBITMAP hB = NULL;
	pIPicture->get_Handle((unsigned int*)&hB);
	// Copy the image. Necessary, because upon p's release,
	// the handle is destroyed.
	HBITMAP hBB = (HBITMAP)CopyImage(hB, IMAGE_BITMAP, 0, 0,
		LR_COPYRETURNORG);

	GlobalFree(hImageMemory); //释放全局内存
	pIStream->Release(); //释放pIStream
	pIPicture->Release(); //释放pIPictur
	return hBB;
}

int UIIMEdit::GetObjectPos()
{
	bool findObject = false;

	IRichEditOle *pRichEditOle = GetRichEditOle();
	if (NULL == pRichEditOle)
	{
		return 0;
	}

	int nCount = pRichEditOle->GetObjectCount();
	for (int i = nCount - 1; i >= 0; i--)
	{
		REOBJECT reobj = { 0 };
		reobj.cbStruct = sizeof(REOBJECT);
		pRichEditOle->GetObject(i, &reobj, REO_GETOBJ_POLEOBJ);
		reobj.poleobj->Release();
	}
	return 0;
}

bool UIIMEdit::InsertAniSmiley(BSTR bstrFileName)
{
	GifSmiley::IGifSmileyCtrl* lpAnimator = nullptr;
	::CoCreateInstance(GifSmiley::CLSID_CGifSmileyCtrl, NULL, CLSCTX_INPROC, GifSmiley::IID_IGifSmileyCtrl, (LPVOID*)&lpAnimator);

	if (lpAnimator == NULL)
	{
		return false;
	}

	COLORREF backColor = (COLORREF)(::GetSysColor(COLOR_WINDOW));
	HWND hwnd = (HWND)((long)m_pManager->GetPaintWindow());
	HRESULT hr;


	IRichEditOle *pRichEditOle = GetRichEditOle();
	if (NULL == pRichEditOle)
	{
		return false;
	}

	LPSTORAGE lpStorage = NULL;
	LPOLEOBJECT	lpObject = NULL;
	LPLOCKBYTES lpLockBytes = NULL;
	LPOLECLIENTSITE lpClientSite = NULL;
	BSTR path = NULL;

	//Create lockbytes
	hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if (FAILED(hr))
	{
		return	 false;
	}
	//use lockbytes to create storage
	SCODE sc = ::StgCreateDocfileOnILockBytes(lpLockBytes, STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &lpStorage);
	if (sc != S_OK)
	{
		lpLockBytes->Release();
		return false;
	}

	// retrieve OLE interface for richedit   and  Get site
	pRichEditOle->GetClientSite(&lpClientSite);

	try
	{
		//COM operation need BSTR, so get a BSTR
		path = bstrFileName;

		//Load the gif
		lpAnimator->LoadFromFileSized(path, 0);

		//Set back color
		OLE_COLOR oleBackColor = (OLE_COLOR)backColor;
		lpAnimator->put_BackColor(oleBackColor);


		//get the IOleObject
		hr = lpAnimator->QueryInterface(IID_IOleObject, (void**)&lpObject);
		if (FAILED(hr))
		{
			return	 false;
		}

		//Set it to be inserted
		OleSetContainedObject(lpObject, TRUE);

		//to insert into richedit, you need a struct of REOBJECT
		REOBJECT reobject;
		ZeroMemory(&reobject, sizeof(REOBJECT));

		reobject.cbStruct = sizeof(REOBJECT);

		CLSID clsid;
		hr = lpObject->GetUserClassID(&clsid);


		//set clsid
		reobject.clsid = clsid;
		//can be selected
		reobject.cp = REO_CP_SELECTION;
		//content, but not static
		reobject.dvaspect = DVASPECT_CONTENT;
		//goes in the same line of text line
		reobject.dwFlags = REO_BELOWBASELINE;
		//reobject.dwUser = (DWORD)myObject;

		//the very object
		reobject.poleobj = lpObject;
		//client site contain the object
		reobject.polesite = lpClientSite;
		//the storage 
		reobject.pstg = lpStorage;

		SIZEL sizel = { 0 };
		reobject.sizel = sizel;


		LPOLECLIENTSITE lpObjectClientSite = NULL;
		hr = lpObject->GetClientSite(&lpObjectClientSite);
		if (FAILED(hr) || lpObjectClientSite == NULL)
			lpObject->SetClientSite(lpClientSite);

		pRichEditOle->InsertObject(&reobject);
		//redraw the window to show animation
		::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);


		if (lpClientSite)
		{
			lpClientSite->Release();
			lpClientSite = NULL;
		}
		if (lpObject)
		{
			lpObject->Release();
			lpObject = NULL;
		}
		if (lpLockBytes)
		{
			lpLockBytes->Release();
			lpLockBytes = nullptr;
		}
		if (lpStorage)
		{
			lpStorage->Release();
			lpStorage = NULL;
		}

		if (lpAnimator)
		{
			lpAnimator->Release();
			lpAnimator = NULL;
		}
		if (pRichEditOle)
		{
			pRichEditOle->Release();
		}


	}
	catch (...)
	{
		if (lpClientSite)
		{
			lpClientSite->Release();
			lpClientSite = NULL;
		}
		if (lpObject)
		{
			lpObject->Release();
			lpObject = NULL;
		}
		if (lpStorage)
		{
			lpStorage->Release();
			lpStorage = NULL;
		}

		if (lpAnimator)
		{
			lpAnimator->Release();
			lpAnimator = NULL;
		}

		return false;
	}
	return true;
}

HRESULT UIIMEdit::GetNewStorage(LPSTORAGE* ppStg)
{

	if (!ppStg)
		return E_INVALIDARG;

	*ppStg = NULL;

	//
	// We need to create a new storage for an object to occupy.  We're going
	// to do this the easy way and just create a storage on an HGLOBAL and let
	// OLE do the management.  When it comes to saving things we'll just let
	// the RichEdit control do the work.  Keep in mind this is not efficient, 
	// but this program is just for demonstration.
	//

	LPLOCKBYTES pLockBytes;
	HRESULT hr = CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);
	if (FAILED(hr))
		return hr;

	hr = StgCreateDocfileOnILockBytes(pLockBytes,
		STGM_SHARE_EXCLUSIVE | STGM_CREATE |
		STGM_READWRITE,
		0,
		ppStg);
	pLockBytes->Release();
	return (hr);
}


void UIIMEdit::GetObjectInfo(IRichEditOle *pIRichEditOle)
{
	long count = pIRichEditOle->GetObjectCount();
	if (count)
	{
		REOBJECT reobj = { 0 };
		reobj.cbStruct = sizeof(REOBJECT);
		pIRichEditOle->GetObject(0, &reobj, REO_GETOBJ_POLEOBJ);
		GifSmiley::IGifSmileyCtrl* lpAnimator;
		HRESULT hr = reobj.poleobj->QueryInterface(GifSmiley::IID_IGifSmileyCtrl, (void**)&lpAnimator);
		if (SUCCEEDED(hr))
		{
			BSTR*  fileName = nullptr;
			hr = lpAnimator->FileName(fileName);
		}
		reobj.poleobj->Release();
	}
}

void UIIMEdit::ReleaseAllGif()
{
	IRichEditOle *pRichEditOle = GetRichEditOle();
	if (NULL == pRichEditOle)
	{
		return;
	}

	for (int i = 0; i < pRichEditOle->GetObjectCount(); i++)
	{
		REOBJECT reobj = { 0 };
		reobj.cbStruct = sizeof(REOBJECT);
		pRichEditOle->GetObject(i, &reobj, REO_GETOBJ_POLEOBJ);
		GifSmiley::IGifSmileyCtrl* lpAnimator;
		HRESULT hr = reobj.poleobj->QueryInterface(GifSmiley::IID_IGifSmileyCtrl, (void**)&lpAnimator);
		if (SUCCEEDED(hr))
		{
			BSTR filename;
			lpAnimator->FileName(&filename);
			lpAnimator->FreeImage();
			lpAnimator->Release();
		}
		reobj.poleobj->Release();
	}
}

bool UIIMEdit::GetPicPosAndPathbyOrder(IN UInt32 nOrder, OUT UInt32& nPos, OUT CString& path)
{
	REOBJECT reobj = { 0 };
	reobj.cbStruct = sizeof(REOBJECT);
	IRichEditOle *pRichEditOle = GetRichEditOle();
	pRichEditOle->GetObject(nOrder, &reobj, REO_GETOBJ_POLEOBJ);
	GifSmiley::IGifSmileyCtrl* lpAnimator;
	HRESULT hr = reobj.poleobj->QueryInterface(GifSmiley::IID_IGifSmileyCtrl, (void**)&lpAnimator);
	if (SUCCEEDED(hr))
	{
		CComBSTR  fileName;
		hr = lpAnimator->FileName(&fileName);
		if (SUCCEEDED(hr) && 0 != fileName.Length())
		{
			nPos = reobj.cp;
			path = CString(fileName.m_str);
		}
	}
	reobj.poleobj->Release();
	return TRUE;
}

LRESULT UIIMEdit::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		if ('V' == wParam && ::GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			_ImEditPaste();
		}
		else if (VK_RETURN == wParam)//回车
		{
			SetWantReturn(!WantCtrlEnterBySysSetting());
		}
	}
	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
}

bool UIIMEdit::OnTxViewChanged()
{
	m_pManager->SendNotify(this, UIIMEdit_MSGTYPE_TEXTCHANGED);
	return true;
}

BOOL UIIMEdit::WantCtrlEnterBySysSetting()
{
	module::TTConfig* pTTConfig = module::getSysConfigModule()->getSystemConfig();
	if (pTTConfig)
	{
		if (pTTConfig->sysBaseFlag & module::BASE_FLAG_SENDIMG_BY_CTRLENTER)
		{
			return TRUE;
		}
	}
	return FALSE;
}


/******************************************************************************/