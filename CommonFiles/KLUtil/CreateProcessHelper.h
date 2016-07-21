#ifndef _CREATEPROCESSHELPER_H__INCLUDED_1307CC0A_8ED4_4ee3_8CAF_B9CB891D04B9
#define _CREATEPROCESSHELPER_H__INCLUDED_1307CC0A_8ED4_4ee3_8CAF_B9CB891D04B9

// 4786 'identifier' : identifier was truncated to 'number' characters in the debug information
#pragma warning( disable : 4786 )

#include <Stuff/StdString.h>
#include <string>

/// all functions here are inlined, because usage of the this header
///   is supposed to be in different projects (e.g. static libraries)
///  and this header is shipped as-is, with no dependencies
namespace KLUTIL
{
    /// The CreateProcess() function should get second parameter enclosed
    ///  in quotes, otherwise security problems described in MSDN happens.
    inline tstring encloseInQuotes(const TCHAR *executableFullPath)
    {
        // protection check
        if(!executableFullPath)
            return _T("");

        // path is empty string
        if(!*executableFullPath)
            return _T("");

        // path is already in quotes
        if(*executableFullPath == _T('"') || *executableFullPath == _T('\''))
            return executableFullPath;

        return tstring(_T("\"")) + tstring(executableFullPath) + tstring(_T("\""));
    }

    /// override function for convenience
    inline tstring encloseInQuotes(const tstring &executableFullPath)
    {
        return encloseInQuotes(executableFullPath.c_str());
    }
}

#endif  // _CREATEPROCESSHELPER_H__INCLUDED_1307CC0A_8ED4_4ee3_8CAF_B9CB891D04B9