/*-----------------02.05.99 12:57-------------------
 * Project Prague                                 *
 * Subproject Types definition                    *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/


#ifndef __PR_PROP_H
#define __PR_PROP_H


/* Property Table
 *
 *
 * proparray is two-dimensional tDATA array
 * each property is described by 6 elements of array
 *
 *
 * tDATA proparray[number_of_properties,6] =
 * {
 *      propid, &buffer, buffersize, mode, GetPropFunc, SetPropFunc
 *      propid, offset, buffersize, mode, GetPropFunc, SetPropFunc
 * };
 */

#define mPROPERTY_TABLE_RECORD_LEN  (6)
#define mPROPERTY_TABLE_SIZE(a)     (sizeof(a)/sizeof(a[0])/mPROPERTY_TABLE_RECORD_LEN)

#define mPROPERTY_TABLE(a)          static tDATA a[] = {
#define mPROPERTY_TABLE_END(a)      };
#define mLOCAL_PROPERTY(propid,strtype,member,mode,getfn,setfn) (tDATA)(propid), (tDATA)(&((strtype*)(NULL))->member), (tDATA)(sizeof(((strtype*)(0))->member)), (tDATA)(mode),                                     (tDATA)(getfn), (tDATA)(setfn),
#define mLOCAL_PROPERTY_BUF(propid,strtype,member,mode)         (tDATA)(propid), (tDATA)(&((strtype*)(NULL))->member), (tDATA)(sizeof(((strtype*)(0))->member)), (tDATA)(mode),                                     (tDATA)(NULL),  (tDATA)(NULL),
#define mLOCAL_PROPERTY_FN(propid,getfn,setfn)                    (tDATA)(propid), (tDATA)(0),                           (tDATA)(0),                               (tDATA)(cPROP_BUFFER_NONE),                        (tDATA)(getfn), (tDATA)(setfn),
#define mLOCAL_PROPERTY_REQUIRED_FN(propid,getfn,setfn)           (tDATA)(propid), (tDATA)(0),                           (tDATA)(0),                               (tDATA)(cPROP_BUFFER_NONE|cPROP_REQUIRED),         (tDATA)(getfn), (tDATA)(setfn),
#define mLOCAL_PROPERTY_WRITABLE_OI_FN(propid,getfn,setfn)        (tDATA)(propid), (tDATA)(0),                           (tDATA)(0),                               (tDATA)(cPROP_BUFFER_NONE|cPROP_WRITABLE_ON_INIT), (tDATA)(getfn), (tDATA)(setfn),
#define mLOCAL_PROPERTY_REQ_WRITABLE_OI_FN(propid,getfn,setfn)    (tDATA)(propid), (tDATA)(0),                           (tDATA)(0),                               (tDATA)(cPROP_BUFFER_NONE|cPROP_REQUIRED|cPROP_WRITABLE_ON_INIT), (tDATA)(getfn), (tDATA)(setfn),
#define mLAST_CALL_PROPERTY( getfn, setfn )                       (tDATA)pgPROP_LAST_CALL, (tDATA)(0), (tDATA)(0), (tDATA)(cPROP_BUFFER_NONE), (tDATA)(getfn), (tDATA)(setfn),

#define mpPROPERTY_TABLE(a)					tDATA _##a[] = {
#define mpPROPERTY_TABLE_END(a)			}; static tDATA a[sizeof(_##a)/sizeof(tDATA)]; { for(unsigned int i=0;i<sizeof(_##a)/sizeof(tDATA);++i) a[i]=_##a[i]; }
#if defined (__unix__) && defined (__cplusplus)
template <typename T, typename C>
int getSize( T C::* a1) {
  return sizeof ( T );
};
#define mpLOCAL_PROPERTY(propid,strtype,member,mode,getfn,setfn) (tDATA)(propid), (tDATA)(offsetof(strtype,member)-sizeof(cObjImpl)), (tDATA)(getSize ( &strtype::member)), (tDATA)(mode),                                     (tDATA)(getfn), (tDATA)(setfn),
#define mpLOCAL_PROPERTY_BUF(propid,strtype,member,mode)         (tDATA)(propid), (tDATA)(offsetof(strtype,member)-sizeof(cObjImpl)), (tDATA)(getSize ( &strtype::member)), (tDATA)(mode),                                     (tDATA)(NULL),  (tDATA)(NULL),

