#include <lictool.h>
#include <iostream>

#include <platform_compat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined (_WIN32)
  #include <io.h>
#else
  
  #if defined (__unix__)
    #include <unistd.h>
    #include <glob.h>
    #include <limits.h>
  #endif

#endif


#if defined (_MSC_VER)
  #pragma warning (push, 4)
#endif

#if defined (UNICODE)
#define out std::wcout
#define outstream std::wostream
#else
#define out std::cout
#define outstream std::ostream
#endif


//-----------------------------------------------------------------------------
 
const _TCHAR LicStorgaeFile[] = _T ("lic_storage.dat");

//-----------------------------------------------------------------------------
// Command line operation abbreviates

const _TCHAR *BasePath           = _T ("-b");
const _TCHAR *OpKeyReg           = _T ("-reg");
const _TCHAR *OpKeyRegAdd        = _T ("-regAdd");
const _TCHAR *OpKeyRevoke        = _T ("-revoke");
const _TCHAR *OpViewKeys         = _T ("-view");
const _TCHAR *OpCheckActiveKey   = _T ("-chkact");
const _TCHAR *OpCheckKeyFile     = _T ("-chkf");
const _TCHAR *OpCheckKeyFileAdd  = _T ("-chkfAdd");
const _TCHAR *OpKeyFileInfo      = _T ("-ki");
const _TCHAR *OpViewBlacklist    = _T ("-bl");
const _TCHAR *OpCheckBlacklist   = _T ("-chkbl");
const _TCHAR *OpCheckKeyBlacklist = _T ("-chkeybl");

//-----------------------------------------------------------------------------

int writeLicData (const char *s, unsigned int size, void *);
int readLicData  (char **s, unsigned int *size, void *);

bool    regKeyOpParser            (int argc, _TCHAR **argv, params_t *params);
bool    regKeyAddOpParser         (int argc, _TCHAR **argv, params_t *params);
bool    revokeKeyOpParser         (int argc, _TCHAR **argv, params_t *params);
bool    keysViewOpParser          (int argc, _TCHAR **argv, params_t *params);
bool    checkActiveKeyOpParser    (int argc, _TCHAR **argv, params_t *params);
bool    keyFileInfoOpParser       (int argc, _TCHAR **argv, params_t *params);
bool    basesPathsParser          (const string_t& s, params_t *params);
bool    viewBlacklistParser       (int argc, _TCHAR **argv, params_t *params);
bool    checkBlacklistParser      (int argc, _TCHAR **argv, params_t *params);
bool    checkKeyBlaclistParser    (int argc, _TCHAR **argv, params_t *params);
bool    checkKeyFileOpParser      (int argc, _TCHAR **argv, params_t *params);
bool    checkKeyFileAddOpParser   (int argc, _TCHAR **argv, params_t *params);

int loadFileImage  
    (const _TCHAR   *fileName, 
     char           **s, 
     unsigned int   *size
    );

outstream& operator << (outstream&, const CLicensingPolicy::check_info_t&);
outstream& operator << (outstream&, const key_info_t&);
outstream& operator << (outstream&, const functionality_level_t&);
outstream& operator << (outstream&, const date_t&);
outstream& operator << (outstream&, const key_type_t&);
outstream& operator << (outstream&, const key_serial_number_t&);
outstream& operator << (outstream&, const CLicensingPolicy::key_invalid_reason_t);

outstream& printError (outstream&, HRESULT);

//-----------------------------------------------------------------------------

typedef bool (*parser_t) (int, _TCHAR **, params_t *);

typedef struct 
{
  const _TCHAR    *abbreviate;
  op_type_t        opType;
  parser_t         parser;
} op_dispatch_t;

