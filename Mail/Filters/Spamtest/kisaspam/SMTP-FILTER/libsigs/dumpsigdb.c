/* -*- C -*-
 * File: testsigdb.c
 *
 * Created: Sun Jul 20 23:54:16 2003
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include "libsigs.h"
#ifndef WITHOUT_FSTORAGE
#include "libsigs-fst.h"
#endif
#ifdef WITH_LOGGER
#include "smtp-filter/common/logger.h"
#endif // WITH_LOGGER

#ifdef HAVE_MD5CHECK
#include "smtp-filter/common/config.h"
#endif


int main(int ac, char *av[])
{
    int i;
    unsigned char md5str[34];
    md5sigarray *sa = NULL;
#ifndef WITHOUT_FSTORAGE
    fstorage_section_id sid;
#endif
#ifdef WITH_LOGGER
    logger_open("dumpsigdb", LOGGER_OPEN_NORMAL);
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

    if(ac!=2 && ac!=3 && ac!=5)
	{
	    printf("Usage: %s [-fs] infile [-s {main|black|white}]\n",av[0]);
#ifdef WITH_LOGGER
	    logger_close();    
#endif // WITH_LOGGER
	    exit(1);
	}
#ifndef WITHOUT_FSTORAGE    
    if(strcmp(av[1],"-fs") == 0)
    {
        fstorage *fs = fstorage_create();
        struct fstorage_connect_config fscc;

        memset(&fscc, 0, sizeof(fscc));
             
        fscc.pid = FSTORAGE_PID_CFLIB;

        if(fstorage_connect(fs, av[2], O_RDONLY, 0644, FSTORAGE_OPEN_READ_ONLY, &fscc))
        {
#ifdef WITH_LOGGER
            fltlog_error(LOGIDNONE "Couldn't load storage %s (%d).\n", av[2], fstorage_last_error(fs));
#endif // WITH_LOGGER
            goto load_finish;
        }
    
        sid = FSTORAGE_SECTION_GSG;

        if(ac == 5)
        {
            if(strcmp(av[3],"-s") == 0)
            {
                if(strcmp(av[4],"main") == 0)
                    sid = FSTORAGE_SECTION_GSG;
                else if(strcmp(av[4],"black") == 0)
                    sid = FSTORAGE_SECTION_GSG_UPDATE_BLACK;
                else if(strcmp(av[4],"white") == 0)
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

        for (i=0; i<sa->count;i++)
	    {
	        md52str(md5str,sa->array[i]);
	        md5str[32]='\n';
	        md5str[33]='\0';
	        write(1,md5str,33);
	    }

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

        for (i=0; i<sa->count;i++)
	    {
	        md52str(md5str,sa->array[i]);
	        md5str[32]='\n';
	        md5str[33]='\0';
	        write(1,md5str,33);
	    }
    }
finish:
#ifdef WITH_LOGGER
    logger_close();    
#endif // WITH_LOGGER
    return 0;
}
