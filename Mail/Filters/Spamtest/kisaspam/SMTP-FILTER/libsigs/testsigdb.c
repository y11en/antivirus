/* -*- C -*-
 * File: testsigdb.c
 *
 * Created: Sun Jul 20 23:54:16 2003
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "lib/fstorage/fstorage.h"
#include "libsigs.h"
#ifndef WITHOUT_FSTORAGE
#include "libsigs-fst.h"
#endif
#ifdef WITH_LOGGER
#include "smtp-filter/common/logger.h"
#endif // WITH_LOGGER

#ifdef _MSC_VER
#include <windows.h>
#undef random
#define random() ((unsigned long)((rand()<<16)|rand()))
#else
#include <sys/time.h>
#endif
#define NS 1000000

#ifdef HAVE_MD5CHECK
#include "smtp-filter/common/config.h"
#endif

int main(int ac, char *av[])
{
    int res = 0;
    double usec;
    double each;
#ifdef _MSC_VER
    unsigned long  tv1,tv2;
#else
    struct timeval tv1,tv2;
#endif
    int i,idx,r;
    unsigned char md5[16];
    md5sigarray *sa;
#ifndef WITHOUT_FSTORAGE
    fstorage_section_id sid;
#endif

#ifdef WITH_LOGGER
    logger_open("testsigdb", LOGGER_OPEN_NORMAL);
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

        if(fstorage_connect(fs, av[2], O_RDWR, 0644, FSTORAGE_OPEN_READ_WRITE, &fscc))
        {
#ifdef WITH_LOGGER
            fltlog_error(LOGIDNONE "Couldn't load storage %s.", av[2]);
#endif // WITH_LOGGER

            res = 1;
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
            res = 1; 
            goto load_finish;
        }

  #ifdef _MSC_VER
        tv1 = GetTickCount();
#else
        gettimeofday(&tv1,NULL);
#endif
        
        for(i = 0; i< NS; i++)
        {
            idx = random()%sa->count;
            memcpy(md5,sa->array[idx],16);
            r = md5sigfind(sa,md5);
            if(r==-1)
                printf("Not found: %d %d %d!\n",i,idx,r);
        }
#ifdef _MSC_VER
        tv2 = GetTickCount();
#else
        gettimeofday(&tv2,NULL);
#endif
        
#ifdef _MSC_VER
        usec = 1000L*(tv2 - tv1);
#else
        usec = 1000000L*(tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec);
#endif
        each = 1000000.0/(usec/NS);
        printf ("%.0f usec for %d searches, %6.7g/sec\n",usec,NS,each);
    
        if(fstorage_close(fs))
        {
#ifdef WITH_LOGGER
            fltlog_error(LOGIDNONE "Couldn't close storage %s.\n", av[2]);
#endif // WITH_LOGGER
            res = 1;
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
        
#ifdef _MSC_VER
        tv1 = GetTickCount();
#else
        gettimeofday(&tv1,NULL);
#endif
        
        for(i = 0; i< NS; i++)
        {
            idx = random()%sa->count;
            memcpy(md5,sa->array[idx],16);
            r = md5sigfind(sa,md5);
            if(r==-1)
                printf("Not found: %d %d %d!\n",i,idx,r);
        }
#ifdef _MSC_VER
        tv2 = GetTickCount();
#else
        gettimeofday(&tv2,NULL);
#endif
        
#ifdef _MSC_VER
        usec = 1000L*(tv2 - tv1);
#else
        usec = 1000000L*(tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec);
#endif
        each = 1000000.0/(usec/NS);
        printf ("%.0f usec for %d searches, %6.7g/sec\n",usec,NS,each);
    }
finish:
#ifdef WITH_LOGGER
    logger_close();    
#endif // WITH_LOGGER
    return res;
}
