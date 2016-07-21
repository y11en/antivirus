/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject Error defintion header              *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/


#ifndef __PR_ERR_H
#define __PR_ERR_H


#if !defined(PR_tERROR_defined)
#define PR_tERROR_defined
typedef long                 tERROR;
#endif

#if !defined(PR_tDWORD_defined)
#define PR_tDWORD_defined
typedef unsigned int         tDWORD;
#endif


#define SEV_WARN       0
#define SEV_ERROR      1

typedef enum tag_ErrFacility {
	fac_PREDECLARED              = 0,
	fac_IMPLEMENTATION_DECLARED  = 1,
	fac_NATIVE_OS                = 2,
	fac_INTERFACE_DECLARED       = 3,
	fac_RESERVED4                = 4,
	fac_RESERVED5                = 5,
	fac_RESERVED6                = 6,
	fac_RESERVED7                = 7,
} tErrFacility;

#define PR_MAKE_ERR(severity, facility, id, code) ( (tERROR) (((tDWORD)(severity&1)<<31) | ((tDWORD)((facility)&0x7)<<28) | (((tDWORD)((id)&0xffff))<<12) | ((tDWORD)(code&0xfff))) )
#define PR_ERR_SEV(e)    ((((tDWORD)(e)) >> 31) & 0x1)
#define PR_ERR_FAC(e)    ((tErrFacility)((((tDWORD)(e)) >> 28) & 0x7))
#define PR_ERR_ID(e)     ((((tDWORD)(e)) >> 12) & 0xffff)
#define PR_ERR_CODE(e)   ((((tDWORD)(e)) >> 0 ) & 0xfff)

#define PR_MAKE_DECL_ERR(iid,code)                 PR_MAKE_ERR(SEV_ERROR, fac_PREDECLARED,             iid, code)
#define PR_MAKE_DECL_WARN(iid,code)                PR_MAKE_ERR(SEV_WARN,  fac_PREDECLARED,             iid, code)
#define PR_MAKE_IMP_ERR(pid,code)                  PR_MAKE_ERR(SEV_ERROR, fac_IMPLEMENTATION_DECLARED, pid, code)
#define PR_MAKE_IMP_WARN(pid,code)                 PR_MAKE_ERR(SEV_WARN,  fac_IMPLEMENTATION_DECLARED, pid, code)

#define PR_WIN32_TO_PRAGUE_ERR( win32_err )  ( (tERROR) ((fac_NATIVE_OS<<28) | ((~0x70000000) & ((((win32_err)&0x70000000)>>3) | (win32_err)))) )
#define PR_PRAGUE_TO_WIN32_ERR( pr_err )     ( (((~0x70000000) & (pe_err)) | (((pr_err)&0x0e000000)<<3)) & (~0x0e000000) )

#define PR_SUCC(e) (((tERROR)(e)) >= 0)
#define PR_FAIL(e) (((tERROR)(e)) <  0)



#define errOK                                      PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x000 )
#define warnFALSE                                  PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x003 )

// Send message return codes
#define errOK_DECIDED                              PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x001 ) // decider wants to stop distribute message and return positive result
#define errOK_NO_DECIDERS                          PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x002 ) // kernel notify there were no deciders at all


//#ifdef __cplusplus
//inline tERROR pr_ok_decided(tERROR err)            { return err == errOK ? errOK_DECIDED : err; }
//#endif

// common !!! WARNINGS !!! -- PR_FAIL returns 0 on this constants
#define errEOF                                     PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x0041 )  // 65
#define warnINTERFACE_VERSION_RECENT               PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x0042 )  // 66
#define warnINTERFACE_TIMESTAMT_RECENT             PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x0043 )  // 67
#define warnSET_PARENT_ACCESS_MODE_NOT_NEEDED      PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x0044 )  // 68
#define warnDEFAULT                                PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x0045 )  // 69
#define warnOBJECT_IS_CLOSING                      PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x0046 )  // 70
#define warnALREADY                                PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x0047 )
#define warnUPDATED                                PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x0048 )
//#define warnEOF                                    PR_MAKE_ERR( SEV_WARN, fac_PREDECLARED, 0, 0x0041 )  // 65 THE SAME AS ABOVE

