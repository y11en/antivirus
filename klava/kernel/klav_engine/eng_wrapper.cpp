/********************************************************
*  Engine wrapper implementation (for C# applications)  *
********************************************************/
#include <klava/klavdef.h>
#if defined KL_PLATFORM_WINDOWS // Wrapper actual only under Windows platform
#include <klava/klaveng.h>
#include <klava/klav_utils.h>
#include <klava/klav_props.h>
#include <klava/formatting.h>

// macroses & declarations
extern "C" KLAV_ERR KLAV_CALL KLAV_SCANNER_ENGINE_CREATE_FN (hKLAV_Scanner_Engine *phengine);
extern KLAV_Malloc_Alloc g_malloc;

#define CHECK_HANDLE(NAME) if(!NAME) return KLAV_EACCESS;

typedef KLAV_ERR (__stdcall * PFN_KLAV_Message_Printer)(KLAV_CONTEXT_TYPE context, uint32_t level, const char *msg);
typedef KLAV_ERR (__stdcall * PFN_KLAV_Handle_Callback)(KLAV_CONTEXT_TYPE context, KLAV_CALLBACK_CODE code,	void *props);

struct CallbackHandler: public KLAV_IFACE_IMPL(KLAV_Callback_Handler)
{
private:
	PFN_KLAV_Handle_Callback m_impl;
public:
	virtual KLAV_ERR KLAV_CALL handle_callback (
				KLAV_CONTEXT_TYPE  context,
				KLAV_CALLBACK_CODE code,
				KLAV_Properties *  props
				) {
					if(m_impl)
						return (*m_impl)(context, code, (void*)props);
					else
						return KLAV_ENOINIT;
				}
	CallbackHandler(PFN_KLAV_Handle_Callback pfn_impl) { m_impl = pfn_impl; }
};

struct MessagePrintHandler: public KLAV_IFACE_IMPL(KLAV_Message_Printer)
{
private: 
	PFN_KLAV_Message_Printer m_impl;
#define DEFBUFF_SIZE 256
	char msg[DEFBUFF_SIZE];
public:
	virtual KLAV_ERR KLAV_CALL print_message_v (
				KLAV_CONTEXT_TYPE context,
				uint32_t     level,
				const char * fmt,
				va_list ap
				){
					if(m_impl)
					{												
						int cc = kl_vsnprintf(msg, DEFBUFF_SIZE, fmt, ap);
						if(cc < DEFBUFF_SIZE)
							return (*m_impl)(context, level, msg);
						else
						{
							KLAV_Alloc * allocator = & g_malloc;
							char* buff = KLAV_NEW(allocator) char[cc+1];
							kl_vsnprintf(buff, cc+1, fmt, ap);
							KLAV_ERR err = (*m_impl)(context, level, msg);
							KLAV_DELETE(buff, allocator);
							return err;
						}
					}
					else
						return KLAV_ENOINIT;
	}
	MessagePrintHandler(PFN_KLAV_Message_Printer pfn_impl) { m_impl = pfn_impl; }
};


// Wrapper functions implementation

// Scan Engine
extern "C" KLAV_ERR KLAV_CALL SE_CreateHandle(void **hScEngine)
{
	return KLAV_SCANNER_ENGINE_CREATE_FN((KLAV_Scanner_Engine**)hScEngine);
}

extern "C" KLAV_ERR KLAV_CALL SE_FreeHandle(void *hScEngine)
{
	KLAV_ERR err = KLAV_OK;
	if(!hScEngine)
		return KLAV_EALREADY;
	static_cast<hKLAV_Scanner_Engine>(hScEngine)->destroy();	
	return err;
}

