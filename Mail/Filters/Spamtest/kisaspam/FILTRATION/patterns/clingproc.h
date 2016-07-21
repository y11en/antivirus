/*******************************************************************************
 * Proj: Content Filtration                                                    *
 * --------------------------------------------------------------------------- *
 * File: clingproc.h                                                           *
 * Created: Mon Nov 12 18:23:08 2001                                           *
 * Desc: C interface (very simple) to LingProc.                                *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  clingproc.h
 * \brief C interface (very simple) to LingProc.
 */

#ifndef __clingproc_h__
#define __clingproc_h__

enum lp_create_mode
{
  LP_CREATE_READONLY,
  LP_CREATE_READONLY_NOAPPEND, 
  LP_CREATE_NEW,
  LP_CREATE_APPEND
};


#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct __lingproc lingproc;
  typedef struct __docimage docimage;

  lingproc* lp_create(const char* fname, enum lp_create_mode eMode );
  void      lp_destroy(lingproc* lp);
  docimage* lp_process(lingproc* lp, const char* pSubj, size_t ccSubj, const char* pBody, size_t ccBody );
  void      di_destroy(docimage* di);

#ifdef __cplusplus
}
#endif

#endif // __clingproc_h__

/*
 * <eof clingproc.h>
 */
