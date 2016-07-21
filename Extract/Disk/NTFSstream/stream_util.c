#include <prague.h>
#include <windows.h>
#include "ntfsstream.h"
#include <iface/i_io.h>
#include <iface/i_objptr.h>
#include "stream_util.h"



HINSTANCE g_ntdll;
NQIF      g_qif;
tPROPID   g_pid_this_is_the_stream;
tPROPID   g_pid_streams;




// ---
tERROR pr_call _ConvertWin32Err( tDWORD win32err ) {
  switch( win32err ) {
		case ERROR_FILE_NOT_FOUND :
    case ERROR_PATH_NOT_FOUND : return errOBJECT_NOT_FOUND;
    case ERROR_ACCESS_DENIED  : return errACCESS_DENIED;
    default                   : return errUNEXPECTED;
  }
}


// ---
typedef struct tag_ErrMsg {
  DWORD   err;
  tSTRING fmt;
} tErrMsg;




// ---
tERROR _ren_move_by_MoveEx( tWSTRING existing, tWSTRING new_name, tBOOL overwrite, tErrMsg* emsg ) {
  DWORD flags = MOVEFILE_COPY_ALLOWED;
  if ( overwrite )
    flags |= MOVEFILE_REPLACE_EXISTING;
  if ( MoveFileExW(existing,new_name,flags) )
    return errOK;
  emsg->err = GetLastError();
  emsg->fmt = "MoveFileEx returns error %d";
  return errNOT_OK;
}



// ---
tERROR _ren_move_by_Move( tWSTRING existing, tWSTRING new_name, tBOOL overwrite, tErrMsg* emsg ) {
  DWORD   err;
  tSTRING fmt;

  if ( MoveFileW(existing,new_name) )
    return errOK;
  
  err = GetLastError();
  fmt = "MoveFile returns error %d";
  if ( (err == ERROR_ALREADY_EXISTS) && overwrite ) {
    if ( !DeleteFileW(new_name) ) {
      err = GetLastError();
      fmt = "DeleteFile returns error %d";
    }
    else if ( MoveFileW(existing,new_name) )
      return errOK;
    else
      err = GetLastError();
  }

  emsg->err = err;
  emsg->fmt = fmt;
  return errNOT_OK;
}



// ---
tERROR _ren_move_by_Copy( tWSTRING existing, tWSTRING new_name, tBOOL overwrite, tErrMsg* emsg ) {
  if ( CopyFileW(existing,new_name,!overwrite) ) {
    DeleteFileW( existing );
    return errOK;
  }
  
  emsg->err = GetLastError();
  emsg->fmt = "CopyFile returns error %d";
  return errNOT_OK;
}




// ---
tERROR pr_call _ren_move( hObjPtr obj, tWSTRING existing, tWSTRING new_name, tBOOL overwrite, tDWORD* native_error ) {

  tErrMsg err = {errOK,0};
  tERROR  e;
  *native_error = ERROR_SUCCESS;

  e = _ren_move_by_MoveEx( existing, new_name, overwrite, &err );
  if ( PR_SUCC(e) )
    return e;
  
  e = _ren_move_by_Move( existing, new_name, overwrite, &err );
  if ( PR_SUCC(e) )
    return e;
  
  e = _ren_move_by_Copy( existing, new_name, overwrite, &err );
  if ( PR_SUCC(e) )
    return e;

  PR_TRACE(( obj, prtERROR, err.fmt, *native_error=err.err ));
  return _ConvertWin32Err( *native_error );
}




// ---
tERROR pr_call _is_stream_name( const tWSTRING name, tBOOL* preceding_colon ) {
  if ( wcschr(name,L'\\') || wcschr(name,L'/') )
    return errPARAMETER_INVALID;
  if ( *name == L':' )
    *preceding_colon = cTRUE;
  else
    *preceding_colon = cFALSE;
  return errOK;
}



// ---
tERROR pr_call _not_name_of_the_stream( const tWSTRING name ) {
  const tWSTRING colon = (const tWSTRING) wcsrchr( name, L':');
  if ( !colon )
    return errOK;
  if ( (1 == (colon - name)) && iswalpha(*name) )
    return errOK;
  return errPROPERTY_VALUE_INVALID;
}



// ---
tERROR pr_call _io_copy( hIO dst, hIO src, tPTR buffer, tDWORD bsize ) {
  tERROR error;
  tQWORD size;
  tQWORD offset;
  tBOOL  allocated;

  if ( PR_FAIL(error=CALL_IO_SetSize(dst,0)) )
    return error;
  if ( PR_FAIL(error=CALL_IO_GetSize(src,&size,IO_SIZE_TYPE_EXPLICIT)) )
    return error;
  if ( size == 0 )
    return errOK;

  if ( !buffer || !bsize ) {
    if ( size < 0x1000 )
      bsize = (tDWORD)size;
    else
      bsize = 0x1000;
    if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(dst,&buffer,bsize)) )
      return error;
    allocated = cTRUE;
  }
  else
    allocated = cFALSE;

  offset = 0;
  while( offset < size ) {
    tDWORD s;
    if ( (offset+bsize) > size )
      s = (tDWORD)(size-offset);
    else
      s = bsize;
    if ( PR_FAIL(error=CALL_IO_SeekRead(src,&s,offset,buffer,s)) )
      break;
    if ( PR_FAIL(error=CALL_IO_SeekWrite(dst,0,offset,buffer,s)) )
      break;
    offset += s;
  }
  if ( allocated )
    CALL_SYS_ObjHeapFree( dst, buffer );
  return error;
}



