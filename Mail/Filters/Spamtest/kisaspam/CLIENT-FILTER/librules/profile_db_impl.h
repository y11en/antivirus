/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: profile_db_impl.h                                                          *
 * Created: Mon Apr 29 21:07:08 2002                                           *
 * Desc: Profile database (name of profile -> profile).                        *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  profile_db_impl.h
 * \brief Profile database (name of profile -> profile).
 *
 * \note Really now it is just sequence of profiles.
 */

#ifndef __profile_impl_db_h__
#define __profile_impl_db_h__


struct __profile_db
{
    struct {
	profile **array;
	size_t    used;
	size_t    alloc;
    } profiles;

    int          fd;
    unsigned int flags;
};

#endif /* __profile_db_impl_h__ */

/*
 * <eof profile_db.h>
 */
