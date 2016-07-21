// klavsys_os.h
//
// Native platform implementations of system support classes
//

#ifndef klavsys_os_h_INCLUDED
#define klavsys_os_h_INCLUDED

#include <klava/klavsys.h>

////////////////////////////////////////////////////////////////
// OS-native implementations

#if defined(KL_PLATFORM_WINDOWS)
# include <klava/klavsys_windows.h>

typedef KLAV_Win32_Sync_Factory KLAVSYS_Sync_Factory;

#elif defined (KL_PLATFORM_UNIX)
# include <klava/klavsys_unix.h>

typedef KLAV_Unix_Sync_Factory KLAVSYS_Sync_Factory;

#else  // platform type
# error this platform is not supported

#endif // platform type

#endif // klavsys_os_h_INCLUDED

