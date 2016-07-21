/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	userdump_helpers.h
 * \author	Andrew Kazachkov
 * \date	05.06.2003 13:13:44
 * \brief	
 * 
 */

/*
    http://download.microsoft.com/download/win2000srv/Utility/3.0/NT45/EN-US/Oem3sr2.zip
*/

#ifndef __KL_USERDUMP_HELPERS_H__
#define __KL_USERDUMP_HELPERS_H__

/*!
    If true calls userdump
*/
extern bool g_bKLUSERDUMP_DoDump;

/*!
    if true handles exceptions by itself instead of allowing default
    Windows processing.
*/
extern bool g_bKLUSERDUMP_HandleAll;

/*!
    if true shows message boxes while making dump (if dumping is turned on)
    and handling exception (if handling is turned on).
*/
extern bool g_bKLUSERDUMP_ShowMessageBox;

/*!
    if true can use TerminateProcess function while handling exception
    (if handling is turned on), if false ExitProcess is used instead.
*/
extern bool g_bKLUSERDUMP_AllowTerminateProcess;

extern LPCTSTR  g_pKLUSERDUMP_szServiceName;
extern DWORD    g_dwKLUSERDUMP_EventId;
extern void*    g_hMainModule;

extern "C" LONG __stdcall KLUSERDUMP_FilterFunction(IN EXCEPTION_POINTERS *ExceptionPointers);

extern "C" void __cdecl KLUSERDUMP_terminate_function();

extern "C" int __cdecl KLUSERDUMP_handle_bad_alloc( size_t size );

#endif //__KL_USERDUMP_HELPERS_H__