/*-----------------16.03.00 15:43----------------------*
 * Project Prague                                      *
 * Subproject kernel internal structures and utilities *
 * Author Andrew Andy Petrovitch                       *
 * Copyright (c) Kaspersky Lab                         *
 * Purpose Prague API                                  *
 * revision 2                                          *
 *-----------------------------------------------------*/


#ifndef __kernel_h
#define __kernel_h


#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_event.h>
#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_mutex.h>
#include <Prague/iface/i_ifenum.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_semaph.h>
#include <Prague/iface/i_tracer.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktrace.h>

#ifdef __cplusplus
	extern "C" {
#endif


// forward declaration
typedef struct tag_INTERFACE        tINTERFACE;
typedef struct tag_CustProp         tCustProp;
typedef struct tag_MsgHandler       tMsgHandler;
typedef struct tag_MsgCachedHandler tMsgCachedHandler;
typedef struct tag_OBJECT           tOBJECT;


// ---
typedef struct tag_MHArray {
  tUINT              mcount;   // array size
  tMsgHandler*       parr;     // array memory
} tMHArray;



// ---
typedef struct tag_MHCacheArray {
  tUINT              count;    // array size
  tMsgCachedHandler* parr;     // array memory
} tMHCacheArray;


// ---
// wrapper management
typedef const tPTR (pr_call *OneFuncPtr)( struct sHANDLE* handle, tPTR param, tBOOL ex_in, tDWORD num, tDWORD* data );


// Wrapper structure
typedef struct tag_WRAPPER tWRAPPER;


typedef struct sHANDLEX tHANDLEX;


#if !defined(TraceLevel_defined)
	#define TraceLevel_defined
	typedef struct tag_TraceLevel {
		#if defined(_INTEGRAL_MAX_BITS) && (_INTEGRAL_MAX_BITS >= 64)
			tTRACE_LEVEL max;  //  maximum trace level
			tTRACE_LEVEL min;  //  minimum trace level
		#else
			tWORD   max;  //  maximum trace level
			tWORD   min;  //  minimum trace level
		#endif
	} tTraceLevel;
#endif

// ---
// handle definition structure (kernel point of view)
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!! ----- if you want to change offsets in this structure            ----- !!!!!!!!!!!!!!!!
// !!!!!!!!!!! ----- do not forget to change OBJ_TR... macros in the "pr_sys.h" ----- !!!!!!!!!!!!!!!!
// !!!!!!!!!!! ----- code in System_ObjectSwap method                           ----- !!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
typedef struct sHANDLE {

	tOBJECT*            obj;              //  0x00-00  0x00 interface defined part of handle
	tINTERFACE*         iface;            //  0x04-01  0x08 pointer to interface decription
	tCustProp*          custom_prop;      //  0x08-02  0x10 custom (external) properties
	tDWORD              custom_prop_count;//  0x0c-03  0x18 custom (external) properties count

	tDWORD              flags;            //  0x10-04  0x1c some flags
	tDWORD              ref;              //  0x14-05  0x20 lock count
	tDWORD              init_props;       //  0x18-06  0x24 initialization property marks
	tHANDLEX*           ext;              //  0x1c-07  0x28 handle extension

	// handle hierarchy related data
	struct sHANDLE*     parent;           //  0x20-08  0x30 parent handle
	struct sHANDLE*     next;             //  0x24-09  0x38 next handle at the same level
	struct sHANDLE*     prev;             //  0x28-10  0x40 prev handle at the same level
	struct sHANDLE*     child;            //  0x2c-11  0x48 first child

	hHEAP               mem;              //  0x30-12  0x50 linked memory heap
	hOBJECT             sync;             //  0x34-13	 0x58 linked synchronized object
	hTRACER             tracer;           //  0x38-14  0x60 tracer handle if tracing turned on
	tTraceLevel         trace_level;      //  0x3c-15  0x68 trace level for this handle

} tHANDLE;


struct sHANDLEX {
	tMHArray      mh_arr;     //  0x00-00  message handler array
	tMHCacheArray mh_c_arr;   //  0x08-02  cached message handlers

	tWRAPPER*     wrappers;   //  0x10-21  wrapper list
	tDWORD        wrap_data;  //  0x14-22  data passed to wrapper method
	tDWORD        wrap_data2; //  0x18-23  data passed to wrapper method 

	tDWORD        user_data;  //  0x1c-04
	tHANDLE*      find_data1; //  0x20-05
	tDWORD        find_data2; //  0x24-06

	tDATA         reserved0;  //  0x28-26
	tDATA         reserved1;  //  0x2c-27
};


#define _chk_ext_0(h) \
	( (h)->ext ? errOK : (_chk_ext_count,PrAlloc(&(h)->ext,sizeof(tHANDLEX))))

#define _chk_ext(po,h,p,e) \
	( (h)->ext ? ((p)=(h)->ext,errOK) : (PR_SUCC((e)=PrAlloc(&(p),sizeof(tHANDLEX))) ? (_chk_ext_count,wlock(po),((h)->ext=p),wunlock(po),e) : e) )


//#define OBJ_WD(o)               ( (tDWORD*)(((tBYTE*)o)-(3*sizeof(tDWORD)+sizeof(hTRACER))) )
#define HDL_TRACER(h)           ( h->tracer )

#define HDL_TR_MAX_LVL(h)       ( h->trace_level.max )
#define HDL_TR_MIN_LVL(h)       ( h->trace_level.min )

#define HDL_TR_MAX_CHK(h,lvl)   (lvl <= HDL_TR_MAX_LVL(h))
#define HDL_TR_MIN_CHK(h,lvl)   (HDL_TR_MIN_LVL(h) <= lvl)
#define HDL_TR_CHK(h,lvl)       (HDL_TR_MAX_CHK(h,lvl) && HDL_TR_MIN_CHK(h,lvl))
#define HDL_TR_CHECK(h,lvl)     (HDL_TRACER(h) && HDL_TR_CHK(h,lvl) )

// handle state flags
#define hf_OPERATIONAL_MODE          0x00000001  // object in operational mode now
#define hf_MEMORY_LOCKED             0x00000002  // object's memory is locked
#define hf_MESSAGE_HANDLER           0x00000004  // object is message handler (clear message handlers cache on close)
#define hf_MESSAGE_HANDLER_CACHE     0x00000008  // object has message handler cache
#define hf_OBJECT_CLOSED             0x00000010  // object destroyed
#define hf_OBJECT_IS_CLOSING         0x00000020  // object is destroing now
#define hf_OBJECT_PROTECTED          0x00000040  // object code is protected by synchronization object
#define hf_HIDDEN                    0x00000080  // object is hidden (notification is not passed and this object cannot be found)
#define hf_PROTECT_OBJECT_SET        0x00000100  // sync object has been set (not created) from outside and cannot be closed by Unprotect
#define hf_OWN_CP                    0x00000200  // object has own pgOBJECT_CODEPAGE property defined
#define hf_CALLBACK_SENDER           0x00000400  // kernel events on this object are sended by kernel
#define hf_LOCK_EVENT_SET            0x00000800  // synchronization event st for this object
#define hf_NOT_REUSABLE              0x00001000  // this object cannot be reused
#define hf_PROP_SYNCHRONIZED         0x00002000  // this object is synchronized when client accesses to it's properties
#define hf_MEM_SYNCHRONIZED          0x00004000  // this object is synchronized when client calls memory allocation methods


#define OBJ_IS_CONDEMNED(h)          (h->flags & (hf_OBJECT_CLOSED|hf_OBJECT_IS_CLOSING))
#define OBJ_IS_NOT_CONDEMNED(h)      !(h->flags & (hf_OBJECT_CLOSED|hf_OBJECT_IS_CLOSING))

#define OBJ_IS_RECEIVER(h)           !(h->flags & (hf_HIDDEN|hf_OBJECT_CLOSED|hf_OBJECT_IS_CLOSING))
#define OBJ_IS_NOT_RECEIVER(h)       (h->flags & (hf_HIDDEN|hf_OBJECT_CLOSED|hf_OBJECT_IS_CLOSING))

#define PSYNC_ON(h)  (((h)->flags & hf_PROP_SYNCHRONIZED))
#define PSYNC_OFF(h) (!((h)->flags & hf_PROP_SYNCHRONIZED))

#define MSYNC_ON(h)  (((h)->flags & hf_MEM_SYNCHRONIZED))
#define MSYNC_OFF(h) (!((h)->flags & hf_MEM_SYNCHRONIZED))


// interface definition structure (kernel point of view)
struct tag_INTERFACE {

  tIID              iid;              // interface type
  tPID              pid;              // plugin ID
  tDWORD            flags;            // interface flags
  tDWORD            subtype;          // subtype of interface in plugin
  tVERSION          version;          // version
  tVID              vid;              // vendor id
  iINTERNAL*        internal;         // pointer for internal access functions
  tDWORD            in_count;         // counter of internal access functions
  tPTR              external;         // exported interface functions
  tDWORD            ex_count;         // function count in table above
  tDATA*            proptable;        // properties table
  tDWORD            propcount;        // properties count maintained by kernel
  tDATA             propmaxsize;      // required size for properties
  tDWORD            memsize;          // required size for local data incl. prop.
  tDWORD            obj_count;        // count of objects interface creats at the moment
  hPLUGIN           plugin;           // module of the interface
  tDWORD            index;            // interface index in the module
	tTraceLevel       trace_level;			// trace level for this intrface
  tIID              compat;           // the iface is compatible with indicated
	tERROR            last_err;         // last (load ?) error
	tCODEPAGE         cp;               // interface codepage
  tDATA*            pid_user;         // pid property record implemented by user
  tDATA             reserved0;
  tDATA             reserved1;
  //hCRITICAL_SECTION cs;               // critical section to protect loading/unloading
};

// free bit in IFACE_ flags
#define IFACE_SET_VTBL      ((tDWORD)(0x00000008L)) // vtbl must be set in spite of g_unsafe_vtbl

// ---
// system virtual table
extern iSYSTEMVTBL si;


#define LOCAL_IFACE_CLUSTER 0x10
	

// ---
struct tag_MsgHandler {
  struct sHANDLE* handler;
  tDWORD          flags;
  tDWORD          msg_cls;
  tIID            obj_iid;
  tPID            obj_pid;
  tIID            ctx_iid;
  tPID            ctx_pid;
};

// ---
struct tag_MsgCachedHandler {
  tMsgHandler*    handler;
  hOBJECT         recv_point;
};

#define MSG_FLAGS_DECIDER  0x00000001l  // message handler is decider



// ---
// object declaration
struct tag_OBJECT {
#if defined(_DEBUG)
	const tVOID*          guard;        // windows will save some free list pointer here (on deallocate)
	struct sHANDLE*       saved_handle; // i will save back pointer to handle here (on deallocate)
#endif
  struct sHANDLE*       handle; // back pointer to handle
  const tVOID*          vtbl;   // interface defined function table - "Default VTBL" for generic object
  const iSYSTEMVtbl*    sys;    // system defined function table
  const tVOID*          data;   //data;   // internal object data
};



// ---
// just to cust an hOBJECT to the tHANDLE
typedef struct tag_hiOBJECT {
  const tVOID*       vtbl;   // interface defined function table - NULL for generic object
  const iSYSTEMVtbl* sys;    // system defined function table
  const tVOID*       data;   // data
} *hiOBJECT;


typedef struct tag_ProveObject {
	const tVOID* cpp_data;
	const tVOID* c_data;
} tProveObject;


// -----------------------------------------------------------------------
// custom property struct
struct tag_CustProp {
  tPROPID   prid;
  tDWORD    size;
  tDATA     m_data;
};


// ---
typedef struct tag_CustData {
  tDWORD allocated;
  tBYTE  data[1];
} tCustData;



// ---
typedef struct tag_CustStrData {
  tDWORD    allocated;
  tCODEPAGE cp;
  tBYTE     data[1];
} tCustStrData;


#define NOTIFICATION_SINC_CLUSTER 4

// ---
typedef struct tag_NotificationSink {
	tUINT     id;
	tFUNC_PTR func;
	tPTR      ctx;
} tNotificationSink;


// ---
typedef struct tag_NotificationSinkArr {
	tUINT              count;
	tNotificationSink  arr[NOTIFICATION_SINC_CLUSTER];
} tNotificationSinkArr;


// ---
typedef struct tag_cLoaderSectData {
	tVOID*  vtbl;
	tVOID*  sys;
	tVOID*  data;
	tVOID*  DebugInfo;           // critical section Windows object
	tLONG   LockCount;
	tLONG   RecursionCount;
	tDWORD  OwningThread;        // from the thread's ClientId->UniqueThread
	tVOID*  LockSemaphore;
	tDWORD  SpinCount;
	tDWORD  call_addr;
} cLoaderCSData;

// ---
typedef struct tag_PO {  // kernel protection object

	union {
		hCRITICAL_SECTION cs;
		cLoaderCSData*    lcs;
	};

	//hCRITICAL_SECTION cs;
	tINT  usage;
	tBOOL two_level;
	tBOOL condemned;
} tPO;


// ---
typedef struct tag_SynchroEvent {
	struct tag_SynchroEvent* m_next;
	tHANDLE*                 m_handle;
	hSYNC_EVENT              m_event;
} tSynchroEvent;


#define GET_IID(h)             ( ((tHANDLE*)(h))->iface->iid )
#define GET_PID(h)             ( (((tHANDLE*)(h))->iface->pid_user) ? _PropertyGetPID(0,((tHANDLE*)(h)),((tHANDLE*)(h))->iface->pid_user) : ((tHANDLE*)(h))->iface->pid )

#define GET_IID_C(h)           ( ((tHANDLE*)(h))->iface ? GET_IID(h) : IID_NONE )
#define GET_PID_C(h)           ( ((tHANDLE*)(h))->iface ? GET_PID(h) : PID_NONE )

#define CHECK_IID(h,iid)       (((iid)==IID_ANY) || (GET_IID((tHANDLE*)(h)) == iid))
#define CHECK_PID(h,pid)       (((pid)==PID_ANY) || (GET_PID((tHANDLE*)(h)) == pid))

#define CHECK_IID_C(h,iid)     (((iid)==IID_ANY) || (GET_IID_C((tHANDLE*)(h)) == iid))
#define CHECK_PID_C(h,pid)     (((pid)==PID_ANY) || (GET_PID_C((tHANDLE*)(h)) == pid))

#define CHECK(h,iid,pid)       ( CHECK_IID(h,iid)   && CHECK_PID(h,pid)   )
#define CHECK_C(h,iid,pid)     ( CHECK_IID_C(h,iid) && CHECK_PID_C(h,pid) )

#define OBJ_PROVE_OFFSET       ( _toi(1) )

#define C_HDL_TO_OBJ_OFFSET    ( _toi( _toi(&(((struct tag_OBJECT*)0)->vtbl)) / sizeof(const tVOID*)) )
#define C_OBJ_TO_OBJ_OFFSET    ( _toi( _toi(&(((struct tag_hiOBJECT*)0)->vtbl)) / sizeof(const tVOID*)) )
#define C_OBJ_TO_DATA_OFFSET   ( _toi(sizeof(struct tag_hiOBJECT) / sizeof(const tVOID*)) - C_OBJ_TO_OBJ_OFFSET )

#define CPP_HDL_TO_OBJ_OFFSET  ( _toi(_toi(&(((struct tag_OBJECT*)0)->sys)) / sizeof(const tVOID*)) )
#define CPP_OBJ_TO_OBJ_OFFSET  ( _toi(_toi(&(((struct tag_hiOBJECT*)0)->sys)) / sizeof(const tVOID*)) )
#define CPP_OBJ_TO_DATA_OFFSET ( _toi(sizeof(struct tag_hiOBJECT) / sizeof(const tVOID*)) - CPP_OBJ_TO_OBJ_OFFSET )

#define IS_C_OBJ(o)            ( OBJD(o,C_OBJ_TO_DATA_OFFSET)   == hiOBJ(OBJD(o,-C_OBJ_TO_OBJ_OFFSET))->data )
#define IS_CPP_OBJ(o)          ( OBJD(o,CPP_OBJ_TO_DATA_OFFSET) == hiOBJ(OBJD(o,-CPP_OBJ_TO_OBJ_OFFSET))->data )

#define CUST_TO_C_OBJ(o)       ( o ? (IS_CPP_OBJ(o) ? OBJD(o,-CPP_OBJ_TO_OBJ_OFFSET) : o) : 0 )

#define hiOBJ(o)               ((hiOBJECT)(o))
#define KOBJ(o)                (((tOBJECT*)(((hiOBJECT)(o))+1))-1)
#define OBJD(o,s)              ((tPTR)(((tPTR*)(o))+(s)))
#define MakeHandle(o)          ( (o) ? (IS_CPP_OBJ(o) ? KOBJ(OBJD(o,-CPP_OBJ_TO_OBJ_OFFSET))->handle : (IS_C_OBJ(o) ? KOBJ(OBJD(o,-C_OBJ_TO_OBJ_OFFSET))->handle : 0)) : 0 )

//#define MakeHandle(o)        ( ((o) == 0) ? 0 : ((tHANDLE*)(((tOBJECT*)((*(tPTR**)&((tDWORD)(o)))-1))->handle)) )
//#define MakeHandle(object)   ( ((object) == 0) ? 0 : ((tHANDLE*)(((tDWORD)(((tHANDLE*)(((tOBJECT*)(object))+1))-1)) & 0xfffffff8)) )
#define MakeObject(h)          ( (((h) == 0) || ((h)->obj == 0)) ? 0 : ((hOBJECT)(&(h)->obj->vtbl)) )

// VTBL manipulation
//#define hvtbl(h)                    ovtbl( (h)->obj )
//#define ovtbl(o)                    ((tPTR*)&(o)->vtbl)
//#define _get_ovtbl(obj)             ((obj)->vtbl)
#define _get_vtbl(handle)           ( (handle)->obj ? (handle)->obj->vtbl : 0 )
#define _is_vtbl(handle,ptr)        ( _get_vtbl(handle) == (ptr) )

// implemented in iface.c
tERROR  _set_vtbl( tHANDLE* handle, const tPTR vtbl );


#define hdata(h)               odata( (h)->obj )
#define odata(o)               ((tPTR*)&(o)->data)


// ---
// synchronization issues

#define enter_lf()                                    \
  po = g_po

#define leave_lf()                                    \
  if ( po && po->condemned && !po->usage ) {          \
    _SyncCloseObj( po );                              \
  }


#if defined(_DEBUG)
	//#define CHECK_LOCKS
#endif

#if defined(CHECK_LOCKS)

	tVOID _cs_dbg_before_leave( tPO* po );
	tVOID _cs_dbg_after_leave( tPO* po );

	tVOID _cs_dbg_before_enter( tPO* po );
	tVOID _cs_dbg_after_enter( tPO* po );

	#define DBG_ENT_0(po)     (_cs_dbg_before_enter(PO(po)),0),
	#define DBG_ENT_1(po)     ,(_cs_dbg_after_enter(PO(po)),0)

	#define DBG_LEAVE_0(po)   (_cs_dbg_before_leave(PO(po))),
	#define DBG_LEAVE_1(po)   ,(_cs_dbg_after_leave(PO(po)))
#else
	#define DBG_ENT_0(po)
	#define DBG_ENT_1(po)
	#define DBG_LEAVE_0(po)
	#define DBG_LEAVE_1(po)
#endif

#define PO(po)       ( (tPO*)(po) )
#define PO_CS(po)    ( PO(po)->cs )
#define PO_CHECK(po) ( PO(po) && PO_CS(po) )

#if defined(__cplusplus)
	#define PO_ENTER(po,level)  ( DBG_ENT_0(po)   PO_CS(po)->Enter(level)  DBG_ENT_1(po) )
	#define PO_LEAVE(po,plevel) ( DBG_LEAVE_0(po) PO_CS(po)->Leave(plevel) DBG_LEAVE_1(po) )
#else
	#define PO_ENTER(po,level)  ( DBG_ENT_0(po)   PO_CS(po)->vtbl->Enter(PO_CS(po),level)  DBG_ENT_1(po) )
	#define PO_LEAVE(po,plevel) ( DBG_LEAVE_0(po) PO_CS(po)->vtbl->Leave(PO_CS(po),plevel) DBG_LEAVE_1(po) )
#endif


//#define                _HC( h, po, object, error )       ( h=_handle_check_debug_func(po,object,error) )
//#define                _HC( h, po, object, error )       (h=_handle_check(po,object,error))

#define _HC(  h, po, o, e )           ( (h=MakeHandle(o)) ? ((*(e)=errOK), rlock(po),h)                   : ((*(e)=errHANDLE_INVALID),0) )
#define _HCC( h, po, lock, o, e )     ( (h=MakeHandle(o)) ? ((*(e)=errOK), rlockc(po,lock),h)             : ((*(e)=errHANDLE_INVALID),lock=0,0) )
#define _HCM( h, po, lock, o, e )     ( (h=MakeHandle(o)) ? ((*(e)=errOK), rlockc(po,lock=MSYNC_ON(h)),h) : ((*(e)=errHANDLE_INVALID),lock=0,0) )
#define _HCP( h, po, lock, o, e )     ( (h=MakeHandle(o)) ? ((*(e)=errOK), rlockc(po,lock=PSYNC_ON(h)),h) : ((*(e)=errHANDLE_INVALID),lock=0,0) )

#define rlock(po)                     (( PO_CHECK(po) )                      ? (PO_ENTER(po,SHARE_LEVEL_READ) ,++PO(po)->usage) : 0 )
#define runlock(po)                   (( PO_CHECK(po) )                      ? (--PO(po)->usage,PO_LEAVE(po,0))    : 0 )

#define rllock(po,level)              (( PO_CHECK(po) )                      ? (PO_ENTER(po,level)            ,++PO(po)->usage) : 0 )
#define rlunlock(po,plev)             (( PO_CHECK(po) )                      ? (--PO(po)->usage,PO_LEAVE(po,plev)) : 0 )

#define wlock(po)                     (( PO_CHECK(po) && PO(po)->two_level ) ? (PO_ENTER(po,SHARE_LEVEL_WRITE),++PO(po)->usage) : 0 )
#define wunlock(po)                   (( PO_CHECK(po) && PO(po)->two_level ) ? (--PO(po)->usage,PO_LEAVE(po,0))    : 0 )

#define rlockc( po, l )               ( (l) ? (rlock(po),1) : 0 )
#define runlockc( po, l )             ( (l) ? (runlock(po),1) : 0 )

#define rllockc(po,l,lev)             ( (l) ? (rllock(po,lev),1)    : 0 )
#define rlunlockc(po,l,plev)          ( (l) ? (rlunlock(po,plev),1) : 0 )

#define lock_ex( sync_obj, level )    ( (sync_obj) ? ((sync_obj)->vtbl->Enter((sync_obj),level)) : 0 )
#define unlock_ex( sync_obj, level )  ( (sync_obj) ? (sync_obj)->vtbl->Leave((sync_obj),level) : 0 )


#define _PR_ENTER_PROTECTED_0(po) {                              \
  tExceptionReg er;                                              \
  tSHARE_LEVEL  lev = SHARE_LEVEL_ERROR;                         \
  rlunlock( po, &lev );                                          \
  if ( errOK == si.EnterProtectedCode(0,&er) )

#define _PR_LEAVE_PROTECTED_0(po,e)                              \
  else                                                           \
    e = errUNEXPECTED;                                           \
  si.LeaveProtectedCode( 0, &er );                               \
  rllock( po, lev ); }


