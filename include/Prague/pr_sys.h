/*-----------------02.05.99 12:57-------------------
 * Project Prague                                 *
 * Subproject Kernel                              *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 * revision 2                                     *
--------------------------------------------------*/
// generic handle definition

#ifndef __PR_SYS_H
#define __PR_SYS_H


#define pgMODULE_ID           mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001009 )

#include <stdarg.h>

#if defined(__cplusplus) && !defined(_USE_VTBL)

  #if !defined( ROOT_DEFINED )
    #define ROOT_DEFINED
    struct cRoot;
    typedef cRoot* hROOT;
  #endif
  struct cHeap;   typedef cHeap*   hHEAP;
  struct cTracer; typedef cTracer* hTRACER;
  struct cPlugin; typedef cPlugin* hPLUGIN;

#else

  #if !defined( ROOT_DEFINED )
    #define ROOT_DEFINED
	  typedef struct tag_hROOT*   hROOT;
  #endif
  typedef struct tag_hHEAP*   hHEAP;
  typedef struct tag_hTRACER* hTRACER;
  typedef struct tag_hPLUGIN* hPLUGIN;

#endif

// ---
// ObjectFind constant ("up" and "down" parameters)
#define OF_ALL_LEVELS       cMAX_UINT

// ObjectFind flags
#define OFF_CHILD_FIRST     0x1
#define OFF_ITSELF          0x2


// trace destination FLAGs ("who" parameter for TraceLevelSet, TraceLevelGet methods)
// these flags define area for the method
#define tlsALL_IFACES     (0x0000)
#define tlsTHIS_IFACE     (0x0001)

#define tlsALL_SUBTYPES   (0x0000) 
#define tlsTHIS_SUBTYPE   (0x0002) // doesn't take into account without one of tlsTHIS_IFACE flags

#define tlsALL_PLUGINS    (0x0000)
#define tlsTHIS_PLUGIN    (0x0004)

#define tlsALL_VENDORS    (0x0000)
#define tlsTHIS_VENDOR    (0x0008)

#define tlsID_MASK        (0x00ff)
                          
// if no of below flags are set it means existing object are affected
#define tlsEXISTING       (0x0000) // only existing objects are affected
#define tlsAND_NEW        (0x4000) // both new and existing objects are affected
#define tlsNEW_ONLY       (0x8000) // only new objects are affected
// if no of above flags are set it means existing object are affected

// special cases
#define tlsTHIS_OBJECT    (0xff | tlsEXISTING)
#define tlsLIKE_THIS      (tlsTHIS_IFACE | tlsTHIS_SUBTYPE | tlsTHIS_PLUGIN | tlsTHIS_VENDOR | tlsAND_NEW)
#define tlsNEW_LIKE_THIS  (tlsTHIS_IFACE | tlsTHIS_SUBTYPE | tlsTHIS_PLUGIN | tlsTHIS_VENDOR | tlsNEW_ONLY)

#define tlsALL_IDS        (tlsALL_IFACES  | tlsALL_SUBTYPES | tlsALL_PLUGINS | tlsALL_VENDORS)
#define tlsALL_OBJECTS    (tlsALL_IDS | tlsAND_NEW)
#define tlsALL_NEW_OBJS   (tlsALL_IDS | tlsNEW_ONLY)
#define tlsALL_EXIST_OBJS (tlsALL_IDS | tlsEXISTING)


// message class and id constants
//#define msg_clsKERNEL    (0x00000001l)
#define msg_clsBROADCAST (0x00000002l)

#if !defined(TraceLevel_defined)
	#define TraceLevel_defined
	typedef struct tag_TraceLevel {
		#if defined(_INTEGRAL_MAX_BITS) && (_INTEGRAL_MAX_BITS >= 64)
			tTRACE_LEVEL max;  //  maximum trace level
			tTRACE_LEVEL min;  //  minimum trace level
		#else
			tWORD  max;  //  maximum trace level
			tWORD  min;  //  minimum trace level
		#endif
	} tTraceLevel;
#endif
	
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!! ----- change these macroses if tHANDLE structure (kernel.h) changed  ----- !!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// object macros
#if defined(_WIN64)
	#define _HND(obj)                 ( *(((tPTR*)(obj)) - 1) )
	#define _HND_OFFS(obj,type,offs)  ( ((type*)_HND(obj)) + offs )
	#define _HND_IFACE(obj)           ( *_HND_OFFS(obj,tPTR,1) )

	#define OBJ_IID(obj)              ( _HND_IFACE(obj) ? *(((tIID*)_HND_IFACE(obj))+0) : IID_NONE )
	#define OBJ_PID(obj)              ( _HND_IFACE(obj) ? *(((tIID*)_HND_IFACE(obj))+1) : PID_NONE )
	  
	// object trace definitions
	#define OBJ_TRACER(obj)           ( *((hTRACER*)_HND_OFFS(obj,tBYTE,0x60)) )

	#define OBJ_TRACE_LEVEL(obj)      ( ((tTraceLevel*)_HND_OFFS(obj,tBYTE,0x68)) )
	#define OBJ_TR_MIN(obj)           ( ((tTraceLevel*)_HND_OFFS(obj,tBYTE,0x68))->min )
	#define OBJ_TR_MAX(obj)           ( ((tTraceLevel*)_HND_OFFS(obj,tBYTE,0x68))->max )
	  
	#define OBJ_TR_MAXC(obj,lvl)      (lvl <= OBJ_TR_MAX(obj))
	#define OBJ_TR_MINC(obj,lvl)      (OBJ_TR_MIN(obj) <= lvl)
	#define OBJ_TR_CHK(obj,lvl)       (OBJ_TR_MAXC(obj,lvl) && OBJ_TR_MINC(obj,lvl))
	#define OBJ_TR_CHECK(obj,lvl)     (OBJ_TRACER(obj) && OBJ_TR_CHK(obj,lvl) )
#else
	#define _HND(obj)                 ( (tDWORD*)*(((tDWORD*)(obj)) - 1) )
	#define _HND_OFFS(obj,offs)       ( _HND(obj) + offs )
	#define _HND_IFACE(obj)           ( (*((tDWORD**)_HND_OFFS(obj,1))) )

	#define OBJ_IID(obj)              ( _HND_IFACE(obj) ? *(_HND_IFACE(obj)+0) : IID_NONE )
	#define OBJ_PID(obj)              ( _HND_IFACE(obj) ? *(_HND_IFACE(obj)+1) : PID_NONE )
	  
	// object trace definitions
	#define OBJ_TRACER(obj)           ( *((hTRACER*)_HND_OFFS(obj,14)) )

	#define OBJ_TRACE_LEVEL(obj)      ( ((tTraceLevel*)_HND_OFFS(obj,15)) )
	#define OBJ_TR_MIN(obj)           ( ((tTraceLevel*)_HND_OFFS(obj,15))->min )
	#define OBJ_TR_MAX(obj)           ( ((tTraceLevel*)_HND_OFFS(obj,15))->max )
	  
	#define OBJ_TR_MAXC(obj,lvl)      (lvl <= OBJ_TR_MAX(obj))
	#define OBJ_TR_MINC(obj,lvl)      (OBJ_TR_MIN(obj) <= lvl)
	#define OBJ_TR_CHK(obj,lvl)       (OBJ_TR_MAXC(obj,lvl) && OBJ_TR_MINC(obj,lvl))
	#define OBJ_TR_CHECK(obj,lvl)     (OBJ_TRACER(obj) && OBJ_TR_CHK(obj,lvl) )
#endif

// RegisterMsgHandler "flags" parameter
#define rmhLISTENER    0x00000000L
#define rmhDECIDER     0x00000001L


// ----------  interface flags  ----------
//#define IFACE_STATIC                   ((tDWORD)(0x00000001L)) // interface is static
#define IFACE_UNSWAPPABLE              ((tDWORD)(0x00000002L)) // interface is not subject to swap
#define IFACE_PROP_TRANSFER            ((tDWORD)(0x00000004L)) // unknown properties must be transfered to parent
#define IFACE_PROTECTED_BY_CS          ((tDWORD)(0x00000010L)) // interface is protected by critical section
#define IFACE_PROTECTED_BY_MUTEX       ((tDWORD)(0x00000040L)) // interface is protected by mutex
#define IFACE_OWN_CP                   ((tDWORD)(0x00000080L)) // interface has own pgOBJECT_CODEPAGE implementation
#define IFACE_SYSTEM                   ((tDWORD)(0x00000100L)) // interface is system, means it will be find by PID_SYSTEM(PID_ANY)
  
#define IFACE_CONDEMNED                ((tDWORD)(0x00020000L)) // interface is condemned to unload
#define IFACE_FREE_UNUSED              ((tDWORD)(0x00020000L)) // interface is condemned to unload
#define IFACE_LOADED                   ((tDWORD)(0x00040000L)) // interface is loaded, registerred and ready
  
#define IFACE_EXPLICIT_PARENT          ((tDWORD)(0x00100000L)) // 
#define IFACE_EXPLICIT_HOST            ((tDWORD)(0x00200000L)) // 
		
// #define IFACE_UN_UPDATABLE             ((tDWORD)(0x00000008L)) // interface is not subject to update
// #define IFACE_PROTECTED_BY_SEM         ((tDWORD)(0x00000020L)) // interface is protected by semaphore

// ---
// structure to set exception protected code frame
// MUST be allocated on STACK
typedef tDWORD tExceptionReg[3];

typedef tDWORD tStructDescriptor[5];

#define PR_STRUCT( type_name )     \
  tStructDescriptor type_name##descriptor; \
