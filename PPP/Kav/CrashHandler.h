#if defined(_WIN32)

#ifndef __CRASHHANDLER_H__
#define __CRASHHANDLER_H__

void SetCrashHandlers();
bool WriteFullDump();

#endif // __CRASHHANDLER_H__

#endif // defined(_WIN32)