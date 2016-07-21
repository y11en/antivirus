#include "stdafx.h"

#include <mapix.h>
#include "../../../interceptors/OutlookPlugin/mapiedk/mapiedk.h"
#include "OLAddrBook.h"

#define PR_SMTP_ADDRESS PROP_TAG(PT_TSTRING, 0x39FE)

namespace AddrBook
{

	static CMAPIEDK* g_pMAPIEDK = NULL;	

	COLAddrBookInitializer::COLAddrBookInitializer():m_pAddrBook(NULL), m_bInited(false)
	{
		MAPIINIT_0 mapiInit = {MAPI_INIT_VERSION, MAPI_MULTITHREAD_NOTIFICATIONS};

		HRESULT hr = E_FAIL;

		if (GetMAPIEDK(g_pMAPIEDK, NULL))
		{
			hr = g_pMAPIEDK->pMAPIInitialize(&mapiInit);
		}		

		m_bInited = SUCCEEDED(hr);
	}

	void COLAddrBookInitializer::SetAddrBook(LPADRBOOK pAddrBook)
	{
		if (!m_pAddrBook)
		{
			m_pAddrBook = pAddrBook;
			m_pAddrBook->AddRef();
		}
	}

	COLAddrBookInitializer::~COLAddrBookInitializer()
	{
		if (m_pAddrBook)
			m_pAddrBook->Release();

		if (g_pMAPIEDK)
			g_pMAPIEDK->pMAPIUninitialize();
	}

	//////////////////////////////////////////////////////////////////////////

	COLFolderBase::COLFolderBase(const char * name, const TOLInitializer& initRef, LPABCONT pABCont):
	CAddrFolderBase(name), m_initRef(initRef), m_pABCont(pABCont)
	{
		if (m_pABCont)
			m_pABCont->AddRef();
	}

	COLFolderBase::~COLFolderBase()
	{
		if (m_pABCont)
			m_pABCont->Release();
	}

	bool COLFolderBase::GetChildFolders(AddrFolders& folders)
	{
		folders.clear();

		if (ChildRequest())
		{
			folders = m_childs;
			return true;
		}

		return false;
	}