#else
#define mpLOCAL_PROPERTY(propid,strtype,member,mode,getfn,setfn) (tDATA)(propid), (tDATA)(&((strtype*)(NULL))->member)-sizeof(cObjImpl), (tDATA)(sizeof(((strtype*)(0))->member)), (tDATA)(mode),                                     (tDATA)(getfn), (tDATA)(setfn),
#define mpLOCAL_PROPERTY_BUF(propid,strtype,member,mode)         (tDATA)(propid), (tDATA)(&((strtype*)(NULL))->member)-sizeof(cObjImpl), (tDATA)(sizeof(((strtype*)(0))->member)), (tDATA)(mode),                                     (tDATA)(NULL),  (tDATA)(NULL),
#endif
#define mpLOCAL_PROPERTY_FN(propid,getfn,setfn)                    (tDATA)(propid), (tDATA)(0),                           (tDATA)(0),                               (tDATA)(cPROP_BUFFER_NONE),                        (tDATA)(getfn), (tDATA)(setfn),
#define mpLOCAL_PROPERTY_REQUIRED_FN(propid,getfn,setfn)           (tDATA)(propid), (tDATA)(0),                           (tDATA)(0),                               (tDATA)(cPROP_BUFFER_NONE|cPROP_REQUIRED),         (tDATA)(getfn), (tDATA)(setfn),
#define mpLOCAL_PROPERTY_WRITABLE_OI_FN(propid,getfn,setfn)        (tDATA)(propid), (tDATA)(0),                           (tDATA)(0),                               (tDATA)(cPROP_BUFFER_NONE|cPROP_WRITABLE_ON_INIT), (tDATA)(getfn), (tDATA)(setfn),
#define mpLOCAL_PROPERTY_REQ_WRITABLE_OI_FN(propid,getfn,setfn)    (tDATA)(propid), (tDATA)(0),                           (tDATA)(0),                               (tDATA)(cPROP_BUFFER_NONE|cPROP_REQUIRED|cPROP_WRITABLE_ON_INIT), (tDATA)(getfn), (tDATA)(setfn),
#define mpLAST_CALL_PROPERTY( getfn, setfn )                       (tDATA)pgPROP_LAST_CALL, (tDATA)(0), (tDATA)(0), (tDATA)(cPROP_BUFFER_NONE), (tDATA)(getfn), (tDATA)(setfn),

#define mSHARED_PROPERTY(propid,value)                            (tDATA)(propid), (tDATA)(value),                       (tDATA)(sizeof(value)),                   cPROP_BUFFER_SHARED,                               (tDATA)(NULL),  (tDATA)(NULL),
#define mSHARED_PROPERTY_PTR(propid,value,size)                   (tDATA)(propid), (tDATA)(&value),                      (tDATA)(size),                            cPROP_BUFFER_SHARED_PTR,                           (tDATA)(NULL),  (tDATA)(NULL),
#define mSHARED_PROPERTY_VAR(propid,value,size)                   (tDATA)(propid), (tDATA)(&value),                      (tDATA)(size),                            cPROP_BUFFER_SHARED_VAR,                           (tDATA)(NULL),  (tDATA)(NULL),

#define cPROP_BUFFER_NONE        0
#define cPROP_BUFFER_READ        1
#define cPROP_BUFFER_WRITE       2
#define cPROP_BUFFER_READ_WRITE  3
#define cPROP_BUFFER_SHARED      4      // no functions will be called
#define cPROP_BUFFER_SHARED_PTR  0x0C   // no functions will be called ( buffer contains pointer to data )
#define cPROP_REQUIRED           0x10   // property is required for initializing handle
#define cPROP_WRITABLE_ON_INIT   0x20   // property available for write on init only
#define cPROP_BUFFER_SHARED_VAR  0x44   // no functions will be called ( buffer contains pointer to pointer to data )
#define cPROP_BUFFER_HSTRING     0x80   // property is a buffer contained hSTRING object


