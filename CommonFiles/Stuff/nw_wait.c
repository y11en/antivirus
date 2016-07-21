#include "nw_wait.h"
#include <windows.h>
#include <tchar.h>

// ---
int Condition_Wait( unsigned milliseconds, StopConditionProc stop, void* param ) {
  DWORD   i;
  HANDLE  event;

  if ( (milliseconds == INFINITE_TIME) && !stop )
    return 0;
  
  event = CreateEvent( 0, FALSE, FALSE, _T("MyOwnEvent") );

  for( i=0; i<milliseconds; i+=WAIT_ATOM ) {
    if ( stop && stop(param) )
      break;
    WaitForSingleObject( event, WAIT_ATOM );
  }
  CloseHandle( event );
  return 1;

}