struct type_name { tStructDescriptor* dsc;

#define PR_FIELD( type, name )     type name
#define PR_STRUCT_END( type_name ) }; typedef struct type_name type_name



// ---
// descriptor of the message handler register info
struct tMsgHandlerDescr {
  hOBJECT object;   // registered object
  tDWORD  flags;    // desider or listener
  tDWORD  msg_cls;  // message class filter
  tIID    obj_iid;  // sender iid filter
  tPID    obj_pid;  // sender pid filter
  tIID    ctx_iid;  // context iid filter
  tPID    ctx_pid;  // context pid filter
};
typedef struct tMsgHandlerDescr tMsgHandlerDescr;



// ---
typedef tERROR (pr_call *tReservedMeth)( hOBJECT object );
#define RESERVED_COUNT 56 // 03.07.05 (58.26.11.04)

// ---
// callback of get/set internal data method
typedef tERROR (pr_call *tObjSerializeFunc)       ( hOBJECT object, hOBJECT to_serialize, tPTR serialized, tPTR ctx );
typedef tERROR (pr_call *tObjCheckCustomPropFunc) ( hOBJECT object, tPROPID prop_id,      tPTR ctx );
typedef tERROR (pr_call *tObjDeserializeFunc)     ( hOBJECT object, tPTR    serialized,   hOBJECT* deserialized, tPTR ctx );

// ---------------------------------------------------------------------------------------------------------------------------
// system methods prototypes

//EXTERN_C_BEG
// -----------------------------------------
// object management
typedef tERROR  (pr_call *tObjectCreate         )( hOBJECT object, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype );
typedef tERROR  (pr_call *tObjectCreateDone     )( hOBJECT object );
typedef tERROR  (pr_call *tObjectRevert         )( hOBJECT object, tBOOL reuse_custom_prop );
typedef tERROR  (pr_call *tObjectCreateQuick    )( hOBJECT object, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype );
typedef tERROR  (pr_call *tObjectCheck          )( hOBJECT object, hOBJECT to_check, tIID iid, tPID pid, tDWORD subtype, tBOOL do_not_check_int_struct);
typedef tERROR  (pr_call *tObjectValid          )( hOBJECT object, hOBJECT to_check );
typedef tERROR  (pr_call *tObjectClose          )( hOBJECT object );
typedef tERROR  (pr_call *tObjectSwap           )( hOBJECT object, hOBJECT o1, hOBJECT o2 );
typedef tERROR  (pr_call *tObjectFind           )( hOBJECT object, hOBJECT* result, tIID iid, tPID pid, tDWORD up, tDWORD down, tDWORD flags ); 
typedef tERROR  (pr_call *tObjectGetInternalData)( hOBJECT object, tDWORD* size, tDWORD obj_size, tBYTE* data, tDWORD data_size, tObjSerializeFunc convert, tObjCheckCustomPropFunc prop_check, tPTR ctx ); 
typedef tERROR  (pr_call *tObjectSetInternalData)( hOBJECT object, tDWORD* size, tDWORD obj_size, const tBYTE* data, tDWORD data_size, tObjDeserializeFunc convert, tPTR ctx ); 
typedef tERROR  (pr_call *tValidateLock         )( hOBJECT object, hOBJECT to_lock, tIID iid, tPID pid );
typedef tERROR  (pr_call *tRelease              )( hOBJECT object );

// -----------------------------------------
// low level object management
typedef tERROR  (pr_call *tHandleCreate        )( hOBJECT object, hOBJECT* new_obj, tDWORD memsize );
typedef tERROR  (pr_call *tInterfaceSet        )( hOBJECT object, tIID iid, tPID pid, tDWORD subtype );


// -----------------------------------------
// parent/child/object enumerate management
typedef hOBJECT (pr_call *tParentGet           )( hOBJECT object, tIID iid );
typedef tERROR  (pr_call *tParentSet           )( hOBJECT object, hOBJECT* old_parent, hOBJECT new_parent );
typedef tERROR  (pr_call *tChildGetFirst       )( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid );
typedef tERROR  (pr_call *tChildGetLast        )( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid );
typedef tERROR  (pr_call *tObjectGetNext       )( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid );
typedef tERROR  (pr_call *tObjectGetPrev       )( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid );


// -----------------------------------------
// property management
typedef tERROR  (pr_call *tPropertyIsPresent   )( hOBJECT object, tPROPID prop_id );
typedef tERROR  (pr_call *tPropertyGet         )( hOBJECT object, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size );
typedef tBOOL   (pr_call *tPropertyGetBool     )( hOBJECT object, tPROPID prop_id );
typedef tBYTE   (pr_call *tPropertyGetByte     )( hOBJECT object, tPROPID prop_id );
typedef tWORD   (pr_call *tPropertyGetWord     )( hOBJECT object, tPROPID prop_id );
typedef tUINT   (pr_call *tPropertyGetUInt     )( hOBJECT object, tPROPID prop_id );
typedef tDWORD  (pr_call *tPropertyGetDWord    )( hOBJECT object, tPROPID prop_id );
typedef tQWORD  (pr_call *tPropertyGetQWord    )( hOBJECT object, tPROPID prop_id );
typedef tDATA   (pr_call *tPropertyGetData     )( hOBJECT object, tPROPID prop_id );
typedef tPTR    (pr_call *tPropertyGetPtr      )( hOBJECT object, tPROPID prop_id );
typedef hOBJECT (pr_call *tPropertyGetObj      )( hOBJECT object, tPROPID prop_id );
typedef tERROR  (pr_call *tPropertyGetStr      )( hOBJECT object, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size, tCODEPAGE receive_cp );
typedef tERROR  (pr_call *tPropertyGetStrCP    )( hOBJECT object, tCODEPAGE* cp, tPROPID prop_id );

typedef tERROR  (pr_call *tPropertySet         )( hOBJECT object, tDWORD* out_size, tPROPID prop_id, const tPTR buffer, tDWORD size );
typedef tERROR  (pr_call *tPropertySetBool     )( hOBJECT object, tPROPID prop_id, tBOOL   val );
typedef tERROR  (pr_call *tPropertySetByte     )( hOBJECT object, tPROPID prop_id, tBYTE   val );
typedef tERROR  (pr_call *tPropertySetWord     )( hOBJECT object, tPROPID prop_id, tWORD   val );
typedef tERROR  (pr_call *tPropertySetUInt     )( hOBJECT object, tPROPID prop_id, tUINT   val );
typedef tERROR  (pr_call *tPropertySetDWord    )( hOBJECT object, tPROPID prop_id, tDWORD  val );
typedef tERROR  (pr_call *tPropertySetQWord    )( hOBJECT object, tPROPID prop_id, tQWORD  val );
typedef tERROR  (pr_call *tPropertySetData     )( hOBJECT object, tPROPID prop_id, tDATA   val );
typedef tERROR  (pr_call *tPropertySetPtr      )( hOBJECT object, tPROPID prop_id, tPTR    val );
typedef tERROR  (pr_call *tPropertySetObj      )( hOBJECT object, tPROPID prop_id, hOBJECT val );
typedef tERROR  (pr_call *tPropertySetStr      )( hOBJECT object, tDWORD* out_size, tPROPID prop_id, const tPTR pStrz, tDWORD size, tCODEPAGE src_cp );

typedef tERROR  (pr_call *tPropertyDelete      )( hOBJECT object, tPROPID prop_id );
typedef tDWORD  (pr_call *tPropertySize        )( hOBJECT object, tPROPID prop_id );
typedef tERROR  (pr_call *tPropertyEnumId      )( hOBJECT object, tPROPID* prop_id_result, tDWORD* cookie );
typedef tERROR  (pr_call *tPropertyEnumCustomId)( hOBJECT object, tPROPID* prop_id_result, tDWORD* cookie );

typedef tDWORD  (pr_call *tPropertyBroadcast   )( hOBJECT object, tIID iid, tDWORD type, tPROPID prop_id, tPTR buffer, tDWORD size );


// -----------------------------------------
// ordinary memory management
typedef tERROR  (pr_call *tObjHeapAlloc        )( hOBJECT object, tPTR* result, tDWORD size );
typedef tERROR  (pr_call *tObjHeapRealloc      )( hOBJECT object, tPTR* result, tPTR orig, tDWORD size );
typedef tERROR  (pr_call *tObjHeapFree         )( hOBJECT object, tPTR ptr );
typedef tERROR  (pr_call *tObjHeapResolve      )( hOBJECT object, hHEAP* result, tPTR ptr );

// -----------------------------------------
// recognize methods
typedef tERROR  (pr_call *tRecognize            )( hOBJECT object, tIID by_iid, tPID by_pid, tDWORD type ); 
typedef tERROR  (pr_call *tRecognizeByList      )( hOBJECT object, tPID* result, tIID by_iid, const tPID* by_pid_list, tDWORD count, tDWORD type ); 

// -----------------------------------------
// error management
typedef tERROR  (pr_call *tErrorCodeGetOld     )( hOBJECT object );


// -----------------------------------------
// message management
typedef tDWORD  (pr_call *tBroadcastMsg            )( hOBJECT object, tIID iid, tDWORD type, tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen );
typedef tERROR  (pr_call *tSendMsg                 )( hOBJECT object, tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen );
typedef tERROR  (pr_call *tIntMsgReceive           )( hOBJECT object, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen );
typedef tERROR  (pr_call *tRegisterMsgHandler      )( hOBJECT object, tDWORD msg_cls, tDWORD flags, tPTR tree_top, tIID iid_obj, tPID pid_obj, tIID iid_ctx, tPID pid_ctx );
typedef tERROR  (pr_call *tRegisterMsgHandlerList  )( hOBJECT object, const tMsgHandlerDescr* handler_list, tDWORD handler_count );
typedef tERROR  (pr_call *tGetMsgHandlerList       )( hOBJECT object, tDWORD* result_count, tMsgHandlerDescr* handler_list, tDWORD handler_count );
typedef tERROR  (pr_call *tUnregisterMsgHandler    )( hOBJECT object, tDWORD msg_cls, tPTR tree_top );
typedef tERROR  (pr_call *tUnregisterMsgHandlerList)( hOBJECT object, const tMsgHandlerDescr* handler_list, tDWORD handler_count );


// -----------------------------------------
// trace routines
typedef tERROR  (pr_call *tTraceLevelGet       )( hOBJECT object, tDWORD who, tDWORD* max_level, tDWORD* min_level );
typedef tERROR  (pr_call *tTraceLevelSet       )( hOBJECT object, tDWORD who, tDWORD max_level, tDWORD min_level );
typedef tERROR  (pr_call *tTracerGet           )( hOBJECT object, hTRACER* result );


// -----------------------------------------
// code protection routines
typedef tERROR  (pr_call *tEnterProtectedCode  )( hOBJECT object, tExceptionReg* reg );
typedef tERROR  (pr_call *tLeaveProtectedCode  )( hOBJECT object, tExceptionReg* reg );
typedef tERROR  (pr_call *tGetExceptionInfo    )( hOBJECT object, tExceptionReg* reg, tDWORD* excpt_code, tPTR* info );
typedef tERROR  (pr_call *tThrowException      )( hOBJECT object, tDWORD excpt_code );


// -----------------------------------------
// synchronization routines
typedef tERROR  (pr_call *tSyncCreate          )( hOBJECT object, hOBJECT* sync_result, tIID sync_iid, tPID sync_pid, tSTRING name );
typedef tERROR  (pr_call *tSyncSet             )( hOBJECT object, hOBJECT sync_obj );
typedef tERROR  (pr_call *tSyncGet             )( hOBJECT object, hOBJECT* sync_result );
typedef tERROR  (pr_call *tSyncClose           )( hOBJECT object );
typedef tERROR  (pr_call *tSyncLock            )( hOBJECT object, tDWORD wait_time );
typedef tERROR  (pr_call *tSyncUnlock          )( hOBJECT object );
typedef tERROR  (pr_call *tSyncProtectObject   )( hOBJECT object, tIID sync_iid, tPID sync_pid, tSTRING name );
typedef tERROR  (pr_call *tSyncUnprotectObject )( hOBJECT object );


// -----------------------------------------
// user data management
typedef tERROR  (pr_call *tUserDataGet         )( hOBJECT object, tDWORD* data );
typedef tERROR  (pr_call *tUserDataSet         )( hOBJECT object, tDWORD  data );

//EXTERN_C_END


#if defined(__cplusplus) && !defined(_USE_VTBL)

// -----------------------------------------
// object management
#define CALL_SYS_ObjectCreate(obj,new_obj,iid,pid,subtype)                ((obj)->sysCreateObject    (((hOBJECT*)new_obj),iid,pid,subtype))
#define CALL_SYS_ObjectCreateDone(obj)                                    ((obj)->sysCreateObjectDone())
#define CALL_SYS_ObjectRevert(obj,reuse_custom_prop)                      ((obj)->sysRevertObject    (reuse_custom_prop))
#define CALL_SYS_ObjectCreateQuick(obj,new_obj,iid,pid,subtype)           ((obj)->sysCreateObjectQuick(((hOBJECT*)new_obj),iid,pid,subtype))
#define CALL_SYS_ObjectCheck(obj,to_check,iid,pid,subtype,do_not_check_int_struct)     ((obj)->sysCheckObject      (to_check,iid,pid,subtype,do_not_check_int_struct))
#define CALL_SYS_ObjectValid(obj,to_check)                                ((obj)->sysIsValidObject    (to_check))
#define CALL_SYS_ObjectClose(obj)                                         ((obj)->sysCloseObject      ())
#define CALL_SYS_ObjectSwap(obj,o1,o2)                                    ((obj)->sysSwapObject       (((hOBJECT)o1),((hOBJECT)o2)))
#define CALL_SYS_ObjectFind(obj,result,iid,pid,up,down,flags)             ((obj)->sysFindObject       (((hOBJECT*)result),iid,pid,up,down,flags))
#define CALL_SYS_ObjectGetInternalData(obj,size,obj_size,data,data_size,convert,prop_enum,ctx) ((obj)->sysGetInternalData(size,obj_size,data,data_size,convert,prop_enum,ctx))
#define CALL_SYS_ObjectSetInternalData(obj,size,obj_size,data,data_size,convert,ctx)           ((obj)->sysSetInternalData(size,obj_size,data,data_size,convert,ctx))
#define CALL_SYS_ValidateLock(obj,to_lock,iid,pid)                        ((obj)->sysValidateLock     (to_lock,iid,pid))
#define CALL_SYS_Release(obj)                                             ((obj)->sysRelease())

// -----------------------------------------
// low level object management
#define CALL_SYS_HandleCreate(obj,new_obj,memsize)                        ((obj)->sysCreateHandle       (((hOBJECT*)new_obj),memsize))
#define CALL_SYS_InterfaceSet(obj,iid,pid,subtype)                        ((obj)->sysSetIface       (iid,pid,subtype))

// -----------------------------------------
// parent/child/object enumerate management
#define CALL_SYS_ParentGet(obj,iid)                                       ((obj)->sysGetParent          (iid))
#define CALL_SYS_ParentSet(obj,result,new_parent)                         ((obj)->sysSetParent          (result,new_parent))
#define CALL_SYS_ChildGetFirst(obj,ret_obj,iid,pid)                       ((obj)->sysGetFirstChild      (ret_obj,iid,pid))
#define CALL_SYS_ChildGetLast(obj,ret_obj,iid,pid)                        ((obj)->sysGetLastChild       (ret_obj,iid,pid))
#define CALL_SYS_ObjectGetNext(obj,ret_obj,iid,pid)                       ((obj)->sysGetNextObject      (ret_obj,iid,pid))
#define CALL_SYS_ObjectGetPrev(obj,ret_obj,iid,pid)                       ((obj)->sysGetPrevObject      (ret_obj,iid,pid))

// -----------------------------------------
// property management
#define CALL_SYS_PropertyIsPresent(obj,prop_id)                           ((obj)->propIsPresent  (prop_id))
#define CALL_SYS_PropertyGet(obj,out_size,prop_id,buffer,size)            ((obj)->propGet        (out_size,prop_id,buffer,size))
#define CALL_SYS_PropertyGetBool(obj,prop_id)                             ((obj)->propGetBool    (prop_id))
#define CALL_SYS_PropertyGetByte(obj,prop_id)                             ((obj)->propGetByte    (prop_id))
#define CALL_SYS_PropertyGetWord(obj,prop_id)                             ((obj)->propGetWord    (prop_id))
#define CALL_SYS_PropertyGetUInt(obj,prop_id)                             ((obj)->propGetUInt    (prop_id))
#define CALL_SYS_PropertyGetDWord(obj,prop_id)                            ((obj)->propGetDWord   (prop_id))
#define CALL_SYS_PropertyGetQWord(obj,prop_id)                            ((obj)->propGetQWord   (prop_id))
#define CALL_SYS_PropertyGetData(obj,prop_id)                             ((obj)->propGetData    (prop_id))
#define CALL_SYS_PropertyGetPtr(obj,prop_id)                              ((obj)->propGetPtr     (prop_id))
#define CALL_SYS_PropertyGetObj(obj,prop_id)                              ((obj)->propGetObj     (prop_id))
#define CALL_SYS_PropertyGetStr(obj,out_size,prop_id,buffer,size,receive_cp) ((obj)->propGetStr     (out_size,prop_id,buffer,size,receive_cp))
#define CALL_SYS_PropertyGetStrCP(obj,cp,prop_id)                         ((obj)->propGetStrCP   (cp,prop_id))

#define CALL_SYS_PropertySet(obj,out_size,prop_id,buffer,size)            ((obj)->propSet        (out_size,prop_id,buffer,size))
#define CALL_SYS_PropertySetBool(obj,prop_id,val)                         ((obj)->propSetBool    (prop_id,val))
#define CALL_SYS_PropertySetByte(obj,prop_id,val)                         ((obj)->propSetByte    (prop_id,val))
#define CALL_SYS_PropertySetWord(obj,prop_id,val)                         ((obj)->propSetWord    (prop_id,val))
#define CALL_SYS_PropertySetUInt(obj,prop_id,val)                         ((obj)->propSetUInt    (prop_id,val))
#define CALL_SYS_PropertySetDWord(obj,prop_id,val)                        ((obj)->propSetDWord   (prop_id,val))
#define CALL_SYS_PropertySetQWord(obj,prop_id,val)                        ((obj)->propSetQWord   (prop_id,val))
#define CALL_SYS_PropertySetData(obj,prop_id)                             ((obj)->propSetData    (prop_id))
#define CALL_SYS_PropertySetPtr(obj,prop_id,val)                          ((obj)->propSetPtr     (prop_id,val))
#define CALL_SYS_PropertySetObj(obj,prop_id,val)                          ((obj)->propSetObj     (prop_id,val))
#define CALL_SYS_PropertySetStr(obj,out_size,prop_id,pStrz,size,src_cp)   ((obj)->propSetStr     (out_size,prop_id,pStrz,size,src_cp))

#define CALL_SYS_PropertyDelete(obj,prop_id)                              ((obj)->propDelete     (prop_id))
#define CALL_SYS_PropertySize(obj,prop_id)                                ((obj)->propSize       (prop_id))
#define CALL_SYS_PropertyEnumId(obj,prop_id,cookie)                       ((obj)->propEnumId     (prop_id,cookie))
#define CALL_SYS_PropertyEnumCustomId(obj,prop_id,cookie)                 ((obj)->propEnumCustomId(prop_id,cookie))

#define CALL_SYS_PropertyBroadcast(obj,iid,type,prop_id,buffer,size)      ((obj)->propBroadcast  (iid,type,prop_id,buffer,size))


// -----------------------------------------
// ordinary memory management
#define CALL_SYS_ObjHeapAlloc(obj,result,size)                            ((obj)->heapAlloc       (result,size))
#define CALL_SYS_ObjHeapRealloc(obj,result,orig,size)                     ((obj)->heapRealloc     (result,orig,size))
#define CALL_SYS_ObjHeapFree(obj,ptr)                                     ((obj)->heapFree        (ptr))
#define CALL_SYS_ObjHeapResolve(obj,result,ptr)                           ((obj)->heapResolve     (result,ptr))


// -----------------------------------------
// recognize methods
#define CALL_SYS_Recognize(obj,by_iid,by_pid,type)                         ((obj)->sysRecognize         (by_iid,by_pid,type))
#define CALL_SYS_RecognizeByList(obj,result,by_iid,by_pid_list,count,type) ((obj)->sysRecognizeByList   (result,by_iid,by_pid_list,count,type))

// -----------------------------------------
// error management
#define CALL_SYS_ErrorCodeGet(obj)                                        ((obj)->errorCodeGetOld       ())


// -----------------------------------------
// message management
#define CALL_SYS_BroadcastMsg(obj,iid,type,msg_cls,msg_id,ctx,pbuff,pblen)                      ((obj)->sysBroadcastMsg(iid,type,msg_cls,msg_id,ctx,pbuff,pblen))
#define CALL_SYS_SendMsg(obj,msg_cls,msg_id,ctx,pbuff,pblen)                                    ((obj)->sysSendMsg(msg_cls,msg_id,ctx,pbuff,pblen))
#define CALL_SYS_IntMsgReceive(obj,msg_cls,msg_id,send_point,ctx,recieve_point,pbuff,blen)      ((obj)->sysIntMsgReceive(msg_cls,msg_id,send_point,ctx,recieve_point,pbuff,blen))
#define CALL_SYS_RegisterMsgHandler(obj,msg_cls,flags,tree_top,iid_obj,pid_obj,iid_ctx,pid_ctx) ((obj)->sysRegisterMsgHandler(msg_cls,flags,tree_top,iid_obj,pid_obj,iid_ctx,pid_ctx))
#define CALL_SYS_RegisterMsgHandlerList(obj,handler_list,handler_count)                         ((obj)->sysRegisterMsgHandlerList(handler_list,handler_count))
#define CALL_SYS_GetMsgHandlerList(obj,result_count,handler_list,handler_count)                 ((obj)->sysGetMsgHandlerList(result_count,handler_list,handler_count))
#define CALL_SYS_UnregisterMsgHandler(obj,msg_cls,tree_top)                                     ((obj)->sysUnregisterMsgHandler(msg_cls,tree_top))
#define CALL_SYS_UnregisterMsgHandlerList(obj,handler_list,handler_count)                       ((obj)->sysUnregisterMsgHandlerList(handler_list,handler_count))


// -----------------------------------------
// trace routines
#define CALL_SYS_TraceLevelGet(obj,who,max_level,min_level)               ((obj)->sysGetTraceLevel      (who,max_level,min_level))
#define CALL_SYS_TraceLevelSet(obj,who,max_level,min_level)               ((obj)->sysSetTraceLevel      (who,max_level,min_level))
#define CALL_SYS_TracerGet(obj,result)                                    ((obj)->sysGetTracer          (result))


// -----------------------------------------
// code protection routines
#define CALL_SYS_EnterProtectedCode(obj,reg)                              ((obj)->sysEnterProtectedCode (reg))
#define CALL_SYS_LeaveProtectedCode(obj,reg)                              ((obj)->sysLeaveProtectedCode (reg))
#define CALL_SYS_GetExceptionInfo(obj,reg,excpt_code,info)                ((obj)->sysGetExceptionInfo   (reg,excpt_code,info))
#define CALL_SYS_ThrowException(obj,excpt_code)                           ((obj)->sysThrowException     (excpt_code))


// -----------------------------------------
// synchronization routines
#define CALL_SYS_SyncCreate(obj,sync_result,type,name,semaphore_count)    ((obj)->syncCreate         (sync_result,type,name,semaphore_count))
#define CALL_SYS_SyncSet(obj,sync_obj)                                    ((obj)->syncSet            (sync_obj))
#define CALL_SYS_SyncGet(obj,sync_result)                                 ((obj)->syncGet            (sync_result))
#define CALL_SYS_SyncClose(obj)                                           ((obj)->syncClose          ())
#define CALL_SYS_SyncLock(obj,wait_time)                                  ((obj)->syncLock           (wait_time))
#define CALL_SYS_SyncUnlock(obj)                                          ((obj)->syncUnlock         ())
#define CALL_SYS_SyncProtectObject(obj,sync_iid,sync_pid,name)            ((obj)->syncProtectObject  (sync_iid,sync_pid,name))
#define CALL_SYS_SyncUnprotectObject(obj)                                 ((obj)->syncUnprotectObject())


// -----------------------------------------
// user data management
//#define CALL_SYS_UserDataGet(obj,data)                                    ((obj)->sysGetUserData        (data))
//#define CALL_SYS_UserDataSet(obj,data)                                    ((obj)->sysSetUserData        (data))

#else //if defined(__cplusplus) && !defined(_USE_VTBL)

// -----------------------------------------
// object management
#define CALL_SYS_ObjectCreate(obj,new_obj,iid,pid,subtype)                ((obj)->sys->ObjectCreate       (((hOBJECT)(obj)),new_obj,iid,pid,subtype))
#define CALL_SYS_ObjectCreateDone(obj)                                    ((obj)->sys->ObjectCreateDone   (((hOBJECT)(obj))))
#define CALL_SYS_ObjectRevert(obj,reuse_custom_prop)                      ((obj)->sys->ObjectRevert(((hOBJECT)(obj)),reuse_custom_prop))
#define CALL_SYS_ObjectCreateQuick(obj,new_obj,iid,pid,subtype)           ((obj)->sys->ObjectCreateQuick  (((hOBJECT)(obj)),new_obj,iid,pid,subtype))
#define CALL_SYS_ObjectCheck(obj,to_check,iid,pid,subtype,do_not_check_int_struct)     ((obj)->sys->ObjectCheck        (((hOBJECT)(obj)),to_check,iid,pid,subtype,do_not_check_int_struct))
#define CALL_SYS_ObjectValid(obj,to_check)                                ((obj)->sys->ObjectValid        (((hOBJECT)(obj)),to_check))
#define CALL_SYS_ObjectClose(obj)                                         ((obj)->sys->ObjectClose        (((hOBJECT)(obj))))
#define CALL_SYS_ObjectSwap(obj,o1,o2)                                    ((obj)->sys->ObjectSwap         (((hOBJECT)(obj)),((hOBJECT)o1),((hOBJECT)o2)))
#define CALL_SYS_ObjectFind(obj,result,iid,pid,up,down,flags)             ((obj)->sys->ObjectFind         (((hOBJECT)(obj)),((hOBJECT*)result),iid,pid,up,down,flags))
#define CALL_SYS_ObjectGetInternalData(obj,size,obj_size,data,data_size,convert,prop_enum,ctx) ((obj)->sys->ObjectGetInternalData(((hOBJECT)(obj)),size,obj_size,data,data_size,convert,prop_enum,ctx))
#define CALL_SYS_ObjectSetInternalData(obj,size,obj_size,data,data_size,convert,ctx)           ((obj)->sys->ObjectSetInternalData(((hOBJECT)(obj)),size,obj_size,data,data_size,convert,ctx))
#define CALL_SYS_ValidateLock(obj,to_lock,iid,pid)                        ((obj)->sys->ValidateLock     (((hOBJECT)(obj)),to_lock,iid,pid))
#define CALL_SYS_Release(obj)                                             ((obj)->sys->Release(((hOBJECT)(obj))))

// -----------------------------------------
// low level object management
#define CALL_SYS_HandleCreate(obj,new_obj,memsize)                        ((obj)->sys->HandleCreate       (((hOBJECT)(obj)),new_obj,memsize))
#define CALL_SYS_InterfaceSet(obj,iid,pid,subtype)                        ((obj)->sys->InterfaceSet       (((hOBJECT)(obj)),iid,pid,subtype))

// -----------------------------------------
// parent/child/object enumerate management
#define CALL_SYS_ParentGet(obj,iid)                                       ((obj)->sys->ParentGet          (((hOBJECT)(obj)),iid))
#define CALL_SYS_ParentSet(obj,result,new_parent)                         ((obj)->sys->ParentSet          (((hOBJECT)(obj)),result,new_parent))
#define CALL_SYS_ChildGetFirst(obj,ret_obj,iid,pid)                       ((obj)->sys->ChildGetFirst      (((hOBJECT)(obj)),ret_obj,iid,pid))
#define CALL_SYS_ChildGetLast(obj,ret_obj,iid,pid)                        ((obj)->sys->ChildGetLast       (((hOBJECT)(obj)),ret_obj,iid,pid))
#define CALL_SYS_ObjectGetNext(obj,ret_obj,iid,pid)                       ((obj)->sys->ObjectGetNext      (((hOBJECT)(obj)),ret_obj,iid,pid))
#define CALL_SYS_ObjectGetPrev(obj,ret_obj,iid,pid)                       ((obj)->sys->ObjectGetPrev      (((hOBJECT)(obj)),ret_obj,iid,pid))

// -----------------------------------------
// property management
#define CALL_SYS_PropertyIsPresent(obj,prop_id)                           ((obj)->sys->PropertyIsPresent  (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGet(obj,out_size,prop_id,buffer,size)            ((obj)->sys->PropertyGet        (((hOBJECT)(obj)),out_size,prop_id,buffer,size))
#define CALL_SYS_PropertyGetBool(obj,prop_id)                             ((obj)->sys->PropertyGetBool    (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGetByte(obj,prop_id)                             ((obj)->sys->PropertyGetByte    (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGetWord(obj,prop_id)                             ((obj)->sys->PropertyGetWord    (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGetUInt(obj,prop_id)                             ((obj)->sys->PropertyGetUInt    (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGetDWord(obj,prop_id)                            ((obj)->sys->PropertyGetDWord   (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGetQWord(obj,prop_id)                            ((obj)->sys->PropertyGetQWord   (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGetData(obj,prop_id)                             ((obj)->sys->PropertyGetQData   (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGetPtr(obj,prop_id)                              ((obj)->sys->PropertyGetPtr     (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGetObj(obj,prop_id)                              ((obj)->sys->PropertyGetObj     (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyGetStr(obj,out_size,prop_id,buffer,size,receive_cp) ((obj)->sys->PropertyGetStr     (((hOBJECT)(obj)),out_size,prop_id,buffer,size,receive_cp))
#define CALL_SYS_PropertyGetStrCP(obj,cp,prop_id)                         ((obj)->sys->PropertyGetStrCP   (((hOBJECT)(obj)),cp,prop_id))

#define CALL_SYS_PropertySet(obj,out_size,prop_id,buffer,size)            ((obj)->sys->PropertySet        (((hOBJECT)(obj)),out_size,prop_id,buffer,size))
#define CALL_SYS_PropertySetBool(obj,prop_id,val)                         ((obj)->sys->PropertySetBool    (((hOBJECT)(obj)),prop_id,val))
#define CALL_SYS_PropertySetByte(obj,prop_id,val)                         ((obj)->sys->PropertySetByte    (((hOBJECT)(obj)),prop_id,val))
#define CALL_SYS_PropertySetWord(obj,prop_id,val)                         ((obj)->sys->PropertySetWord    (((hOBJECT)(obj)),prop_id,val))
#define CALL_SYS_PropertySetUInt(obj,prop_id,val)                         ((obj)->sys->PropertySetUInt    (((hOBJECT)(obj)),prop_id,val))
#define CALL_SYS_PropertySetDWord(obj,prop_id,val)                        ((obj)->sys->PropertySetDWord   (((hOBJECT)(obj)),prop_id,val))
#define CALL_SYS_PropertySetQWord(obj,prop_id,val)                        ((obj)->sys->PropertySetQWord   (((hOBJECT)(obj)),prop_id,val))
#define CALL_SYS_PropertySetData(obj,prop_id)                             ((obj)->sys->PropertySetData    (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertySetPtr(obj,prop_id,val)                          ((obj)->sys->PropertySetPtr     (((hOBJECT)(obj)),prop_id,val))
#define CALL_SYS_PropertySetObj(obj,prop_id,val)                          ((obj)->sys->PropertySetObj     (((hOBJECT)(obj)),prop_id,val))
#define CALL_SYS_PropertySetStr(obj,out_size,prop_id,pStrz,size,src_cp)   ((obj)->sys->PropertySetStr     (((hOBJECT)(obj)),out_size,prop_id,pStrz,size,src_cp))

#define CALL_SYS_PropertyDelete(obj,prop_id)                              ((obj)->sys->PropertyDelete     (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertySize(obj,prop_id)                                ((obj)->sys->PropertySize       (((hOBJECT)(obj)),prop_id))
#define CALL_SYS_PropertyEnumId(obj,prop_id,cookie)                       ((obj)->sys->PropertyEnumId     (((hOBJECT)(obj)),prop_id,cookie))
#define CALL_SYS_PropertyEnumCustomId(obj,prop_id,cookie)                 ((obj)->sys->PropertyEnumCustomId(((hOBJECT)(obj)),prop_id,cookie))

#define CALL_SYS_PropertyBroadcast(obj,iid,type,prop_id,buffer,size)      ((obj)->sys->PropertyBroadcast  (((hOBJECT)(obj)),iid,type,prop_id,buffer,size))


// -----------------------------------------
// ordinary memory management
#define CALL_SYS_ObjHeapAlloc(obj,result,size)                            ((obj)->sys->ObjHeapAlloc       (((hOBJECT)(obj)),result,size))
#define CALL_SYS_ObjHeapRealloc(obj,result,orig,size)                     ((obj)->sys->ObjHeapRealloc     (((hOBJECT)(obj)),result,orig,size))
#define CALL_SYS_ObjHeapFree(obj,ptr)                                     ((obj)->sys->ObjHeapFree        (((hOBJECT)(obj)),ptr))
#define CALL_SYS_ObjHeapResolve(obj,result,ptr)                           ((obj)->sys->ObjHeapResolve     (((hOBJECT)(obj)),result,ptr))


// -----------------------------------------
// recognize methods
#define CALL_SYS_Recognize(obj,by_iid,by_pid,type)                         ((obj)->sys->Recognize         (((hOBJECT)(obj)),by_iid,by_pid,type))
#define CALL_SYS_RecognizeByList(obj,result,by_iid,by_pid_list,count,type) ((obj)->sys->RecognizeByList   (((hOBJECT)(obj)),result,by_iid,by_pid_list,count,type))

// -----------------------------------------
// error management
#define CALL_SYS_ErrorCodeGet(obj)                                        ((obj)->sys->ErrorCodeGet       (((hOBJECT)(obj))))


// -----------------------------------------
// message management
#define CALL_SYS_BroadcastMsg(obj,iid,type,msg_cls,msg_id,ctx,pbuff,pblen)                      ((obj)->sys->BroadcastMsg(((hOBJECT)(obj)),iid,type,msg_cls,msg_id,ctx,pbuff,pblen))
#define CALL_SYS_SendMsg(obj,msg_cls,msg_id,ctx,pbuff,pblen)                                    ((obj)->sys->SendMsg(((hOBJECT)(obj)),msg_cls,msg_id,ctx,pbuff,pblen))
#define CALL_SYS_IntMsgReceive(obj,msg_cls,msg_id,send_point,ctx,recieve_point,pbuff,blen)      ((obj)->sys->IntMsgReceive(((hOBJECT)(obj)),msg_cls,msg_id,send_point,ctx,recieve_point,pbuff,blen))
#define CALL_SYS_RegisterMsgHandler(obj,msg_cls,flags,tree_top,iid_obj,pid_obj,iid_ctx,pid_ctx) ((obj)->sys->RegisterMsgHandler(((hOBJECT)(obj)),msg_cls,flags,tree_top,iid_obj,pid_obj,iid_ctx,pid_ctx))
#define CALL_SYS_RegisterMsgHandlerList(obj,handler_list,handler_count)                         ((obj)->sys->RegisterMsgHandlerList(((hOBJECT)(obj)),handler_list,handler_count))
#define CALL_SYS_GetMsgHandlerList(obj,result_count,handler_list,handler_count)                 ((obj)->sys->GetMsgHandlerList(((hOBJECT)(obj)),result_count,handler_list,handler_count))
#define CALL_SYS_UnregisterMsgHandler(obj,msg_cls,tree_top)                                     ((obj)->sys->UnregisterMsgHandler(((hOBJECT)(obj)),msg_cls,tree_top))
#define CALL_SYS_UnregisterMsgHandlerList(obj,handler_list,handler_count)                       ((obj)->sys->UnregisterMsgHandlerList(((hOBJECT)(obj)),handler_list,handler_count))


// -----------------------------------------
// trace routines
#define CALL_SYS_TraceLevelGet(obj,who,max_level,min_level)               ((obj)->sys->TraceLevelGet      (((hOBJECT)(obj)),who,max_level,min_level))
#define CALL_SYS_TraceLevelSet(obj,who,max_level,min_level)               ((obj)->sys->TraceLevelSet      (((hOBJECT)(obj)),who,max_level,min_level))
#define CALL_SYS_TracerGet(obj,result)                                    ((obj)->sys->TracerGet          (((hOBJECT)(obj)),result))


// -----------------------------------------
// code protection routines
#define CALL_SYS_EnterProtectedCode(obj,reg)                              ((obj)->sys->EnterProtectedCode (((hOBJECT)(obj)),reg))
#define CALL_SYS_LeaveProtectedCode(obj,reg)                              ((obj)->sys->LeaveProtectedCode (((hOBJECT)(obj)),reg))
#define CALL_SYS_GetExceptionInfo(obj,reg,excpt_code,info)                ((obj)->sys->GetExceptionInfo   (((hOBJECT)(obj)),reg,excpt_code,info))
#define CALL_SYS_ThrowException(obj,excpt_code)                           ((obj)->sys->ThrowException     (((hOBJECT)(obj)),excpt_code))


// -----------------------------------------
// synchronization routines
#define CALL_SYS_SyncCreate(obj,sync_result,type,name,semaphore_count)    ((obj)->sys->SyncCreate         (((hOBJECT)(obj)),sync_result,type,name,semaphore_count))
#define CALL_SYS_SyncSet(obj,sync_obj)                                    ((obj)->sys->SyncSet            (((hOBJECT)(obj)),sync_obj))
#define CALL_SYS_SyncGet(obj,sync_result)                                 ((obj)->sys->SyncGet            (((hOBJECT)(obj)),sync_result))
#define CALL_SYS_SyncClose(obj)                                           ((obj)->sys->SyncClose          (((hOBJECT)(obj))))
#define CALL_SYS_SyncLock(obj,wait_time)                                  ((obj)->sys->SyncLock           (((hOBJECT)(obj)),wait_time))
#define CALL_SYS_SyncUnlock(obj)                                          ((obj)->sys->SyncUnlock         (((hOBJECT)(obj))))
#define CALL_SYS_SyncProtectObject(obj,sync_iid,sync_pid,name)            ((obj)->sys->SyncProtectObject  (((hOBJECT)(obj)),sync_iid,sync_pid,name))
#define CALL_SYS_SyncUnprotectObject(obj)                                 ((obj)->sys->SyncUnprotectObject(((hOBJECT)(obj))))


// -----------------------------------------
// user data management
//#define CALL_SYS_UserDataGet(obj,data)                                    ((obj)->sys->UserDataGet        (((hOBJECT)(obj)),data))
//#define CALL_SYS_UserDataSet(obj,data)                                    ((obj)->sys->UserDataSet        (((hOBJECT)(obj)),data))

#endif //if defined(__cplusplus) && !defined(_USE_VTBL)


struct tag_iSYSTEMVtbl {
	tPropertyGet               PropertyGet;
	tPropertyGetBool           PropertyGetBool;
	tPropertyGetWord           PropertyGetWord;
	tPropertyGetUInt           PropertyGetUInt;
	tPropertyGetDWord          PropertyGetDWord;
	tPropertyGetQWord          PropertyGetQWord;

