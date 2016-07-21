#include "psw_heuristic.h"
#include "../../Extract/Arc/MiniArchiver/Rar/rar.h"

#include <Prague/prague.h>
#include <Prague/pr_oid.h>

#include <Extract/iface/i_minarc.h>
#include <Extract/plugin/p_miniarc.h>
#include <Extract/plugin/p_minizip.h>

#include <memory.h>

#define ZIP_METH_STORED        0
#define ZIP_METH_DEFLATED      8
#define ZIP_METH_ENHDEFLATED   9
#define RAR_METH_STORED        0x30

#define MIN_SUSP_SIZE          1024       // 1 kb
#define MAX_SUSP_SIZE          150*1024   // 150 kb

static const tBYTE SuspMark[8] = {
  0x1E, 0xF1, 0xD0, 0xBA, 0x10, 0x1C, 0x59, 0x54
};

tERROR PswHeuristic( hOS p_os, hObjPtr p_objptr, hIO* p_ioptr )
{
  tERROR  Error;
  tDWORD  PID;
  tBYTE   ObjName[32];
  tDWORD  ObjNameSize;
  tBYTE   ObjSign[32];
  tDWORD  ObjSignSize;
  tDWORD  ObjNameExt;
  tDWORD  Method;
  tQWORD  USize;
  tDWORD  PSize;
  tDWORD  FileNum;
  hIO     hSuspIO;
  hObjPtr hTmpObj;

  // check host plugin id
  PID = CALL_SYS_PropertyGetDWord(p_os, pgPLUGIN_ID);
  if ( PID == PID_UNIVERSAL_ARCHIVER )
    PID = CALL_SYS_PropertyGetDWord(p_os, ppMINI_ARCHIVER_PID);
  switch( PID )
  {
  case PID_RAR:
  case PID_MINIZIP:
    if ( cFALSE == CALL_SYS_PropertyGetBool(p_objptr, pgIS_FOLDER) )
      break;  // supported arc, not folder
  default:
    return(errOBJECT_PASSWORD_PROTECTED); // unknown arc type
  }

  // get object name
  ObjNameSize = 0;
  if ( PR_FAIL(Error = CALL_SYS_PropertyGet(p_objptr, &ObjNameSize,
    pgOBJECT_NAME, &ObjName[0], sizeof(ObjName))) )
  {
    return(errOBJECT_PASSWORD_PROTECTED); // long name or error
  }
  if ( ObjNameSize < 5 )
    return(errOBJECT_PASSWORD_PROTECTED); // too short name

  // check extension
  ObjNameExt  = *(tDWORD*)(&ObjName[ObjNameSize-5]);
  ObjNameExt |= 0x20202020; // lowercase
  switch( ObjNameExt )
  {
  case 'exe.':  // .exe
  case 'moc.':  // .com
  case 'tab.':  // .bat
  case 'rcs.':  // .scr
  case 'fip.':  // .pif
  case 'dmc.':  // .cmd
  case 'sbv.':  // .vbs
    break; // executable
  default:
    return(errOBJECT_PASSWORD_PROTECTED); // not executable
  }

  // get object sizes
  USize = CALL_SYS_PropertyGetQWord(p_objptr, pgOBJECT_SIZE_Q);
  if ( USize < MIN_SUSP_SIZE || USize > MAX_SUSP_SIZE )
    return(errOBJECT_PASSWORD_PROTECTED); // invalid size

  // check compression method
  Method = CALL_SYS_PropertyGetDWord(p_objptr, pgOBJECT_COMPRESSION_METHOD);
  switch( PID )
  {
  case PID_MINIZIP:
    switch( Method )
    {
    case ZIP_METH_ENHDEFLATED:
      PSize = CALL_SYS_PropertyGetDWord(p_objptr, pgOBJECT_COMPRESSED_SIZE);
      if ( (USize + 2*USize/100) > (PSize + 12) )
        return(errOBJECT_PASSWORD_PROTECTED); // only 'fake' compression
    case ZIP_METH_STORED:
    case ZIP_METH_DEFLATED:
      break; // supported method
    default:
      return(errOBJECT_PASSWORD_PROTECTED); // invalid method
    }
    break;
  case PID_RAR:
    switch( Method )
    {
    case RAR_METH_STORED:
      break; // supported method
    default:
      return(errOBJECT_PASSWORD_PROTECTED); // invalid method
    }
    break;
  default:
    return(errOBJECT_PASSWORD_PROTECTED); // invalid plugin
  }

  // clone obj-ptr (for file enumeration)
  hTmpObj = (NULL);
  if ( PR_FAIL(Error = CALL_ObjPtr_Clone(p_objptr, &hTmpObj)) )
    return(errOBJECT_PASSWORD_PROTECTED);

  // calc number of files
  if ( PR_FAIL(Error = CALL_ObjPtr_Reset(hTmpObj, cTRUE)) )
  {
    CALL_SYS_ObjectClose(hTmpObj);
    return(errOBJECT_PASSWORD_PROTECTED);
  }
  FileNum = 0;
  while( PR_SUCC(Error = CALL_ObjPtr_Next(hTmpObj)) )
  {
    FileNum++;
    if ( FileNum > 1 )
    {
      CALL_SYS_ObjectClose(hTmpObj);
      return(errOBJECT_PASSWORD_PROTECTED);
    }
  }
  CALL_SYS_ObjectClose(hTmpObj);

  // create suspicion marker file
  hSuspIO = NULL;
  if ( PR_FAIL(Error = CALL_SYS_ObjectCreateQuick(p_objptr, &hSuspIO, IID_IO,
    PID_TMPFILE, SUBTYPE_ANY)) )
  {
    return(errOBJECT_PASSWORD_PROTECTED); // can't create
  }
  
  // get unique signature
  ObjSignSize = 0;
  memset(&ObjSign[0], 0, sizeof(ObjSign));
  CALL_SYS_PropertyGet(p_objptr, &ObjSignSize, pgOBJECT_SIGNATURE, &ObjSign[0], sizeof(ObjSign));

  // write suspicion marker
  if ( PR_FAIL(Error = CALL_IO_SeekWrite(hSuspIO, NULL, 0, (tBYTE*)(&SuspMark[0]),
    sizeof(SuspMark))) )
  {
    CALL_SYS_ObjectClose(hSuspIO);
    return(errOBJECT_PASSWORD_PROTECTED);
  }

  // write unique id
  if ( PR_FAIL(Error = CALL_IO_SeekWrite(hSuspIO, NULL, sizeof(SuspMark), &ObjSign,
    ObjSignSize)) )
  {
    CALL_SYS_ObjectClose(hSuspIO);
    return(errOBJECT_PASSWORD_PROTECTED);
  }

  CALL_SYS_PropertySetDWord(hSuspIO, pgOBJECT_ORIGIN, OID_PASSWORDED_EXE);

  // save result
  *p_ioptr = hSuspIO;
  return(errOK);
}
