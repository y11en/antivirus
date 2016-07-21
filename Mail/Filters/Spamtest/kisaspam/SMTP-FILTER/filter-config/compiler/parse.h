/*******************************************************************************
 * Proj: SMTP filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: parse.h                                                               *
 * Created: Mon Apr 22 21:33:36 2002                                           *
 * Desc: Parse file formats (XML input data).                                  *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  parse.h
 * \brief Common interface to parse of file formats.
 */

#ifndef __parse_h__
#define __parse_h__

#include "smtp-filter/common/list_db.h"
#include "smtp-filter/common/profile.h"
#include "smtp-filter/common/profile_db.h"
#include "smtp-filter/libdnscheck/dns-checker.h"
#include "filter.h"

#include "samples.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int      parse_main(const char *fname);

    list_id  parse_email(const char *fname, list_db *ldb);
    list_id  parse_ip_list(const char *fname, list_db *ldb);
//    list_id  parse_rbl_list(const char *fname, list_db *ldb);

    dns_checker_check_id_t parse_rbl_list(const char *fname, profile_db *pdb);

#define PARSE_PROFILE_DEFAULTS       0x00000000UL
#define PARSE_PROFILE_CAN_BE_OMITTED 0x00000001UL

    profile *parse_profile(const char *fname, list_db *ldb, profile_db *pdb, content_filter *cf, unsigned int needed_type,
			   unsigned int flags);

    int      parse_profile_db(list_db *ldb, 
			      profile_db *pdb, 
			      content_filter *cf);

    samples_node *parse_samples(const char *fname);


#ifdef __cplusplus
}
#endif

#endif /* __parse_h__ */

/*
 * <eof parse.h>
 */
