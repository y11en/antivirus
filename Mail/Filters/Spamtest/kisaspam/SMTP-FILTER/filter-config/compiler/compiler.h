/*******************************************************************************
 * Proj: SMTP-Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: compiler.h                                                            *
 * Created: Sat Aug 24 17:53:40 2002                                           *
 * Desc: Compiler interface functions.                                         *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  compiler.h
 * \brief Compiler interface functions.
 */

#ifndef __compiler_h__
#define __compiler_h__

#ifdef __cplusplus
extern "C"
{
#endif 

    int compiler_warning(const char *format, ...);
    int compiler_error(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* __compiler_h__ */


/*
 * <eof compiler.h>
 */
