/*******************************************************************************
 * Proj: Content filtration                                                    *
 * --------------------------------------------------------------------------- *
 * File: sigdb.h                                                               *
 * Created: Sat Nov  3 14:23:45 2001                                           *
 * Desc: Support for signature checking.                                       *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  sigdb.h
 * \brief Support for signature checking.
 */

#ifndef __sigdb_h__
#define __sigdb_h__

#include <stdio.h>

#include <Filtration/letters/letters.h>
#include <Filtration/patterns/clingproc.h>
#include <Filtration/API/message.h>

#include <Filtration/stopdict/stopdict.h>

#ifdef WIN32
#pragma pack(push, 8)
#else
#pragma pack(8)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct _sigdb sigdb;
    typedef struct _sigdb_dociterator sigdb_dociterator;

#define SIGDB_READ  0x1
#define SIGDB_RW    0x2
    
#define DEFAULT_SIGDB_CACHESIZE_MBYTES 2

    sigdb* sigdb_open(const char* name, unsigned int mode, lingproc* lp);
    sigdb* sigdb_open_ex(const char* name, unsigned int mode, lingproc* lp, unsigned int cache_bytes);
    int    sigdb_close(sigdb* sdb);
    int    sigdb_flush(sigdb *sdb);

    typedef unsigned int      sig_cat;
    typedef struct _signature signature;

    /*
     * m may be == NULL, if not needed save message in archive.
     */
    signature* sigdb_sig_create(sigdb* sdb, docimage* di, message* m);

#ifdef WITH_COMPRESSED_SIGNATURES
#define SIGDB_SIG_COMPRESSED 0x00000002UL
#endif 

#define SIGDB_SIG_NOTCUT     0x00000001UL
#define SIGDB_SIG_DEFAULTS   0x00000000UL
    signature* sigdb_sig_create_ex(sigdb* sdb, docimage* di, message* m, unsigned int flags);

    void sigdb_sig_destroy(sigdb* sdb, signature* s);

#ifdef WITH_DATED_SIG
    int sigdb_sig_add_timed(sigdb* sdb, signature* sig, sig_cat catid, 
                            unsigned int time);
#endif

    int sigdb_sig_add(sigdb* sdb, signature* sig, sig_cat catid);

    sigdb_dociterator* sigdb_get_dociterator(sigdb* sdb);

    /**
     * \return non-zero at error.
     */
    int sigdb_dociterator_get_message(sigdb_dociterator* di, message* m, char* buf, size_t* buf_sz);
    unsigned int sigdb_dociterator_get_catid(sigdb_dociterator* didi);

#ifdef WITH_DATED_SIG
    unsigned int sigdb_dociterator_get_time(sigdb_dociterator *didi);
#endif

    /**
     * \return non-zero, if di stays at last document in sigdb.
     */
    int sigdb_dociterator_next(sigdb_dociterator* di);
    void sigdb_dociterator_free(sigdb_dociterator* di);

    typedef struct 
    {
	sig_cat CID;
	double  image;
    } sigdb_cr_entry;
    
    typedef struct
    {
	size_t          alloc;
	size_t          used;
	sigdb_cr_entry* res;
    } sigdb_check_result;
    
    void sigdb_check_free_result(sigdb_check_result* sdbcr);
    
    /*
     * result must be == NULL at first call. sigdb_check realloc data inside it, if needed.
     */
    sigdb_check_result*  sigdb_check(sigdb* sdb, signature* s, sigdb_check_result* result);

    struct sigdb_version_info
    {
	unsigned major, minor, cur_major, cur_minor, compatible;
    };

    int sigdb_get_version_info(const char* name, sigdb_version_info* rvi);

    int sigdb_print(sigdb *sdb, lingproc *lp, docimage* di, signature *s, FILE *f);
    int sigdb_print_image(sigdb *sdb, lingproc *lp, docimage *di, FILE *f);

    int sigdb_extract(sigdb *sdb, lingproc *lp, docimage* di,
	              size_t filtration_size, bool dump, const char* out_file_name);
#ifdef __cplusplus
}
#endif

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif // __sigdb_h__

/*
 * <eof sigdb.h>
 */
