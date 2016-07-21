#if defined (_MSC_VER)
  #pragma warning (disable : 4127) // conditional expression is constant
  #pragma warning (disable : 4786)
#endif

#if defined (_WIN32)
    #include "../include/LoadResStr.h"
#endif

#include <algorithm>
#include "../include/licensing_policy.h"
#include "../../sign/sign.h"
#include "../include/helpers.h"
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined (_WIN32)
    #include "../include/getupdateconfig_win32.h"
#else
  #if defined (__unix__)
    #if !defined (__DO_NOT_USE_XML)
      #include <unixxmlhelper.h>
    #endif
  #endif
#endif

#if defined (_WIN32)
  #include <io.h>
  #include "licpolicy_resource.h"
#else

  #if defined (__unix__)
    #include <unistd.h>
    #include <fnmatch.h>
    #include <dirent.h>
    #include <libgen.h>
    #include <errno.h>
    #include "../include/messages.h"
  #else
    #include <errno.h>
    #include "../include/messages.h"
  #endif

#endif

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

using namespace LicensingPolicy;
using namespace SecureData;

#if defined (_WIN32)
  const _TCHAR PathDelimiter = _TCHAR('\\');
#else

  #if defined (__unix__)
    const _TCHAR PathDelimiter = _TCHAR ('/');
  #else

    #if defined (__MWERKS__)
      const _TCHAR PathDelimiter = _TCHAR ('\\');
    #else
      #error Unknown platform
    #endif

  #endif

#endif

const _TCHAR KeyFilePattern[]              = _T ("*.key");
const _TCHAR AppInfoFilePattern[]          = _T ("*.kli");
const _TCHAR KAVsetFilePattern[]           = _T ("kavset.xml");
const _TCHAR BlackListFilePattern[]        = _T ("black.lst");

const file_size_t MaxKAVSetFileSize        = 1024 * 100;
const _TCHAR      UpdateDateAttrPath[]     = _T ("UpdateFiles");
const _TCHAR      UpdateDateAttrName[]     = _T ("UpdateDate");


#if defined (__MWERKS__)

extern "C"
{
  int SIGNAPI _sign_check_buffer_callback ( sign_check_call_back cb,
                                            void* params,
                                            void* buffer,
                                            int buff_len,
                                            int* out_len,
                                            int kasp_key,
                                            void* keys[],
                                            int key_count,
                                            int all_of
                                          );

  void* _memcpy (void *dest, const void *src, unsigned int size)
  {
    return (memcpy (dest, src, size));
  }

  void* (*_ptrAMemAlloc)(unsigned int) = malloc;
  void  (*_ptrAMemFree) (void *)       = free;


}

#endif

#if defined (_WIN32)

void GetCurrentDate(date_t& date)
{
	SYSTEMTIME st = { 0 };
	GetSystemTime(&st);
	date.m_year = st.wYear;
	date.m_month = st.wMonth;
	date.m_day = st.wDay;
}

#else

void GetCurrentDate(date_t& date)
{
  time_t		t = time (0);
  struct tm*	pTime = gmtime (&t);
  date_t		tmp;
  
  if (pTime)
  {
	tmp.m_day	= static_cast<unsigned int> (t.tm_mday);
	tmp.m_month	= static_cast<unsigned int> (t.tm_mon + 1); 
	tmp.m_year	= static_cast<unsigned int> (t.tm_year + 1900);
  }
}

#endif

typedef struct
{
  const std::string   *buf;
  unsigned int         cb;
} sign_callback_param_t;

//-----------------------------------------------------------------------------

const size_t DateTolerance = 2; // This constant is used in date comparison operations
								// as maximum allowed value of date violation.
								// Actually, it's workaround for false licensing violation
								// detection in time zones different from GMT+3 (Moscow).
								// This workaround is needed only if key creation date
								// and anti-virus databases release date are
								// stored as GMT+3 time zone value.

//*****************************************************************************
//
//              CLicensingPolicy
//

//-----------------------------------------------------------------------------

CLicensingPolicy::CLicensingPolicy ()
  : m_initialized (false),
    m_lic_storage (0),
    m_getUpdateConfigProc (0)
{

#if defined (_WIN32)
  sign_init_mem (malloc, free);
#endif
}

//-----------------------------------------------------------------------------