static op_dispatch_t parserDispTbl[] =
{
  {OpKeyReg,          otKeyReg,             regKeyOpParser},
  {OpKeyRegAdd,       otKeyRegAdd,          regKeyAddOpParser},
  {OpKeyRevoke,       otKeyRevoke,          revokeKeyOpParser},
  {OpViewKeys,        otViewInstalledKeys,  keysViewOpParser},
  {OpKeyFileInfo,     otKeyFileInfo,        keyFileInfoOpParser},
  {OpCheckActiveKey,  otCheckActiveKey,     checkActiveKeyOpParser},
  {OpViewBlacklist,   otViewBlacklist,      viewBlacklistParser},
  {OpCheckBlacklist,  otCheckAgainstBlacklist, checkBlacklistParser},
  {OpCheckKeyBlacklist,otCheckKeyBlacklist, checkKeyBlaclistParser},
  {OpCheckKeyFile,    otCheckKeyFile,       checkKeyFileOpParser},
  {OpCheckKeyFileAdd, otCheckKeyFileAdd,    checkKeyFileAddOpParser},
  {0,             otUnknown,            0}
};

ILicenseStorage licenseStorage = {readLicData, writeLicData};

CLicensingPolicy licPolicy;

//-----------------------------------------------------------------------------

void showHelp ()
{
  out << _T ("Usage:")  << std::endl
    << OpKeyReg         << _T (" <key file>         - register new key (replace mode)") << std::endl
    << OpKeyRegAdd      << _T (" <key file>         - register new key (add mode)") << std::endl
    << OpViewKeys       << _T ("                    - view registered keys") << std::endl
    << OpKeyFileInfo    << _T (" <key file>         - view key information") << std::endl
    << OpCheckKeyFile    << _T (" <key file>         - check key file (replace mode)") << std::endl
    << OpCheckKeyFileAdd    << _T (" <key file>         - check key file (add mode)") << std::endl
    << OpCheckActiveKey << _T ("                    - check active key") << std::endl;
}

//-----------------------------------------------------------------------------
// argv must point to first real command line parameter after this program name
// itself
//
bool parseCmdLine (int argc, _TCHAR **argv, params_t *params)
{

  if (!params) 
  {
    return (false);
  }

  params->clear ();

  if (0 == _tcscmp (argv[0], BasePath)) 
  {

    if (argc < 2)
    {
      out << _T ("bases paths expected") << std::endl;
      return (false);
    }

    if (! basesPathsParser (argv[1], params)) 
    {
      return (false);
    }
    argv += 2;
    argc -= 2;
  }

  parser_t parser = 0;
  size_t   i      = 0;       
  
  while (!parser && parserDispTbl[i].abbreviate) 
  {
    if (0 == _tcscmp (parserDispTbl[i].abbreviate, argv[0]))
    {
      parser = parserDispTbl[i].parser;
    }
    else
    {
      i++;
    }
  }

  if (!parser) 
  {
    params->opType = otUnknown;
    return (false);
  }

  params->opType = parserDispTbl[i].opType;

  return (parser (argc, argv, params));

}

//-----------------------------------------------------------------------------

bool regKeyOpParser (int argc, _TCHAR **argv, params_t *params)
{

  if (argc < 2) 
  {
    out << _T ("error: key file name expected") << std::endl;
    return (false);
  }

  params->keyFileName = argv[1];
  params->keyAddMode = ILicensingPolicy::kamReplace;

  return (true);

}

//-----------------------------------------------------------------------------

bool regKeyAddOpParser (int argc, _TCHAR **argv, params_t *params)
{
    bool    res = regKeyOpParser (argc, argv, params);
    params->keyAddMode = ILicensingPolicy::kamAdd;
    return (res);
}

//-----------------------------------------------------------------------------

bool revokeKeyOpParser (int argc, _TCHAR **argv, params_t *params)
{

    if (argc >= 2) 
    {
        params->serialNumber = argv[1];
    }

    return (true);
}

//-----------------------------------------------------------------------------

bool keysViewOpParser (int, _TCHAR **, params_t *)
{
  return (true);
}

//-----------------------------------------------------------------------------

bool checkActiveKeyOpParser (int argc, _TCHAR **argv, params_t *params)
{
  if (argc > 1) 
  {
      params->basesPaths.push_back (argv[1]); 
  }
  return (true);
}

//-----------------------------------------------------------------------------