extern "C" KLAV_ERR KLAV_CALL SE_SetOption(void *hScEngine, const char *optname, const char *optval)
{
	CHECK_HANDLE(hScEngine)
	static_cast<hKLAV_Scanner_Engine>(hScEngine)->set_option(optname, optval);
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL SE_GetProperiesHandle(void *hScEngine, void **hProps)
{
	CHECK_HANDLE(hScEngine)	
	*hProps = (void*)static_cast<hKLAV_Scanner_Engine>(hScEngine)->get_properties();
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL SE_CreatePropertyBagHandle(void *hScEngine, void **hPropBag)
{
	CHECK_HANDLE(hScEngine)
	*hPropBag = (void*)static_cast<hKLAV_Scanner_Engine>(hScEngine)->create_property_bag();	
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL SE_SetIface(void *hScEngine, uint32_t ifc_id, void *ifc)
{
	CHECK_HANDLE(hScEngine)
	static_cast<hKLAV_Scanner_Engine>(hScEngine)->set_iface(ifc_id, ifc);
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL SE_GetIface(void *hScEngine, uint32_t ifc_id, void **ifc)
{
	CHECK_HANDLE(hScEngine)
	*ifc = (void*)static_cast<hKLAV_Scanner_Engine>(hScEngine)->get_iface(ifc_id);
	if(!*ifc)
		return KLAV_ENOTFOUND;
	else
		return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL SE_Load(void *hScEngine, void *hProps, const char *db_path)
{
	CHECK_HANDLE(hScEngine)
	return static_cast<hKLAV_Scanner_Engine>(hScEngine)->load(db_path, (KLAV_Properties*)hProps);	
}

extern "C" KLAV_ERR KLAV_CALL SE_Unload(void *hScEngine)
{
	CHECK_HANDLE(hScEngine)
	return static_cast<hKLAV_Scanner_Engine>(hScEngine)->unload();
}

extern "C" KLAV_ERR KLAV_CALL SE_ProcessFile(void *hScEngine, void *hProps, const char *path)
{
	CHECK_HANDLE(hScEngine)
	return static_cast<hKLAV_Scanner_Engine>(hScEngine)->process_file(path, (KLAV_Properties*)hProps);
}

extern "C" KLAV_ERR KLAV_CALL SE_GetEngineHandle(void *hScEngine, void **hEngine)
{
	CHECK_HANDLE(hScEngine)
	*hEngine = (void*)static_cast<hKLAV_Scanner_Engine>(hScEngine)->get_engine();
	if(!*hEngine)
		return KLAV_ENOTFOUND;
	else
		return KLAV_OK;
}

extern "C" KLAV_EXTERN_C KLAV_ERR KLAV_CALL SE_SetCallbackIface(void *hScEngine, PFN_KLAV_Handle_Callback Callback)
{
	KLAV_Alloc * allocator = & g_malloc;
	CallbackHandler *ch = KLAV_NEW (allocator) CallbackHandler (Callback);	
	return SE_SetIface(hScEngine, KLAV_IFACE_ID(Callback_Handler), (void*)ch);
}

extern "C" KLAV_EXTERN_C KLAV_ERR KLAV_CALL SE_FreeCallbackIface(void *hScEngine)
{
	void* ch = 0;
	if(SE_GetIface(hScEngine, KLAV_IFACE_ID(Callback_Handler), &ch)==KLAV_OK)
	{
		KLAV_Alloc * allocator = & g_malloc;
		KLAV_ERR err = SE_SetIface(hScEngine, KLAV_IFACE_ID(Callback_Handler), 0);
		KLAV_DELETE((CallbackHandler*)ch, allocator);
		return err;
	}
	return KLAV_ENOTFOUND;
}

extern "C" KLAV_EXTERN_C KLAV_ERR KLAV_CALL SE_SetMessagePrintIface(void *hScEngine, PFN_KLAV_Message_Printer MessagePrinter)
{
	KLAV_Alloc * allocator = & g_malloc;
	MessagePrintHandler *mph = KLAV_NEW (allocator) MessagePrintHandler (MessagePrinter);
	return SE_SetIface(hScEngine, KLAV_IFACE_ID(Message_Printer), (void*)mph);
}

extern "C" KLAV_EXTERN_C KLAV_ERR KLAV_CALL SE_FreeMessagePrintIface(void *hScEngine)
{
	void* mph = 0;
	if(SE_GetIface(hScEngine, KLAV_IFACE_ID(Message_Printer), &mph)==KLAV_OK)
	{
		KLAV_Alloc * allocator = & g_malloc;		
		KLAV_ERR err = SE_SetIface(hScEngine, KLAV_IFACE_ID(Message_Printer), 0);
		KLAV_DELETE((MessagePrintHandler*)mph, allocator);
		return err;
	}
	return KLAV_ENOTFOUND;
}

// KLAV Properties
#define KP_GET_PROPVAL(_val) { CHECK_HANDLE(hProps) \
	const klav_propval_t* ppVal = static_cast<KLAV_Properties*>(hProps)->get_property(id);\
	if(ppVal)\
		*pVal = ppVal->_val;\
	else\
		return KLAV_ENOTFOUND;\
		return KLAV_OK; }

extern "C" KLAV_ERR KLAV_CALL KP_GetIntProperty(void* hProps, klav_propid_t id, int32_t *pVal) KP_GET_PROPVAL(i)
extern "C" KLAV_ERR KLAV_CALL KP_GetUIntProperty(void* hProps, klav_propid_t id, uint32_t *pVal) KP_GET_PROPVAL(ui)
extern "C" KLAV_ERR KLAV_CALL KP_GetLongProperty(void* hProps, klav_propid_t id, int64_t *pVal) KP_GET_PROPVAL(l)
extern "C" KLAV_ERR KLAV_CALL KP_GetULongProperty(void* hProps, klav_propid_t id, uint64_t *pVal) KP_GET_PROPVAL(ul)
extern "C" KLAV_ERR KLAV_CALL KP_GetStrProperty(void* hProps, klav_propid_t id, char **pVal) KP_GET_PROPVAL(s)
extern "C" KLAV_ERR KLAV_CALL KP_GetHandleProperty(void* hProps, klav_propid_t id, void **pVal) KP_GET_PROPVAL(p)
#undef KP_GET_PROPVAL

#define SET_KLAV_PROP(pv, id, pVal)	klav_propval_t pv; \
	switch(KLAV_PROPID_TYPE(id))\
	{\
	case KLAV_PROP_TYPE_INT: \
		pv.i = *(int32_t*)pVal;\
		break;\
	case KLAV_PROP_TYPE_UINT:\
		pv.ui = *(uint32_t*)pVal;\
		break;\
	case KLAV_PROP_TYPE_LONG:\
		pv.l = *(int64_t*)pVal;\
		break;\
	case KLAV_PROP_TYPE_ULONG:\
		pv.ul = *(uint64_t*)pVal;\
		break;\
	case KLAV_PROP_TYPE_STR:\
		pv.s = (char*)pVal;\
		break;\
	case KLAV_PROP_TYPE_TIME:\
		break;\
	case KLAV_PROP_TYPE_PTR:\
		pv.p = pVal;\
	default: ;\
	}

// returns KLAV_ENOTFOUND if the property is missing, KLAV_EINVAL if property value is invalid
extern "C" KLAV_ERR KLAV_CALL KP_SetProperty(void* hProps, klav_propid_t id, void* pVal)
{ 
	CHECK_HANDLE(hProps) 
	SET_KLAV_PROP(pv, id, pVal)
	return static_cast<KLAV_Properties*>(hProps)->set_property(id, &pv); 
}

extern "C" KLAV_ERR KLAV_CALL KP_GetPropertyCount(void* hProps, unsigned int * pCnt)
{
	CHECK_HANDLE(hProps)
	*pCnt = static_cast<KLAV_Properties*>(hProps)->get_property_count();
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL KP_GetPropertyByNo(void* hProps, unsigned int idx, void * pVal, klav_propid_t* pId)
{
	CHECK_HANDLE(hProps)
	klav_propval_t pt; pt = pVal;
	*pId = static_cast<KLAV_Properties*>(hProps)->get_property_by_no(idx, &pt);
	return KLAV_OK;
}

// KLAV PropertyBag
extern "C" KLAV_ERR KLAV_CALL KPB_SetProperties(void* hDstPropBag, void* hSrcPropBag)
{
	CHECK_HANDLE(hDstPropBag)
	CHECK_HANDLE(hSrcPropBag)
	return static_cast<KLAV_Property_Bag*>(hDstPropBag)->set_properties(static_cast<KLAV_Property_Bag*>(hSrcPropBag));
}

extern "C" KLAV_ERR KLAV_CALL KPB_ClearProperties(void* hPropBag)
{
	CHECK_HANDLE(hPropBag)
	static_cast<KLAV_Property_Bag*>(hPropBag)->clear_properties();
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL KPB_FreeHandle(void* hPropBag)
{
	CHECK_HANDLE(hPropBag)
	static_cast<KLAV_Property_Bag*>(hPropBag)->destroy();
	return KLAV_OK;
}

// KLAV Engine 
extern "C" KLAV_ERR KLAV_CALL KE_FreeHandle(void* hEngine)
{
	CHECK_HANDLE(hEngine)
	static_cast<KLAV_Engine*>(hEngine)->destroy();
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL KE_GetPropertiesHandle(void *hEngine, void **hProps)
{
	CHECK_HANDLE(hEngine)
	*hProps = (void*)static_cast<KLAV_Engine*>(hEngine)->get_properties();
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL KE_CreatePropertyBagHandle(void *hEngine, void **hPropBag)
{
	CHECK_HANDLE(hEngine)
	*hPropBag = (void*)static_cast<KLAV_Engine*>(hEngine)->create_property_bag();
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL KE_SetIface(void *hEngine, uint32_t ifc_id, void *ifc)
{
	CHECK_HANDLE(hEngine)
	return static_cast<KLAV_Engine*>(hEngine)->set_iface(ifc_id, ifc);	
}

extern "C" KLAV_ERR KLAV_CALL KE_GetIface(void *hEngine, uint32_t ifc_id, void **ifc)
{
	CHECK_HANDLE(hEngine)
	*ifc = (void*)static_cast<KLAV_Engine*>(hEngine)->get_iface(ifc_id);	
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL KE_Load(void *hEngine, void *hProp)
{
	CHECK_HANDLE(hEngine)
	return static_cast<KLAV_Engine*>(hEngine)->load((KLAV_Properties*)hProp);	
}

extern "C" KLAV_ERR KLAV_CALL KE_Unload(void *hEngine)
{
	CHECK_HANDLE(hEngine)
	return static_cast<KLAV_Engine*>(hEngine)->unload();
}

extern "C" KLAV_ERR KLAV_CALL KE_GetGlobalContext(void *hEngine, KLAV_CONTEXT_TYPE *pContext)
{
	CHECK_HANDLE(hEngine)
	*pContext = static_cast<KLAV_Engine*>(hEngine)->get_global_context();
	return KLAV_OK;
}

extern "C" KLAV_ERR KLAV_CALL KE_CreateContext(void *hEngine, void *hProps,	KLAV_CONTEXT_TYPE *pContext)
{
	CHECK_HANDLE(hEngine)
	return static_cast<KLAV_Engine*>(hEngine)->create_context((KLAV_Properties*)hProps, pContext);
}

extern "C" KLAV_ERR KLAV_CALL KE_DestroyContext(void *hEngine, KLAV_CONTEXT_TYPE Context)
{
	CHECK_HANDLE(hEngine)
	return static_cast<KLAV_Engine*>(hEngine)->destroy_context(Context);
}

extern "C" KLAV_ERR KLAV_CALL KE_ActivateContext(void *hEngine, KLAV_CONTEXT_TYPE Context, void *object, void *hProps)
{
	CHECK_HANDLE(hEngine)
	return static_cast<KLAV_Engine*>(hEngine)->activate_context(Context, object, (KLAV_Properties*)hProps);
}

extern "C" KLAV_ERR KLAV_CALL KE_DeactivateContext(void* hEngine, KLAV_CONTEXT_TYPE Context)
{
	CHECK_HANDLE(hEngine)
	return static_cast<KLAV_Engine*>(hEngine)->deactivate_context(Context);
}
	
extern "C" KLAV_ERR KLAV_CALL KE_FlushContext(void* hEngine, KLAV_CONTEXT_TYPE Context)
{
	CHECK_HANDLE(hEngine)
	return static_cast<KLAV_Engine*>(hEngine)->flush_context(Context);
}

extern "C" KLAV_ERR KLAV_CALL KE_ProcessAction(void* hEngine, KLAV_CONTEXT_TYPE Context, const char *action_name, void *action_object,	void *hProps)
{
	CHECK_HANDLE(hEngine)
		return static_cast<KLAV_Engine*>(hEngine)->process_action(Context, action_name, action_object, (KLAV_Properties*)hProps);
}

#define KP_GET_CONT_PROPVAL(_val) { CHECK_HANDLE(hEngine) \
	klav_propval_t Val;\
    KLAV_ERR err = static_cast<KLAV_Engine*>(hEngine)->get_context_property(Context, prop_id, index, &Val);\
	*pVal = Val._val;\
	return err; }
extern "C" KLAV_ERR KLAV_CALL KE_GetContextIntProperty(void* hEngine, KLAV_CONTEXT_TYPE Context, klav_propid_t prop_id, uint32_t index, int32_t *pVal) KP_GET_CONT_PROPVAL(i)
extern "C" KLAV_ERR KLAV_CALL KE_GetContextUIntProperty(void* hEngine, KLAV_CONTEXT_TYPE Context, klav_propid_t prop_id, uint32_t index, uint32_t *pVal) KP_GET_CONT_PROPVAL(ui)
extern "C" KLAV_ERR KLAV_CALL KE_GetContextLongProperty(void* hEngine, KLAV_CONTEXT_TYPE Context, klav_propid_t prop_id, uint32_t index, int64_t *pVal) KP_GET_CONT_PROPVAL(l)
extern "C" KLAV_ERR KLAV_CALL KE_GetContextULongProperty(void* hEngine, KLAV_CONTEXT_TYPE Context, klav_propid_t prop_id, uint32_t index, uint64_t *pVal) KP_GET_CONT_PROPVAL(ul)
extern "C" KLAV_ERR KLAV_CALL KE_GetContextStrProperty(void* hEngine, KLAV_CONTEXT_TYPE Context, klav_propid_t prop_id, uint32_t index, char **pVal) KP_GET_CONT_PROPVAL(s)
extern "C" KLAV_ERR KLAV_CALL KE_GetContextHandleProperty(void* hEngine, KLAV_CONTEXT_TYPE Context, klav_propid_t prop_id, uint32_t index, void **pVal) KP_GET_CONT_PROPVAL(p)
#undef KP_GET_CONT_PROPVAL

extern "C" KLAV_ERR KLAV_CALL KE_SetContextProperty(void* hEngine, KLAV_CONTEXT_TYPE Context, klav_propid_t prop_id, uint32_t index, void *val)
{
	CHECK_HANDLE(hEngine) 
	SET_KLAV_PROP(pv, prop_id, val)
	return static_cast<KLAV_Engine*>(hEngine)->set_context_property(Context, prop_id, index, &pv);
}

#endif // defined KL_PLATFORM_WINDOWS