	tPropertySet               PropertySet;
	tPropertySetBool           PropertySetBool;
	tPropertySetWord           PropertySetWord;
	tPropertySetUInt           PropertySetUInt;
	tPropertySetDWord          PropertySetDWord;
	tPropertySetQWord          PropertySetQWord;

	tPropertyDelete            PropertyDelete;
	tPropertySize              PropertySize;

	tPropertyBroadcast         PropertyBroadcast;

	tParentGet                 ParentGet;
	tParentSet                 ParentSet;
	tHandleCreate              HandleCreate;
	tInterfaceSet              InterfaceSet;
	tObjectCreate              ObjectCreate;
	tObjectCreateDone          ObjectCreateDone;
	tObjectCheck               ObjectCheck;
	tObjectValid               ObjectValid;
	tObjectClose               ObjectClose;
	tObjectSwap                ObjectSwap;
	tObjectFind                ObjectFind;

	tErrorCodeGetOld           ErrorCodeGetOld;

	tBroadcastMsg              BroadcastMsg;
	tSendMsg                   SendMsg;
	tRegisterMsgHandler        RegisterMsgHandler;
	tUnregisterMsgHandler      UnregisterMsgHandler;

	tTraceLevelGet             TraceLevelGet;
	tTraceLevelSet             TraceLevelSet;
	tTracerGet                 TracerGet;

