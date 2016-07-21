/* Copyright (C) 2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file	ExpatUtilsExt.h
 * \author	Victor V. Troitsky
 * \brief	Helpers for parsing XML file from memory buffer
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 11.01.05 v 1.00 created by VVT
 *==========================================================================
*/
#ifndef __EXPATUTILS_EXT_H__
#define __EXPATUTILS_EXT_H__

#include "ZTools/ExpatUtils/ExpatUtils.h"

//Functions

// returns 0 if succeed, otherwise non-zero value (error code)
int LoadXMLFileFromMemory(const char* buffer, size_t size, XMLTag& tag, enum eXMLTypes type,
    _STL::string *err = NULL, int SrcCodePage = 0, int DestCodePage = 0);


#endif //__EXPATUTILS_EXT_H__