// property ranges definition
/*
 * there are four property ranges defined now
 *
 * system properties (beginning with 'ps')
 *      system level properties like current date, time
 *      CPU, native information et cetra.
 *
 * global properties (beginning with 'pg')
 *      properties that describe plugins and their interfaces
 *      like vendor, versions and so on.
 *
 * local properties (beginning with 'pl')
 *      additional properties that defined by plugin's vendor.
 *      There is only one requirement for that kind of properties:
 *      they should be documented.
 *
 * private properties (beginning with 'pp')
 *      may be used internally in inter-interface communications
 *      not required to be documented.
 */

// property identifier is dword with 5 fields
//           +-----------------+---------+-------------+-------------------------------------------
// bit count:| callback stop:1 | range:4 | heritable:1 | type:7   | id:20
//           |        3        |   322   |      2      | 222 2222 | 1111 1111 11           
//           |        1        |   098   |      7      | 654 3210 | 9876 5432 1098 7654 3210
//           |        x        |   xxx   |      x      | xxx xxxx | xxxx xxxx xxxx xxxx xxxx
//           +-----------------+---------+-------------+----------+--------------------------------
// max value:|        1        |    7    |      1      |   127    | 1 048 575
//           +-----------------+---------+-------------+----------+--------------------------------

/* callback stop flag -- used by kernel event callback system */
#define pCALLBACK_TMP_STOP            0x80000000

/* property range definition, maximum 7 */
#define pRANGE_MASK                   0x70000000
#define pRANGE_SYSTEM                 0x10000000
#define pRANGE_GLOBAL                 0x20000000
#define pRANGE_LOCAL                  0x30000000
#define pRANGE_PRIVATE                0x40000000
#define pRANGE_CUSTOM                 0x50000000

// ---------------------------------------------------------
#define PROP_RANGE( p )               ( (p) & pRANGE_MASK )
#define IS_PROP_RANGE( p, a )         ( PROP_RANGE(p) == pRANGE_##a )

// property is heritable (CreateObject or, rather, HandleCreate will copy those properties to the child)
// it works for CUSTOM property only
#define pCUSTOM_HERITABLE             0x08000000  


// propety types definition, maximum 127
#define pTYPE_MASK          0x07F00000
#define pTYPE_SHIFT         (20)

