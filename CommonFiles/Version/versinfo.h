#ifndef __VERSINFO_H__
#define __VERSINFO_H__

////////////////////////////////////////////////////////////////////
//
//  VERSINFO.H
//  DLL version info extracting class definitions
//  AVP	general application software
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <crtdbg.h>
//
// class CVersionInfo
// ~~~~~ ~~~~~~~~~~~~~~~~~~
// Access to a hModule's VERSIONINFO resource.
//
class CVersionInfo {
  public:
    // CFileOS values are returned by GetFileOS()
    enum CFileOS { OSUnknown    = VOS_UNKNOWN,
                   DOS          = VOS_DOS,
                   OS216        = VOS_OS216,
                   OS232        = VOS_OS232,
                   NT           = VOS_NT,
                   Windows16    = VOS__WINDOWS16,
                   PM16         = VOS__PM16,
                   PM32         = VOS__PM32,
                   Windows32    = VOS__WINDOWS32,
                   DosWindows16 = VOS_DOS_WINDOWS16,
                   DosWindows32 = VOS_DOS_WINDOWS32,
                   OS216PM16    = VOS_OS216_PM16,
                   OS232PM32    = VOS_OS232_PM32,
                   NTWindows32  = VOS_NT_WINDOWS32
    };
    // CFileType is returned by GetFileType()
    enum CFileType { TypeUnknown = VFT_UNKNOWN,
                     App         = VFT_APP,
                     DLL         = VFT_DLL,
                     DevDriver   = VFT_DRV,
                     Font        = VFT_FONT,
                     VirtDevice  = VFT_VXD,
                     StaticLib   = VFT_STATIC_LIB
    };
    // CFileSubType values are returned by GetFileSubType() if GetFileType
    // returned DevDriver or Font
    enum CFileSubType { UnknownDevDriver,  //VFT2_UNKNOWN
                        PtrDriver,         //VFT2_DRV_PRINTER
                        KybdDriver,        //VFT2_DRV_KEYBOARD
                        LangDriver,        //VFT2_DRV_LANGUAGE
                        DisplayDriver,     //VFT2_DRV_DISPLAY
                        MouseDriver,       //VFT2_DRV_MOUSE
                        NtwkDriver,        //VFT2_DRV_NETWORK
                        SysDriver,         //VFT2_DRV_SYSTEM
                        InstallableDriver, //VFT2_DRV_INSTALLABLE
                        SoundDriver,       //VFT2_DRV_SOUND
                        UnknownFont,       //VFT2_UNKNOWN
                        RasterFont,        //VFT2_FONT_RASTER
                        VectorFont,        //VFT2_FONT_VECTOR
                        TrueTypeFont       //VFT2_FONT_TRUETYPE
    };
    CVersionInfo(HINSTANCE hModule);
    CVersionInfo(const TCHAR * pszModFName);
   ~CVersionInfo();

    VS_FIXEDFILEINFO & GetFixedInfo();

    DWORD     GetSignature() const;
    DWORD     GetStrucVersion() const;
    DWORD     GetFileVersionMS() const;
    DWORD     GetFileVersionLS() const;
    DWORD     GetProductVersionMS() const;
    DWORD     GetProductVersionLS() const;
    BOOL      IsFileFlagSet(DWORD flag) const;
    DWORD     GetFileFlagsMask() const;
    DWORD     GetFileFlags() const;
    BOOL      IsDebug() const;
    BOOL      InfoInferred() const;
    BOOL      IsPatched() const;
    BOOL      IsPreRelease() const;
    BOOL      IsPrivateBuild() const;
    BOOL      IsSpecialBuild() const;
    DWORD     GetFileOS() const;				// returns CFileOS values
    DWORD     GetFileType() const;			// returns CFileType values
    DWORD     GetFileSubType() const;		// returns CFileSubType values
    FILETIME  GetFileDate() const;

    BOOL GetInfoString(const TCHAR * str, const TCHAR *& value, UINT lang=0);

    BOOL GetFileDescription(const TCHAR *& fileDesc, UINT lang=0);
    BOOL GetFileVersion(const TCHAR *& fileVersion, UINT lang=0);
    BOOL GetInternalName(const TCHAR *& internalName, UINT lang=0);
    BOOL GetLegalCopyright(const TCHAR *& copyright, UINT lang=0);
    BOOL GetOriginalFilename(const TCHAR *& originalFilename, UINT lang=0);
    BOOL GetProductName(const TCHAR *& prodName, UINT lang=0);
    BOOL GetProductVersion(const TCHAR *& prodVersion, UINT lang=0);
    BOOL GetSpecialBuild(const TCHAR *& debug, UINT lang=0);