	void COLFolderBase::ExtractChildFolders()
	{
		m_childs.clear();

		if (!m_initRef->Inited())
			return;

		if (!m_pABCont)
			return;

		LPMAPITABLE pChildTable = NULL;
		if (SUCCEEDED(m_pABCont->GetHierarchyTable(0, &pChildTable)))
		{
			SizedSPropTagArray ( 2, sptCols ) = {2, PR_DISPLAY_NAME, PR_ENTRYID};
			LPSRowSet pRowSet = NULL;
			if (SUCCEEDED(g_pMAPIEDK->pHrQueryAllRows(pChildTable, reinterpret_cast<SPropTagArray*>(&sptCols), NULL, NULL, 0, &pRowSet)))
			{
				for (ULONG i = 0; i < pRowSet->cRows; ++i)
				{
					string name;
					if (LPSPropValue pPVN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_DISPLAY_NAME))
					{
						name = pPVN->Value.lpszA ? pPVN->Value.lpszA : "";
					}

					LPABCONT pChildContainer = NULL;
					if (LPSPropValue pPVE = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_ENTRYID))
					{
						SBinary entry = pPVE->Value.bin;
						ULONG ulObjType = 0;
						LPUNKNOWN pUnk = NULL;

						if (SUCCEEDED(m_pABCont->OpenEntry(entry.cb, (LPENTRYID)entry.lpb, NULL, MAPI_BEST_ACCESS, &ulObjType, &pUnk)))
						{
							if (MAPI_ABCONT == ulObjType)
							{
								pChildContainer = (LPABCONT)pUnk;
								pChildContainer->AddRef();
							}
						}

						if (pUnk)
							pUnk->Release();
					}

					if (pChildContainer)
					{
						m_childs.push_back(TAddrFolderPtr(new COLAddrFolder(name.c_str(), m_initRef, pChildContainer)));
						pChildContainer->Release();
					}
				}

				g_pMAPIEDK->pFreeProws(pRowSet);
				
			}
		}

		if (pChildTable)
			pChildTable->Release();

	}
	
	//////////////////////////////////////////////////////////////////////////

	COLAddrFolder::COLAddrFolder(const char * name, const TOLInitializer& initRef, LPABCONT pABCont):
	COLFolderBase(name, initRef, pABCont), m_bIsChildExtracted(false), m_bIsContentExtracted(false)
	{
	}

	bool COLAddrFolder::GetContents(Strings& strings)
	{
		strings.clear();
		
		if (!m_bIsContentExtracted)
		{
			ExtractContents();
			m_bIsContentExtracted = true;
		}

		strings = m_contents;		
		return true;
	}

	void COLAddrFolder::ExtractContents()
	{
		m_contents.clear();

		if (!m_initRef->Inited())
			return;

		if (!m_pABCont)
			return;

		LPMAPITABLE pContentTable = NULL;
		if (SUCCEEDED(m_pABCont->GetContentsTable(0, &pContentTable)))
		{
			SizedSPropTagArray ( 4, sptCols ) = {4, PR_DISPLAY_NAME, PR_SMTP_ADDRESS, PR_EMAIL_ADDRESS, PR_ADDRTYPE};
			LPSRowSet pRowSet = NULL;
			if (SUCCEEDED(g_pMAPIEDK->pHrQueryAllRows(pContentTable, reinterpret_cast<SPropTagArray*>(&sptCols), NULL, NULL, 0, &pRowSet)))
			{
				m_contents.reserve(pRowSet->cRows);
				for (ULONG i = 0; i < pRowSet->cRows; ++i)
				{
					string addrType;
					if (LPSPropValue lpDN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_ADDRTYPE))
					{
						addrType = lpDN->Value.lpszA ? lpDN->Value.lpszA : "";
					}

					if (0 == addrType.compare("SMTP"))
					{
						if (LPSPropValue lpDN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_EMAIL_ADDRESS))
						{
							if (lpDN->Value.lpszA)
								m_contents.push_back(lpDN->Value.lpszA);
						}
					}
					else
					{
						if (LPSPropValue lpDN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_SMTP_ADDRESS))
						{
							if (lpDN->Value.lpszA)
								m_contents.push_back(lpDN->Value.lpszA);
						}
					}					
				}

				g_pMAPIEDK->pFreeProws(pRowSet);

			}
		}		
		
		if (pContentTable)
			pContentTable->Release();
	}

	bool COLAddrFolder::ChildRequest()
	{
		if (!m_bIsChildExtracted)
		{
			ExtractChildFolders();
			m_bIsChildExtracted = true;
		}

		return true;
	}


	//////////////////////////////////////////////////////////////////////////

	bool COLAddrBook::Init()
	{
		m_initRef.reset(new COLAddrBookInitializer);
		return m_initRef->Inited();
	}
	
	bool COLAddrBook::ChildRequest()
	{
		if (!m_bIsChildExtracted)
		{
			ExtractChildFolders();
			m_bIsChildExtracted = true;
		}

		return true;
	}

	void COLAddrBook::ExtractChildFolders()
	{
		if (!m_initRef->Inited())
			return;

		Profiles profiles;
		GetProfiles(profiles);
		
		if (1 == profiles.size())
		{
			// профиль только один, его и будем открывать
			SessionLogOn(NULL, "", "", MAPI_NO_MAIL | MAPI_EXTENDED | MAPI_NEW_SESSION | MAPI_USE_DEFAULT);
		}
		else
		if (!profiles.empty())
		{
			// профилей более одного, запрашиваем пользователя
			SessionLogOn(NULL, "", "", MAPI_NO_MAIL | MAPI_EXTENDED | MAPI_NEW_SESSION | MAPI_LOGON_UI);
		}		

		COLFolderBase::ExtractChildFolders();
	}

	void COLAddrBook::GetProfiles(Profiles& profiles)
	{
		profiles.clear();

		LPPROFADMIN pProfAdmin = NULL;
		if (SUCCEEDED(g_pMAPIEDK->pMAPIAdminProfiles(0, &pProfAdmin)))
		{
			LPMAPITABLE pProfTable = NULL;
			if (SUCCEEDED(pProfAdmin->GetProfileTable(0, &pProfTable)))
			{
				SizedSPropTagArray ( 2, sptCols ) = {2, PR_DISPLAY_NAME, PR_DEFAULT_PROFILE};
				LPSRowSet pRowSet = NULL;

				if (SUCCEEDED(g_pMAPIEDK->pHrQueryAllRows(pProfTable, reinterpret_cast<SPropTagArray*>(&sptCols), NULL, NULL, 0, &pRowSet)))
				{
					for (ULONG i = 0; i < pRowSet->cRows; ++i)
					{
						bool bDefaultProfile = false;
						if (LPSPropValue pPVN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_DEFAULT_PROFILE))
						{
							bDefaultProfile = pPVN->Value.b != 0;
						}

						if (LPSPropValue pPVN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_DISPLAY_NAME))
						{
							profiles.push_back(std::make_pair(pPVN->Value.lpszA ? pPVN->Value.lpszA : "", bDefaultProfile));
						}
					}

					g_pMAPIEDK->pFreeProws(pRowSet);
				}
			}
			
			if (pProfTable)
				pProfTable->Release();
		}

		if (pProfAdmin)
			pProfAdmin->Release();

	}

	bool COLAddrBook::SessionLogOn(ULONG hWnd, LPCTSTR profile, LPCTSTR pswd, ULONG flags)
	{
		bool bResult = false;
		LPMAPISESSION pSession = NULL;
		HRESULT hr = g_pMAPIEDK->pMAPILogonEx(hWnd, (LPTSTR)profile, (LPTSTR)pswd, flags, &pSession);

		if (pSession)
		{
			LPADRBOOK pAddrBook = NULL;
			if (SUCCEEDED(pSession->OpenAddressBook(NULL, NULL, AB_NO_DIALOG, &pAddrBook)))
			{
				m_initRef->SetAddrBook(pAddrBook);

				// открываем корневой каталог
				ULONG ulObjType = 0;
				LPUNKNOWN pUnk = NULL;
				if (SUCCEEDED(pAddrBook->OpenEntry(0, NULL, NULL, MAPI_BEST_ACCESS, &ulObjType, &pUnk)))
				{
					if (MAPI_ABCONT == ulObjType)
					{
						LPABCONT pContainer = NULL;
						m_pABCont = (LPABCONT)pUnk;
						m_pABCont->AddRef();
						bResult = true;
					}			
				}

				if (pUnk)
					pUnk->Release();
			}

			if (pAddrBook)
				pAddrBook->Release();
		}

		if (pSession)
			pSession->Release();

		return bResult;
	}


}