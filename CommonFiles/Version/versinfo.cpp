////////////////////////////////////////////////////////////////////
//
//  VERSINFO.CPP
//  DLL version info extracting class implementation
//  AVP	general application software
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "versinfo.h"

static TCHAR viRootStr[] =             _T("\\");
static TCHAR viTransStr[] =            _T("\\VarFileInfo\\Translation");
static TCHAR viSubBlockQueryFmt[] =    _T("\\StringFileInfo\\%08lx\\%s");
static TCHAR viFileDescStr[] =         _T("FileDescription");
static TCHAR viFileVersionStr[] =      _T("FileVersion");
static TCHAR viInternalNameStr[] =     _T("InternalName");
static TCHAR viLegalCopyrightStr[] =   _T("LegalCopyright");
static TCHAR viOriginalFilenameStr[] = _T("OriginalFilename");
static TCHAR viProductNameStr[] =      _T("ProductName");
static TCHAR viProductVersionStr[] =   _T("ProductVersion");
static TCHAR viSpecialBuildStr[] =     _T("SpecialBuild");

//---
static BOOL IsFile( const TCHAR * pszFileName ) {
  if ( pszFileName ) {
  	BOOL bResult = FALSE;
    HANDLE          hFinder;
    WIN32_FIND_DATA w32fd;

    UINT uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

    hFinder = ::FindFirstFile( pszFileName, &w32fd );
    if ( hFinder != INVALID_HANDLE_VALUE ) {
      ::FindClose( hFinder );

      bResult = !(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }
    ::SetErrorMode( uiOldMode );

		return bResult;
  }
  return FALSE;
}


//
// Construct a CVersionInfo given a module or instance handle
//
CVersionInfo::CVersionInfo( HINSTANCE hModule) :
  pBuff( 0 ),
  dwLang( 0 ),
  pFixedInfo( 0 ) {

  TCHAR    modFName[_MAX_PATH];
  ::GetModuleFileName(hModule, modFName, sizeof modFName);
// Этого делать  нельзя - ::OemToAnsi(modFName, modFName);

  if ( !Init(modFName) ) {
    delete pBuff;  
    pBuff = 0; 
  }
}

//
// Construct a CVersionInfo given a module filename
//
CVersionInfo::CVersionInfo(const TCHAR * modFName)
:
  pBuff(0),
  dwLang(0),
  pFixedInfo(0)
{
  if (!Init(modFName)) {
    delete pBuff;    
    pBuff = 0; 
  }
}

//
// Internal initialization fills in the data members
//
BOOL
CVersionInfo::Init(const TCHAR * modFName)
{
	if ( !IsFile(modFName) )
		return FALSE;

  DWORD fvHandle;
  DWORD infoSize = ::GetFileVersionInfoSize((TCHAR *)modFName, &fvHandle);
  if (!infoSize) {
		return FALSE;
	}

  pBuff  = new TCHAR[UINT(infoSize)];
  if (!::GetFileVersionInfo((TCHAR *)modFName, fvHandle, infoSize, pBuff))
    return FALSE;

  // Copy string to buffer so if the -dc compiler switch (Put constant
  // strings in code segments) is on, VerQueryValue will work under Win16.
  // This works around a problem in Microsoft's ver.dll which writes to the
  // string pointed to by subBlockName.
  //
  UINT    vInfoLen;                 // not really used, but must be passed
  TCHAR* subBlockName = viRootStr;
  if (!::VerQueryValue(pBuff, subBlockName, (void **)&pFixedInfo, &vInfoLen))
    return FALSE;

  subBlockName = viTransStr;
  DWORD * trans;
  if (!::VerQueryValue(pBuff, subBlockName, (void **)&trans, &vInfoLen))
    return FALSE;

  // Swap the words so sprintf will print the lang-charset in the correct
  // format.
  //
  dwLang = MAKELONG(HIWORD(*trans), LOWORD(*trans));

  return TRUE;
}

//
// Clean up the new'd buffer
//
CVersionInfo::~CVersionInfo()
{
  delete[] pBuff;
	pBuff = 0;
}

//
// Query any given "\StringFileInfo\lang-charset\<str>" version info string
// lang indicates the language translation, may be 0 to signify file default.
//
BOOL
CVersionInfo::GetInfoString(const TCHAR * str, const TCHAR *& value, UINT lang)
{
  UINT    vSize;
  TCHAR    subBlockName[255];

  _stprintf(subBlockName, viSubBlockQueryFmt, lang ? DWORD(lang) : dwLang, str);
  return pBuff ? ::VerQueryValue(pBuff, subBlockName, (void **)&value, &vSize)
              : FALSE;
}

//
// Commonly used, predefined info string queries. Pass requested language thru,
// may be 0 to signify default.
//
BOOL CVersionInfo::GetFileDescription(const TCHAR *& fileDesc, UINT lang)
{
  return GetInfoString(viFileDescStr, fileDesc, lang);
}

//
// Retrieve the file version information in the requested language id.
//
BOOL
CVersionInfo::GetFileVersion(const TCHAR *& fileVersion, UINT lang)
{
  return GetInfoString(viFileVersionStr, fileVersion, lang);
}

//
// Retrieve the internal name of the module.
//
BOOL
CVersionInfo::GetInternalName(const TCHAR *& internalName, UINT lang)
{
  return GetInfoString(viInternalNameStr, internalName, lang);
}

//
// Retrieve the copyright message.
//
BOOL
CVersionInfo::GetLegalCopyright(const TCHAR *& copyright, UINT lang)
{
  return GetInfoString(viLegalCopyrightStr, copyright, lang);
}

//
// Retrieve the original filename.
//
BOOL
CVersionInfo::GetOriginalFilename(const TCHAR *& originalFilename, UINT lang)
{
  return GetInfoString(viOriginalFilenameStr, originalFilename, lang);
}

//
// Retrieve the product name this module is associated with.
//
BOOL
CVersionInfo::GetProductName(const TCHAR *& prodName, UINT lang)
{
  return GetInfoString(viProductNameStr, prodName, lang);
}

//
// Retrieve the version of the product.
//
BOOL
CVersionInfo::GetProductVersion(const TCHAR *& prodVersion, UINT lang)
{
  return GetInfoString(viProductVersionStr, prodVersion, lang);
}

//
// Retrieve the special build number.
//
BOOL
CVersionInfo::GetSpecialBuild(const TCHAR *& specialBuild, UINT lang)
{
  return GetInfoString(viSpecialBuildStr, specialBuild, lang);
}

//
// Get the language name string associated with a language/charset code
//
TCHAR *
CVersionInfo::GetLanguageName(UINT lang)
{
  static TCHAR langStr[128];
  ::VerLanguageName(UINT(lang), langStr, sizeof langStr);

  return langStr;
}

// returns values of TFileType if GetFileType() returned
// DevDriver or Font.
// If GetFileType() returned VirtDriver, this function returns the virtual
// device indentifier included in the virtual device control block.
DWORD CVersionInfo::GetFileSubType() const
{
  DWORD fileSubType = pFixedInfo->dwFileSubtype;

  switch (GetFileType()) {
    case DevDriver:
      switch (pFixedInfo->dwFileSubtype) {
        case VFT2_UNKNOWN:
          fileSubType = UnknownDevDriver;
          break;
        case VFT2_DRV_PRINTER:
          fileSubType = PtrDriver;
          break;
        case VFT2_DRV_KEYBOARD:
          fileSubType = KybdDriver;
          break;
        case VFT2_DRV_LANGUAGE:
          fileSubType = LangDriver;
          break;
        case VFT2_DRV_DISPLAY:
          fileSubType = DisplayDriver;
          break;
        case VFT2_DRV_MOUSE:
          fileSubType = MouseDriver;
          break;
        case VFT2_DRV_NETWORK:
          fileSubType = NtwkDriver;
          break;
        case VFT2_DRV_SYSTEM:
          fileSubType = SysDriver;
          break;
        case VFT2_DRV_INSTALLABLE:
          fileSubType = InstallableDriver;
          break;
        default: //case VFT2_DRV_SOUND:
          fileSubType = SoundDriver;
      }
      break;
    case Font:
      switch (pFixedInfo->dwFileSubtype) {
        case VFT2_UNKNOWN:
          fileSubType = UnknownFont;
          break;
        case VFT2_FONT_RASTER:
          fileSubType = RasterFont;
          break;
        case VFT2_FONT_VECTOR:
          fileSubType = VectorFont;
          break;
        default: //case VFT2_FONT_TRUETYPE:
          fileSubType = TrueTypeFont;
      }
      break;
  }
  return fileSubType;
}

//
//
//
FILETIME CVersionInfo::GetFileDate() const
{
  FILETIME ft = {pFixedInfo->dwFileDateLS, pFixedInfo->dwFileDateMS};
  return ft;
}