// last change 18.05.07
// correspond to pr_types.h
#define pTYPE_NOTHING       ( 0 )
#define pTYPE_BYTE          ( tid_BYTE         << pTYPE_SHIFT )
#define pTYPE_WORD          ( tid_WORD         << pTYPE_SHIFT )
#define pTYPE_DWORD         ( tid_DWORD        << pTYPE_SHIFT )
#define pTYPE_QWORD         ( tid_QWORD        << pTYPE_SHIFT )
#define pTYPE_STRING        ( tid_STRING       << pTYPE_SHIFT )
#define pTYPE_CODEPAGE      ( tid_CODEPAGE     << pTYPE_SHIFT )
#define pTYPE_LCID          ( tid_LCID         << pTYPE_SHIFT )
#define pTYPE_DATETIME      ( tid_DATETIME     << pTYPE_SHIFT )
#define pTYPE_BINARY        ( tid_BINARY       << pTYPE_SHIFT )
#define pTYPE_INT           ( tid_INT          << pTYPE_SHIFT )
#define pTYPE_IFACEPTR      ( tid_IFACEPTR     << pTYPE_SHIFT )
#define pTYPE_PTR           ( tid_PTR          << pTYPE_SHIFT )
#define pTYPE_OBJECT        ( tid_OBJECT       << pTYPE_SHIFT )
#define pTYPE_BOOL          ( tid_BOOL         << pTYPE_SHIFT )
#define pTYPE_CHAR          ( tid_CHAR         << pTYPE_SHIFT )
#define pTYPE_WCHAR         ( tid_WCHAR        << pTYPE_SHIFT )
#define pTYPE_WSTRING       ( tid_WSTRING      << pTYPE_SHIFT )
#define pTYPE_ERROR         ( tid_ERROR        << pTYPE_SHIFT )
#define pTYPE_UINT          ( tid_UINT         << pTYPE_SHIFT )
#define pTYPE_SBYTE         ( tid_SBYTE        << pTYPE_SHIFT )
#define pTYPE_SHORT         ( tid_SHORT        << pTYPE_SHIFT )
#define pTYPE_LONG          ( tid_LONG         << pTYPE_SHIFT )
#define pTYPE_LONGLONG      ( tid_LONGLONG     << pTYPE_SHIFT )
#define pTYPE_IID           ( tid_IID          << pTYPE_SHIFT )
#define pTYPE_PID           ( tid_PID          << pTYPE_SHIFT )
#define pTYPE_ORIG_ID       ( tid_ORIG_ID      << pTYPE_SHIFT )
#define pTYPE_OS_ID         ( tid_OS_ID        << pTYPE_SHIFT )
#define pTYPE_VID           ( tid_VID          << pTYPE_SHIFT )
#define pTYPE_PROPID        ( tid_PROPID       << pTYPE_SHIFT )
#define pTYPE_VERSION       ( tid_VERSION      << pTYPE_SHIFT )
#define pTYPE_DATA          ( tid_DATA         << pTYPE_SHIFT )
#define pTYPE_FUNC_PTR      ( tid_FUNC_PTR     << pTYPE_SHIFT )
#define pTYPE_EXPORT        ( tid_EXPORT       << pTYPE_SHIFT )
#define pTYPE_IMPORT        ( tid_IMPORT       << pTYPE_SHIFT )
#define pTYPE_TRACE_LEVEL   ( tid_TRACE_LEVEL  << pTYPE_SHIFT )
#define pTYPE_SERIALIZABLE  ( tid_SERIALIZABLE << pTYPE_SHIFT )
#define pTYPE_DOUBLE        ( tid_DOUBLE       << pTYPE_SHIFT )
#define pTYPE_TYPE_ID       ( tid_TYPE_ID      << pTYPE_SHIFT )
#define pTYPE_VECTOR        ( tid_ VECTOR      << pTYPE_SHIFT )
#define pTYPE_STRING_OBJ    ( tid_STRING_OBJ   << pTYPE_SHIFT )
#define pTYPE_BUFFER        ( tid_BUFFER       << pTYPE_SHIFT )
#define pTYPE_STR_DSC       ( tid_STR_DSC      << pTYPE_SHIFT )
#define pTYPE_SIZE_T        ( tid_SIZE_T       << pTYPE_SHIFT )
#define pTYPE_IMPL_ID       ( tid_IMPL_ID      << pTYPE_SHIFT )
#define pTYPE_MSG_ID        ( tid_MSG_ID       << pTYPE_SHIFT )
#define pTYPE_MSG_CLS_ID    ( tid_MSG_CLS_ID   << pTYPE_SHIFT )
// last change 18.05.07
// --------------------------------------------------------------------
#define PR_PROP_TYPE(p)     ( (p) & pTYPE_MASK )
#define IS_PROP_TYPE(p,t)   ( PR_PROP_TYPE(p) == t )

#define PR_PROP_CODE_TYPE(p)   (( (p) & pTYPE_MASK ) >> pTYPE_SHIFT )
#define IS_PROP_CODE_TYPE(p,t) ( PR_PROP_CODE_TYPE(p) == t )



/* property number mask */

#define pNUMBER_MASK        0x000FFFFF

#define pUSER_MIN_PROP_ID   0x00001000
#define pUSER_MAX_PROP_ID   pNUMBER_MASK

/* property maker macro */
#define mPROPERTY_MAKE(range, type, propid)         ( (range         ) | (type) | (pNUMBER_MASK & (propid)) )
#define mPROPERTY_MAKE_SYSTEM(type, propid)         ( (pRANGE_SYSTEM ) | (type) | (pNUMBER_MASK & (propid)) )
#define mPROPERTY_MAKE_GLOBAL(type, propid)         ( (pRANGE_GLOBAL ) | (type) | (pNUMBER_MASK & (propid)) )
#define mPROPERTY_MAKE_LOCAL(type, propid)          ( (pRANGE_LOCAL  ) | (type) | (pNUMBER_MASK & (propid)) )
#define mPROPERTY_MAKE_PRIVATE(type, propid)        ( (pRANGE_PRIVATE) | (type) | (pNUMBER_MASK & (propid)) )
#define mPROPERTY_MAKE_CUSTOM(type, propid)         ( (pRANGE_CUSTOM ) | (type) | (pNUMBER_MASK & (propid)) )

