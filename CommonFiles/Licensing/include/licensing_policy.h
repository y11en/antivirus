#if !defined (__LICENSING_POLICY_H__)
#define __LICENSING_POLICY_H__

#if defined (_MSC_VER)
  // disable warning 4786
  // 'identifier' : identifier was truncated to 'number' characters in the 
  // debug information
  #pragma warning (disable : 4786)
  #pragma once
#endif

//-----------------------------------------------------------------------------

#include "licensing_intf.h"
#include "secure_data.h"
#include "appinfo.h"
#include "blacklist.h"
#include "updateconfig.h"

#if defined (__MWERKS__)
  #include <sign.h>
#endif

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif


namespace LicensingPolicy {

using namespace SecureData;

//-----------------------------------------------------------------------------

class CLicensingPolicy : public ILicensingPolicy
{

public:

  typedef std::vector<installed_key_info_t> installed_key_info_list_t;
  
                        CLicensingPolicy    ();
  virtual               ~CLicensingPolicy   ();

  virtual HRESULT       init                (ILicenseStorage *,
                                             const _app_info_t&   appInfo,
                                             void                 *context,
                                             const string_list_t& basesPaths,
                                             checking_mode_t   checkMode = cmFullCheck,
											 bool			   bIgnoreStorageIntegrityErrors = false
                                            );

  virtual HRESULT       setAppInfo          (const _app_info_t& appInfo,
                                             void               *context
                                            );
  virtual HRESULT       setBasesPaths       (const string_list_t& basesPaths);

  // ---------------- Licensing checking routines ---------------------------

  //! Checks application licensing mode against active key
  virtual HRESULT       checkActiveKey      (check_info_t *pCheckInfo);

  
  //! Returns licensing checking info for pointed key file
  virtual HRESULT       checkKeyFile        (const string_t&    keyFileName,
                                             check_info_t       *checkInfo,
                                             key_add_mode_t     addMode = kamReplace
                                            );
  //! Returns licensing checking info for key file preloaded from disk
  virtual HRESULT       checkKeyInMemory    (const string_t&       keyFileName,
                                             const char           *fileImage,
                                             size_t                fileSize,
                                             check_info_t         *checkInfo,
                                             key_add_mode_t       addMode = kamReplace
                                            );

    //! Returns licensing checking info for key files found in pointed location
  virtual HRESULT       checkKeyFiles       (const string_t&    dir,
                                             check_info_list_t  *checkList,
                                             key_add_mode_t     addMode = kamReplace
                                            );

  //! Returns license key info presented as preloaded key file
  virtual HRESULT       getKeyInfo          (const char  *fileImage,
                                             size_t       fileSize,
                                             key_info_t  *keyInfo
                                            );

  //! Checks if active key serial number found in provided black list
  virtual HRESULT       checkBlacklistFile  (const string_t& blacklistFileName);

  //! Checks if active key serial number found in provided black list
  virtual HRESULT       checkBlacklist      (const char *blackList,
                                             size_t      size
                                            );
  //! Check arbitrary license key against arbitrary black list
  virtual HRESULT       checkKeyBlacklist   (const string_t&    keyFileName,
                                             const string_t&    blacklistFileName
                                            );
  virtual HRESULT       checkKeyBlacklist   (const char         *keyFileBody,
                                             size_t             keyFileSize,
                                             const char         *blacklistBody,
                                             size_t             blacklistSize
                                            );
  
  // Checks whether specified key can be registered as reserved.
  virtual HRESULT		checkKeyFileAsReserved	(const string_t& keyFileName,
											check_info_t* checkInfo);

  virtual HRESULT		checkKeyAsReserved (const string_t& keyFileName,
	  size_t keyFileSize, const char* keyImage, check_info_t* checkInfo);

  // ---------------- Licensing managment routines ---------------------------

  //! New key registration
  virtual HRESULT       addKey              (const string_t&    fileName,
                                             key_add_mode_t     addMode  = kamReplace
                                            );
  virtual HRESULT       addKey              (const string_t&    fileName,
                                             const char         *fileBody,
                                             size_t             fileSize,
                                             key_add_mode_t     addMode = kamReplace
                                            );
  //! New key registration with explicit black list checking  
  virtual HRESULT       addKeyBlacklist     (const string_t&    fileName,
                                             const char         *keyFileBody,
                                             size_t             keyFileSize,
                                             const char         *blacklistBody,
                                             size_t             blacklistSize
                                            );
  virtual HRESULT       addKeyBlacklist     (const string_t&    fileName,
                                             const string_t&    blacklistFileName
                                            );
  //! Active key deletion
  virtual HRESULT       revokeActiveKey     ();
  //! Registered key deletion using key serial number
  virtual HRESULT       revokeKey           (const key_serial_number_t& serNum);

