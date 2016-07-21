// profile_compiler.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include "profile_db.h"
#include "parse.h"

#include <fcntl.h>

#if 0
#include <smtp-filter\adc\adc.link.h>
#include <gnu/libnet.link.h>
#include <smtp-filter/contrib/libpatricia/libpatricia.link.h>
#include <smtp-filter/common/common.link.h>
#include <smtp-filter/filter-config/xmlfiles/xmlfiles.link.h >
#include <gnu/libexpat.link.h>
#include <gnu/libdb1.link.h>
#include <ZTools/ExpatUtils/ExpatUtils.link.h>
#include <ZTools/ZUtils/ZUtils.link.h>
#include <gnu\libregex.link.h>

#include <conio.h>

#endif

#ifndef WIN32
#define _O_BINARY 0
#endif

int main(int argc, char* argv[])
{
    int i = 0;
    if (argc < 3)
    {
        printf("\nusage:\n"
            "profile_compiler.exe out_file_path.pdb in_file_path_1.xml [in_file_path_2.xml ...]\n");
        return 10;
    }
    
    char *pszPdbPath = argv [1];
    
    profile_db     *pdb;
    pdb = profile_db_open(pszPdbPath, _O_BINARY | O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (0 == pdb)
    {
        printf("\nCan't create pdb-file %s\n", pszPdbPath);
        return 20;
    }

#if 0
    list_db        *ldb;
	ldb = list_db_open(pszLdbPath, _O_BINARY | O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (0 == ldb)
    {
        printf("\nCan't create ldb-file %s\n", pszLdbPath);
        return 21;
    }
#endif
    
    profile *p = 0;
    for (i = 2; i < argc; i ++ )
    {
        p = parse_profile(argv[i], PROFILE_COMMON, PARSE_PROFILE_DEFAULTS);
        if( p )
    		profile_db_add(pdb, p);
        else
        {
            printf("\nCan't parse xml-file %s\n", argv[i]);
            return 30;
        }
    }
    profile_db_close(pdb);

#if 0
    list_db_close(ldb);
#endif

	return 0;
}

