/* Copyright (C) 2004 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file  MainInterpreter.h
 * \author Victor V. Troitsky
 * \brief Formal rules processing 
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 07.12.04: v 1.00 created by VVT
 *==========================================================================
 */
#ifndef __MAININTERPRETER_H__
#define __MAININTERPRETER_H__
#ifndef  _SPAMFILTER_H
#include "spamfilter.h"
#endif
#include "client-filter/SpamTest/SpamTest.h"

extern "C" {
	HRESULT InitFormalRules(LPCSTR szPath);
	HRESULT DoneFormalRules();
	UINT SpamTestProfile(message_with_headers *msgh);
};

#endif //__MAININTERPRETER_H__
