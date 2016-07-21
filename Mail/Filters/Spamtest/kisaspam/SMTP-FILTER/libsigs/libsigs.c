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


#include "libsigs.h"

#ifdef WITH_LOGGER
#include "smtp-filter/common/logger.h"
#endif // WITH_LOGGER

/* 
 * Logger settings 
 */
#define MY_LOG_FILEID    "030"
#define MY_LOG_MAXMSGID  "0000"

/*
#ifdef _MSC_VER
static 
#else
*/

/* inline  */
/*
#endif
*/

int md5sigcmp(const md5sum a, const md5sum b)
{
    int i;
    for(i=0;i<16 && a[i] == b[i]; i++)
	;
    if(i==16) return 0;
    else return a[i]-b[i];
}

int qmd5sigcmp(const void * a, const void * b)
{
#define A ((unsigned char *)a)
#define B ((unsigned char *)b)

    int i;
    for(i=0;i<16 && A[i] == B[i]; i++)
	;
    if(i==16) return 0;
    else return A[i]-B[i];

#undef A
#undef B
}


static int do_sigfind(md5sum *array,int lo, int hi,md5sum what)
{
    int idx;
    if(!md5sigcmp(array[lo],what))
	return lo;
    if(!md5sigcmp(array[hi],what))
	return hi;
    idx = (lo+hi)/2;
    if(idx == lo) return -1; // only 1 or 2 members remain
    if(md5sigcmp(array[idx],what)>0)
	return do_sigfind(array,lo,idx,what);
    else
	return do_sigfind(array,idx,hi,what);
}

int md5sigfind(md5sigarray *sa, md5sum what)
{
    return do_sigfind(sa->array,0,sa->count-1,what);
}

void md5sigdump(md5sigarray *sa)
{
    size_t i,j;
    for(i=0;i<sa->count; i++)
	{
	    char mdbuf[33];
	    static const char hex[]="0123456789abcdef";
	    for(j=0;j<16;j++)
		{
		    mdbuf[j+j] = hex[sa->array[i][j]>>4];
		    mdbuf[j+j+1] = hex[sa->array[i][j]&0xf];
		}
	    mdbuf[32]='\0';
	    printf("%s\n",mdbuf);
	}
}

void str2md5(unsigned char to[16], char *str)
{
    int i;
    char sbuf[5],*p;
    sbuf[0] = '0';
    sbuf[1] = 'x';
    sbuf[4] = '\0';
    for(i=0;i<16;i++)
	{
	    sbuf[2]=str[i+i];
	    sbuf[3]=str[i+i+1];
	    to[i] = (unsigned char)strtol(sbuf,&p,16);
	}
}

void md52str(char to[33], unsigned char *from)
{
    static const char hex[]="0123456789abcdef";
    int j;
    for(j=0;j<sizeof(md5sum);j++)
	{
	    to[j+j] = hex[from[j]>>4];
	    to[j+j+1] = hex[from[j]&0xf];
	}
    to[32]='\0';
}

md5sigarray * mkmd5siglist(char *filename)
{
    return mkmd5siglistex(filename, SORT_LIST);
}

md5sigarray * mkmd5siglistex(char *filename, sort_list_t sort_flag)
{
    FILE *f;
    struct stat st;
    off_t projalloc;
    md5sigarray *ret;
    char sbuf[64];
    MD5_CTX ctx;

    if(stat(filename,&st)<0)
	return NULL;
    projalloc = (st.st_size+31)/32;
    f = fopen(filename,"r");
    if(!f) return NULL;
    if(NULL==(ret = malloc(sizeof(md5sigarray))))
	{
	    fclose(f); return NULL;
	}
    bzero(ret, sizeof(md5sigarray));
    if(NULL==(ret->array = malloc(sizeof(md5sum)*(projalloc+1))))
	{
            free(ret);
	    fclose(f); return NULL;
	}
    while(fgets(sbuf,64,f))
	{
	    str2md5(ret->array[ret->count++],sbuf);
	}
    if (SORT_LIST == sort_flag)
        qsort(ret->array,ret->count,sizeof(md5sum),qmd5sigcmp);
    ret->magic = SIGMAGIC;
#  ifndef LIBSIGS_DONT_USE_MMAP
    ret->mmaped_array_size = 0; /* malloc'ed not mmaped */
    ret->mmaped_array = NULL; /* malloc'ed not mmaped */
#  endif
    MD5Init(&ctx);
    MD5Update(&ctx,(void*)ret->array,sizeof(md5sum)*ret->count);
    MD5Final(ret->cksum,&ctx);
    return ret;
}