#define mPROPERTY_GET_RANGE(propid)                 ( (propid) & pRANGE_MASK )

// ---
// beginning of the named property identificator range
#define cPROPERTY_NAMED_CUSTOM    (0x00008000)


/* system properties */

/*
#define psKERNEL_VERSION        mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000001 )
#define psBYTE_ORDER            mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000002 )
#define psINTEGER_SIZE          mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000003 )
#define psCHAR_SIZE             mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000004 )
#define psCURRENT_TIME          mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000005 )
#define psANSI_CP               mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000006 )
#define psOEM_CP                mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000007 )
#define psLANGUAGE              mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000008 )
#define psTIME_ZONE             mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000009 )
#define psCPU_TYPE              mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x0000000A )
#define psCPU_VENDOR            mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x0000000B )
#define psCPU_VERSION           mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x0000000C )
#define psCPU_SERIAL            mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x0000000D )
#define psCPU_FREQUENCY         mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x0000000E )
#define psOS_STYLE              mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x0000000F )
#define psOS_TYPE               mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000010 )
#define psOS_NAME               mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000011 )
#define psOS_VENDOR             mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000012 )
#define psOS_VERSION            mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000013 )
#define psOS_VERSION_VERBAL     mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000014 )
#define psCURRENT_USER_NAME     mPROPERTY_MAKE_SYSTEM( pTYPE_ , 0x00000015 )
*/

#define psREGISTER_INTERFACE_PROC   mPROPERTY_MAKE_SYSTEM(pTYPE_DWORD, 0x00000001)
#define psOBJECT_OPERATIONAL        mPROPERTY_MAKE_SYSTEM(pTYPE_BOOL , 0x00000002)
#define psCALLBACK_SENDER           mPROPERTY_MAKE_SYSTEM(pTYPE_BOOL , 0x00000003)


/* global properties */
#define pgNATIVE_ERR                  mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000020 )

#define pgOBJECT_NAME                 mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
#define pgOBJECT_NAME_CP              mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x00000040 )
#define pgOBJECT_FULL_NAME            mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000041 )
#define pgOBJECT_FULL_NAME_CP         mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x00000041 )
#define pgOBJECT_COMPLETE_NAME        mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000042 )
#define pgOBJECT_COMPLETE_NAME_CP     mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x00000042 )
#define pgOBJECT_ALT_NAME             mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000043 )
#define pgOBJECT_ALT_NAME_CP          mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x00000043 )
#define pgOBJECT_PATH                 mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000044 )
#define pgOBJECT_PATH_CP              mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x00000044 )
#define pgOBJECT_SIZE                 mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000045 )
#define pgOBJECT_SIZE_Q               mPROPERTY_MAKE_GLOBAL( pTYPE_QWORD   , 0x00000045 )
#define pgOBJECT_SIGNATURE            mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x00000046 )
#define pgOBJECT_SUPPORTED            mPROPERTY_MAKE_GLOBAL( pTYPE_ORIG_ID , 0x00000047 )
#define pgOBJECT_ORIGIN               mPROPERTY_MAKE_GLOBAL( pTYPE_ORIG_ID , 0x00000048 )
#define pgOBJECT_OS_TYPE              mPROPERTY_MAKE_GLOBAL( pTYPE_OS_ID   , 0x00000049 )
#define pgOBJECT_OPEN_MODE            mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004a )
#define pgOBJECT_NEW                  mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000004b )
#define pgOBJECT_CODEPAGE             mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x0000004c )
#define pgOBJECT_LCID                 mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004d )
#define pgOBJECT_ACCESS_MODE          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004e )
#define pgOBJECT_USAGE_COUNT          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004f )
#define pgOBJECT_COMPRESSED_SIZE      mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000050 )
#define pgOBJECT_HEAP                 mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00000051 )
#define pgOBJECT_AVAILABILITY         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000052 )
#define pgOBJECT_BASED_ON             mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00000053 )
#define pgOBJECT_ATTRIBUTES           mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000054 )
#define pgOBJECT_HASH                 mPROPERTY_MAKE_GLOBAL( pTYPE_QWORD   , 0x00000055 )
#define pgOBJECT_REOPEN_DATA          mPROPERTY_MAKE_GLOBAL( pTYPE_BINARY  , 0x00000056 )  // applicable for OS, ObjPtr & IO objects: unique name and/or state of the object used for reopen it at the same state
#define pgOBJECT_INHERITABLE_HEAP     mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00000057 )
#define pgOBJECT_NATIVE_IO            mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x00000058 )
#define pgOBJECT_PROP_SYNCHRONIZED    mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00000059 )
#define pgOBJECT_MEM_SYNCHRONIZED     mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x0000005a )
#define pgOBJECT_NATIVE_PATH          mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000060 )

