// version.h

// Program version
#ifdef FSAV
#define FPIRUN_MAJOR      4
#define FPIRUN_MINOR      4
#define FPIRUN_BUILD      1141
#else
#define FPIRUN_MAJOR    1
#define FPIRUN_MINOR    60
#define FPIRUN_STRING   "1.60"
#endif

#if defined(_WIN32)
#define FPIRUN_PLATFORM "Win32"
#elif defined(_WINDOWS)
#define FPIRUN_PLATFORM "Win16"
#elif defined(OS2)
#define FPIRUN_PLATFORM "OS/2"
#elif defined(UNIX)
#define FPIRUN_PLATFORM "Unix"
#else
#define FPIRUN_PLATFORM "Other"
#endif

#ifdef _DEBUG
#define FPIRUN_MODE     "Debug"
#else
#define FPIRUN_MODE     "Release"
#endif

#define FPIRUN_VERSION FPIRUN_STRING " " FPIRUN_PLATFORM " " FPIRUN_MODE