	tEnterProtectedCode        EnterProtectedCode;
	tLeaveProtectedCode        LeaveProtectedCode;
	tGetExceptionInfo          GetExceptionInfo;
	tThrowException            ThrowException;

	tSyncCreate                SyncCreate;
	tSyncGet                   SyncGet;
	tSyncClose                 SyncClose;
	tSyncLock                  SyncLock;
	tSyncUnlock                SyncUnlock;
	tSyncProtectObject         SyncProtectObject;
	tSyncUnprotectObject       SyncUnprotectObject;

	tObjectCreateQuick         ObjectCreateQuick;

	tUserDataGet               UserDataGet;
	tUserDataSet               UserDataSet;

	tChildGetFirst             ChildGetFirst;
	tChildGetLast              ChildGetLast;
	tObjectGetNext             ObjectGetNext;
	tObjectGetPrev             ObjectGetPrev;

	tObjHeapAlloc              ObjHeapAlloc;
	tObjHeapRealloc            ObjHeapRealloc;
	tObjHeapFree               ObjHeapFree;
	tObjHeapResolve            ObjHeapResolve;

	tSyncSet                   SyncSet;

	tPropertyGetStr            PropertyGetStr;
	tPropertySetStr            PropertySetStr;
	tPropertyGetStrCP          PropertyGetStrCP;
	tPropertyIsPresent         PropertyIsPresent;
	tObjectRevert              ObjectRevert;

