////////////////////////////////////////////////////////////////////
//
//  csystem.cpp
//  System information wrapper
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "csystem.h"

//
// Return an initialized version info object
//
CSystem::CVersionInfo& CSystem::GetVerInfo()
{
  static CVersionInfo versionInfo;
  return versionInfo;
}

//
// Return an initialized system info object
//
CSystem::CSystemInfo& CSystem::GetSystemInfo()
{
  static CSystemInfo systemInfo;
  return systemInfo;
}

//
// Return true if the system has exception suport built in and the runtime
// library uses it for stack unwinding support.
//
bool CSystem::SupportsExceptions()
{
  static bool exceptionOK = GetPlatformId() != Win32s;
  return exceptionOK;
}

//
// Return true if the system has thread suport built in and the runtime
// library uses it.
//
bool CSystem::SupportsThreads()
{
  static bool threadOK = GetPlatformId() != Win32s;
  return threadOK;
}

//
// Return true if the system has suport for per-instance data built in and the
// runtime library uses it.
//
bool CSystem::SupportsInstanceData()
{
  static bool instanceDataOK = GetPlatformId() != Win32s;
  return instanceDataOK;
}

//
// Does this OS support a full 3d UI? (Basically, is it Win95?)
//
bool CSystem::Has3dUI()
{
  return GetVerInfo().GetMajorVersion() >= 4;  // Win95 or NT w/ New Shell
}


//
// Get the windows version number in a 16bit unsigned int, with the bytes in
// the correct order: major in high byte, minor in low byte. Uses older
// GetVersion() API call.
//
uint CSystem::GetVersion() {
  static uint version = uint( ((::GetVersion()&0xFF) << 8) |
                              ((::GetVersion()&0xFF00) >> 8) );
  return version;
}

//
// Get the windows version flags number in a 16bit unsigned int. This is the
// high 16bits of the older GetVersion() API call.
//
uint CSystem::GetVersionFlags() {
  static uint versionFlags = uint(::GetVersion() >> 16);
  return versionFlags;
}

