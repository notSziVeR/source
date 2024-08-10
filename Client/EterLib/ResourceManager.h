#pragma once

#include "Resource.h"
#include "FileLoaderThread.h"

#include <set>
#include <map>
#include <string>

class CResourceManager : public CSingleton<CResourceManager>
{
public:
	CResourceManager();
	virtual ~CResourceManager();

	void		LoadStaticCache(const char* c_szFileName);
	void		LoadMotionFile(const std::string& c_szFileName);

	void		DestroyDeletingList();
	void		Destroy();

	static void		BeginThreadLoading();
	static void		EndThreadLoading();

	CResource *	InsertResourcePointer(const DWORD dwFileCRC, CResource* pResource);
	CResource *	FindResourcePointer(const DWORD dwFileCRC);
	CResource *	GetResourcePointer(const char * c_szFileName);
	CResource *	GetTypeResourcePointer(const char * c_szFileName, const int iType = -1);

	bool		isResourcePointerData(const DWORD dwFileCRC);

	void		RegisterResourceNewFunctionPointer(const char* c_szFileExt, CResource * (*pResNewFunc)(const char* c_szFileName));
	void		RegisterResourceNewFunctionByTypePointer(const int iType, CResource * (*pNewFunc) (const char* c_szFileName));

	void		DumpFileListToTextFile(const char* c_szFileName);
	static bool IsFileExist(const char * c_szFileName);

	void		Update();
	void		ReserveDeletingResource(CResource * pResource);

public:
	void		ProcessBackgroundLoading();
	void		PushBackgroundLoadingSet(const std::set<std::string> & LoadingSet);

protected:
	void		__DestroyDeletingResourceMap();
	void		__DestroyResourceMap();
	void		__DestroyCacheMap();

	static DWORD	__GetFileCRC(const char * c_szFileName, const char ** c_pszLowerFile = nullptr);

protected:
	typedef std::map<DWORD,	CResource *>									TResourcePointerMap;
	typedef std::map<std::string, CResource* (*)(const char*)>				TResourceNewFunctionPointerMap;
	typedef std::map<int, CResource* (*)(const char*)>						TResourceNewFunctionByTypePointerMap;
	typedef std::map<CResource *, DWORD>									TResourceDeletingMap;
	typedef std::map<DWORD, std::string>									TResourceRequestMap;
	typedef std::map<long, CResource*>										TResourceRefDecreaseWaitingMap;

protected:
	TResourcePointerMap						m_pCacheMap;
	TResourcePointerMap						m_pResMap;
	TResourceNewFunctionPointerMap			m_pResNewFuncMap;
	TResourceNewFunctionByTypePointerMap	m_pResNewFuncByTypeMap;
	TResourceDeletingMap					m_ResourceDeletingMap;
	TResourceRequestMap						m_RequestMap;
	TResourceRequestMap						m_WaitingMap;
	TResourceRefDecreaseWaitingMap			m_pResRefDecreaseWaitingMap;

	static CFileLoaderThread				ms_loadingThread;
};

extern int g_iLoadingDelayTime;