// common errors
#define errUNEXPECTED                              PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0040 )
#define errNOT_ENOUGH_MEMORY                       PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0041 )
#define errNOT_SUPPORTED                           PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0042 )
#define errNOT_IMPLEMENTED                         PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0043 )
#define errBUFFER_TOO_SMALL                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0044 )
#define errACCESS_DENIED                           PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0045 )
#define errPARAMETER_INVALID                       PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0046 )
#define errGENERATE_UNIQUE_ID                      PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0047 )
#define errEXCEPTION_NOT_FIRED                     PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0048 )
#define errOUT_OF_OBJECT                           PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0049 )
#define errNOT_MATCHED                             PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x004a )
#define errNOT_OK                                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x004b )
#define errNOT_FOUND                               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x004c )
#define errFOUND                                   PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x004d )
#define errBOF                                     PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x004e )
#define errLOCKED                                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x004f )
#define errNOT_LOCKED                              PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0050 )
#define errOPERATION_CANCELED                      PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0051 )
#define errTIMESTAMP_UNSUITABLE                    PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0052 )
#define errOBSOLETE_METHOD_CALLED                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0054 ) // method called became obsolete
#define errSTRING_LEN_NOT_SPECIFIED                PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0055 )
#define errEND_OF_THE_LIST                         PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0056 )
#define errMETHOD_NOT_FOUND                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0057 )
#define errSTACK_EMPTY                             PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0058 )
#define errMEDIA_ERROR                             PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0059 )
#define errBAD_SIGNATURE                           PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x005a )
#define errBAD_VERSION                             PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x005b )
#define errBAD_SIZE                                PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x005c )
#define errBAD_TYPE                                PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x005d )
#define errBAD_INDEX                               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x005e )
#define errWRITE_PROTECT                           PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x005f )
#define errEXCEPTION                               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0060 )
#define errNO_LICENCE                              PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0061 )
#define errOUT_OF_SPACE                            PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0062 )
#define errNOT_READY                               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0063 )
#define errLESS_THAN                               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0064 )
#define errMORE_THAN                               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0065 )
#define errPROHIBITED                              PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0066 )
#define errALREADY                                 PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0067 )
#define errNEED_REBOOT                             PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0068 )

// handle errors
#define errHANDLE_POOL_DESTROYED                   PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0080 )
#define errHANDLE_POOL_OVERFLOW                    PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0081 )
#define errHANDLE_INVALID                          PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0082 )
#define errHANDLE_WRONG_STATUS                     PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0083 )
#define errHANDLE_CHAIN_CORRUPTED                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0084 )
#define errHANDLE_LIMIT_EXCEEDED                   PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0085 )

// object errors
#define errOBJECT_INCOMPATIBLE                     PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00c0 )
#define errOBJECT_READ_ONLY                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00c1 )
#define errOBJECT_NOT_INITIALIZED                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00c2 )
#define errOBJECT_CANNOT_BE_INITIALIZED            PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00c3 )
#define errOBJECT_ALREADY_FREED                    PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00c4 )
#define errOBJECT_NOT_FOUND                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00c5 )
#define errOBJECT_NOT_CREATED                      PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00c6 )
#define errOBJECT_BAD_INTERNAL_STATE               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00c7 )
#define errOBJECT_PASSWORD_PROTECTED               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00c8 )
#define errOBJECT_READ                             PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00ca )
#define errOBJECT_WRITE                            PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00cb )
#define errOBJECT_SEEK                             PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00cc )
#define errOBJECT_RESIZE                           PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00cd )
#define errOBJECT_WRITE_NOT_SUPPORTED              PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00ce )
#define errOBJECT_DATA_SIZE_UNDERSTATED            PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00cf )
#define errOBJECT_INVALID                          PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00d0 )
#define errOBJECT_ALREADY_EXISTS                   PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00d1 )
#define errOBJECT_BAD_PTR                          PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00d2 )
#define errOBJECT_IS_CLOSING                       PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00d3 )
#define errOBJECT_IS_CLOSED                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00d4 )
#define errOBJECT_IS_LOCKED                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00d5 )
#define errOBJECT_CANNOT_BE_REVERTED               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00d6 )
#define errOBJECT_NOT_DELETED                      PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x00d7 )
#define errOBJECT_DATA_CORRUPTED                   PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00d8 )
#define warnOBJECT_DATA_CORRUPTED                  PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x00d8 )
#define errOBJECT_TOO_LONG_RECURRING_STRUCTURES    PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x00d9 )


