#ifndef __KLDELAYLOAD_H__
#define __KLDELAYLOAD_H__

#include <delayimp.h>

#define KLDELAYLOAD_DL_CHECK()    \
        GetExceptionCode() == \
            VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND) || \
        GetExceptionCode() == \
            VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND)


#endif //__KLDELAYLOAD_H__
