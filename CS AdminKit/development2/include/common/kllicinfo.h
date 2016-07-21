/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	kllicinfo.h
 * \author	Andrew Kazachkov
 * \date	04.06.2003 11:13:00
 * \brief	
 * 
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef __KL_LICINFO_H__
#define __KL_LICINFO_H__

namespace KLLIC
{
    //Errors
	enum{
		KLLIC_KEY_CHECK_FAILED=KLLICERRSTART,
        KLLIC_KEY_INSTALL_FAILED,
        KLLIC_BAD_VERSION,
        KLLIC_CANT_LOAD_MODULE,        
        KLLIC_PRODUCT_NOT_FOUND,
        KLLIC_CANT_GET_PRODUCT_FOLDER,
        KLLIC_PRODUCT_INFO_ABSENT,
        KLLIC_KEY_SIGN_MISTMACH,
        KLLIC_APP_INFO_SIGN_MISMATCH,
        KLLIC_APPINFO_CANT_RESTORE,
        KLLIC_KAVSETXML_DATE_CANT_GET,
        KLLIC_KAVSETXML_SIGN_MISMATCH,
        KLLIC_KEYINFO_CANT_RESTORE,
        KLLIC_BLACKLIST_NOT_FOUND,
        KLLIC_KEY_SERIAL_NOT_FOUND
	};

	const KLERR::ErrorDescription c_errorDescriptions[]={
		{KLLIC_KEY_CHECK_FAILED,		L"Checking key validity failed"},
        {KLLIC_KEY_INSTALL_FAILED,		L"Key installation failed"},
        {KLLIC_BAD_VERSION,             L"Module '%ls' has bad version"},
        {KLLIC_CANT_LOAD_MODULE,        L"Cannot load module '%ls' ('%ls')"},
        {KLLIC_PRODUCT_NOT_FOUND,       L"Cannot install key because product '%u' is not installed"},        
        {KLLIC_CANT_GET_PRODUCT_FOLDER, L"Cannot obtain folder for product '%ls'"},
        {KLLIC_PRODUCT_INFO_ABSENT,     L"Cannot install key because product info file is absent"},
        {KLLIC_KEY_SIGN_MISTMACH,       L"Key signature mistmach"},
        {KLLIC_APP_INFO_SIGN_MISMATCH,  L"Appinfo signature mistmach"},
        {KLLIC_APPINFO_CANT_RESTORE,    L"Cannot obtain application info"},
        {KLLIC_KAVSETXML_DATE_CANT_GET, L"Cannot obtain kavset date"},
        {KLLIC_KAVSETXML_SIGN_MISMATCH, L"Kavset signature mistmach"},
        {KLLIC_KEYINFO_CANT_RESTORE,    L"Cannot obtain key info"},
        {KLLIC_BLACKLIST_NOT_FOUND,     L"Cannot found black list"},
        {KLLIC_KEY_SERIAL_NOT_FOUND,    L"Cannot obtain key serial number"}

	};

    const wchar_t c_szwTaskInstallKey[]=L"InstallKey";

    const wchar_t c_szwKeyContents[]=L"KLLIC_KEY_CONTENTS"; //BINARY_T
    const wchar_t c_szwKeyFileName[]=L"KLLIC_KEY_FNAME";
    const wchar_t c_szwIsCurrentKey[]=L"KLLIC_IS_CURRENT_KEY";

	// Эти макросы определяются в SettingsStorage.h, но для версии 0.12b они дублируются здесь:
	//! Информация о лицензионных ключах в Product Info Settings Storage
	#define KLPRSS_SECTION_LIC_INFO			L"LicensingInfo"
	//! информация об активном лицензионном ключе
	#define KLPRSS_VAL_LIC_KEY_CURRENT		L"CurrentKey"
	//! информация о резервном лицензионном ключе
	#define KLPRSS_VAL_LIC_KEY_NEXT			L"NextKey"


    #define KLLIC_KEY_TYPE                      L"KLLIC_KEY_TYPE"
    #define KLLIC_PROD_SUITE_ID                 L"KLLIC_PROD_SUITE_ID"
    #define KLLIC_APP_ID                        L"KLLIC_APP_ID"
    #define KLLIC_INSTALL_DATE                  L"KLLIC_INSTALL_DATE"
    #define KLLIC_CREATION_DATE                 L"KLLIC_CREATION_DATE"
    #define KLLIC_LIMIT_DATE                    L"KLLIC_LIMIT_DATE"
	#define KLLIC_EXPIRATION_DATE               L"KLLIC_EXPIRATION_DATE"
    #define KLLIC_LICENSE_PERIOD                L"KLLIC_LICENSE_PERIOD"
    #define KLLIC_LIC_COUNT                     L"KLLIC_LIC_COUNT"
    #define KLLIC_EFFECTIVE_LICENSE_PERIOD      L"KLLIC_EFFECTIVE_LICENSE_PERIOD"
    #define KLLIC_EFFECTIVE_LIC_COUNT           L"KLLIC_EFFECTIVE_LIC_COUNT"
    #define KLLIC_SERIAL                        L"KLLIC_SERIAL"
    #define KLLIC_MAJ_VER                       L"KLLIC_MAJ_VER"
    #define KLLIC_PROD_NAME                     L"KLLIC_PROD_NAME"
    #define KLLIC_LICINFO                       L"KLLIC_LICINFO"
    #define KLLIC_SUPPORT_INFO                  L"KLLIC_SUPPORT_INFO"
    #define KLLIC_CUSTOMER_INFO                 L"KLLIC_CUSTOMER_INFO"


