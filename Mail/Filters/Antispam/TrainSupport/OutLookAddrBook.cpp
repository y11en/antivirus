
#include <mapix.h>

#include "../../../interceptors/OutlookPlugin/mapiedk/mapiedk.h"
static CMAPIEDK* g_pMAPIEDK = NULL;

#include "OutLookAddrBook.h"

#define PR_SMTP_ADDRESS PROP_TAG(PT_TSTRING, 0x39FE)

namespace MAPIAccess
{
	_COM_SMARTPTR_TYPEDEF(IMAPISession, IID_IMAPISession);
	_COM_SMARTPTR_TYPEDEF(IMailUser, IID_IMailUser);
	_COM_SMARTPTR_TYPEDEF(IDistList, IID_IDistList);
	_COM_SMARTPTR_TYPEDEF(IMAPITable, IID_IMAPITable);
	_COM_SMARTPTR_TYPEDEF(IMsgStore, IID_IMsgStore);
	_COM_SMARTPTR_TYPEDEF(IMAPIFolder, IID_IMAPIFolder);
	_COM_SMARTPTR_TYPEDEF(IProfAdmin, IID_IProfAdmin);

	CMAPIInitializer::CMAPIInitializer()
	{
		MAPIINIT_0 mapiInit = {MAPI_INIT_VERSION, MAPI_MULTITHREAD_NOTIFICATIONS}; 
		
		HRESULT hr = E_FAIL;

		if (GetMAPIEDK(g_pMAPIEDK, NULL))
		{
			hr = g_pMAPIEDK->pMAPIInitialize(&mapiInit);
		}		

		if (FAILED(hr))
			throw CMAPIException("MAPI initialize failed");
	}

	CMAPIInitializer::~CMAPIInitializer()
	{
		g_pMAPIEDK->pMAPIUninitialize();
	}

	COutLookAddrBook::COutLookAddrBook()
	{
		m_mapiRef.reset(new CMAPIInitializer);
	}

	COutLookAddrBook::~COutLookAddrBook()
	{
	}

	bool COutLookAddrBook::TryMAPIInitialize()
	{
		try
		{
			CMAPIInitializer m_initializer;
			return true;
		}
		catch (const CMAPIException&)
		{			
		}
		return false;
	}

	bool COutLookAddrBook::GetProfilesCount(unsigned int& profileCount)
	{
		profileCount = 0;

		IProfAdminPtr pProfAdmin;
		if (S_OK == g_pMAPIEDK->pMAPIAdminProfiles(0, &pProfAdmin))
		{
			IMAPITablePtr pProfileTable;
			if (S_OK == pProfAdmin->GetProfileTable(0, &pProfileTable))
			{
				ULONG ulRowCount = 0;
				if (S_OK == pProfileTable->GetRowCount(0, &ulRowCount))
				{
					profileCount = ulRowCount;
				}
			}
		}

		return false;
	}

	bool COutLookAddrBook::GetProfiles(Profiles& profiles)
	{
		profiles.clear();

		IProfAdminPtr pProfAdmin;
		if (S_OK == g_pMAPIEDK->pMAPIAdminProfiles(0, &pProfAdmin))
		{
			IMAPITablePtr pProfTable;
			if (S_OK == pProfAdmin->GetProfileTable(0, &pProfTable))
			{
				SizedSPropTagArray ( 2, sptCols ) = {2, PR_DISPLAY_NAME, PR_DEFAULT_PROFILE};
				LPSRowSet pRowSet = NULL;
				if (S_OK == g_pMAPIEDK->pHrQueryAllRows(pProfTable, reinterpret_cast<SPropTagArray*>(&sptCols), NULL, NULL, 0, &pRowSet))
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
					return true;
				}
			}
		}

