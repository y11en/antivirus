/*******************************************************************************
 * Proj: Content Filtration                                                    *
 * --------------------------------------------------------------------------- *
 * File: stopdict.h                                                            *
 * Created: Wed Dec 26 18:18:52 2001                                           *
 * Desc: Interface to stop words dictionary.                                   *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  stopdict.h
 * \brief Interface to stop words dictionary.
 */

#ifndef __stopdict_h__
#define __stopdict_h__

#include <wintypes.h>
#include <libmorph/Morph.h>

#ifdef __cplusplus
extern "C" 
{
#endif

    typedef struct __stopdict_container stopdict_container;

#define SDC_READ        0
#define SDC_READWRITE   1

    stopdict_container* sdc_open(stopdict_container* sdc, const char* fname, int mode);
    void                sdc_close(stopdict_container* sdc);

    typedef struct __stopdict stopdict;

    struct __stopdict
    {
	LEXID* dict;
	size_t size;
    };

    int                 sdc_add_dict(stopdict_container* sdc, const stopdict* sd);
    stopdict*           sdc_load_dict(stopdict_container* sdc, stopdict* sd, unsigned int index);
    void                sd_destroy(stopdict* sd);

    stopdict*           sd_prepare(stopdict* sd, LEXID* lexems, size_t size);
#define SD_USE_CAPS 0x000001
#define SD_USE_FORM 0x000002
    int                 sd_check(stopdict* sc, LEXID lid, unsigned int flags);

#ifdef __cplusplus
}
#endif



#endif // __stopdict_h__

/*
 * <eof stopdict.h>
 */
