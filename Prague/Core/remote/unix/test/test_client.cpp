#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include <prague.h>
#include "rpc_buffer.h"


using namespace PragueRPC;

 unsigned int thePID = getpid ();

void* proc ( void* aDummy )
{
  tDWORD aResult = 2;
  tCHAR aBuffer [] = "4444";
  ConnectionID anID = 0;
  if ( !connectToRPCService ( "Server", anID ) ) {
    printf ( "Can't connect to the server!\n" );
    return 0;
  }
  for ( int i = 0; i < 100; i++ ) {
    tQWORD anOffset = 3;
    tERROR anError = Buffer_SeekRead  ( anID, 1212, &aResult, anOffset, aBuffer, sizeof ( aBuffer ) );
    if ( PR_SUCC ( anError ) )
      printf ( "%d Result: %d %s, %d\n" , thePID, aResult, aBuffer, anOffset );
    else {
      printf ( "Something is wrong.\n" );
      break;
    }
  }
  //  printf ( "That's all.\n" );
  return 0;
}
 
int main ()
{
  startRPCService ( "Client", 4 );
  //  registerInterface ( BufferInterface );
  pthread_t aThreads [1];
  for ( int i = 0; i < sizeof ( aThreads ) / sizeof ( pthread_t ); i++ )
    pthread_create ( &aThreads [ i ], 0, proc, 0 );
  for ( int i = 0; i < sizeof ( aThreads ) / sizeof ( pthread_t ); i++ )
    pthread_join ( aThreads [ i ], 0 );
  stopRPCService ();
  return 0;
}