  tPropertyGetPtr            PropertyGetPtr;
  tPropertyGetObj            PropertyGetObj;
  tPropertySetPtr            PropertySetPtr;
  tPropertySetObj            PropertySetObj;
  
	tPropertyEnumId            PropertyEnumId;
  tPropertyEnumCustomId      PropertyEnumCustomId;

	tPropertyGetByte           PropertyGetByte;
	tPropertySetByte           PropertySetByte;

  tRegisterMsgHandlerList    RegisterMsgHandlerList;
  tGetMsgHandlerList         GetMsgHandlerList;
  tUnregisterMsgHandlerList  UnregisterMsgHandlerList;

  tIntMsgReceive             IntMsgReceive;

  tObjectGetInternalData     ObjectGetInternalData;
  tObjectSetInternalData     ObjectSetInternalData;

  tRecognize                 Recognize;
  tRecognizeByList           RecognizeByList;

	tValidateLock              ValidateLock;
	tRelease                   Release;

	tPropertyGetData           PropertyGetData;
	tPropertySetData           PropertySetData;

  tReservedMeth              reserved[RESERVED_COUNT];
};



#if defined( __cplusplus )

#define PR_PURE_RET_ERR  { return errOBJECT_NOT_INITIALIZED; }
#define PR_PURE_RET_ZERO { return 0; }

// ---
struct pr_novtable iObj {

protected:
	iObj() {}

public:
  // property management
  virtual tERROR   pr_call propGet( tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size ) PR_PURE_RET_ERR;
  virtual tBOOL    pr_call propGetBool( tPROPID prop_id ) PR_PURE_RET_ZERO;
  virtual tWORD    pr_call propGetWord( tPROPID prop_id ) PR_PURE_RET_ZERO;
  virtual tUINT    pr_call propGetUInt( tPROPID prop_id ) PR_PURE_RET_ZERO;
  virtual tDWORD   pr_call propGetDWord( tPROPID prop_id ) PR_PURE_RET_ZERO;
  virtual tQWORD   pr_call propGetQWord( tPROPID prop_id ) PR_PURE_RET_ZERO;
          