bool keyFileInfoOpParser  (int argc, _TCHAR **argv, params_t *params)
{
  
  if (argc < 2) 
  {
    return (false);
  }

  params->keyFileName = argv[1];

  return (true);

}

//-----------------------------------------------------------------------------

bool checkKeyFileOpParser (int argc, _TCHAR **argv, params_t *params)
{
  if (argc < 2) 
  {
    return (false);
  }

  params->keyFileName = argv[1];
  params->keyAddMode = ILicensingPolicy::kamReplace;

  return (true);
}

//-----------------------------------------------------------------------------

bool checkKeyFileAddOpParser (int argc, _TCHAR **argv, params_t *params)
{
    bool    res;

    res = checkKeyFileOpParser (argc, argv, params);

    params->keyAddMode = ILicensingPolicy::kamAdd;

    return (res);

}

//-----------------------------------------------------------------------------

bool basesPathsParser (const string_t& s, params_t *params)
{

  if (! s.size ()) 
  {
    return (false);
  }

  string_t::size_type pos = 0;
  string_t::size_type pos1 = s.find_first_of (_T (";"), pos);

  while ( (string_t::npos != pos) && (pos < s.size ()) )
  {
    if (string_t::npos != pos1) 
    {
      params->basesPaths.push_back (s.substr (pos, pos1 - pos));
      pos = pos1 + 1;
    }
    else
    {
      params->basesPaths.push_back (s.substr (pos, s.size () - pos));
      pos = string_t::npos;
    }

    pos1 = s.find_first_of (_T (";"), pos);

  }

  return (true);
}

//-----------------------------------------------------------------------------

HRESULT   initLicensing (const _app_info_t&                appInfo,
                         const string_list_t&              basesPaths,
                         CLicensingPolicy::checking_mode_t checkMode
                        )
{
    HRESULT hr;

    hr = licPolicy.init (&licenseStorage,
                          appInfo,
                          0,
                          basesPaths,
                          checkMode
                         );

    if (FAILED (hr)) 
    {
        out << _T ("Failed to initialize licensing library.") << std::endl;
        printError (out, hr) << std::endl;
    }

    return (hr);
}

//-----------------------------------------------------------------------------

bool checkActiveKey ()
{

  CLicensingPolicy::check_info_t checkInfo;
  HRESULT                        hr;

  hr = licPolicy.checkActiveKey (&checkInfo);

  printError (out, hr) << std::endl;
  
  if (FAILED (hr)) 
  {
    printError (out, hr) << std::endl;
    return (false);
  }
  else
  {
    out << checkInfo << std::endl;
  }

  return (true);

}

//-----------------------------------------------------------------------------

bool checkKeyFile 
    (const string_t& keyFileName, 
     key_add_mode_t addMode
    )
{

  CLicensingPolicy::check_info_t checkInfo;
  HRESULT                        hr;

  hr = licPolicy.checkKeyFile (keyFileName, &checkInfo, addMode);

  printError (out, hr) << std::endl;
  
  if (FAILED (hr)) 
  {
    printError (out, hr) << std::endl;
    return (false);
  }
  else
  {
    out << checkInfo << std::endl;
  }

  return (true);
}

//-----------------------------------------------------------------------------

bool regKey (const string_t& keyFileName, key_add_mode_t addMode)
{

  CLicensingPolicy::check_info_t checkInfo;
  key_info_t                     keyInfo;
  HRESULT                        hr;

  hr = licPolicy.getKeyFileInfo (keyFileName, &keyInfo);

  if (FAILED (hr)) 
  {
    out << _T ("Cannot get info from license key ") << keyFileName
        << _T (". Error: ") << printError (out, hr) << std::endl;
    return (false);
  }
  else
  {
    out << keyInfo << std::endl;
  }

  hr = licPolicy.addKey (keyFileName, addMode);

  if (SUCCEEDED (hr))
  {
    if (S_OK == hr) 
    {
      out << _T ("Key successfully registered") << std::endl;
    }
    else
    {
      printError (out, hr) << std::endl;
    }
  }
  else
    printError (out, hr) << std::endl;

  return (SUCCEEDED (hr));

}

