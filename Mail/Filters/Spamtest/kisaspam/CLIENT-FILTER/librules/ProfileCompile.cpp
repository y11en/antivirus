/* Copyright (C) 2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file   ProfileCompile.cpp
 * \author Victor V. Troitsky
 * \brief  Filtration Profiles Compiler
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 06.01.05: v 1.00 created by VVT from ALK source
 *==========================================================================
 */

#include "commhdr.h"
#pragma hdrstop

#if !defined(__SPAMSTUB)
#include "ProfileCompile.h"
#include "SpamFilter.h"

#include "profile_db.h"
#include <smtp-filter/common/list_db.h>
#include <smtp-filter/paths/paths.h>
#include "parse.h"

//#include "CFUtils.h"
//#include "CFDataDll.h"
#include <client-filter/SpamTest/CFilterDll.h>
#include "LogFile.h"
#include <client-filter/PluginOLOE/idStr.h>
#include <client-filter/SpamTest/SpamTest.h>

#ifndef KIS_USAGE
    #include <ZTools/ExpatUtils/ExpatUtils.link.h>
#endif

#include "XmlFilesPool.h"

DWORD CompileProfilesInMemory(LPCTSTR pszOut, SrcXmlFileData* srcXmlFiles, size_t filesCount)
{
	list_db        *ldb;
    profile_db     *pdb;
    content_filter *cf;

	DWORD dwRC = 0;
	
#define RETURN(C) \
	do {				\
		dwRC = C;	\
		goto finish; } \
	while(0);

	XmlFilesPool xmlPool(srcXmlFiles,filesCount);

	char fileName[MAX_PATH+1];

//TODO: Should be moved out 	
	//Init cfilter.dll
	InitMkSample();
	
	//Open filter
	g_CFilterDll.Open(pszOut);
	
	//Get pointer to filter
	cf = g_CFilterDll.GetFilter();
	if(!cf)	
	{
		RETURN(CF_EXIT_UNABLE_TO_INIT_CFILTER);
	}
	
	//Create main.pdb
	_makepath( fileName, NULL, NULL, pszOut, "pdb" );
    pdb = profile_db_open(fileName, _O_BINARY | O_RDWR | O_CREAT | O_TRUNC, 0644);
	if(!pdb)
	{
		RETURN(CF_EXIT_CANT_CREATE_PROFILE_DB_FILE);
	}

	//Create main.ldb
	_makepath( fileName, NULL, NULL, pszOut, "ldb" );
    ldb = list_db_open(fileName, _O_BINARY | O_RDWR | O_CREAT | O_TRUNC, 0644);
	if(!ldb)
	{
		RETURN(CF_EXIT_CANT_CREATE_LIST_DB_FILE);
	}

    init_errors();

	if(pdb && ldb && cf)
	    if(parse_profile_db(ldb, pdb, cf, &xmlPool))
		{
			RETURN(CF_EXIT_PROFILE_PARSE_ERROR);
		}
    
	if(is_errors())
		RETURN(CF_EXIT_PROFILE_PARSE_ERROR)
	
	if(is_warnings())
		RETURN(CF_EXIT_PROFILE_PARSE_WARNINGS)



finish:
		
	//Close list db
	if(ldb)
		list_db_close(ldb);
    
	//Close profile db
	if(pdb)
		profile_db_close(pdb);
	
	//Close filter
    if(cf)
		g_CFilterDll.Close(FILTER_DEFAULTS);
	
	return dwRC;

#undef RETURN
}



#endif //!defined(__SPAMSTUB)
/*
 * <eof profile-compile.cpp>
 */
