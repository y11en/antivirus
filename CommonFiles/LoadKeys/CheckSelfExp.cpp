////////////////////////////////////////////////////////////////////
//
//  CheckUse.cpp
//  AVP key loading and preprocessing (old and new formats)
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include <time.h>
#include <imagehlp.h>
#include <Stuff\text2bin.h>
#include "CheckUse.h"


BOOL CheckSelfExpir(SYSTEMTIME& stExpir)
{
	time_t tt_mod;
	tt_mod=(time_t)GetTimestampForLoadedLibrary((HINSTANCE)GetModuleHandle(NULL));
	if(gmtime(&tt_mod)){
		tm tm_mod=*gmtime(&tt_mod);
		tm_mod.tm_year+=1900;
		tm_mod.tm_mon+=1;
		if(stExpir.wYear>tm_mod.tm_year) return TRUE;
		if(stExpir.wYear<tm_mod.tm_year) return FALSE;
		if(stExpir.wMonth>tm_mod.tm_mon) return TRUE;
		if(stExpir.wMonth<tm_mod.tm_mon) return FALSE;
		if(stExpir.wDay>=tm_mod.tm_mday)return TRUE;
	}
	return FALSE;
}