//-----------------------------------------------------------------------------

bool revokeKey (const string_t& serialNumber)
{

    HRESULT                                 hr;
    LicensingPolicy::key_serial_number_t    sn;

    if (!serialNumber.empty ()) 
    {
        sn.number.parts.keySerNum = _ttoi (serialNumber.c_str ());
    }

    if (!sn.number.parts.keySerNum)
    {
        hr = licPolicy.revokeActiveKey ();
    }
    else
    {
        hr = licPolicy.revokeKey (sn);
    }

    if (SUCCEEDED (hr))
    {
        if (S_OK == hr) 
        {
          out << _T ("Key is revoked successfully") << std::endl;
        }
        else
        {
          printError (out, hr) << std::endl;
        }
    }
    else
        printError (out, hr) << std::endl;

    return (SUCCEEDED (hr));

}

//-----------------------------------------------------------------------------

int writeLicData (const char *s, unsigned int size, void *)
{
  if (!s || !size) 
  {
    return (-1);
  }

  int file = _topen (LicStorgaeFile, O_CREAT | O_WRONLY | O_BINARY | O_TRUNC, S_IWRITE);

  if (-1 == file) 
  {
    return (-1);
  }

  int r = static_cast<int> (_write (file, s, size));

  _close (file);

  return (r);

}

//-----------------------------------------------------------------------------

int readLicData  (char **s, unsigned int *size, void *)
{
  if (!s || !size) 
  {
    return (-1);
  }


  int file = _topen (LicStorgaeFile, O_RDONLY | O_BINARY);

  if (-1 == file) 
  {
    return (0);
  }

  long f_size = _lseek (file, 0, SEEK_END);

  
  if (-1 == f_size) 
  {
    _close (file);
    return (-1);
  }

  *s = reinterpret_cast<char*> (malloc (static_cast<unsigned int>(f_size)));

  if (!*s) 
  {
    return (0);
  }

  *size = static_cast<unsigned int> (f_size);

  _lseek (file, 0, SEEK_SET);

  int r = static_cast<int> (_read (file, *s, *size));

  _close (file);

  return (r ? *size : 0);

}

//-----------------------------------------------------------------------------

outstream& operator << (outstream& os, const CLicensingPolicy::check_info_t& ci)
{

  os << _T ("Key file name                       : ") << ci.keyFileName        << std::endl
     << _T ("Key validity expiration date        : ") << ci.keyExpirationDate  << std::endl
     << _T ("Application license expiration date : ") << ci.appLicenseExpDate  << std::endl
     << _T ("Days to license expiration          : ") << ci.daysTillExpiration << std::endl
     << _T ("Functionality level                 : ") << ci.funcLevel;

  if (CLicensingPolicy::kirValid !=ci.invalidReason) 
  {
    os << std::endl << _T ("Key invalidity reason               : ") << ci.invalidReason;
  }
  else
  {
    os << std::endl << ci.invalidReason;
  }

  return (os);
}

//-----------------------------------------------------------------------------

outstream& operator << (outstream& os, const key_info_t& ki)
{

  os << _T ("Key creation date       : ") << ki.keyCreationDate    << std::endl
     << _T ("Key serial number       : ") << ki.keySerNum          << std::endl
     << _T ("Key type                : ") << ki.keyType            << std::endl
     << _T ("Key life span           : ") << ki.keyLifeSpan        << std::endl 
     << _T ("Key limit life date     : ") << ki.keyExpDate         << std::endl
     << _T ("Product name            : ") << ki.productName        << std::endl
     << _T ("Product id              : ") << ki.productId          << std::endl
     << _T ("Major product version   : ") << ki.productVer         << std::endl
     << _T ("Application id          : ") << ki.appId              << std::endl
     << _T ("Customer information    : ") << ki.customer_info      << std::endl;
//     << _T ("Support information     : ") << ki.supportInfo;   
     
  return (os);

}

//-----------------------------------------------------------------------------

