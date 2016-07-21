/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: TestInstaller.c 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::

--- revision history ---------
9/12/99 Version 1.1: Paul Ryan
+ adjustment to fit recasted PGPinstl configuration

2/8/99 Version 1.0: Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include <stdlib.h>		//for exit()
#include <stdio.h>		//for printf()
#include <direct.h>		//for chdir(), etc.
#include <stdarg.h>		//needed by windef.h
#include <windef.h>		//needed by winbase.h
#include <winbase.h>	//for LoadLibrary(), etc.


void main( int  argc, 
			char *const  argv[])	{
	char  pc_SERVERNM[] = "Test/Gambol", 
			pc_FILENM_MAIL_TEMPLATE[] = /*"mail\\PGP Testing.ntf"*/
										"mail46.ntf", 
			pc_FILENM_PGP_TEMPLATE[] = "PGP Plugin Template.nsf", 
			pc_MAIL_DBS[] = "mail\\ptest.nsf"// | "
//								"mail\\Test PGP 2.nsf|xnamesx.nsf"
							/*"/*mail\\Paul Ryan.nsf"*/, 
			pc_PROCNM_INSTALLER[] = "xus_InstallNotesComponents", 
			pc_PROCNM_UNINSTALLER[] = "xus_UninstallNotesComponents";

	char  pc_extFileNm[] = "c:\\Notes\\client\\PGPinstl.dll", 
			pc_origDir[255] = {(char) NULL}, * pc_end;
	HMODULE  h_dll;
	FARPROC  pr;
	WORD  us_err = (WORD) NULL;
	BOOL  f_failure;
	DWORD  ul_err;

	if (pc_end = strrchr( pc_extFileNm, '\\'))	{
		char * pc;
		WORD  us;

		if (!getcwd( pc_origDir, 255))
			exit( -1);

		if (!( pc = malloc( (us = pc_end - pc_extFileNm) + 1)))
			exit( -1);
		memcpy( pc, pc_extFileNm, us);
		pc[ us] = (char) NULL;

		f_failure = chdir( pc);
		free( pc);

		if (f_failure)
			exit( -1);
	} //if (pc_end = strrchr( pc_extFileNm

	if (f_failure = !(h_dll = LoadLibrary( pc_extFileNm)))	{
		ul_err = GetLastError();
		goto errJump;
	}
	if (f_failure = !(pr = GetProcAddress( h_dll, pc_PROCNM_INSTALLER/*
													pc_PROCNM_UNINSTALLER*/)))
		goto errJump;

	//installer call
	us_err = pr( pc_SERVERNM, /*NULL*/pc_FILENM_MAIL_TEMPLATE, NULL
									/*pc_FILENM_PGP_TEMPLATE*/, ""/*NULL
									"f:\\Notes\\4.5 server\\xusers.txt"*/, 
									/*""NULL*/ pc_MAIL_DBS);
	//uninstaller call
//	us_err = pr( pc_SERVERNM, ""/*NULL pc_FILENM_PGP_TEMPLATE*/, /*NULL*/
//								pc_FILENM_MAIL_TEMPLATE, 
//								""/*"f:\\Notes\\4.5 server\\xusers.txt"*/, 
//								/*""NULL*/ pc_MAIL_DBS);

errJump:
	if (!FreeLibrary( h_dll))
		if (!( us_err || f_failure))
			f_failure = TRUE;

	if (pc_origDir)
		if (chdir( pc_origDir))
			if (!( us_err || f_failure))
				f_failure = TRUE;

	exit( us_err + f_failure);
} //main(