  //! Reads information about old format keys
  virtual HRESULT       importLegacyKeysInfo(installed_keys_list_t *keysList);

  // ---------------- Licensing managment helper routines ---------------------

  //! Returns active licensing key info
  virtual HRESULT       getActiveKeyInfo    (key_info_t *keyInfo);

  //! Returns reserve licensing key info
  virtual HRESULT       getReserveKeyInfo   (key_info_t *keyInfo);

  //! Returns info about installed keys alone with licensing chcking info
  virtual HRESULT       getInstalledKeysInfo(check_info_list_t *checkInfo);
  
  //! Returns installed keys extra info from licensing storage
  HRESULT				getInstalledKeysInfo(installed_key_info_list_t *keysInfo);
  
  //! Returns licensing key info
  virtual HRESULT       getKeyFileInfo      (const string_t&  fileName,
                                             key_info_t      *keyInfo
                                            );
  
  //! Returns info from license keys found in pointed dir
  virtual HRESULT       getKeyFilesInfo     (const string_t&  dir,
                                             key_info_list_t *keysList
                                            );


  virtual HRESULT       checkFileSign       (const string_t& fileName);

  virtual HRESULT       validateSign        (const string_t&    fileName,
                                             const std::string& fileSign
                                            );

  virtual sys_err_code_t getStoredError      ();

  virtual HRESULT       getKeyInvalidReasonStr (key_invalid_reason_t invalidReason,
                                                string_t             *message
                                               );
          HRESULT       mapBlacklistCode    (CBlacklist::ResultCodeT);
		  
		  HRESULT		clearTrialLimitDate();
		  
		  HRESULT		isTrialPeriodOver(bool& isOver);

private:

  typedef enum 
  {
    ccdUnknown  = 0,
    ccdCurrent,             //!< Control date is current date
    ccdFloat                //!< Control date is defined depends on installed keys 
  } check_control_date_t;

  typedef struct
  {
    check_control_date_t controlDateType;
    bool                 ignoreInstalledKeys;
    date_t               installationDate;
    date_t               expirationDate;
    date_t               controlDate;
    key_info_t           keyInfo;
    serial_number_list_t blackList;
    date_t               currentDate;
    date_t               lastUpdateDate;
    string_t             moreRecentBasePath;
    installed_key_info_t activeKeyInfo;
    installed_key_info_t reserveKeyInfo;
    installed_key_info_t trialKeyInfo;
    date_t               lastKeyExpDate;
    date_t               lastKnownDate;
    date_t               trialLimitDate;
  } check_input_data_t;

                        CLicensingPolicy    (const CLicensingPolicy&);

          CLicensingPolicy&
                        operator =          (const CLicensingPolicy&);


          bool          isInitialized       ();

          HRESULT       enumFiles           (const string_t& dir, 
                                             const string_t& pattern,
                                             file_list_t* fileList
                                            );
          HRESULT       enumFiles           (const string_list_t& dirList,
                                             const string_t&      pattern,
                                             file_list_t*         fileList                                            
                                            );
#if defined (WIN32)
          HRESULT       Win32EnumFiles      (const string_t& dir, 
                                             const string_t& pattern,
                                             file_list_t*    fileList
                                            );
          std::wstring  char2Unicode        (const char *s);
#else