outstream& operator << (outstream& os, const functionality_level_t& fl)
{
  _TCHAR *s;

  switch (fl)
  {
    case flNoFeatures               : s = _T ("No features"); break;
    case flOnlyUpdates              : s = _T ("Only updates"); break;
    case flFunctionWithoutUpdates   : s = _T ("No updates"); break;
    case flFullFunctionality        : s = _T ("Full functionality"); break;
    default                         : s = _T ("Unknown"); break;
  };

  os << s;
  return (os);
}

//-----------------------------------------------------------------------------

outstream& operator << (outstream& os, const date_t& d)
{

  os << d.m_day <<  _T (".") << d.m_month << _T (".") << d.m_year;

  return (os);
}

//-----------------------------------------------------------------------------
/*
outstream& operator << (outstream& os, const CLicensingPolicy::key_invalid_reason_t& ir)
{

  _TCHAR *s;

  switch (ir)
  {
    case CLicensingPolicy::kirValid             : s = _T ("Valid key"); break;
    case CLicensingPolicy::kirExpired           : s = _T ("Key is expired"); break;
    case CLicensingPolicy::kirCorrupted         : s = _T ("key is corrupted"); break;
    case CLicensingPolicy::kirNotSigned         : s = _T ("Key is not signed or sign is invalid"); break;
    case CLicensingPolicy::kirWrongProduct      : s = _T ("Key is wrong for this product"); break;
    case CLicensingPolicy::kirNotFound          : s = _T ("Key is not found"); break;
    case CLicensingPolicy::kirBlackListed       : s = _T ("Key is found in black list"); break;
    case CLicensingPolicy::kirTrialAlredyUsed   : s = _T ("Trial key already used"); break;
    case CLicensingPolicy::kirIllegalBaseUpdate : s = _T ("Antivirus bases have been updated after key expiration"); break;
    default                                     : s = _T ("Unknown"); break;
  }

  os << s;
  return (os);

}
*/
//-----------------------------------------------------------------------------

outstream& operator << (outstream& os, const key_type_t& kt)
{

  _TCHAR *s;

  switch (kt)
  {
    case ktBeta        : s = _T ("beta"); break;
    case ktTrial       : s = _T ("trial"); break;
    case ktTest        : s = _T ("test"); break;
    case ktOEM         : s = _T ("OEM"); break;
    case ktCommercial  : s = _T ("commercial"); break;
    default            : s = _T ("unknown"); break;
  }

  os << s;
  return (os);
}

//-----------------------------------------------------------------------------

outstream& operator << (outstream& os, const key_serial_number_t& sn)
{
  
  os << std::hex << sn.number.parts.appId << _T ("-")
     << sn.number.parts.memberId << _T ("-")
     << sn.number.parts.keySerNum
     << std::dec;

  return (os);

}

//-----------------------------------------------------------------------------

outstream& printError (outstream& os, HRESULT hr)
{

  if (_FACILITY_LICENSING_ == HRESULT_FACILITY (hr))
  {
    os << getLicErrMsg (hr);
    if (LIC_E_SYS_ERROR == hr) 
    {
        os << _T ("") << licPolicy.getStoredError () << std::endl;
    }
  }
  else
  {
    os << std::hex << hr << std::dec;
  }

  os << std::endl;

  return (os);

}

outstream& operator <<
    (outstream&                                     os, 
     const CLicensingPolicy::key_invalid_reason_t   reason
    )
{
    string_t    _tmp;
    HRESULT     _hr;

    _hr = licPolicy.getKeyInvalidReasonStr (reason, &_tmp);

    if (SUCCEEDED (_hr)) 
    {
        os << _tmp.c_str ();
    }
    else
    {
        os << _T ("cannon get key invalidity reason description");
    }

    return (os);

}


//-----------------------------------------------------------------------------

bool keyFileInfo (const string_t& keyFileName)
{

  key_info_t keyInfo;
  HRESULT    hr;

  if (FAILED (hr = licPolicy.getKeyFileInfo (keyFileName, &keyInfo))) 
  {
    printError (out, hr) << std::endl;
  }
  else
  {
    out << keyInfo << std::endl;
  }

  return (S_OK);
  
}

