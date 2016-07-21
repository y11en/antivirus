/* -*- C -*-
 * File: mksigdb.c
 *
 * Created: Sun Jul 20 23:48:41 2003
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
#ifdef HAVE_MD5CHECK
#include "smtp-filter/common/config.h"
#endif

typedef enum {S_MAIN, S_BLACK, S_WHITE} section_type_t;

typedef struct
{
    char *infile;
    int is_fstorage;
    char *outfile;
    int is_section;
    section_type_t section_type;
    int add_mode;
    sort_list_t sort_mode;
    int do_print_version;
}
params_t;

int parse_command_line (int ac, char *av[], params_t *params);

int main(int ac, char *av[])
{
    params_t args;
    int res = 0;
    md5sigarray *sa = NULL;
#ifndef WITHOUT_FSTORAGE
    fstorage_section_id sid;
#endif

    if ( 0 != parse_command_line(ac, av, &args) )
    {
	    printf("Usage: %s infile [-fs] outfile [-s {main|black|white}] [-a] [-nosort]\n"
               "       %s --print-version\n" ,av[0],av[0]);
        exit (1);
    }

	if( args.do_print_version )
	{
		print_version();
		return 0;
	}

#ifdef WITH_LOGGER
    logger_open("mksigdb", LOGGER_OPEN_NORMAL);
    logger_add_file(2, LOGGER_ALL | LOGGER_USER);
#endif//WITH_LOGGER

    if((sa = mkmd5siglistex(args.infile, args.sort_mode)))
	{
#ifndef WITHOUT_FSTORAGE
        if(args.is_fstorage)
        {
            fstorage *fs = fstorage_create();
            struct fstorage_connect_config fscc;

            memset(&fscc, 0, sizeof(fscc));
             
            fscc.pid = FSTORAGE_PID_CFLIB;
            if(args.add_mode)
            {
                if(fstorage_connect(fs, args.outfile, O_RDWR, 0644, FSTORAGE_OPEN_READ_WRITE, &fscc))
                {
#ifdef WITH_LOGGER
                    fltlog_error(LOGIDNONE "Couldn't open storage %s.", args.outfile);
#endif // WITH_LOGGER
                    res = 1;
                    goto load_finish;
                }
            }
            else if(fstorage_connect(fs, args.outfile, O_CREAT | O_TRUNC | O_RDWR, 0644, FSTORAGE_OPEN_READ_WRITE, &fscc))
            {
#ifdef WITH_LOGGER
                fltlog_error(LOGIDNONE "Couldn't create storage %s.", args.outfile);
#endif // WITH_LOGGER
                res = 1;
                goto load_finish;
            }

            sid = FSTORAGE_SECTION_GSG;
            switch (args.section_type)
            {
            case S_MAIN:
                sid = FSTORAGE_SECTION_GSG;
                break;
            case S_BLACK:
                sid = FSTORAGE_SECTION_GSG_UPDATE_BLACK;
                break;
            case S_WHITE:
                sid = FSTORAGE_SECTION_GSG_UPDATE_WHITE;
                break;
            default:
#ifdef WITH_LOGGER
                fltlog_error(LOGIDNONE "Wrong section.");
#endif // WITH_LOGGER
                res = 1;
                goto load_finish;
            }

            if(fstorage_md5sigsave(fs, sa, sid) != 0)
            {
#ifdef WITH_LOGGER
                fltlog_error(LOGIDNONE "Couldn't save md5sum array to storage %s.", args.outfile);
#endif // WITH_LOGGER
                res = 1;
                goto load_finish;
            }

            if(fstorage_close(fs))
            {
#ifdef WITH_LOGGER
                fltlog_error(LOGIDNONE "Couldn't close storage %s.", args.outfile);
#endif // WITH_LOGGER
                res = 1;
                goto load_finish;
            }

            load_finish:

                fstorage_destroy(fs);
        }
        else
#endif // WITHOUT_FSTORAGE
        {

	        if(0!=md5sigsave(sa,args.outfile))
            {
#ifdef WITH_LOGGER
                	fltlog_error(LOGIDNONE "open/write");
#endif // WITH_LOGGER
            }
        }
    }
    else
    {
#ifdef WITH_LOGGER
        fltlog_error(LOGIDNONE "open/read");
#endif // WITH_LOGGER
        res = 1;
    }
/* finish: */
#ifdef WITH_LOGGER
    logger_close();    
#endif // WITH_LOGGER
    return res;
}

int parse_command_line (int ac, char *av[], params_t *args)
{
    int i = 1;
    if( NULL == args || NULL == av )
        return 1;

    memset (&args, 0, sizeof (params_t));
    args->sort_mode = SORT_LIST; // default value

    if (i < ac && 0 == strcmp ("--print-version", av[i]))
    {
        args->do_print_version = 1;
        return 0;
    }

    if ( i < ac && 0 == strcmp ("-nosort", av[i]) ) // old mksigdb keep -nosort as first argument
    {
        args->sort_mode = DONT_SORT_LIST;
        i ++;
    }

    if ( i < ac )
        args->infile = av[i++];

    if ( i < ac && 0 == strcmp ("-fs", av[i]) )
    {
        args->is_fstorage = 1;
        i ++;
    }

    if ( i < ac )
        args->outfile = av[i++];

    while (i < ac)
    {
        if (i < ac && 0 == strcmp ("-s", av[i]) )
        {
            args->is_section = 1;
            i ++;
            if ( i < ac && 0 == strcmp ("black", av[i]) )
                args->section_type = S_BLACK;
            else if ( i < ac && 0 == strcmp ("white", av[i]) )
                args->section_type = S_WHITE;
            else if ( i < ac && 0 == strcmp ("main", av[i]) )
                args->section_type = S_MAIN;
            else
            {
                printf ("-s option needs one of main|black|white options.\n");
                return 1;
            }
        }
        else if ( i < ac && 0 == strcmp ("-a", av[i]) )
            args->add_mode = 1;
        else if ( i < ac && 0 == strcmp ("-nosort", av[i]) )
            args->sort_mode = DONT_SORT_LIST;
        else
        {
            printf ("Wrong option %s\n", av[i]);
            return 1;
        }

        i ++;
    }

    if (NULL == args->infile)
    {
        printf("infine undefined\n");
        return 1;
    }
    if (NULL == args->outfile)
    {
        printf("outfile undefined\n");
        return 1;
    }

    return 0;
}
