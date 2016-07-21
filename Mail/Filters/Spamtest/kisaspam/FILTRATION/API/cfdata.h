/*******************************************************************************
 * Project: Content Filtration Library                                         *
 * (C) 2001-2003, Ashmanov & Partners company, Moscow, Russia                  *
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

#ifdef WIN32
#pragma pack(push, 8)
#else
#pragma pack(8)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * Error types (for type field of cfd_error, see below.)
     */

#define ERROR_OK         0
#define ERROR_PARSE      1
#define ERROR_STRUCT     2 
#define ERROR_SYS        3
#define ERROR_PAT        4
#define ERROR_BIN        5
#define ERROR_CREATE     6
#define ERROR_MERGE      7
#define ERROR_UPK        8

    /*
     * Error codes for ERROR_STRUCT type.
     */
#define PE_NOERROR       0
#define PE_UNKNOWNTAG    1
#define PE_UNEXPECTEDTAG 2
#define PE_BADATTRIBUTE  3
#define PE_BADQUANT      4
#define PE_REFONLY       5
#define PE_REDEFINE      6
#define PE_CATID         7
#define PE_NOTEXT        8
#define PE_NEWTERMS      9
#define PE_CPCONVERT     10
#define PE_SAMPLEFILE    11
#define PE_NOTUNIQCATID  12
#define PE_DECODE        13

    /*
     * Internal error in compiler, possible it is a bug.
     */
#define PE_INTERNAL      9999

    /*
     * All warning codes greater than PE_WARNINGS
     */
#define PE_WARNINGS      10000

    /*
     * Warning codes.
     */
#define PW_DUPTERMS       (PE_WARNINGS + 1)
#define PW_CANTOPENFILE   (PE_WARNINGS + 2)
#define PW_UNSUPPORTEDMSG (PE_WARNINGS + 3)
#define PW_BADCHARS       (PE_WARNINGS + 4)
#define PW_CAT_EMPTY      (PE_WARNINGS + 5)
#define PW_UPK_HAS_EXTERNAL_LINKS (PE_WARNINGS + 6)

    /*
     * Error codes for ERROR_PARSE
     */
#define PE_XML_NONE                           0
#define PE_XML_NO_MEMORY                      1
#define PE_XML_SYNTAX                         2
#define PE_XML_NO_ELEMENTS                    3
#define PE_XML_INVALID_TOKEN                  4
#define PE_XML_UNCLOSED_TOKEN                 5
#define PE_XML_PARTIAL_CHAR                   6
#define PE_XML_TAG_MISMATCH                   7
#define PE_XML_DUPLICATE_ATTRIBUTE            8
#define PE_XML_JUNK_AFTER_DOC_ELEMENT         9
#define PE_XML_PARAM_ENTITY_REF              10
#define PE_XML_UNDEFINED_ENTITY              11
#define PE_XML_RECURSIVE_ENTITY_REF          12
#define PE_XML_ASYNC_ENTITY                  13
#define PE_XML_BAD_CHAR_REF                  14
#define PE_XML_BINARY_ENTITY_REF             15
#define PE_XML_ATTRIBUTE_EXTERNAL_ENTITY_REF 16
#define PE_XML_MISPLACED_XML_PI              17
#define PE_XML_UNKNOWN_ENCODING              18
#define PE_XML_INCORRECT_ENCODING            19
#define PE_XML_UNCLOSED_CDATA_SECTION        20
#define PE_XML_EXTERNAL_ENTITY_HANDLING      21
#define PE_XML_NOT_STANDALONE                22
#define PE_XML_UNEXPECTED_STATE              23

    /**
     * Error codes for ERROR_MERGE
     */

#define PE_MERGE_DIFFLIMIT                    1
#define PE_MERGE_RELLIMIT                     2

    /**
     * Error codes for ERROR_UPK
     */

#define PE_UPK_COULNDT_OPEN                   1
#define PE_UPK_NODATA                         2

    /**
     * \brief Information about error.
     */
    struct __cfd_error
    {
	unsigned char type;      /*!< Type of error, see ERROR_* */
    
	unsigned int  line;      /*!< Line number of current file */
	unsigned int  column;    /*!< Character number in current line */
    
	int           code;      /*!< Error code, corresponding to specified type. */
    };
    typedef struct __cfd_error cfd_error;

    typedef struct __cfd_state_config cfd_state_config;

    typedef int (*cfd_state_warning)(cfd_state_config* sc, unsigned int code);
    typedef void (*cfd_state_next_file)(cfd_state_config* sc, const char* fname);
    typedef void (*cfd_percentage)(cfd_state_config* sc, unsigned int percentage);
    typedef void (*cfd_before_save)(cfd_state_config* sc);
  
    /**
     * \brief User defined run-time compile behavior.
     */
    struct __cfd_state_config
    {
	void*        ptr;               /*!< User defined data */
	cfd_error    error;             /*!< Filled at error */
	unsigned int file_no;           /*!< Index of file processed from fi argument of cfdata_compile() */

	unsigned int cur_file_size;
	unsigned int bytes_processed;

	cfd_state_warning   warning;    /*!< Function, called at warnings. If it returns non-zero code, 
					 *   compilation breaks and error filled by ERROR_STRUCT with 
					 *   some of PW_* error code */
	cfd_state_next_file next_file;  /*!< Function, called when processing of new file starts (by ref or
					 *   next file in fi argument list) */

	cfd_percentage      percentage; /*!< Function, called when changed percents of file processed. */

        cfd_before_save     before_save; 

        int (* each_item)(cfd_state_config *sc);
    };

#define CFD_FI_XML    0
#define CFD_FI_BIN    1
#define CFD_FI_UPK    2

    /**
     * \brief Compile file info (in and out formats).
     */
    struct __cfd_file_info
    {
	const char* name;  /*!< Name: file name for CFD_FI_XML and content
			    *   filter database name for CFD_FI_BIN */
	int         type;  /*!< CFD_FI_XML or CFD_FI_BIN */
        
        const char *password; /*!< For CFD_FI_UPK type */
    };

    typedef struct __cfd_file_info cfd_file_info;

    /**
     * \brief Create content filter database.
     *
     * \param out --- output.
     * \param sc  --- compilation config, may be == NULL.
     * \param fi  --- array of input data files.
     * \param fi_size --- size of fi array.
     */
    int cfdata_compile(const char* out, cfd_state_config* sc, 
		       cfd_file_info* fi, unsigned int fi_size);

    int cfdata_compile_append(const char* out, cfd_state_config* sc, 
		       cfd_file_info* fi, unsigned int fi_size);

    int cfdata_export(const char* flt_base_name, int no_defaults, const char *out);

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