  virtual tERROR   pr_call propSet( tDWORD* out_size, tPROPID prop_id, const tPTR buffer, tDWORD size ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call propSetBool( tPROPID prop_id, tBOOL  val ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call propSetWord( tPROPID prop_id, tWORD  val ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call propSetUInt( tPROPID prop_id, tUINT  val ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call propSetDWord( tPROPID prop_id, tDWORD val ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call propSetQWord( tPROPID prop_id, tQWORD val ) PR_PURE_RET_ERR;
          
  virtual tERROR   pr_call propDelete( tPROPID prop_id ) PR_PURE_RET_ERR;
  virtual tDWORD   pr_call propSize( tPROPID prop_id ) PR_PURE_RET_ZERO;
  virtual tDWORD   pr_call propBroadcast( tIID iid, tDWORD type, tPROPID prop_id, const tPTR buffer, tDWORD size ) PR_PURE_RET_ZERO;
  
  virtual hOBJECT  pr_call sysGetParent( tIID iid ) PR_PURE_RET_ZERO;
  virtual tERROR   pr_call sysSetParent( hOBJECT* old_parent, hOBJECT new_parent ) PR_PURE_RET_ERR;
  
  // low level object management
  virtual tERROR   pr_call sysCreateHandle( hOBJECT dad, tDWORD memsize ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysSetIface( tIID iid, tPID pid,tDWORD subtype ) PR_PURE_RET_ERR;
  
  // object management
  virtual tERROR   pr_call sysCreateObject( hOBJECT* obj, tIID iid, tPID pid=PID_ANY, tDWORD subtype=SUBTYPE_ANY ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysCreateObjectDone() PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysCheckObject( hOBJECT to_check, tIID iid=IID_ANY, tPID pid=PID_ANY, tDWORD subtype=SUBTYPE_ANY, tBOOL do_not_check_int_struct=cFALSE ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysIsValidObject( hOBJECT to_check ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysCloseObject() PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysSwapObject( hOBJECT o1, hOBJECT o2 ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysFindObject( hOBJECT* result, tIID iid, tPID pid, tDWORD up, tDWORD down, tDWORD flags ) PR_PURE_RET_ERR;
  
  virtual tERROR   pr_call errorCodeGetOld() PR_PURE_RET_ERR;
  
  // message management
  virtual tDWORD   pr_call sysBroadcastMsg( tIID iid, tDWORD type, tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen ) PR_PURE_RET_ZERO;
  virtual tERROR   pr_call sysSendMsg( tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysRegisterMsgHandler( tDWORD msg_cls, tDWORD flags, tPTR tree_top, tIID iid_obj, tPID pid_obj, tIID iid_ctx, tPID pid_ctx ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysUnregisterMsgHandler( tDWORD msg_cls, tPTR tree_top ) PR_PURE_RET_ERR;
  
  // trace routines
  virtual tERROR   pr_call sysGetTraceLevel( tDWORD who, tDWORD* max_level, tDWORD* min_level ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysSetTraceLevel( tDWORD who, tDWORD max_level, tDWORD min_level ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysGetTracer( hTRACER* result ) PR_PURE_RET_ERR;
  
  // code protection routines
  virtual tERROR   pr_call sysEnterProtectedCode( tExceptionReg* reg ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysLeaveProtectedCode( tExceptionReg* reg ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysGetExceptionInfo  ( tExceptionReg* reg, tDWORD* excpt_code, tPTR* info ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysThrowException    ( tDWORD excpt_code ) PR_PURE_RET_ERR;
  
  // synchronization routines
  virtual tERROR   pr_call syncCreate( hOBJECT* sync_result, tIID sync_iid, tPID sync_pid=PID_ANY, tSTRING name=0 ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call syncGet( hOBJECT* sync_result ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call syncClose() PR_PURE_RET_ERR;
  virtual tERROR   pr_call syncLock( tDWORD wait_time ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call syncUnlock() PR_PURE_RET_ERR;
  virtual tERROR   pr_call syncProtectObject( tIID sync_iid, tPID sync_pid, tSTRING name ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call syncUnprotectObject() PR_PURE_RET_ERR;
  
  virtual tERROR   pr_call sysCreateObjectQuick( hOBJECT* result, tIID iid, tPID pid=PID_ANY, tDWORD subtype=SUBTYPE_ANY ) PR_PURE_RET_ERR;
  
  virtual tERROR   pr_call sysGetUserData( tDWORD* data ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysSetUserData( tDWORD  data ) PR_PURE_RET_ERR;
  
  // parent/child/object enumerate management
  virtual tERROR   pr_call sysGetFirstChild( hOBJECT* ret_obj, tIID iid=IID_ANY, tPID pid=PID_ANY ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysGetLastChild( hOBJECT* ret_obj, tIID iid=IID_ANY, tPID pid=PID_ANY ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysGetNextObject( hOBJECT* ret_obj, tIID iid=IID_ANY, tPID pid=PID_ANY ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysGetPrevObject( hOBJECT* ret_obj, tIID iid=IID_ANY, tPID pid=PID_ANY ) PR_PURE_RET_ERR;
  
  // ordinary memory management
  virtual tERROR   pr_call heapAlloc( tPTR* ptr, tDWORD size ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call heapRealloc( tPTR* ptr, tPTR exist, tDWORD size ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call heapFree( tPTR ptr ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call heapResolve( hHEAP* result, tPTR ptr ) PR_PURE_RET_ERR;
  
  virtual tERROR   pr_call syncSet( hOBJECT sync_obj ) PR_PURE_RET_ERR;
  
  // property management
  virtual tERROR   pr_call propGetStr( tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size, tCODEPAGE receive_cp=cCP_ANSI ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call propSetStr( tDWORD* out_size, tPROPID prop_id, const tPTR pStrz, tDWORD size=0, tCODEPAGE src_cp=cCP_ANSI ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call propGetStrCP( tCODEPAGE* cp, tPROPID prop_id ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call propIsPresent( tPROPID prop_id ) PR_PURE_RET_ERR;
  
  virtual tERROR   pr_call sysRevertObject( bool reuse_custom_prop ) PR_PURE_RET_ERR;
  
  virtual tPTR     pr_call propGetPtr( tPROPID prop_id ) PR_PURE_RET_ZERO;
  virtual hOBJECT  pr_call propGetObj( tPROPID prop_id ) PR_PURE_RET_ZERO;
  
  virtual tERROR   pr_call propSetPtr( tPROPID prop_id, tPTR val ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call propSetObj( tPROPID prop_id, hOBJECT val ) PR_PURE_RET_ERR;
  
  virtual tERROR   pr_call sysEnumPropId( tPROPID* prop_id_result, tDWORD* cookie ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysEnumPropCustomId( tPROPID* prop_id_result, tDWORD* cookie ) PR_PURE_RET_ERR;

  virtual tBYTE    pr_call propGetByte( tPROPID prop_id ) PR_PURE_RET_ZERO;
  virtual tERROR   pr_call propSetByte( tPROPID prop_id, tBYTE val ) PR_PURE_RET_ERR;
	
  virtual tERROR   pr_call sysRegisterMsgHandlerList  ( const tMsgHandlerDescr* handler_list, tDWORD handler_count ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysGetMsgHandlerList       ( tDWORD* result_count, tMsgHandlerDescr* handler_list, tDWORD handler_count ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysUnregisterMsgHandlerList( const tMsgHandlerDescr* handler_list, tDWORD handler_count ) PR_PURE_RET_ERR;

  virtual tERROR   pr_call sysCallInternalMsgReceive( tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen ) PR_PURE_RET_ERR;

  virtual tERROR   pr_call sysGetInternalData( tDWORD* size, tDWORD obj_size, tBYTE* data, tDWORD data_size, tObjSerializeFunc convert, tObjCheckCustomPropFunc prop_enum, tPTR ctx ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysSetInternalData( tDWORD* size, tDWORD obj_size, const tBYTE* data, tDWORD data_size, tObjDeserializeFunc convert, tPTR ctx ) PR_PURE_RET_ERR;

  virtual tERROR   pr_call sysRecognize( tIID by_iid, tPID by_pid, tDWORD type ) PR_PURE_RET_ERR;
  virtual tERROR   pr_call sysRecognizeByList( tPID* result, tIID by_iid, const tPID* by_pid_list, tDWORD count, tDWORD type ) PR_PURE_RET_ERR;

	virtual tERROR   pr_call sysValidateLock( hOBJECT to_lock, tIID iid = IID_ANY, tPID pid = PID_ANY ) PR_PURE_RET_ERR;
	virtual tERROR   pr_call sysRelease     () PR_PURE_RET_ERR;

  virtual tDATA    pr_call propGetData( tPROPID prop_id ) PR_PURE_RET_ZERO;
  virtual tERROR   pr_call propSetData( tPROPID prop_id ) PR_PURE_RET_ZERO;

  // some system supported properties
  hROOT     pr_call sysGetRoot()                                                              { return (hROOT)sysGetParent( 2 /*IID_ROOT*/ ); }
  tIID      pr_call propGetIID()                                                              { return propGetDWord(pgINTERFACE_ID); }
  tPID      pr_call propGetPID()                                                              { return propGetDWord(pgPLUGIN_ID); }
  tBOOL     pr_call propIsOperational()                                                       { return propGetBool(psOBJECT_OPERATIONAL); }
  tDWORD    pr_call propGetSubtype()                                                          { return propGetDWord(pgINTERFACE_SUBTYPE); }
  tIID      pr_call propGetCompatibility()                                                    { return propGetDWord(pgINTERFACE_COMPATIBILITY); }
  tDWORD    pr_call propGetIfaceFlags()                                                       { return propGetDWord(pgINTERFACE_FLAGS); }
  tVERSION  pr_call propGetVersion()                                                          { return propGetDWord(pgINTERFACE_VERSION); }
  tVID      pr_call propGetVendor()                                                           { return propGetDWord(pgVENDOR_ID); }
  hPLUGIN   pr_call propGetPlugin()                                                           { return (hPLUGIN)propGetObj(pgPLUGIN_HANDLE); }
  tCODEPAGE pr_call propGetCodepage()                                                         { return propGetDWord(pgOBJECT_CODEPAGE); }
  tERROR    pr_call propGetIfaceComment( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI )      { return propGetStr(0,pgINTERFACE_COMMENT,buff,size,cp); }
  hHEAP     pr_call propGetHeap()                                                             { return (hHEAP)propGetObj(pgOBJECT_HEAP); }
  tERROR    pr_call propSetHeap( hHEAP h )                                                    { return propSetObj(pgOBJECT_HEAP,(hOBJECT)h); }
  hHEAP     pr_call propGetIheritableHeap()                                                   { return (hHEAP)propGetObj(pgOBJECT_INHERITABLE_HEAP); }
  tDWORD    pr_call propSetTraceLevel()                                                       { return propGetDWord(pgTRACE_LEVEL); }
  tERROR    pr_call propGetTraceLevel( tDWORD level )                                         { return propSetDWord(pgTRACE_LEVEL,level); }
  tDWORD    pr_call propGetTraceLevelMin()                                                    { return propGetDWord(pgTRACE_LEVEL_MIN); }
  tERROR    pr_call propGetTraceLevelMin( tDWORD level )                                      { return propSetDWord(pgTRACE_LEVEL_MIN,level); }
  tDWORD    pr_call propGetUsageCount()                                                       { return propGetDWord(pgOBJECT_USAGE_COUNT); }
//  tBOOL     pr_call callback_sender()                                                         { return propGetBool(psCALLBACK_SENDER); }
//  tERROR    pr_call callback_sender( tBOOL val )                                              { return propSetBool(psCALLBACK_SENDER,val); }

protected:
  // property aliases
  tERROR   pr_call get( tPROPID propid, tPTR buff, tDWORD size )                              { return propGet(0,propid,buff,size); }
  tBOOL    pr_call getBool( tPROPID propid )                                                  { return propGetBool(propid); }
  tBYTE    pr_call getByte( tPROPID propid )                                                  { return propGetByte(propid); }
  tWORD    pr_call getWord( tPROPID propid )                                                  { return propGetWord(propid); }
  tUINT    pr_call getUInt( tPROPID propid )                                                  { return propGetUInt(propid); }
  tDWORD   pr_call getDWord( tPROPID propid )                                                 { return propGetDWord(propid); }
  tQWORD   pr_call getQWord( tPROPID propid )                                                 { return propGetQWord(propid); }
  tERROR   pr_call getStr( tPROPID propid, tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI )    { return propGetStr(0,propid,buff,size,cp); }
  hOBJECT  pr_call getObj( tPROPID propid )                                                   { return propGetObj(propid); }
  tPTR     pr_call getPtr( tPROPID propid )                                                   { return propGetPtr(propid); }
  
  tERROR   pr_call set( tPROPID propid, tPTR buff, tDWORD size )                              { return propSet(0,propid,buff,size); }
  tERROR   pr_call setBool( tPROPID propid, tBOOL val )                                       { return propSetBool(propid,val); }
  tERROR   pr_call setByte( tPROPID propid, tBYTE val )                                       { return propSetByte(propid,val); }
  tERROR   pr_call setWord( tPROPID propid, tWORD val )                                       { return propSetWord(propid,val); }
  tERROR   pr_call setUInt( tPROPID propid, tUINT val )                                       { return propSetUInt(propid,val); }
  tERROR   pr_call setDWord( tPROPID propid, tDWORD val )                                     { return propSetDWord(propid,val); }
  tERROR   pr_call setQWord( tPROPID propid, tQWORD val )                                     { return propSetQWord(propid,val); }
  tERROR   pr_call setStr( tPROPID propid, tPTR buff, tDWORD size=0, tCODEPAGE cp=cCP_ANSI )  { return propSetStr(0,propid,buff,size,cp); }
  tERROR   pr_call setObj( tPROPID propid, hOBJECT val )                                      { return propSetObj(propid,val); }
  tERROR   pr_call setPtr( tPROPID propid, tPTR val )                                         { return propSetPtr(propid,val); }

};

struct pr_abstract iEmpty {
  virtual tERROR dummy() { return 0x80000043; /*errNOT_IMPLEMENTED*/}
};
struct pr_abstract cObject : private iEmpty, public iObj {
	private : tPTR	m_data;
};
typedef cObject cObj;

#include "iface/i_root.h"

extern "C++" {
// -------
template<class Type, bool Unload = false> class cAutoObj {
public:
	cAutoObj() : m_obj(0), m_own(true) {}
	cAutoObj( Type* object, bool owner=true ) : m_obj(object), m_own(owner) {}
	cAutoObj( cObj* parent, tIID iid, tPID pid, tDWORD subtype = SUBTYPE_ANY ) : m_obj(0), m_own(true) { create(parent,iid,pid,subtype); }
	~cAutoObj() { checkClean(); }
	
public:
	Type&    operator *()             { return *m_obj; }
	operator iObj*()                  { return m_obj; }
	operator Type*()                  { return m_obj; }
	operator Type**()                 { return &m_obj; }
	operator const Type*()      const { return m_obj; }
	operator hOBJECT()                { return (hOBJECT)m_obj; }
	operator hOBJECT*()         const { return (hOBJECT*)&m_obj; }
	Type**   operator &()             { return &m_obj; }

	tERROR   quick( cObj* parent, tIID iid, tPID pid = PID_ANY, tDWORD subtype = SUBTYPE_ANY );
	tERROR   create( cObj* parent, tIID iid, tPID pid = PID_ANY, tDWORD subtype = SUBTYPE_ANY );
	tERROR   done();

public:
	bool    ownership()         const { return m_own; }
	bool    ownership( bool owner )   { bool own=m_own; m_own=owner; return own; }
	int     operator  !()       const { return m_obj==0; }
	Type*&  obj()                     { return m_obj; }
	Type*   relinquish()              { Type* obj = m_obj; m_obj = 0; return obj; }
	tERROR  clean();
	tERROR  checkClean()              { if ( m_own ) return clean(); m_obj = 0; return errOK; }
	Type*   operator->()              { return m_obj; }
	
	cAutoObj<Type>& operator =( Type* another_obj ) { 
		if ( m_obj && m_own ) 
			m_obj->sysCloseObject(); /*CALL_SYS_ObjectClose(m_obj);*/ 
		m_obj = another_obj; 
		return *this;
	}
protected:
	Type* m_obj;
	bool  m_own;
};


// ---
template<> class cAutoObj<cObj> {
public:
	cAutoObj() : m_obj(0) {}
	cAutoObj( cObj* object ) : m_obj(object) {}
	cAutoObj( cObj* parent, tIID iid, tPID pid = PID_ANY, tDWORD subtype = SUBTYPE_ANY ) : m_obj(0) { create(parent,iid,pid,subtype); }
	~cAutoObj() { if ( m_obj ) m_obj->sysCloseObject(); /*CALL_SYS_ObjectClose(m_obj);*/ }
	
public:
	cObj&    operator *()        { return *m_obj; }
	operator iObj*()             { return m_obj; }
	operator cObj*()             { return m_obj; }
	operator cObj**()            { return &m_obj; }
	operator const cObj*() const { return m_obj; }
	cObj**   operator &()        { return &m_obj; }
	
public:
	int     operator  !()  const { return m_obj==0; }
	cObj*&  obj()                { return m_obj; }
	cObj*   relinquish()         { cObj* obj = m_obj; m_obj = 0; return obj; }
	tERROR  clean()              { if ( m_obj ) { tERROR err = m_obj->sysCloseObject(); /*CALL_SYS_ObjectClose(m_obj);*/ m_obj = 0; return err; } return 0; }
	cObj*   operator->()         { return m_obj; }
	
	// ---
	tERROR quick( cObj* parent, tIID iid, tPID pid = PID_ANY, tDWORD subtype = SUBTYPE_ANY ) {
		if ( m_obj ) 
			m_obj->sysCloseObject();
		return parent->sysCreateObjectQuick( (hOBJECT*)&m_obj, iid, pid, subtype );
	}
	// ---
	tERROR create( cObj* parent, tIID iid, tPID pid = PID_ANY, tDWORD subtype = SUBTYPE_ANY ) {
		if ( m_obj ) 
			m_obj->sysCloseObject();
		return parent->sysCreateObject( (hOBJECT*)&m_obj, iid, pid, subtype );
	}
	// ---
	tERROR done() {
		if ( !m_obj )
			return errOBJECT_BAD_INTERNAL_STATE;
		return m_obj->sysCreateObjectDone();
	}

	cAutoObj<cObj>& operator =( cObj* another_obj ) {
		if ( m_obj ) 
			m_obj->sysCloseObject(); /*CALL_SYS_ObjectClose(m_obj);*/
		m_obj = another_obj; 
		return *this;
	}
protected:
	cObj* m_obj;
};


// ---
template <class Type, bool Unload> 
inline tERROR cAutoObj<Type, Unload>::quick( cObj* parent, tIID iid, tPID pid, tDWORD subtype ) {
	if ( m_obj && m_own ) 
		m_obj->sysCloseObject();
	return parent->sysCreateObjectQuick( (hOBJECT*)&m_obj, iid, pid, subtype );
}

// ---
template <class Type, bool Unload> 
inline tERROR cAutoObj<Type, Unload>::create( cObj* parent, tIID iid, tPID pid, tDWORD subtype ) {
	if ( m_obj && m_own ) 
		m_obj->sysCloseObject();
	return parent->sysCreateObject( (hOBJECT*)&m_obj, iid, pid, subtype );
}

// ---
template <class Type, bool Unload> 
inline tERROR cAutoObj<Type, Unload>::done() {
	if ( !m_obj )
		return errOBJECT_BAD_INTERNAL_STATE;
	return m_obj->sysCreateObjectDone();
}

template <class Type, bool Unload> 
inline tERROR cAutoObj<Type, Unload>::clean() { 
	if ( m_obj ) {
		tPID pid = Unload ? m_obj->propGetPID() : 0;
		tERROR err = m_obj->sysCloseObject(); 
		/*CALL_SYS_ObjectClose(m_obj);*/ 
		m_obj = 0;
		if ( Unload && g_root )
		{
			hPLUGIN  plg;
			tDATA    cookie = 0;
			if (PR_SUCC(CALL_Root_GetPluginInstance(g_root, &plg, &cookie, pid)))
			{
				// call Load at first for correct plugin reference counting
				CALL_Plugin_Load(plg);
				CALL_Plugin_Unload(plg, cTRUE);
			}
		}
		return err; 
	} 
	return errOK;
}

}


// -------
#define PR_NEW_DECLARE \
	void* operator new( size_t size ) { tPTR ptr = NULL; g_root->heapAlloc(&ptr, (tDWORD)size); return ptr; } \
	void  operator delete( void* p )  { g_root->heapFree(p); } \
	void* operator new[]( size_t size ) { tPTR ptr = NULL; g_root->heapAlloc(&ptr, (tDWORD)size); return ptr; } \
	void  operator delete[]( void* p )  { g_root->heapFree(p); }

#include <new>

//#if defined (_MSC_VER)
//#ifndef __PLACEMENT_NEW_INLINE
//#define __PLACEMENT_NEW_INLINE
//inline void *__cdecl operator new(size_t, void *_)	{ return _; }
//inline void __cdecl operator delete(void *, void *) {return; }
//#endif
//#else
//#include <new>
//#endif


#if !defined( cFuncPtrBase_declared )
#	define cFuncPtrBase_declared
	template< typename tfPTR, unsigned int cls_id, unsigned int func_id, unsigned int importer_id >
	class cFuncPtrBase {
	protected:
		tfPTR fptr;
	public:
		cFuncPtrBase() : fptr(0) {}
		tERROR load();
		operator bool() { return PR_SUCC(load()); }
	};
#endif

#endif // __cplusplus



/*
 * properties/messages broadcast types
 *
 * property broadcast - PropertySet call for all FIRST LEVEL
 * children with given IID
 *
 * any case PropretySet method
 * will be called for all apropriate objects
 *
 *
 *
 * message broadcast - ReceiveMsg call for all children with given IID
 * If flag cBROADCAST_ALL_LEVELS is not present - depth is first level only
 *
 * any case PropretySet method
 * will be called for all apropriate objects
 *
 */


#define cBROADCAST_AND           0x00    // result is true if all objects accepted broadcast
#define cBROADCAST_OR            0x01    // result is true if at least one object accepted broadcast
#define cBROADCAST_COUNT         0x02    // result is count of accepted broadcasts
#define cBROADCAST_FIRST_LEVEL   0x10    // depth of broadcasts is one level down
#define cBROADCAST_ALL_LEVELS    0x30    // depth of broadcasts is all levels down
#define cBROADCAST_OBJECT_ITSELF 0x40    // message is sent to object itself


// ---
#if defined(_PRAGUE_TRACE_FRAME_)

	#if defined(_PRAGUE_TRACE_)
		#undef _PRAGUE_TRACE_
		#undef PR_TRACE_A0
		#undef PR_TRACE_A1
		#undef PR_TRACE_A2
		#undef PR_TRACE_A3
		#undef PR_TRACE_A4
	#endif

	#define _PRAGUE_TRACE_
	#define PR_TRACE_A0(obj,p0)              PR_TRACE( (obj,prtFUNCTION_FRAME,p0) )
	#define PR_TRACE_A1(obj,p0,p1)           PR_TRACE( (obj,prtFUNCTION_FRAME,p0,p1) )
	#define PR_TRACE_A2(obj,p0,p1,p2)        PR_TRACE( (obj,prtFUNCTION_FRAME,p0,p1,p2) )
	#define PR_TRACE_A3(obj,p0,p1,p2,p3)     PR_TRACE( (obj,prtFUNCTION_FRAME,p0,p1,p2,p3) )
	#define PR_TRACE_A4(obj,p0,p1,p2,p3,p4)  PR_TRACE( (obj,prtFUNCTION_FRAME,p0,p1,p2,p3,p4) )

#else

	#define PR_TRACE_A0(obj,p0)
	#define PR_TRACE_A1(obj,p0,p1)
	#define PR_TRACE_A2(obj,p0,p1,p2)
	#define PR_TRACE_A3(obj,p0,p1,p2,p3)
	#define PR_TRACE_A4(obj,p0,p1,p2,p3,p4)

#endif

#ifndef __STRINGIZER_DEFINED
#define __STRINGIZER_DEFINED
#define _STRINGIZER2(x) #x
#define _STRINGIZER(x)  _STRINGIZER2(x)
#endif

#ifndef TR_MODULE
#define TR_MODULE ?
#endif

#if defined( _PRAGUE_TRACE_ )

	#define TRACE_PREFIX "\x01\xD8\xAD\xFE\x9F\xDA\xC9\xBF"
	#define TRACE_PREFIX_DW1 0xFEADD801
	#define TRACE_PREFIX_DW2 0xBFC9DA9F

	#define TR TRACE_PREFIX _STRINGIZER(TR_MODULE) "\t"

	EXTERN_C tBOOL pr_call trace_func( tPTR o, tTRACE_LEVEL l, tSTRING f, ... );

	// define to trace
  #define PR_TRACE(param)  trace_func param

  #if !defined(__cplusplus) || defined(_USE_VTBL)
    #define GET_ROOT(o) (((hOBJECT)o)->sys->ParentGet( (hOBJECT)o, IID_ROOT ))
  #else
    #define GET_ROOT(o) (dynamic_cast<iObj*>(((hOBJECT)o))->sysGetParent( IID_ROOT ))
  #endif

		// define to make tracer function
	#define PR_MAKE_TRACE_FUNC                                                      \
		EXTERN_C tBOOL pr_call trace_func( tPTR o, tTRACE_LEVEL l, tSTRING f, ... ) { \
			hTRACER t;                                                                  \
			va_list a;                                                                  \
			tERROR  e;                                                                  \
			tTraceLevel* tl;                                                            \
			if ( !o )                                                                   \
			  o = g_root;                                                               \
			if ( !o || !_HND(o) )                                                       \
				return cFALSE;                                                            \
			tl = OBJ_TRACE_LEVEL(o);                                                    \
			if ( ((l) < tl->min) || (tl->max < (l)) )                     \
				return cFALSE;                                                            \
			t = OBJ_TRACER(o);                                                          \
			if ( !t )                                                                   \
				return cFALSE;                                                            \
			va_start( a, f );                                                           \
			e = CALL_Tracer_TraceV( t, (hOBJECT)o, l, f, VA_LIST_ADDR(a) );             \
			va_end( a );                                                                \
			return PR_SUCC(e);                                                          \
		}

	#define PR_TRACE_VARS \
		const tVOID* p1;    \
		const tVOID* p2

	#define PR_TRACE_PREPARE_STR_CP(s,cp)    \
		(( ((cp)==cCP_UNICODE) ? (p1="",p2=(s)) : (p1=(s),p2=L"") ),p1), p2

	#define PR_TRACE_COND_LEVEL(err,l1,l2) \
		( PR_SUCC(err) ? (l1) : (l2) )

#else

  #define PR_MAKE_TRACE_FUNC

  #define PR_TRACE(param)                     
  #define PR_TRACE_A0(obj,p0)              
  #define PR_TRACE_A1(obj,p0,p1)           
  #define PR_TRACE_A2(obj,p0,p1,p2)        
  #define PR_TRACE_A3(obj,p0,p1,p2,p3)     
  #define PR_TRACE_A4(obj,p0,p1,p2,p3,p4)  

	#define PR_TRACE_VARS
	#define PR_TRACE_PREPARE_STR_CP(s)
	#define PR_TRACE_COND_LEVEL(err,l1,l2)

#endif


// ---
// reports error on destructor just only under _PRAGUE_TRACE_
#if defined(__cplusplus)
	struct cReportError : public cERROR {
		#if defined( _PRAGUE_TRACE_ )
			iObj*        m_obj;
			tTRACE_LEVEL m_level;
			const tCHAR* m_msg;
		#endif

		tERROR operator = (tERROR p) { return _=p; }
		tERROR operator = (const cERROR& o) { return _=o._; }

		cReportError( iObj* obj, tTRACE_LEVEL level, const tCHAR* msg ) : 
			cERROR()
				#if defined( _PRAGUE_TRACE_ )
					, m_obj(obj), m_level(level), m_msg(msg) 
				#endif
			{}
		
		#if defined( _PRAGUE_TRACE_ )
			~cReportError() {
				if ( PR_SUCC(_) ) {
					PR_TRACE(( m_obj, m_level, "%s succeed" ));
				}
				else {
					PR_TRACE(( m_obj, m_level, "%s failed (%terr)...", _ ));
				}
			}
		#endif
	};
#endif

// ---
// import/export function descriptor
/*
typedef struct tag_FUNC_DSC {
  tFUNC_PTR* func_ptr;
  tDWORD     func_cls;
  tDWORD     func_id;
} tFUNC_DSC;
*/

#define EXPORT_TABLE(table_name)                tEXPORT table_name[] = {
#define   EXTERN_NAME(name,ext_cls_id,ext_id)     { ((tDATA)&(name)), (tDATA)(ext_cls_id), (tDATA)(ext_id) },
#define EXPORT_TABLE_END                        {0,0,0}};

#define IMPORT_TABLE(table_name)                tIMPORT table_name[] = {
#define   EXTERN_NAME(name,ext_cls_id,ext_id)     { ((tDATA)&(name)), (tDATA)(ext_cls_id), (tDATA)(ext_id) },
#define IMPORT_TABLE_END                        {0,0,0}};


// OS dependent exception handling interface
typedef struct siPROT {
  tERROR (pr_call *Enter)            ( tPTR reserved, tExceptionReg* );             // enter exception protected code section
  tERROR (pr_call *Leave)            ( tPTR reserved, tExceptionReg* );             // leave exception protected code section
  tERROR (pr_call *GetExceptionInfo) ( tPTR reserved, tExceptionReg*, tDWORD* excpt_code, tPTR* info ); // get exception information
  tERROR (pr_call *Throw)            ( tPTR reserved, tDWORD excpt_code );          // throw an exception
  tERROR (pr_call *Initialize  )     ( tVOID );                                     // initialize exception handling system
  tERROR (pr_call *Deinitialize)     ( tVOID );                                     // deinitialize exception handling system
} iPROT;


#ifndef countof
  #define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

#if !defined( __RPC__ )
#include <Prague/iface/i_ifenum.h>
#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_tracer.h>

// CPP function frame tracer object
#if defined(__cplusplus) && !defined(PR_TRACE_FUNC_FRAME)
	#if defined (_PRAGUE_TRACE_)
		#define PR_TRACE_FUNC_FRAME(funcname)                  CFuncFrame iFuncFrame(*this, funcname, &error)
		#define PR_TRACE_FUNC_FRAME_(_this, funcname, perror)  CFuncFrame iFuncFrame(_this, funcname, perror)
		#define PR_TRACE_FUNC_FRAME__(funcname, perror)        CFuncFrame iFuncFrame(*this, funcname, perror)
	#else
		#define PR_TRACE_FUNC_FRAME(funcname)
		#define PR_TRACE_FUNC_FRAME_(_this, funcname, perror)
		#define PR_TRACE_FUNC_FRAME__(funcname, perror)
	#endif
	class CFuncFrame {
		private:
			hOBJECT      m_this;
			tERROR*      m_pError;
			const tCHAR* m_szFuncName;

		public:	
			CFuncFrame( hOBJECT p_this, const char* p_szFuncName, tERROR* p_pError ) :
				m_this((hOBJECT)p_this), m_pError(p_pError), m_szFuncName(p_szFuncName) {
				PR_TRACE(( m_this, prtFUNCTION_FRAME, "Enter %s", m_szFuncName ));
			}
			~CFuncFrame() {
				if ( m_pError )
					PR_TRACE(( m_this, prtFUNCTION_FRAME, "Leave %s, %terr", m_szFuncName, *m_pError ));
				else
					PR_TRACE(( m_this, prtFUNCTION_FRAME, "Leave %s", m_szFuncName ));
			}
	};
#endif // defined(__cplusplus) && !defined(PR_TRACE_FUNC_FRAME)

#if !defined(reterr)
	#define reterr return error = 
#endif


#endif // __RPC__

#if defined(__cplusplus) && defined(cFuncPtrBase_declared) && !defined(cFuncPtrBase_implemented)
#	include "iface/i_root.h"
#if !defined(cFuncPtrBase_implemented)
#define cFuncPtrBase_implemented
template<typename tfPTR, unsigned int cls_id, unsigned int func_id, unsigned int importer_id >
inline tERROR cFuncPtrBase<tfPTR,cls_id,func_id,importer_id>::load() {
	if ( fptr )
		return errOK;
	if ( !::g_root )
		return errUNEXPECTED;
#if !defined(_USE_VTBL)		
	return ::g_root->ResolveImportFunc( (tFUNC_PTR*)&fptr, cls_id, func_id, importer_id );
#else
	return CALL_Root_ResolveImportFunc( g_root, (tFUNC_PTR*)&fptr, cls_id, func_id, importer_id );
#endif
}
#endif
#endif // defined(__cplusplus) && defined(cFuncPtrBase_declared)

#endif // __PR_SYS_H
