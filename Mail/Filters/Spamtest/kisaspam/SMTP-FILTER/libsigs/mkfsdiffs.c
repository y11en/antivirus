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

#include "gnu/md5a/md5a.h"


md5sigarray* init_md5array(u_int32_t max_count)
{
    md5sigarray* sa;

    if ( NULL == (sa = malloc(sizeof(md5sigarray))) )
        return NULL;
    memset(sa, 0, sizeof(md5sigarray));
    if ( NULL == (sa->array = malloc(sizeof(md5sum)*(max_count))))
        return NULL;
    memset(sa->array, 0, sizeof(md5sum)*(max_count));

    sa->count = max_count;
    sa->magic = SIGMAGIC;
    return sa;
}

int sig_md5array(md5sigarray* sa)
{
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx,(void*)sa->array,sizeof(md5sum)*sa->count);
    MD5Final(sa->cksum,&ctx);
    return 0;    
}

int build_md5diffs(md5sigarray* sa1, md5sigarray* sa2, md5sigarray* sa_white, md5sigarray* sa_black)
{
    unsigned int i1 = 0, i2 = 0, iw = 0, ib = 0;
    
    // sort sa1, sa2 to make sure
    qsort(sa1->array,sa1->count,sizeof(md5sum),qmd5sigcmp);
    qsort(sa2->array,sa2->count,sizeof(md5sum),qmd5sigcmp);

    while (i1 < sa1->count && i2 < sa2->count )
    {
        while( (i1 < sa1->count && i2 < sa2->count ) && 
                qmd5sigcmp(sa1->array[i1], sa2->array[i2]) == 0)
            { i1 ++; i2 ++; }
        while( i1 < sa1->count && 
                qmd5sigcmp(sa1->array[i1], sa2->array[i2]) < 0 )
            memcpy(sa_white->array[iw++], sa1->array[i1++], sizeof(md5sum));
        while( i2 < sa2->count && 
                qmd5sigcmp(sa1->array[i1], sa2->array[i2]) > 0 )
            memcpy(sa_black->array[ib++], sa2->array[i2++], sizeof(md5sum));
    }
    while (i1 < sa1->count)
        memcpy(sa_white->array[iw++], sa1->array[i1++], sizeof(md5sum));
    while (i2 < sa2->count)
        memcpy(sa_black->array[ib++], sa2->array[i2++], sizeof(md5sum));
    sa_white->count = iw;
    sa_black->count = ib;
    return 0;
}

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
    int max_count = 0;

    if(ac!=4)
    {
        printf("Usage: %s infile_1 infile_2 outfile\n",av[0]);
        //        logger_close();
        exit(1);
    }
        
    fs1     = fstorage_create();
    fs2     = fstorage_create();
    fs_out  = fstorage_create();
    
    memset(&fscc, 0, sizeof(fscc));
    
    fscc.pid = FSTORAGE_PID_CFLIB;
    
    if( FSTORAGE_OK != fstorage_connect(fs1, av[1], O_RDONLY, 0644, FSTORAGE_OPEN_READ_ONLY, &fscc) ||
        FSTORAGE_OK != fstorage_connect(fs2, av[2], O_RDONLY, 0644, FSTORAGE_OPEN_READ_ONLY, &fscc) ||
        FSTORAGE_OK != fstorage_connect(fs_out, av[3], O_CREAT | O_TRUNC | O_RDWR, 0644, FSTORAGE_OPEN_READ_WRITE, &fscc)
        )
        {res = 1; goto load_finish; }
    memset(&fscc, 0, sizeof(fscc));
    fscc.pid = FSTORAGE_PID_CFLIB;
    
    if( 0 == ( sa1 = fstorage_loadmd5sig(fs1, FSTORAGE_SECTION_GSG) ) ||
        0 == ( sa2 = fstorage_loadmd5sig(fs2, FSTORAGE_SECTION_GSG) )
        )
        {res = 1; goto unload_md5arrays;}

    max_count = max(sa1->count, sa2->count);
    if ( 0 == (sa_white = init_md5array(max_count) ) ||
         0 == (sa_black = init_md5array(max_count) )
            )
        {res = 1; goto unload_md5arrays;}

    build_md5diffs(sa1, sa2, sa_white, sa_black);
    sig_md5array(sa_white);
    sig_md5array(sa_black);


    if( 0 != fstorage_md5sigsave(fs_out, sa_white, FSTORAGE_SECTION_GSG_UPDATE_WHITE) ||
        0 != fstorage_md5sigsave(fs_out, sa_black, FSTORAGE_SECTION_GSG_UPDATE_BLACK) )
        {res = 1;   goto unload_md5arrays;}

unload_md5arrays:
    
    if (sa1->array)
        fstorage_unloadmd5sig(sa1);
    if (sa2->array)
        fstorage_unloadmd5sig(sa2);
    if (sa_white->array)
        unloadmd5sig(sa_white);
    if (sa_black->array)
        unloadmd5sig(sa_black);
    
    if(fstorage_close(fs1))
        res = 1;
    if(fstorage_close(fs2))
        res = 1;
    if(fstorage_close(fs_out))
        res = 1;
    if (res)
        goto load_finish;

load_finish:
    
    fstorage_destroy(fs1);
    fstorage_destroy(fs2);
    fstorage_destroy(fs_out);
    /* finish: */
    if (res)
        printf("was errors");
    return res;
}
