/*-----------------02.05.99 12:57-------------------
 * Project Prague                                 *
 * Subproject Kernel                              *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 * revision 2                                     *
--------------------------------------------------*/
// generic handle definition

#ifndef __PR_VTBL_H
#define __PR_VTBL_H


// system interface definition ( internal representation )
// any handle has this interface

// -----------------------------------------
// property management
typedef tERROR  (pr_call *tPropertyInitialized)( hOBJECT object, tPROPID prop_id, tBOOL val );

// -----------------------------------------
// object assotiated memory management
typedef tERROR (pr_call *tMemorySize    )( hOBJECT object, tDWORD* size );
typedef tERROR (pr_call *tMemoryRealloc )( hOBJECT object, tDWORD size );

// -----------------------------------------
// error management methods
typedef tVOID  (pr_call *tErrorCodeSetOld)( hOBJECT object, tERROR error );

// -----------------------------------------
// memory management methods
#define CALL_SYS_MemorySize(obj,size)                ((obj)->sys->MemorySize   (((hOBJECT)(obj)),size))
#define CALL_SYS_MemoryRealloc(obj,size)             ((obj)->sys->MemoryRealloc(((hOBJECT)(obj)),size))
#define CALL_SYS_ErrorCodeSet(obj,error)             ((obj)->sys->ErrorCodeSet (((hOBJECT)(obj)),error))
#define CALL_SYS_PropertyInitialized(obj,propid,val) ((obj)->sys->PropertyInitialized(((hOBJECT)(obj)),propid,val))

// -----------------------------------------
typedef struct tag_iSYSTEMVTBL {
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

  tReservedMeth              reserved[RESERVED_COUNT];
	
	tMemorySize                MemorySize;
	tMemoryRealloc             MemoryRealloc;
	tErrorCodeSetOld           ErrorCodeSetOld;
	tPropertyInitialized       PropertyInitialized;

} iSYSTEMVTBL;

#if defined( __cplusplus ) //&& !defined(_USE_VTBL)

	#define OBJ_IMPL( class_name )                                                \
		protected:                                                                  \
			class_name() {}                                                           \
			class_name(const class_name&) {}                                          \
		private:                                                                    \
			tPTR  m_data;                                                             \
			struct hObj {                                                             \
			  tPTR         vtbl;                                                      \
			  iSYSTEMVTBL* sys;                                                       \
			};                                                                        \
			operator iSYSTEMVTBL* () { return ((hObj*)this)->sys; }                   \
		protected:                                                                  \
			tERROR	pr_call propInitialized( tPROPID prop_id, tBOOL val ) {           \
			  iSYSTEMVTBL* sys = *this;                                               \
				return sys->PropertyInitialized((hOBJECT)this, prop_id, val);           \
      }                                                                         \
		public:                                                                     \
			typedef void (class_name::*TFn)();                                        \
			union cIFnCust {                                                          \
				cIFnCust(TFn fn):u_fn(fn){};                                            \
				operator tDATA(){ return u_ifn; }                                       \
				TFn   u_fn;                                                             \
				tDATA u_ifn;                                                            \
			}

#if !defined(_USE_VTBL)
struct pr_abstract cObjImpl : public cObj {
private:
	cObjImpl();
//private:
  //typedef struct tag_hOBJ {
  //  const tPTR         vtbl;   // interface defined function table - NULL for generic object
  //  const iSYSTEMVTBL* sys;    // system defined function table
  //} *hOBJ;
//protected:

	//iSYSTEMVTBL* isys()                         { return (iSYSTEMVTBL*)((hOBJ)this)->sys; }

	//tERROR	pr_call memorySize(tDWORD* size)    { return isys()->MemorySize((hOBJECT)this, size); }
	//tERROR	pr_call memoryRealloc(tDWORD size)	{ return isys()->MemoryRealloc((hOBJECT)this, size); }

	//tERROR	pr_call propInitialized(tPROPID prop_id, tBOOL val) { return isys()->PropertyInitialized((hOBJECT)this, prop_id, val); }
	//tVOID	pr_call errorCodeSetOld(tERROR error)                 { isys()->ErrorCodeSetOld((hOBJECT)this, error); }

//	typedef void (cObjImpl::*TFn)();

//	union cIFnCust {
//		cIFnCust(TFn fn):u_fn(fn){};
//		operator tDWORD(){ return u_ifn; }
//		TFn				u_fn;
//		tDWORD		u_ifn;
//	};
};


#define FN_CUST(fn)	((tDATA)cIFnCust((TFn)&Scope::fn))

#define mINTERNAL_TABLE_BEGIN(iface)		static iINTERNAL i_##iface##_vtbl, *_i_vtbl = &i_##iface##_vtbl; \
												_i_vtbl->ObjectNew=(tIntFnObjectNew)ObjectNew;
#define mINTERNAL_METHOD( method)				_i_vtbl->method=(tIntFn##method)FN_CUST(method);
#define mINTERNAL_STATIC(method)				_i_vtbl->method=(tIntFn##method)method;
#define mINTERNAL_TABLE_END(iface)

#define mEXTERNAL_TABLE_BEGIN(iface)		static i##iface##Vtbl e_##iface##_vtbl = {
#define mEXTERNAL_TABLE_BEGIN_EX(iface, pface)		static i##iface##Vtbl e_##pface##_vtbl = {
#define mEXTERNAL_METHOD(iface, method)	(fnp##iface##_##method)FN_CUST(method),
#define mEXTERNAL_TABLE_END(iface)			};

#define mDECLARE_INITIALIZATION_NO_DELETE(scope) \
	public:                                                                \
		typedef scope Scope;                                                 \
		inline void *__cdecl operator new(size_t, void *_)	{ return _; }    \
		static tERROR pr_call Register( hROOT root );                        \
		static tERROR pr_call ObjectNew( hOBJECT object, tBOOL construct ) { \
			if (construct)                                                     \
				new ((void*)object) scope;                                       \
			else                                                               \
				((scope*)object)->scope::~scope();                               \
			return errOK;                                                      \
		}

#define mDECLARE_INITIALIZATION(scope) \
	public:                                                                \
		inline void __cdecl operator delete(void *, void *) {return; }       \
		mDECLARE_INITIALIZATION_NO_DELETE(scope)


#endif // !defined(_USE_VTBL)

#endif // __cplusplus

#endif
