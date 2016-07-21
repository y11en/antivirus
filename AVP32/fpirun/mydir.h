// mydir.h -- functions for reading directories

#ifdef _WIN32
#include <io.h>
#else
#include <dos.h>
#endif

#ifdef _WIN32
typedef struct _finddata_t  my_finddata_t;
typedef long                my_findhandle_t;
#else
typedef struct _find_t      my_finddata_t;
typedef long                my_findhandle_t;
#endif

#define MY_FINDHANDLE_INVALID   (-1)

my_findhandle_t my_findfirst(const char* pattern, my_finddata_t* info);
int my_findnext(my_findhandle_t handle, my_finddata_t* info);
void my_findclose(my_findhandle_t handle);
