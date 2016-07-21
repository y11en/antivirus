#ifndef __cfdata_errors_h__
#define __cfdata_errors_h__

#include <sys/types.h>

#include "cf_errors.h"
/*------------------------------------------------------------------------------*/
/* CFData Components                                                            */
/*------------------------------------------------------------------------------*/
enum cf_components
{
  CFDATA_COMP_NONE        = 0,
  CFDATA_COMP_SYSTEM      = 1,
  CFDATA_COMP_CATALOG     = 2,
  CFDATA_COMP_SIGDB       = 3,
  CFDATA_COMP_TERMS       = 4,
  CFDATA_COMP_UPACK       = 5,
  CFDATA_COMP_XMLPARSER   = 6,
  CFDATA_COMP_EXPAT       = 7
};
/*------------------------------------------------------------------------------*/
/* CFLT_ERR_DATA (Invalid Data) Subtypes                                        */
/*------------------------------------------------------------------------------*/
#define CFLT_EDATA_NONE     0

/* Errors - negative */
#define CFLT_EDATA_UNKNOWN_TAG              -1
#define CFLT_EDATA_UNEXPECTED_TAG           -2
#define CFLT_EDATA_INVALID_ATTRIBUTE        -3
#define CFLT_EDATA_INVALID_ATTRIBUTE_VALUE  -4
#define CFLT_EDATA_INVALID_REF              -5
#define CFLT_EDATA_MISSING_CATID            -6
#define CFLT_EDATA_REPEATED_CATID           -7
#define CFLT_EDATA_MISSING_TEXT_ATTRIBUTE   -8
#define CFLT_EDATA_ENCODING_ERROR           -9

/* Warnings - positive */
#define CFLT_EDATA_CAT_EMPTY            1
#define CFLT_EDATA_CAT_ATTRS_CONFLICT   2
#define CFLT_EDATA_TERM_DUPLICATED      3
#define CFLT_EDATA_TERM_EMPTY           4
#define CFLT_EDATA_TERM_MORPH           5
#define CFLT_EDATA_TERM_CONFLICT        6
#define CFLT_EDATA_INVALID_CHARS        7
#define CFLT_EDATA_UNSUPPORTED_FORMAT   8

#define CFLT_EDATA_UNKNOWN_WARNING      10

/*------------------------------------------------------------------------------*/
/* Error/Warning info structure                                                 */
/*------------------------------------------------------------------------------*/
#ifdef WIN32
#pragma pack(push, 8)
#else
#pragma pack(8)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    struct __cfdata_error
    {
	enum cf_errors        err_code;       /* general error code */
	enum cf_components    err_component;  /* program component in which error occured */
	int                   err_sub_code;   /* detaild info for given error code */
    
	unsigned int line;          /* line number in current file */
	unsigned int column;        /* column number in current line */

        char file_name[512];        /* file name */
        char data_text[512];        /* text of invalid data object */
    };
    typedef struct __cfdata_error cfdata_error;

#ifdef __cplusplus
}
#endif 

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif
/*------------------------------------------------------------------------------*/
/* Error Description Functions                                                  */
/*------------------------------------------------------------------------------*/
const char* cf_error_description(enum cf_errors err_code);

const char* cfdata_error_invalid_data_sub_description(int sub_code);
const char* cfdata_error_description(const cfdata_error* error,
                              char* buf, size_t buf_size, int print_file_name);

unsigned int cf_warning_level(int code);
/*------------------------------------------------------------------------------*/
#endif
