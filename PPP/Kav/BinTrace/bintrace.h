#ifndef __BINTRACE__H
#define __BINTRACE__H

#ifndef _SIZE_T_DEFINED
#if defined (__APPLE__)
    #include <stdlib.h>
#else 
    typedef unsigned int size_t;
    #define _SIZE_T_DEFINED
#endif
#endif

#if defined (__unix__)
#define __int64 long long int
#endif
#define TRACE_PREFIX "\x01\xD8\xAD\xFE\x9F\xDA\xC9\xBF"
#define TRACE_PREFIX_DW1 0xFEADD801
#define TRACE_PREFIX_DW2 0xBFC9DA9F

enum enumBTTimeFormats{
        eBTTimeFormat_time_t    = 1,
        eBTTimeFormat_FILETIME  = 2,
        eBTTimeFormat_PRAGUE_DT = 3,
};

#ifdef __cplusplus
extern "C" {
#endif

int bt_serialize(void* buffer, size_t buffer_size, size_t* preq_size, void** pbtdata, size_t* pbtsize, unsigned __int64 _time, const char* trace_format, void* trace_args, const char* hdr_format, ...);
int bt_serialize_v(void* buffer, size_t buffer_size, size_t* preq_size, void** pbtdata, size_t* pbtsize, unsigned __int64 _time, const char* trace_format, void* trace_args, const char* hdr_format, void* hdr_args);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __BINTRACE__H
