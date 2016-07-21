/*******************************************************************************
 * Project: Content Filtration Library                                         *
 * (C) 2001-2003, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: filter.h                                                              *
 * Created: Fri Nov 09 13:07:50 2001                                           *
 * Desc: API for content filtration library.                                   *
 *******************************************************************************/

/**
 * \file  filter.h
 * \brief API for content filtration library.
 */

#ifndef __cf_filter_h__
#define __cf_filter_h__

#include "relevant.h"
#include "categories.h"
#include "message.h"
#include "language.h"

#ifdef WIN32
#pragma pack(push, 8)
#else
#pragma pack(8)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct __content_filter content_filter;

#define FILTER_NONBLOCK 0x00000001
#define FILTER_DEFAULTS 0
    
    /**
     * \brief Open filter base.
     *
     * \return NULL if open fails. Non-null pointer must 
     *         be released with filter_close() call.
     */
    content_filter* filter_open(const char* name);
  
    /**
     * \brief Determine list of relevant categories for the message.
     * 
     * \param cf --- opened filter.
     * \param m --- message.
     * \param fr --- filter_result structure for reuse (can be NULL).
     *
     * \return non-NULL pointer (must be relesed with filter_result_free() call
     *         at end of checking process) or NULL at fatal memory problems.
     */
    filter_result*  filter_check(content_filter* cf, message* m, filter_result* fr, unsigned int flags);
    
    /**
     * \brief Close opened filter base.
     */
    int            filter_close(content_filter* cf, unsigned int flags);
    
    /** 
     * \brief Returns rubricator of the opened content filter database.
     */
    const rubricator*  filter_get_rubricator(content_filter* cf);


#define FILTER_FORMAT_AUTO   0x00000000UL
#define FILTER_FORMAT_TEXT   0x00010000UL
#define FILTER_FORMAT_HTML   0x00020000UL
#define FILTER_FORMAT_XML    0x00030000UL
#define FILTER_FORMAT_RTF    0x00040000UL
#define FILTER_FORMAT_MSWORD 0x00050000UL

/*
 * Mask for supposed codepage (see language.h, CP_* constants),
 * set CP_UNKNOWN if auto-detect is needed.
 */
#define FILTER_CP_MASK       0x0000FFFFUL
#define FILTER_FORMAT_MASK   0x00FF0000UL

    filter_result*  filter_check_text(content_filter *cf, const unsigned char *data, size_t length, 
				      unsigned int helpers, filter_result *fr, unsigned int flags);

    filter_result*  filter_check_url(content_filter *cf, const char *url, int url_length, 
				     filter_result *fr, unsigned int flags);


    
#ifdef __cplusplus
}
#endif

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif /* __cf_filter_h__ */

/*
 * <eof filter.h>
 */