#define _PR_ENTER_PROTECTED_0P(po,h) {                           \
  tExceptionReg er;                                              \
  tSHARE_LEVEL  lev = SHARE_LEVEL_ERROR;                         \
  tINT lock = rlunlockc( po, PSYNC_ON(h), &lev );                \
  if ( errOK == si.EnterProtectedCode(0,&er) )
	
#define _PR_LEAVE_PROTECTED_0P(po,e)                             \
  else                                                           \
    e = errUNEXPECTED;                                           \
  si.LeaveProtectedCode( 0, &er );                               \
  rllockc( po, lock, lev ); }


#define _PR_ENTER_PROTECTED(h) {                                 \
  tExceptionReg er;                                              \
  hCRITICAL_SECTION cs = 0;                                      \
  tSHARE_LEVEL  lev = SHARE_LEVEL_ERROR;                         \
  rlunlock( po, &lev );                                          \
  if ( h && h->sync && (h->flags & hf_OBJECT_PROTECTED) ) {      \
    cs = (hCRITICAL_SECTION)h->sync;                             \
    CALL_CriticalSection_Enter( cs, SHARE_LEVEL_WRITE );         \
  }                                                              \
  if ( errOK == si.EnterProtectedCode(0,&er) )


#define _PR_LEAVE_PROTECTED(e)                                   \
  else                                                           \
    e = errUNEXPECTED;                                           \
  si.LeaveProtectedCode( 0, &er );                               \
  if ( cs )                                                      \
    CALL_CriticalSection_Leave( cs, 0 );                         \
  rllock( po, lev ); }