int md5sigsave(md5sigarray *sa, char *filename)
{
    int file  = _open(filename,O_RDWR|O_TRUNC|O_CREAT|O_BINARY,0644);
    if(file<0) return -1;
    _write(file,sa,sizeof(md5sigarray_fileheader));
    _write(file,sa->array,sizeof(md5sum)*(sa->count));
    _close(file);
    return 0;
}

#ifdef LIBSIGS_DONT_USE_MMAP
static int forcibly_read(int fd, void *buf, size_t size)
{
    size_t offset = 0;
    int rc;
    while(offset<size) {
        int to_read = size-offset;
        if(to_read>4096) to_read = 4096;
        rc = _read(fd, (void*)(((char*)buf)+offset), to_read);
        if(rc<1) return 0;
        offset+=rc;
    }
    return 1;
}
#endif


md5sigarray *loadmd5sig(char *filename)
{
    int file  = _open(filename,O_RDONLY|O_BINARY);
    size_t rs;
#if defined(OS_LINUX) && !defined(LIBSIGS_DONT_USE_MMAP)
    int psize = getpagesize();
#endif
    md5sigarray_fileheader header;
    md5sigarray *ret;
    /* struct stat st; */
    if(file<0) return NULL;

    _read(file, &header, sizeof(header));
    ret = (md5sigarray*)malloc(sizeof(md5sigarray));
    ret->magic = header.magic;
    ret->count = header.count;
    memcpy(ret->cksum, header.cksum, sizeof(md5sum));

    rs = header.count * sizeof(md5sum);

#if !defined(LIBSIGS_DONT_USE_MMAP)
#if defined(OS_LINUX)
    rs = ((rs+sizeof(header))/psize+1)*psize;
#else
    rs += sizeof(header);
#endif
#endif

#ifdef LIBSIGS_DONT_USE_MMAP
    /*ret->mmaped_array_size = 0;*/
    ret->array = (md5sum*)malloc(rs);
    if(!forcibly_read(file, ret->array, rs)) 
        {
            _close(file);
            free(ret->array);
            free(ret);
            return NULL;
        }
    _close(file);
    if((ret->magic!=SIGMAGIC)||(md5checkdata(ret)!=0))
	{
	    free(ret->array);
            free(ret);
	    return NULL;
	}
#else
    ret->mmaped_array_size = rs;
    ret->mmaped_array = mmap(NULL,rs,PROT_READ,MAP_SHARED,file,0);
    ret->array = (md5sum*)((char*)(ret->mmaped_array)+sizeof(header));
    close(file);
    if(ret->array == MAP_FAILED) 
        {
            free(ret);
            return NULL;
        }
    if((ret->magic!=SIGMAGIC)||(md5checkdata(ret)!=0))
	{
	    munmap(ret,rs);
            free(ret);
	    return NULL;
	}
#endif
    return ret;
}

void unloadmd5sig(md5sigarray *sa)
{
#ifndef LIBSIGS_DONT_USE_MMAP
    if(sa->mmaped_array) {
        munmap(sa->mmaped_array, sa->mmaped_array_size);
    } else {
        free(sa->array);
    }
#else
    free(sa->array);
#endif    
    free(sa);
}

int md5checkdata(md5sigarray *sa)
{
    unsigned char cksum[16];
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx,(void*)sa->array,sizeof(md5sum)*sa->count);
    MD5Final(cksum,&ctx);
    if(memcmp(cksum,sa->cksum,16))
	return -1;
    return 0;
}

void md5sigsort(md5sigarray *sa)
{
    qsort(sa->array,sa->count,sizeof(md5sum),md5sigcmp);
}


#ifdef TEST_CREATE
int main()
{
    md5sigarray *sa = mkmd5siglist("/home/lexa/mdlist");
    md5sigsave(sa,"/home/lexa/md.data");
    return 0;
}
#endif
#ifdef TEST_BENCH
#include <sys/time.h>
#define NS 1000000
int main()
{
    long usec;
    double each;
    struct timeval tv1,tv2;
    int i,idx,r;
    unsigned char md5[16];

    md5sigarray *sa = loadmd5sig("/home/lexa/md.data");
    if(sa==NULL) return 1;

    gettimeofday(&tv1,NULL);
    for(i = 0; i< NS; i++)
	{
	    idx = random()%sa->count;
	    memcpy(md5,sa->array[idx],16);
	    r = md5sigfind(sa,md5);
	    if(r==-1)
		printf("Not found: %d %d %d!\n",i,idx,r);
	}
    gettimeofday(&tv2,NULL);
    usec = 1000000L*(tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec);
    each = 1000000.0/(usec/NS);
    printf ("%ld usec for %d searches, %6.3g per sec\n",usec,NS,each);

    return 0;
}
#endif