  #if defined (__unix__)
          HRESULT       unixEnumFiles       (const string_t& dir, 
                                             const string_t& pattern,
                                             file_list_t*    fileList
                                            );
  #else
    #if defined (__MWERKS__)
          HRESULT       novellEnumFiles     (const string_t& dir, 
                                             const string_t& pattern,
                                             file_list_t*    fileList
                                            );
    #else
      #error Unknown platform - do know how to build files enumerator
    #endif
  #endif
                                   
#endif
          bool          isTrialKeyUsed      (const check_input_data_t&  inputData,
                                             check_info_t              *checkInfo
                                            );
          bool          isBasesInvalid      (const check_input_data_t&  inputData,
                                             check_info_t              *checkInfo
                                            );
/*
          HRESULT       loadAppInfoFile     (const string_t& fileName,
                                             app_info_t *appInfo
                                            );
*/
          HRESULT       makeFullFileName    (const string_t&  dir,
                                             const string_t&  fileName,
                                             string_t        *fullName
                                            );
          HRESULT       extractFileName     (string_t *fullName);
          HRESULT       doPolicyChecking    (check_input_data_t&    inputData,
                                             check_info_t           *checkInfo
                                            );
          HRESULT       prepareChecking     (const string_t&             keyFile,
                                             check_input_data_t         *checkInputData
                                            );
          HRESULT       prepareChecking     (const file_image_t&         fileImage,
                                             check_input_data_t         *checkInputData
                                            );
/*          HRESULT       loadXML             (const string_t& fileName);
          HRESULT       getXMLAttrValue     (const string_t& fileName,
                                             const string_t& attrPath,
                                             const string_t& attrName,
                                             string_t *value
                                            );
*/
          HRESULT       loadUpdateConfig    (UpdateConfig::update_config_t *);
          HRESULT       getFileSize         (const string_t& fileName,
                                             file_size_t *size
                                            );
          HRESULT       getFileSize         (const int    fileHandle,
                                             file_size_t  *size
                                            );
          HRESULT       date_cast           (const string_t& dateStr,
                                             date_t *date
                                            );
          HRESULT       findMoreRecentBases (string_t *path, 
                                             date_t *date
                                            );
          HRESULT       loadKeyFile         (const string_t& fileName);
          HRESULT       loadKeyFromMemory   (const char *keyBuf,
                                             size_t      bufSize
                                            );

          bool          isKeyWrongForProduct(const check_input_data_t&  inputData,
                                             check_info_t              *checkInfo 
                                            );
          bool          isKeyBlacklisted    (const check_input_data_t&  inputData,
                                             check_info_t              *checkInfo 
                                            );
          bool          isKeyExpired        (const check_input_data_t&  inputData,
                                             check_info_t              *checkInfo 
                                            );

          HRESULT       checkBufferSign     (const std::string&   buf,
                                             size_t              *cleanBufSize 
                                            );
          date_t        getCurrDate         () ;
          HRESULT       loadFileImage       (const string_t&  fileName,
                                             file_image_t    *fileImage,
                                             file_size_t      maxAllowedSize = 0
                                            );
          HRESULT       getKeyInfo          (const file_image_t&  fileImage,
                                             key_info_t          *keyInfo
                                            );
          HRESULT       checkKeyInMemory    (const file_image_t&  fileImage,
                                             check_input_data_t  *checkInputData,
                                             check_info_t        *checkInfo
                                            );
          HRESULT       isActiveKey         (const key_serial_number_t& serNum);
          HRESULT       isReserveKey        (const key_serial_number_t& serNum);
          HRESULT       saveSecureData      ();
          HRESULT       loadSecureData      ();
          HRESULT       addKey              (const string_t&     keyFileName,
                                             const file_image_t& keyFileImage,
                                             key_add_mode_t      addMode
                                            );
          int           dateDiff            (const date_t& date1,
                                             const date_t& date2
                                            );
          void          convertAppInfo      (const _app_info_t& appInfo);
          date_t        getLastKeyExpDate   ();
          HRESULT       promoteReserveKey   ();
          void          storeSysError       ();
          void          setSysError         (sys_err_code_t);

          void          updateLastKnownDate (const date_t& d);
          bool          isTrialInUse        (const check_input_data_t&  inputData);
          HRESULT       isKeyInUse          (const key_serial_number_t& serNum);
		  static bool	isTrialPeriodOver(const check_input_data_t& inputData);

#if defined (__MWERKS__)
  static  int SIGNAPI   signCallBack        (void* params, void* buffer, int buff_len);
#endif

#if defined (_WIN32)
#endif
  

  CDataTree                         m_dataTree;
//  CAppInfo                   m_appInfo;
  app_info_t                        m_appInfo;
  void                              *m_context;
  CKeyInfo                          m_keyInfo;
  bool                              m_initialized;
  ILicenseStorage                   *m_lic_storage;
  UpdateConfig::getUpdateConfig_t   m_getUpdateConfigProc;
  UpdateConfig::update_config_t     m_updateConfig;
  CSecureData                       m_secureData;
  string_t                          m_keysPath;
  string_list_t                     m_basesPaths;
  string_t                          m_productBasePath;
  string_t                          m_activeKeyName;
  checking_mode_t                   m_checkMode;
  installed_key_info_t              m_activeKeyInfo;
  installed_key_info_t              m_reserveKeyInfo;
  sys_err_code_t                    m_sysError;
  
};

//-----------------------------------------------------------------------------

}; // namespace LicensingPolicy


#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#endif