	//! Тип ключа (Commercial, Beta, Trial, Test, OEM)
    const wchar_t c_szwKeyType[] = KLLIC_KEY_TYPE;	// INT_T
    
	//! Идентификатор продукта, прописанный в лицензионном ключе
    const wchar_t c_szwKeyProdSuiteID[]=KLLIC_PROD_SUITE_ID;	// INT_T

	//! Идентификатор приложения, прописанный в лицензионном ключе
    const wchar_t c_szwKeyAppID[]=KLLIC_APP_ID;	// INT_T

	//! Дата и время инсталляции ключа
    const wchar_t c_szwKeyInstallDate[]=KLLIC_INSTALL_DATE; // DATE_TIME_T

	//! Дата и время создания ключа
    const wchar_t c_szwKeyCreationDate[]=KLLIC_CREATION_DATE; // DATE_TIME_T

	//! Дата и время устаревания ключа
    const wchar_t c_szwKeyLimitDate[]=KLLIC_LIMIT_DATE; // DATE_TIME_T
    
    //! descriptional license info
    const wchar_t c_szwKeyLicenseInfo[] = KLLIC_LICINFO; // STRING_T    

    //! support company properties
    const wchar_t c_szwKeySupportInfo[] = KLLIC_SUPPORT_INFO; // STRING_T

    //! customer info
    const wchar_t c_szwKeyCustomerInfo[] = KLLIC_CUSTOMER_INFO; // STRING_T

	//! Дата окончания действия лицензии на данном компьютере
	const wchar_t c_szwKeyExpirationDate[]=KLLIC_EXPIRATION_DATE;	// DATE_TIME_T

	//! Срок действия лицензии
    const wchar_t c_szwKeyLicPeriod[]=KLLIC_LICENSE_PERIOD;    	// INT_T

	//! Ограничение на количество лицензий
    const wchar_t c_szwKeyLicCount[]=KLLIC_LIC_COUNT;	// INT_T

	//! Срок действия лицензии
    const wchar_t c_szwKeyLicEffectivePeriod[]=KLLIC_EFFECTIVE_LICENSE_PERIOD;    	// INT_T

	//! Ограничение на количество лицензий
    const wchar_t c_szwKeyLicEffectiveCount[]=KLLIC_EFFECTIVE_LIC_COUNT;	// INT_T

	//! Серийный номер ключа
    const wchar_t c_szwKeySerial[]=KLLIC_SERIAL;	// STRING_T

	//! Старшая версия продукта, прописанная в лицензионном ключе
    const wchar_t c_szwKeyMajVer[]=KLLIC_MAJ_VER;	// INT_T

	//! Имя приложения, прописанное в лицензионном ключе
    const wchar_t c_szwKeyProdName[]=KLLIC_PROD_NAME;	// STRING_T

    //! container containing allowed autokeys
    const wchar_t c_szwAllowedAutoKeys[]=L"KLLIC_ALLOWED_AUTOKEYS";	// PARAMS_T

    //! array of serial numbers of disallowed autokeys
    const wchar_t c_szwDisallowedAutoKeys[]=L"KLLIC_DISALLOWED_AUTOKEYS";	// ARRAY_T|STRING_T

    //! license file body, BINARY_T
    #define KLLIC_LICFILE       L"KLLIC_LICFILE"

    //! license info file, STRING_T
    #define KLLIC_LICENSE_INFO L"KLLIC_LICENSE_INFO"

    //! support info file, STRING_T
    #define KLLIC_SUPPORT_INFO L"KLLIC_SUPPORT_INFO"

    //! customer info file, STRING_T
    #define KLLIC_CUSTOMER_INFO L"KLLIC_CUSTOMER_INFO"


    //! Events
    #define KLLIC_EVENT_CODES_BASE  1024L
    
    //each section for these product/version contains key body (as encrypted binary ADMLIC_KEYDATA)
    #define ADMLIC_KEYDATA_PRODUCT         L"ADMLIC_KEYDATA"
    #define ADMLIC_KEYDATA_VERSION         L"1.0"

    #define ADMLIC_KEYDATA                  L"KeyData" //BINARY_T

    //each section for these product/version means key info
    #define ADMLIC_KEYINFO_PRODUCT         L"ADMLIC_KEYINFO"
    #define ADMLIC_KEYINFO_VERSION         L"1.0"

};

#endif //__KL_LICINFO_H__