#define pgINTERFACE_ID                mPROPERTY_MAKE_GLOBAL( pTYPE_IID     , 0x00000080 )
#define pgINTERFACE_SUBTYPE           mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000081 )
#define pgINTERFACE_VERSION           mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
#define pgINTERFACE_REVISION          mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000085 )
#define pgINTERFACE_COMMENT           mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgINTERFACE_COMMENT_CP        mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x00000083 )
#define pgINTERFACE_SUPPORTED         mPROPERTY_MAKE_GLOBAL( pTYPE_IID     , 0x00000084 )
#define pgINTERFACE_FLAGS             mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00000086 )
#define pgINTERFACE_COMPATIBILITY     mPROPERTY_MAKE_GLOBAL( pTYPE_IID     , 0x00000087 )
#define pgINTERFACE_CODEPAGE          mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x00000088 )
#define psINTERFACE_COMPATIBILITY_BASE mPROPERTY_MAKE_GLOBAL( pTYPE_IID     , 0x00000089 )

#define pgVENDOR_ID                   mPROPERTY_MAKE_GLOBAL( pTYPE_VID     , 0x000000a1 )
#define pgVENDOR_NAME                 mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x000000a2 )
#define pgVENDOR_NAME_CP              mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x000000a2 )
#define pgVENDOR_COMMENT              mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x000000a3 )
#define pgVENDOR_COMMENT_CP           mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x000000a3 )

#define pgPLUGIN_ID                   mPROPERTY_MAKE_GLOBAL( pTYPE_PID     , 0x000000c0 )
#define pgPLUGIN_VERSION              mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x000000c1 )
#define pgPLUGIN_NAME                 mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x000000c2 )
#define pgPLUGIN_NAME_CP              mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x000000c2 )
#define pgPLUGIN_COMMENT              mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x000000c3 )
#define pgPLUGIN_COMMENT_CP           mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x000000c3 )
#define pgPLUGIN_HANDLE               mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x000000c4 )
#define pgPLUGIN_CODEPAGE             mPROPERTY_MAKE_GLOBAL( pTYPE_CODEPAGE, 0x000000c5 )
#define pgPLUGIN_EXPORT_COUNT         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD,    0x000000c6 )

#define pgTRACE_LEVEL                 mPROPERTY_MAKE_GLOBAL( pTYPE_UINT    , 0x000000e3 )
#define pgTRACE_LEVEL_MIN             mPROPERTY_MAKE_GLOBAL( pTYPE_UINT    , 0x000000e4 )
#define pgTRACE_OBJ                   mPROPERTY_MAKE_GLOBAL( pTYPE_OBJECT  , 0x000000e5 )

// prop id mask for any property
// if present it is called after all other properties
#define pgPROP_LAST_CALL              mPROPERTY_MAKE_GLOBAL( 0,             0x00000011)

/*
 * required properties by interfaces
 * should be defined in interface headers
 */

/*
 * local properties for interface should be
 * defined in separate header files shipped
 * with plugin
 */


// helper defines for get/set implementation of simple property types
#define _integrated_type_prop_set(_type, _val) \
	{                                            \
		*out_size = sizeof(_type);                 \
		if (NULL != buffer)                        \
		{                                          \
			if (sizeof(_type) < size)                \
				error = errBAD_SIZE;                   \
			else                                     \
				_val = *(_type*) buffer;               \
		}                                          \
	}

#define _integrated_type_prop_get(_type, _val) \
	{                                            \
		*out_size = sizeof(_type);                 \
		if (NULL != buffer)                        \
		{                                          \
			if (sizeof(_type) < size)                \
				error = errBAD_SIZE;                   \
			else                                     \
				*(_type*) buffer = _val;               \
		}                                          \
	}



#endif
