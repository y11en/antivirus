#if defined (_MSC_VER)
    #pragma warning (disable : 4786)
#endif

#if defined (_WIN32)

#if !defined (STRICT)
#define STRICT
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#define _WIN32_DCOM

#include <windows.h>
#include <objbase.h>

class CComLib 
{
public:
  CComLib ()
  {
    ::CoInitialize (0);
  }
  ~CComLib ()
  {
    ::CoUninitialize ();
  }
};
#endif

const char    *KeyFile1 = "../../../test data/corporate suite/0003BFDE.key";
const char    *KeyFile2 = "";

//-----------------------------------------------------------------------------
// Includes

#include <iostream>
//#include "../test data/1110r.h"
//#include "../test data/samba/license_info.h"
//#include "../test data/wks50/appinfo107r.h"
#include "../test data/isav/1109r.h"
//#include "../test data/antispam/appinfo1116r.h"
//#include "../test data/antispam/Astaro_Appinfo.h"
//#include "../test data/smtp-gateway/AppInfo1110b.h "
//#include "../../../test data/novell/1114r.h"
//#include "../test data/personal 4.6/licinfo1101r.h"
//#include "../test data/workstation/AppInfo107r.h "
//#include "../test data/workstation/AppInfo107r.h "
#include <lictool.h>

//-----------------------------------------------------------------------------

#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

//-----------------------------------------------------------------------------

#if defined (UNICODE)
#define out std::wcout
#define outstream std::wostream
#else
#define out std::cout
#define outstream std::ostream
#endif

#if !defined (_WIN32)
#define _tmain main
#endif

//-----------------------------------------------------------------------------


outstream& operator << (outstream&, const params_t&);

//-----------------------------------------------------------------------------

int _tmain (int argc, _TCHAR **argv)
{

  for (int i = 0; i < argc; ++i)
  {
      out << argv[i] << std::endl;
  }

  if (argc < 2)
  {
    showHelp ();
    return (0);
  }

  params_t params;

  if (!parseCmdLine (argc - 1, argv + 1, &params))
  {
    if (otUnknown == params.opType) 
    {
      out << _T (" : unknown operation") << std::endl;
    }
    else
    {
      out << _T ("bad command line") << std::endl;
    }
    return (-1);
  }

  if (FAILED (initLicensing (AppInfo, params.basesPaths, CLicensingPolicy::cmKeyOnly))) 
  {
    return (-1);
  }

//  synchronizeKeys (AppInfo, KeyFile1, 0);

#if defined (_WIN32)
  SetThreadLocale (MAKELCID (MAKELANGID (LANG_ENGLISH, SUBLANG_NEUTRAL), SORT_DEFAULT));
  CComLib comLib;
#endif

  switch(params.opType) 
  {
    case otKeyReg             : 
    case otKeyRegAdd          : regKey (params.keyFileName, params.keyAddMode); break;
    case otKeyRevoke          : revokeKey (params.serialNumber); break;
    case otCheckActiveKey     :	checkActiveKey (); break;
    case otKeyFileInfo        : keyFileInfo (params.keyFileName); break;
    case otViewInstalledKeys  : viewInstalledKeys (); break;
    case otViewBlacklist      : viewBlacklist (params.blacklistName); break;
    case otCheckAgainstBlacklist: checkAgainstBlackList (params.blacklistName.c_str ()); break;
    case otCheckKeyBlacklist  : checKeyBlacklist (params.keyFileName, params.blacklistName); break;
    case otCheckKeyFile       : 
    case otCheckKeyFileAdd    : 
        checkKeyFile (params.keyFileName, params.keyAddMode); break;
    default                   : break;
  }

//  recreateTest (AppInfo, params.basesPaths, CLicensingPolicy::cmFullCheck);

  return (0);

}

//-----------------------------------------------------------------------------

outstream& operator << (outstream& os, const params_t& p)
{
  _TCHAR *opStr;

  switch(p.opType)
  {
    case otKeyReg            : opStr = _T ("key registration"); break;
    case otKeyRevoke         : opStr = _T ("key recall"); break;
    case otCheckActiveKey    : opStr = _T ("active key checking"); break;
    case otViewInstalledKeys : opStr = _T ("installed keys viewing"); break;
    case otCheckKeyFile      : opStr = _T ("key file checking"); break;
    default                  : opStr = _T ("unknown operation"); break;
  }

  os << _T ("Operation     : ") << opStr << std::endl
     << _T ("Key file name : ") << p.keyFileName << std::endl
     << _T ("Bases paths   : ") << p.basesPaths.size () << std::endl;
    
  for (size_t i = 0; i < p.basesPaths.size (); i++)
  {
    os << _T ("                ") << p.basesPaths[i] << std::endl;
  }

  return (os);
  
}

//-----------------------------------------------------------------------------
