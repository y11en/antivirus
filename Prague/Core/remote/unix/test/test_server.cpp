#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include <prague.h>
#include "rpc_buffer.h"

using namespace PragueRPC;

int main ()
{
  startRPCService ( "Server", 5 );
  registerInterface ( PRrpc_buffer_ServerIfHandle );
  getchar ();
  stopRPCService ();
  return 0;
}