#define _PR_ENTER_PROTECTED_EXT(h,e) {                           \
  tExceptionReg er;                                              \
  hCRITICAL_SECTION cs = 0;                                      \
  if ( h->sync && (h->flags & hf_OBJECT_PROTECTED) ) {           \
    cs = (hCRITICAL_SECTION)h->sync;                             \
    CALL_CriticalSection_Enter( cs, SHARE_LEVEL_WRITE );         \
  }                                                              \
  if ( errOK == si.EnterProtectedCode(0,&er) )


#define _PR_LEAVE_PROTECTED_EXT(e)                               \
  else                                                           \
    e = errUNEXPECTED;                                           \
  si.LeaveProtectedCode( 0, &er );                               \
  if ( cs )                                                      \
    CALL_CriticalSection_Leave( cs, 0 ); }



// ---
#define TYPE_ASSIGN(type,stream,value)            (((*(type*)(stream))=(value)),++(*(type**)&(stream)));
#define TYPE_GET(type,stream)                     (*(*(type**)&stream)++)

// ---
#define CPLIST_SIGNATURE  0xf31ee06f
#define CPLIST_VERSION    1
#define CPLIST_HEAD_SIZE  (sizeof(tDWORD)/*signature*/ + sizeof(tDWORD)/*version*/ + sizeof(tDWORD)/*count*/ + sizeof(tDWORD)/*size*/)