		return false;
	}


	bool COutLookAddrBook::SessionLogOn(ULONG hWnd, LPCTSTR profile, LPCTSTR pswd, FLAGS flags)
	{
		IMAPISessionPtr pSession;
		HRESULT hr = g_pMAPIEDK->pMAPILogonEx(hWnd, (LPTSTR)profile, (LPTSTR)pswd, flags, &pSession);

		if (FAILED(hr))
			return false;
	
		if (pSession)
		{
			if (S_OK == pSession->OpenAddressBook(NULL, NULL, AB_NO_DIALOG, &m_pAddrBook))
			{
				return true;
			}
		}

		return false;
	}


	bool COutLookAddrBook::GetRootFolder(CAddrFolder& folderHandler)
	{
		if (!m_pAddrBook)
			return false;

		// open the address book's root container
		ULONG ulObjType = 0;
		IUnknownPtr pUnk;
		if (S_OK == m_pAddrBook->OpenEntry(0, NULL, NULL, MAPI_BEST_ACCESS, &ulObjType, &pUnk))
		{
			if (MAPI_ABCONT == ulObjType)
			{
				IABContainerPtr pContainer;
				folderHandler.m_pContainer.Attach(reinterpret_cast<IABContainer*>(pUnk.Detach()));
				folderHandler.m_mapiRef = m_mapiRef;
				folderHandler.m_pAddrBook = m_pAddrBook;
				folderHandler.m_name = ""; // root folder name
				return true;			
			}			
		}

		return false;
	}



	bool CAddrFolder::GetContents(Strings& contents) const
	{
		contents.clear();

		if (!m_pContainer)
			return false;
				
		IMAPITablePtr pContentTable;
		if (S_OK == m_pContainer->GetContentsTable(0, &pContentTable))
		{
			SizedSPropTagArray ( 4, sptCols ) = {4, PR_DISPLAY_NAME, PR_SMTP_ADDRESS, PR_EMAIL_ADDRESS, PR_ADDRTYPE};
			LPSRowSet pRowSet = NULL;
			if (S_OK == g_pMAPIEDK->pHrQueryAllRows(pContentTable, reinterpret_cast<SPropTagArray*>(&sptCols), NULL, NULL, 0, &pRowSet))
			{
				contents.reserve(pRowSet->cRows);
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
								contents.push_back(lpDN->Value.lpszA);
						}
					}
					else
					{
						if (LPSPropValue lpDN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_SMTP_ADDRESS))
						{
							if (lpDN->Value.lpszA)
								contents.push_back(lpDN->Value.lpszA);
						}
					}					
				}

				g_pMAPIEDK->pFreeProws(pRowSet);

				return true;
			}
		}		

		return false;
	}

	bool CAddrFolder::GetChildFolders(AddrFolders& folders) const
	{
		folders.clear();

		if (!m_pContainer)
			return false;

		IMAPITablePtr pChildTable;
		if (S_OK == m_pContainer->GetHierarchyTable(0, &pChildTable))
		{
			SizedSPropTagArray ( 2, sptCols ) = {2, PR_DISPLAY_NAME, PR_ENTRYID};
			LPSRowSet pRowSet = NULL;
			if (S_OK == g_pMAPIEDK->pHrQueryAllRows(pChildTable, reinterpret_cast<SPropTagArray*>(&sptCols), NULL, NULL, 0, &pRowSet))
			{
				for (ULONG i = 0; i < pRowSet->cRows; ++i)
				{
					string name;
					if (LPSPropValue pPVN = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_DISPLAY_NAME))
					{
						name = pPVN->Value.lpszA ? pPVN->Value.lpszA : "";
					}

					IABContainerPtr pChildContainer;
					if (LPSPropValue pPVE = g_pMAPIEDK->pPpropFindProp(pRowSet->aRow[i].lpProps, pRowSet->aRow[i].cValues, PR_ENTRYID))
					{
						SBinary entry = pPVE->Value.bin;
						ULONG ulObjType = 0;
						IUnknownPtr pUnk;

						if (S_OK == m_pContainer->OpenEntry(entry.cb, reinterpret_cast<ENTRYID*>(entry.lpb), NULL, MAPI_BEST_ACCESS, &ulObjType, &pUnk))
						{
							if (MAPI_ABCONT == ulObjType)
							{
								pChildContainer.Attach(reinterpret_cast<IABContainer*>(pUnk.Detach()));
							}
						}
					}

					if (pChildContainer)
					{
						folders.push_back(CAddrFolder(pChildContainer, name.c_str(), m_mapiRef, m_pAddrBook));
					}
				}

				g_pMAPIEDK->pFreeProws(pRowSet);

				return true;
			}
		}

		return false;
	}

} // namespace MAPIAccess



