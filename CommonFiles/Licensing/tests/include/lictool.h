#ifndef __LICTOOL_H__
#define __LICTOOL_H__

#if defined (_MSC_VER)
  #pragma once
  #pragma warning (disable : 4786)
#endif

//-----------------------------------------------------------------------------

#include <licensing_policy.h>
namespace c_intf 
{
#include <licensing_policy_c_intf.h>
}

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

//-----------------------------------------------------------------------------

using namespace LicensingPolicy;
typedef ILicensingPolicy::key_add_mode_t key_add_mode_t;

//! Operations utility supports
typedef enum 
{
  otUnknown   = 0,
  otKeyReg,
  otKeyRegAdd,
  otKeyRevoke,
  otViewInstalledKeys,
  otCheckActiveKey,
  otCheckKeyFile,
  otCheckKeyFileAdd,
  otKeyFileInfo,
  otViewBlacklist,
  otCheckAgainstBlacklist,
  otCheckKeyBlacklist
} op_type_t;

typedef struct 
{

  void    clear ()
  {
    opType = otUnknown;
    keyFileName.resize (0);
    basesPaths.clear ();
    serialNumber.resize (0);
  }

  op_type_t      opType;
  string_t       keyFileName;
  string_list_t  basesPaths;
  string_t       blacklistName;
  string_t       serialNumber;
  key_add_mode_t keyAddMode;
} params_t;

//-----------------------------------------------------------------------------

void      showHelp ();
bool      parseCmdLine         (int argc, _TCHAR **argv, params_t *params);


HRESULT   initLicensing        (const _app_info_t&                appInfo,
                                const string_list_t&              basesPaths,
                                CLicensingPolicy::checking_mode_t checkMode
                               );
bool      regKey               (const string_t& keyFileName, key_add_mode_t addMode);
bool      revokeKey            (const string_t& serialNumber);
bool      viewInstalledKeys    ();
bool      checkActiveKey       ();
bool      keyFileInfo          (const string_t& keyFileName);
bool      viewBlacklist        (const string_t& fileName);
bool      checkAgainstBlackList(const _TCHAR *blacklistFileName);
bool      checKeyBlacklist     (const string_t& keyFileName, const string_t& blacklistFileName);
bool      checkKeyFile         (const string_t& keyFileName, key_add_mode_t addMode);

void      recreateTest         (const _app_info_t&                appInfo,
                                const string_list_t&              basesPaths,
                                CLicensingPolicy::checking_mode_t checkMode
                               );
void      verifyFileSign       ();
bool      synchronizeKeys      (_app_info_t  &appInfo,   
                                const char   *keyFile1,
                                const char   *keyFile2
                               );


//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (pop)
#endif

#endif
