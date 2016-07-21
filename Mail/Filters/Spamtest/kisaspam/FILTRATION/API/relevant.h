/*******************************************************************************
 * Project: Content Filtration Library                                         *
 * (C) 2001-2003, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: relevant.h                                                            *
 * Created: Fri Nov 09 12:36:06 2001                                           *
 * Desc: Datastructures for relevant categories array (results of content      *
 *       recognition)                                                          *
 *******************************************************************************/

/**
 * \file  relevant.h
 * \brief Datastructures for relevant categories array (results of content
 *        recognition)
 */

#ifndef __relevant_h__
#define __relevant_h__

#include <string.h>

#ifdef WIN32
#pragma pack(push, 8)
#else
#pragma pack(8)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * \brief Type of relevance beetwen message and category.
   */
  typedef enum 
  { 
    rt_sample, /*!< By sample matching */
    rt_strict, /*!< By strict character terms */
    rt_fuzzy,  /*!< By semantic image */
    rt_url     /*!< By address (url) of document */
  } relevance_type;

  /**
   * \brief Element of results array
   */
  struct __filter_result_entry
  {
    unsigned int   CID;

    relevance_type rt;
    double         weight;
    unsigned int   relevant_index;
  };
  
  typedef struct __filter_result_entry filter_result_entry;

  /**
   * \brief Error codes of filter_check().
   */
#define FR_OK       0
#define FR_BADPTR   1
#define FR_SIGERROR 2
#define FR_BADCATID 3
#define FR_PATERROR 4
#define FR_MEMALLOC 5
#define FR_BLOCKED  6
#define FR_URLERROR 7

  /**
   * \brief Result of message checking.
   */
  struct __filter_result
  {
    size_t               alloc; /*!< Allocated size of result. For internal use only. */
    size_t               used;  /*!< Amount of entries filled in result (if error_code == FR_OK). */
    filter_result_entry* res;

    unsigned int error_code;    /*!< One of FR_*, FR_OK if normal. */
  };

  typedef struct __filter_result filter_result;

  /**
   * \brief Release memory allocated in filter_result.
   */
  void           filter_result_free(filter_result* fr);

#ifdef __cplusplus
}
#endif

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif // __relevant_h__

/*
 * <eof relevant.h>
 */