//-----------------------------------------------------------------------------

bool viewInstalledKeys ()
{

  CLicensingPolicy::check_info_list_t checkList;
  HRESULT                             hr;

  if (SUCCEEDED (hr = licPolicy.getInstalledKeysInfo (&checkList)))
  {

    if (!checkList.size ()) 
    {
      out << _T ("No installed keys") << std::endl;
      return (true);
    }

    CLicensingPolicy::check_info_list_t::iterator  i (checkList.begin ());
    CLicensingPolicy::check_info_t                *checkInfo;

    while (i != checkList.end ()) 
    {

      checkInfo = &(*i);

      out << _T ("-------------------------------------------") << std::endl
          << *checkInfo                                         << std::endl
                                                                << std::endl
          << checkInfo->keyInfo                                 << std::endl;
      i++;
    }

	bool trialPeriodOver = false;
	licPolicy.isTrialPeriodOver(trialPeriodOver);

	if (trialPeriodOver)
		out << _T("Trial period is over")  << std::endl;

    return (true);

  }
  else
  {
    printError (out, hr) << std::endl;
    return (false);
  }

}

//-----------------------------------------------------------------------------

bool viewBlacklistParser 
    (int        , 
     _TCHAR     **argv, 
     params_t   *params
    )
{

    params->blacklistName = argv[1];
    return (true);

}

//-----------------------------------------------------------------------------

bool checkBlacklistParser 
    (int        /*argc*/, 
     _TCHAR     **argv, 
     params_t   *params
    )
{
    params->blacklistName = argv[1];

    return (true);
}

//-----------------------------------------------------------------------------

bool checkKeyBlaclistParser  (int argc, _TCHAR **argv, params_t *params)
{
    if (argc < 3) 
    {
        return (false);
    }

    params->keyFileName = argv[1];
    params->blacklistName = argv[2];

    return (true);
}

//-----------------------------------------------------------------------------

bool viewBlacklist  (const string_t& fileName)
{

    LicensingPolicy::CBlacklist                 _bl;
    LicensingPolicy::CBlacklist::ResultCodeT    _ret;
    char                                        *_buf;
    size_t                                      _bufSize;

    if (loadFileImage (fileName.c_str (), &_buf, &_bufSize) < 0) 
    {
        std::cout << "Cannot load blacklist file " << std::endl;
    }

    _ret = _bl.loadBlackList (_buf, _bufSize);

    delete [] _buf;

    if (LicensingPolicy::CBlacklist::rcOK != _ret)
    {
        std::cout << "Failed to load blacklist" << std::endl;
        return (false);
    }

    LicensingPolicy::serial_number_list_t   _snl = _bl.getSerNumList ();
    LicensingPolicy::serial_number_list_t::iterator _it (_snl.begin ());

    while (_snl.end () != _it) 
    {
        std::cout << std::hex << _it->number.parts.keySerNum << std::endl;
        ++_it;
    }

    return (true);

}

//-----------------------------------------------------------------------------

void recreateTest (const _app_info_t&                appInfo,
                   const string_list_t&              basesPaths,
                   CLicensingPolicy::checking_mode_t checkMode
                  )
{

  for (size_t i = 0; i < 1000; ++i)
  {
    {

      CLicensingPolicy  lp;
      HRESULT           hr;

      hr = lp.init  (&licenseStorage,
                     appInfo,
                     0,
                     basesPaths,
                     checkMode
                    );

      if (FAILED (hr))
      {
        printError (out, hr) << std::endl;
      }
      else
      {
        CLicensingPolicy::check_info_t  ci;
        LicensingPolicy::key_info_t     ki;

        lp.checkActiveKey (&ci);
        lp.getActiveKeyInfo (&ki);
      }

    }
  }

}

//-----------------------------------------------------------------------------