#define CPSIZE(cp,os)     (\
  sizeof(tDWORD)/*prop id*/ + \
  sizeof(tDWORD)/*size value*/ + \
  (IS_PROP_TYPE((cp)->prid,pTYPE_OBJECT) ? os : (cp)->size) + \
  ((IS_PROP_TYPE((cp)->prid,pTYPE_STRING) || IS_PROP_TYPE((cp)->prid,pTYPE_WSTRING)) ? sizeof(tCODEPAGE) : 0))



// ---
// system virtual table - part of generic object
// property management functions -- sys_prop.c
tERROR  pr_call System_PropertyIsPresent   ( hOBJECT object, tPROPID prop );
tERROR  pr_call System_PropertyGet         ( hOBJECT object, tDWORD* out_size, tPROPID prop, tPTR buffer, tDWORD size );
tBOOL   pr_call System_PropertyGetBool     ( hOBJECT object, tPROPID prop );
tBYTE   pr_call System_PropertyGetByte     ( hOBJECT object, tPROPID prop );
tWORD   pr_call System_PropertyGetWord     ( hOBJECT object, tPROPID prop );
tUINT   pr_call System_PropertyGetUInt     ( hOBJECT object, tPROPID prop );
tDWORD  pr_call System_PropertyGetDWord    ( hOBJECT object, tPROPID prop );
tQWORD  pr_call System_PropertyGetQWord    ( hOBJECT object, tPROPID prop );
tDATA   pr_call System_PropertyGetData     ( hOBJECT object, tPROPID prop );
tPTR    pr_call System_PropertyGetPtr      ( hOBJECT object, tPROPID prop );
hOBJECT pr_call System_PropertyGetObj      ( hOBJECT object, tPROPID prop );
tERROR  pr_call System_PropertyInitialized ( hOBJECT object, tPROPID prop_id, tBOOL val );
	
// prop_str.c
tERROR  pr_call System_PropertyGetStr      ( hOBJECT object, tDWORD* out_size, tPROPID prop, tPTR buffer, tDWORD size, tCODEPAGE receive_cp );
tERROR  pr_call System_PropertyGetStrCP    ( hOBJECT object, tCODEPAGE* cp, tPROPID propid );
	
tERROR  pr_call System_PropertySet         ( hOBJECT object, tDWORD* out_size, tPROPID prop, tPTR buffer, tDWORD size );
tERROR  pr_call System_PropertySetBool     ( hOBJECT object, tPROPID prop, tBOOL   val );
tERROR  pr_call System_PropertySetByte     ( hOBJECT object, tPROPID prop, tBYTE   val );
tERROR  pr_call System_PropertySetWord     ( hOBJECT object, tPROPID prop, tWORD   val );
tERROR  pr_call System_PropertySetUInt     ( hOBJECT object, tPROPID prop, tUINT   val );
tERROR  pr_call System_PropertySetDWord    ( hOBJECT object, tPROPID prop, tDWORD  val );
tERROR  pr_call System_PropertySetQWord    ( hOBJECT object, tPROPID prop, tQWORD  val );
tERROR  pr_call System_PropertySetData     ( hOBJECT object, tPROPID prop, tDATA   val );
tERROR  pr_call System_PropertySetPtr      ( hOBJECT object, tPROPID prop, tPTR    val );
tERROR  pr_call System_PropertySetObj      ( hOBJECT object, tPROPID prop, hOBJECT val );
// prop_str.c
tERROR  pr_call System_PropertySetStr      ( hOBJECT object, tDWORD* out_size, tPROPID prop, tPTR pStrz, tDWORD size, tCODEPAGE src_cp );

tERROR  pr_call System_PropertyDelete      ( hOBJECT object, tPROPID prop );
tDWORD  pr_call System_PropertySize        ( hOBJECT object, tPROPID prop );

tERROR  pr_call System_PropertyEnumId      ( hOBJECT object, tPROPID* prop_id_result, tDWORD* cookie );
tERROR  pr_call System_PropertyEnumCustomId( hOBJECT object, tPROPID* prop_id_result, tDWORD* cookie );

tDWORD  pr_call System_PropertyBroadcast   ( hOBJECT object, tIID iid, tDWORD type, tPROPID prop, tPTR buffer, tDWORD size );
        
// low level object management -- sys_hand.c
tERROR  pr_call System_HandleCreate        ( hOBJECT object, hOBJECT* new_obj, tDWORD memsize );
tERROR  pr_call System_InterfaceSet        ( hOBJECT object, tIID iid, tPID pid, tDWORD subtype );
                                     
// memory management -- sys_mem.c
tERROR  pr_call System_MemorySize          ( hOBJECT object, tDWORD* size );
tERROR  pr_call System_MemoryRealloc       ( hOBJECT object, tDWORD size );

