#if !defined(KAV_SHELL_H)
#define KAV_SHELL_H

#if defined (_WIN32)
#include <windows.h>
#include <tchar.h>
#elif defined (__unix__)
#include <unix/tchar.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

// Ключи командной строки
enum enSHELLCMD {
#define _KAVSHELL_CMD( cmd ) cmd,
	#include "shellinc.h"
};
#undef  _KAVSHELL_CMD

// return codes from ExecuteKAVShellCommand

// Generic codes
#define SHELL_RET_OK                         0 //  Command processed successfuly
#define SHELL_RET_PARAMETER_INVALID          1 //  Parameter invalid
#define SHELL_RET_FAILED                     2 //  Unknown failure
#define SHELL_RET_TASK_FAILED                3 //  Task failed (some error occured)
#define SHELL_RET_TASK_STOPPED               4 //  Task has been stopped by user, schedule or other reason

// Scanning tasks return codes
#define SHELL_RET_SCAN_NO_THREATS            0 //  No threats detected
#define SHELL_RET_SCAN_ALL_THREATED        101 //  Threats were detected and successfuly threated
#define SHELL_RET_SCAN_UNTHREATED          102 //  Some threats detected


// Updater return codes
//#define SHELL_RET_UPDATER_ERRORS           200 //  Some errors occured during update

bool IsKAVShellCommand(const TCHAR* szCommand, enSHELLCMD* pCommandId);
int  ExecuteKAVShellCommand(enSHELLCMD nCommand, int argc, TCHAR* argv[]);
bool TerminateShellCommand();
size_t __gets(TCHAR* szString, size_t size, bool bPassword);


#if defined(__cplusplus)
} // extern "C"
#endif

#endif// KAV_SHELL_H
