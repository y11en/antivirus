/* -*- C -*-
 * File: mksigdb.c
 *
 * Created: Sun Jul 20 23:48:41 2003
 */
#include <stdio.h>
#include <process.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "libsigs.h"
#ifndef WITHOUT_FSTORAGE
#include "libsigs-fst.h"
#endif

#include "gnu/md5a/md5a.h"

int main(int ac, char *av[])
{
    int res = 0;
    md5sigarray *sa1 = NULL;
    md5sigarray *sa2 = NULL;
    md5sigarray *sa_black = NULL;
    md5sigarray *sa_white = NULL;
    fstorage *fs1 = NULL;
    fstorage *fs2 = NULL;
    fstorage *fs_out = NULL;
    struct fstorage_connect_config fscc;
    int max_len = 0;
    FILE *f;
    int i;
    char s [33];

    if(ac!=4)
    {
        printf("Usage: %s infile_1 out_file -m|w|b\n",av[0]);
        //        logger_close();
        exit(1);
    }
        
    fs1     = fstorage_create();
    
    memset(&fscc, 0, sizeof(fscc));
    
    fscc.pid = FSTORAGE_PID_CFLIB;
    
    if( fstorage_connect(fs1, av[1], O_RDONLY, 0644, FSTORAGE_OPEN_READ_ONLY, &fscc)
        )
        {res = 1; goto load_finish; }
    memset(&fscc, 0, sizeof(fscc));
    fscc.pid = FSTORAGE_PID_CFLIB;
    
    if( 0 == ( sa1 = fstorage_loadmd5sig(fs1, 
        av[3][1] == 'b' ? FSTORAGE_SECTION_GSG_UPDATE_BLACK :
        av[3][1] == 'w' ? FSTORAGE_SECTION_GSG_UPDATE_WHITE :
            FSTORAGE_SECTION_GSG) )
        )
        {res = 1; goto unload_md5arrays;}

    f = fopen(av[2], "wb" );

    for (i = 0; i < (int)sa1->count; i ++)
    {
        md52str(s,sa1->array[i]);
        fprintf(f, "%s\n", s);
    }

    fclose(f);
unload_md5arrays:
    
    if (sa1->array)
        fstorage_unloadmd5sig(sa1);
    
    if(fstorage_close(fs1))
        res = 1;

load_finish:
    
    fstorage_destroy(fs1);
    /* finish: */
    return res;
}
