#if !defined (__LICENSING_POLICY_C_INTF_H__)
#define __LICENSING_POLICY_C_INTF_H__

#if defined (_MSC_VER)
  // disable warning 4786
  // 'identifier' : identifier was truncated to 'number' characters in the 
  // debug information
  #pragma warning (disable : 4786)
  #pragma once
#endif

#include "lic_storage_intf.h"
#include "err_defs.h"
#include "appinfo_struct.h"

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

#if defined (__cplusplus)
extern "C"
{
#endif

/*---------------------------------------------------------------------------*/

typedef unsigned int _dword_t;
typedef struct  
{
  unsigned int day;
  unsigned int month;
  unsigned int year;
} _date_t_;

typedef struct  
{
  _dword_t memberId;
  _dword_t appId;
  _dword_t serNum;
} _key_serial_number_t;

typedef struct 
{
  _dword_t     objId;
  _dword_t     licenseNumber;
} _licensed_object_t;

typedef struct 
{
  _dword_t            size;
  _licensed_object_t *vector;
} _licensed_obj_list_t;

typedef struct
{
  _dword_t   componentId;
  _dword_t   funcLevel;
} _component_level_t;

typedef struct
{
  _dword_t            size;
  _component_level_t *vector;
} _components_level_list_t;

//! Possible key invalidity reason
typedef enum 
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
  kirIllegalBaseUpdate
} _key_invalid_reason_t;

typedef enum
{
  ktUnknown = 0,
  ktBeta,
  ktTrial,
  ktTest,
  ktOEM,
  ktCommercial
} _key_type_t;

typedef enum 
{
  flUnknown = 0,
  flNoFeatures,
  flNoUpdates,
  flFullFunctionality
} _functionality_level_t;


//! Checking modes
typedef enum
{
  cmUnknown            = 0,
  cmFullCheck          = 1,                  //!< Do bases update checking using kavset.xml
                                             //!< and key serial number checking against black.lst 
  cmKeyOnly            = 2                   //!< Check key against system date only
} _checking_modes_t;

typedef struct
{
  _dword_t                    keyVer;            //!< key file structure version      
  _date_t                     keyCreationDate;   //!< key file creation date  
  _key_serial_number_t        keySerNum;         //!< key file serial number 
  _key_type_t                 keyType;           //!< key type (commercial, trial, ...) 
  _dword_t                    keyLifeSpan;       //!< key validity period since installation (days)
  _date_t                     keyExpDate;        //!< key limit validity period (days)
  _dword_t                    licenseCount;      //!< license number (key ver 1-3)
  _TCHAR*                     productName;       //!< product name
  _dword_t                    appId;             //!< application name
  _dword_t                    productId;         //!< product id
  _TCHAR*                     productVer;        //!< major product version
  _licensed_obj_list_t        licensedObjLst;    //!< licensed objects with license count
  _TCHAR*                     licenseInfo;       //!< descriptional license info
  _TCHAR*                     supportInfo;       //!< support company properties
  _dword_t                    marketSector;      //!< market sector 
  _dword_t                    nodesNumber;       //!< network nodes number 
  _dword_t                    emailsNumber;      //!< email address number
  _dword_t                    dailyTraffic;      //!< licensed daily traffic value
  _dword_t                    totalTraffic;      //!< licensed total traffic
  _dword_t                    connectionsNumber; //!< licensed simultaneous connections number
  _dword_t                    checkThreadsNumber;//!< licensed simultaneous antivirus checking threads
  _components_level_list_t    compFuncLevel;     //!< components functionality levels list
  _TCHAR*                     customer_info;     //!< customer info
} _key_info_t;

//! Licensing key checking results
typedef struct
{
  _TCHAR*                 keyFileName;         //!< Key file name (without path)
  _key_info_t*            keyInfo;             //!< Info from key file
  _key_invalid_reason_t   invalidReason;       //!< Key invalidity reason (or kirValid)
  _date_t                 keyExpirationDate;   //!< Key expiration date
  _date_t                 appLicenseExpDate;   //!< App license expiration date
  _dword_t                daysTillExpiration;  //!< Days left till app  expiration day
  _functionality_level_t  funcLevel;           //!< Application functionality level provided by this key
  unsigned int            componentFuncBitMask;//!< Component functionality level bit mask
} _check_info_t;

typedef struct
{
  unsigned int     size;
  _key_info_t     *vector; 
} _key_info_list_t;

typedef struct
{
  unsigned int     size;
  _check_info_t   *vector;
} _check_info_list_t;