// interface errors
#define errINTERFACE_NOT_FOUND                     PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0100 )
#define errINTERFACE_NOT_LOADED                    PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0101 )
#define errINTERFACE_LIMIT_EXCEEDED                PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0102 )
#define errINTERFACE_INCOMPATIBLE                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0103 )
#define errINTERFACE_NO_MORE_ENTRIES               PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0104 )
#define errINTERFACE_VERSION_CONFLICT              PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0105 )
#define errINTERFACE_ROOT_NOT_REGISTERED           PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0106 )
#define errINTERFACE_NOT_ASSIGNED_YET              PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0107 )
#define errINTERFACE_ALREADY_REGISTERED            PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0108 )
#define errINTERFACE_HAS_NO_RECOGNIZER             PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0109 )

// cookie erros
#define errCOOKIE_ALREADY_PRESENT                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0140 )
#define errCOOKIE_NOT_PRESENT                      PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0141 )

// property erors
#define errPROPERTY_NOT_FOUND                      PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0180 )
#define errPROPERTY_NOT_WRITABLE                   PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0181 )
#define errPROPERTY_ALREADY_PRESENT                PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0182 )
#define errPROPERTY_INVALID                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0183 )
#define errPROPERTY_RDONLY                         PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0184 )
#define errPROPERTY_STR_CODEPAGE_UNKNOWN           PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0185 ) // -- string property can be get/set by PropertyG(S)etStr method only !!!
#define errPROPERTY_NOT_READABLE                   PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0186 )
#define errPROPERTY_VALUE_INVALID                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0187 )
#define errPROPERTY_NOT_IMPLEMENTED                PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0188 )

// module erros
#define errMODULE_IS_BAD                           PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x01c0 )
#define errMODULE_NOT_FOUND                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x01c1 )
#define errMODULE_NOT_VERIFIED                     PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x01c2 )
#define errMODULE_HAS_NO_INIT_ENTRY                PR_MAKE_ERR( SEV_WARN,  fac_PREDECLARED, 0, 0x01c3 )
#define errMODULE_CANNOT_BE_LOADED                 PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x01c4 )
#define errMODULE_UNKNOWN_FORMAT                   PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x01c5 )
#define errMODULE_CANNOT_BE_INITIALIZED            PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x01c6 )

// codepage erros
#define errCODEPAGE_UNKNOWN                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0200 )
#define errCODEPAGE_NOT_SUPPORTED                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0201 )
#define errCODEPAGE_CONVERSION_UNAVAILABLE         PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0202 )

// synchronization errors
#define errSYNCHRONIZATION_FAILED                  PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0220 )
#define errSYNCHRONIZATION_TIMEOUT                 PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0221 )
#define errSYNCHRONIZATION_OBJECT_ABANDONED        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0222 )
#define errTIMEOUT                                 errSYNCHRONIZATION_TIMEOUT
#define errABANDONED                               errSYNCHRONIZATION_OBJECT_ABANDONED

// datetime errors
#define errDT_OUT_OF_RANGE                         PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0230 )
#define errDT_DIFF_NEGATIVE                        PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0231 )

// object proxy errors
#define errPROXY_STATE_INVALID                     PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0240 )

//#define errLSAT_AVAILABLE_ERROR                   PR_MAKE_ERR( SEV_ERROR, fac_PREDECLARED, 0, 0x0FFF ) // 0xFFF is maximum error code!


#endif
