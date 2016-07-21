#if !defined (__LICENSING_INTF_H__)
#define __LICENSING_INTF_H__

#if defined (_MSC_VER)
  // disable warning 4786
  // 'identifier' : identifier was truncated to 'number' characters in the 
  // debug information
  #pragma warning (disable : 4786)
  #pragma once
#endif

//-----------------------------------------------------------------------------

#include "err_defs.h"
#include <string>
#include <vector>
#include <algorithm>

#include "datatree.h"
#include "keyfile.h"
#include "lic_storage_intf.h"
//#include <xmlhelper.h>

#include "appinfo_struct.h"

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif


//-----------------------------------------------------------------------------

namespace LicensingPolicy {

//using helpers::IXMLHelper;

//-----------------------------------------------------------------------------

class ILicensingPolicy
{
public:

  //! Possible key invalidity reason
  enum key_invalid_reason_t
  {
    kirUnknown     = 0,
    kirValid,
    kirExpired,
    kirCorrupted,
    kirNotSigned,
    kirWrongProduct,
    kirNotFound,
    kirBlackListed,
    kirTrialAlredyUsed,
    kirIllegalBaseUpdate,
    kirInvalidBases,
    kirKeyCreationDateInvalid,
    kirTrialAlreadyInUse,
    kirInvalidBlacklist,
    kirInvalidUpdateDescription,
	kirCantBeUsedAsReserved,
	kirKeyInstallDateInvalid,
	kirTrialPeriodIsOver,
	kirConflictsWithCommercial
  };

  //! Checking modes
  typedef enum
  {
    cmUnknown            = 0,
    cmKeyOnly            = 1,                  //!< Check key against system date only
    cmKeyAndBlacklist    = 2,                  //!< Check key and blacklist
    cmFullCheck          = 3,                  //!< Do bases update checking using kavset.xml
                                               //!< and key serial number checking against black.lst 
  } checking_mode_t;
  
  typedef enum
  {
    kamUnknown = 0,
    kamAdd,
    kamReplace
  } key_add_mode_t;

  //! Licensing key checking results
  typedef struct
  {
    string_t               keyFileName;         //!< Key file name (without path)
    key_info_t             keyInfo;             //!< Info from key file
    key_invalid_reason_t   invalidReason;       //!< Key invalidity reason (or kirValid)
    date_t                 keyExpirationDate;   //!< Key expiration date
    date_t                 appLicenseExpDate;   //!< App license expiration date
    dword_t                daysTillExpiration;  //!< Days left till app  expiration day
    functionality_level_t  funcLevel;           //!< Application functionality level provided by this key
    unsigned int           componentFuncBitMask;//!< Component functionality level bit mask
	date_t                 keyInstallationDate;	//!< Date when key is planned to be installed
  } check_info_t;

  typedef struct 
  {
    key_info_t      keyInfo;
    date_t          keyExpDate; 
  } installed_key_t;

  typedef std::vector<key_info_t>            key_info_list_t;
  typedef std::vector<installed_key_t>       installed_keys_list_t;
  typedef string_list_t                      file_list_t;
  typedef unsigned int                       product_id_t;

  typedef std::vector<check_info_t>          check_info_list_t;

  class CCheckInfoFindKeyPred : public std::unary_function<check_info_t, 
                                                            bool
                                                           >
  {
    public:
      CCheckInfoFindKeyPred (const key_serial_number_t& sn) : m_sn (sn) {}
      bool operator () (const check_info_t& ci)
      {
        return (ci.keyInfo.keySerNum == m_sn);
      }
    private:
      key_serial_number_t   m_sn;
  };
  
  //! Library initialization method. All methods will return E_FAIL if init hasn`t been called yet
  virtual HRESULT       init                (ILicenseStorage      *licenseStorage,
                                             const _app_info_t&    appInfo,
                                             void                 *context,
                                             const string_list_t&  basesPaths,
                                             checking_mode_t       checkMode = cmFullCheck,
											 bool				   bIgnoreStorageIntegrityErrors = false
                                            ) = 0;

  virtual HRESULT       setAppInfo          (const _app_info_t& appInfo,
                                             void               *context
                                            ) = 0;
  virtual HRESULT       setBasesPaths       (const string_list_t& basesPaths) = 0;


  // ---------------- Licensing checking routines ---------------------------

  //! Checks application licensing mode against active key
  virtual HRESULT       checkActiveKey      (check_info_t *pCheckInfo) = 0;

  
  //! Returns licensing checking info for pointed key file
  virtual HRESULT       checkKeyFile        (const string_t&    keyFileName,
                                             check_info_t       *checkInfo,
                                             key_add_mode_t     addMode = kamReplace
                                            ) = 0;

