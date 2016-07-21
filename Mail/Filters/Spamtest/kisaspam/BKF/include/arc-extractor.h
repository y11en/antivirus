/*******************************************************************************
 * Proj: Content Filtration Library                                            *
 * --------------------------------------------------------------------------- *
 * File: arc-extractor.h                                                       *
 * Created: Tue Jan  6 16:48:05 2004                                           *
 * Desc: Archive extractor                                                     *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  arc-extractor.h
 * \brief Archive extractor
 */

#ifndef __arc_extractor_h__
#define __arc_extractor_h__

#include "message.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct message_extractor_config {

        unsigned int depth;

        unsigned int total_memory_limit;
        unsigned int one_file_limit;
    };

#define MESSAGE_EXTRACT_ERROR          (-1)
#define MESSAGE_EXTRACT_OK             ( 0)
    int message_extract_archives(message *m, struct message_extractor_config *mec);

#ifdef __cplusplus
}
#endif

#endif /* __arc_extractor_h__ */

/*
 * <eof arc-extractor.h>
 */
