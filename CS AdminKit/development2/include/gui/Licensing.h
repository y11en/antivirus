// Licensing.h: interface for the KLADMSRV::DataProtection 
//
//////////////////////////////////////////////////////////////////////

#if !defined(_LICENSING_H_INCLUDED_)
#define _LICENSING_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <std/base/klstd.h>

namespace KLPlugin {

class KLSTD_NOVTABLE CLicensing : public KLSTD::KLBase
{
public:
	class CLicenseInfo {
	public:
		typedef enum {
			ktUnknown = 0,
			ktBeta,
			ktTrial,
			ktTest,
			ktOEM,
			ktCommercial
		} KEY_TYPE;

		bool m_bExpired;
		std::wstring m_wstrSerialNumber;	// key file serial number 
		KEY_TYPE m_eType;					// key type (commercial, trial, ...) 
		long m_lLifeSpan;					// key validity period since installation (days)
		std::wstring m_wstrExpirationDate;	// key expiration date as text in users locale
		long m_lLicenseCount;				// license number (key ver 1-3)
		std::wstring m_wstrProductName;		// product name
		long m_lAppId;						// application ID
		long m_lProductId;					// product id
		std::wstring m_wstrProductVer;		// major product version
		std::wstring m_wstrComment;			// descriptional license info
		std::wstring m_wstrSupportInfo;		// support company properties
		std::wstring m_wstrCustomerInfo;	// customer info
	};

	// Может выбросить исключение KLSTD::STDE_BADFORMAT в 
	// случае неправильного формата файла лицензии
    virtual void GetLicenseInfo(const wchar_t* pwchLicenseFilePath,
								CLicenseInfo& licinfo) = 0;

	// Может выбросить исключение KLSTD::STDE_BADFORMAT в 
	// случае неправильного формата файла лицензии
    virtual void GetLicenseInfoFromMemory(const char* pBuffer,
										  size_t nSize,
										  CLicenseInfo& licinfo) = 0;
};
};

#endif // !defined(_LICENSING_H_INCLUDED_)