  //! Returns licensing checking info for key file preloaded from disk
  virtual HRESULT       checkKeyInMemory    (const string_t&       keyFileName,
                                             const char           *fileBody,
                                             size_t                fileSize,
                                             check_info_t         *checkInfo,
                                             key_add_mode_t        addMode = kamReplace
                                            ) = 0;

    //! Returns licensing checking info for key files found in pointed location
  virtual HRESULT       checkKeyFiles       (const string_t&    dir,
                                             check_info_list_t  *checkList,
                                             key_add_mode_t     addMode = kamReplace
                                            ) = 0;


  //! Checks if active key serial number found in provided black list
  virtual HRESULT       checkBlacklistFile  (const string_t& blacklistFileName) = 0;

  //! Checks if active key serial number found in provided black list
  virtual HRESULT       checkBlacklist      (const char *blackList,
                                             size_t      size
                                            ) = 0;
  //! Check arbitrary license key against arbitrary black list
  virtual HRESULT       checkKeyBlacklist   (const string_t&    keyFileName,
                                             const string_t&    blacklistFileName
                                            ) = 0;
  virtual HRESULT       checkKeyBlacklist   (const char         *keyFileBody,
                                             size_t             keyFileSize,
                                             const char         *blacklistBody,
                                             size_t             blacklistSize
                                            ) = 0;

  // Checks whether specified key can be registered as reserved.
  virtual HRESULT		checkKeyFileAsReserved	(const string_t& keyFileName,
											check_info_t* checkInfo) = 0;
  virtual HRESULT		checkKeyAsReserved (const string_t& keyFileName,
	  size_t keyFileSize, const char* keyImage, check_info_t* checkInfo) = 0;

  // ---------------- Licensing managment routines ---------------------------

  //! New key registration
  virtual HRESULT       addKey              (const string_t&    fileName,
                                             key_add_mode_t     addMode = kamReplace
                                            ) = 0;
  virtual HRESULT       addKey              (const string_t&    fileName,
                                             const char         *fileBody,
                                             size_t             fileSize,
                                             key_add_mode_t     addMode = kamReplace
                                            ) = 0;
  //! New key registration with explicit black list checking  
  virtual HRESULT       addKeyBlacklist     (const string_t&    keyFileName,
                                             const char         *keyFileBody,
                                             size_t             keyFileSize,
                                             const char         *blacklistBody,
                                             size_t             blacklistSize
                                            ) = 0;
  virtual HRESULT       addKeyBlacklist     (const string_t&    fileName,
                                             const string_t&    blacklistFileName
                                            ) = 0;

  //! Active key deletion
  virtual HRESULT       revokeActiveKey     () = 0;
  //! Registered key deletion using key serial number
  virtual HRESULT       revokeKey           (const key_serial_number_t& serNum) = 0;

  //! Reads information about old format keys
  virtual HRESULT       importLegacyKeysInfo(installed_keys_list_t *keysList) = 0;


  // ---------------- Licensing managment helper routines ---------------------

  //! Returns active licensing key info
  virtual HRESULT       getActiveKeyInfo    (key_info_t *keyInfo) = 0;

  //! Returns reserve licensing key info
  virtual HRESULT       getReserveKeyInfo   (key_info_t *keyInfo) = 0;

  //! Returns info about installed keys alone with licensing chcking info
  virtual HRESULT       getInstalledKeysInfo(check_info_list_t *checkInfo) = 0;
  
  //! Returns licensing key info
  virtual HRESULT       getKeyFileInfo      (const string_t&  fileName,
                                             key_info_t      *keyInfo
                                            ) = 0;
  
  //! Returns info from license keys found in pointed dir
  virtual HRESULT       getKeyFilesInfo     (const string_t&  dir,
                                             key_info_list_t *keysList
                                             ) = 0;

  //! Returns license key info presented as preloaded key file
  virtual HRESULT       getKeyInfo          (const char  *fileBody,
                                             size_t       fileSize,
                                             key_info_t  *keyInfo
                                            ) = 0;


  virtual HRESULT       checkFileSign       (const string_t& fileName) = 0;

  virtual HRESULT       validateSign        (const string_t&    fileName,
                                             const std::string& fileSign
                                            ) = 0;

  //! Returns stored system error. Meaningfull only when LIC_E_SYS_ERROR returned
  virtual sys_err_code_t getStoredError     () = 0;

  //! Returns string description for key invalidity code
  virtual HRESULT       getKeyInvalidReasonStr (key_invalid_reason_t invalidReason,
                                                string_t             *message
                                               ) = 0;

}; // class CLicensingIntf


//-----------------------------------------------------------------------------

}; // namespace Licensing

#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#endif
