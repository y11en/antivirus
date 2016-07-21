#include "../include/licensing_policy.h"
#include "../include/licensing_policy_c_intf.h"
#include <algorithm>

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

//-----------------------------------------------------------------------------

static _TCHAR*                      stringToChar      (const LicensingPolicy::string_t&);
static _check_info_t*               checkInfoToC      (const LicensingPolicy::ILicensingPolicy::check_info_t&,
                                                       _check_info_t *ci
                                                      );
static _key_info_t*                 keyInfoToC        (const LicensingPolicy::key_info_t&,
                                                       _key_info_t * ki
                                                      );
static _date_t                      dateToC           (const LicensingPolicy::date_t&);
static _functionality_level_t       funcLevelToC      (LicensingPolicy::functionality_level_t);
static _key_invalid_reason_t        invReasonToC      (LicensingPolicy::ILicensingPolicy::key_invalid_reason_t);
static _components_level_list_t     compLevelToC      (LicensingPolicy::components_level_list_t);
static _key_serial_number_t         keySerNumToC      (const LicensingPolicy::key_serial_number_t&);
static _key_type_t                  keyTypeToC        (LicensingPolicy::key_type_t);
static _licensed_obj_list_t         licObjListToC     (const LicensingPolicy::licensed_object_list_t&);
static LicensingPolicy::ILicensingPolicy::checking_mode_t
                                    checkModeToCpp    (_checking_modes_t);
static _check_info_list_t*          checkInfoListToC  (const LicensingPolicy::ILicensingPolicy::check_info_list_t&,
                                                       _check_info_list_t * 
                                                      );
//-----------------------------------------------------------------------------

static LicensingPolicy::CLicensingPolicy licPolicy;

//-----------------------------------------------------------------------------

HRESULT init (ILicenseStorage    *licenseStorage,
              _app_info_t        *appInfo,
              const _TCHAR         *basePath,
              _checking_modes_t   checkMode
             )
{

  if (!licenseStorage || !appInfo)
  {
    return (E_INVALIDARG);
  }

  LicensingPolicy::string_list_t                     basesPaths;
  LicensingPolicy::CLicensingPolicy::checking_mode_t cm; 

  if (basePath)
  {
    basesPaths.push_back (LicensingPolicy::string_t (basePath));
    cm = checkModeToCpp (checkMode);
  }
  else
  {
    cm = LicensingPolicy::CLicensingPolicy::cmKeyOnly;
  }


  return (licPolicy.init (licenseStorage,
                          *appInfo,
                          0,
                          basesPaths,
                          cm
                          )
         );

}

//-----------------------------------------------------------------------------

HRESULT addKey (const _TCHAR *fileName, char *fileBody, size_t fileSize)
{
  
  if (!fileBody || !fileSize) 
  {
    return (E_INVALIDARG);
  }

  LicensingPolicy::string_t fName;

  if (!fileName) 
  {
    fName = fileName;
  }

  return (licPolicy.addKey (fileName, fileBody, fileSize));
  
}

//-----------------------------------------------------------------------------

HRESULT checkActiveKey  (_check_info_t *pCheckInfo)
{

  if (!pCheckInfo) 
  {
    return (E_INVALIDARG);
  }

  memset (pCheckInfo, 0, sizeof (_check_info_t));

  LicensingPolicy::ILicensingPolicy::check_info_t ci;
  HRESULT                                         hr;


  hr = licPolicy.checkActiveKey (&ci);

  if (FAILED (hr)) 
  {
    return (hr);
  }

  return (checkInfoToC (ci, pCheckInfo) ? hr : E_OUTOFMEMORY);

}

//-----------------------------------------------------------------------------

