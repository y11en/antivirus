#ifndef _MAPIEDK_FUNC_TYPES_H_
#define _MAPIEDK_FUNC_TYPES_H_

#define _PRIVATE_EDKDEBUG_H
#include <edk.h>

#define MAPIEDK_FUNC_DEF(_func_name_)	_func_name_##_t* p##_func_name_;
#define MAPIEDK_FUNC_NULL(_func_name_)	p##_func_name_ = 0;
#define MAPIEDK_FUNC_INIT(_func_name_)	p##_func_name_ = _func_name_;
#define MAPIEDK_FUNC_INIT_FROM_MODULE(hModule, bInitRes, _func_name_)\
	p##_func_name_ = (_func_name_##_t*)GetProcAddress(hModule, #_func_name_);\
	bInitRes &= (p##_func_name_ != NULL);
#define MAPIEDK_FUNC_INIT_FROM_MODULE_BY_NAME(hModule, bInitRes, _func_name_, _dll_name_)\
	p##_func_name_ = (_func_name_##_t*)GetProcAddress(hModule, #_dll_name_);\
	bInitRes &= (p##_func_name_ != NULL);


typedef MAPIINITIALIZE     MAPIInitialize_t;
typedef MAPIUNINITIALIZE   MAPIUninitialize_t;
typedef MAPILOGONEX        MAPILogonEx_t;
typedef MAPIALLOCATEBUFFER MAPIAllocateBuffer_t;;
typedef MAPIALLOCATEMORE   MAPIAllocateMore_t;
typedef MAPIFREEBUFFER     MAPIFreeBuffer_t;
typedef MAPIADMINPROFILES  MAPIAdminProfiles_t;

typedef SCODE (STDMETHODCALLTYPE CreateIProp_t)(
  LPCIID lpInterface,                  
  ALLOCATEBUFFER FAR * lpAllocateBuffer,   
  ALLOCATEMORE FAR * lpAllocateMore,   
  FREEBUFFER FAR * lpFreeBuffer,       
  LPVOID lpvReserved,                  
  LPPROPDATA FAR * lppPropData         
);
typedef HRESULT (STDMETHODCALLTYPE HrQueryAllRows_t)(
  LPMAPITABLE ptable,    
  LPSPropTagArray ptaga,   
  LPSRestriction pres,   
  LPSSortOrderSet psos,  
  LONG crowsMax,         
  LPSRowSet FAR * pprows 
);
typedef ULONG (STDMETHODCALLTYPE UlRelease_t)(
  LPVOID punk  
);
typedef BOOL (STDMETHODCALLTYPE FPropExists_t)(
  LPMAPIPROP pobj,   
  ULONG ulPropTag    
);
typedef void (STDMETHODCALLTYPE FreePadrlist_t)(
  LPADRLIST padrlist  
);
typedef void (STDMETHODCALLTYPE FreeProws_t)(
  LPSRowSet prows  
);
typedef SCODE (STDMETHODCALLTYPE ScDupPropset_t)(
  int cprop,                           
  LPSPropValue rgprop,                 
  LPALLOCATEBUFFER lpAllocateBuffer,   
  LPSPropValue FAR * prgprop           
);

typedef HRESULT (STDMETHODCALLTYPE HrGetOneProp_t) (LPMAPIPROP lpMapiProp, ULONG ulPropTag, LPSPropValue FAR *lppProp) ;
typedef HRESULT (HrMAPIGetPropBinary_t) (  IN LPMAPIPROP lpObj,  IN ULONG ulPropTag,  OUT ULONG *lpcbProp,  OUT LPVOID *lppvProp);
typedef HRESULT (HrMAPIGetPropBoolean_t) (  IN LPMAPIPROP lpObj,  IN ULONG ulPropTag,  OUT BOOL *lpfProp);
typedef HRESULT (HrMAPIGetPropString_t) (  IN LPMAPIPROP lpObj,  IN ULONG ulPropTag,  OUT ULONG *lpcbProp,  OUT LPVOID *lppvProp);
typedef HRESULT (HrMAPIGetPropLong_t) (  IN LPMAPIPROP lpObj,  IN ULONG ulPropTag,  OUT ULONG *lpulProp) ;
typedef HRESULT (HrMAPISetPropBinary_t) (  IN LPMAPIPROP lpObj,  IN ULONG ulPropTag,  IN ULONG cbProp,  IN LPVOID lpvProp) ;
typedef HRESULT (HrMAPISetPropBoolean_t) (  IN LPMAPIPROP lpObj,  IN ULONG ulPropTag,  IN BOOL fProp) ;
typedef HRESULT (HrMAPISetPropString_t) (  IN LPMAPIPROP lpObj,  IN ULONG ulPropTag,  IN LPVOID lpvProp);
typedef HRESULT (HrMAPISetPropLong_t) (  IN LPMAPIPROP lpObj,  IN ULONG ulPropTag,  IN ULONG ulProp);
typedef HRESULT (HrMAPIDeleteMessage_t) (  IN LPMAPIFOLDER lpFolder,  IN ULONG cbeid,  IN LPENTRYID lpeid);
typedef HRESULT (HrMAPIMoveMessage_t) (  IN LPMAPIFOLDER lpSrcFolder,  IN LPMAPIFOLDER lpDstFolder,  IN ULONG cbeid,  IN LPENTRYID lpeid);
typedef HRESULT (HrMAPICopyMessage_t) (  IN LPMAPIFOLDER lpSrcFolder,  IN LPMAPIFOLDER lpDstFolder,  IN ULONG cbeid,  IN LPENTRYID lpeid);
typedef HRESULT (HrMAPIGetPropSystime_t) (  IN LPMAPIPROP lpObj,  IN ULONG ulPropTag,  OUT LPFILETIME lpSystime);
typedef HRESULT (HrMAPIOpenFolderEx_t) ( IN LPMDB lpMdb,  IN CHAR chSep,  IN LPCSTR lpszFolderPath,  OUT LPMAPIFOLDER * lppFolder);
typedef HRESULT (HrMAPICreateSizedAddressList_t) (  IN ULONG cEntries,  OUT LPADRLIST *lppAdrList);
typedef HRESULT (HrMAPISetAddressList_t) (  IN ULONG iEntry,  IN ULONG cProps,   IN LPSPropValue lpPropValues,  IN OUT LPADRLIST lpAdrList);
typedef HRESULT (HrOpenExchangePrivateStore_t) ( IN LPMAPISESSION lphSession,  OUT LPMDB * lppMDB);
typedef HRESULT (HrMAPIFindOutbox_t) (  IN LPMDB lpMdb,  OUT ULONG *lpcbeid,  OUT LPENTRYID *lppeid);
typedef HRESULT (HrMAPICreateAddressList_t) (  IN ULONG cProps,   IN LPSPropValue lpPropValues,  OUT LPADRLIST *lppAdrList);
typedef LPSPropValue (STDAPICALLTYPE LpValFindProp_t) (ULONG ulPropTag, ULONG cValues, LPSPropValue lpPropArray);
typedef LPSPropValue (STDAPICALLTYPE PpropFindProp_t) (LPSPropValue lpPropArray, ULONG cValues, ULONG ulPropTag);
typedef HRESULT (HrMAPIFindDefaultMsgStore_t) (IN LPMAPISESSION lplhSession, OUT ULONG * lpcbeid, OUT LPENTRYID * lppeid);
typedef HRESULT (HrMAPIFindInbox_t) (  IN LPMDB lpMdb,  OUT ULONG *lpcbeid,  OUT LPENTRYID *lppeid);


#endif//_MAPIEDK_FUNC_TYPES_H_