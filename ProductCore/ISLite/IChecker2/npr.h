#ifndef _NPR_H_
#define _NPR_H_

typedef unsigned int	     tDWORD;
typedef void                 tVOID;
typedef unsigned char        tBYTE;
typedef unsigned short       tWORD;
typedef unsigned int         tBOOL;
typedef char                 tCHAR;
typedef char*                tSTRING;
typedef unsigned short       tWCHAR;
typedef unsigned short*      tWSTRING;
typedef long                 tERROR;
#if defined (__unix__)
#define __int64 long long
#endif
typedef unsigned __int64   tQWORD;
typedef signed __int64     tLONGLONG;

typedef void*                tPTR;
typedef signed int           tINT;           // native signed counter type
typedef unsigned int         tUINT;          // native unsigned counter type
typedef signed char          tSBYTE;
typedef signed short         tSHORT;
typedef signed long          tLONG;
typedef unsigned long        tIID;           // plugin interface identifier
typedef unsigned long        tPID;           // plugin identifier
typedef unsigned long        tORIG_ID;       // object origin identifier
typedef unsigned long        tOS_ID;         // os type identifier
typedef unsigned long        tVID;           // vendor ideftifier
typedef unsigned long        tVERSION;       // version of any identifier: High word - version, Low word - subversion
typedef unsigned long        tCODEPAGE;      // codepage identifier
typedef unsigned long        tLCID;          // locale identifier
typedef unsigned long        tDATA;          // universal data storage

#define cDATETIME_BYTE_SIZE 8
typedef unsigned char        tDATETIME[cDATETIME_BYTE_SIZE];   // universal time and date storage
typedef tDATETIME tDT;

#define SEV_WARN       0
#define SEV_ERROR      1

#define FAC_KERNEL     0
#define FAC_USER       1

#define PR_ERR_SEV(e)    ((((tDWORD)(e)) >> 31) & 0x1)
#define PR_ERR_FAC(e)    ((((tDWORD)(e)) >> 24) & 0x7f)
#define PR_ERR_CODE(e)   ((((tDWORD)(e)) >> 0 ) & 0xffffff)

#define PR_MAKE_ERR(facility,code)    ( (tERROR) (((tDWORD)(SEV_ERROR)<<31) | ((tDWORD)(facility)<<24) | ((tDWORD)(code))) )
#define PR_MAKE_WARN(facility,code)   ( (tERROR) ( ((tDWORD)(facility)<<16) | ((tDWORD)(code))) )

#define PR_SUCC(e) (((tERROR)(e)) >= 0)
#define PR_FAIL(e) (((tERROR)(e)) <  0)

#define errOK                                      PR_MAKE_WARN( FAC_KERNEL, 0 )
#define errEOF                                     PR_MAKE_WARN( FAC_KERNEL, 0x2041 )  // 65

#define errUNEXPECTED                              PR_MAKE_ERR( FAC_KERNEL, 0x0040 )
#define errNOT_ENOUGH_MEMORY                       PR_MAKE_ERR( FAC_KERNEL, 0x0041 )
#define errNOT_SUPPORTED                           PR_MAKE_ERR( FAC_KERNEL, 0x0042 )
#define errNOT_IMPLEMENTED                         PR_MAKE_ERR( FAC_KERNEL, 0x0043 )
#define errPARAMETER_INVALID                       PR_MAKE_ERR( FAC_KERNEL, 0x0046 )
#define errOUT_OF_OBJECT                           PR_MAKE_ERR( FAC_KERNEL, 0x0049 )
#define errNOT_MATCHED                             PR_MAKE_ERR( FAC_KERNEL, 0x004a )
#define errNOT_OK                                  PR_MAKE_ERR( FAC_KERNEL, 0x004b )
#define errNOT_FOUND                               PR_MAKE_ERR( FAC_KERNEL, 0x004c )
#define errOBJECT_NOT_INITIALIZED                  PR_MAKE_ERR( FAC_KERNEL, 0x00c2 )
#define errOBJECT_CANNOT_BE_INITIALIZED            PR_MAKE_ERR( FAC_KERNEL, 0x00c3 )
#define errOBJECT_ALREADY_EXISTS                   PR_MAKE_ERR( FAC_KERNEL, 0x00d1 )
#define errMODULE_UNKNOWN_FORMAT                   PR_MAKE_ERR( FAC_KERNEL, 0x01c5 )

#define errUSER_DEFINED                            PR_MAKE_ERR( FAC_USER, 0x0 )
#define errUSER_DEFINED_WARN                       PR_MAKE_WARN( FAC_USER, 0x0 )

#define errDT_OUT_OF_RANGE   (errUSER_DEFINED + (0x00000001))
#define errDT_DIFF_NEGATIVE  (errUSER_DEFINED + (0x00000002))

#define PR_TRACE(list)
#define PR_TRACE_A0(a,b)
#define PR_TRACE_A1(a,b,c)

#define cTRUE (1)
#define cFALSE (0)

#define countof(array) (sizeof(array)/sizeof((array)[0]))

#define pr_call

#endif //_NPR_H_
