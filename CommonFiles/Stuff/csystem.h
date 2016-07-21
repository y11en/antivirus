////////////////////////////////////////////////////////////////////
//
//  csystem.h
//  System information wrapper
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __CSYSTEM_H
#define __CSYSTEM_H

#include <windows.h>
#include <tchar.h>
#include "stddefs.h"
//
//
// class CSystem
// ~~~~~ ~~~~~~~
class CSystem {
  public:

    // High level capability support queries
    //
    static bool SupportsExceptions();
    static bool SupportsThreads();
    static bool SupportsInstanceData();
    static bool Has3dUI();

    // More specific platform type queries
    //
    static bool IsNT();
    static bool IsWin95();
    static bool IsWin32s();

    enum CPlatformId {
      Win32s       = VER_PLATFORM_WIN32s,
      Win32Windows = VER_PLATFORM_WIN32_WINDOWS,
      Win32NT      = VER_PLATFORM_WIN32_NT,
      Win16        = -1,
    };
    static uint GetPlatformId();

    // Version information
    //
    static uint GetVersion();
    static uint GetVersionFlags();

    static uint GetMajorVersion();
    static uint GetMinorVersion();
    static uint GetBuildNumber();

    static const TCHAR* GetVersionStr();

    // Hardware information
    //
    enum CProcessor {
      Intel386     = PROCESSOR_INTEL_386,
      Intel486     = PROCESSOR_INTEL_486,
      IntelPentium = PROCESSOR_INTEL_PENTIUM,
      MipsR4000    = PROCESSOR_MIPS_R4000,
      Alpha21064   = PROCESSOR_ALPHA_21064,
      Ppc601       = -1,
      Ppc603       = -1,
      Ppc604       = -1,
      Ppc620       = -1,
    };
    static CProcessor GetProcessorType();

  protected:

    // Class that encapsulates the OSVERSIONINFO struct and the GetVersionEx()
    // Win32 call.
    //
    class CVersionInfo : private OSVERSIONINFO { //private _OSVERSIONINFOA {
      public:
        CVersionInfo();
        uint GetMajorVersion() const;
        uint GetMinorVersion() const;
        uint GetBuildNumber() const;
        uint GetPlatformId() const;
        const TCHAR* GetVersionStr() const;
    };

    // Return an initialized version info object
    //
    static CVersionInfo& GetVerInfo();

    // Class that encapsulates the _SYSTEM_INFO struct and the GetSystemInfo()
    // Win32 call.
    //
    class CSystemInfo : private _SYSTEM_INFO {
      public:
        CSystemInfo();
        CProcessor GetProcessorType() const;
    };

    // Return an initialized system info object
    //
    static CSystemInfo& GetSystemInfo();
};

//----------------------------------------------------------------------------
// Inlines
//

//
inline bool CSystem::IsNT() {
  return GetPlatformId() == Win32NT;
}

//
inline bool CSystem::IsWin95() {
  return GetPlatformId() == Win32Windows;
}

//
inline bool CSystem::IsWin32s() {
  return GetPlatformId() == Win32s;
}

//
inline uint CSystem::GetPlatformId() {
  return GetVerInfo().GetPlatformId();
}

//
inline CSystem::CProcessor CSystem::GetProcessorType() {
  return GetSystemInfo().GetProcessorType();
}

//
inline uint CSystem::GetMajorVersion() {
  return GetVerInfo().GetMajorVersion();
}

//
inline uint CSystem::GetMinorVersion() {
  return GetVerInfo().GetMinorVersion();
}

//
inline uint CSystem::GetBuildNumber() {
  return GetVerInfo().GetBuildNumber();
}

//
inline const TCHAR* CSystem::GetVersionStr() {
  return GetVerInfo().GetVersionStr();
}

//
inline CSystem::CSystemInfo::CSystemInfo() {
  // no struct size to fill in...
  ::GetSystemInfo(this);
}

//
inline CSystem::CProcessor CSystem::CSystemInfo::GetProcessorType() const {
  return (CProcessor)dwProcessorType;
}

//
inline CSystem::CVersionInfo::CVersionInfo() {
  dwOSVersionInfoSize = sizeof *this;
  ::GetVersionEx(this);
}

//
inline uint CSystem::CVersionInfo::GetMajorVersion() const {
  return dwMajorVersion;
}

//
inline uint CSystem::CVersionInfo::GetMinorVersion() const {
  return dwMinorVersion;
}

//
inline uint CSystem::CVersionInfo::GetBuildNumber() const {
  return dwBuildNumber;
}

//
inline uint CSystem::CVersionInfo::GetPlatformId() const {
  return dwPlatformId;
}

//
inline const TCHAR* CSystem::CVersionInfo::GetVersionStr() const {
  return szCSDVersion;
}

#endif  // WINSYS_SYSTEM_H
