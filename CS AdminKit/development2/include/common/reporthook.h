/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	reporthook.h
 * \author	Andrew Kazachkov
 * \date	07.08.2003 10:35:26
 * \brief	
 * 
 */

#ifndef __KLSTD_REPORTHOOK_H__
#define __KLSTD_REPORTHOOK_H__

namespace KLSTD
{
    int __cdecl ReportHook(int nReportType, char* szaMessage, int* pnReturnValue);

    void SetupServiceDirectory();
};
#endif //__KLSTD_REPORTHOOK_H__