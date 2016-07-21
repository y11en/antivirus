/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: config.h                                                              *
 * Created: Sun Jun 23 20:09:47 2002                                           *
 * Desc: Configuration parameters (text messages and so on)                    *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  config.h
 * \brief Configuration parameters (text messages and so on).
 */

#ifndef __config_h__
#define __config_h__

#ifdef __cplusplus
extern "C" 
{
#endif

#ifndef _WIN32
#include "anti-spam-build/build_info.h"
#endif //_WIN32


#define MAX_REGISTERS     12
#define REGISTER_RBL	0
#define REGISTER_TRICKS 1
#define REGISTER_DCC_BODY 2
#define REGISTER_DCC_FUZ1 3
#define REGISTER_DCC_FUZ2 4
#define REGISTER_MESSAGE_TEXT_LEN 5
#define REGISTER_MESSAGE_IMG_COUNT 6

#define GSG_FILE_STORAGE_SUFFIX	".fst"

/*
 * All following macroses are placed here only for you information.
 * Please, don't uncomment they here, use you Makefile.smtp.local
 * file instead. For example, you can turn on license-hole ability
 * in this way:
 *    CXXOPTIONS += -DHAVE_LICENSE_HOLE
 *
 * #define HAVE_LICENSE_HOLE 1
 * #define TIME_LIMIT        1
 *
 * #define TIME_LIMIT_YEAR 103
 * #define TIME_LIMIT_MONTH 5 
 * #define TIME_LIMIT_MDAY 30 
 *
 * From paths.cpp:
 *
 * #define FILTER_PATH_ROOT "/root/of/filter"
 */

#ifndef TIME_LIMIT_YEAR 
#define TIME_LIMIT_YEAR 103
#endif

#ifndef TIME_LIMIT_MONTH
#define TIME_LIMIT_MONTH 5
#endif

#ifndef TIME_LIMIT_MDAY
#define TIME_LIMIT_MDAY 30
#endif

#define SMTP_FILTER_VERSION "2.1.1"

#ifndef SMTP_FILTER_BUILD
	#define SMTP_FILTER_BUILD "0.0.0"
#endif

#define MAX_ATTRIBUTE_TEXT 2000

#ifdef FILTER_SERVER


typedef struct _config 
{
    char *bounce_text;
    char *reject_text;
    char *hostname;

    int inited;

	_config(): inited(0), bounce_text(NULL), reject_text(NULL), hostname(NULL) {};

} config_structure;

    int config_set_bounce_text(config_structure &CONFIG, const char *text);
    int config_set_reject_text(config_structure &CONFIG, const char *text);
    int config_set_hostname(config_structure &CONFIG, const char *text);

    int config_save(config_structure &CONFIG, const char *fname);
    int config_load(config_structure &CONFIG, const char *fname);
    int config_flush(config_structure &CONFIG);

    const char *config_get_bounce_text(config_structure &CONFIG);
    const char *config_get_reject_text(config_structure &CONFIG);
    const char *config_get_hostname(config_structure &CONFIG);
#else
    
    int config_set_bounce_text(const char *text);
    int config_set_reject_text(const char *text);
    int config_set_hostname(const char *text);

    int config_save(const char *fname);
    int config_load(const char *fname);
    int config_flush();

    const char *config_get_bounce_text();
    const char *config_get_reject_text();
    const char *config_get_hostname();
#endif

    int print_version();
    int snprintf_short_version(char *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __config_h__ */


/*
 * <eof config.h>
 */
