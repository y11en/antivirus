#include "../include/err_defs.h"

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

//-----------------------------------------------------------------------------
// Licensing error messages

typedef struct
{
  HRESULT  code;
  _TCHAR  *msg;
} err_map_t;

static const err_map_t errMsgMap[] =
{

  // Success codes

  {LIC_S_KEY_ALREADY_IN_USE,       _T ("Key is already in use")},

  // Error codes

  {LIC_E_KEY_NOT_FOUND,            _T ("Key is not found")},
  {LIC_E_NOT_SIGNED,               _T ("Digital sign is not found or corrupted")},
  {LIC_E_SECURE_DATA_CORRUPT,      _T ("Licensing secure data is corrupted")},
  {LIC_E_KEY_CORRUPT,              _T ("Key is corrupted")},
  {LIC_E_KEY_INST_EXP,             _T ("Key installation period is expired")},
  {LIC_E_NO_ACTIVE_KEY,            _T ("No active key")},
  {LIC_E_KEY_IS_INVALID,           _T ("Key is invalid")},
  {LIC_E_WRITE_SECURE_DATA,        _T ("Licensing secure data saving failed")},
  {LIC_E_READ_SECURE_DATA,         _T ("Licensing secure data reading failed")},
  {LIC_E_IO_ERROR,                 _T ("I/O error")},
  {LIC_E_CANNOT_FIND_BASES,        _T ("Cannot find antivirus bases")},
  {LIC_E_CANT_REG_MORE_KEYS,       _T ("Both active and reserve key already registered")},
  {LIC_E_NOT_INITIALIZED,          _T ("Licensing library is not initialized")},
  {LIC_E_INVALID_BASES,            _T ("Antivirus bases corrupted or manually updated")},
  {LIC_E_DATA_CORRUPTED,           _T ("Data corrupted")},
  {LIC_E_CANT_REG_EXPIRED_RESERVE_KEY, _T ("Expired license key cannot be registered as reserve key")},
  {LIC_E_SYS_ERROR,                _T ("System error")},
  {LIC_E_BLACKLIST_CORRUPTED,      _T ("Black list corrupted")},
  {LIC_E_SIGN_DOESNT_CONFIRM_SIGNATURE, _T ("File sign does not confirm given signature")},
  {LIC_E_CANT_REG_BETA_KEY_FOR_RELEASE, _T ("Cannot register beta-key for commercial product release")},
  {LIC_E_CANT_REG_NON_BETA_KEY_FOR_BETA, _T ("Only beta-key can be registered for beta product release")},
  {LIC_E_KEY_WRONG_FOR_PRODUCT,     _T ("This license key is not for this product")},
  {LIC_E_KEY_IS_BLACKLISTED,        _T ("License key is blocked by Kaspersky Lab")},
  {LIC_E_TRIAL_ALREADY_USED,        _T ("Trial key has been already used on this computer")},
  {LIC_E_KEY_IS_CORRUPTED,          _T ("License key is corrupted")},
  {LIC_E_DIGITAL_SIGN_CORRUPTED,    _T ("Digital signature not found, corrupted, or does not match the Kaspersky Lab digital signature")},
  {LIC_E_CANNOT_REG_EXPIRED_NON_COMMERCIAL_KEY, _T ("Cannot register expired non-commercial key")},
  {LIC_W_KEY_DOESNT_PROVIDE_SUPPORT, _T ("License key has expired. It provides you full featured product functionality")
                                       _T ("but does not allow you to update anti-virus bases and does not provide you technical support")},
  {LIC_E_KEY_CREATION_DATE_INVALID,  _T ("License key creattion date is invalid. Check system date settings.")},
  {LIC_E_CANNOT_INSTALL_SECOND_TRIAL,      _T ("Cannot register trial key while another trial key is in use")},
  {LIC_E_BLACKLIST_CORRUPTED_OR_NOT_FOUND, _T ("Blacklist file is corrupted or not found")},
  {LIC_E_UPDATE_DESC_CORRUPTED_OR_NOT_FOUND, _T ("Update description is corrupted or not found")},
  {LIC_E_LICENSE_INFO_WRONG_FOR_PRODUCT, _T ("Internal license info wrong for this product")},
  {LIC_E_CANNOT_REG_NONVALID_RESERVE_KEY, _T ("Cannot register non-valid reserve key")},
  
  // This must be the last line
  {S_OK,                           _T ("Message is not found for this error")}
};

_TCHAR* getLicErrMsg (HRESULT hr)
{
  unsigned int i = 0;

  while ( (S_OK != errMsgMap[i].code) && (errMsgMap[i].code != hr) )
  {
    i++;
  }

  return (errMsgMap[i].msg);

}
