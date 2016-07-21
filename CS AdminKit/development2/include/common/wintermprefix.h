/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	wintermprefix.h
 * \author	Andrew Kazachkov
 * \date	09.04.2003 13:04:24
 * \brief	
 * 
 */
#ifdef _WIN32
#ifndef __KL_WINTERMPREFIX_H__
#define __KL_WINTERMPREFIX_H__

#include <tchar.h>

#define KLWTS_LOCAL     _T("Local\\")
#define KLWTS_GLOBAL    _T("Global\\")

//KLWTS
extern "C" bool KLWTS_CheckTerminalServicesSupported();

#endif //__KL_WINTERMPREFIX_H__
#endif //_WIN32