// high level object management sys_obj.c 
hOBJECT pr_call System_ParentGet           ( hOBJECT object, tIID iid );
tERROR  pr_call System_ParentSet           ( hOBJECT object, hOBJECT* old_parent, hOBJECT new_parent );
tERROR  pr_call System_ObjectCreate        ( hOBJECT object, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype );
tERROR  pr_call System_ObjectCreateDone    ( hOBJECT object );
tERROR  pr_call System_ObjectRevert        ( hOBJECT object, tBOOL reuse_custom_prop );
tERROR  pr_call System_ObjectCheck         ( hOBJECT object, hOBJECT to_check, tIID iid, tPID pid, tDWORD subtype, tBOOL do_not_check_int_struct );
tERROR  pr_call System_ObjectValid         ( hOBJECT object, hOBJECT to_check );
tERROR  pr_call System_ObjectClose         ( hOBJECT object );
tERROR  pr_call System_ObjectSwap          ( hOBJECT object, hOBJECT o1, hOBJECT o2 );
tERROR  pr_call System_ObjectFind          ( hOBJECT object, hOBJECT* result, tIID iid, tPID pid, tDWORD up, tDWORD down, tDWORD flags ); 
tERROR  pr_call System_ObjectGetInternalData( hOBJECT object, tDWORD* size, tDWORD obj_size, tBYTE* data, tDWORD data_size, tObjSerializeFunc convert, tObjCheckCustomPropFunc prop_check, tPTR ctx ); 
tERROR  pr_call System_ObjectSetInternalData( hOBJECT object, tDWORD* size, tDWORD obj_size, const tBYTE* data, tDWORD data_size, tObjDeserializeFunc convert, tPTR ctx ); 
tERROR  pr_call System_ObjectValidateLock  ( hOBJECT object, hOBJECT to_lock, tIID iid, tPID pid );
tERROR  pr_call System_ObjectRelease       ( hOBJECT object );

// error management -- system.c      
tVOID   pr_call System_ErrorCodeSet        ( hOBJECT object, tERROR error );
tERROR  pr_call System_ErrorCodeGet        ( hOBJECT object );

// message management -- system.c
tDWORD  pr_call System_BroadcastMsg            ( hOBJECT object, tIID iid, tDWORD type, tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* blen );
tERROR  pr_call System_SendMsg                 ( hOBJECT object, tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* blen );
tERROR  pr_call System_IntMsgReceive           ( hOBJECT object, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen );
tERROR  pr_call System_RegisterMsgHandler      ( hOBJECT object, tDWORD msg_cls, tDWORD flags, tPTR tree_top, tIID obj_iid, tPID obj_pid, tIID ctx_iid, tPID ctx_pid );
tERROR  pr_call System_RegisterMsgHandlerList  ( hOBJECT object, const tMsgHandlerDescr* handler_list, tDWORD handler_count );
tERROR  pr_call System_GetMsgHandlerList       ( hOBJECT object, tDWORD* result_count, tMsgHandlerDescr* handler_list, tDWORD handler_count );
tERROR  pr_call System_UnregisterMsgHandler    ( hOBJECT object, tDWORD msg_cls, tPTR tree_top );
tERROR  pr_call System_UnregisterMsgHandlerList( hOBJECT object, const tMsgHandlerDescr* handler_list, tDWORD handler_count );

// trace management -- sys_trc.c
tERROR  pr_call System_TraceLevelGet       ( hOBJECT object, tDWORD who, tDWORD* max_level, tDWORD* min_level );
tERROR  pr_call System_TraceLevelSet       ( hOBJECT object, tDWORD who, tDWORD max_level, tDWORD min_level );
tERROR  pr_call System_TracerGet           ( hOBJECT object, hTRACER* result );

// code protection methods -- system.c
tERROR  pr_call System_EnterProtectedCode  ( hOBJECT object, tExceptionReg* reg );
tERROR  pr_call System_LeaveProtectedCode  ( hOBJECT object, tExceptionReg* reg );
tERROR  pr_call System_GetExceptionInfo    ( hOBJECT object, tExceptionReg* reg, tDWORD* ex_code, tPTR* info );
tERROR  pr_call System_ThrowException      ( hOBJECT object, tDWORD ex_code );

// synchronization -- sys_sync.c
tERROR  pr_call System_SyncCreate          ( hOBJECT object, hOBJECT* sync_result, tIID sync_iid, tPID sync_pid, tSTRING name );
tERROR  pr_call System_SyncSet             ( hOBJECT object, hOBJECT  sync_obj );
tERROR  pr_call System_SyncGet             ( hOBJECT object, hOBJECT* sync_result );
tERROR  pr_call System_SyncClose           ( hOBJECT object );
tERROR  pr_call System_SyncLock            ( hOBJECT object, tDWORD wait_time );
tERROR  pr_call System_SyncUnlock          ( hOBJECT object );
tERROR  pr_call System_SyncProtectObject   ( hOBJECT object, tIID sync_iid, tPID sync_pid, tSTRING name );
tERROR  pr_call System_SyncUnprotectObject ( hOBJECT object );

// high level object management -- sys_obj.c 
tERROR  pr_call System_ObjectCreateQuick   ( hOBJECT object, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype );

// user data -- system.c
tERROR  pr_call System_UserDataGet         ( hOBJECT object, tDWORD* data );
tERROR  pr_call System_UserDataSet         ( hOBJECT object, tDWORD  data );

// child/object enumerate methods -- sys_obj.c
tERROR  pr_call System_ChildGetFirst       ( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid );
tERROR  pr_call System_ChildGetLast        ( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid );
tERROR  pr_call System_ObjectGetNext       ( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid );
tERROR  pr_call System_ObjectGetPrev       ( hOBJECT object, hOBJECT* ret_obj, tIID iid, tPID pid );

// ordinary memory management
tERROR  pr_call System_ObjHeapAlloc        ( hOBJECT object, tPTR* result, tDWORD size );
tERROR  pr_call System_ObjHeapRealloc      ( hOBJECT object, tPTR* result, tPTR orig, tDWORD size );
tERROR  pr_call System_ObjHeapFree         ( hOBJECT object, tPTR ptr );
tERROR  pr_call System_ObjHeapResolve      ( hOBJECT object, hHEAP* result, tPTR ptr );
	
// -----------------------------------------
// recognize methods -- sys_hand.c
tERROR  pr_call System_Recognize           ( hOBJECT object, tIID by_iid, tPID by_pid, tDWORD type );
tERROR  pr_call System_RecognizeByList     ( hOBJECT object, tPID* result, tIID by_iid, const tPID* by_pid_list, tDWORD count, tDWORD type );

// reserved system methods -- system.c
tERROR  pr_call System_Reserved            ( hOBJECT object );

typedef tBOOL (*tCheckHandConditionFn)( tHANDLE* handle, tPTR param );

// handle functions
void        pr_call    _HandlePoolInit( void );
void        pr_call    _HandlePoolDeinit( void );
void        pr_call    _HandleFree( tPO* po, tHANDLE* handle );
tERROR      pr_call    _HandleCreate( tPO* po, tHANDLE* parent, tHANDLE** new_handle );
tHANDLE*    pr_call    _handle_check( tPO* po, tVOID* obj, tERROR* err );

#if defined( _DEBUG )
	tHANDLE*  pr_call    _handle_check_debug_func( tPO* po, tVOID* obj, tERROR* err );
	tVOID     pr_call    _handle_uncheck_p_debug_func( tPO* po, tHANDLE* handle );
#endif

#define                _HandleCheck_real(po,object,error) _HandleCheck( po, object, error, 0 )
tHANDLE*    pr_call    _HandleCheck( tPO* po, hOBJECT object, tERROR* error, tBOOL any );
tVOID       pr_call    _ObjectInit0( tHANDLE* handle );
  
// handle find functions
tHANDLE*    pr_call    _HandleFind( tPO* po, tHANDLE* handle, tIID iid, tPID pid, tDWORD up, tDWORD down, tDWORD flags );
tHANDLE*    pr_call    _HandleFindStat( tPO* po, tDWORD* cookie, tCheckHandConditionFn check_fn, tPTR param );

tHANDLE*    pr_call    _ParentSet( tPO* po, tHANDLE* handle, tHANDLE* new_parent, tERROR* error );
                       
