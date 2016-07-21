#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <rpc_buffer.h>

using namespace PragueRPC;

bool Buffer_SeekRead_Stub ( Request& aRequest )
{
  ENTER;
  rpcBUFFER _this;
  tDWORD*  result = 0;
  tQWORD   offset;
  tCHAR*   buffer = 0;
  tDWORD   size;
  tERROR   anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( offset ) ||
       !aRequest.getParameter ( size ) ||
       !aRequest.getParameter ( buffer, size ) )
    return false;
  if ( !aRequest.check () )
    return false;
#ifdef _TEST_
  *result = getpid ();
  buffer [2] = '!';
  anError = errOK;
#else
  anError = PRBuffer_SeekRead ( aRequest.getConnectionID (), _this, result, offset, buffer, size );
#endif
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( buffer, size );
  aRequest.addParameter ( anError );
  return true;
}

bool Buffer_SeekWrite_Stub ( Request& aRequest )
{
  ENTER;
  rpcBUFFER _this = 0;
  tDWORD*  result = 0;
  tQWORD   offset;
  tCHAR*   buffer = 0;
  tDWORD   size;
  tERROR   anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( offset ) ||
       !aRequest.getParameter ( size ) ||
       !aRequest.getParameter ( buffer, size ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRBuffer_SeekWrite ( aRequest.getConnectionID (), _this, result, offset, buffer, size );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( anError );
  return true;
}

bool Buffer_GetSize_Stub ( Request& aRequest )
{
  ENTER;
  rpcBUFFER _this = 0;
  tQWORD*  result = 0;
  IO_SIZE_TYPE type;
  tERROR   anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( type ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRBuffer_GetSize ( aRequest.getConnectionID (), _this, result, type );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( anError );
  return true;
}

bool Buffer_SetSize_Stub ( Request& aRequest )
{
  ENTER;
  rpcBUFFER _this = 0;
  tQWORD  new_size;
  tERROR   anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( new_size ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRBuffer_SetSize ( aRequest.getConnectionID (), _this, new_size );
  aRequest.addParameter ( anError );
  return true;
}

bool Buffer_Flush_Stub ( Request& aRequest )
{
  ENTER;
  rpcBUFFER _this = 0;
  tERROR   anError;
  if ( !aRequest.getParameter ( _this ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRBuffer_Flush ( aRequest.getConnectionID (), _this );
  aRequest.addParameter ( anError );
  return true;
}


Method BufferMethods [] = 
  { { iCountCRC32str ( "Buffer_SeekRead" ), (void*)&Buffer_SeekRead_Stub },
    { iCountCRC32str ( "Buffer_SeekWrite"), (void*)&Buffer_SeekWrite_Stub },
    { iCountCRC32str ( "Buffer_GetSize"), (void*)&Buffer_GetSize_Stub },
    { iCountCRC32str ( "Buffer_SetSize"), (void*)&Buffer_SetSize_Stub },
    { iCountCRC32str ( "Buffer_Flush"), (void*)&Buffer_Flush_Stub },
    { 0, 0 }, };

Interface PRrpc_buffer_ServerIfHandle = { { 0x6DEC2539, 0x5026, 0x495B, 0xA1, 0x1C, { 0xFB,0x0C,0x9F,0x95,0xBB,0x9C } }, BufferMethods };
