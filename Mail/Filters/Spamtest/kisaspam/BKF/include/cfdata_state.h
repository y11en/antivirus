#ifndef __cfdata_state_h__
#define __cfdata_state_h__

#include "cfdata_errors.h"

#ifdef WIN32
#pragma pack(push, 8)
#else
#pragma pack(8)
#endif

#ifdef __cplusplus
extern "C"
{
#endif


    typedef struct __cfd_state_config cfd_state_config;

    typedef int  (*cfd_state_warning)(cfd_state_config* sc, const cfdata_error* error);
    typedef void (*cfd_state_next_file)(cfd_state_config* sc, const char* fname);
    typedef void (*cfd_percentage)(cfd_state_config* sc, unsigned int percentage);
  
    struct __cfd_state_config
    {
	void*        ptr;               /*!< User defined data */
	cfdata_error    error;             /*!< Filled at error */
	unsigned int file_no;           /*!< Index of file processed from fi argument of cfdata_compile() */

	unsigned int cur_file_size;
	unsigned int bytes_processed;

	cfd_state_warning   warning;    /*!< Function, called at warnings. If it returns non-zero code, 
					 *   compilation breaks and error filled by ERROR_STRUCT with 
					 *   some of PW_* error code */
	cfd_state_next_file next_file;  /*!< Function, called when processing of new file starts (by ref or
					 *   next file in fi argument list) */

	cfd_percentage      percentage; /*!< Function, called when changed percents of file processed. */

        int (* each_item)(cfd_state_config *sc);
    };

#ifdef __cplusplus
}
#endif 

#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

#endif
