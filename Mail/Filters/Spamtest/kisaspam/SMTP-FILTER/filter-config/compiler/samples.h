/*******************************************************************************
 * Proj: SMTP Filter                                                           *
 * (C) 2001-2002, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: samples.h                                                             *
 * Created: Thu Jul 11 21:54:11 2002                                           *
 * Desc: Structures for samples parsing.                                       *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  samples.h
 * \brief Structures for samples parsing.
 */

#ifndef __samples_h__
#define __samples_h__

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    struct samples_node
    {
	char *id;

	struct {
	    char   **names;
	    size_t   alloc;
	} files;

	samples_node *brother;
	samples_node *son;
    };

    samples_node *samples_add(samples_node *root, const char *path, const char *filename);
    int           samples_free(samples_node *root);

#ifdef __cplusplus
}
#endif

#endif /* __samples_h__ */

/*
 * <eof samples.h>
 */
