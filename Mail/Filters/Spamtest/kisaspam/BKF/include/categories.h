/*******************************************************************************
 * Proj: Content Filtration Library, version 2                                 *
 * --------------------------------------------------------------------------- *
 * File: categories.h                                                          *
 * Created: Sun Jan 16 18:05:41 2005                                           *
 * Desc: New API to catalogue.                                                 *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  categories.h
 * \brief New API to catalogue.
 */

#ifndef __cf_categories_h__
#define __cf_categories_h__

#include "filter.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct tag_cf_category flt_category;

    const flt_category *flt_category_first(const content_filter *cf);
    const flt_category *flt_category_find(const content_filter *cf, cf_category_id cid);

    const flt_category *flt_category_parent(const flt_category *c);
    const flt_category *flt_category_brother(const flt_category *c);
    const flt_category *flt_category_first_son(const flt_category *c);

    const char        *flt_category_get_text_id(const flt_category *c);
    const char        *flt_category_get_title(const flt_category *c);
    cf_category_id     flt_category_get_id(const flt_category *c);
    int                flt_category_is_hidden(const flt_category *c);

    int                flt_category_get_rel_limit(const flt_category *c);
    int                flt_category_get_diff_limit(const flt_category *c);

#ifdef __cplusplus
}
#endif

#endif /* __cf_categories_h__ */

/*
 * <eof categories.h>
 */
