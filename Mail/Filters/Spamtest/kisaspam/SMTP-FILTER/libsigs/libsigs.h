/* -*- C -*-
 * File: libsigs.h
 *
 * Created: Fri Jul 18 13:08:45 2003
 */
#ifdef _WIN32
#define LIBSIGS_DONT_USE_MMAP
#else
#undef LIBSIGS_DONT_USE_MMAP
#endif //_WIN32

#ifndef __libsigs_h
#define __libsigs_h

#include <sys/types.h>
#ifdef _WIN32
#include "_include/nix_types.h"
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum {SORT_LIST, DONT_SORT_LIST} sort_list_t;
    typedef unsigned char md5sum[16];

#define SIGMAGIC 0xaf761419

    typedef struct 
    {
	u_int32_t magic;
	u_int32_t count;
	md5sum    cksum;
    } md5sigarray_fileheader;
    
    typedef struct 
    {
	u_int32_t magic;
	u_int32_t count;
	md5sum    cksum;
#      ifndef LIBSIGS_DONT_USE_MMAP
        size_t    mmaped_array_size; /* zero if malloc'ed */
        void      *mmaped_array;     /* NULL if malloc'ed */
#      endif
	md5sum    *array;
    } md5sigarray;
    
    int md5sigcmp(const md5sum a, const md5sum b);
    int qmd5sigcmp(const void * a, const void * b);
    int md5sigfind(md5sigarray *sa, md5sum what);
    void md5sigdump(md5sigarray *sa);
    void str2md5(unsigned char to[16], char *str);
    void md52str(char to[33], unsigned char *from);
    md5sigarray * mkmd5siglist(char *filename);
    md5sigarray * mkmd5siglistex(char *filename, sort_list_t sort_flag);
    int md5sigsave(md5sigarray *sa, char *filename);
    md5sigarray *loadmd5sig(char *filename);
    void unloadmd5sig(md5sigarray *sa);
    int md5checkdata(md5sigarray *sa);
    void md5sigsort(md5sigarray *sa);

   
#ifdef __cplusplus
}
#endif

#endif







