/* Copyright (C) 2004-2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file  SpamTestCFG.h
 * \author Alexey P. Ivanov
 * \brief SpamTest compile flags
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 26.03.04: v 1.00 created by API
 *==========================================================================
 */
#ifndef  _SPAMTESTCFG_H
#define  _SPAMTESTCFG_H

//#define __SPAMSTUB

#if !defined(__SPAMSTUB)

#define WITHOUT_BAYES
//#define WITHOUT_CFLIB
//#define WITH_LOGDEBUG_LEV2

#else //__SPAMSTUB

#define WITHOUT_BAYES
#define WITHOUT_CFLIB

#endif//__SPAMSTUB

//#define WITH_SPAMTEST_CHECKLIC

#ifdef WITHOUT_BAYES
#define TRAIN_VISIBLE NOT WS_VISIBLE | WS_DISABLED
#else
#define TRAIN_VISIBLE WS_VISIBLE
#endif

#endif//_SPAMTESTCFG_H