CLicensingPolicy::~CLicensingPolicy ()
{
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::init  (ILicenseStorage *lic_storage,
	const _app_info_t& appInfo, void *context, const string_list_t&  basesPaths,
	checking_mode_t checkMode, bool bIgnoreStorageIntegrityErrors)
{

  if (m_initialized)
  {
    return (S_OK);
  }

  if (!lic_storage ||
      (checkMode != cmFullCheck && checkMode != cmKeyOnly && checkMode != cmKeyAndBlacklist) ||
      (!lic_storage->readSecureData || !lic_storage->writeSecureData)
     )
    return (E_INVALIDARG);

  if (getCurrDate().empty()) return E_UNEXPECTED;
  
  HRESULT hr;

  try
  {

    m_lic_storage = lic_storage;
    m_context = context;
    convertAppInfo (appInfo);

#if defined (_WIN32)

//    m_xmlHelper = new helpers::CMSXMLHelper;
    #if !defined (__DO_NOT_USE_XML)
	    m_getUpdateConfigProc = &UpdateConfig::Win32GetUpdateConfig;
    #endif


#else

  #if defined (__unix__)

    #if !defined (__DO_NOT_USE_XML)

    #endif

  #endif

#endif

    m_basesPaths = basesPaths;
    m_checkMode = checkMode;

	hr = loadSecureData ();

    if (FAILED (hr))
    {
		if (hr == LIC_E_SECURE_DATA_CORRUPT) 
		{
			if (!bIgnoreStorageIntegrityErrors) 
			{
				return (hr);
			}
			else
			{
				m_secureData.clear ();
			}
		}
		else
		{
			return (hr);
		}
    }

    m_initialized = true;

    hr = S_OK;

    updateLastKnownDate (getCurrDate ());
    hr = saveSecureData ();

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::setAppInfo (const _app_info_t&   appInfo,
                                      void                *context
                                     )
{

  if (!isInitialized ())
  {
    return (LIC_E_NOT_INITIALIZED);
  }

  HRESULT hr;

  try
  {
    m_context = context;
    convertAppInfo (appInfo);
    hr = E_NOTIMPL;
  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::setBasesPaths (const string_list_t& basesPaths)
{

  HRESULT   hr;

  try
  {
    m_basesPaths = basesPaths;
    hr = S_OK;
  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkActiveKey (check_info_t *checkInfo)
{

  if (!isInitialized ())
    return (LIC_E_NOT_INITIALIZED);

  if (!checkInfo)
  {
    return (E_INVALIDARG);
  }

  if (getCurrDate().empty()) return E_UNEXPECTED;
  
  HRESULT  hr;


  try
  {

    updateLastKnownDate (getCurrDate ());
    hr = saveSecureData ();

    if (FAILED (hr)) 
    {
        return (hr);
    }
    
    installed_key_info_t    installedKeyInfo;
    file_image_t            keyFileImage;
    check_input_data_t      checkInputData;

    if (FAILED (hr = m_secureData.getActiveKey (&installedKeyInfo, &keyFileImage)))
    {
      return (hr);
    }

    if (S_FALSE == hr)
    {
      // There is no active key at all
      return (LIC_E_NO_ACTIVE_KEY);
    }

    checkInfo->keyFileName = installedKeyInfo.keyFileName;
    checkInputData.controlDateType = ccdCurrent;
    checkInputData.ignoreInstalledKeys = false;

    if (FAILED (hr = prepareChecking (keyFileImage, &checkInputData)))
    {
        return (hr);
    }

    hr = doPolicyChecking (checkInputData, checkInfo);

    if (FAILED (hr))
    {
      // Policy checking failed (application must treat situation as licensing
      // agreement violation)
      return (hr);
    }

    if (kirValid == checkInfo->invalidReason)
    {
      return (S_OK);
    }


    // Here we are have to check if there are both active and reserve key
    // and active one is expired

    if (!checkInputData.reserveKeyInfo.serialNumber.empty () &&
        ((kirExpired == checkInfo->invalidReason) || (kirTrialPeriodIsOver == checkInfo->invalidReason)) )
    {

      hr = promoteReserveKey ();

      if (FAILED (hr))
      {
          return (hr);
      }

      return (checkActiveKey (checkInfo));

    }


  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}
//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkKeyFile 
    (const string_t&    fileName,
     check_info_t       *checkInfo,
     key_add_mode_t     addMode
    )
{

  if (!isInitialized ())
    return (LIC_E_NOT_INITIALIZED);

  if (!checkInfo)
    return (E_INVALIDARG);

  if (getCurrDate().empty()) return E_UNEXPECTED;

  HRESULT hr;

  try
  {

    check_input_data_t      checkInputData;

    checkInfo->keyFileName = fileName;
    extractFileName (&checkInfo->keyFileName);

    switch (addMode)
    {
        case kamAdd     : 
            checkInputData.controlDateType = ccdFloat; 
            checkInputData.ignoreInstalledKeys = false;
            break;
        case kamReplace : 
            checkInputData.controlDateType = ccdCurrent;
            checkInputData.ignoreInstalledKeys = true;
            break;
        default : return (E_INVALIDARG);
    }

    if (FAILED (hr = prepareChecking (fileName, &checkInputData)))
    {
      return (hr);
    }

    hr = doPolicyChecking (checkInputData, checkInfo);

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkKeyFiles (const string_t&    dir,
                                         check_info_list_t  *checkList,
                                         key_add_mode_t     addMode
                                        )
{

  if (!checkList)
    return (E_INVALIDARG);

  if (getCurrDate().empty()) return E_UNEXPECTED;
  
  HRESULT hr;

  try
  {

    file_list_t         fileList;
    check_info_t        checkInfo;

    checkList->clear ();

    if (FAILED (hr = enumFiles (dir, KeyFilePattern, &fileList)))
      return (hr);

    if (!fileList.size ())
      return (S_OK);

    for (size_t i = 0; i < fileList.size (); i++)
    {
      if (SUCCEEDED (checkKeyFile (fileList[i], &checkInfo, addMode)))
      {
        checkList->push_back (checkInfo);
      }

    }

    hr = !checkList->empty () ? S_OK : E_FAIL;

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getKeyInfo (const char  *fileImage,
                                      size_t       fileSize,
                                      key_info_t  *keyInfo
                                     )
{

  if (!isInitialized ())
  {
    return (LIC_E_NOT_INITIALIZED);
  }

  if (!fileImage || !fileSize || !keyInfo)
  {
    return (E_INVALIDARG);
  }

  if (getCurrDate().empty()) return E_UNEXPECTED;
  
  HRESULT hr;

  try
  {
    file_image_t  keyFileImage;

    keyFileImage.image.assign (fileImage, fileSize);

   hr = getKeyInfo (keyFileImage, keyInfo);

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkBlacklistFile  (const string_t& blacklistFileName)
{

    if (!isInitialized ()) 
    {
        return (LIC_E_NOT_INITIALIZED);
    }

	if (getCurrDate().empty()) return E_UNEXPECTED;
	
    HRESULT     _hr;

    try
    {

        key_info_t              _keyInfo;
        CBlacklist              _bl;
        CBlacklist::ResultCodeT _ret;

        _hr = getActiveKeyInfo (&_keyInfo);

        if (FAILED (_hr))
        {
          return (_hr);
        }

        setSysError (0);

        _ret = _bl.loadBlackList (blacklistFileName.c_str ());

        if (CBlacklist::rcOK == _ret)
        {
            _hr = _bl.findKey (_keyInfo.keySerNum) ? LIC_E_KEY_IS_BLACKLISTED : S_OK;
        }
        else
        {
            if (CBlacklist::rcFileNotFound == _ret || CBlacklist::rcReadError == _ret) 
            {
                storeSysError ();
            }
            _hr = mapBlacklistCode (_ret);
        }

    }
    catch (std::bad_alloc&)
    {
        _hr = E_OUTOFMEMORY;
    }

    return (_hr);
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkBlacklist (const char  *blacklistImage,
                                          size_t      imageSize
                                         )
{
  if (!isInitialized ())
  {
    return (LIC_E_NOT_INITIALIZED);
  }

  if (getCurrDate().empty()) return E_UNEXPECTED;

  if (!blacklistImage || !imageSize)
  {
    return (E_INVALIDARG);
  }

  HRESULT       hr;

  try
  {

    key_info_t keyInfo;

    hr = getActiveKeyInfo (&keyInfo);

    if (FAILED (hr))
    {
      return (hr);
    }

    file_image_t  fileImage;
    size_t        cleanSize;

    fileImage.image.assign (blacklistImage, imageSize);

    hr = checkBufferSign (fileImage.image, &cleanSize);

    if (FAILED (hr))
    {
      return (LIC_E_NOT_SIGNED);
    }

    fileImage.image.resize (cleanSize);

    CBlacklist  bl;
    CBlacklist::ResultCodeT _ret;

    _ret = bl.loadBlackList (fileImage.image.c_str (), (unsigned int)fileImage.image.size ());

    if (CBlacklist::rcOK == _ret)
    {
        hr = bl.findKey (keyInfo.keySerNum) ? LIC_E_KEY_IS_BLACKLISTED : S_OK;
    }
    else
    {
        if (CBlacklist::rcFileNotFound == _ret || CBlacklist::rcReadError == _ret) 
        {
            storeSysError ();
        }
        hr = mapBlacklistCode (_ret);
    }

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkKeyBlacklist   
    (const string_t&    keyFileName,
     const string_t&    blacklistFileName
    )
{

	if (!isInitialized ())
	{
		return (LIC_E_NOT_INITIALIZED);
	}

	if (getCurrDate().empty()) return E_UNEXPECTED;
	
    HRESULT _hr;

    try
    {
        file_image_t    _keyImage;
        file_image_t    _blacklistImage;

        _hr = loadFileImage (keyFileName, &_keyImage);

        if (FAILED (_hr))
        {
            return (_hr);
        }

        _hr = loadFileImage (blacklistFileName, &_blacklistImage);

        if (FAILED (_hr))
        {
            return (_hr);
        }
            
        _hr = checkKeyBlacklist (_keyImage.image.c_str (), 
                                 _keyImage.image.size (), 
                                 _blacklistImage.image.c_str (),
                                 _blacklistImage.image.size ()
                                );

    }
    catch (const std::bad_alloc&) 
    {
        _hr = E_OUTOFMEMORY;
    }

    return (_hr);

}

//-----------------------------------------------------------------------------
// Checks whether specified key can be registered as reserved.
// Returnes invalidity reason kirCantBeUsedAsReserved even if key can be
// installed as reserved but will be immediately promoted to be active one
// (i.e. active key is absent or presented but expired)

HRESULT CLicensingPolicy::checkKeyFileAsReserved (const string_t& keyFileName,
	check_info_t* checkInfo)
{

	if (!isInitialized ()) return (LIC_E_NOT_INITIALIZED);

	if (!checkInfo || keyFileName.empty ()) return (E_INVALIDARG);

	if (getCurrDate().empty()) return E_UNEXPECTED;
	
	HRESULT	hr;

	try
	{

		check_input_data_t      checkInputData;

		checkInfo->keyFileName = keyFileName;
		extractFileName (&checkInfo->keyFileName);

		checkInputData.controlDateType = ccdFloat; 
		checkInputData.ignoreInstalledKeys = false;

		if (FAILED (hr = prepareChecking (keyFileName, &checkInputData))) return (hr);

		hr = doPolicyChecking (checkInputData, checkInfo);

		if (SUCCEEDED (hr) && 
			( (checkInfo->invalidReason == kirValid) || (checkInfo->invalidReason == kirExpired) ))
		{
			// Key can be successfully added. It will be reserved key only
			// if non-expired active key is presented
			check_info_t	checkInfoTmp;
			
			checkActiveKey (&checkInfoTmp);

			if ( (checkInfoTmp.invalidReason == kirExpired) && 
				(!checkInputData.activeKeyInfo.serialNumber.empty ()) )
			{
				checkInfo->invalidReason = kirCantBeUsedAsReserved;
				checkInfo->funcLevel = flNoFeatures;
			}
		}

	}

	catch (const std::bad_alloc&)
	{
		hr = E_OUTOFMEMORY;
	}

	return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkKeyAsReserved (const string_t& keyFileName,
	size_t keyFileSize, const char* keyImage, check_info_t* checkInfo)
{
	if (!isInitialized ()) return (LIC_E_NOT_INITIALIZED);
	
	if (!checkInfo || !keyImage || !keyFileSize) return (E_INVALIDARG);

	if (getCurrDate().empty()) return E_UNEXPECTED;
	
	HRESULT	hr;

	try
	{
		
		check_input_data_t      checkInputData;
		file_image_t			keyFileImage;
		
		keyFileImage.image.assign (keyImage, keyFileSize);
		
		checkInfo->keyFileName = keyFileName;
		extractFileName (&checkInfo->keyFileName);

		checkInputData.controlDateType = ccdFloat; 
		checkInputData.ignoreInstalledKeys = false;

		if (FAILED (hr = prepareChecking (keyFileImage, &checkInputData))) return (hr);
		
		hr = doPolicyChecking (checkInputData, checkInfo);

		if (SUCCEEDED (hr) && (checkInfo->invalidReason == kirValid)) 
		{
			// Key can be successfully added. It will be reserved key only
			// if non-expired active key is presented
			
			if (checkInputData.activeKeyInfo.serialNumber.empty () ||
				checkInfo->keyExpirationDate < getCurrDate ()) 
			{
				checkInfo->invalidReason = kirCantBeUsedAsReserved;
			}
		}

	}
	
	catch (const std::bad_alloc&)
	{
		hr = E_OUTOFMEMORY;
	}
	
	return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkKeyBlacklist   
    (const char         *keyFileBody,
     size_t             keyFileSize,
     const char         *blacklistBody,
     size_t             blacklistSize
    )
{

    if (!keyFileBody || !keyFileSize || !blacklistBody || !blacklistSize) 
    {
        return (E_INVALIDARG);
    }

	if (getCurrDate().empty()) return E_UNEXPECTED;
	
    HRESULT _hr;

    try
    {
        key_info_t  _keyInfo;

        _hr = getKeyInfo (keyFileBody, keyFileSize, &_keyInfo);

        if (FAILED (_hr))
        {
            return (_hr);
        }

        CBlacklist              _bl;
        CBlacklist::ResultCodeT _ret;

        _ret = _bl.loadBlackList (blacklistBody, (unsigned int)blacklistSize);

        if (CBlacklist::rcOK != _ret) 
        {
            _hr = mapBlacklistCode (_ret);
        }
        else
        {
            _hr = _bl.findKey (_keyInfo.keySerNum) ? LIC_E_KEY_IS_BLACKLISTED : S_OK;
        }

    }
    catch (const std::bad_alloc&)
    {
        _hr = E_OUTOFMEMORY;
    }

    return (_hr);
    
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::addKey 
    (const string_t&    fileName,
     key_add_mode_t     addMode
    )
{

  if (!isInitialized ())
    return (LIC_E_NOT_INITIALIZED);

  if (getCurrDate().empty()) return E_UNEXPECTED;

  HRESULT      hr;

  try
  {

    file_image_t fileImage;

    if (FAILED (hr = loadFileImage (fileName, &fileImage)))
    {
      return (hr);
    }

    hr = addKey (fileName, fileImage, addMode);

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//! Registeres new license key
/*!
\param keyFileName   - [in] license key file name, informational only. This function will
                            never load this file from disk
\param keyFileImage  - [in] license key file exact image
\return S_OK                        - new key successfully registered
\return LIC_S_KEY_ALREADY_IN_USE    - this key already registered and in use at moment
\return E_FAIL                      - unknown error
\return LIC_E_CANT_REG_MORE_KEYS    - both active and reserve key are registered and in use
\note
\pre
\post
*/

HRESULT CLicensingPolicy::addKey 
    (const string_t&     keyFileName,
     const file_image_t& keyFileImage,
     key_add_mode_t      addMode
    )
{

  HRESULT                hr;
  check_info_t           checkInfo;

  if (getCurrDate().empty()) return E_UNEXPECTED;

  try
  {

    key_info_t             keyInfo;
    check_input_data_t     checkInputData;

    checkInfo.keyFileName = keyFileName;

    extractFileName (&checkInfo.keyFileName);

    switch (addMode)
    {
        case kamAdd     : 
            checkInputData.controlDateType = ccdFloat; 
            checkInputData.ignoreInstalledKeys = false;
            break;
        case kamReplace : 
            checkInputData.controlDateType = ccdCurrent;
            checkInputData.ignoreInstalledKeys = true;
            break;
        default : return (E_INVALIDARG);
    }

    if (FAILED (hr = checkKeyInMemory (keyFileImage, &checkInputData, &checkInfo)))
    {
      return (hr);
    }


    if (kirValid != checkInfo.invalidReason)
    {
      if (kirInvalidBases != checkInfo.invalidReason &&
          kirIllegalBaseUpdate != checkInfo.invalidReason &&
          kirExpired != checkInfo.invalidReason &&
          kirInvalidBlacklist != checkInfo.invalidReason &&
          kirInvalidUpdateDescription != checkInfo.invalidReason
         )
      {
        switch (checkInfo.invalidReason)
        {
          case kirBlackListed : return (LIC_E_KEY_IS_BLACKLISTED); break;
          case kirWrongProduct :
              if (m_appInfo.isRelease && ktBeta == checkInputData.keyInfo.keyType)
              {
                  return (LIC_E_CANT_REG_BETA_KEY_FOR_RELEASE);
              }
              else
                  if (!m_appInfo.isRelease && ktBeta != checkInputData.keyInfo.keyType)
                  {
                      return (LIC_E_CANT_REG_NON_BETA_KEY_FOR_BETA);
                  }
                  else
                  {
                      return (LIC_E_KEY_WRONG_FOR_PRODUCT);
                  }
              break;
          case kirNotSigned : return (LIC_E_DIGITAL_SIGN_CORRUPTED); break;
          case kirCorrupted : return (LIC_E_KEY_IS_CORRUPTED); break;
          case kirTrialAlredyUsed : return (LIC_E_TRIAL_ALREADY_USED); break;
          case kirKeyCreationDateInvalid : return (LIC_E_KEY_CREATION_DATE_INVALID); break;
          case kirTrialAlreadyInUse : return (LIC_E_CANNOT_INSTALL_SECOND_TRIAL); break;
          case kirInvalidUpdateDescription : return (LIC_E_UPDATE_DESC_CORRUPTED_OR_NOT_FOUND); break;
          case kirKeyInstallDateInvalid : return (LIC_E_KEY_INSTALL_DATE_INVALID); break;
		  case kirTrialPeriodIsOver: return (LIC_E_TRIAL_PERIOD_OVER); break;
		  case kirConflictsWithCommercial: return (LIC_E_KEY_CONFLICTS_WITH_COMMERCIAL); break;
          default : return (LIC_E_KEY_IS_INVALID); break;
        }
        
      }
    }

    //-------------------------
    // Check if this is active key

    if ((checkInputData.keyInfo.keySerNum == checkInputData.activeKeyInfo.serialNumber))
    {
      // Key is already in use
      return (LIC_S_KEY_ALREADY_IN_USE);
    }

    //-----------------------------

    if ( !checkInputData.activeKeyInfo.serialNumber.empty () && 
         !checkInputData.reserveKeyInfo.serialNumber.empty () &&
         kamAdd == addMode
       ) 
    {
        // Both active and reserve keys are installed. We cannot install more
        // than one reserve key.
        if (checkInputData.keyInfo.keySerNum == checkInputData.reserveKeyInfo.serialNumber)
        {
          return (LIC_S_KEY_ALREADY_IN_USE);
        }
        else
        {
          return (LIC_E_CANT_REG_MORE_KEYS);
        }
    }

    //-------------------------
    // Check if this we trying to register expired non-commercial key
    if (kirExpired == checkInfo.invalidReason &&
        ktCommercial != checkInputData.keyInfo.keyType
       )
    {
      // Cannot register expired key other than commercial
      return (LIC_E_CANNOT_REG_EXPIRED_NON_COMMERCIAL_KEY);
    }

    

    //---------------------------------------------------------------------------
    // Now registering new key key

    // Check if we`re going to register reserve key. Reserve key cannot be
    // expired

    if (!checkInputData.activeKeyInfo.serialNumber.empty () &&
        kirExpired == checkInfo.invalidReason &&
        kamAdd == addMode
       ) 
    {
      return (LIC_E_CANT_REG_EXPIRED_RESERVE_KEY);
    }

    // Check if we`re going to register non-valid reserve key
    if (!checkInputData.activeKeyInfo.serialNumber.empty () &&
        kirValid != checkInfo.invalidReason &&
        kamAdd == addMode
       )
    {
        return (LIC_E_CANNOT_REG_NONVALID_RESERVE_KEY);
    }

    installed_key_info_t ki;

    ki.installDate = checkInputData.installationDate;
    ki.expirationDate = checkInputData.expirationDate;
    ki.usedToBeActiveKey = false;

    if (!checkInfo.keyFileName.empty ())
    {
      ki.keyFileName = checkInfo.keyFileName;
    }
    else
    {
      ki.keyFileName = _T ("<unknown>");
    }

    ki.serialNumber = checkInputData.keyInfo.keySerNum;

    if (!checkInputData.activeKeyInfo.serialNumber.empty () && addMode == kamAdd)
    {
      // Register as reserve key
      if (FAILED (hr = m_secureData.storeReserveKey (ki, keyFileImage)))
        return (hr);
    }
    else
    {
      // Register as active key.
      // First, remove current active key if presented
      if (!checkInputData.activeKeyInfo.serialNumber.empty ())
          revokeKey (checkInputData.activeKeyInfo.serialNumber);
      ki.usedToBeActiveKey = true;
      if (FAILED (hr = m_secureData.storeActiveKey (ki, keyFileImage)))
        return (hr);
    }

    if (ktTrial == checkInfo.keyInfo.keyType )
    {
      if (FAILED (hr = m_secureData.storeTrialKeyInfo (ki)))
      {
        return (hr);
      }
      // Update trial limit date
	  if (checkInputData.trialLimitDate.empty ()) 
	  {
		  m_secureData.setTrialLimitDate (checkInfo.keyExpirationDate);
	  }
    }


    if (FAILED (hr = saveSecureData ()))
    {
      return (hr);
    }

  } // try

  catch (const std::bad_alloc&) {
    hr = E_OUTOFMEMORY;
  }

  return ((kirExpired == checkInfo.invalidReason) ? LIC_W_KEY_DOESNT_PROVIDE_SUPPORT : 
                                                    S_OK);

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//! Adds new key
/*!
\param fileName - [in] key file name. Used for info only; functions does not read any disk files
\param fileBody - [in] exact key file image
\param fileSize - [in] file size
\return S_OK              - key successfully added
\       E_FAIL            - unspecified error
\       E_INVALIDARG      - invalid arguments (fileBody == 0 or fileSize == 0)
\       LIC_E_NOT_SIGNED  - key file is not signed by Kaspersky Lab key
\       LIC_E_KEY_CORRUPT - key file is corrupted (but signed properly)
\note
\pre
\post
*/

HRESULT CLicensingPolicy::addKey 
    (const string_t&  fileName,
     const char      *fileImage,
     size_t           fileSize,
     key_add_mode_t   addMode
    )
{

  if (!isInitialized ())
    return (LIC_E_NOT_INITIALIZED);

  if (!fileImage || !fileSize)
  {
    return (E_INVALIDARG);
  }

  if (getCurrDate().empty()) return E_UNEXPECTED;

  file_image_t keyFileImage;


  keyFileImage.image.assign (fileImage, fileSize);

  return (addKey (fileName, keyFileImage, addMode));

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::addKeyBlacklist
    (const string_t&    fileName,
     const char         *keyFileBody,
     size_t             keyFileSize,
     const char         *blacklistBody,
     size_t             blacklistSize
    )
{
    if (!isInitialized ()) 
    {
        return (LIC_E_NOT_INITIALIZED);
    }

    if (!keyFileBody || !keyFileSize || !blacklistBody || !blacklistSize) 
    {
        return (E_INVALIDARG);
    }

	if (getCurrDate().empty()) return E_UNEXPECTED;
	
    HRESULT _hr;

    try
    {
        _hr = checkKeyBlacklist (keyFileBody, keyFileSize, blacklistBody, blacklistSize);

        if (FAILED (_hr))
        {
            return (_hr);
        }

        _hr = addKey (fileName, keyFileBody, keyFileSize);

    }
    catch (const std::bad_alloc&)
    {
        _hr = E_OUTOFMEMORY;
    }

    return (_hr);
    
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::addKeyBlacklist 
    (const string_t&    fileName,
     const string_t&    blacklistFileName
    )
{

    if (!isInitialized ()) 
    {
        return (LIC_E_NOT_INITIALIZED);
    }

	if (getCurrDate().empty()) return E_UNEXPECTED;
	
    HRESULT _hr;

    try
    {
        _hr = checkKeyBlacklist (fileName, blacklistFileName);

        if (FAILED (_hr))
        {
            return (_hr);
        }

        _hr = addKey (fileName);

    }
    catch (const std::bad_alloc&)
    {
        _hr = E_OUTOFMEMORY;
    }

    return (_hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::revokeActiveKey ()
{

  if (!isInitialized ())
    return (LIC_E_NOT_INITIALIZED);

  if (getCurrDate().empty()) return E_UNEXPECTED;

  HRESULT hr;

  try
  {

    installed_key_info_t keyInfo;

    if (FAILED (hr = m_secureData.getActiveKeyInfo (&keyInfo)) || (S_FALSE == hr))
    {
      return (hr);
    }

    if (FAILED (hr = m_secureData.clearActiveKey ()))
    {
      return (hr);
    };

    if (S_OK == m_secureData.getReserveKeyInfo (&keyInfo))
    {
      m_secureData.clearReserveKey ();
    }

    if (FAILED (saveSecureData ()))
    {
      loadSecureData ();
      hr = LIC_E_WRITE_SECURE_DATA;
    };

  } // try

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);


}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::revokeKey (const key_serial_number_t& serNum)
{

  if (!isInitialized ())
    return (LIC_E_NOT_INITIALIZED);

  if (getCurrDate().empty()) return E_UNEXPECTED;

  HRESULT hr;

  try
  {

    installed_key_info_t keyInfo;

    if (FAILED (hr = m_secureData.getActiveKeyInfo (&keyInfo)))
    {
      return (hr);
    }

    if (serNum == keyInfo.serialNumber)
    {
      return (revokeActiveKey ());
    }

    if (FAILED (hr = m_secureData.getReserveKeyInfo (&keyInfo)))
    {
      return (hr);
    }

    if (serNum == keyInfo.serialNumber)
    {
      m_secureData.clearReserveKey ();
      if (FAILED (saveSecureData ()))
      {
        loadSecureData ();
        hr = LIC_E_WRITE_SECURE_DATA;
      }
      else
      {
        hr = S_OK;
      }
    }
    else
    {
      hr = S_FALSE;
    }


  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::importLegacyKeysInfo(installed_keys_list_t *keysList)
{

  if (!isInitialized ())
  {
    return (LIC_E_NOT_INITIALIZED);
  }

  if (!keysList)
  {
    return (E_INVALIDARG);
  }

  return (E_NOTIMPL);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getActiveKeyInfo (key_info_t *keyInfo)
{

  if (!isInitialized ())
    return (LIC_E_NOT_INITIALIZED);

  if (!keyInfo)
    return (E_INVALIDARG);

  HRESULT                 hr;
  installed_key_info_t    instKeyInfo;
  file_image_t            fileImage;

  if (FAILED (hr = m_secureData.getActiveKey (&instKeyInfo, &fileImage)))
  {
    return (hr);
  }

  if (S_FALSE == hr)
  {
    return (S_FALSE);
  }

  if (FAILED (hr = getKeyInfo (fileImage, keyInfo)))
  {
    return (hr);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getReserveKeyInfo   (key_info_t *keyInfo)
{

  if (!isInitialized ())
  {
    return (LIC_E_NOT_INITIALIZED);
  }

  if (!keyInfo)
  {
    return (E_INVALIDARG);
  }

  HRESULT     hr;

  try
  {

    file_image_t          image;
    installed_key_info_t  resKeyInfo;

    if (FAILED (hr = m_secureData.getReserveKey (&resKeyInfo, &image)))
    {
      return (hr);
    }

    if (S_FALSE == hr)
    {
      return (hr);
    }

    if (FAILED (hr = getKeyInfo (image, keyInfo)))
    {
      return (hr);
    }

    hr = S_OK;

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getInstalledKeysInfo(check_info_list_t *checkInfoList)
{

  if (!isInitialized ())
  {
    return (LIC_E_NOT_INITIALIZED);
  }

  if (! checkInfoList)
  {
    return (E_INVALIDARG);
  }

  if (getCurrDate().empty()) return E_UNEXPECTED;

  HRESULT hr;

  try
  {

    checkInfoList->clear ();

    installed_key_info_t     installedKeyInfo;
    file_image_t             keyFileImage;
    check_input_data_t       checkInputData;
    check_info_t             checkInfo;


    // Workaround. Required in situation when both active and reserve keys
    // are presented. If active key is expired it must be autonatically revoked
    // and reserved key must become active one. CheckActiveKey handles this
    // logic. If this call will be avoid user will see two keys (including
    // expired active key) until next checkActiveKey call throuh normal
    // license checking
    checkActiveKey (&checkInfo);

    if (S_OK != (hr = m_secureData.getActiveKey (&installedKeyInfo, &keyFileImage)))
    {
      return (hr);
    }

    checkInputData.controlDateType = ccdCurrent;
    checkInputData.ignoreInstalledKeys = false;

    if (FAILED (hr = checkKeyInMemory (keyFileImage, &checkInputData, &checkInfo)))
    {
      return (hr);
    }

    checkInfo.keyFileName = installedKeyInfo.keyFileName;
    checkInfoList->push_back (checkInfo);

    if (S_OK != (hr = m_secureData.getReserveKey (&installedKeyInfo, &keyFileImage)))
    {
      return (hr);
    }

    checkInputData.controlDateType = ccdFloat;

    if (FAILED (hr = checkKeyInMemory (keyFileImage, &checkInputData, &checkInfo)))
    {
      return (hr);
    }

    checkInfo.keyFileName = installedKeyInfo.keyFileName;
    checkInfoList->push_back (checkInfo);

    hr = S_OK;

  } // try

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getInstalledKeysInfo(installed_key_info_list_t *keysInfo)
{

  if (!isInitialized ())
  {
    return (LIC_E_NOT_INITIALIZED);
  }

  if (!keysInfo)
  {
    return (E_INVALIDARG);
  }

  HRESULT hr;

  try
  {

    keysInfo->clear();

    installed_key_info_t     installedKeyInfo;
    file_image_t             keyFileImage;
    check_info_t             checkInfo;


    // Workaround. Required in situation when both active and reserve keys
    // are presented. If active key is expired it must be autonatically revoked
    // and reserved key must become active one. CheckActiveKey handles this
    // logic. If this call will be avoid user will see two keys (including
    // expired active key) until next checkActiveKey call throuh normal
    // license checking
    checkActiveKey (&checkInfo);

    if (S_OK != (hr = m_secureData.getActiveKey (&installedKeyInfo, &keyFileImage)))
    {
      return (hr);
    }

    keysInfo->push_back(installedKeyInfo);

    if (S_OK != (hr = m_secureData.getReserveKey (&installedKeyInfo, &keyFileImage)))
    {
      return (hr);
    }

    keysInfo->push_back(installedKeyInfo);

    hr = S_OK;

  } // try

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getKeyFileInfo (const string_t& fileName,
                                          key_info_t *keyInfo
                                         )
{

  if (!isInitialized ())
    return (LIC_E_NOT_INITIALIZED);

  HRESULT  hr;

  try
  {

    file_image_t  fileImage;

    if (FAILED (hr = loadFileImage (fileName, &fileImage)))
      return (hr);

    if (FAILED (hr = getKeyInfo (fileImage, keyInfo)))
    {
      return (hr);
    }

    hr = S_OK;

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getKeyFilesInfo (const string_t& dir,
                                           key_info_list_t *keysList
                                          )
{

  if (!keysList)
    return (E_INVALIDARG);

  HRESULT        hr;

  try
  {
  file_list_t    fileList;

  keysList->clear ();

  hr = enumFiles (dir, KeyFilePattern, &fileList);

  if (FAILED (hr))
    return (hr);

  if (!fileList.size ())
    return (S_OK);

  for (size_t i = 0; i < fileList.size (); i++)
  {
    if (SUCCEEDED (loadKeyFile (fileList[i])))
      keysList->push_back (m_keyInfo.getKeyInfo ());
  }

  hr = (fileList.size () == keysList->size () ? S_OK :
          (!keysList->empty () ? S_FALSE : E_FAIL));

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------
/*
HRESULT CLicensingPolicy::getAppInfo (const string_t& appInfoFileName,
                                      app_info_t *appInfo
                                     )
{

  if (!isInitialized ())
    return (E_FAIL);

  m_dataTree.setFileName (appInfoFileName);

  m_appInfo.setDataTree (m_dataTree);

  if (m_appInfo.loadAppInfo (appInfoFileName))
  {
    *appInfo = m_appInfo.getAppInfo ();
    return (S_OK);
  }
  else
    return (E_FAIL);
}
*/
//-----------------------------------------------------------------------------
/*
HRESULT CLicensingPolicy::checkKeyFile (const string_t&  fileName,
                                        check_info_t    *checkInfo
                                       )
{

  if (!isInitialized ())
    return (E_FAIL);

  if (!checkInfo)
    return (E_INVALIDARG);

  key_info_t keyInfo;
  CBlacklist blackList;
  date_t     lastUpdateDate;
  HRESULT    hr;

  if (FAILED (hr = prepareChecking (fileName,
                                    &keyInfo,
                                    &blackList,
                                    &lastUpdateDate
                                   )))
  {
    return (hr);
  }

  CAppInfo _appInfo (appInfo);

  return (doPolicyChecking (_appInfo.getAppInfo (),
                            keyInfo,
                            blackList,
                            lastUpdateDate,
                            checkInfo
                           ));

}
*/
//-----------------------------------------------------------------------------
/*
HRESULT CLicensingPolicy::getCheckedKeysList (const string_t&                   dir,
                                              const appinfo_struct::app_info_t& appInfoFileName,
                                              check_info_list_t                 *checkList
                                             )
{

  if (!checkList)
    return (E_INVALIDARG);

  HRESULT             hr;
  file_list_t         fileList;
  check_info_t        checkInfo;

  checkList->clear ();

  if (FAILED (hr = enumFiles (dir, KeyFilePattern, &fileList)))
    return (hr);

  if (!fileList.size ())
    return (S_OK);

  for (size_t i = 0; i < fileList.size () && SUCCEEDED (hr); i++)
  {
    if (SUCCEEDED (checkKeyFile (fileList[i], appInfoFileName, &checkInfo)))
    {
      std::pair <check_info_list_t::iterator, bool> p =
        checkList->insert (check_info_pair_t (fileList[i], checkInfo));
      if (!p.second)
        hr = E_OUTOFMEMORY;
    }

  }

  return (hr);

}
*/
//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkFileSign (const string_t& fileName)
{

  HRESULT      hr;

  try
  {

    file_image_t image;

    if (FAILED (hr = loadFileImage (fileName, &image)))
    {
      return (hr);
    }

    hr = checkBufferSign (image.image, 0);

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}


//-----------------------------------------------------------------------------

bool CLicensingPolicy::isInitialized ()
{
  return (m_initialized);
}


//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::enumFiles (const string_t& dir,
                                     const string_t& pattern,
                                     file_list_t* fileList
                                    )
{

#if defined (_WIN32)
  return (Win32EnumFiles (dir, pattern, fileList));
#else

#if defined (__unix__)
  return (unixEnumFiles (dir, pattern, fileList));
#else

#if defined (__MWERKS__)
  return (novellEnumFiles (dir, pattern, fileList));
#endif // Novell

#endif // unix

#endif // Windows

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::enumFiles (const string_list_t& dirList,
                                     const string_t& pattern,
                                     file_list_t* fileList
                                    )
{
  HRESULT hr = S_OK;

  for (unsigned int i = 0; i < dirList.size (); i++)
  {
    hr = enumFiles (dirList[i], pattern, fileList);
  }

  if (FAILED (hr) && !fileList->empty ())
    hr = S_FALSE;

  return (hr);
}

//-----------------------------------------------------------------------------

#if defined (_WIN32)

HRESULT CLicensingPolicy::Win32EnumFiles (const string_t& dir,
                                          const string_t& pattern,
                                          file_list_t* fileList
                                         )
{
  WIN32_FIND_DATA findData;
  HANDLE          hFind;
  HRESULT         hr;
  string_t        s;

  makeFullFileName (dir, pattern, &s);

  hFind = ::FindFirstFile (s.c_str (), &findData);

  if (INVALID_HANDLE_VALUE == hFind)
    return (ERROR_FILE_NOT_FOUND == GetLastError () ? S_FALSE : E_FAIL);

  hr = S_OK;

  while (S_OK == hr)
  {

    string_t fName (findData.cFileName);
    makeFullFileName (dir, fName, &s);
    fileList->push_back (s);

    if (!::FindNextFile (hFind, &findData))
      hr = ERROR_NO_MORE_FILES == GetLastError () ? S_FALSE : E_FAIL;
  }

  ::FindClose (hFind);

  return (FAILED (hr) ? hr : S_OK);

}

#endif // if defined (_WIN32)

//-----------------------------------------------------------------------------

#if defined (__unix__)

HRESULT CLicensingPolicy::unixEnumFiles (const string_t& dir,
                                         const string_t& pattern,
                                         file_list_t*    fileList
                                         )
{

  if (!fileList)
  {
    return (E_INVALIDARG);
  }


	DIR                     *hDir;
	struct dirent           *file;
  string_t                s;
  size_t                  dirLen;

	hDir = opendir(dir.c_str ());

  if (!hDir)
  {
    return (E_FAIL);
  }

  s.resize (dir.size () * 2);
  s = dir;

  if (!s.empty ())
  {
    if (PathDelimiter != s[s.size () - 1])
    {
      s += PathDelimiter;
    }
  }

  dirLen = s.size ();

  file = readdir (hDir);

  while (file)
  {

		struct stat f_stat;

    s.resize (dirLen);

    s += file->d_name;

    if (!stat (s.c_str (), &f_stat))
    {

      // Is it file (i.e. not directory)?

      if (! (f_stat.st_mode | S_IFDIR))
      {

        if (!fnmatch (pattern.c_str (), file->d_name, 0))
        {
          fileList->push_back (s);
        }

      }

    }

    file = readdir (hDir);

  }

  closedir (hDir);


  return (S_OK);

}

#endif // #if defined (__unix__)

//-----------------------------------------------------------------------------

#if defined (__MWERKS__)

HRESULT CLicensingPolicy::novellEnumFiles (const string_t& dir,
                                           const string_t& pattern,
                                           file_list_t*    fileList
                                          )
{

  if (!fileList)
  {
    return (E_INVALIDARG);
  }

  return (E_NOTIMPL);

}

#endif // #if defined (__MWERKS__)

//-----------------------------------------------------------------------------
/*
HRESULT CLicensingPolicy::loadAppInfoFile (const string_t& fileName,
                                           app_info_t *appInfo
                                          )
{

  HRESULT       hr;
  file_list_t   fileList;

  // TODO Add file lock here!!!

  if (FAILED (hr = checkFileSign (fileName)))
    return (hr);

  m_appInfo.setDataTree (m_dataTree);

  hr = m_appInfo.loadAppInfo (fileName) ? S_OK : E_FAIL;

  if (SUCCEEDED (hr))
    *appInfo = m_appInfo.getAppInfo ();

  return (hr);

}
*/
//-----------------------------------------------------------------------------
/*
HRESULT CLicensingPolicy::getActiveKeyInfo (key_info_t *keyInfo)
{

  if (!isInitialized ())
    return (E_FAIL);

  if (!keyInfo)
    return (E_INVALIDARG);

  string_t fullFileName;
  HRESULT  hr;

  if (m_activeKeyName.empty ())
    return (S_FALSE);

  if (FAILED (makeFullFileName (m_keysPath, m_activeKeyName, &fullFileName)))
    return (E_FAIL);

  if (FAILED (hr = loadKeyFile (fullFileName)))
    return (hr);

  *keyInfo = m_keyInfo.getKeyInfo ();

  return (S_OK);

}
*/
//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::makeFullFileName (const string_t& dir,
                                            const string_t& fileName,
                                            string_t *fullName
                                           )
{

  if (!fullName)
  {
    return (E_INVALIDARG);
  }

#if defined (__unix__)

  *fullName = dir;

  if (!fullName->empty () && ((*fullName)[fullName->size() - 1] != PathDelimiter) )
  {
    *fullName += PathDelimiter;
  }

  *fullName += fileName;

  return (S_OK);

#else

  _TCHAR pathBuf [_MAX_PATH];
  _TCHAR drive [_MAX_DRIVE];
  _TCHAR directory[_MAX_DIR];

  _tsplitpath (dir.c_str (), drive, directory, 0, 0);
  _tmakepath (pathBuf, 0, dir.c_str (), fileName.c_str (), 0);

  *fullName = pathBuf;

#endif

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::extractFileName (string_t *fullName)
{

  if (!fullName || fullName->empty ())
  {
    return (E_INVALIDARG);
  }

#if defined (__unix__)

  HRESULT hr;

  try
  {
    _TCHAR *buf = new _TCHAR[fullName->size () + 1];

    if (!buf)
    {
      return (E_OUTOFMEMORY);
    }

    try
    {
      _tcscpy (buf, fullName->c_str ());
      *fullName = basename (buf);

      hr = S_OK;
    }

    catch (...)
    {
      delete [] buf;
      throw;
    }

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

#else

  _TCHAR  name[_MAX_FNAME];
  _TCHAR  ext[_MAX_EXT];

  _tsplitpath (fullName->c_str (), 0, 0, name, ext);

  *fullName = name;
  *fullName += ext;

#endif

  return (S_OK);

}

//-----------------------------------------------------------------------------
//!
/*!
\param
\param
\return S_OK     - application license is valid and provides full functionality level
\return S_FALSE  - application license expired or invalid. checkInfo contains detailed info
\note
\pre
\post
*/

HRESULT CLicensingPolicy::doPolicyChecking 
    (check_input_data_t&    inputData,
     check_info_t           *checkInfo
    )
{


  if (!checkInfo)
	return (E_INVALIDARG);
	
  if (getCurrDate().empty()) return E_UNEXPECTED;

  checkInfo->invalidReason = kirUnknown;
  checkInfo->appLicenseExpDate = inputData.lastKeyExpDate < inputData.expirationDate ? 
                                    inputData.expirationDate : inputData.lastKeyExpDate;
  checkInfo->daysTillExpiration = 0;

  checkInfo->keyInfo = inputData.keyInfo;
  checkInfo->keyExpirationDate = inputData.expirationDate;
  checkInfo->componentFuncBitMask = 0;
  checkInfo->keyInstallationDate = inputData.installationDate;

  isKeyWrongForProduct (inputData, checkInfo);

  if (kirUnknown != checkInfo->invalidReason) 
  {
      return (S_FALSE);
  }

  isKeyExpired (inputData, checkInfo);

  checkInfo->appLicenseExpDate = inputData.lastKeyExpDate < inputData.expirationDate ? 
                                    inputData.expirationDate : inputData.lastKeyExpDate;

  date_t    currDate = getCurrDate ();

  if (kirExpired != checkInfo->invalidReason) 
  {
      if (checkInfo->appLicenseExpDate < currDate)
      {
        checkInfo->daysTillExpiration = 0;
      }
      else
      {
        checkInfo->daysTillExpiration = checkInfo->appLicenseExpDate.diff (currDate);
      }
  }
  else
  {
	  if (currDate < checkInfo->appLicenseExpDate)
	  {
		  checkInfo->keyExpirationDate = currDate;
		  checkInfo->appLicenseExpDate = currDate;
	  }
      checkInfo->daysTillExpiration = 0;
  }

  if ( (kirUnknown != checkInfo->invalidReason) &&
 			!(kirExpired == checkInfo->invalidReason && ktCommercial == inputData.keyInfo.keyType))
  {
 		if (kirExpired == checkInfo->invalidReason && ktCommercial != inputData.keyInfo.keyType)
 		{
 			checkInfo->funcLevel = flNoFeatures;
 		}
 
 		if (kirExpired == checkInfo->invalidReason && ktTrial == inputData.keyInfo.keyType && isTrialPeriodOver(inputData))
 		{
 			checkInfo->invalidReason = kirTrialPeriodIsOver;
 		}
 		return (S_FALSE);
  }

  ///////////////////////////////////////////////////////////////
  // 

  if (m_checkMode > cmKeyOnly)
  {

      HRESULT   hr;
      
      hr = loadUpdateConfig (&m_updateConfig);

      if (FAILED (hr))
      {
          checkInfo->funcLevel = flOnlyUpdates;
          checkInfo->invalidReason = kirInvalidUpdateDescription;
          return (S_FALSE);
      }

      inputData.lastUpdateDate = m_updateConfig.updateDate;

      updateLastKnownDate (inputData.lastUpdateDate);
      hr = saveSecureData ();

      if (FAILED (hr)) 
      {
          return (hr);
      }


      if (!m_basesPaths.empty ()) 
      {
          inputData.moreRecentBasePath = m_basesPaths[0];
      }

      if (!m_updateConfig.blackListFileName.empty ())
      {

        CBlacklist                  bl;
        CBlacklist::ResultCodeT     ret;
        file_image_t                blacklistImage;
        size_t                      cleanBufSize;
        string_t                    tmp;
        HRESULT                     hr;

        if (!m_basesPaths.empty ()) 
        {
            makeFullFileName (m_basesPaths[0], m_updateConfig.blackListFileName, &tmp);
        }
        else
        {
            tmp = m_updateConfig.blackListFileName;
        }

        hr = loadFileImage (tmp, &blacklistImage);

        if (FAILED (hr)) 
        {
            checkInfo->funcLevel = flOnlyUpdates;
            checkInfo->invalidReason = kirInvalidBlacklist;
            return (S_FALSE);
        }

        hr = checkBufferSign (blacklistImage.image, &cleanBufSize);

        if (FAILED (hr)) 
        {
            checkInfo->funcLevel = flOnlyUpdates;
            checkInfo->invalidReason = kirInvalidBlacklist;
            return (S_FALSE);
        }

		UpdateConfig::file_info_t* pBlackListInfo = 
			m_updateConfig.FindFileInfo (m_updateConfig.blackListFileName);

		if (pBlackListInfo == 0) 
		{
            checkInfo->funcLevel = flOnlyUpdates;
            checkInfo->invalidReason = kirInvalidBlacklist;
            return (S_FALSE);
		}

		hr = validateSign (tmp, pBlackListInfo->signature);
		
		if (FAILED (hr))
		{
            checkInfo->funcLevel = flOnlyUpdates;
            checkInfo->invalidReason = kirInvalidBlacklist;
            return (S_FALSE);
		}

        ret = bl.loadBlackList (blacklistImage.image.c_str (), (unsigned int)blacklistImage.image.size ());

        if (CBlacklist::rcOK != ret) 
        {
            checkInfo->funcLevel = flOnlyUpdates;
            checkInfo->invalidReason = kirInvalidBlacklist;
            return (S_FALSE);
        }

        const serial_number_list_t&   blacklist = bl.getSerNumList ();

        if (std::find (blacklist.begin (), blacklist.end (),
            inputData.keyInfo.keySerNum) != blacklist.end ())
        {
            checkInfo->invalidReason = kirBlackListed;
            checkInfo->funcLevel = flOnlyUpdates;
            return (S_FALSE);
        }

      } // if (!m_updateConfig.blackListFileName.empty ())

//      if (currDate < inputData.lastUpdateDate) 
/*
	  if (inputData.lastUpdateDate > currDate) 
	  {
		  if ( inputData.lastUpdateDate.diff (currDate) > DateTolerance) 
		  {
			  checkInfo->invalidReason = kirInvalidBases;
			  checkInfo->funcLevel = flOnlyUpdates;
		  }
	  }
*/
      if (checkInfo->appLicenseExpDate < inputData.lastUpdateDate) 
      {
		  if(inputData.lastUpdateDate.diff(checkInfo->appLicenseExpDate) > DateTolerance)
		  {
			  checkInfo->invalidReason = kirIllegalBaseUpdate;
			  checkInfo->funcLevel = flNoFeatures;
		  }
      }
  
  }

  ///////////////////////////////////////////////////////////////

  if (cmFullCheck == m_checkMode)
  {

    if (isBasesInvalid (inputData, checkInfo))
    {
      return (S_FALSE);
    }

  }

  if (kirExpired == checkInfo->invalidReason && ktCommercial != inputData.keyInfo.keyType)
  {
    checkInfo->invalidReason = kirExpired;
    checkInfo->funcLevel = flNoFeatures;
  }
  else
  {
    if (kirUnknown == checkInfo->invalidReason)
    {
      checkInfo->invalidReason = kirValid;
      checkInfo->funcLevel = flFullFunctionality;
    }
  }

  // deny commercial and non-commercial keys co-existence
  if (checkInfo->keyInfo.keyType != ktCommercial
      && !inputData.activeKeyInfo.serialNumber.empty()
      && inputData.activeKeyInfo.serialNumber != checkInfo->keyInfo.keySerNum /*infinite loop protection*/)
  {
    check_info_t actKeyInfo;
    if (SUCCEEDED(checkActiveKey(&actKeyInfo))
        && actKeyInfo.funcLevel >= flOnlyUpdates
        && actKeyInfo.keyInfo.keyType == ktCommercial)
    {
      checkInfo->invalidReason = kirConflictsWithCommercial;
      checkInfo->funcLevel = flNoFeatures;
    }
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::prepareChecking (const string_t&        keyFile,
                                           check_input_data_t    *checkInputData
                                          )
{


  if (!checkInputData)
    return (E_INVALIDARG);

  HRESULT      hr;
  file_image_t keyFileImage;

  if (FAILED (hr = loadFileImage (keyFile, &keyFileImage)))
  {
    return (hr);
  }

  return (prepareChecking (keyFileImage, checkInputData));

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//! Collects source data for checking license key
/*!
\param fileImage       - [in] key file image
\param checkInputData  - [out] source data to perform checking
\return
\note
\pre Secure license data from license storage must loaded before prepareChecking call
\post
\note
*/

HRESULT CLicensingPolicy::prepareChecking (const file_image_t&  fileImage,
                                           check_input_data_t  *checkInputData
                                          )
{

  if (!checkInputData)
  {
    return (E_INVALIDARG);
  }


  HRESULT hr = getKeyInfo (fileImage, &checkInputData->keyInfo);

  if (FAILED (hr))
  {
    return (hr);
  }

  date_t  currDate = getCurrDate();

  if (checkInputData->ignoreInstalledKeys) 
  {
      checkInputData->lastKeyExpDate = currDate;
  }
  else
  {
      checkInputData->lastKeyExpDate = getLastKeyExpDate ();
  }

  if (ccdCurrent == checkInputData->controlDateType)
  {
    checkInputData->controlDate = currDate;
  }
  else
  {
    if (currDate < checkInputData->lastKeyExpDate) 
    {
      checkInputData->controlDate = checkInputData->lastKeyExpDate;
    }
    else
    {
      checkInputData->controlDate = currDate;
    }
  }

  m_secureData.getActiveKeyInfo (&checkInputData->activeKeyInfo);
  m_secureData.getReserveKeyInfo (&checkInputData->reserveKeyInfo);
  m_secureData.getTrialKeyInfo (&checkInputData->trialKeyInfo);

  hr = m_secureData.getLastKnownDate (&checkInputData->lastKnownDate);

  if (FAILED (hr)) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  hr = m_secureData.getTrialLimitDate (&checkInputData->trialLimitDate);
  if (FAILED (hr)) 
  {
    return (LIC_E_SECURE_DATA_CORRUPT);
  }

  installed_key_info_t instKeyInfo;

  if (FAILED (hr = m_secureData.findKey (checkInputData->keyInfo.keySerNum, &instKeyInfo)))
  {
    return (hr);
  }

  bool  usePrevInstDates;

  if (S_OK == hr) 
  {
    // Key was installed earlier on this computer.
    // Was it used to be active key?
    if (instKeyInfo.usedToBeActiveKey)
    {
        // Yes, use dates from previous installation
        usePrevInstDates = true;
    }
    else
    {
        // No, ignore old information, assume current date is installation date 
        usePrevInstDates = false;
    }
  }
  else
  {
      // Key is new for this computer. It can be installed from current date or
      // from expiration date of last key using at moment.
      usePrevInstDates = false;
  }

  if (usePrevInstDates) 
  {
    checkInputData->installationDate = instKeyInfo.installDate;
    checkInputData->expirationDate = instKeyInfo.expirationDate;
  }
  else
  {
    if (checkInputData->lastKeyExpDate < currDate)
    {
      checkInputData->installationDate = currDate;
    }
    else
    {
//      checkInputData->installationDate = checkInputData->lastKeyExpDate;
		// wrong reserve key information in a call getInstalledKeysInfo() fixed
		checkInputData->installationDate =
			// check whether current key is an installed reserve key
			checkInputData->keyInfo.keySerNum == checkInputData->reserveKeyInfo.serialNumber ?
			checkInputData->reserveKeyInfo.installDate :
			checkInputData->lastKeyExpDate;
    }

    checkInputData->expirationDate = checkInputData->installationDate +
                                     static_cast<int> (checkInputData->keyInfo.keyLifeSpan);
//	if (checkInputData->keyInfo.keyExpDate < checkInputData->expirationDate)
    if ( (checkInputData->expirationDate > checkInputData->keyInfo.keyExpDate) &&
		 (checkInputData->expirationDate.diff (checkInputData->keyInfo.keyExpDate) > DateTolerance))
    {
      checkInputData->expirationDate = checkInputData->keyInfo.keyExpDate + DateTolerance;
    }
  }

  if (ktTrial == checkInputData->keyInfo.keyType)
  {
	  // adjust control date only when installation date is less than control date
      if ((checkInputData->installationDate < checkInputData->controlDate) || 
		  (checkInputData->installationDate == checkInputData->controlDate))
	  {
		  // Check if control date is earlier than "last known date" (time cheat).
		  // Set control date to "last known date" id so
		  if (checkInputData->controlDate < checkInputData->lastKnownDate)
		  {
			  checkInputData->controlDate = checkInputData->lastKnownDate;
		  }
	  }

      // Correct expiration date according to trial limit date
      if (checkInputData->trialLimitDate < checkInputData->expirationDate &&
          !checkInputData->trialLimitDate.empty ()
         )
      {
          checkInputData->expirationDate = checkInputData->trialLimitDate;
      }
  }

  checkInputData->lastUpdateDate.clear ();

  return (hr);

}

//-----------------------------------------------------------------------------
/*
HRESULT CLicensingPolicy::loadXML  (const string_t& fileName)
{

  HRESULT     hr;

  file_image_t xmlImage;

  if (FAILED (hr = loadFileImage (fileName, &xmlImage)))
  {
    return (hr);
  }

  size_t cleanBufSize; // Size of buffer data without digital sign

  if (FAILED (hr = checkBufferSign (xmlImage.image, &cleanBufSize)))
  {
    return (hr);
  }

  if (!m_xmlHelper->load (xmlImage.image.c_str (), cleanBufSize))
    return (E_FAIL);

  return (S_OK);

}
*/
//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getFileSize (const string_t& fileName,
                                       file_size_t *size
                                      )
{

  setSysError (0);

  int file = _topen (fileName.c_str (), O_RDONLY);

  if (!file)
  {
    storeSysError ();
    return (LIC_E_SYS_ERROR);
  }

  if (static_cast<file_size_t> (-1) == (*size = _lseek (file, 0, SEEK_END)))
  {
    _close (file);
    storeSysError ();
    return (LIC_E_SYS_ERROR);
  }

  _close (file);

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getFileSize (const int    fileHandle,
                                       file_size_t  *size
                                      )
{

  setSysError (0);

  file_size_t currPos = _lseek (fileHandle, 0, SEEK_CUR);

  if (static_cast<file_size_t> (-1) == currPos)
  {
    storeSysError ();
    return (LIC_E_SYS_ERROR);
  }


  setSysError (0);

  if (static_cast<file_size_t> (-1) == (*size = _lseek (fileHandle, 0, SEEK_END)))
  {
    storeSysError ();
    return (LIC_E_SYS_ERROR);
  }

  _lseek (fileHandle, currPos, SEEK_SET);

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::date_cast (const string_t& dateStr,
                                     date_t *date
                                    )
{

  if (!date)
    return (E_INVALIDARG);

  // !!! Suppose date presented in "ddmmyyyy" format.

  if (dateStr.size () < 8)
    return (E_FAIL);

  date->m_day = static_cast<word_t> (_ttoi (dateStr.substr (0, 2).c_str ()));
  date->m_month = static_cast<word_t> (_ttoi (dateStr.substr (2, 2).c_str ()));
  date->m_year = static_cast<word_t> (_ttoi (dateStr.substr (4, 4).c_str ()));

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::findMoreRecentBases  (string_t *,
                                                date_t   *
                                               )
{
  return (E_NOTIMPL);
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::loadKeyFile (const string_t& fileName)
{

  HRESULT hr = checkFileSign (fileName);

  if (FAILED (hr))
    return (hr);

  if (!m_keyInfo.loadKeyInfo (fileName))
    return (E_FAIL);

  return (S_OK);

}

//-----------------------------------------------------------------------------

bool CLicensingPolicy::isKeyWrongForProduct (const check_input_data_t&  inputData,
                                             check_info_t              *checkInfo
                                         )
{

  if (itApplicationInfo == m_appInfo.infoType)
  {
    // Its an application

    if (inputData.keyInfo.productId)
    {
      // Check if application is part of product the key is intended for

      product_list_t::iterator i = m_appInfo.prodInfoLst.find (inputData.keyInfo.productId);
      if (m_appInfo.prodInfoLst.end () == i)
      {
        // This key is generated for another product
        checkInfo->invalidReason = kirWrongProduct;
        checkInfo->funcLevel = flNoFeatures;
        return (true);
      }

      // Check product major version conformance

      if (!inputData.keyInfo.productVer.empty() &&
          (inputData.keyInfo.productVer != i->second.majorVersion))
      {
        // Product major versions declared in key and app info does not conform
        // We have to check app compatibility list for supported legacy products

        if (m_appInfo.appCompatList.end () == m_appInfo.appCompatList.find (inputData.keyInfo.appId))
        {
          checkInfo->invalidReason = kirWrongProduct;
          checkInfo->funcLevel = flNoFeatures;
          return (true);
        }

      }

    }


    //---------------------------------------------------------------------------
    // Check application id conformance

    if (m_appInfo.id != inputData.keyInfo.appId)
    {

      // Check application compatibility list for app id retrieved from key

      if (m_appInfo.appCompatList.end () == m_appInfo.appCompatList.find (inputData.keyInfo.appId))
      {
        checkInfo->invalidReason = kirWrongProduct;
        checkInfo->funcLevel = flNoFeatures;
        return (true);
      }

    }


    // Check if trying use beta key for release product or vice versa

    if ((ktBeta == inputData.keyInfo.keyType &&  m_appInfo.isRelease) ||
        (ktBeta != inputData.keyInfo.keyType && !m_appInfo.isRelease)
       )
    {
      checkInfo->invalidReason = kirWrongProduct;
      checkInfo->funcLevel = flNoFeatures;
      return (true);
    }

    // Check if another trial key is in use right now

    if (ktTrial == inputData.keyInfo.keyType && !inputData.ignoreInstalledKeys)
    {
      if (isTrialInUse (inputData) && 
          (inputData.keyInfo.keySerNum != inputData.trialKeyInfo.serialNumber)
         )
      {
          checkInfo->invalidReason = kirTrialAlreadyInUse;
          checkInfo->funcLevel = flNoFeatures;
          return (true);
      }
      else
      {
          return (false);
      }
    }

  } // if (aitApplicationInfo == m_appInfo.infoType)
  else
  {

    if (itComponentInfo == m_appInfo.infoType)
    {

      // Its a component

      if ( (m_appInfo.appList.end () == m_appInfo.appList.find (inputData.keyInfo.appId)) ||
           (m_appInfo.appCompatList.end () == m_appInfo.appCompatList.find (inputData.keyInfo.appId))
         )
      {
        checkInfo->invalidReason = kirWrongProduct;
        checkInfo->funcLevel = flNoFeatures;
        return (true);
      }

    }
    else
    {
      // Unknown type info, treat it as wrong key
      return (true);
    }

  }

  // Key is normal for product
  return (false);

}

//-----------------------------------------------------------------------------

bool CLicensingPolicy::isKeyBlacklisted (const check_input_data_t&  inputData,
                                         check_info_t              *checkInfo
                                        )
{

  if (std::find (inputData.blackList.begin (), inputData.blackList.end (),
      inputData.keyInfo.keySerNum) != inputData.blackList.end ())
  {
    checkInfo->invalidReason = kirBlackListed;
    checkInfo->funcLevel = flNoFeatures;
    return (true);
  }
  else
  {
    return (false);
  }

}

//-----------------------------------------------------------------------------

bool CLicensingPolicy::isTrialKeyUsed (const check_input_data_t&  inputData,
                                       check_info_t              *checkInfo
                                      )
{

  if (inputData.trialKeyInfo.serialNumber.empty ())
  {
    return (false);
  }

  if (inputData.trialKeyInfo.expirationDate < inputData.lastKnownDate)
  {
    checkInfo->invalidReason = kirTrialAlredyUsed;
    checkInfo->funcLevel = flNoFeatures;
    return (true);
  }
  else
  {
      return (false);
  }

/*

  if (! ((inputData.activeKeyInfo.serialNumber == inputData.keyInfo.keySerNum) ||
         (inputData.reserveKeyInfo.serialNumber == inputData.keyInfo.keySerNum) ||
         (inputData.trialKeyInfo.serialNumber == inputData.keyInfo.keySerNum)
        )
     )
  {
    checkInfo->invalidReason = kirTrialAlredyUsed;
    checkInfo->funcLevel = flNoFeatures;
    return (true);
  }
  else
  {
    return (false);
  }
*/


}


//-----------------------------------------------------------------------------

bool CLicensingPolicy::isBasesInvalid (const check_input_data_t&  inputData,
                                       check_info_t              *checkInfo
                                      )
{
	if ( (checkInfo->funcLevel == flNoFeatures) || (checkInfo->funcLevel == flOnlyUpdates) )
	{
		return (true);
	}

  file_image_t   image;
  HRESULT        hr;
  string_t       fullName;
  size_t         i;

  i = 0;
  hr = S_OK;

  while ( (S_OK == hr) && (i < m_updateConfig.filesList.size ()))
  {
    makeFullFileName (inputData.moreRecentBasePath,
                      m_updateConfig.filesList[i].name,
                      &fullName
                     );
    hr = validateSign (fullName, m_updateConfig.filesList[i].signature);
    i++;
  }

  if (S_OK != hr)
  {
      checkInfo->invalidReason = kirInvalidBases;
      checkInfo->funcLevel = flOnlyUpdates;
  }

  return (S_OK != hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::validateSign (const string_t&    fileName,
                                        const std::string& fileSign
                                       )
{

  file_image_t image;
  HRESULT      hr;
  std::string  sign;
  size_t       cleanSize;

  if (FAILED (hr = loadFileImage (fileName, &image)))
  {
    return (hr);
  }


  if (S_OK != (hr = checkBufferSign (image.image, &cleanSize)))
  {
    return (hr);
  }

  sign.assign (image.image.c_str () + cleanSize, image.image.size () - cleanSize);

  return (std::string::npos != sign.find (fileSign) ? S_OK : 
                                                      LIC_E_SIGN_DOESNT_CONFIRM_SIGNATURE);

}

//-----------------------------------------------------------------------------

bool CLicensingPolicy::isKeyExpired (const check_input_data_t&  inputData,
                                     check_info_t              *checkInfo
                                    )
{


  // Check key creation date.
  //	if (inputData.controlDate < inputData.keyInfo.keyCreationDate)
  if ( (inputData.keyInfo.keyCreationDate > inputData.controlDate) &&
	  (inputData.keyInfo.keyCreationDate.diff (inputData.controlDate) > DateTolerance))
  {
      // Key creation date cannot be less than control date
      checkInfo->invalidReason = kirKeyCreationDateInvalid;
      checkInfo->funcLevel = flNoFeatures;
      return (true);
  }

  // Check key installation date
  if (inputData.controlDate < inputData.installationDate)
  {
      checkInfo->invalidReason = kirKeyInstallDateInvalid;
      checkInfo->funcLevel = flNoFeatures;
      return (false);
  }

  if (inputData.expirationDate < inputData.controlDate)
  {
    checkInfo->invalidReason = kirExpired;
    checkInfo->funcLevel = flFunctionWithoutUpdates;
    checkInfo->daysTillExpiration = 0;
    return (false);
  }
  else
  {
    return (false);
  }

  // Check if key validity period is expired
  if ( (inputData.currentDate > inputData.keyInfo.keyExpDate) &&
	   (inputData.currentDate.diff (inputData.keyInfo.keyExpDate) > DateTolerance) )
  {
    // Return key validity period end date as key expiration date
    checkInfo->keyExpirationDate = inputData.keyInfo.keyExpDate;
    checkInfo->invalidReason = kirExpired;
    checkInfo->funcLevel = flFunctionWithoutUpdates;
    checkInfo->daysTillExpiration = 0;
    return (false);
  }

  // Check if key subscription period is expired
  if (!inputData.expirationDate.empty () &&
      inputData.expirationDate < inputData.currentDate
     )
  {
    // Return key expiration date evaluated during key registration
    checkInfo->keyExpirationDate = inputData.expirationDate;
    checkInfo->invalidReason = kirExpired;
    checkInfo->funcLevel = flFunctionWithoutUpdates;
    checkInfo->daysTillExpiration = 0;
    return (false);
  }

  // Key is not expired
  return (false);

}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//! checks buffer sign agains default Kaspersky Lab public key
/*!
\param buffer - [in] buffer to check
\return S_OK - sign check ok, LIC_E_NOT_SIGNED otherwise
*/

HRESULT CLicensingPolicy::checkBufferSign (const std::string&  buf,
                                           size_t              *cleanBufSize
                                          )
{

#if !defined (__MWERKS__)

  int s = 0;
  int r = 0;


  r = sign_check_buffer (const_cast<char*> (buf.c_str ()),
                         static_cast<int> (buf.size ()),
                         &s,
                         1,
                         0,
                         0,
                         0);


  if (SIGN_OK == r)
  {
    if (cleanBufSize)
    {
      *cleanBufSize = s;
    }
  }


  return (SIGN_OK == r ? S_OK : LIC_E_NOT_SIGNED);

#else

  try
  {

  int   s;
  sign_callback_param_t cbp = {&buf, 0};
  char *tmpBuf = new char [buf.size ()];


  if (!tmpBuf)
  {
    return (E_OUTOFMEMORY);
  }


  int r =  _sign_check_buffer_callback (signCallBack,
                                        &cbp,
                                        tmpBuf,
                                        buf.size (),
                                        &s,
                                        1,
                                        0,
                                        0,
                                        0
                                       );

  if (SIGN_OK == r)
  {
    if (cleanBufSize)
    {
      *cleanBufSize = static_cast<unsigned int> (s);
    }
  }

  delete [] tmpBuf;

  return (SIGN_OK == r ? S_OK : LIC_E_NOT_SIGNED);

  }

  catch (const std::bad_alloc&)
  {
    return (E_OUTOFMEMORY);
  }

#endif

}

//-----------------------------------------------------------------------------

date_t CLicensingPolicy::getCurrDate ()
{
  date_t		tmp;
  GetCurrentDate(tmp);
  return (tmp);
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkKeyInMemory 
    (const string_t&    keyFileName,
     const char         *fileImage,
     size_t             fileSize,
     check_info_t       *checkInfo,
     key_add_mode_t     addMode
    )
{

  if (!isInitialized ())
  {
    return (LIC_E_NOT_INITIALIZED);
  }

  if (!fileImage || !fileSize || !checkInfo)
  {
    return (E_INVALIDARG);
  }

  HRESULT             hr;

  try
  {

    file_image_t        keyFileImage;
    check_input_data_t  checkInputData;

    keyFileImage.image.assign (fileImage, fileSize);

    checkInfo->keyFileName = keyFileName;
    extractFileName (&checkInfo->keyFileName);

    checkInfo->keyFileName = keyFileName;

    extractFileName (&checkInfo->keyFileName);

    switch (addMode)
    {
        case kamAdd     : 
            checkInputData.controlDateType = ccdFloat; 
            checkInputData.ignoreInstalledKeys = false;
            break;
        case kamReplace : 
            checkInputData.controlDateType = ccdCurrent;
            checkInputData.ignoreInstalledKeys = true;
            break;
        default : return (E_INVALIDARG);
    }

    hr = checkKeyInMemory (keyFileImage, &checkInputData, checkInfo);

  }

  catch (const std::bad_alloc&)
  {
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::checkKeyInMemory 
    (const file_image_t&  fileImage,
     check_input_data_t  *checkInputData,
     check_info_t        *checkInfo
   )
{

  if (!checkInfo || !checkInputData)
  {
    return (E_INVALIDARG);
  }

  HRESULT prepare_hr;

  checkInfo->appLicenseExpDate.clear ();
  checkInfo->keyExpirationDate.clear ();
  checkInfo->daysTillExpiration = 0;
  checkInfo->funcLevel = flUnknown;
  checkInfo->componentFuncBitMask = 0;

  if (FAILED (prepare_hr = prepareChecking (fileImage, checkInputData)))
  {

    if (LIC_E_NOT_SIGNED == prepare_hr)
    {
      checkInfo->invalidReason = kirNotSigned;
      checkInfo->funcLevel = flNoFeatures;
      return (S_FALSE);
    }

    if ( (LIC_E_CANNOT_FIND_BASES != prepare_hr) &&
         (LIC_E_INVALID_BASES != prepare_hr)
       )
    {
      return (prepare_hr);
    }
  }

  HRESULT hr = doPolicyChecking (*checkInputData, checkInfo);

  if (FAILED (hr))
  {
    return (hr);
  }

  if (kirValid == checkInfo->invalidReason)
  {
    if (FAILED (prepare_hr))
    {
      checkInfo->invalidReason = kirInvalidBases;
      checkInfo->funcLevel = flNoFeatures;
      return (S_FALSE);
    }
    else
    {
      return (hr);
    }
  }
  else
  {
    return (hr);
  }
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::loadKeyFromMemory (const char *,
                                             size_t
                                            )
{
  return (E_NOTIMPL);
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::loadFileImage (const string_t&  fileName,
                                         file_image_t    *fileImage,
                                         file_size_t      maxAllowedSize
                                        )
{

  if (!fileImage)
  {
    return (E_INVALIDARG);
  }

  HRESULT     hr;
  file_size_t size;


  CFILE file;

  setSysError (0);

  file = _topen (fileName.c_str (), O_RDONLY | O_BINARY);

  if (-1 == file.get ())
  {
    storeSysError ();
    return (LIC_E_SYS_ERROR);
  }

  if (FAILED (hr = getFileSize (file.get (), &size)))
  {
    return (hr);
  }


  // Prevent processing abnormally big files (key file cannot be of page file size ...)
  if (maxAllowedSize)
  {
    if (size > maxAllowedSize)
    {
      return (E_FAIL);
    }
  }

  char *buf = 0;

  try
  {
    size_t sz = static_cast<size_t> (size);

    buf = new char [sz];

    // MSVC does not throw std::bad_alloc
    if (!buf)
    {
      return (E_OUTOFMEMORY);
    }

    setSysError (0);

    if (-1 == _read (file.get (), buf, (unsigned int)sz))
    {
      delete [] buf;
      storeSysError ();
      return (LIC_E_SYS_ERROR);
    }

    fileImage->image.assign (buf, sz);

    delete [] buf;

  }

  catch (std::bad_alloc&)
  {
    delete [] buf;
    hr = E_OUTOFMEMORY;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getKeyInfo (const file_image_t&  fileImage,
                                      key_info_t          *keyInfo
                                     )
{

  if (!keyInfo)
  {
    return (E_INVALIDARG);
  }

  HRESULT hr;

  if (FAILED (hr = checkBufferSign (fileImage.image, 0)))
  {
    return (hr);
  }

  if (!m_keyInfo.loadKeyInfo (fileImage.image.c_str (), fileImage.image.size ()))
  {
    return (LIC_E_KEY_CORRUPT);
  }


  *keyInfo = m_keyInfo.getKeyInfo ();

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::isActiveKey (const key_serial_number_t& serNum)
{

  installed_key_info_t keyInfo;
  HRESULT              hr;

  if (FAILED (hr = m_secureData.getActiveKeyInfo (&keyInfo)))
  {
    return (hr);
  }

  return (serNum == keyInfo.serialNumber ? S_OK : S_FALSE);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::isReserveKey (const key_serial_number_t& serNum)
{

  installed_key_info_t keyInfo;
  HRESULT              hr;

  if (FAILED (hr = m_secureData.getReserveKeyInfo (&keyInfo)))
  {
    return (hr);
  }

  return (serNum == keyInfo.serialNumber ? S_OK : S_FALSE);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::saveSecureData ()
{
  std::string buf;
  HRESULT     hr;

  if (FAILED (hr = m_secureData.serialize (&buf)))
  {
    return (hr);
  }

  if (buf.size () != static_cast<unsigned int> (m_lic_storage->writeSecureData (buf.c_str (), (unsigned int)buf.size (), m_context)))
  {
    return (LIC_E_WRITE_SECURE_DATA);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::loadSecureData ()
{
  char        *buf = 0;
  unsigned int size = 0;
  HRESULT      hr;

  if (m_lic_storage->readSecureData (&buf, &size, m_context) < 0)
  {
    return (LIC_E_READ_SECURE_DATA);
  }

  if (!size)
  {
    return (S_OK);
  }

  try
  {

    hr = SUCCEEDED (m_secureData.deserialize (buf, size)) ? S_OK :
                                                            LIC_E_SECURE_DATA_CORRUPT;
    free (buf); // !!! Must be deallocated by free, not delete
  }

  catch (const std::bad_alloc&)
  {
    free (buf);
    hr = E_OUTOFMEMORY;
  }

  catch (...)
  {
    free (buf);
    throw;
  }

  return (hr);

}

//-----------------------------------------------------------------------------

int CLicensingPolicy::dateDiff (const date_t& date1,
                                const date_t& date2
                               )
{

  time_t     d1, d2;
  struct tm  t = {0};

  t.tm_mday = static_cast<int> (date1.m_day);
  t.tm_mon = static_cast<int> (date1.m_month + 1);
  t.tm_year = static_cast<int> (date1.m_year - 1900);

  d1 = mktime (&t);

  t.tm_mday = static_cast<int> (date2.m_day);
  t.tm_mon = static_cast<int> (date2.m_month + 1);
  t.tm_year = static_cast<int> (date2.m_year - 1900);

  d2 = mktime (&t);

  return (int)((d1 - d2) / (60 * 60 * 24));

}

//-----------------------------------------------------------------------------

void CLicensingPolicy::convertAppInfo (const _app_info_t& appInfo)
{

  m_appInfo.creationDate.m_day = appInfo.appInfoDate.day;
  m_appInfo.creationDate.m_month = appInfo.appInfoDate.month;
  m_appInfo.creationDate.m_year = appInfo.appInfoDate.year;

  m_appInfo.infoType          = static_cast<LicensingPolicy::app_info_type_t> (static_cast<int> (appInfo.infoType));
  m_appInfo.id                = appInfo.id;

#if defined (_WIN32) && defined (UNICODE)
  m_appInfo.name              = char2Unicode (appInfo.name);
  m_appInfo.ver               = char2Unicode (appInfo.ver);
#else
  m_appInfo.name              = appInfo.name;
  m_appInfo.ver               = appInfo.ver;
#endif

  m_appInfo.isRelease         = 0 != appInfo.isRelease;

  size_t i;

  for (i = 0; i < appInfo.compList.componentsNumber; i++)
  {
    m_appInfo.componentList.insert (component_list_pair_t (
      appInfo.compList.componentList[i].componentId,
      appInfo.compList.componentList[i].funcLevel));
  }


  for (i = 0; i < appInfo.appList.appsNumber; i++)
  {
    m_appInfo.appList.insert (app_component_pair_t (
      appInfo.appList.list[i].appId,
      appInfo.appList.list[i].funcLevelMask));
  }

  for (i = 0; i < appInfo.productList.productsNumber; i++)
  {

    product_info_t pi;

    pi.productId = appInfo.productList.productsList[i].prodId;
    pi.pubKeyId  = appInfo.productList.productsList[i].publicKeyId;

#if defined (WIN32) && defined (UNICODE)
    pi.productName = char2Unicode (appInfo.productList.productsList[i].prodName);
    pi.majorVersion = char2Unicode (appInfo.productList.productsList[i].prodMajorVer);
#else
    pi.productName = appInfo.productList.productsList[i].prodName;
    pi.majorVersion = appInfo.productList.productsList[i].prodMajorVer;
#endif

    m_appInfo.prodInfoLst.insert (product_list_pair_t(pi.productId, pi));
  }


  for (i = 0; i < appInfo.appCompatList.appsNumber; i++)
  {
#if defined (_WIN32) && defined (UNICODE)
    m_appInfo.appCompatList.insert (app_compat_pair_t (
      appInfo.appCompatList.appsList[i].appId,
      char2Unicode (appInfo.appCompatList.appsList[i].appName)
      ));
#else
    m_appInfo.appCompatList.insert (app_compat_pair_t (
      appInfo.appCompatList.appsList[i].appId,
      std::string (appInfo.appCompatList.appsList[i].appName)
      ));
#endif
  }

}

//-----------------------------------------------------------------------------

date_t CLicensingPolicy::getLastKeyExpDate ()
{

  installed_key_info_t  ki;
  HRESULT               hr;
  date_t                activeKeyExpDate;
  date_t                reserveKeyExpDate;
  date_t                currDate;
  file_image_t          fi;
  key_info_t            keyInfo;
  date_t                trialLimitDate;
  date_t                lastKnownDate;

  activeKeyExpDate.clear ();
  reserveKeyExpDate.clear ();
  currDate = getCurrDate ();

  hr = m_secureData.getTrialLimitDate (&trialLimitDate);
  if (FAILED (hr))
  {
    return (currDate);
  }

  hr = m_secureData.getLastKnownDate (&lastKnownDate);
  if (FAILED (hr))
  {
    return (currDate);
  }

  hr = m_secureData.getReserveKeyInfo (&ki);

  if (FAILED (hr))
  {
    return (currDate);
  }

  if (S_OK == hr)
  {
    reserveKeyExpDate = ki.expirationDate;

    hr = m_secureData.getReserveKey (&ki, &fi);
    if (FAILED (hr)) 
    {
      return (currDate);
    }

    // Check reserve key type. Is it trial?


    hr = getKeyInfo (fi, &keyInfo);
    if (FAILED (hr)) 
    {
      return (currDate);
    }

    if (ktTrial != keyInfo.keyType)
    {
      return (reserveKeyExpDate);
    }

    // Reserve key is trial - need to check is it treated as expired due to
    // time cheat

    if (lastKnownDate < trialLimitDate)
    {
      return (reserveKeyExpDate);
    }

    // We have expired trial key installed as reserve one. Last key expiration date
    // must be determined by active key.
  }


  hr = m_secureData.getActiveKeyInfo (&ki);

  if (FAILED (hr))
  {
    return (currDate);
  }

  if (S_OK == hr)
  {
    activeKeyExpDate = ki.expirationDate;

    hr = m_secureData.getActiveKey (&ki, &fi);
    if (FAILED (hr)) 
    {
        return (currDate);
    }

    hr = getKeyInfo (fi, &keyInfo);
    if (FAILED (hr)) 
    {
      return (currDate);
    }

    if (ktTrial != keyInfo.keyType)
    {
      return (activeKeyExpDate);
    }

    // Reserve key is trial - need to check is it treated as expired due to
    // time cheat

    if (lastKnownDate < trialLimitDate)
    {
      return (activeKeyExpDate);
    }

    return (currDate);

  }

  return (currDate);

}

//-----------------------------------------------------------------------------

#if defined (WIN32)

std::wstring CLicensingPolicy::char2Unicode (const char *s)
{

  std::wstring ws (L"");

  if (!s)
    return (ws);

  int      length;

  length = (int)strlen (s);

  if (!length)
  {
    return (ws);
  }

  try
  {

    // MS VC++ 6.0 new operator does not throw std::bad_alloc, but
    // all might be...
    wchar_t *buf = new wchar_t[length + 1];

    if (!buf)
    {
      return (ws);
    }

    if (MultiByteToWideChar (1251,
                             0,
                             s,
                             length,
                             buf,
                             length
                            ) == length)
    {
      buf[length] = 0;
      ws = buf;
    }

    delete [] buf;

  }
  catch (std::bad_alloc&)
  {
  }

  return (ws);

}

#endif // #if defined (WIN32)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//! Returns memory buffer to check digital sign
/*!
\param params                - [in]   application specific parameter (see sign_check_buffer_callback)
\param buffer                - [out]  buffer to fill with data
\param buff_len              - [in]   buffer size
\return
\note params is expected to be pointer to sign_callback_param_t
\pre
\post
*/
#if defined (__MWERKS__)

int SIGNAPI CLicensingPolicy::signCallBack (void* params,
                                            void* buffer,
                                            int   buff_len
                                           )
{

  sign_callback_param_t *p = reinterpret_cast<sign_callback_param_t*> (params);

  if (p->cb < p->buf->size ())
  {

    unsigned int bytesToWrite = (p->buf->size () - p->cb);

    bytesToWrite = (bytesToWrite < static_cast<unsigned int> (buff_len)) ?
                    bytesToWrite :
                    static_cast<unsigned int> (buff_len);


    memcpy (buffer, p->buf->c_str () + p->cb, bytesToWrite);

    p->cb += bytesToWrite;

    return (static_cast<int> (bytesToWrite));

  }
  else
  {
    return (0);
  }


}

#endif

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::loadUpdateConfig (UpdateConfig::update_config_t *updateConfig)
{


    HRESULT         _hr;
    file_list_t     _kavSetFiles;
    file_image_t    _xmlImage;
    size_t          _cleanImageSize;
    string_t        _tmp;

    if (!m_basesPaths.empty ()) 
    {
        _tmp = m_basesPaths[0];
    }

    _hr = enumFiles (_tmp, KAVsetFilePattern, &_kavSetFiles);

    if (FAILED (_hr))
    {
        storeSysError ();
        return (LIC_E_SYS_ERROR);
    }

    if (_kavSetFiles.empty ())
    {
        return (LIC_E_UPDATE_DESC_CORRUPTED_OR_NOT_FOUND);
    }

    _hr = loadFileImage (_kavSetFiles[0], &_xmlImage);

    if (FAILED (_hr))
    {
        return (LIC_E_UPDATE_DESC_CORRUPTED_OR_NOT_FOUND);
    }

    _hr = checkBufferSign (_xmlImage.image, &_cleanImageSize);

    if (FAILED (_hr))
    {
        return (_hr);
    }

    _xmlImage.image.resize (_cleanImageSize);

	if (m_getUpdateConfigProc)
		_hr = m_getUpdateConfigProc (_xmlImage, updateConfig);
	else
		_hr = S_OK;// ??

    if (FAILED (_hr))
    {
        return (_hr);
    }

    return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::promoteReserveKey ()
{

    HRESULT hr;


    if (SUCCEEDED (hr = m_secureData.moveReserveKeyToActive ()))
    {
        if (FAILED (hr = saveSecureData ()))
        {
            // If we cannot save secure data to storage we must try to renew it
            // memory since it was changed by moveReserveKeyToActive
            loadSecureData ();
            return (hr);
        }
    }

    return (S_OK);

}

//-----------------------------------------------------------------------------

void CLicensingPolicy::storeSysError ()
{
#if defined (_WIN32)
    m_sysError = GetLastError ();
#else

    #if defined (__unix__)
    m_sysError = errno;
    #else
    m_sysError = 0;
    #endif
#endif
}

//-----------------------------------------------------------------------------

sys_err_code_t CLicensingPolicy::getStoredError ()
{
    return (m_sysError);
}

//-----------------------------------------------------------------------------

void CLicensingPolicy::setSysError (sys_err_code_t errCode)
{
    m_sysError = errCode;
#if defined (_WIN32)
    SetLastError (errCode);
#endif
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::getKeyInvalidReasonStr 
    (key_invalid_reason_t invalidReason,
     string_t             *message  
    )
{

#if defined (_WIN32)
    UINT            _msg;
#else
    const _TCHAR*   _msg;
#endif

    switch (invalidReason)
    {
        case kirValid               : _msg = IDS_KeyValid; break;
        case kirExpired             : _msg = IDS_KeyIsExpired; break;
        case kirCorrupted           : _msg = IDS_KeyIsCorrupted; break;
        case kirNotSigned           : _msg = IDS_KeyIsNotSigned; break;
        case kirWrongProduct        : _msg = IDS_KeyIsWrongForProduct; break;
        case kirNotFound            : _msg = IDS_KeyNotFound; break;
        case kirBlackListed         : _msg = IDS_KeyIsBlacklisted; break;
        case kirTrialAlredyUsed     : _msg = IDS_TrialIsAlreadyUsed; break;
        case kirIllegalBaseUpdate   : _msg = IDS_IllegalBaseUpdate; break;
        case kirInvalidBases        : _msg = IDS_BasesCorrupted; break;
        case kirInvalidBlacklist    : _msg = IDS_InvalidBlacklist; break;
        case kirInvalidUpdateDescription : _msg = IDS_InvalidUpdateDescription; break;
		case kirCantBeUsedAsReserved : _msg = IDS_CantBeUsedAsReserved; break;
        default                     : _msg = IDS_KeyIsCorrupted; break;
    }

#if defined (_WIN32)

    DWORD   _ret;
    DWORD   _bufSize = 0;
    _TCHAR  *_buf;


    _ret = loadResString (GetModuleHandle (0),
                          _msg,
                          LANGIDFROMLCID (GetThreadLocale ()),
                          0,
                          &_bufSize
                         );
    if (ERROR_MORE_DATA == _ret) 
    {
        _buf = new _TCHAR[_bufSize + 1];
        if (!_buf) 
        {
            return (E_OUTOFMEMORY);
        }
        _ret = loadResString (GetModuleHandle (0),
                              _msg,
                              LANGIDFROMLCID (GetThreadLocale ()),
                              _buf,
                              &_bufSize
                             );
        if (ERROR_SUCCESS == _ret) 
        {
            *message = _buf;
        }
        delete [] _buf;

        return (ERROR_SUCCESS == _ret ? S_OK : E_FAIL);
    }
    else
    {
        return (E_FAIL);
    }

#else

    *message = _msg;
    return (S_OK);

#endif

}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::mapBlacklistCode (CBlacklist::ResultCodeT code)
{

    HRESULT _hr;

    switch (code)
    {
        case CBlacklist::rcFileNotFound :
        case CBlacklist::rcReadError :
            _hr = LIC_E_SYS_ERROR;
            break;
        case CBlacklist::rcOutOfMemory      : _hr = E_OUTOFMEMORY; break;
        case CBlacklist::rcCorruptedFile    : _hr = LIC_E_BLACKLIST_CORRUPTED; break;
        case CBlacklist::rcInvalidParam     : _hr = E_INVALIDARG; break;
        default                             : _hr = LIC_E_DATA_CORRUPTED; break;
    }

    return (_hr);
}

//-----------------------------------------------------------------------------

void CLicensingPolicy::updateLastKnownDate (const date_t& d)
{

    date_t  tmp;

	// if no trial keys were installed do not update last known date
	if (!m_secureData.hasTrialKeyInvo())
		return;

    if (FAILED(m_secureData.getLastKnownDate(&tmp)))
        return;

    if (tmp < d) 
    {
        m_secureData.setLastKnownDate (d);
    }
}

//-----------------------------------------------------------------------------

bool CLicensingPolicy::isTrialInUse (const check_input_data_t& inputData)
{
    return ( (inputData.activeKeyInfo.serialNumber == inputData.trialKeyInfo.serialNumber &&
              !inputData.activeKeyInfo.serialNumber.empty ()) ||
             (inputData.reserveKeyInfo.serialNumber == inputData.trialKeyInfo.serialNumber &&
              !inputData.reserveKeyInfo.serialNumber.empty ())
           );
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::isKeyInUse (const key_serial_number_t& serNum)
{

    HRESULT hr;

    hr = isActiveKey (serNum);

    if (FAILED (hr))
    {
        return (hr);
    }
    else
    {
        if (S_OK == hr)
        {
            return (S_OK);
        }
    }

    hr = isReserveKey (serNum);

    return (hr);
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::clearTrialLimitDate()
{
	HRESULT hr = m_secureData.setTrialLimitDate (date_t());

	if (FAILED(hr))
		return hr;

	if (FAILED(saveSecureData()))
	{
		loadSecureData();
		hr = LIC_E_WRITE_SECURE_DATA;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------

bool CLicensingPolicy::isTrialPeriodOver(const check_input_data_t& inputData)
{
	if (inputData.trialLimitDate.empty()) 
		return false;
	if (inputData.trialLimitDate < inputData.lastKnownDate)
		return true;

	return false;
}

//-----------------------------------------------------------------------------

HRESULT CLicensingPolicy::isTrialPeriodOver(bool& isOver)
{
	HRESULT hr;
    updateLastKnownDate (getCurrDate ());
    if (FAILED(hr = saveSecureData ()))
		return hr;

	check_input_data_t checkInputData;
	if (FAILED(hr = m_secureData.getLastKnownDate (&checkInputData.lastKnownDate)))
		return hr;
	if (FAILED(hr = m_secureData.getTrialLimitDate (&checkInputData.trialLimitDate)))
		return hr;

	isOver = isTrialPeriodOver(checkInputData);

	return S_OK;
}

//-----------------------------------------------------------------------------
