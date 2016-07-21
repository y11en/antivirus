#include <locale>

#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include "codec.h"
#include "ansi.h"
#include "oem.h"
#include "widechar.h"

#if defined ( __unix__ )
  using namespace CodecUnix;
#elif defined ( _WIN32 )
  using namespace CodecWin;
#endif

extern "C" hROOT g_root;

extern "C" tERROR pr_call PrStringCodec ( 
  const tBYTE * pInBuf, tUINT InBufLen, tCODEPAGE InCoding, 
  tBYTE * pOutBuf, tUINT OutBufLen, tCODEPAGE OutCoding, 
  tUINT * pReqOutBufLen 
) {
	
	tPTR   wrk;
	tERROR error;
	tUINT  OutLen;
	
	pReqOutBufLen = ( pReqOutBufLen == 0 ) ? &OutLen : pReqOutBufLen;
	
	if ( InCoding == OutCoding ) {
		*pReqOutBufLen = InBufLen;
		if ( OutBufLen >= InBufLen ) {
			memcpy ( pOutBuf, pInBuf, InBufLen );
			error = errOK;
		}
		else {
			memcpy ( pOutBuf, pInBuf, OutBufLen );
			error = errBUFFER_TOO_SMALL;
		}
	}
	else {
		error = errCODEPAGE_NOT_SUPPORTED;
		
		switch ( OutCoding ) {
			case cCP_OEM:
				switch ( InCoding ) {
					case cCP_UNICODE:
						error = wc2mb ( pInBuf, InBufLen/sizeof(tWCHAR), pOutBuf, OutBufLen, &InBufLen );
						pInBuf = pOutBuf;
						//  proceed to case cCP_ANSI
						
					case cCP_ANSI:
						*pReqOutBufLen = InBufLen;
						if ( OutBufLen < InBufLen ) {
							if ( !PrAnsiToOem(pInBuf,pOutBuf,OutBufLen) )
								error = errUNEXPECTED;
							else
								error = errBUFFER_TOO_SMALL;
						}
						else if ( !PrAnsiToOem(pInBuf,pOutBuf,InBufLen) )
							error = errUNEXPECTED;
						else
							error = errOK;
						break;
				}
				break;
				
			case cCP_ANSI:
				switch ( InCoding ) {
					case cCP_UNICODE:
						error = wc2mb ( pInBuf, InBufLen/sizeof(tWCHAR), pOutBuf, OutBufLen, pReqOutBufLen );
						break;
						
					case cCP_OEM:
						*pReqOutBufLen = InBufLen;
						if ( OutBufLen < InBufLen ) {
							if ( !PrOemToAnsi ( pInBuf, pOutBuf, OutBufLen ) )
								error = errUNEXPECTED;
							else
								error = errBUFFER_TOO_SMALL;
						}
						else if ( !PrOemToAnsi ( pInBuf, pOutBuf, InBufLen ) )
							error = errUNEXPECTED;
						else
							error = errOK;
						break;
				}
				break;
				
			case cCP_UNICODE:
				wrk = 0;
				switch ( InCoding ) {
					case cCP_OEM:
						if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(g_root,&wrk,InBufLen)) ) 
							break;
						if ( !PrOemToAnsi(pInBuf,wrk,InBufLen) ) {
							CALL_SYS_ObjHeapFree(g_root,wrk);
							error = errUNEXPECTED;
							break;
						}
						pInBuf = reinterpret_cast<const tBYTE *> ( wrk );
						//  proceed to case cCP_ANSI
						
					case cCP_ANSI:
						error = mb2wc( pInBuf, InBufLen, pOutBuf, OutBufLen/sizeof(tWCHAR), pReqOutBufLen );
						if ( wrk ) 
							CALL_SYS_ObjHeapFree( g_root, wrk );
						break;
				}
				break;
		}
	}    
	
	return error;
}

//////////////////////////////////////////////////////////////////////////

extern "C" tERROR pr_call PrStringChCase ( const tBYTE * pInBuf, tUINT InBufLen, tCODEPAGE InCoding, 
                                          tBYTE * pOutBuf, tSTR_CASE case_code )
{
  tERROR error = errUNEXPECTED;
  
  if ( case_code != cSTRING_TO_UPPER && case_code != cSTRING_TO_LOWER )
  {
    return errPARAMETER_INVALID;
  }
  
  switch ( InCoding )
  {
  case cCP_OEM:
    {
      if (    case_code == cSTRING_TO_UPPER && PrOemToUpper ( pInBuf, pOutBuf, InBufLen ) 
        || case_code == cSTRING_TO_LOWER && PrOemToLower ( pInBuf, pOutBuf, InBufLen ) )
      {
        error = errOK;
      }
    }
    break;
    
  case cCP_ANSI:
    {
      if (    case_code == cSTRING_TO_UPPER && PrAnsiToUpper ( pInBuf, pOutBuf, InBufLen ) 
        || case_code == cSTRING_TO_LOWER && PrAnsiToLower ( pInBuf, pOutBuf, InBufLen ) )
      {
        error = errOK;
      }
    }
    break;
    
  case cCP_UNICODE:
    {
      if ( InBufLen % sizeof ( tWCHAR ) != 0 )
      {
        error = errPARAMETER_INVALID;
      }
      else if (    case_code == cSTRING_TO_UPPER && PrWideCharToUpper ( reinterpret_cast<const tWCHAR *> ( pInBuf ), reinterpret_cast<tWCHAR *> ( pOutBuf ), InBufLen / sizeof ( tWCHAR ) ) 
        || case_code == cSTRING_TO_LOWER && PrWideCharToLower ( reinterpret_cast<const tWCHAR *> ( pInBuf ), reinterpret_cast<tWCHAR *> ( pOutBuf ), InBufLen / sizeof ( tWCHAR ) ) )
      {
        error = errOK;
      }
    }
    break;
    
  default:
    {
      error = errCODEPAGE_NOT_SUPPORTED;
    }
    break;
  }
  
  return error;
}
