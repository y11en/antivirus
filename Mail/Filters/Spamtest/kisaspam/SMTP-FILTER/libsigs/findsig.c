/* -*- C -*-
 * File: findsig.c
 *
 * Created: Sun Mar 21 13:33:29 2004
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "libsigs.h"
#ifndef WITHOUT_FSTORAGE
#include "libsigs-fst.h"
#endif
#ifdef WITH_LOGGER
#include "smtp-filter/common/logger.h"
#endif // WITH_LOGGER

#ifndef _MSC_VER
#include <sys/time.h>
#endif
#define NS 1000000

#ifdef HAVE_MD5CHECK
#include "smtp-filter/common/config.h"
#endif


int main(int ac, char *av[])
{
    double usec;
    double each;
    int i,idx,r;
    unsigned char md5[16];
    char str[33];
    md5sigarray *sa;
#ifndef WITHOUT_FSTORAGE
    fstorage_section_id sid;
#endif

#ifdef WITH_LOGGER
    logger_open("findsig", LOGGER_OPEN_NORMAL);
    logger_add_file(2, LOGGER_ALL | LOGGER_USER);
#endif // WITH_LOGGER

#ifdef HAVE_MD5CHECK
    for(i = 1; i < ac; i++)
	{
	    if(strcmp(av[i], "--print-version") == 0)
		{
		    print_version();
#ifdef WITH_LOGGER
		    logger_close();    
#endif // WITH_LOGGER
		    return 0;
		}
	}
#endif

    if(ac!=3 && ac!=4 && ac!=5)
	{
	    printf("Usage: %s [-fs] file md5 [-s {main|black|white}]\n",av[0]);
	    /* 
	    printf("  as=%i\n", ac);
	    for(i = 0; i < ac; i++) {
	      printf("  %i: %s", i, av[i]);
	    }
	    */
	    exit(1);
	}
#ifndef WITHOUT_FSTORAGE
    if(strcmp(av[1],"-fs") == 0)
    {
        fstorage *fs = fstorage_create();
        struct fstorage_connect_config fscc;

        memset(&fscc, 0, sizeof(fscc));
             
        fscc.pid = FSTORAGE_PID_CFLIB;

        if(fstorage_connect(fs, av[2], O_RDWR, 0644, FSTORAGE_OPEN_READ_WRITE, &fscc))
        {
#ifdef WITH_LOGGER
            fltlog_error(LOGIDNONE "Couldn't load storage %s.", av[2]);
#endif // WITH_LOGGER

            goto load_finish;
        }
    
        sid = FSTORAGE_SECTION_GSG;

        if(ac == 6)
        {
            if(strcmp(av[4],"-s") == 0)
            {
                if(strcmp(av[5],"main") == 0)
                    sid = FSTORAGE_SECTION_GSG;
                else if(strcmp(av[5],"black") == 0)
                    sid = FSTORAGE_SECTION_GSG_UPDATE_BLACK;
                else if(strcmp(av[5],"white") == 0)
                    sid = FSTORAGE_SECTION_GSG_UPDATE_WHITE;
            }
        }

        sa = fstorage_loadmd5sig(fs, sid);

        if(sa==NULL) 
        {   
#ifdef WITH_LOGGER
            fltlog_error(LOGIDNONE "open fstorage/find gsg section");
#endif // WITH_LOGGER
            goto load_finish; 
	}

        str2md5(md5,av[3]);
        md52str(str,md5);
        printf("Looking for %s in %s...",str,av[2]);
        r = md5sigfind(sa,md5);
        if(r==-1)
	    printf("Not found\n");
        else
	    printf("found at %d\n",r);
    
        if(fstorage_close(fs))
        {
#ifdef WITH_LOGGER
            fltlog_error(LOGIDNONE "Couldn't close storage %s.", av[2]);
#endif // WITH_LOGGER
            goto load_finish;
        }

        load_finish:

            fstorage_destroy(fs);
    }
    else
#endif
    {

        sa = loadmd5sig(av[1]);
        if(sa==NULL) 
        {
#ifdef WITH_LOGGER
            fltlog_error(LOGIDNONE "open/mmap");
#endif // WITH_LOGGER
	    goto finish;
	}

        str2md5(md5,av[2]);
        md52str(str,md5);
        printf("Looking for %s in %s...",str,av[1]);
        r = md5sigfind(sa,md5);
        if(r==-1)
	    printf("Not found\n");
        else
	    printf("found at %d\n",r);
    }
finish:
#ifdef WITH_LOGGER
    logger_close();        
#endif // WITH_LOGGER
    return 0;
}