HRESULT checkKeyAndInstall  (const _TCHAR     *fileName,
                             char             *fileBody,
                             unsigned int      fileSize,
                             _check_info_t    *checkInfo
                            )
{

  if (!fileBody || !fileSize || !checkInfo) 
  {
    return (E_INVALIDARG);
  }

  LicensingPolicy::string_t                       fName;
  LicensingPolicy::ILicensingPolicy::check_info_t ci;
  LicensingPolicy::ILicensingPolicy::check_info_t activeKeyCheckInfo;
  HRESULT                                         hr;
  HRESULT                                         hr1;

  if (fileName) 
  {
    fName = fileName;
  }


  hr1 = licPolicy.checkActiveKey (&activeKeyCheckInfo);

  hr = licPolicy.checkKeyInMemory (fName, fileBody, fileSize, &ci);

  if (FAILED (hr)) 
  {
    return (hr);
  }

  if (!checkInfoToC (ci, checkInfo)) 
  {
    return (E_OUTOFMEMORY);
  }
  
  if (LicensingPolicy::ILicensingPolicy::kirValid != ci.invalidReason) 
  {
    return (hr);
  }

 
  hr = licPolicy.addKey (fName, fileBody, fileSize);

  
  return (hr);

}

//-----------------------------------------------------------------------------

HRESULT revokeActiveKey ()
{
  return (licPolicy.revokeActiveKey ());
}

//-----------------------------------------------------------------------------