typedef struct
{
  _key_serial_number_t    keySerialNumber;
  _key_invalid_reason_t   invalidReason;
  _date_t                 keyExpDate;
  int                     isActiveKey;
} _light_check_info_t;

typedef struct
{
  unsigned int        size;
  _light_check_info_t *vector;
} _light_check_info_list_t;

typedef struct
{
  unsigned int    size;
  char            *image;
} _key_file_image_t;

typedef struct
{
  unsigned int        size;
  _key_file_image_t   *vector;
} _key_file_img_list_t;

//-----------------------------------------------------------------------------

          HRESULT       init                (ILicenseStorage      *licenseStorage,
                                             _app_info_t          *appInfo,
                                             const _TCHAR         *basePath,
                                             _checking_modes_t     checkMode
                                            );


  // ---------------- Licensing checking routines ---------------------------

          //! Checks application licensing mode against active key
          HRESULT       checkActiveKey      (_check_info_t *pCheckInfo);
          //! Checks key file and installes it if the key is new 
          HRESULT       checkKeyAndInstall  (const _TCHAR     *fileName,
                                             char             *fileBody,
                                             unsigned int    fileSize,
                                             _check_info_t  *checkInfo
                                            );
          //! Checks key presented as file image                                            
          HRESULT       checkKeyInMemory    (const _TCHAR *fileName,
                                             char         *fileBody,
                                             unsigned int fileSize,
                                             _check_info_t  *checkInfo
                                            );
          //! Returns licensing checking info for pointed key file
          HRESULT       checkKeyFile        (const _TCHAR    *keyFileName,
                                             _app_info_t   *appInfo, 
                                             _check_info_t  *checkInfo
                                            );
          //! Returns licensing checking info for key files found in pointed location
          HRESULT       checkKeyFiles       (const _TCHAR        *dir,
                                             _check_info_list_t *checkList 
                                            );

          //! Checks if active key serial number found in provided black list
          HRESULT       checkBlacklist      (const _TCHAR *blackList,
                                             size_t      size
                                            );

//#if defined (__MWERKS__)
          HRESULT       syncKeys            (const _key_file_img_list_t *keyFilesList,
                                             _light_check_info_list_t   *keysCheckInfo,
                                             _date_t                    *appLicExpDate,
                                             _functionality_level_t     *funcLevel
                                            );
//#endif

  // ---------------- Licensing managment routines ---------------------------

          //! New key registration
          HRESULT       addKey              (const _TCHAR      *fileName,
                                             _TCHAR            *fileBody,
                                             size_t             fileSize
                                            );
          //! Active key deletion
          HRESULT       revokeActiveKey     ();
          //! Registered key deletion using key serial number
          HRESULT       revokeKey           (const _key_serial_number_t *serNum);
          

  // ---------------- Licensing managment helper routines ---------------------
          

          //! Returns active licensing key info
          HRESULT       getActiveKeyInfo    (_key_info_t *keyInfo);

          //! Returns installed keys info alone with licensing checking info
          HRESULT       getInstalledKeysInfo (_check_info_list_t *checkInfo);

          //! Returns licensing key info
          HRESULT       getKeyFileInfo      (const _TCHAR     *fileName,
                                             _key_info_t      *keyInfo
                                            );
  
          //! Returns info from license keys found in pointed dir
          HRESULT       getKeyFilesInfo     (const _TCHAR       *dir,
                                             _key_info_list_t   *keysList
                                            );

          HRESULT       getKeyInfo          (const char   *fileBody,
                                             size_t       fileSize,
                                             _key_info_t  *keyInfo
                                            );


  
          //! Frees memory allocated for key_info_t structure members
          HRESULT       freeKeyInfo         (_key_info_t *keyInfo);

          //! Frees memory allocated for key_info_list_t structure members
          HRESULT       freeKeysInfo        (_key_info_list_t *keysInfo) ;

          //! Frees memory allocated for check_info_t structure members
          HRESULT       freeCheckInfo       (_check_info_t *checkInfo);

          //! Frees memory allocated for check_info_list_t structure members
          HRESULT       freeCheckInfoList   (_check_info_list_t *checkInfo);

          //! Frees memory allocated for _light_check_info_list_t structure members
          HRESULT       freeLightCheckInfoList
                                            (_light_check_info_list_t *checkInfoList);

          HRESULT       checkFileSign       (const _TCHAR *fileName);


//-----------------------------------------------------------------------------

#if defined (__cplusplus)
}; // extern "C"
#endif

#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#endif
