/*******************************************************************************
 * Project: Content Filtration Library, version 2                              *
 * (C) 2001-2004, Ashmanov & Partners company, Moscow, Russia                  *
 * Contact: info@ashmanov.com, http://www.ashmanov.com                         *
 * --------------------------------------------------------------------------- *
 * File: cfdata.h                                                              *
 * Created: Sun Nov 18 16:56:50 2001                                           *
 * Desc: API for data compiler.                                                *
 *******************************************************************************/

/**
 * \file  cfdata.h
 * \brief API for data compiler.
 */

#ifndef __cfdata_h__
#define __cfdata_h__

#include <stdio.h>
#include "cfdata_state.h"

#ifdef WIN32
#pragma pack(push, 8)
#else
#pragma pack(8)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define CFD_FI_XML    0
#define CFD_FI_BIN    1
#define CFD_FI_UPK    2
#define CFD_FI_XML_IN_MEMORY    3

    /**
     * \brief Compile file info (in and out formats).
     */
    struct __cfd_file_info
    {
	const char* name;  /*!< Name: file name for CFD_FI_XML and content
			    *   filter database name for CFD_FI_BIN */
	int         type;  /*!< CFD_FI_XML or CFD_FI_BIN */
        
        const char *password; /*!< For CFD_FI_UPK type */
        
        const char *file_contents;         /*!< For CFD_FI_XML_IN_MEMORY type */
        unsigned int file_contents_size;   /*!< For CFD_FI_XML_IN_MEMORY type */
    };

    typedef struct __cfd_file_info cfd_file_info;

#define CFD_COMPILE_DEFAULTS     0x00000000U

#define CFD_COMPILE_TRANSPORT    0x00000001U
#define CFD_COMPILE_NO_EXPORT    0x00000002U
#define CFD_COMPILE_SKIP_DUPSIGS 0x00000004U
#define CFD_COMPILE_APPEND       0x00000008U

    struct cfd_compile_config {
        unsigned int flags;
        double       sigdup_image; 
    };

    /**
     * \brief Create content filter database.
     *
     * \param out --- output.
     * \param sc  --- compilation config, may be == NULL.
     * \param fi  --- array of input data files.
     * \param fi_size --- size of fi array.
     */
    enum cf_errors cfdata_compile(const char* out, const struct cfd_compile_config *cfd_cc, 
                                  cfd_state_config* sc, const char* template_base, 
                                  cfd_file_info* fi, unsigned int fi_size);

    enum cf_errors cfdata_export(const char* flt_base_name, int no_defaults, const char *out);

    struct cfd_default_config {

        /* nothing */

    };

    enum cf_errors cfdata_make_defaults(const char *fname, struct cfd_default_config *config);

#ifdef __cplusplus
}
#endif 

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif // __cfdata_h__


/*
 * <eof cfdata.h>
 */