HRESULT getActiveKeyInfo (_key_info_t *keyInfo)
{

  LicensingPolicy::key_info_t ki;
  HRESULT                     hr;

  if (!keyInfo) 
  {
    return (E_INVALIDARG);
  }

  
  hr = licPolicy.getActiveKeyInfo (&ki);

  if (FAILED (hr)) 
  {
    return (hr);
  }

  if (!keyInfoToC (ki, keyInfo))
  {
    return (E_OUTOFMEMORY);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT getKeyInfo (const char   *fileBody,
                    size_t       fileSize,
                    _key_info_t  *keyInfo
                   )
{

  if (!fileBody || !fileSize || !keyInfo) 
  {
    return (E_INVALIDARG);
  }

  LicensingPolicy::key_info_t ki;
  HRESULT                     hr;

  if (FAILED (hr = licPolicy.getKeyInfo (fileBody, fileSize, &ki))) 
  {
    return (hr);
  }

  if (!keyInfoToC (ki, keyInfo)) 
  {
    return (E_OUTOFMEMORY);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT checkKeyInMemory (const _TCHAR   *fileName,
                          char           *fileBody,
                          unsigned int   fileSize,
                          _check_info_t  *checkInfo
                         )
{

  if (!fileBody || !fileSize || !checkInfo)
  {
    return (E_INVALIDARG);
  }
  
  LicensingPolicy::string_t                       fName;
  LicensingPolicy::ILicensingPolicy::check_info_t ci;
  
  if (fileName)
  {
    fName = fileName;
  }
  
  HRESULT hr = licPolicy.checkKeyInMemory (fName,
                                           fileBody,
                                           fileSize,
                                           &ci
                                          );
  
  if (FAILED (hr))
  {
    return (hr);
  }

  if (!checkInfoToC (ci, checkInfo)) 
  {
    return (E_OUTOFMEMORY);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT getInstalledKeysInfo (_check_info_list_t *checkInfo)
{

  if (!checkInfo) 
  {
    return (E_INVALIDARG);
  }

  LicensingPolicy::ILicensingPolicy::check_info_list_t cil;
  HRESULT                             hr;

  if (FAILED (hr = licPolicy.getInstalledKeysInfo (&cil)))
  {
    return (hr);
  }


  if (!checkInfoListToC (cil, checkInfo)) 
  {
    return (E_OUTOFMEMORY);
  }

  return (S_OK);
  

}

//-----------------------------------------------------------------------------

void    getLightCheckInfo 
  (const _key_file_image_t&                                       keyImage,
   const LicensingPolicy::key_info_t&                             keyInfo,
   const LicensingPolicy::ILicensingPolicy::check_info_list_t&    installedKeys,
   const LicensingPolicy::key_serial_number_t&                    activeKeySerNum,
   _light_check_info_t                                            *checkInfo
  )
{

  memset (checkInfo, 0, sizeof (_light_check_info_t));

  // TODO!
  LicensingPolicy::ILicensingPolicy::check_info_list_t::const_iterator i
      (std::find_if (installedKeys.begin (), 
                     installedKeys.end (), 
                     LicensingPolicy::ILicensingPolicy::CCheckInfoFindKeyPred (keyInfo.keySerNum)
                    ));


  const LicensingPolicy::ILicensingPolicy::check_info_t *ci;
  LicensingPolicy::ILicensingPolicy::check_info_t cci;

  if (i != installedKeys.end ()) 
  {
    ci = &(*i);
  }
  else
  {
    // Key was not installed
    licPolicy.checkKeyInMemory (_T (""), keyImage.image, keyImage.size, &cci);
    ci = &cci;
  }

  checkInfo->invalidReason = invReasonToC (ci->invalidReason);
  checkInfo->keyExpDate = dateToC (ci->keyExpirationDate);
  checkInfo->keySerialNumber = keySerNumToC (ci->keyInfo.keySerNum);
  checkInfo->isActiveKey = !activeKeySerNum.empty () &&
                           (ci->keyInfo.keySerNum == activeKeySerNum);


}

HRESULT getActiveKeySerNum (LicensingPolicy::key_serial_number_t *sn)
{
  
  LicensingPolicy::key_info_t ki;

  HRESULT hr = licPolicy.getActiveKeyInfo (&ki);

  if (FAILED (hr)) 
  {
    return (hr);
  }

  if (S_OK == hr) 
  {
    *sn = ki.keySerNum;
  }
  else
  {
    sn->clear ();
  }
    
  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT getReserveKeySerNum (LicensingPolicy::key_serial_number_t *sn)
{
  
  LicensingPolicy::key_info_t ki;

  HRESULT hr = licPolicy.getReserveKeyInfo (&ki);

  if (FAILED (hr)) 
  {
    return (hr);
  }

  if (S_OK == hr) 
  {
    *sn = ki.keySerNum;
  }
  else
  {
    sn->clear ();
  }
    
  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT syncKeys  (const _key_file_img_list_t *keyFilesList,
                   _light_check_info_list_t   *keysCheckInfo,
                   _date_t                    *appLicExpDate,
                   _functionality_level_t     *funcLevel
                  )
{

  if (!keyFilesList || !keysCheckInfo || !appLicExpDate || !funcLevel)
  {
    return (E_INVALIDARG);
  }

  HRESULT                                               hr;
  HRESULT                                               hr_CheckActiveKey;
  LicensingPolicy::ILicensingPolicy::check_info_list_t  installedKeys;
  LicensingPolicy::ILicensingPolicy::key_info_list_t    kil;
  LicensingPolicy::key_info_t                           activeKeyInfo;
  LicensingPolicy::key_info_t                           reserveKeyInfo;
  LicensingPolicy::key_info_t                           ki;
  LicensingPolicy::ILicensingPolicy::check_info_t       ci;
  unsigned int                                          c;
  bool                                                  needToRevokeActiveKey;
  bool                                                  needToRevokeReserveKey;
  LicensingPolicy::key_serial_number_t                  activeKeySerNum;
  LicensingPolicy::key_serial_number_t                  reserveKeySerNum;


  keysCheckInfo->size = 0;
  keysCheckInfo->vector = 0;
  memset (appLicExpDate, 0, sizeof (_date_t));
  *funcLevel = flUnknown;
  
  activeKeySerNum.clear ();
  reserveKeySerNum.clear ();

  // Get active key serial number

  if (FAILED (hr = getActiveKeySerNum (&activeKeySerNum))) 
  {
    return (hr);
  }

  // Get reserve key serial number
  
  if (FAILED (hr = getReserveKeySerNum (&reserveKeySerNum))) 
  {
    return (hr);
  }

  // Get information about keys given for doing synchronization

  for (c = 0; c < keyFilesList->size; ++c)
  {

    hr = licPolicy.getKeyInfo  (keyFilesList->vector[c].image, 
                                keyFilesList->vector[c].size,
                                &ki
                               );
    if (SUCCEEDED (hr)) 
    {
      kil.push_back (ki);
    }
    else
    {
      return (hr);
    }
  }


  // Check if active key installed and must be revoked

  if (!activeKeySerNum.empty ()) 
  {
    if (!kil.empty ()) 
    {
      needToRevokeActiveKey = activeKeySerNum != kil.begin ()->keySerNum; 
    }
    else
    {
      needToRevokeActiveKey = true;
    }
  }
  else
  {
    needToRevokeActiveKey = false;
  }

//---------

  if (needToRevokeActiveKey) 
  {
    if (FAILED (hr = licPolicy.revokeActiveKey ()))
    {
      return (hr);
    }
    else
    {
      activeKeySerNum.clear ();
      reserveKeySerNum.clear ();
    }
  }

//---------

  if (!reserveKeySerNum.empty ()) 
  {

    if (kil.size () > 1) 
    {
      needToRevokeReserveKey = reserveKeySerNum != kil[1].keySerNum; 
    }
    else
    {
      needToRevokeReserveKey = false;
    }

  }
  else
  {
    needToRevokeReserveKey = false;
  }

  if (needToRevokeReserveKey) 
  {
    if (FAILED (hr = licPolicy.revokeKey (reserveKeyInfo.keySerNum))) 
    {
      return (hr);
    }
    else
    {
      reserveKeySerNum.clear ();
    }
  }



  for (c = 0; c < keyFilesList->size; ++c)
  {
    licPolicy.addKey  (_T (""),
                       keyFilesList->vector[c].image, 
                       keyFilesList->vector[c].size
                      );
  }

  if (FAILED (hr_CheckActiveKey = licPolicy.checkActiveKey (&ci))) 
  {
    return (hr_CheckActiveKey);
  }


  *appLicExpDate  = dateToC (ci.appLicenseExpDate);
  *funcLevel      = funcLevelToC (ci.funcLevel);


  if (FAILED (hr = licPolicy.getInstalledKeysInfo (&installedKeys))) 
  {
    return (hr);
  }

  if (installedKeys.empty ()) 
  {
    return (LIC_E_NO_ACTIVE_KEY);
  }


  if (FAILED (hr = getActiveKeySerNum (&activeKeySerNum))) 
  {
    return (hr);
  }

  if (FAILED (hr = getReserveKeySerNum (&reserveKeySerNum))) 
  {
    return (hr);
  }


  keysCheckInfo->vector = reinterpret_cast<_light_check_info_t*> (
                            malloc (sizeof (_light_check_info_t) * keyFilesList->size));

  if (!keysCheckInfo->vector) 
  {
    return (E_OUTOFMEMORY);
  }

  keysCheckInfo->size = keyFilesList->size;


  for (unsigned int i = 0; i < kil.size (); ++i)
  {
    getLightCheckInfo (keyFilesList->vector[i],
                       kil[i],
                       installedKeys,
                       activeKeySerNum,
                       keysCheckInfo->vector + i
                      );
  }


  return (hr_CheckActiveKey);

}

//-----------------------------------------------------------------------------

_TCHAR* stringToChar (const LicensingPolicy::string_t& s)
{

  _TCHAR *ss = reinterpret_cast<_TCHAR*> (malloc ( (s.size () + 1) * sizeof (_TCHAR)));

  if (!ss) 
  {
    return (0);
  }

  _tcscpy (ss, s.c_str ());

  return (ss);

}

//-----------------------------------------------------------------------------

_check_info_t* checkInfoToC (const LicensingPolicy::ILicensingPolicy::check_info_t& cpp,
                             _check_info_t *ci
                            )
{

  if (!ci) 
  {
    return (0);
  }

  memset (ci, 0, sizeof (_check_info_t));

  if (! (ci->keyFileName = stringToChar (cpp.keyFileName)))
 {
   freeCheckInfo (ci);
   return (0);
 }

 if (! (ci->keyInfo = (_key_info_t *) malloc (sizeof (_key_info_t))) )
 {
   freeCheckInfo (ci);
   return (0);
 }
 else
 {
   memset (ci->keyInfo, 0, sizeof (_key_info_t));
 }


 if (!keyInfoToC (cpp.keyInfo, ci->keyInfo)) 
 {
   freeCheckInfo (ci);
   return (0);
 }

 ci->funcLevel = funcLevelToC (cpp.funcLevel);
 ci->daysTillExpiration = cpp.daysTillExpiration;
 ci->componentFuncBitMask = cpp.componentFuncBitMask;
 ci->invalidReason = invReasonToC (cpp.invalidReason);
 ci->keyExpirationDate = dateToC (cpp.keyExpirationDate);
 ci->appLicenseExpDate = dateToC (cpp.appLicenseExpDate);

 return (ci);

}

//-----------------------------------------------------------------------------

_key_info_t* keyInfoToC (const LicensingPolicy::key_info_t& cpp,
                         _key_info_t *c 
                        )
{

  if (!c) 
  {
    return (0);
  }

  c->appId                                = cpp.appId;
  c->compFuncLevel                        = compLevelToC (cpp.compFuncLevel);

  c->customer_info                        = stringToChar (cpp.customer_info);
  if (!c->customer_info)
  {
    freeKeyInfo (c);
    return (0);
  }

  c->licensedObjLst                       = licObjListToC (cpp.licensedObjLst);

  c->keyCreationDate                      = dateToC (cpp.keyCreationDate);
  c->keyExpDate                           = dateToC (cpp.keyExpDate);
  c->keyLifeSpan                          = cpp.keyLifeSpan;
  c->keySerNum                            = keySerNumToC (cpp.keySerNum);
  c->keyType                              = keyTypeToC (cpp.keyType);
  c->keyVer                               = cpp.keyVer;
  c->licenseCount                         = cpp.licenseCount;

  c->licenseInfo                          = stringToChar (cpp.licenseInfo);
  if (!c->licenseInfo)
  {
    freeKeyInfo (c);
    return (0);
  }

  c->marketSector                         = cpp.marketSector;
  c->productId                            = cpp.productId;

  c->productName                          = stringToChar (cpp.productName);
  if (!c->productName)
  {
    freeKeyInfo (c);
    return (0);
  }


  c->productVer                           = stringToChar (cpp.productVer);
  if (!c->productVer)
  {
    freeKeyInfo (c);
    return (0);
  }

  c->supportInfo                          = stringToChar (cpp.supportInfo);
  if (!c->supportInfo)
  {
    freeKeyInfo (c);
    return (0);
  }

  return (c);

}

//-----------------------------------------------------------------------------

_date_t dateToC (const LicensingPolicy::date_t& cpp)
{
  _date_t c;

  c.day   = cpp.m_day;
  c.month = cpp.m_month;
  c.year  = cpp.m_year;

  return (c);

}

//-----------------------------------------------------------------------------

_functionality_level_t funcLevelToC (LicensingPolicy::functionality_level_t cpp)
{

  _functionality_level_t c;

#pragma message ("===============================>>>Add OnlyUpdates functionality level here!")

  switch (cpp)
  {
    case LicensingPolicy::flNoFeatures              : c = flNoFeatures; break;
    case LicensingPolicy::flFunctionWithoutUpdates  : c = flNoUpdates; break;
    case LicensingPolicy::flFullFunctionality       : c = flFullFunctionality; break;
    default :
      c = flUnknown; break;
  }

  return (c);

}

//-----------------------------------------------------------------------------

_key_invalid_reason_t invReasonToC (LicensingPolicy::ILicensingPolicy::key_invalid_reason_t cpp)
{

  _key_invalid_reason_t c;

  switch (cpp)
  {
    case LicensingPolicy::ILicensingPolicy::kirValid              : c = kirValid; break;
    case LicensingPolicy::ILicensingPolicy::kirExpired            : c = kirExpired; break;
    case LicensingPolicy::ILicensingPolicy::kirCorrupted          : c = kirCorrupted; break;
    case LicensingPolicy::ILicensingPolicy::kirNotSigned          : c = kirNotSigned; break;
    case LicensingPolicy::ILicensingPolicy::kirWrongProduct       : c = kirWrongProduct; break;
    case LicensingPolicy::ILicensingPolicy::kirNotFound           : c = kirNotFound; break;
    case LicensingPolicy::ILicensingPolicy::kirBlackListed        : c = kirBlackListed; break;
    case LicensingPolicy::ILicensingPolicy::kirTrialAlredyUsed    : c = kirTrialAlredyUsed; break;
    case LicensingPolicy::ILicensingPolicy::kirIllegalBaseUpdate  : c = kirIllegalBaseUpdate; break;
    default:
      c = kirUnknown; break;
  }

  return (c);

}

//-----------------------------------------------------------------------------

_components_level_list_t compLevelToC (LicensingPolicy::components_level_list_t cpp)
{

  _components_level_list_t c;

  c.size = (_dword_t)cpp.size ();
  c.vector = 0;

  if (!c.size)
  {
    return (c);
  }

  c.vector = reinterpret_cast<_component_level_t*> (malloc (cpp.size () * 
                                                           sizeof (_component_level_t)));

  if (!c.vector) 
  {
    c.size = 0;
    return (c);
  }

  LicensingPolicy::components_level_list_t::iterator i (cpp.begin ());

  _component_level_t* p = c.vector;

  while (i != cpp.end ()) 
  {
    p->componentId = i->first;
    p->funcLevel   = i->second;
    p++;
    i++;
  }

  return (c);

}

//-----------------------------------------------------------------------------

_key_serial_number_t keySerNumToC (const LicensingPolicy::key_serial_number_t& cpp)
{

  _key_serial_number_t c;

  c.memberId   = cpp.number.parts.memberId;
  c.appId      = cpp.number.parts.appId;
  c.serNum     = cpp.number.parts.keySerNum;

  return (c);

}

//-----------------------------------------------------------------------------

_key_type_t keyTypeToC (LicensingPolicy::key_type_t cpp)
{

  _key_type_t c;

  switch (cpp)
  {
    case LicensingPolicy::ktBeta      : c = ktBeta; break;
    case LicensingPolicy::ktTrial     : c = ktTrial; break;
    case LicensingPolicy::ktTest      : c = ktTest; break;
    case LicensingPolicy::ktOEM       : c = ktOEM; break;
    case LicensingPolicy::ktCommercial: c = ktCommercial; break;
    default:
      c = ktUnknown; break;
  }

  return (c);

}

//-----------------------------------------------------------------------------

_licensed_obj_list_t licObjListToC (const LicensingPolicy::licensed_object_list_t& cpp)
{

  _licensed_obj_list_t c;

  c.size = (_dword_t)cpp.size ();
  c.vector = 0;

  if (!c.size) 
  {
    return (c);
  }

  c.vector = reinterpret_cast<_licensed_object_t*> (malloc (c.size * 
                                                            sizeof (_licensed_object_t)));

  if (!c.vector) 
  {
    c.size = 0;
    return (c);
  }

  LicensingPolicy::licensed_object_list_t::const_iterator i (cpp.begin ());
  _licensed_object_t *lo = c.vector;


  while (i != cpp.end ())
  {
    lo->objId = i->first;
    lo->licenseNumber = i->second;
    i++;
    lo++;
  }

  return (c);

}

//-----------------------------------------------------------------------------

LicensingPolicy::CLicensingPolicy::checking_mode_t checkModeToCpp (_checking_modes_t c)
{

  LicensingPolicy::CLicensingPolicy::checking_mode_t cpp;

  switch (c)
  {
    case cmFullCheck : cpp = LicensingPolicy::CLicensingPolicy::cmFullCheck; break;
    case cmKeyOnly   : cpp = LicensingPolicy::CLicensingPolicy::cmKeyOnly; break;
    default:
      cpp = LicensingPolicy::CLicensingPolicy::cmUnknown; break;
  }

  return (cpp);

}

//-----------------------------------------------------------------------------

HRESULT freeKeyInfo (_key_info_t *keyInfo)
{

  if (!keyInfo) 
  {
    return (S_OK);
  }

  if (keyInfo->compFuncLevel.vector)
  {
    free (keyInfo->compFuncLevel.vector);
  }

  if (keyInfo->customer_info) 
  {
    free (keyInfo->customer_info);
  }

  if (keyInfo->licensedObjLst.vector) 
  {
    free (keyInfo->licensedObjLst.vector);
  }

  if (keyInfo->licenseInfo)
  {
    free (keyInfo->licenseInfo);
  }

  if (keyInfo->productName) 
  {
    free (keyInfo->productName);
  }

  if (keyInfo->productVer) 
  {
    free (keyInfo->productVer);
  }

  if (keyInfo->supportInfo) 
  {
    free (keyInfo->supportInfo);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT freeKeysInfo (_key_info_list_t *)
{
  return (E_NOTIMPL);
}

//-----------------------------------------------------------------------------

HRESULT freeCheckInfo (_check_info_t *checkInfo)
{

  if (!checkInfo) 
  {
    return (S_OK);
  }


  if (checkInfo->keyFileName) 
  {
    free (checkInfo->keyFileName);
  }

  if (checkInfo->keyInfo) 
  {
    freeKeyInfo (checkInfo->keyInfo);
    free (checkInfo->keyInfo);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT freeCheckInfoList (_check_info_list_t *ci)
{

  if (!ci) 
  {
    return (E_INVALIDARG);
  }

  for (unsigned int i = 0; i < ci->size; ++i)
  {
    freeCheckInfo (ci->vector + i);
  }

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT freeLightCheckInfoList (_light_check_info_list_t *checkInfoList)
{

  if (!checkInfoList) 
  {
    return (S_OK);
  }

  if (!checkInfoList->vector) 
  {
    return (S_OK);
  }

  free (checkInfoList->vector);

  return (S_OK);

}

//-----------------------------------------------------------------------------

HRESULT checkFileSign (const _TCHAR *fileName)
{

  if (!fileName) 
  {
    return (E_INVALIDARG);
  }

  LicensingPolicy::string_t fName (fileName);

  return (licPolicy.checkFileSign (fName));

}

//-----------------------------------------------------------------------------


_check_info_list_t* checkInfoListToC  (const LicensingPolicy::ILicensingPolicy::check_info_list_t& cpp,
                                       _check_info_list_t *c 
                                      )
{
  
  if (!c) 
  {
    return (0);
  }

  memset (c, 0, sizeof (_check_info_list_t));

  if (cpp.empty ()) 
  {
    return (c);
  }


  c->vector = reinterpret_cast<_check_info_t*> (malloc (sizeof (_check_info_t) * cpp.size ()));

  if (!c->vector) 
  {
    return (0);
  }

  c->size = (unsigned int) cpp.size ();

  LicensingPolicy::ILicensingPolicy::check_info_list_t::const_iterator end (cpp.end ());
  LicensingPolicy::ILicensingPolicy::check_info_list_t::const_iterator i (cpp.begin ());
  unsigned int counter = 0;

  while (end != i)
  {

    if (!checkInfoToC (*i, c->vector + counter))
    {
      freeCheckInfoList (c);
      return (0);
    }

    ++counter;
    ++i;
  }

  return (c);

}

//-----------------------------------------------------------------------------