tERROR      pr_call    _ObjectCreate( tPO* po, tHANDLE* handle, tHANDLE** new_handle, tINTERFACE* iface ); 
tERROR      pr_call    _ObjectIIDCreate( tPO* po, tHANDLE* handle, tHANDLE** new_handle, tIID iid, tPID pid, tDWORD subtype );
tERROR      pr_call    _ObjectCreateDone( tPO* po, tHANDLE* handle );
tERROR      pr_call    _ObjectRevert( tPO* po, tHANDLE* handle, tBOOL reuse_custom_prop, tDATA* pids );
tERROR      pr_call    _ObjectClose( tPO* po, tHANDLE* handle, tDATA* pids );
tERROR      pr_call    _ObjectCheckLocked( tPO* po, tHANDLE* handle, tDATA* pids );
tERROR      pr_call    _ObjectRelease( tPO* po, tHANDLE* handle );
                      
tERROR      pr_call    _SerializerInit();
tERROR      pr_call    _SerializerDeinit();

// interface functions
void        pr_call    _InterfacePoolInit( void );
void        pr_call    _InterfacePoolDeinit( void );
tBOOL       pr_call    _InterfaceUnload( tPO* po, tINTERFACE* iface );
tERROR      pr_call    _InterfaceGetProp( tPO* po, tINTERFACE* iface, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size );
tERROR      pr_call    _InterfaceAlloc( tPO* po, tINTERFACE** ret_iface );
void        pr_call    _InterfaceFree( tPO* po, tINTERFACE* iface );
tERROR      pr_call    _InterfaceSetNew( tPO* po, tHANDLE* handle, tINTERFACE* niface );
//tERROR      pr_call    _InterfaceReplace( tPO* po, tHANDLE* handle, tINTERFACE* niface );
tERROR      pr_call    _InterfaceLoad( tPO* po, tINTERFACE* iface );
tERROR      pr_call    _InterfaceCompatible( tINTERFACE* iface, tIID iid );
tERROR      pr_call    _InterfaceCompatibleTopBase( tINTERFACE* iface, tIID* iid );
tERROR      pr_call    _InterfaceCheck( tINTERFACE** iface, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, tDWORD flags, hPLUGIN implemented_module );

// interface find functions
typedef tBOOL (*tCheckIFaceConditionFn)( tINTERFACE* iface, tPTR param );
tINTERFACE* pr_call    _InterfaceFind( tDWORD* cookie, tIID iid, tPID pid, tDWORD subtype, tVID vid, tBOOL check_all );
tINTERFACE* pr_call    _InterfaceFindInt( tDWORD* cookie, tCheckIFaceConditionFn check_fn, tPTR param );

tERROR      pr_call    _Interface_0_Init( tPO* po, tINTERFACE* iface );
tERROR      pr_call    _add_iface_to_local_cache( tINTERFACE* iface, tINTERFACE*** iface_table, tUINT* count );

// notification sink methods
tERROR      pr_call    _ns_add( tPO* po, tSNS_ACTION action, tDWORD* id, tFUNC_PTR func, tPTR ptr );
tERROR      pr_call    _ns_del( tPO* po, tDWORD id );
tERROR      pr_call    _ns_get_arr( tSNS_ACTION action, tNotificationSink** sinks, tUINT* count );

// type size
extern const tDWORD _type_size[];
#define  _TypeSize(tid)  _type_size[(((tid)&pTYPE_MASK)>>pTYPE_SHIFT)]



// property functions
tBOOL        pr_call    _PropTableCheck( const tDATA* table, tDWORD propcount, tDATA* maxsize, tBOOL* code_page_implemented, tDATA** pid_user );
const tDATA* pr_call    _PropTableSearch( const tINTERFACE* iface, tPROPID prop, tDWORD* required_pos );
tDWORD       pr_call    _PropInitDword( tDATA* table, tDWORD propcount );
tERROR       pr_call    _PropertyGet( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size );
tERROR       pr_call    _PropertyGetStr( tPO* po, tHANDLE* from, tDATA* result, tPROPID propid, tSTRING buff, tDWORD size, tCODEPAGE receive_cp );
tERROR       pr_call    _PropertyGetStrCP( tPO* po, tHANDLE* handle, tCODEPAGE* cp, tPROPID propid );
tERROR       pr_call    _PropertySet( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size );
tERROR       pr_call    _PropertySetStr( tPO* po, tHANDLE* from, tDATA* result, tPROPID propid, tSTRING pStrz, tDWORD size, tCODEPAGE receive_cp );
tDWORD       pr_call    _PropertySize( tPO* po, tHANDLE* handle, tPROPID prop );
tPID         pr_call    _PropertyGetPID( tPO* po, tHANDLE* handle, tDATA* pid_prop_desc );
tERROR       pr_call    _PropertyGetSync( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size );

// custom properties functions
tCustProp*   pr_call    _CustomPropertyAdd( tHANDLE* handle, tPROPID pid );
tERROR       pr_call    _CustomPropertyGet( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size );
tERROR       pr_call    _CustomPropertyGetStr( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size, tCODEPAGE cp );
tERROR       pr_call    _CustomPropertyGetAll( tPO* po, tHANDLE* handle, tUINT obj_size, tDWORD* out_size, tBYTE** buffer, tDWORD* size, tObjSerializeFunc func, tObjCheckCustomPropFunc prop_check, tPTR ctx );
tERROR       pr_call    _CustomPropertySet( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size, tBOOL use_sink );
tERROR       pr_call    _CustomPropertySetStr( tPO* po, tHANDLE* handle, tDATA* out_size, tPROPID prop, tPTR buffer, tDWORD size, tCODEPAGE cp, tBOOL use_sink );
tERROR       pr_call    _CustomPropertySetAll( tHANDLE* handle, tUINT obj_size, tDATA* out_size, tBYTE** buffer, tDWORD* size, tObjDeserializeFunc func, tPTR ctx );
tDWORD       pr_call    _CustomPropertySize( tHANDLE* handle, tPROPID prop );
tERROR       pr_call    _CustomPropertyDelete( tPO* po, tHANDLE* handle, tPROPID prop );
tERROR       pr_call    _CustomPropertyDeleteList( tPO* po, tHANDLE* handle );
#define                 _CustomPropertyTypeInPlace(pid)   (sizeof(tPTR) >= _TypeSize(pid))
tPTR         pr_call    _CustomPropertyAddr( tCustProp* cp );
tCustProp*   pr_call    _CustomPropertyFind( tHANDLE* handle, tPROPID propid );
tERROR       pr_call    _CustomPropInherit( tPO* po, tHANDLE* handle ); // try to inherit heritable propeties
	
// --- tracing
#define TRACER_GET_CREATE_IF_NOT_EXIST 1
tERROR       pr_call    _TraceLevelPropSet( tPO* po, tHANDLE* handle, tDWORD max_level, tDWORD min_level );
tERROR       pr_call    _TraceLevelSetHandle( tPO* po, tHANDLE* handle, tDWORD who, tDWORD max_level, tDWORD min_level );
tERROR       pr_call    _TraceLevelSetID( tPO* po, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tVID vid, tDWORD max_level, tDWORD min_level );
hTRACER      pr_call    _TracerGet( tPO* po, tHANDLE* handle, tERROR* error, tDWORD flags );

// memory routines
tERROR       pr_call    _MemoryRealloc( tPO* po, tHANDLE* handle, tDWORD size );

// synchronization -- sys_sync.c
tERROR       pr_call    _SyncCreate( tPO* po, tHANDLE* handle, hOBJECT* sync_result, tIID sync_iid, tPID sync_pid, tSTRING name );
tERROR       pr_call    _SyncProtectObject( tPO* po, tHANDLE* handle, tIID sync_iid, tPID sync_pid, tSTRING name );
tERROR       pr_call    _SyncUnprotectObject( tPO* po, tHANDLE* handle );
tERROR       pr_call    _SyncCreateAvailable( hOBJECT dad, hCRITICAL_SECTION* cs );
tERROR       pr_call    _SyncChangeObj( tPO* po, tIID iid, tPID pid, tDWORD subtype );
tERROR       pr_call    _SyncCloseObj( tPO* po );
  
