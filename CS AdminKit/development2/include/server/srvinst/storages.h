// Storages.h: interface for the CStorages class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGES_H__D9A893BB_9922_4357_B69C_2C5B26E80203__INCLUDED_)
#define AFX_STORAGES_H__D9A893BB_9922_4357_B69C_2C5B26E80203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define KLSRV_NODOMAIN  L"=nodomain"

namespace KLSRV
{

	class CStorages  
	{
	public:
		CStorages(const std::wstring& wstrRootDirecory);
		virtual ~CStorages();
		std::wstring CreateHostStorageLocation(
                                            const std::wstring&	wstrID,
											const std::wstring&	wstrType,
											AVP_dword			dwFlags);// CF_OPEN_EXISTING, CF_CREATE_NEW, CF_OPEN_ALWAYS

		bool RemoveHostStorage(
									const std::wstring&	wstrID,
									const std::wstring&	wstrType,
									bool				bExcept);
	protected:
		const std::wstring	m_wstrRootDirectory;
		
	};
};

#endif // !defined(AFX_STORAGES_H__D9A893BB_9922_4357_B69C_2C5B26E80203__INCLUDED_)
