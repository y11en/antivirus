/*******************************************************************************
 * Proj: SMTP-Filter                                                           *
 * --------------------------------------------------------------------------- *
 * File: astaro-license.h                                                      *
 * Created: Wed Aug  6 08:31:51 2003                                           *
 * Desc: Astaro license wrapper (C interface)                                  *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  astaro-license.h
 * \brief Astaro license wrapper (C interface)
 */

#ifndef __astaro_license_h__
#define __astaro_license_h__

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * This function returns number of days till expiration of key.
     */

#define ASTARO_LICENSE_KEY_ERROR       -4
#define ASTARO_LICENSE_LIB_ERROR       -3
#define ASTARO_ACTIVE_KEY_SEARCH_ERROR -2
#define ASTARO_ACTIVE_KEY_INVALID      -1

    int astaro_active_key_check();

#define ASTARO_KEY_INSTALLED     0
#define ASTARO_KEY_INSTALL_ERROR 1
    int         astaro_active_key_install(const char *key_fname);
    const char *astaro_install_error_description();

    int astaro_print_active_key_info(FILE *f);

#ifdef __cplusplus
}
#endif

#endif /* __astaro_license_h__ */

/*
 * <eof astaro-license.h>
 */
