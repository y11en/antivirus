/* -*- C -*-
 * File: updatesigdb.c
 *
 * Created: Sun Jun 05 23:54:16 2005
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#ifndef _MSC_VER
 #include <unistd.h>
#endif
#include "libsigs.h"
#include "libsigs-fst.h"
#ifdef WITH_LOGGER
#include "smtp-filter/common/logger.h"
#endif // WITH_LOGGER

#ifdef HAVE_MD5CHECK
#include "smtp-filter/common/config.h"
#endif


int main(int ac, char *av[])
{
    int res = 0;
    size_t i;
    fstorage *res_fs;
    fstorage *old_fs;
    struct fstorage_connect_config fscc;

    size_t upd_count = 0;
    fstorage *upd_fs[MAX_GSG_UPDATE_FILES];
    //struct fstorage_connect_config upd_fscc[MAX_GSG_UPDATE_FILES];

#ifdef WITH_LOGGER
    logger_open("updatesigdb", LOGGER_OPEN_NORMAL);
    logger_add_file(2, LOGGER_ALL | LOGGER_USER);
#endif // WITH_LOGGER

    memset(&upd_fs, 0, sizeof(fstorage *)*MAX_GSG_UPDATE_FILES);

#ifdef HAVE_MD5CHECK
    int i;
    for(i = 1; i < ac; i++)
	{
	    if(strcmp(av[i], "--print-version") == 0)
		{
		    print_version();
#ifdef WITH_LOGGER
		    logger_close()
#endif // WITH_LOGGER
		    return 0;
		}
	}
#endif
    if(ac < 4)
	{
	    fprintf(stderr,"Usage: %s res_fstorage in_fstorage upd_fs1 [upd_fs2 ...]\n",av[0]);
#ifdef WITH_LOGGER
	    logger_close();
#endif // WITH_LOGGER
	    exit(1);
	}
    
    res_fs = fstorage_create();
    old_fs = fstorage_create();

    memset(&fscc, 0, sizeof(fscc));
    fscc.pid = FSTORAGE_PID_CFLIB;
    if(fstorage_connect(res_fs, av[1], O_CREAT | O_TRUNC | O_RDWR, 0644, FSTORAGE_OPEN_READ_WRITE, &fscc))
    {
#ifdef WITH_LOGGER
        fltlog_error(LOGIDNONE "Couldn't create result storage %s.\n", av[1]);
#endif // WITH_LOGGER
        res = 1;
        goto load_finish;
    }
    
    fscc.pid = FSTORAGE_PID_CFLIB;
    //if(fstorage_connect_shared(old_fs, av[2], &fscc))
    if(fstorage_connect(old_fs, av[2], O_RDONLY, 0644, FSTORAGE_OPEN_READ_ONLY, &fscc))
    {
#ifdef WITH_LOGGER
        fltlog_error(LOGIDNONE "Couldn't open storage %s.", av[2]);
#endif // WITH_LOGGER
        res = 1;
        goto load_finish;
    }
    
    upd_count = 0;
    for(i = 3; i < ac && i < MAX_GSG_UPDATE_FILES; i++)
    {
        fscc.pid = FSTORAGE_PID_CFLIB;
        upd_fs[upd_count] = fstorage_create();
        //if(fstorage_connect_shared(upd_fs[upd_count], av[i], &fscc))
        if(fstorage_connect(upd_fs[upd_count], av[i], O_RDONLY, 0644, FSTORAGE_OPEN_READ_ONLY, &fscc))
        {
#ifdef WITH_LOGGER
            fltlog_error(LOGIDNONE "Couldn't open storage %s.", av[i]);
#endif // WITH_LOGGER
        }
        else
            upd_count++;
    
    }

    if(fstorage_updatemd5sig(res_fs, old_fs, upd_count, upd_fs))
    {
#ifdef WITH_LOGGER
        fltlog_error(LOGIDNONE "Couldn't update storage.");
#endif // WITH_LOGGER
    }

    
    if(fstorage_close(res_fs))
    {
#ifdef WITH_LOGGER
        fltlog_error(LOGIDNONE "Couldn't close storage %s.", av[1]);
#endif // WITH_LOGGER
    }
    
    if(fstorage_close(old_fs))
    {
#ifdef WITH_LOGGER
        fltlog_error(LOGIDNONE "Couldn't close storage %s.\n", av[2]);
#endif // WITH_LOGGER
    }

#ifdef WITH_LOGGER
    for(i = 0; i < upd_count; i++)
        if(fstorage_close(upd_fs[i]))
        {
            fltlog_error(LOGIDNONE "Couldn't close storage %s.\n", av[i+3]);
        }
#endif // WITH_LOGGER
    

load_finish:
    
    fstorage_destroy(res_fs);
    fstorage_destroy(old_fs);
    for(i = 0; i < upd_count; i++)
        fstorage_destroy(upd_fs[i]);
    
#ifdef WITH_LOGGER
    logger_close();    
#endif // WITH_LOGGER
    return res;
}
