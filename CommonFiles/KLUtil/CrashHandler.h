// CrashHandler.h: interface for the CCrashHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRASHHANDLER_H__ABFCC1C2_3CB0_449E_AF3C_2CB3C1B47FCE__INCLUDED_)
#define AFX_CRASHHANDLER_H__ABFCC1C2_3CB0_449E_AF3C_2CB3C1B47FCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "defs.h"

namespace KLUTIL
{
	typedef bool (WINAPI *fnNotifyAboutCrashDump)(LPCTSTR pszMiniDumpFile, LPCTSTR pszFullDumpFile);

	KLUTIL_API void SetNotifyAboutCrashDump(fnNotifyAboutCrashDump pfnNotifyAboutCrashDump);

	// При записи дампа, он будет класться в папку, в которой лежит модуль hCrashedModule
	KLUTIL_API void SetCrashedModule(HMODULE hCrashedModule);

	KLUTIL_API LONG WINAPI WriteCrashDumpExceptionFilter(EXCEPTION_POINTERS *ExceptionPointers);

} // namespace KLUTIL


#endif // !defined(AFX_CRASHHANDLER_H__ABFCC1C2_3CB0_449E_AF3C_2CB3C1B47FCE__INCLUDED_)
