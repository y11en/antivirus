#include "stdafx.h"
#include "OEAddrBook.h"


namespace AddrBook
{
	COEWabHandler::COEWabHandler(): m_fWabOpen(NULL), m_hWABLib(NULL), m_pAddrBook(NULL), m_pWABObj(NULL)
	{
		HKEY keyResult;

		// Запрашиваем путь к wab32.dll
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, WAB_DLL_PATH_KEY, 0, KEY_QUERY_VALUE, &keyResult))
		{
			char path[MAX_PATH];
			DWORD pathLen = MAX_PATH;

			DWORD dwType = 0;
			if (ERROR_SUCCESS == RegQueryValueEx(keyResult, "", NULL, &dwType, (LPBYTE)path, &pathLen))
			{
				string RealPath;
				if (REG_EXPAND_SZ)
				{
					char pathConvert [MAX_PATH];
					ExpandEnvironmentStrings(path, pathConvert, MAX_PATH);
					RealPath = pathConvert;
				}
				else
				{
					RealPath = path;
				}

				if (m_hWABLib = LoadLibrary(RealPath.c_str()))
				{
					m_fWabOpen = (LPWABOPEN)GetProcAddress(m_hWABLib, "WABOpen");
					OpenAddrBook(&m_pAddrBook, &m_pWABObj);	
				}
			}

			RegCloseKey(keyResult);
		}
	}

	COEWabHandler::~COEWabHandler()
	{
		if (m_pWABObj)
			m_pWABObj->Release();
		
		if (m_pAddrBook)
			m_pAddrBook->Release();
		
		if (m_hWABLib)
			FreeLibrary(m_hWABLib);
	}

	bool COEWabHandler::OpenAddrBook(LPADRBOOK * ppAddrBook, LPWABOBJECT * ppWABObj)
	{
		if (!ppAddrBook)
			return false;

		if (!ppWABObj)
			return false;

		if (!m_fWabOpen)
			return false;

		WAB_PARAM params = {0};
		params.cbSize = sizeof(WAB_PARAM);
		params.ulFlags = WAB_ENABLE_PROFILES;
		
		HRESULT hr = (m_fWabOpen)(ppAddrBook, ppWABObj, &params, 0);

		return SUCCEEDED(hr);
	}


	COEAddrBookInitializer::COEAddrBookInitializer(const TOEWabHandler& handler) :
	m_parentRef(handler), m_pAddrBook(NULL), m_pWABObj(NULL)
	{
	}

	COEAddrBookInitializer::~COEAddrBookInitializer()
	{
		if (m_pAddrBook)
			m_pAddrBook->Release();
		

		if (m_pWABObj)
			m_pWABObj->Release();		
	}

	bool COEAddrBookInitializer::GetABContByName(const char * name, LPABCONT * ppABCont)
	{
		if (!m_parentRef)
			return false;

		if (!name)
			return false;

		if (!ppABCont)
			return false;

		*ppABCont = NULL;

		HKEY identityKey;
		bool bResult = true;
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, "Identities", 0, KEY_SET_VALUE, &identityKey))
		{
			bResult = (ERROR_SUCCESS == RegSetValueExA(identityKey, "Last User ID", 0, REG_SZ, (const BYTE*)name, strlen(name)));
			RegCloseKey(identityKey);
		}

		if (bResult)
			bResult = m_parentRef->OpenAddrBook(&m_pAddrBook, &m_pWABObj);

		if (m_pAddrBook)
		{
			ULONG		ulObjType = 0;
			LPUNKNOWN	pUnk = NULL;

			// Открываем корневой каталог адресной книги
			if (SUCCEEDED(m_pAddrBook->OpenEntry(0, NULL, NULL, MAPI_BEST_ACCESS, &ulObjType, &pUnk)))
			{
				switch(ulObjType)
				{
				case MAPI_ABCONT:
					*ppABCont = (LPABCONT)pUnk;
					(*ppABCont)->AddRef();
					break;
				}

				if (pUnk)
					pUnk->Release();
			}
		}

		return bResult;
	}

	void COEAddrBookInitializer::Free(void *ptr)
	{
		if (m_pWABObj)
			m_pWABObj->FreeBuffer(ptr);
	}

	COEFolderBase::~COEFolderBase()
	{
		if (m_pABCont)
			m_pABCont->Release();
	}


	bool COEFolderBase::GetChildFolders(AddrFolders& folders)
	{
		folders.clear();

		if (ChildRequest())
		{
			folders = m_childs;
			return true;
		}

		return false;
	}

	void COEFolderBase::ExtractChildFolders()
	{
		m_childs.clear();

		if (m_pABCont)
		{
			LPMAPITABLE pMAPITable = NULL;
			if (SUCCEEDED(m_pABCont->GetContentsTable(WAB_LOCAL_CONTAINERS, &pMAPITable)))
			{
				ULONG ulRowCount = 0;
				pMAPITable->GetRowCount(0, &ulRowCount);

				if (ulRowCount > 0)
				{
					LPSRowSet pRowSet = NULL;
					if (SUCCEEDED(pMAPITable->QueryRows(ulRowCount, 0, &pRowSet)))
					{
						for (ULONG i = 0; i < pRowSet->cRows; ++i)
						{
							SBinary entry = {0};
							string name;
							SRow * pRow = &pRowSet->aRow[i];
							for (ULONG j = 0; j < pRow->cValues; ++j)
							{	
								SPropValue& prop = pRow->lpProps[j];
								switch (prop.ulPropTag)
								{
								case PR_ENTRYID:
									entry = prop.Value.bin;
									break;
								case PR_DISPLAY_NAME_A:
									name = prop.Value.lpszA ? prop.Value.lpszA : "";
									break;
								}
							}

							if (entry.cb != 0)
							{
								// по entry получаем Address book container
								ULONG ulObjType = 0;
								LPUNKNOWN pUnk = NULL;
								
								if (SUCCEEDED(m_pABCont->OpenEntry(entry.cb, (LPENTRYID)entry.lpb, NULL, MAPI_BEST_ACCESS, &ulObjType, &pUnk)))
								{
									if (MAPI_ABCONT == ulObjType)
									{
										// нашли дочерний каталог, добавляем							
										m_childs.push_back(TAddrFolderPtr(new COEAddrFolder(name.c_str(), (LPABCONT)pUnk, m_initRef)));
									}
								}

								if (pUnk)
									pUnk->Release();
							}

							m_initRef->Free(pRow);
						}
					}

					if (pRowSet)
						m_initRef->Free(pRowSet);
				}
			}

			if (pMAPITable)
				pMAPITable->Release();
		}
	}


	COEIdentityFolder::COEIdentityFolder(const char * name, const char * identityId, const TOEWabInitializer& initRef) :
	COEFolderBase(name, initRef), m_id(identityId ? identityId : "")
	{
	}


	bool COEIdentityFolder::ChildRequest()
	{
		if (m_pABCont)
			return true;

		bool bRes = m_initRef->GetABContByName(m_id.c_str(), &m_pABCont);

		// извлекаем дочерние каталоги
		ExtractChildFolders();

		return bRes;
	}	

	COEAddrFolder::COEAddrFolder(const char * name, LPABCONT pABCont, const TOEWabInitializer& initRef):
	COEFolderBase(name, initRef), m_bIsChildExtracted(false), m_bIsContentExtracted(false)
	{
		m_pABCont = pABCont;
		if (m_pABCont)
			m_pABCont->AddRef();
	}

	COEAddrFolder::~COEAddrFolder()
	{		
	}

	bool COEAddrFolder::GetContents(Strings& strings)
	{
		strings.clear();

		if (!m_pABCont)
			return false;

		if (!m_bIsContentExtracted)
		{
			ExtractContents();
			m_bIsContentExtracted = true;
		}

		strings = m_contents;		
		return true;
	}

	void COEAddrFolder::ExtractContents()
	{
		m_contents.clear();

		LPMAPITABLE pMAPITable = NULL;
		if (SUCCEEDED(m_pABCont->GetContentsTable(WAB_LOCAL_CONTAINERS, &pMAPITable)))
		{
			ULONG ulRowCount = 0;
			pMAPITable->GetRowCount(0, &ulRowCount);

			if (ulRowCount > 0)
			{
				LPSRowSet pRowSet = NULL;
				if (SUCCEEDED(pMAPITable->QueryRows(ulRowCount, 0, &pRowSet)))
				{
					for (ULONG i = 0; i < pRowSet->cRows; ++i)
					{
						SBinary entry = {0};
						string email;
						SRow * pRow = &pRowSet->aRow[i];
						for (ULONG j = 0; j < pRow->cValues; ++j)
						{	
							SPropValue& prop = pRow->lpProps[j];
							switch (prop.ulPropTag)
							{
							case PR_EMAIL_ADDRESS_A:
								email = prop.Value.lpszA ? prop.Value.lpszA : "";
								break;
							}
						}

						if (!email.empty())
						{
							m_contents.push_back(email);								
						}

						m_initRef->Free(pRow);
					}
				}

				if (pRowSet)
					m_initRef->Free(pRowSet);
			}
		}

		if (pMAPITable)
			pMAPITable->Release();
	}

	bool COEAddrFolder::ChildRequest()
	{
		if (!m_pABCont)
			return false;

		if (!m_bIsChildExtracted)
		{
			ExtractChildFolders();
			m_bIsChildExtracted = true;
		}

		return true;
	}

	bool COEAddrBook::Init()
	{
		if (!m_bInited)
		{
			// получаем список identities и создаем доступ к ним
			m_WABhandler.reset(new COEWabHandler);
			m_bInited = m_WABhandler->Inited();
			if (m_bInited)
				m_bInited = CreateIdentitiesList();
		}

		return m_bInited;
	}

	bool COEAddrBook::CreateIdentitiesList()
	{
		m_childs.clear();

		HKEY keyIdent = NULL;
		// откроем хранилище identities
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, "Identities", 0, KEY_ENUMERATE_SUB_KEYS, &keyIdent))
		{
			for (DWORD i = 0; ; ++i)
			{
				const size_t buff_size = 100;
				static char ident_id [buff_size];

				DWORD dwKeyNameLen = buff_size;
				FILETIME ft = {0};
				LONG lRes = RegEnumKeyEx(keyIdent, i, ident_id, &dwKeyNameLen, NULL, NULL, NULL, &ft);

				if (ERROR_SUCCESS == lRes)
				{
					// Получим имя identity
					string keyName = "Identities\\";
					keyName += ident_id;
					HKEY keyDesc = NULL;
					if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, keyName.c_str(), 0, KEY_QUERY_VALUE, &keyDesc))
					{
						const size_t buff_size = 256;
						static char ident_name[buff_size];
						DWORD dwIdentNameLen = buff_size;

						if (ERROR_SUCCESS == RegQueryValueEx(keyDesc, "Username", NULL, NULL, (LPBYTE)ident_name, &dwIdentNameLen))
						{
							TOEWabInitializer initer(new COEAddrBookInitializer(m_WABhandler));
							if (initer->Inited())
								m_childs.push_back(TAddrFolderPtr(new COEIdentityFolder(ident_name, ident_id, initer)));
						}

						RegCloseKey(keyDesc);
					}
				}
				else
					if (ERROR_NO_MORE_ITEMS == lRes)
					{
						break;
					}
			}
			RegCloseKey(keyIdent);
		}

		return !m_childs.empty();
	}

}

