/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: profile_db.h                                                          *
 * Created: Mon Apr 29 21:07:08 2002                                           *
 * Desc: Profile database (name of profile -> profile).                        *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  profile_db.h
 * \brief Profile database (name of profile -> profile).
 *
 * \note Really now it is just sequence of profiles.
 */

#ifndef __profile_db_h__
#define __profile_db_h__

#include "profile.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct __profile_db profile_db;

    profile_db *profile_db_open(const char *name, int flags, int mode);
    void        profile_db_close(profile_db *pdb);

    /*
     * Then you add profile to profile database, this profile 
     * became property of database, i.e. it's destroing when 
     * database is destroing.
     */
    int         profile_db_add(profile_db *pdb, profile *p);
    /* profile    *profile_search(profile_db *pdb, const char *name); */

    int         profile_db_iterate(profile_db *pdb, 
				   int (* callback)(void *user_data, profile *p), 
				   void *user_data,
                   bool skipFormal);

#ifdef __cplusplus
}
#endif


#endif /* __profile_db_h__ */

/*
 * <eof profile_db.h>
 */
