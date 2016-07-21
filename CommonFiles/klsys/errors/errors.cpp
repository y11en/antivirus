#if defined (__unix__)
#include <errno.h>
#elif defined (_WIN32)
#include <windows.h>
#endif

#include "errors.h"

int getError ()
{
#if defined (__unix__)
  return errno;
#elif defined (_WIN32)
  return GetLastError ();
#endif
}

int isFileNotFound( int anError )
{
#if defined (__unix__)
  if ( anError == ENOENT )
    return 1;
#elif defined (_WIN32)
  if ( anError == ERROR_FILE_NOT_FOUND )
    return 1;
#else
#error "Implementation is needed!"
#endif
  return 0;
}

int isInvalidArgument ( int anError )
{
#if defined (__unix__)
  if ( anError == EINVAL ) 
    return 1;
#elif defined (_WIN32)
  if ( anError == ERROR_BAD_ARGUMENTS )
    return 1;
#else
#error "Implementation is needed!"
#endif
  return 0;
}


int isAccessDenied ( int anError )
{
#if defined (__unix__)
  if ( anError == EACCES || anError == EPERM )
    return 1;
#elif defined (_WIN32)
  if ( anError == ERROR_ACCESS_DENIED )
    return 1;
#else
#error "Implementation is needed!"
#endif
  return 0;
}

int isNotEnoughMemory ( int anError )
{
#if defined (__unix__)
  if ( anError == ENOMEM )
    return 1;
#elif defined (_WIN32)
  if ( anError == ERROR_NOT_ENOUGH_MEMORY )
    return 1;
#else
#error "Implementation is needed!"
#endif
  return 0;
}

int isTooManyOpenFiles ( int anError )
{
#if defined (__unix__)
  if ( anError == ENFILE || anError == EMFILE )
    return 1;
#elif defined (_WIN32)
  if ( anError ==  ERROR_TOO_MANY_OPEN_FILES )
    return 1;
#else
#error "Implementation is needed!"
#endif
  return 0;
}
