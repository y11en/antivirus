/* -*- C -*-
 * File: libsigs.c
 *
 * Created: Wed Jul 16 21:38:35 2003
 */

#include <sys/types.h>
#include <sys/stat.h>
#ifdef _MSC_VER
#include <io.h>
#else  // UNIX/Linux
#include <sys/mman.h>
#include <unistd.h>
#define O_BINARY 0
#endif //_MSC_VER
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "_include/ntoh.h"

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif


#ifdef USE_CUSTOM_HEAP
#include <win32-filter/include/mem_heap.h>
#endif


#if 0
#ifdef OS_LINUX
#define PROTOTYPES 1
#include <md5global.h>
#endif
#include <md5.h>
#else
# include "gnu/md5a/md5a.h"
#endif

#include "_include/ntoh.h"

#include "libsigs.h"
#include "libsigs-fst.h"

/* #include "smtp-filter/common/logger.h" */

/* 
 * Logger settings 
 */
#define MY_LOG_FILEID    "030"
#define MY_LOG_MAXMSGID  "0000"

#define SIGS_HEADER_SIZE 24

static int sigs_header_load(md5sigarray_fileheader *header, void *data, size_t size)
{
    int res = -1;
    unsigned char *p = NULL;

#define RETURN  do { goto finish; } while(0)

    if(!data)
        RETURN;

    if(size < SIGS_HEADER_SIZE)
        RETURN;

    p = (unsigned char *)data;

    header->magic = ntohl(*((u_int32_t *)p));
    p += sizeof(u_int32_t);

    if(header->magic != SIGMAGIC)
        RETURN;

    header->count = ntohl(*((u_int32_t *)p));
    p += sizeof(u_int32_t);

    memcpy(header->cksum, p, sizeof(header->cksum));

    res = 0;

#undef RETURN
  finish:

    return res;
}

static int sigs_header_save(md5sigarray_fileheader *header, void *data, size_t size)
{
    int res = -1;
    unsigned char *p = NULL;

#define RETURN do { goto finish; } while(0)

    if(!data)
        RETURN;

    if(size < SIGS_HEADER_SIZE)
        RETURN;

    p = (unsigned char *)data;

    *((u_int32_t *)p) = htonl(header->magic);
    p += sizeof(u_int32_t);

    *((u_int32_t *)p) = htonl(header->count);
    p += sizeof(u_int32_t);

    memcpy(p, header->cksum, sizeof(header->cksum));

    res = 0;

#undef RETURN
  finish:

    return res;
}


md5sigarray *fstorage_loadmd5sig(fstorage *fs, fstorage_section_id sid /*= FSTORAGE_SECTION_GSG*/)
{
    md5sigarray_fileheader header;
    md5sigarray *ret = NULL;
    fstorage_section *fss = NULL;
    
    size_t section_size;

    if(fstorage_load_section(fs, sid))
    {
        // fltlog_error(LOGIDNONE "GSG: load: can't load section 0x%08X!", sid);
	/* @MSGDESC
	 * Can't load specified file strorage section with GSG signatures
	 * 
	 * Skip GSG checks */
        return NULL;
    }

    fss = fstorage_find_section(fs, sid);
    if(!fss)
    {
        // fltlog_error(LOGIDNONE "GSG: load: can't find section 0x%8X!", sid);
	/* @MSGDESC
	 * Can't find specified file strorage section with GSG signatures
	 * 
	 * Skip GSG checks */
        return NULL;
    }

    section_size = fstorage_section_get_size(fss);
    
    if(section_size == 0)
    {
        // fprintf(stderr, "Section size = 0\n", sid);
        // fltlog_error(LOGIDNONE "GSG: load: section(0x%8X) size = 0 !", sid);
	/* @MSGDESC
	 * Specified file strorage section with GSG signatures is empty
	 * 
	 * Possible GSG checksum base is corrupted of just empty. Skip GSG checks */

        return NULL;
    }

    if(sigs_header_load(&header, fstorage_section_get_all_data(fss), 
                        fstorage_section_get_size(fss))) 
        {
            return NULL;
        }


    ret = (md5sigarray*)malloc(sizeof(md5sigarray));

    ret->magic = header.magic;
    ret->count = header.count;
    memcpy(ret->cksum, header.cksum, sizeof(md5sum));
    
    ret->array = (md5sum *)((char*)fstorage_section_get_all_data(fss) + SIGS_HEADER_SIZE);

    if((ret->magic!=SIGMAGIC)||(md5checkdata(ret)!=0))
    {
        // fltlog_error(LOGIDNONE "GSG: load: wrong checksum of gsg signatures", sid);
	/* @MSGDESC
	 * Wrong checksum of gsg signatures base.
	 * 
	 * Possible GSG checksum base is corrupted. Skip GSG checks */
        free(ret);
        return NULL;
    }

    return ret;
}