    UINT   GetLanguage() const;
    TCHAR	 *GetLanguageName() const;

    static TCHAR *GetLanguageName(UINT language);

		BOOL IsInited() { return !!pBuff; }

  protected:
    BOOL Init(const TCHAR * modFName);

    void						 *pBuff;           // new'd File version info buffer
    DWORD             dwLang;           // Default language translation
    VS_FIXEDFILEINFO *pFixedInfo;  // Fixed file info structure

  private:
    // Don't allow this object to be copied.
    //
    CVersionInfo(const CVersionInfo&);
    CVersionInfo& operator =(const CVersionInfo&);
};

//
// Return the version information about this module.
//
inline VS_FIXEDFILEINFO &CVersionInfo::GetFixedInfo()
{
  _ASSERT(pFixedInfo);
  return *pFixedInfo;
}

//
inline DWORD CVersionInfo::GetSignature() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwSignature;
}

//
inline DWORD CVersionInfo::GetStrucVersion() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwStrucVersion;
}

//
// Get the major file version (first 32-bits).
//
inline DWORD CVersionInfo::GetFileVersionMS() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwFileVersionMS;
}

//
// Get the minor file version (last 32-bits).
//
inline DWORD CVersionInfo::GetFileVersionLS() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwFileVersionLS;
}

//
// Get the major product version number (first 32-bits).
//
inline DWORD CVersionInfo::GetProductVersionMS() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwProductVersionMS;
}

//
// Get the minor product version number (last 32-bits).
//
inline DWORD CVersionInfo::GetProductVersionLS() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwProductVersionLS;
}

//
// Return TRUE if the flag has been set in the version info.
//
inline BOOL CVersionInfo::IsFileFlagSet(DWORD flag) const
{
  _ASSERT(pFixedInfo);
  return (pFixedInfo->dwFileFlagsMask & flag) && (pFixedInfo->dwFileFlags & flag);
}

//
inline DWORD CVersionInfo::GetFileFlagsMask() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwFileFlagsMask;
}

//
inline DWORD CVersionInfo::GetFileFlags() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwFileFlags;
}

//
inline BOOL CVersionInfo::IsDebug() const
{
  _ASSERT(pFixedInfo);
  return (pFixedInfo->dwFileFlags & pFixedInfo->dwFileFlagsMask & VS_FF_DEBUG) ?
         TRUE : FALSE;
}

//
inline BOOL CVersionInfo::InfoInferred() const
{
  _ASSERT(pFixedInfo);
  return (pFixedInfo->dwFileFlags & pFixedInfo->dwFileFlagsMask & VS_FF_INFOINFERRED) ?
          TRUE : FALSE;
}

//
inline BOOL CVersionInfo::IsPatched() const
{
  _ASSERT(pFixedInfo);
  return (pFixedInfo->dwFileFlags & pFixedInfo->dwFileFlagsMask & VS_FF_PATCHED) ?
          TRUE : FALSE;
}

//
inline BOOL CVersionInfo::IsPreRelease() const
{
  _ASSERT(pFixedInfo);
  return (pFixedInfo->dwFileFlags & pFixedInfo->dwFileFlagsMask & VS_FF_PRERELEASE) ?
          TRUE : FALSE;
}

//
inline BOOL CVersionInfo::IsPrivateBuild() const
{
  _ASSERT(pFixedInfo);
  return (pFixedInfo->dwFileFlags & pFixedInfo->dwFileFlagsMask & VS_FF_PRIVATEBUILD) ? 
          TRUE : FALSE;
}

//
inline BOOL CVersionInfo::IsSpecialBuild() const
{
  _ASSERT(pFixedInfo);
  return (pFixedInfo->dwFileFlags & pFixedInfo->dwFileFlagsMask & VS_FF_SPECIALBUILD) ?
          TRUE : FALSE;
}

// returns CFileOS values
inline DWORD CVersionInfo::GetFileOS() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwFileOS;
}

//
inline DWORD CVersionInfo::GetFileType() const
{
  _ASSERT(pFixedInfo);
  return pFixedInfo->dwFileType;
}

//
// Return the language id of this module.
//
inline UINT CVersionInfo::GetLanguage() const
{
  return UINT(dwLang);
}

//
// Return the language name of this module.
//
inline TCHAR *CVersionInfo::GetLanguageName() const
{
  return GetLanguageName(GetLanguage());
}


#endif
