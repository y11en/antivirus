// mydir.c -- functions for reading directories

#include "mydir.h"

my_findhandle_t my_findfirst(const char* pattern, my_finddata_t* info)
{
    my_findhandle_t handle;
#ifdef _WIN32
    handle = _findfirst(pattern, info);
    return handle;
#else
    handle = (_dos_findfirst(pattern, _A_NORMAL | _A_HIDDEN | _A_RDONLY | _A_SUBDIR | _A_SYSTEM, info) == 0)
        ? 0 : MY_FINDHANDLE_INVALID;
    return handle;
#endif
}

int my_findnext(my_findhandle_t handle, my_finddata_t* info)
{
#ifdef _WIN32
    return (_findnext(handle, info) != -1);
#else
    return (_dos_findnext(info) == 0);
#endif
}

void my_findclose(my_findhandle_t handle)
{
#ifdef _WIN32
    _findclose(handle);
#else
#endif
}