// wrapper management
tERROR       pr_call    _WrapperAdd( tPO* po, tHANDLE* handle, OneFuncPtr addr_func, tPTR param );
tERROR       pr_call    _WrapperWithdraw( tPO* po, tHANDLE* handle, OneFuncPtr addr_func, tPTR param );
tPTR         pr_call    _WrapperNext( tHANDLE* handle );
tPTR         pr_call    _WrapperData( tHANDLE* handle );
tERROR       pr_call    _WrapperCallNext( hOBJECT* obj_ptr, ... );


// msg handler routines
tERROR       pr_call    _MsgSend( tPO* po, tHANDLE* handle, tDWORD msg_cls, tDWORD msg_id, tHANDLE* ctx, tPTR pbuff, tDWORD* blen, tDATA* iids );
tERROR       pr_call    _GetMsgHandlerList( tPO* po, tHANDLE* handle, tUINT obj_size, tUINT* result, tBYTE** buffer, tDWORD* buff_size, tObjSerializeFunc func, tPTR ctx );
tERROR       pr_call    _SetMsgHandlerList( tPO* po, tHANDLE* handle, tUINT obj_size, tUINT* result, tBYTE** buffer, tDWORD* buff_size, tObjDeserializeFunc func, tPTR ctx );


// msg handler routines
tMsgHandler* pr_call   _mh_add          ( tMHArray* mha, tHANDLE* handler, tDWORD flags, tDWORD msg_cls, tIID o_iid, tPID o_pid, tIID c_iid, tPID c_pid, tERROR* error );
tBOOL        pr_call   _mh_remove       ( tMHArray* mha, tHANDLE* handler, tDWORD msg_cls );
tBOOL        pr_call   _mh_remove_by_ind( tMHArray* mha, tDWORD ind );
tBOOL        pr_call   _mh_remove_all   ( tMHArray* mha );
tMsgHandler* pr_call   _mh_find         ( tMHArray* mha, tDWORD* start_point, tHANDLE* obj );
tMsgHandler* pr_call   _mh_find_handler ( tMHArray* mha, tDWORD* start_point, tHANDLE* obj, tDWORD msg_cls, tIID o_iid, tPID o_pid );
tMsgHandler* pr_call   _mh_c_add        ( tMHCacheArray* mhc, tMsgHandler* mh, hOBJECT recv_point, tERROR* error );
tBOOL        pr_call   _mh_c_remove     ( tMHCacheArray* mhc, tMsgHandler* mh );
tVOID        pr_call   _ClearMsgHandlerCache( tHANDLE* handle );


// memory routines
tVOID        pr_call _MemInitialize( tVOID );
tERROR       pr_call _MemInitObject( tPO* po );
tERROR       pr_call PrAlloc( tPTR* ret_ptr, tUINT mem_size );
tERROR       pr_call PrRealloc( tPTR* ret_ptr, tPTR ptr, tUINT mem_size );
tERROR       pr_call PrFree( tPTR ptr );
tERROR       pr_call _MemDeinitialize( tPO* po );
	
// syncronization routines
tERROR       pr_call _SyncInitObj( tPO* po );
tERROR       pr_call _SyncDeinitObj( tVOID );

// impex manager routines
tERROR       pr_call _ImpexInitObj( tPO* po );
tERROR       pr_call _ImpexDeinitObj();
	
// impex manager routines
tERROR       pr_call _TracerInitObj( tPO* po );
tERROR       pr_call _TracerDeinitObj( tPO* po );

// working with the handle hierarchy (handle.c)
// all of these must be called under wlock condition
tVOID        pr_call _Remove( tHANDLE* handle );
tVOID        pr_call _AddBefore( tHANDLE* handle, tHANDLE* pos );
tVOID        pr_call _AddAfter( tHANDLE* handle, tHANDLE* pos );
tVOID        pr_call _AddToChildList( tHANDLE* handle, tHANDLE* parent );

tINT         pr_call _compare_timestamp( hPLUGIN first, hPLUGIN second );
	
tERROR       pr_call _SerializerInit();
tERROR       pr_call _SerializerDeinit();

tBOOL        pr_call _check_plugin_exports_used( hPLUGIN plugin );

#define mmove  memmove
#define scpy   strcpy
#define mcpy   memcpy
#define mset   memset
#define mcmp   memcmp
#define slen   strlen

  
// ---
// not initialized object function initiator
tERROR pr_call _ObjectNotInitializedDummy( hOBJECT _this );
const tPTR NotInitializedMethod( tHANDLE* handle, tPTR param, tBOOL ex_in, tDWORD num, tDWORD* data );


#define METHOD_MAX_COUNT 0x80

// ---
// not initialized object function array
typedef tERROR (pr_call *_ObjNotInitializedFunctionType)( hOBJECT );
extern _ObjNotInitializedFunctionType _ObjNotInitializedFunctions[METHOD_MAX_COUNT];

// freed object function array
typedef tERROR (pr_call *_ObjFreedFunctionType)( hOBJECT );
extern _ObjFreedFunctionType _ObjFreedFunctions[METHOD_MAX_COUNT];


#define IF_NOT_HIDDEN( h, e )        if ( ((h)->flags & hf_HIDDEN) ) {(e) = errOK;} else