int loadFileImage  
    (const _TCHAR   *fileName, 
     char           **s, 
     unsigned int   *size
    )
{
  if (!s || !size) 
  {
    return (-1);
  }


  int file = _topen (fileName, O_RDONLY | O_BINARY);

  if (-1 == file) 
  {
    return (-1);
  }

  long f_size = _lseek (file, 0, SEEK_END);

  
  if (-1 == f_size) 
  {
    _close (file);
    return (-1);
  }

  *s =  new char[(static_cast<unsigned int>(f_size))];

  if (!*s) 
  {
    return (0);
  }

  *size = static_cast<unsigned int> (f_size);

  _lseek (file, 0, SEEK_SET);

  int r = static_cast<int> (_read (file, *s, *size));

  _close (file);

  return (r ? *size : 0);

}

//-----------------------------------------------------------------------------

std::string Signature ("0XLSznpdI71fB300e7Uwj1coL/enQrDnh9ZQi+dvMgaSmk0oQRYxeUmIv0");
string_t FileName (_T ("s:\\updates5\\avp.set"));

void verifyFileSign ()
{

    HRESULT _hr;

    _hr = licPolicy.validateSign (FileName, Signature);

}

//-----------------------------------------------------------------------------

bool checkAgainstBlackList (const _TCHAR *blacklistFileName)
{

    LicensingPolicy::CBlacklist                 _bl;
    char                                        *_buf;
    size_t                                      _bufSize;
    std::string                                 _blBuf;
    HRESULT                                     _hr;

    if (loadFileImage (blacklistFileName, &_buf, &_bufSize) < 0) 
    {
        std::cout << "Cannot load blacklist file " << std::endl;
        return (false);
    }

    _blBuf.assign (_buf, _bufSize);

    delete [] _buf;

    _hr = licPolicy.checkBlacklist (_blBuf.c_str (), _blBuf.size ());
//    _hr = licPolicy.checkBlacklistFile (blacklistFileName);

    if (SUCCEEDED (_hr))
    {
        out << _T ("Active key is not found in black list") << std::endl;
    }
    else
    {
        printError (out, _hr);
    }

    return (true);
}

//-----------------------------------------------------------------------------

bool checKeyBlacklist (const string_t& keyFileName, const string_t& blacklistFileName)
{

    HRESULT _hr;
    
    _hr = licPolicy.checkKeyBlacklist (keyFileName, blacklistFileName);

    if (SUCCEEDED (_hr)) 
    {
        out << _T ("License key is not found in black list") << std::endl;
    }
    else
    {
        printError (out, _hr);
    }

    return (true);

}

//-----------------------------------------------------------------------------

#if !defined (UNICODE) && !defined (_UNICODE)

bool synchronizeKeys 
    (_app_info_t  &appInfo,
     const char   *keyFile1,
     const char   *keyFile2
    )
{

    if (!keyFile1 && !keyFile2) 
    {
        return (false);
    }

    c_intf::_key_file_img_list_t    _img_list;

    _img_list.size = keyFile1 && keyFile2 ? 2 : 1;
    _img_list.vector = new c_intf::_key_file_image_t[_img_list.size];

    size_t  _index = 0;
    int     _ret;

    if (keyFile1) 
    {
        _ret = loadFileImage(keyFile1, &_img_list.vector[_index].image, &_img_list.vector[_index].size);
        if (_ret < 0) 
        {
            return (false);
        }
    }

    
    if (keyFile2) 
    {
        _index++;
        _ret = loadFileImage(keyFile2, &_img_list.vector[_index].image, &_img_list.vector[_index].size);
        if (_ret < 0) 
        {
            return (false);
        }
    }

    c_intf::_light_check_info_list_t    _checkInfo;
    _date_t                             _expDate;
    c_intf::_functionality_level_t      _funcLevel;
    HRESULT                             _hr;

    _hr = c_intf::init (&licenseStorage, &appInfo, "", c_intf::cmKeyOnly);

    _hr = c_intf::syncKeys (&_img_list, &_checkInfo, &_expDate, &_funcLevel);



    return (true);    
}

#endif

//-----------------------------------------------------------------------------
