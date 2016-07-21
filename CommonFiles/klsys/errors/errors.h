#if !defined (_ERRORS_H_)
#define _ERRORS_H_

#if defined (__cplusplus)
extern "C" {
#endif
int getError ();

int isFileNotFound( int anError );

int isInvalidArgument ( int anError );

int isAccessDenied ( int anError );

int isNotEnoughMemory ( int anError );

  int isTooManyOpenFiles ( int anError );

#if defined (__cplusplus)
}
#endif

#endif
