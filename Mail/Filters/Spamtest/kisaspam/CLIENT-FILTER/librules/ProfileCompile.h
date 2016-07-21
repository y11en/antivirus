/* Copyright (C) 2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file   ProfileCompile.h
 * \author Victor V. Troitsky
 * \brief  Structures for profile compilation routines
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 06.01.05: v 1.00 created by VVT
 *==========================================================================
 */
#ifndef __PROFILECOMPILE_H__
#define __PROFILECOMPILE_H__

#define PROFILE_KPAS_DETECT				"detect-kpas.xml"
#define PROFILE_KPAS_DETECT_SUPPL       "detect-kpas-suppl.xml"

//This string must correspond the name of formal.xml profile !!!
#define PROFILE_FORMAL_NAME             "Analize Message Headers"

//Compile profiles error codes 
#define CF_EXIT_PROFILE_PARSE_WARNINGS			300
#define CF_EXIT_UNABLE_TO_INIT_CFILTER			301
#define CF_EXIT_CANT_CREATE_PROFILE_DB_FILE		302
#define CF_EXIT_CANT_CREATE_LIST_DB_FILE		303
#define CF_EXIT_PROFILE_PARSE_ERROR				304

//Content of source XML file
struct SrcXmlFileData {
	LPCSTR szFileName;
	LPCSTR szFileData;
	DWORD dwFileLen;
};

//The only function imported from librules.lib for profile compilation !!!
DWORD CompileProfilesInMemory(LPCTSTR pszOut, SrcXmlFileData* srcXmlFiles, size_t filesCount);

#endif // __PROFILECOMPILE_H__