int fstorage_md5sigsave(fstorage *fs, md5sigarray *sa, fstorage_section_id sid/* = FSTORAGE_SECTION_GSG*/)
{
    fstorage_section *fss = NULL;
    size_t section_size;
    md5sigarray_fileheader header;

    section_size = SIGS_HEADER_SIZE + sa->count*sizeof(md5sum);

    fss = fstorage_get_section(fs, sid);

    if(!fss)
        return -1;

    if(fstorage_section_realloc(fss, section_size))
        return -1;
    
    memset(&header, 0, sizeof(header));

    header.magic = sa->magic;
    header.count = sa->count;
    memcpy(header.cksum, sa->cksum, sizeof(header.cksum));

    if(sigs_header_save(&header, fstorage_section_get_all_data(fss), 
                        fstorage_section_get_size(fss)))
        {
            return -1;
        }


    memcpy((char*)fstorage_section_get_all_data(fss) + SIGS_HEADER_SIZE,
              sa->array, sizeof(md5sum)*sa->count);

    return 0;
}

void fstorage_unloadmd5sig(md5sigarray *sa)
{
    if(sa)
        free(sa);
}

/*
    fs      - should be opened in write mode
    upd_fs  - read mode
*/
int fstorage_updatemd5sig(fstorage *res_fs, fstorage *old_fs, size_t upd_count, fstorage **upd_fs)
{
    size_t sum_size;
    int found, cmp_res, i;
    MD5_CTX ctx;        
    int curr_sig;

    md5sigarray_fileheader header, res_header;

    md5sum *add_ptr = NULL;
    /* md5sum *del_ptr = NULL; */

    //Base sections
    fstorage_section *res_fss = NULL;
    md5sum *res_ptr = NULL;
    
    size_t old_fss_size;
    size_t old_fss_pos;
    md5sum *old_ptr;
    fstorage_section *old_fss = NULL;
    
    //Update sections
    /* size_t upd_black_count = 0; */
    size_t upd_black_size0  [MAX_GSG_UPDATE_FILES];    //size
    size_t upd_black_pos0   [MAX_GSG_UPDATE_FILES];     //curr_pos
    md5sum *upd_black_ptr0  [MAX_GSG_UPDATE_FILES];

    size_t *upd_black_size  = upd_black_size0 + 1;    //size
    size_t *upd_black_pos   = upd_black_pos0  + 1;     //curr_pos
    md5sum **upd_black_ptr  = upd_black_ptr0  + 1;
    
    fstorage_section *fss_upd_black[MAX_GSG_UPDATE_FILES];
    
    /* size_t upd_white_count = 0; */
    size_t upd_white_size[MAX_GSG_UPDATE_FILES];
    size_t upd_white_pos[MAX_GSG_UPDATE_FILES];
    md5sum *upd_white_ptr[MAX_GSG_UPDATE_FILES];
    fstorage_section *fss_upd_white[MAX_GSG_UPDATE_FILES];

    //Get main GSG sections
    res_fss = fstorage_section_create(res_fs, FSTORAGE_SECTION_GSG);
    if(!res_fss)
        return -1;
    
    if(fstorage_load_section(old_fs, FSTORAGE_SECTION_GSG))
    {
        return -1;
    }
    
    old_fss = fstorage_find_section(old_fs, FSTORAGE_SECTION_GSG);
    if(!old_fss)
        return -1;
    
    //Read old headers (size and magic summs)

    if(sigs_header_load(&header, fstorage_section_get_all_data(old_fss), 
                        fstorage_section_get_size(old_fss)))
        return -1;
    
    old_fss_size = header.count;
    old_fss_pos = 0;
    old_ptr = (md5sum *)((char*)fstorage_section_get_all_data(old_fss) + SIGS_HEADER_SIZE);

    upd_black_size[-1]  = old_fss_size;
    upd_black_pos[-1]   = old_fss_pos;
    upd_black_ptr[-1]   = old_ptr;

    sum_size = old_fss_size;
    
    //Get update GSG sections
    for(i = 0; i < (int)upd_count; i++)
    {
        fstorage_load_section(upd_fs[i], FSTORAGE_SECTION_GSG_UPDATE_BLACK);
        fss_upd_black[i] = fstorage_find_section(upd_fs[i], FSTORAGE_SECTION_GSG_UPDATE_BLACK);
        
        fstorage_load_section(upd_fs[i], FSTORAGE_SECTION_GSG_UPDATE_WHITE);
        fss_upd_white[i] = fstorage_find_section(upd_fs[i], FSTORAGE_SECTION_GSG_UPDATE_WHITE);
    }


    for(i = 0; i < (int)upd_count; i++)
    {
        if(fss_upd_black[i])
        {
            if(sigs_header_load(&header, fstorage_section_get_all_data(fss_upd_black[i]), 
                                fstorage_section_get_size(fss_upd_black[i])))
                return -1;
            
            upd_black_size[i] = header.count;    
            upd_black_ptr[i] = (md5sum *)((char*)fstorage_section_get_all_data(fss_upd_black[i]) + SIGS_HEADER_SIZE);
            sum_size+=header.count;
        }
        else
        {
            upd_black_size[i] = 0;    
            upd_black_ptr[i] = NULL;
        }
        upd_black_pos[i] = 0;
        
        if(fss_upd_white[i])
            {
                if(sigs_header_load(&header, fstorage_section_get_all_data(fss_upd_white[i]), 
                                    fstorage_section_get_size(fss_upd_white[i])))
                    return -1;
                
                upd_white_size[i] = header.count;    
                upd_white_ptr[i] = (md5sum *)((char*)fstorage_section_get_all_data(fss_upd_white[i]) + SIGS_HEADER_SIZE);
                sum_size+=header.count;
            }
        else
            {
                upd_white_size[i] = 0;  
                upd_white_ptr[i] = NULL;            
            }
        upd_white_pos[i] = 0;
    }

    //Resize result section
    if(fstorage_section_realloc(res_fss,sum_size*sizeof(md5sum)+SIGS_HEADER_SIZE))
    {
        return -1;
    }
    //Write header

       memset(&res_header, 0, sizeof(res_header));
       res_header.magic = SIGMAGIC;
       res_header.count = 0;

    res_ptr = (md5sum *)((char*)fstorage_section_get_all_data(res_fss) + SIGS_HEADER_SIZE);
   
    while(1)
    {
        //Find next md5sum for addition (the least one) 
        curr_sig = -2;
        add_ptr = NULL;
        for(i = upd_count - 1; i >= -1; i--)
        {
            if(upd_black_pos[i] < upd_black_size[i])
            {
                if(curr_sig == -2)
                {
                    add_ptr = upd_black_ptr[i];
                    curr_sig = i;
                }
                else if((cmp_res = md5sigcmp(*add_ptr,*upd_black_ptr[i])) > 0) 
                {
                    add_ptr = upd_black_ptr[i];
                    curr_sig = i;
                }
                else if(cmp_res == 0) //skip if found the same md5sum
                {
                    upd_black_ptr[i]++;
                    upd_black_pos[i]++;
                }
            }
        }
        //Skip it or exit if it is not exist
        if(curr_sig == -2)
            break;
        else
        {
            upd_black_ptr[curr_sig]++;
            upd_black_pos[curr_sig]++;
        };

        //Check the md5sum found in white lists
        found = 0;
       // int cmp_res;
        for(i = 0; (i < (int)upd_count) && (found == 0); i++)
        {
            while(upd_white_pos[i] < upd_white_size[i])
            {
                if((cmp_res = md5sigcmp(*add_ptr,*upd_white_ptr[i])) > 0)
                {
                    upd_white_pos[i]++;
                    upd_white_ptr[i]++;
                }
                else
                {
                    if(cmp_res == 0 && i >= curr_sig)
                        found = 1;
                    break;                                            
                }
            }
        }

        //If not exist in white lists then add it into result list
            //increment counter
        if(!found)
        {
            memcpy(res_ptr, add_ptr, sizeof(md5sum));
            res_ptr++;
            res_header.count++;
        }
    }
    
    //Calculate checksum
    MD5Init(&ctx);
    MD5Update(&ctx,((char*)fstorage_section_get_all_data(res_fss)+SIGS_HEADER_SIZE),sizeof(md5sum)*res_header.count);
    MD5Final(res_header.cksum, &ctx);

       if(sigs_header_save(&res_header, fstorage_section_get_all_data(res_fss), 
                           fstorage_section_get_size(res_fss)))
           return -1;

    //Correct section size
    if(fstorage_section_realloc(res_fss, res_header.count*sizeof(md5sum)+SIGS_HEADER_SIZE))
    {
        return -1;
    }

    return 0;
}