// ---
//#ifdef _DEBUG
//
//  void call_int_meth( tPO* po, tHANDLE* h, void* m, tERROR* err, tDWORD n, tPTR p1, tPTR p2, tPTR p3, tPTR p4, tPTR p5, tPTR p6, tPTR p7, tPTR p8 );
//  #define CALL_INT_METH_0( h, m, e )                                  {IF_NOT_HIDDEN(h,e) call_int_meth( po, h, (h)->iface->internal->m, &e, 0, 0, 0, 0, 0, 0, 0, 0, 0 );}
//  #define CALL_INT_METH_1( h, m, e, p1 )                              {IF_NOT_HIDDEN(h,e) call_int_meth( po, h, (h)->iface->internal->m, &e, 1, ((tPTR)(p1)), 0, 0, 0, 0, 0, 0, 0 );}
//  #define CALL_INT_METH_2( h, m, e, p1, p2 )                          {IF_NOT_HIDDEN(h,e) call_int_meth( po, h, (h)->iface->internal->m, &e, 2, ((tPTR)(p1)), ((tPTR)(p2)), 0, 0, 0, 0, 0, 0 );}
//  #define CALL_INT_METH_6( h, m, e, p1, p2, p3, p4, p5, p6 )          {IF_NOT_HIDDEN(h,e) call_int_meth( po, h, (h)->iface->internal->m, &e, 6, ((tPTR)(p1)), ((tPTR)(p2)), ((tPTR)(p3)), ((tPTR)(p4)), ((tPTR)(p5)), ((tPTR)(p6)), 0, 0 );}
//  #define CALL_INT_METH_8( h, m, e, p1, p2, p3, p4, p5, p6, p7, p8 )  {IF_NOT_HIDDEN(h,e) call_int_meth( po, h, (h)->iface->internal->m, &e, 8, ((tPTR)(p1)), ((tPTR)(p2)), ((tPTR)(p3)), ((tPTR)(p4)), ((tPTR)(p5)), ((tPTR)(p6)), ((tPTR)(p8)), ((tPTR)(p8)) );}
//  
//  #define CALL_CHILD_NEW( h, e, p1, p2, p3, p4 )                                     \
//    IF_NOT_HIDDEN( h, e ) {                                                          \
//      _PR_ENTER_PROTECTED(h) {                                                       \
//        e = (h)->iface->internal->ChildNew( MakeObject(h), (p1), (p2), (p3), (p4) ); \
//      } _PR_LEAVE_PROTECTED(e);                                                      \
//    }
//
//  #define CALL_MSG_RECV( h, e, p1, p2, p3, p4, p5, p6, p7 ) \
//    IF_NOT_HIDDEN( h, e ) {                                 \
//      _PR_ENTER_PROTECTED(h) {                              \
//        e = (h)->iface->internal->MsgReceive( MakeObject(h), (p1), (p2), (p3), (p4), (p5), (p6), (p7) ); \
//      } _PR_LEAVE_PROTECTED(e);                             \
//    }
//
//  #define CALL_INT_OBJ_NEW( h, i, e, o )                              {call_int_meth( po, h, i->internal->ObjectNew,  &e, 1, ((tPTR)(o)), 0, 0, 0, 0, 0, 0, 0 );}
//  
//#else


  #define CALL_INT_METH_0( h, m, e )                                                      \
    IF_NOT_HIDDEN( h, e ) {                                                               \
      _PR_ENTER_PROTECTED(h) {                                                            \
        e = (h)->iface->internal->m( MakeObject(h) );                                     \
      } _PR_LEAVE_PROTECTED(e);                                                           \
    }

  #define CALL_INT_METH_1( h, m, e, p1 )                                                  \
    IF_NOT_HIDDEN( h, e ) {                                                               \
      _PR_ENTER_PROTECTED(h) {                                                            \
        e = (h)->iface->internal->m( MakeObject(h), (p1) );                               \
      } _PR_LEAVE_PROTECTED(e);                                                           \
    }

  #define CALL_INT_METH_2( h, m, e, p1, p2 )                                              \
    IF_NOT_HIDDEN( h, e ) {                                                               \
      _PR_ENTER_PROTECTED(h) {                                                            \
        e = (h)->iface->internal->m( MakeObject(h), (p1) );                               \
      } _PR_LEAVE_PROTECTED(e);                                                           \
    }

  #define CALL_CHILD_NEW( h, e, p1, p2, p3, p4 )                                          \
    IF_NOT_HIDDEN( h, e ) {                                                               \
      _PR_ENTER_PROTECTED(h) {                                                            \
        e = (h)->iface->internal->ChildNew( MakeObject(h), (p1), (p2), (p3), (p4) );      \
      } _PR_LEAVE_PROTECTED(e);                                                           \
    }

  #define CALL_INT_METH_6( h, m, e, p1, p2, p3, p4, p5, p6 )                              \
    IF_NOT_HIDDEN( h, e ) {                                                               \
      _PR_ENTER_PROTECTED(h) {                                                            \
        e = (h)->iface->internal->m( MakeObject(h), (p1), (p2), (p3), (p4), (p5), (p6) ); \
      } _PR_LEAVE_PROTECTED(e);                                                           \
    }

  #define CALL_MSG_RECV( h, e, p1, p2, p3, p4, p5, p6, p7 )                               \
    IF_NOT_HIDDEN( h, e ) {                                                               \
      _PR_ENTER_PROTECTED(h) {                                                            \
        e = (h)->iface->internal->MsgReceive( MakeObject(h), (p1), (p2), (p3), (p4), (p5), (p6), (p7) ); \
      } _PR_LEAVE_PROTECTED(e);                                                           \
    }

  #define CALL_INT_METH_8( h, m, e, p1, p2, p3, p4, p5, p6, p7, p8 )                      \
    IF_NOT_HIDDEN( h, e ) {                                                               \
      _PR_ENTER_PROTECTED(h) {                                                            \
        e = (h)->iface->internal->m( MakeObject(h), (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8) ); \
      } _PR_LEAVE_PROTECTED(e);                                                           \
    }

  #define CALL_INT_OBJ_NEW( h, i, e, o )                                                  \
      _PR_ENTER_PROTECTED(h) {                                                            \
        e = (i)->internal->ObjectNew( MakeObject(h), o );                                 \
      } _PR_LEAVE_PROTECTED(e);

//#endif



// ---
// Property table macros
#define PROP_IDENTIFIER(a)   ((tPROPID)(a[0]))
#define PROP_BUFFER(a)       (((tPTR)(a[1])))
#define PROP_OFFSET(a)       ((tDATA)(a[1]))
#define PROP_SIZE(a)         ((tDATA)(a[2]))
#define PROP_MODE(a)         ((tDATA)(a[3]))
#define PROP_GETFN(a)        ((tIntFnPropIO)(a[4]))
#define PROP_SETFN(a)        ((tIntFnPropIO)(a[5]))
#define PROP_ARRAY_SIZE      6


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Root. Inner data structure
typedef struct tag_RootData {
	tSTRING*       named_props;
	tUINT          named_count;
	tOUTPUT_FUNC   output_func; // current output func for system tracer
	tDWORD         load_flags;  // force modules to load if some flags is set
	tDWORD         unuse_period;
	tBOOL          unload_unused_plugins;
} tRootData;
// AVP Prague stamp end


// ---
typedef struct tag_RootObject {
  tOBJECT   o;
  tRootData d;
} tRootObject;


// ---
extern tRootObject root_obj;


#define ROOT_KERNEL_SET      (0x01000000L)
#define ROOT_LOADER_SET      (0x02000000L)
#define ROOT_APPLICATION_SET (0x04000000L)
#define ROOT_STRING_SET      (0x08000000L)


#define USER_SINKS 3

// ---
// global kernel objects
extern tHANDLE*              g_hRoot;
extern tHANDLE*              g_plugins;
//extern tHANDLE*              g_statics;
extern hHEAP                 g_heap;
extern hHEAP                 g_kernel_heap;
extern hTRACER               g_tracer;

extern tPO*                  g_po;
extern tIID                  g_sync_iids[];
extern tDWORD                g_sync_iids_count;
extern tNotificationSinkArr* g_sinks[USER_SINKS];
extern tBOOL                 g_exit_now;

// ---
extern tSynchroEvent*        g_synchro_beg_closers;
extern tSynchroEvent*        g_synchro_end_closers;
//extern tHANDLE*            g_condemned_list;

extern tBOOL                 g_unsave_vtbl; // vtbl ptr must not be changed

// ---
// handle of the plugin loading at the moment to place to tINTERFACE::plugin field during interface registration
extern hPLUGIN g_now_loading;

// ---
// count of objects
extern tUINT g_handle_count;
extern tUINT g_handle_count_peak;
extern tUINT g_handle_ext_count;
extern tUINT g_handle_ext_count_peak;

#define _chk_ext_count \
	(++g_handle_ext_count, ((g_handle_ext_count > g_handle_ext_count_peak) ? (g_handle_ext_count_peak = g_handle_ext_count) : (g_handle_ext_count_peak)))

// ---
// external(loader) protointerfaces 
extern iPROT* Prot;

#if defined(_PRAGUE_TRACE_)
	#ifdef PR_TRACE
		#undef PR_TRACE
		#define PR_TRACE(param)  pr_trace param
	#endif
#endif


#if defined(_DEBUG)

	#define CALLER_INFO( first_stack_parameter, array ) \
		(( GetCallerId && PR_SUCC(GetCallerId(&first_stack_parameter,array,2*sizeof(tDATA))) ) ? array : 0 )

#else

	#define CALLER_INFO( first_stack_parameter, array ) \
		0

#endif


#define PR_TRACE_HANDLE_LOCKS( h, l, s )                                                           \
	PR_TRACE((                                                                                       \
		MakeObject(h), l, "krn\tobjref(obj=0x%08x, ref:%03d, %tiid, %tpid, hdl=0x%08x): %s",      \
		(!h->obj ? 0 : (((tDATA)h->obj) + sizeof(tHANDLE*))), h->ref, GET_IID_C(h), GET_PID_C(h), h, s \
	))


#if !defined(_DEBUG)
	#define HANDLE_LOCK(ci,h,l,s)   \
		++h->ref;                     \
		PR_TRACE_HANDLE_LOCKS(h,l,s)

#else

	#define HANDLE_LOCK(ci,h,l,s)   \
		++h->ref;                     \
		if ( ci && h->ext ) {         \
			h->ext->reserved0 = ci[0];  \
			h->ext->reserved1 = ci[1];  \
		}                             \
		PR_TRACE_HANDLE_LOCKS(h,l,s); \
		_lock_handle( h, l, s )

	tVOID _lock_handle( tHANDLE* h, tDWORD trace_level, const tCHAR* description );

#endif

#if defined( _DEBUG )
	tVOID _check_handle_for_msg_recv( tHANDLE* handle );
#else
	#define _check_handle_for_msg_recv( handle )
#endif

#ifdef __cplusplus
	}
#endif

#endif


