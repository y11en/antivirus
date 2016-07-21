// -------------------------------------------

#include "avs.h"

#define KLAV_PRAGUE 1
#include <klava/klav_prague.h>
#include <klava/klaveng.h>
#include <klava/klavsys.h>
#include <klava/klav_dbg.h>
#include <klava/stream_packet.h>

#include <klava/klavstl/vector.h>
#include <klava/klavstl/buffer.h>
#include <Extract/plugin/p_btimages.h>
#include <Prague/plugin/p_vmarea.h>

struct tStreamCtx : public KLAV_Stream_Packet
{
	tStreamCtx() : KLAV_Stream_Packet(NULL, 0, 0, 0), m_ctx(NULL){}

	KLAV_CONTEXT_TYPE m_ctx;
};

struct tICData
{
	tDWORD    dwAVBTimeFirstCheck; //time rounded to minutes
	tDWORD    dwAVBTimeLastCheck;  //time rounded to minutes
	tQWORD    qwLevel;
};

struct tReopenData
{
	tDWORD    size;
	tPROPID   propid;
	tIID      iid;
	tPID      pid;
	tCODEPAGE cp;
	tBYTE     data[1];
};

struct tMemAlloc
{
	tMemAlloc(){}

	cObj*  m_obj;
	tPTR   m_ptr;
	tBOOL  m_lock;
	tDWORD m_size;
	tDWORD m_timestamp;
};

#define IO_CACHE_SIZE         128
#define IO_CACHE_ROTATE_SIZE  0x4000
#define IO_CACHE_CHUNK_SIZE   0x00010000

struct CCachedBuff
{
	klav_filepos_t m_from;
	uint32_t       m_size;
	uint8_t*       m_data;
};

extern void InitBootProps(cObj* obj, KLAV_IO_Object* pObj, KLAV_IO_Object*& pTmp);
extern void InitMbrProps (cObj* obj, KLAV_IO_Object* pObj, KLAV_IO_Object*& pTmp);

// -------------------------------------------

class CKLAVEngine :
	public CAVSEngine,
	public KLAV_IFACE_IMPL(KLAV_DSO_Loader),
	public KLAV_IFACE_IMPL(KLAV_Callback_Handler),
	public KLAV_IFACE_IMPL(KLAV_Message_Printer),
	public KLAV_IFACE_IMPL(KLAV_Object_System_Manager),
	public KLAV_IFACE_IMPL(KLAV_Yield_Handler),
	public KLAV_IFACE_IMPL(KLAV_Temp_Object_Manager)
{
public:
	CKLAVEngine ();

	virtual tERROR Load(cAVSUpdateInfo* pInfo);
	virtual tERROR Init();
	virtual tERROR InitDbgAPI();
	virtual tERROR Unload();

	virtual tERROR GetBasesInfo(cAVP3BasesInfo& pInfo);

	virtual tERROR ICGetData(cObj* pObj, tPID pPid, tPTR& pICdata);
	virtual tERROR ICCheckData(tPTR pICdata, tQWORD pBitData, tDWORD pMandatoryScanDays);
	virtual tERROR ICUpdate(cObj* pObj, tPID pPid, tPTR pICdata, tQWORD pBitData, tBOOL pDeleteStatus);
	virtual tERROR ICFree(tPTR& pICdata);

	virtual tDWORD InitClassMask(cScanSettings& pSettings);
	virtual tERROR InitContext(CObjectContext& pCtx);
	virtual tERROR FreeContext(CObjectContext& pCtx);
	virtual tERROR InitContextProps(CObjectContext& pCtx, KLAV_Properties* io_props, KLAV_IO_Object*& pTmp);

	virtual tERROR ProcessObject(CObjectContext& pCtx);
	virtual tERROR ProcessStream(CObjectContext& pCtx, enChunkType eType, tPTR pChunk, tDWORD nSize);

	virtual tERROR ProcessThreats(CObjectContext& pCtx);
	virtual tERROR ProcessThreatTail(CObjectContext* pCtx);

	virtual tERROR SetWriteAccess(CObjectContext& pCtx, cObj*& pDeletable);
	virtual tERROR AddReopenData(cIO* pIO, tPTR pData, tDWORD nPos, tDWORD pUserData);

	virtual tERROR UpdateBTImages(CObjectContext& pCtx, bool bSave);

	virtual tERROR CheckEIP(tPTR eip);

	virtual enDetectStatus GetDetectStatus(CObjectContext& pCtx);
	virtual enDetectType   GetDetectType(CObjectContext& pCtx);
	virtual enDetectDanger GetDetectDanger(CObjectContext& pCtx);
	virtual tERROR GetDetectName(CObjectContext& pCtx, cStringObj& strName);
	virtual tERROR GetPakerName(CObjectContext& pCtx, cStringObj& strName);
	virtual tERROR GetReopenData(CObjectContext& pCtx, cCharVector& pData);
	virtual bool   GetDisinfectability(CObjectContext& pCtx);
	virtual tDWORD GetDetectBehaviour(CObjectContext& pCtx);
	virtual bool   IsShouldBeScan(CObjectContext& pCtx);

public:
	CObjectContext* get_ctx(KLAV_CONTEXT_TYPE);

	virtual KLAV_ERR KLAV_CALL yield (KLAV_CONTEXT_TYPE, uint32_t);
	virtual uint32_t KLAV_CALL handle_callback(KLAV_CONTEXT_TYPE context, KLAV_CALLBACK_CODE code, KLAV_Properties* props);
	virtual uint32_t KLAV_CALL print_message_v(KLAV_CONTEXT_TYPE context, uint32_t level, const char *fmt, va_list ap);
	virtual KLAV_ERR KLAV_CALL process_subobject(KLAV_CONTEXT_TYPE parent_context, void* object, KLAV_Properties* props);
	virtual KLAV_ERR KLAV_CALL process_get_parent(CObjectContext& pCtx, KLAV_Properties* props);
	virtual KLAV_ERR KLAV_CALL create_temp_object(KLAV_CONTEXT_TYPE context, KLAV_Properties * props, KLAV_IO_Object **ptempobj);

// KLAV_DSO_Loader api
	virtual KLAV_ERR KLAV_CALL dl_open  (const char *path, void **phdl);
	virtual void *   KLAV_CALL dl_sym   (void *hdl, const char *name);
	virtual void     KLAV_CALL dl_close (void *hdl);

public:
	struct Allocator : public KLAV_IFACE_IMPL(KLAV_Alloc)
	{
		Allocator(CKLAVEngine* hEngine, cObj* obj) : m_hEngine(hEngine), m_hObject(obj){}

		virtual uint8_t * KLAV_CALL alloc (size_t size);
		virtual uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize);
		virtual void KLAV_CALL free (void *ptr);
	protected:
		CKLAVEngine* m_hEngine;
		cObj*        m_hObject;
	};

	struct CachedIO : public KLAV_IFACE_IMPL(KLAV_IO_Object), public Allocator
	{
		CachedIO(CKLAVEngine* hEngine, cObj* obj, cIO* io) :
			Allocator(hEngine, obj), m_hio(io), m_inited(obj == (cObj*)io), m_ctx(0)
		{
			m_props = KLAV_Property_Bag_Create(this);
			m_ownio = obj != (cObj*)io;
			m_rwsize = 0;
		}

		~CachedIO()
		{
			clear_cache();
			if( m_ownio )
				m_hio->sysCloseObject();
		}

		void init(KLAV_CONTEXT_TYPE ctx)
		{
//			m_inited = true;
			m_ctx = ctx;
		}

		inline KLAV_ERR rotate()
		{
			m_rwsize %= IO_CACHE_ROTATE_SIZE;
			return m_ctx ? m_hEngine->yield(m_ctx, 0) : KLAV_OK;
		}

		void clear_cache()
		{
			for(CCachedBuff* pd = m_cache; pd->m_data; pd++)
				free(pd->m_data), pd->m_data = NULL;
			m_inited = false;
		}

		virtual uint32_t KLAV_CALL  get_io_version(){ return PID_AVS; }

		virtual void *   KLAV_CALL  get_io_iface(int ifc_id)
		{
			return ((ifc_id == KLAV_IFIO_PR_HOBJECT) || (ifc_id == KLAV_IFIO_PR_HIO)) ? m_hio : NULL;
		}

		virtual KLAV_ERR KLAV_CALL  seek_read(klav_filepos_t pos, void *buf, uint32_t size, uint32_t *pnrd)
		{
			if( (m_rwsize += size) > IO_CACHE_ROTATE_SIZE )
				if( KLAV_FAIL(rotate()) )
					return KLAV_EREAD;

			if( !m_inited )
				return m_hio->SeekRead(pnrd, pos, buf, size);

			uint32_t result = read_cache(pos, buf, size);
			if( pnrd) *pnrd = result;
			return result < size ? errEOF : errOK;
		}

		virtual KLAV_ERR KLAV_CALL  seek_write(klav_filepos_t pos, const void *buf, uint32_t size, uint32_t *pnwr)
		{
			if( (m_rwsize += size) > IO_CACHE_ROTATE_SIZE )
				if( KLAV_FAIL(rotate()) )
					return KLAV_EWRITE;

			if( m_inited ) clear_cache();
			return KLAV_PR_ERROR(m_hio->SeekWrite(pnwr, pos, (tPTR)buf, size));
		}

		virtual KLAV_ERR KLAV_CALL  get_size(klav_filepos_t *psize)
		{
			return KLAV_PR_ERROR(m_hio->GetSize(psize, IO_SIZE_TYPE_EXPLICIT));
		}

		virtual KLAV_ERR KLAV_CALL  set_size(klav_filepos_t size)
		{
			if( m_inited ) clear_cache();
			return KLAV_PR_ERROR(m_hio->SetSize(size));
		}

		virtual KLAV_ERR KLAV_CALL  flush()
		{
			return KLAV_PR_ERROR(m_hio->Flush());
		}

		virtual KLAV_ERR KLAV_CALL  ioctl(uint32_t code, void *buf, size_t bufsize){ return KLAV_ENOIMPL; }
		virtual KLAV_ERR KLAV_CALL  query_map(klav_filepos_t pos, uint32_t size, uint32_t * pflags){ return KLAV_ENOIMPL; }
		virtual KLAV_ERR KLAV_CALL  map_data(klav_filepos_t pos, uint32_t size, uint32_t flags, klav_iomap_t * iomap){ return KLAV_ENOIMPL; }
		virtual KLAV_ERR KLAV_CALL  unmap_data(klav_iomap_t * iomap){ return KLAV_ENOIMPL; }
		virtual KLAV_ERR KLAV_CALL  close(){ return KLAV_ENOIMPL; }
		virtual hKLAV_Properties KLAV_CALL get_properties(){ return m_props; }
		virtual void KL_CDECL       destroy(){ KLAV_DELETE(this, this); }

	private:
		uint32_t read_cache(klav_filepos_t pos, void * buff, uint32_t size);

		KLAV_Property_Bag_Holder m_props;
		CCachedBuff              m_cache[IO_CACHE_SIZE+1];
		cIO*                     m_hio;
		tDWORD                   m_rwsize;
		bool                     m_inited;
		bool                     m_ownio;
		KLAV_CONTEXT_TYPE        m_ctx;
	};

	uint8_t* MemCapture(cObj* obj, size_t size);
	void     MemFree(tMemAlloc& mem);
	bool     MemRelease(void *ptr);
	void     MemDestroyCtx(cObj* obj);

	bool     IsFileUpdated(const char* name);
	tERROR   ProcessAction(KLAV_CONTEXT_TYPE kcontext, const char *action_name, void *action_object);

private:
	hKLAV_Engine     m_hEngine;
	tDWORD           m_tmBasesDate;
	tDWORD           m_nNumRecords;
	void *           m_engine_hdll;
	PrHMODULE	     m_klavdbg_dll;
	KLAV_Debug_API * m_klavdbg_api;
	cAVSUpdateInfo*  m_pCurInfo;
	tBOOL            m_bSafeScan;
	tDWORD           m_nPageSize;

	tPTR             m_pBaseAddress;
	tDWORD           m_pCodeSize;

	KLAV_Pr_Alloc              m_allocator;
	KLAV_Pr_Sync_Factory       m_sync_factory;
	KLAV_Pr_Virtual_Memory     m_vm;
	hKLAV_IO_Library           m_io_library;
	KLAV_Pr_Proc_Resolver      m_ppl_proc_resolver;
	hKLAV_Proc_Resolver        m_dso_proc_resolver;
	KLAV_Pr_KFB_Loader         m_db_loader;
	KLAV_Debug_Proc_Resolver * m_dbg_proc_resolver;
	KLAV_DB_Manager *          m_db_manager;

	cVector<tMemAlloc>         m_vMems;
	cCriticalSection*          m_lock;

	PrHMODULE                  m_hmodAdvApi;
	PrHMODULE                  m_hmodKernel;
};

// -------------------------------------------

class CKLAVEngineHnd : public CAVSEngineHnd
{
public:
	CKLAVEngineHnd(AVSImpl* pAvs){}

	CAVSEngine* CreateEngine(enEngineType eType){ return new CKLAVEngine(); }
};

// -------------------------------------------

CAVSEngineHnd* CreateEngineHnd_KLAV(AVSImpl* pAvs)
{
	return new CKLAVEngineHnd(pAvs);
}
// -------------------------------------------


CKLAVEngine::CKLAVEngine() :
		m_hEngine (0),
		m_tmBasesDate(0),
		m_nNumRecords(0),
		m_pCurInfo(NULL),
		m_engine_hdll(0),
		m_klavdbg_dll(0),
		m_klavdbg_api(0),
		m_allocator((hOBJECT)g_root), 
		m_sync_factory((hOBJECT)g_root, &m_allocator),
		m_io_library(0),
		m_dso_proc_resolver(0),
		m_dbg_proc_resolver(0),
		m_db_manager(0),
		m_ppl_proc_resolver(&m_allocator, PID_AVS, 0),
		m_bSafeScan(cTRUE),
		m_pBaseAddress(0),
        m_pCodeSize(0)
{
	m_nPageSize = vm_pagesize();
}

tERROR CKLAVEngine::Load(cAVSUpdateInfo* pInfo)
{
	cStrObj sBaseName;
	bool use_old_kfb_loader = true;

	if( m_engtype == ENGINE_EMUL )
		sBaseName = m_avs->m_settings.m_sEmulBase;
	else if( m_engtype == ENGINE_STREAM )
		sBaseName = m_avs->m_settings.m_sStreamBase;
	else
	{
		sBaseName = m_avs->m_settings.m_sKlavaBase;
		use_old_kfb_loader = false;
	}

	if( sBaseName.empty() )
		return errOK;

	cStrObj sDllName(sBaseName);

	tDWORD pos = sBaseName.find_first_of(L":");
	if( pos != cStrObj::npos )
	{
		sDllName.assign(sBaseName, 0, pos);
		sBaseName.erase(0, pos+1);
	}

	KLAV_IO_Library_Create_Parms io_parms;
	memset (& io_parms, 0, sizeof (io_parms));

	cStringObj sProductDir = "%ProductRoot%";
	klav_string product_dir;
	if (errOK_NO_DECIDERS != m_avs->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING,
					(hOBJECT)cAutoString(sProductDir), 0, 0))
	{
		product_dir.assign((const wchar_t *) sProductDir.c_str (cCP_UNICODE), sProductDir.length(), & m_allocator);
		io_parms.iolib_product_dir = product_dir.c_str ();
	}

	klav_string base_dir ((const wchar_t *) m_avs->m_settings.m_sBaseFolder.c_str (cCP_UNICODE), & m_allocator);
	io_parms.iolib_base_dir = base_dir.c_str ();

	m_io_library = KLAV_Pr_IO_Library_Create((hOBJECT)m_avs, & io_parms);
	if( !m_io_library )
		return errUNEXPECTED;

	m_db_loader.init(&m_allocator, &m_vm);
	m_dso_proc_resolver = KLAV_Create_DSO_Proc_Resolver(&m_allocator, this, 0);

	if( m_dso_proc_resolver )
		m_ppl_proc_resolver.set_ext_resolver(m_dso_proc_resolver);

	KLAV_DBM_Init_Parms dbm_init_parms;
	dbm_init_parms.struct_size = sizeof (dbm_init_parms);
	dbm_init_parms.allocator = & m_allocator;
	dbm_init_parms.tracer = this;
	dbm_init_parms.io_library = m_io_library;
	
	KLAV_ERR kerr = KLAV_DBM_Create (& dbm_init_parms, & m_db_manager);
	if (KLAV_FAIL (kerr))
		return errUNEXPECTED;

	klav_string basename_utf ((wchar_t *) m_avs->m_settings.m_sBaseFolder.c_str (cCP_UNICODE), & m_allocator);
	kerr = KLAV_DBM_Open (m_db_manager, basename_utf.c_str ());
	if (KLAV_FAIL (kerr))
		return errOBJECT_NOT_FOUND;

	m_pCurInfo = pInfo;

	sDllName += ".kdl";
	dl_open(sDllName.c_str(cCP_ANSI), &m_engine_hdll);

	if( !m_engine_hdll )
	{
		PR_TRACE((g_root, prtERROR, "avs\tCKLAVEngine: load of %S failed", sDllName.data()));
		m_pCurInfo = NULL;
		return errMODULE_CANNOT_BE_LOADED;
	}

	PFN_KLAV_Engine_Create pfn_creator =(PFN_KLAV_Engine_Create)
		 dl_sym(m_engine_hdll, KLAV_ENGINE_CREATE_NAME);

	if( pfn_creator == 0)
	{
		PR_TRACE((g_root, prtERROR, "avs\tCKLAVEngine: PFN_KLAV_Engine_Create not found"));
		m_pCurInfo = NULL;
		return errMODULE_IS_BAD;
	}

	hKLAV_Engine engine = NULL;
	if( KLAV_FAILED(pfn_creator(KLAV_ENGINE_VERSION_CURRENT, &m_allocator, &engine)))
	{
		PR_TRACE((g_root, prtERROR, "avs\tCKLAVEngine: PFN_KLAV_Engine_Create call failed"));
		m_pCurInfo = NULL;
		return errUNEXPECTED;
	}

	KLAV_SET_IFACE(engine, Sync_Factory, &m_sync_factory);
	KLAV_SET_IFACE(engine, Virtual_Memory, &m_vm);
	KLAV_SET_IFACE(engine, DSO_Loader, (KLAV_DSO_Loader*)this);
	KLAV_SET_IFACE(engine, IO_Library, m_io_library);
	KLAV_SET_IFACE(engine, Temp_Object_Manager, (KLAV_Temp_Object_Manager*)this);
	KLAV_SET_IFACE(engine, Proc_Resolver, &m_ppl_proc_resolver);
	KLAV_SET_IFACE(engine, DB_Manager, m_db_manager);

	if (use_old_kfb_loader)
		KLAV_SET_IFACE(engine, DB_Loader, &m_db_loader);

	KLAV_SET_IFACE(engine, Callback_Handler, this);
	KLAV_SET_IFACE(engine, Message_Printer,  this);
	KLAV_SET_IFACE(engine, Object_System_Manager, this);
	KLAV_SET_IFACE(engine, Yield_Handler, this);

	KLAV_STATIC_PROPERTIES (props);

	cStrObj sPath(m_avs->m_settings.m_sBaseFolder);
	sPath.check_last_slash();
	sPath += sBaseName;

	if( !m_dbg_proc_resolver && m_avs->m_settings.m_bDebugMode)
	{
		if (PR_SUCC (InitDbgAPI()))
		{
			cStrObj strDebugDll(sBaseName), strDebugPls(sPath);
			strDebugDll += L".dll";
			strDebugPls += L".pls";

			m_dbg_proc_resolver = m_klavdbg_api->create_dbg_proc_resolver(
				this, m_io_library, this, strDebugDll.c_str(cCP_ANSI), strDebugPls.c_str(cCP_ANSI));
		}
	}

	if( m_dbg_proc_resolver )
		KLAV_SET_IFACE(engine, Debug_Proc_Resolver, m_dbg_proc_resolver);

	if( use_old_kfb_loader )
	{
		sBaseName += ".kfb";
		if( IsFileUpdated(sBaseName.c_str(cCP_ANSI)) )
			sPath += m_pCurInfo->m_sSuffix;

		sPath += ".kfb";
	}

	PR_TRACE((g_root, prtIMPORTANT, "avs\tCKLAVEngine: loading base (%S) ...", sPath.data()));

	cAutoObj<cHeap> pCtxObj;
	KLAV_ERR err = m_avs->sysCreateObjectQuick(pCtxObj, IID_HEAP, PID_LOADER);

	if( KLAV_SUCC(err) && use_old_kfb_loader)
	{
		pCtxObj->propSetObj(pgOBJECT_HEAP, pCtxObj);
		err = m_db_loader.open(pCtxObj, sPath.c_str(cCP_ANSI));
	}

	if( KLAV_SUCC(err) )
		err = pCtxObj->sysCreateObjectQuick((cObj**)&m_lock, IID_CRITICAL_SECTION);

	klav_string db_name ((const wchar_t *) sBaseName.c_str (cCP_UNICODE), & m_allocator);
	KLAV_STATIC_PROPERTY (props, KLAV_PROPID_DB_NAME, db_name.c_str ());
	KLAV_STATIC_PROPERTY (props, KLAV_PROPID_DB_FLAGS, 0);

	if( KLAV_SUCC(err) )
		err = engine->load(&props);

	if( KLAV_FAILED(err) )
	{
		m_db_loader.close();
		engine->destroy();
		PR_TRACE((g_root, prtERROR, "avs\tCKLAVEngine: load base (%S) failed (%terr)...", sPath.data(), err));
		m_pCurInfo = NULL;
		return errUNEXPECTED;
	}

	m_hCtxObj = *pCtxObj.relinquish();
	m_hEngine = engine;

	cDateTime dt(cDateTime::zero);

	klav_propval_t prop;
	if( KLAV_SUCC(engine->get_context_property(NULL, KLAV_PROPID_DB_TIMESTAMP, 0, &prop)) )
		dt = (time_t)prop.ul;
	else
		dt.SetUTC(2000, 1, 1, 0, 0, 0, 0);
	m_tmBasesDate = dt;

	if( KLAV_SUCC(engine->get_context_property(NULL, KLAV_PROPID_DB_RECORD_COUNT, 0, &prop)) )
		m_nNumRecords = prop.ui;
	else
		m_nNumRecords = 0;

	if( KLAV_SUCC(engine->get_context_property(NULL, KLAV_PROPID_ENGINE_KERNEL_INFO, 0, &prop)) )
	{
		KLAV_Kernel_Info* info = (KLAV_Kernel_Info*)prop.p;
		m_pBaseAddress = info->db_code_addr;
		m_pCodeSize = info->db_code_size;
	}
	
	PR_TRACE((g_root, prtERROR, "avs\tCKLAVEngine: load base (%S) done", sPath.data()));
	m_pCurInfo = NULL;
	return errOK;
}

bool CKLAVEngine::IsFileUpdated(const char* name)
{
	if( !m_pCurInfo )
		return false;

	tDWORD i, c = m_pCurInfo->m_aFiles.size();
	for(i = 0; i < c ; i++)
	{
		cStrObj& file = m_pCurInfo->m_aFiles[i];
		if( !file.compare(name, fSTRING_COMPARE_CASE_INSENSITIVE) )
			return true;
	}
	return false;
}

tERROR CKLAVEngine::Init()
{
	if( m_iChecker )
	{
		m_iChecker->propSetDWord(plDB_USER_DATA_SIZE, 4*sizeof(tDWORD));
		m_iChecker->propSetDWord(plDB_USER_VERSION, 0x00000003);
		tERROR error = m_iChecker->sysCreateObjectDone();
			
		if( PR_FAIL(error) )
			PR_TRACE((m_avs, prtIMPORTANT, "InitIChecker: CreateDone failed %terr", error));
		else
			error = m_iChecker->FlushStatusDB();
		
		if( PR_FAIL(error) )
			m_iChecker->sysCloseObject(), m_iChecker = NULL;
	}

	m_bSafeScan = m_avs->m_settings.m_bSafeScan;

	cStrObj sSafeScan = "%SafeScan%";
	m_avs->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING,
		(hOBJECT)cAutoString(sSafeScan), 0, 0);

	if( sSafeScan[0] == '0' )
		m_bSafeScan = cFALSE;

	return errOK;
}

tERROR CKLAVEngine::InitDbgAPI()
{
	KLAV_ERR err = KLAV_OK;

	if( m_klavdbg_api != 0)
		return errOK;

	cStrObj sDllName(KLAV_DSO_PREFIX_NAME ("klavdbg"));
	m_klavdbg_dll = PrLoadLibrary(sDllName.data(), cCP_UNICODE);
	if( m_klavdbg_dll == 0 )
	{
		PR_TRACE((g_root, prtERROR, "avs\tCKLAVEngine: error loading debug API DLL: (%S)", sDllName.data()));
		return KLAV_ENOTFOUND;
	}

	PFN_KLAV_Debug_API_Create pfn = NULL;
	if( m_klavdbg_dll )
		pfn = (PFN_KLAV_Debug_API_Create) PrGetProcAddress (m_klavdbg_dll, KLAV_DEBUG_API_CREATE_NAME);

	if( pfn )
		err = pfn(&m_allocator, &m_klavdbg_api);

	if( KLAV_SUCC (err) && m_klavdbg_api )
		return errOK;

	PrFreeLibrary (m_klavdbg_dll);
	m_klavdbg_dll = 0;

	return errNOT_OK;
}

tERROR CKLAVEngine::Unload()
{
	if( m_hEngine )
		m_hEngine->destroy(), m_hEngine = NULL;
	
	if( m_engine_hdll )
		dl_close(m_engine_hdll), m_engine_hdll = NULL;

	m_db_loader.close();

	size_t i, n = m_vMems.size();
	for(i = 0; i < n; i++)
		MemFree(m_vMems[i]);
	m_vMems.clear();

	if( m_hCtxObj )
		m_hCtxObj->sysCloseObject(), m_hCtxObj = NULL;

	if (m_db_manager != 0)
	{
		KLAV_DBM_Destroy (m_db_manager);
		m_db_manager = 0;
	}

	if( m_dso_proc_resolver )
		KLAV_Destroy_DSO_Proc_Resolver(m_dso_proc_resolver);

	m_ppl_proc_resolver.unload();

	if( m_io_library )
		KLAV_Pr_IO_Library_Destroy (m_io_library);

	if( m_dbg_proc_resolver )
		m_klavdbg_api->destroy_dbg_proc_resolver(m_dbg_proc_resolver), m_dbg_proc_resolver = NULL;

	if( m_klavdbg_api )
		m_klavdbg_api->destroy(), m_klavdbg_api = NULL;

	if( m_klavdbg_dll )
		PrFreeLibrary (m_klavdbg_dll), m_klavdbg_dll = NULL;

	return errOK;
}

tERROR CKLAVEngine::GetBasesInfo(cAVP3BasesInfo& pInfo)
{
	memcpy(&pInfo.m_dtBasesDate, &cDateTime(m_tmBasesDate), sizeof(tDT));
	pInfo.m_dwViruseCount = m_nNumRecords;
	return errOK;
}

tERROR CKLAVEngine::CheckEIP(tPTR eip)
{
	if( m_pBaseAddress < eip && eip < (tBYTE*)m_pBaseAddress + m_pCodeSize )
		return errOK;
	return errNOT_OK;
}

tERROR CKLAVEngine::ICGetData(cObj* pObj, tPID pPid, tPTR& pICdata)
{
	cIChecker* hChecker = (pPid == PID_ICHECKER2) ? m_iChecker : m_iSwift;
	if( !hChecker )
		return errUNEXPECTED;

	switch( pObj->propGetDWord(pgOBJECT_ORIGIN) )
	{
	case OID_AVP3_BOOT:
	case OID_AVP3_MBR:
	case OID_AVP3_DOS_MEMORY:
		return errOBJECT_NOT_FOUND;
	}

	tICData* icdata = NULL;
	tERROR error = pObj->heapAlloc((tPTR*)&icdata, sizeof(tICData));

	if( PR_SUCC(error) )
		error = hChecker->GetStatus(0, (hIO)pObj, NULL, 0, (tDWORD*)icdata, sizeof(tICData));

	if( PR_SUCC(error) )
	{
		pICdata = (tPTR)icdata;
		if( icdata->dwAVBTimeLastCheck < icdata->dwAVBTimeFirstCheck )
			icdata->dwAVBTimeFirstCheck = icdata->dwAVBTimeLastCheck;
	}
	else if( icdata )
		pObj->heapFree(icdata);

	return error;
}

tERROR CKLAVEngine::ICCheckData(tPTR pICdata, tQWORD pBitData, tDWORD pMandatoryScanDays)
{
	tICData* icdata = (tICData*)pICdata;
	
	if( !icdata )
	{
		PR_TRACE((m_avs, prtNOTIFY, "AVPMGR\tEngine_ICCheckData errPARAMETER_INVALID (NULL)"));
		return errPARAMETER_INVALID;
	}

	if(	!icdata->dwAVBTimeLastCheck || (pBitData & ~icdata->qwLevel) )
	{
		PR_TRACE((m_avs, prtNOTIFY, "AVPMGR\tEngine_ICCheckData errNOT_OK %08X %I64X %I64X", icdata->dwAVBTimeLastCheck, pBitData, icdata->qwLevel));
		return errNOT_OK;
	}

	tDWORD tMandatoryScanPeriod = 24*60*pMandatoryScanDays; //in minutes
	tDWORD tLastScanDBTime = icdata->dwAVBTimeLastCheck;
	tDWORD tFirstScanDBTime = icdata->dwAVBTimeFirstCheck;
	tDWORD tSecurePeriod = tLastScanDBTime - tFirstScanDBTime; // period of time(minutes) file was scanned and treated as clean
	tDWORD tBaseTime = m_tmBasesDate / 60;

	if( !tMandatoryScanPeriod )
	{
		if( !tLastScanDBTime )
			return errNOT_OK;

		if( tSecurePeriod < 24*60 )
			return warnFALSE;

		PR_TRACE((m_avs, prtNOTIFY, "AVPMGR\tISTS T(3)" ));
		return errOK; //skip, don't use rescan heuristics for tMandatoryScanPeriod==0
	}

	if( tBaseTime <= tLastScanDBTime )
	{
		PR_TRACE((m_avs, prtNOTIFY, "AVPMGR\tISTS T(1)" ));
		return errOK; //already scaned with tis database
	}

	if( tSecurePeriod < tMandatoryScanPeriod )
	{
		PR_TRACE((m_avs, prtNOTIFY, "AVPMGR\tISTS F(2)" ));
		return errNOT_OK; //should be scanned with each update while in minimum secure period
	}

	// calculate next scan time
	tDWORD tNextScan = tLastScanDBTime + tSecurePeriod/10 + ((tSecurePeriod/10/2)*(tWORD)PrGetTickCount())/0x10000;
	if( tBaseTime > tNextScan )
	{
		PR_TRACE((m_avs, prtNOTIFY, "AVPMGR\tISTS F(4)" ));
		return errNOT_OK; // need to be rescanned
	}

	// don't need to rescan this time
	PR_TRACE((m_avs, prtNOTIFY, "AVPMGR\tISTS T(5)" ));
	return errOK;
}

tERROR CKLAVEngine::ICUpdate(cObj* pObj, tPID pPid, tPTR pICdata, tQWORD pBitData, tBOOL pDeleteStatus)
{
	cIChecker* hChecker = (pPid == PID_ICHECKER2) ? m_iChecker : m_iSwift;
	if( !hChecker )
		return errUNEXPECTED;

	tERROR error = errOK;

	if( pDeleteStatus )
		error = hChecker->DeleteStatus((hIO)pObj);
	else
	{
		tDWORD tBaseTime = m_tmBasesDate / 60;

		tICData* icdata = (tICData*)pICdata;
		icdata->qwLevel = pBitData;
		icdata->dwAVBTimeLastCheck = tBaseTime;
		if( icdata->dwAVBTimeFirstCheck == 0 )
			icdata->dwAVBTimeFirstCheck = tBaseTime;

		if( pBitData & (((tQWORD)1) << 55) )
		{
			icdata->dwAVBTimeFirstCheck = tBaseTime - 60*24*365; // - 1 year
			icdata->qwLevel = (tQWORD)-1;
		}

		error = hChecker->UpdateStatus((hIO)pObj, (tDWORD*)icdata, sizeof(tICData));
	}
	return error;
}

tERROR CKLAVEngine::ICFree(tPTR& pICdata)
{
	m_avs->heapFree(pICdata);
	pICdata = NULL;
	return errOK;
}

tDWORD CKLAVEngine::InitClassMask(cScanSettings& pSettings)
{
	tDWORD nMask = 0;
	switch( pSettings.m_nScanFilter )
	{
	default:
	case SCAN_FILTER_ALL_EXTENSION:  nMask |= KLAV_MF_MASK_PROG; break;
	case SCAN_FILTER_ALL_INFECTABLE: nMask |= KLAV_MF_MASK_SMART; break;
	case SCAN_FILTER_ALL_OBJECTS:    nMask |= KLAV_MF_MASK_ALL; break;
	}

	if( pSettings.m_bScanPacked )		nMask |= KLAV_MF_PACKED;
	if( pSettings.m_bScanArchived )		nMask |= KLAV_MF_ARCHIVED;
	if( pSettings.m_bScanSFXArchived )	nMask |= KLAV_MF_SFX_ARCHIVED;
	if( pSettings.m_bScanMailBases )	nMask |= KLAV_MF_MAILBASES;
	if( pSettings.m_bScanPlainMail )	nMask |= KLAV_MF_MAILPLAIN;
	if( pSettings.m_bScanOLE )			nMask |= KLAV_MF_EMBEDDED;
	if( !pSettings.m_bScanThreats )		nMask |= KLAV_MF_IGNORE_THREATS; 
	if( pSettings.m_bScanVulnerability )nMask |= KLAV_MF_VULNERABILITY;
	return nMask;
}

tERROR CKLAVEngine::InitContext(CObjectContext& pCtx)
{
	if( pCtx.m_fPreProcessing || pCtx.m_fTreatsProcessing || pCtx.m_fCtxInited )
		return errOK;

	pCtx.m_fCtxInited = 1;

	cObj* obj = pCtx.m_pObject;
	if( m_engtype == ENGINE_KLAV )
	{
		if( !pCtx.m_pScanCtx->m_pScanTop )
		{
			cObj* pHeap = NULL;
			obj->sysCreateObjectQuick(&pHeap, IID_HEAP);
			obj->propSetObj(pgOBJECT_HEAP, pHeap);
		}
	}
	else if( obj->propGetDWord(pgPLUGIN_ID) == PID_AVS )
		obj->propSetDWord(CObjectContext::m_propReadonlyError, errOBJECT_WRITE_NOT_SUPPORTED);

	KLAV_IO_Object*& io_object = (KLAV_IO_Object*&)pCtx.m_pIOCtx;
	if( !io_object )
	{
		Allocator alloc(this, obj);
		io_object = KLAV_NEW(&alloc) CachedIO(this, obj, (cIO*)obj);
	}
	else
		return errOK;

	cObj * ioWrap = NULL;
	if( PR_SUCC(obj->sysCreateObject((cObj**)&ioWrap, IID_IO, PID_AVS, 0)) )
	{
		ioWrap->propSetPtr(plKLAV_IO_OBJECT, io_object);
		ioWrap->propSetPtr(plSUBST_IO_OBJECT, obj);
		ioWrap->sysCreateObjectDone();

		pCtx.m_pCached = ioWrap;
	}

	return errOK;
}

tERROR CKLAVEngine::FreeContext(CObjectContext& pCtx)
{
	if( pCtx.m_pCached )
		pCtx.m_pCached->sysCloseObject(), pCtx.m_pCached = NULL;

	if( pCtx.m_pIOCtx )
	{
		if( pCtx.m_fStreamProcessing )
			delete (tStreamCtx*)pCtx.m_pIOCtx;
		else
		{
			((KLAV_IO_Object*)pCtx.m_pIOCtx)->destroy(), pCtx.m_pIOCtx = NULL;
			MemDestroyCtx(pCtx.m_pObject);
		}
	}
	return errOK;
}

tERROR CKLAVEngine::InitContextProps(CObjectContext& pCtx, KLAV_Properties* io_props, KLAV_IO_Object*& pTmp)
{
	KLAV_IO_Object* pObj = (KLAV_IO_Object*)pCtx.m_pIOCtx;

	for(CObjectContext *ctx = pCtx.m_pPrev; ctx; ctx = ctx->m_pPrev)
		if( ctx->m_pIOCtx )
		{
			KLAV_IO_Object* parent_io = (KLAV_IO_Object*)ctx->m_pIOCtx;
			if( parent_io->get_properties() )
				KLAV_Inherit_Properties(parent_io->get_properties(), io_props);
			break;
		}

	if( pCtx.m_pPrev && pCtx.m_pPrev->m_fUnpacked )
		KLAV_Set_Property_Str(io_props, KLAV_PROPID_TRANSFORMER_NAME,
			(tCHAR*)pCtx.m_pPrev->m_sPackerName.c_str(cCP_ANSI));

	cProtectionSettings * Settings = pCtx.m_pScanCtx->m_Settings;
	KLAV_Set_Property_Int(io_props, KLAV_PROPID_HEURISTIC_LEVEL,
		Settings->m_bEmulEnable ? Settings->m_nEmulValue : 0);

	tDWORD flags = 0;
	if( Settings->m_bPartlyDetect )
		flags |= KLAV_MF_PARTLY_DETECT;

	tDWORD packers_flags = m_avs->m_settings.m_nPackerTypeMask;
	if( packers_flags & DETPACKER_SUSPICIOUS )
		flags |= KLAV_MF_SUSPICIOUS_PACKERS;
	if( packers_flags & DETPACKER_MULTIPACKED )
		flags |= KLAV_MF_MULTI_PACKED;
	if( packers_flags & DETPACKER_UNKNOWN )
		flags |= KLAV_MF_UNKNOWN_PACKERS;
	if( packers_flags & DETPACKER_GREY )
		flags |= KLAV_MF_GREY_PACKERS;

	if( m_engtype == ENGINE_KLAV )
	{
		flags |= pCtx.m_pObject->propGetDWord(CObjectContext::m_propActionClassMask);
		flags |= KLAV_MF_CA;
	}

	KLAV_Set_Property_Int(io_props, KLAV_PROPID_OBJECT_MFLAGS, flags);
	KLAV_Set_Property_Str(io_props, KLAV_PROPID_OBJECT_NAME_ANSI, pCtx.m_strObjectName.c_str(cCP_ANSI));

	tDWORD origin = KLAV_OT_FILE;
	if( pCtx.m_nObjectType == OBJTYPE_LOGICALDRIVE )
	{
		origin = KLAV_OT_BOOT;
		InitBootProps(pCtx.m_pObject, pObj, pTmp);
	}
	else if( pCtx.m_nObjectType == OBJTYPE_PHYSICALDISK )
	{
		origin = KLAV_OT_MBR;
		InitMbrProps(pCtx.m_pObject, pObj, pTmp);
	}
	else if( pCtx.m_nObjectType == OBJTYPE_MEMORYPROCESS ||
		     pCtx.m_nObjectType == OBJTYPE_MEMORYPROCESSMODULE )
		origin = KLAV_OT_MEMORY_FILE;

	KLAV_Set_Property_Int(io_props, KLAV_PROPID_OBJECT_ORIGIN, origin);
	return errOK;
}

tERROR CKLAVEngine::ProcessObject(CObjectContext& pCtx)
{
	if( !m_hEngine )
		return errUNEXPECTED;

	if( pCtx.m_fTreatsProcessing )
		return ProcessThreats(pCtx);

	pCtx.m_fPragueObj = !pCtx.m_pIOCtx;
	InitContext(pCtx);

	Allocator alloc(this, pCtx.m_pObject);

	KLAV_STATIC_PROPERTIES(props);
	KLAV_STATIC_PROPERTY(props, KLAV_PROPID_CONTEXT_HEAP, (KLAV_Context*)&alloc);
	KLAV_STATIC_PROPERTY(props, KLAV_PROPID_PARENT_CONTEXT, pCtx.m_pPrev ? pCtx.m_pPrev->m_pProcessCtx : NULL);
//	KLAV_STATIC_PROPERTY(props, KLAV_PROPID_OBJECT_PATH, object_path_utf8.c_str());

	KLAV_CONTEXT_TYPE kcontext = 0;
	KLAV_ERR err = m_hEngine->create_context(&props, &kcontext);
	if( KLAV_SUCC(err) )
		err = m_hEngine->activate_context(kcontext, pCtx.m_pIOCtx, &props);

	KLAV_IO_Object* io_tmp = NULL;
	if( KLAV_SUCC(err) )
	{
		KLAV_Prop_Val prop_ctx ((void *) & pCtx);
		m_hEngine->set_context_property(kcontext,
			KLAV_PROPID_USER_DATA, 0, &prop_ctx.value());

		KLAV_IO_Object* io_obj = (KLAV_IO_Object*)pCtx.m_pIOCtx;
		if( io_obj->get_io_version() == PID_AVS )
			((CachedIO*)io_obj)->init(kcontext);

		pCtx.ProcessScanBegin();

		KLAV_Properties* io_props = io_obj->get_properties();
		InitContextProps(pCtx, io_props, io_tmp);

		klav_string object_path_utf8(pCtx.m_strObjectName.data(), &m_allocator);
		KLAV_Set_Property_Str(io_props, KLAV_PROPID_OBJECT_PATH, object_path_utf8.c_str());

		pCtx.m_pProcessCtx = kcontext;
		err = ProcessAction(kcontext, "PROCESS_OBJECT", 0);
		pCtx.m_pProcessCtx = NULL;

		if( err == errEXCEPTION || err == errSTACK_EMPTY )
			pCtx.ProcessProcessingError((tERROR*)&err);

		long& nSecurityRating = pCtx.m_pScanCtx->m_ProcessInfo->m_nSecurityRating;
		tDWORD nSR = KLAV_Get_Property_Int(io_props, KLAV_PROPID_OBJECT_AVZSR);

		if( nSR < (tDWORD)nSecurityRating )
			nSecurityRating = nSR;

		pCtx.ProcessScanDone();

		if( !(pCtx.m_nStatus & cScanProcessInfo::DETECTED) )
			UpdateBTImages(pCtx, true);

		m_hEngine->deactivate_context(kcontext);
	}

	if( io_tmp )
		io_tmp->destroy();

	if( kcontext )
		m_hEngine->destroy_context(kcontext);

	pCtx.m_pIOCtx = NULL;
	pCtx.m_pCached = NULL;

	MemDestroyCtx(pCtx.m_pObject);
	return err;
}

#ifdef _WIN32
#include <windows.h>

int ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep)
{
	if( code != STATUS_STACK_OVERFLOW )
		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "\nAVP1     EXCEPTION 0x%8X at address 0x%X\n",
			code, ep->ExceptionRecord->ExceptionAddress));

	return EXCEPTION_EXECUTE_HANDLER;
}

bool CheckStack(int stacksize)
{
	__try
	{
		if( !alloca(stacksize) )
			return false;
	}
	__except(1)
	{
		return false;
	}
	return true;
}

tERROR CKLAVEngine::ProcessAction(KLAV_CONTEXT_TYPE kcontext, const char *action_name, void *action_object)
{
	tERROR err;
	if( !CheckStack(0x40000) )
		return errSTACK_EMPTY;

	if( m_bSafeScan )
	{
		__try
		{
			err = m_hEngine->process_action(kcontext, action_name, action_object, NULL);
		}
		__except( ExceptionFilter(_exception_code(), (((struct _EXCEPTION_POINTERS *)_exception_info()))) )
		{
			unsigned int code = _exception_code();
			PR_TRACE(( m_avs, prtALWAYS_REPORTED_MSG, "\nAVS     EXCEPTION 0x%8X in KLAVEngine ProcessObject\n", code));
			err = code != STATUS_STACK_OVERFLOW ? errEXCEPTION : errSTACK_EMPTY;
		}
	}
	else
	{
		err = m_hEngine->process_action(kcontext, action_name, action_object, NULL);
	}
	return err;
}

#else

tERROR CKLAVEngine::ProcessAction(KLAV_CONTEXT_TYPE kcontext, const char *action_name, void *action_object)
{
	return m_hEngine->process_action(kcontext, action_name, action_object, NULL);
}

#endif

tERROR CKLAVEngine::ProcessStream(CObjectContext& pCtx, enChunkType eType, tPTR pChunk, tDWORD nSize)
{
	if( !m_hEngine )
		return errUNEXPECTED;

	tStreamCtx*& pStreamCtx = (tStreamCtx*&)pCtx.m_pIOCtx;

	KLAV_ERR err = KLAV_OK;
	if( !pCtx.m_fProcessing )
	{
		pStreamCtx = new tStreamCtx();
		pStreamCtx->m_struct_size = sizeof(KLAV_Stream_Packet);
		pStreamCtx->m_flags = KLAV_STREAM_PACKET_F_BEGIN;

		err = m_hEngine->create_context(0, &pStreamCtx->m_ctx);
		if( KLAV_SUCC(err) )
			err = m_hEngine->activate_context(pStreamCtx->m_ctx, NULL, NULL);

		if( KLAV_SUCC(err) )
		{
			KLAV_Prop_Val prop_ctx ((void *) &pCtx);
			m_hEngine->set_context_property(pStreamCtx->m_ctx,
				KLAV_PROPID_USER_DATA, 0, &prop_ctx.value());
		}

		pCtx.ProcessScanBegin();
	}

	if( !pStreamCtx )
		return errUNEXPECTED;

	if( KLAV_SUCC(err) )
	{
		if( eType == CHUNK_TYPE_LAST )
			pStreamCtx->m_flags = KLAV_STREAM_PACKET_F_END;

		switch(pCtx.m_pScanCtx->m_ProcessInfo->m_eStreamFormat)
		{
		case STREAM_FORMAT_GZIP: pStreamCtx->m_format = KLAV_STREAM_FORMAT_GZIP; break;
		default: pStreamCtx->m_format = KLAV_STREAM_FORMAT_RAW;
		}

		pStreamCtx->m_data = (uint8_t*)pChunk;
		pStreamCtx->m_size = nSize;

		err = ProcessAction(pStreamCtx->m_ctx, "PROCESS_PACKET", (KLAV_Stream_Packet*)pStreamCtx);
	}

	if( eType == CHUNK_TYPE_LAST )
	{
		pCtx.ProcessScanDone();
		m_hEngine->deactivate_context(pStreamCtx->m_ctx);
		m_hEngine->destroy_context(pStreamCtx->m_ctx);
		delete pStreamCtx;
		pCtx.m_pIOCtx = NULL;
	}
	return err;
}

tERROR CKLAVEngine::ProcessThreatTail(CObjectContext* pCtx)
{
	if( !pCtx || !pCtx->m_fReopened )
		return errOK;

	ProcessThreatTail(pCtx->m_pNext);

	if( pCtx->m_fOSObject )
		pCtx->m_pPrev->ProcessOSPreClose((cOS*)pCtx->m_pObject);
	else
		pCtx->ProcessScanDone();

	pCtx->m_pObject->sysCloseObject();
	pCtx->m_pObject = NULL;
	pCtx->m_fReopened = 0;
	return errOK;
}

tERROR CKLAVEngine::ProcessThreats(CObjectContext& pCtx)
{
	cIO* pIO = (cIO*)pCtx.m_pObject;

	tQWORD qwSize = 0;
	pIO->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT);
	if( !qwSize )
		return errOK;

	tDWORD num = (tDWORD)qwSize/sizeof(tDWORD);
	tDWORD* pIdx = new tDWORD[num];
	if( PR_FAIL(pIO->SeekRead(NULL, 0, pIdx, (tDWORD)qwSize)) )
	{
		delete[] pIdx;
		return errUNEXPECTED;
	}

	cCharVector pReopenCache;

	for(tDWORD i = 0; i < num; i+=2)
	{
		cInfectedObjectInfo pInfo;
		if( PR_FAIL(m_avs->GetTreatInfo(pIdx[i+1], &pInfo)) )
			continue;

		tDWORD datasize = pInfo.m_pReopenData.size();
		if( pReopenCache.size() < datasize )
			pReopenCache.resize(datasize);

		tReopenData* rd = (tReopenData*)pInfo.m_pReopenData.data();
		tReopenData* cc = (tReopenData*)pReopenCache.data();

		if( !memcmp(rd, cc, datasize) )
			continue;

		tERROR err = errOK;
		tDWORD offset = 0, rrsize = 0;
		cObj *obj, *parent = (cObj*)pIO;

		cStringObj strVirtualName;

		CObjectContext* ctx = &pCtx;
		for( ;PR_SUCC(err) && offset < datasize;
				ctx = ctx->m_pNext, parent = obj)
		{
			tReopenData* rr = (tReopenData*)((tBYTE*)rd + offset);
			tReopenData* rc = (tReopenData*)((tBYTE*)cc + offset);

			rrsize = rr->size + offsetof(tReopenData,data);
			offset += rrsize;
			if( !memcmp(rd, cc, offset) )
			{
				obj = ctx->m_pNext->m_pObject;
				strVirtualName.assign(obj, CObjectContext::m_propVirtualName);
				continue;
			}
								  
			if( rr->propid == pgOBJECT_FULL_NAME && PR_SUCC(parent->sysCheckObject(parent, IID_OS, 0,0,0)) )
			{
				cStrObj sName;
				sName.assign(rr->data, rr->cp, rr->size);

				err = ((cOS*)parent)->IOCreate((cIO**)&obj, cAutoString(sName), fACCESS_RW, fOMODE_OPEN_IF_EXIST);
			}
			else if( PR_SUCC(err = parent->sysCreateObject(&obj, rr->iid, rr->pid)) )
			{
				if( rr->pid == PID_WIN32_NFIO )
				{
					obj->propSetDWord(pgOBJECT_ACCESS_MODE, fACCESS_READ|fACCESS_FORCE_READ|fACCESS_FORCE_OPEN);
					obj->propSetDWord(pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST|fOMODE_OPEN_IF_EXECUTABLE);
				}

				if( rr->iid == IID_OS )
					obj->propSetBool(pgMULTIVOL_AS_SINGLE_SET, cTRUE);

				if( rr->propid == pgOBJECT_FULL_NAME )
					err = obj->propSetStr(0, rr->propid, rr->data, rr->size, rr->cp);
				else
					err = obj->propSet(0, rr->propid, rr->data, rr->size);

				if( PR_SUCC(err) )
					err = obj->sysCreateObjectDone();
			}

			if( PR_FAIL(err) )
				break;

			memcpy(cc, rd, offset);
			memset((tBYTE*)cc + offset, 0, datasize - offset);

			ProcessThreatTail(ctx->m_pNext);

			CObjectContext* pThis = ctx->GetNextContext(obj);
			if( rr->iid == IID_OS )
			{
				ctx->ProcessOSEnter((cOS*)obj);
				pThis->m_fOSObject = 1;
			}
			else
			{
				pThis->ProcessScanBegin();
				if( !strVirtualName.empty() )
					strVirtualName += "/";
				strVirtualName += pThis->m_strObjectName;
			}

			strVirtualName.copy(obj, CObjectContext::m_propVirtualName);
			pThis->m_fReopened = 1;
		}

		if( ctx != &pCtx  )
		{
			ProcessObject(*ctx);
			ctx->m_fReopened = 1;
		}
	}

	ProcessThreatTail(pCtx.m_pNext);

	delete[] pIdx;
	return errOK;	
}

tERROR CKLAVEngine::SetWriteAccess(CObjectContext& pCtx, cObj*& pDeletable)
{
	tERROR ret_error = errOK;
	for(CObjectContext* ctx = &pCtx; ctx; ctx = ctx->m_pPrev)
	{
		if( !ctx->m_fProcessing )
			continue;

		cIO*   obj = (cIO*)ctx->m_pObject;
		tPID   pid = obj->propGetDWord(pgPLUGIN_ID);

		tDWORD mode = obj->propGetDWord(pgOBJECT_ACCESS_MODE);
		tDWORD change_mode = fACCESS_WRITE;
		tERROR error = errOK;

		if( pid == PID_NATIVE_FIO )
			change_mode |= fACCESS_CHECK_WRITE;

		if( (mode & change_mode) != change_mode )
		{
			mode |= change_mode;
			mode &= ~fACCESS_FILE_FLAG_NO_BUFFERING;
			error = obj->propSetDWord(pgOBJECT_ACCESS_MODE, mode);
		}

		if( error == warnSET_PARENT_ACCESS_MODE_NOT_NEEDED )
			break;

		if( PR_SUCC(error) )
		{
			if( pid == PID_NATIVE_FIO )
			{
				tQWORD qwFreeSpace, qwSize;
				if( PR_SUCC(PrGetDiskFreeSpace((cObj*)obj, &qwFreeSpace)) )
					if( PR_SUCC(obj->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT)) )
						if( qwSize > qwFreeSpace )
							error = errOUT_OF_SPACE;
			}
			else if( pid == PID_WINDISKIO )
			{
				if( cMEDIA_TYPE_FIXED != obj->propGetDWord(plMediaType) )
				{
					tDWORD count = 0;
					tQWORD qwSize = 0;
					tBYTE  b;

					obj->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT);
					if( qwSize )
						error = obj->SeekRead(&count, 0, &b, 1);

					if( PR_SUCC(error) && count )
					{
						error = obj->SeekWrite(&count, 0, &b, 1);
						if( PR_SUCC(error) )
							error = obj->Flush();
					}
				}
			}
		}

		tERROR err = obj->propGetDWord(CObjectContext::m_propReadonlyError);
		if( PR_SUCC(error) )
			error = err;

		if( PR_SUCC(ret_error) )
			ret_error = error;

		if( pid == PID_WINDISKIO )
			continue;

		bool bDeletable = false;
		if( PR_SUCC(err) )
			if( fAVAIL_DELETE_ON_CLOSE & obj->propGetDWord(pgOBJECT_AVAILABILITY) )
				bDeletable = true;

		if( PR_SUCC(error) )
		{
			if( !pDeletable && bDeletable )
				pDeletable = (cObj*)obj;
		}
		else
		{
			if( error == errACCESS_DENIED )
				pDeletable = NULL;
			else if( bDeletable )
				pDeletable = (cObj*)obj;
		}
	}

	return ret_error;
}

tERROR CKLAVEngine::AddReopenData(cIO* pIO, tPTR pData, tDWORD nPos, tDWORD pUserData)
{
	tQWORD qwSize = 0;
	pIO->GetSize(&qwSize, IO_SIZE_TYPE_EXPLICIT);
	pIO->SeekWrite(NULL, qwSize, &pUserData, sizeof(tDWORD));
	pIO->SeekWrite(NULL, qwSize + sizeof(tDWORD), &nPos, sizeof(tDWORD));
	return errOK;
}

tERROR CKLAVEngine::UpdateBTImages(CObjectContext& pCtx, bool bSave)
{
	if( pCtx.m_nObjectType != OBJTYPE_LOGICALDRIVE &&
		pCtx.m_nObjectType != OBJTYPE_PHYSICALDISK )
		return errOK;

	cAutoObj<cBtImages> hBTImages;
	tERROR error = m_avs->sysCreateObject(hBTImages, IID_BTIMAGES, PID_BTIMAGES);
	if( PR_SUCC(error) )
	{
		cStrObj sBTPath = m_avs->m_settings.m_sDataFolder;
		sBTPath += L"BTImages.dat";
		sBTPath.copy(hBTImages, pgRepositoryName);
		error = hBTImages->sysCreateObjectDone();
	}

	if( PR_FAIL(error) )
		return error;

	if( bSave )
		return hBTImages->Save(pCtx.m_pObject);

	error = hBTImages->Restore(pCtx.m_pObject, BTIMAGES_LAST);
	if( PR_SUCC(error) )
	{
		pCtx.ProcessDisinfected();
		error = errOK_DECIDED;
	}

	return error;
}

enDetectStatus CKLAVEngine::GetDetectStatus(CObjectContext& pCtx)
{
	if( pCtx.m_pHash )
		return pCtx.m_pHash->m_dwStatus;

	KLAV_Properties* props = (KLAV_Properties*)pCtx.m_pContext;
	if( !props )
		return DSTATUS_UNKNOWN;

	switch (KLAV_Get_Property_Int (props, KLAV_PROPID_VERDICT_CONFIDENCE))
	{
	case KLAV_VERDICT_CONFIDENCE_SURE:      return DSTATUS_SURE;
	case KLAV_VERDICT_CONFIDENCE_PARTIAL:   return DSTATUS_PARTIAL;
	case KLAV_VERDICT_CONFIDENCE_HEURISTIC: return DSTATUS_HEURISTIC;
	case KLAV_VERDICT_CONFIDENCE_PROBABLE:  return DSTATUS_PROBABLY;
	}
	return DSTATUS_UNKNOWN;
}

enDetectType CKLAVEngine::GetDetectType(CObjectContext& pCtx)
{
	if( pCtx.m_pHash )
		return pCtx.m_pHash->m_dwType;

	KLAV_Properties* props = (KLAV_Properties*)pCtx.m_pContext;
	if( !props )
		return DETYPE_UNKNOWN;

	return (enDetectType)(KLAV_Get_Property_Int(props, KLAV_PROPID_VERDICT_TYPE));
}

enDetectDanger CKLAVEngine::GetDetectDanger(CObjectContext& pCtx)
{
	if( pCtx.m_pHash )
		return pCtx.m_pHash->m_dwDanger;

	KLAV_Properties* props = (KLAV_Properties*)pCtx.m_pContext;
	if( !props )
		return DETDANGER_UNKNOWN;

	tDWORD danger = KLAV_Get_Property_Int(props, KLAV_PROPID_VERDICT_DANGER);
	switch( danger )
	{
	default:
	case 0: return DETDANGER_HIGH;
	case 1: return DETDANGER_MEDIUM;
	case 2: return DETDANGER_LOW;
	case 3: return DETDANGER_INFORMATIONAL;
	}
	return DETDANGER_UNKNOWN;
}

tDWORD CKLAVEngine::GetDetectBehaviour(CObjectContext& pCtx)
{
	if( pCtx.m_pHash )
		return pCtx.m_pHash->m_dwBehavior;

	KLAV_Properties* props = (KLAV_Properties*)pCtx.m_pContext;
	if( !props )
		return 0;

	return KLAV_Get_Property_Int(props, KLAV_PROPID_VERDICT_BEHAVIOUR);
}

tERROR CKLAVEngine::GetDetectName(CObjectContext& pCtx, cStringObj& strName)
{
	if( pCtx.m_pHash )
	{
		strName = pCtx.m_pHash->m_sName;
		return errOK;
	}

	KLAV_Properties* props = (KLAV_Properties*)pCtx.m_pContext;
	if( !props )
		return errUNEXPECTED;

	const char * detect_name = KLAV_Get_Property_Str(props, KLAV_PROPID_VERDICT_NAME);
	if( !detect_name )
		return errUNEXPECTED;

	strName = detect_name;

	// append short record name (record# section should not present in release KFBs)
	uint32_t record_id = KLAV_Get_Property_Int (props, KLAV_PROPID_VERDICT_RECORD_ID);
	const char * short_name = KLAV_Get_Property_Str(props, KLAV_PROPID_VERDICT_SHORT_NAME);
	if( short_name && short_name[0] != 0 && record_id != 0 )
	{
		strName.append (" ");
		strName.append (short_name);
	}

	return errOK;
}

tERROR CKLAVEngine::GetPakerName(CObjectContext& pCtx, cStringObj& strName)
{
	KLAV_Properties* props = (KLAV_Properties*)pCtx.m_pContext;
	if( !props )
		return errUNEXPECTED;

	const char* sName = KLAV_Get_Property_Str(props, KLAV_PROPID_TRANSFORMER_NAME);
	if( sName )
		strName = sName;
	return errOK;
}

bool CKLAVEngine::GetDisinfectability(CObjectContext& pCtx)
{
	if( pCtx.m_pHash )
		return true;

	if( pCtx.m_pPrev && pCtx.m_pPrev->m_fCurableArchive )
		return true;

	KLAV_Properties* props = (KLAV_Properties*)pCtx.m_pContext;
	if( !props )
		return false;

	if( KLAV_Get_Property_Int(props, KLAV_PROPID_VERDICT_CURABILITY) == 
			KLAV_VERDICT_CURABILITY_CURABLE )
		return true;

	return false;
}

tERROR CKLAVEngine::GetReopenData(CObjectContext& pCtx, cCharVector& pData)
{
	for(CObjectContext* ctx = &pCtx; ctx; ctx = ctx->m_pPrev)
	{
		cObj* obj = ctx->m_pObject;

		tERROR err = obj->propGetDWord(CObjectContext::m_propReadonlyError);
		if( PR_FAIL(err) )
			continue;

		cObj* parent = obj->propGetObj(CObjectContext::m_propExecutableParent);
		if( parent && parent != obj )
			continue;

		if( obj->propGetDWord(pgPLUGIN_ID) == PID_TMPFILE )
			continue;

		tPROPID prop = pgOBJECT_REOPEN_DATA;
		tDWORD size = obj->propSize(prop);
		if( !size && PR_SUCC(obj->sysCheckObject(obj, IID_IO, 0,0,0)) )
		{
			prop = pgOBJECT_FULL_NAME;
			size = CALL_SYS_PropertySize(obj,prop);
		}

		if( !size )
		{
			pData.resize(0);
			continue;
		}

		tDWORD datasize = size + offsetof(tReopenData,data);
		tDWORD oldsize = pData.size();
		pData.resize(oldsize + datasize);

		tReopenData* data = (tReopenData*)(pData.data());
		memmove((tBYTE*)data + datasize, data, oldsize);

		data->size  = size;
		data->propid= prop; 
		data->cp    = obj->propGetDWord(pgOBJECT_FULL_NAME_CP);
		data->iid   = obj->propGetDWord(pgINTERFACE_ID);
		data->pid   = obj->propGetDWord(pgPLUGIN_ID);

		if( !data->cp )
			data->cp = cCP_ANSI;

		if( data->pid == 61015/*PID_MKAVIO*/ ) // zalipuha
			data->pid = PID_WIN32_NFIO;

		if( prop == pgOBJECT_FULL_NAME )
			err = obj->propGetStr(0, prop, data->data, size, data->cp);
		else
			err = obj->propGet(0, prop, data->data, size);

		if( PR_FAIL(err) )
			pData.resize(0);
	}

	if( !pData.size() )
		return errNOT_OK;

	return errOK;
}

bool CKLAVEngine::IsShouldBeScan(CObjectContext& pCtx)
{
	KLAV_Properties* props = (KLAV_Properties*)pCtx.m_pContext;
	if( !props )
		return true;

	tDWORD rflags = KLAV_Get_Property_Int(props, KLAV_PROPID_OBJECT_RFLAGS);
	if( rflags & KLAV_RF_NOT_A_PROGRAMM )
		return false;

	return true;
}

// -------------------------------------------

inline CObjectContext* CKLAVEngine::get_ctx(KLAV_CONTEXT_TYPE ctx)
{
	KLAV_Prop_Val prop;
	m_hEngine->get_context_property(ctx, KLAV_PROPID_USER_DATA, 0, &prop.value());
	
	return (CObjectContext*) prop.value ().p;
}

KLAV_ERR CKLAVEngine::yield(KLAV_CONTEXT_TYPE context, uint32_t)
{
	CObjectContext* pCtx = get_ctx(context);
	if( !pCtx )
		return KLAV_EINVAL;
	tERROR err = pCtx->OnEngineProcessing();
	if(PR_FAIL(err) )
		return KLAV_PR_ERROR (err);

	return KLAV_OK;
}

void convert_name(char* name)
{
	char* cp = name + strlen(name) + 2;
	uint8_t ct = *(cp-1);
	if( ct > 5 ) ct = 0;

	if( ct == 5 )
	{
		uint16_t cpage = *(uint16_t*)cp;
		if( cp && (cpage == 0xFFFE || cpage == 0xFEFF) )
		{
			cStringObj str(name, cpage == 0xFFFE ? cCP_UNICODE_BIGENDIAN : cCP_UNICODE);
			str.copy((tVOID*)name, 0x200, cCP_ANSI);
		}
	}
	else if( ct != 1 )
	{
		cStringObj str(name, cCP_OEM);
		str.copy((tVOID*)name, 0x200, cCP_ANSI);
	}
}

KLAV_ERR CKLAVEngine::handle_callback(KLAV_CONTEXT_TYPE context, KLAV_CALLBACK_CODE code, KLAV_Properties* props)
{
	CObjectContext* pCtx = get_ctx(context);
	if( !pCtx )
		return KLAV_EINVAL;

	pCtx->m_pContext = (hOBJECT)props;

	tERROR error = errOK;
	KLAV_ERR ret = KLAV_OK;
	switch (code)
	{
	case KLAV_CALLBACK_OBJECT_DETECT:
		error = pCtx->ProcessVirusDetect();
		if( PR_SUCC(error) )
			error = pCtx->ProcessDisinfectRequest();

		if( error == errOK_DECIDED )
		{
			if( errOK_DECIDED != UpdateBTImages(*pCtx, false) )
				ret = KLAV_ACTION_CURE;
		}
		break;

	case KLAV_CALLBACK_OBJECT_CHECK:
		{
			tDWORD rflags = KLAV_Get_Property_Int(props, KLAV_PROPID_OBJECT_RFLAGS);
			if( !(rflags & KLAV_RF_EXECUTABLE_FORMAT) && AVLIB_IsExecutableFormatLite )
			{
				tDWORD dwResult = 0;
				AVLIB_IsExecutableFormatLite(pCtx->GetCached(), &dwResult);
				if( dwResult & cAVLIB_EXECUTABLE_FORMAT )
					rflags |= KLAV_RF_EXECUTABLE_FORMAT;
			}

			pCtx->InitExecutableParent(!!(rflags & KLAV_RF_EXECUTABLE_FORMAT));
		}

		if( !pCtx->IsShouldBeScan() )
			return KLAV_ECANCEL;
		break;

	case KLAV_CALLBACK_OBJECT_ARCHIVE_BEGIN:
		ret = pCtx->ProcessArchiveBegin(
			KLAV_Get_Property_Long(props, KLAV_PROPID_OBJECT_OFFSET),
			(tDWORD*)KLAV_Get_Property_Ptr(props, KLAV_PROPID_USER_DATA));

		if( PR_FAIL(ret) )
		{
			if( pCtx->m_nStatus & cScanProcessInfo::DISINFECTED )
				ret = KLAV_RF_CURE|KLAV_RF_DETECT;
			else if( pCtx->m_nStatus & cScanProcessInfo::DETECTED )
				ret = KLAV_RF_DETECT;
		}
		break;

	case KLAV_CALLBACK_OBJECT_ARCHIVE:
		ret = pCtx->ProcessArchiveCheck(
			props ? KLAV_Get_Property_Long(props, KLAV_PROPID_OBJECT_HASH) : 0,
			props ? KLAV_Get_Property_Long(props, KLAV_PROPID_OBJECT_OFFSET) : 0);

		if( PR_FAIL(ret) )
		{
			if( pCtx->m_nStatus & cScanProcessInfo::DETECTED )
				ret = KLAV_RF_DETECT;
		}
		else
		{
			pCtx->ProcessOSEnter(NULL);
			ret = KLAV_OK;
		}
		break;

	case KLAV_CALLBACK_OBJECT_ARCHIVE_DONE:
		pCtx->ProcessArchiveDone(NULL);
		break;

	case KLAV_CALLBACK_OBJECT_CORRUPTED:
		pCtx->ProcessCorrupted();
		break;

	case KLAV_CALLBACK_OBJECT_ENCRYPTED:
		pCtx->ProcessEncrypted();
		break;

	case KLAV_CALLBACK_OBJECT_CURED:
		pCtx->ProcessDisinfected();
		break;

	case KLAV_CALLBACK_OBJECT_CURE_FAILED:
		pCtx->ProcessDisinfectImpossible();
		break;

	case KLAV_CALLBACK_HEURISTIC_PROCESS_PARENT:
		if( pCtx->m_pPrev )
			pCtx->m_pPrev->m_fToPostProcessing = 1;
		break;

	case KLAV_CALLBACK_PARENT_GET_INFO:
		ret = process_get_parent(*pCtx, props);
		break;

	case KLAV_CALLBACK_REGISTER_PASSWORD:
		if( props )
		{
			cStrObj sPswd(KLAV_Get_Property_Str(props, KLAV_PROPID_PASSWORD));
			ret = pCtx->m_pScanCtx->RegisterPassword(sPswd);
		}
		break;

	case KLAV_CALLBACK_GET_PASSWORD:
		if( props )
		{
			cStrObj sPswd;
			if( !pCtx->m_pScanCtx->GetPassword(sPswd, &pCtx->m_nPswdPos) )
				ret = KLAV_ECANCEL;
			else
				KLAV_Set_Property_Str(props, KLAV_PROPID_PASSWORD, sPswd.c_str(cCP_ANSI));
		}
		break;

	case KLAV_CALLBACK_GETAPI:
		{
			const char* sLib  = KLAV_Get_Property_Str(props, KLAV_PROPID_GETAPI_LIBRARY);
			const char* sProc = KLAV_Get_Property_Str(props, KLAV_PROPID_GETAPI_PROC);
			tFUNC_PTR sym = NULL;
			PrHMODULE hmod = NULL, *hCached = NULL;
			if( !_strnicmp(sLib, "advapi32", 8) )
				hCached = &m_hmodAdvApi;
			else if( !_strnicmp(sLib, "kernel32", 8) )
				hCached = &m_hmodKernel;

			if( hCached )
				hmod = *hCached;

			if( !hmod )
				hmod = PrLoadLibrary(sLib, cCP_ANSI);
			if( hmod)
			{
				sym = PrGetProcAddress(hmod, sProc);
				if( hCached )
					*hCached = hmod;
				else
					PrFreeLibrary(hmod);
			}
			return (KLAV_ERR)sym;
		}
		break;

	case KLAV_CALLBACK_OBJECT_NAME:
		convert_name((char*)KLAV_Get_Property_Str(props, KLAV_PROPID_OBJECT_NAME_ANSI));
		break;
	}
	
	return ret;
}

KLAV_ERR CKLAVEngine::process_get_parent(CObjectContext& pCtx, KLAV_Properties* props)
{
	if( !props )
		return KLAV_EINVAL;

	uint32_t dwLevel = KLAV_Get_Property_Int(props, KLAV_PROPID_PARENT_LEVEL);

	CObjectContext* pParent = &pCtx;
	for(uint32_t i = 0; i < dwLevel && pParent; i++)
		pParent = pCtx.m_pPrev;

	if( !pParent )
		return KLAV_ENOTFOUND;

	if( pParent->m_fUnpacked )
		KLAV_Set_Property_Str(props, KLAV_PROPID_TRANSFORMER_NAME,
			(tCHAR*)pParent->m_sPackerName.c_str(cCP_ANSI));

	return KLAV_OK;
}

KLAV_ERR CKLAVEngine::process_subobject(KLAV_CONTEXT_TYPE parent_context, void* object, KLAV_Properties* props)
{
	if( !object )
		return KLAV_EINVAL;

	CObjectContext* pCtx = get_ctx(parent_context);
	if( !pCtx )
		return KLAV_EINVAL;

	if( PR_FAIL(pCtx->OnEngineProcessing()) )
		return KLAV_ECANCEL;

	if( pCtx->m_pObject->propGetBool(CObjectContext::m_propSpecialCure) )
		return errOK;

	KLAV_IO_Object* io = (KLAV_IO_Object*)object;
	cIO* hobj = (cIO*)io->get_io_iface(KLAV_IFIO_PR_HOBJECT);
	if( !hobj )
	{
		tERROR error = pCtx->m_pObject->sysCreateObject((cObj**)&hobj, IID_IO, PID_AVS, 0);
		if( PR_SUCC(error) )
		{
			hobj->propSetPtr(plKLAV_IO_OBJECT, io);
			error = hobj->sysCreateObjectDone();
		}

		if( PR_FAIL(error) )
		{
			if( hobj )
				hobj->sysCloseObject();
			return error;
		}
	}

	uint32_t RFlags = 0;
	KLAV_Properties* io_props = io->get_properties();
	if( io_props )
	{
		RFlags = KLAV_Get_Property_Int(io_props, KLAV_PROPID_OBJECT_RFLAGS);

		const char* sName = KLAV_Get_Property_Str(io_props, KLAV_PROPID_OBJECT_NAME_ANSI);
		if( !sName )
			sName = KLAV_Get_Property_Str(io_props, KLAV_PROPID_TRANSFORMER_NAME);

		hobj->propSetStr(NULL, pgOBJECT_NAME, sName ? (tPTR)sName : (tPTR)"");
		hobj->propSetStr(NULL, pgOBJECT_FULL_NAME, sName ? (tPTR)sName : (tPTR)"");
	}

	if( RFlags & KLAV_RF_PACKED )
	{
		pCtx->m_pContext = (hOBJECT)io_props;
		pCtx->ProcessSubIOEnter();

		if( RFlags & KLAV_RF_PROTECTED )
		{
			pCtx->ProcessPasswordProtected();
			return KLAV_EUNKNOWN;
		}

		if( RFlags & KLAV_RF_IO_ERROR )
		{
			tERROR err = errOBJECT_READ;
			pCtx->ProcessProcessingError(&err);
			return KLAV_EUNKNOWN;
		}

		if( RFlags & KLAV_RF_INTERNAL_ERROR )
			return KLAV_EUNKNOWN;
	}
	else
	{
		if( !(RFlags & KLAV_RF_IN_CURABLE_EMBEDDED) )
			hobj->propSetDWord(CObjectContext::m_propReadonlyError,
				errOBJECT_WRITE_NOT_SUPPORTED);
		else
			pCtx->m_fCurableArchive = 1;
	}

	if( PR_FAIL(pCtx->ProcessSubIO(hobj, object, false, !!(RFlags & KLAV_RF_PROTECTED))) )
		return KLAV_EUNKNOWN;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL CKLAVEngine::create_temp_object(KLAV_CONTEXT_TYPE ctx, KLAV_Properties * props, KLAV_IO_Object **ptempobj)
{
	CObjectContext* pCtx = get_ctx(ctx);    
	if( !pCtx )
		return KLAV_EINVAL;

	cObj* obj = pCtx->m_pObject;
	if( !obj )
		return KLAV_EUNEXPECTED;

	cIO* hio = 0;
	tERROR error = obj->sysCreateObjectQuick((hOBJECT*)&hio, IID_IO, PID_TMPFILE);
	if( PR_FAIL(error) )
		return error;

	Allocator alloc(this, obj);
	*ptempobj = KLAV_NEW(&alloc) CachedIO(this, obj, hio);
	return KLAV_OK;
}

KLAV_ERR CKLAVEngine::print_message_v(KLAV_CONTEXT_TYPE context, uint32_t level, const char *fmt, va_list ap)
{
	char buff[0x1000] = "KLAV_ENGINE\t";
// vsnprintf_s IS NOT portable
//	vsnprintf_s(buff + 12, sizeof(buff)-12, _TRUNCATE, fmt, ap);
	vsnprintf(buff + 12, sizeof(buff)-12, fmt, ap);
	PR_TRACE((m_avs, prtIMPORTANT, "%s", buff));
	return KLAV_OK;
}

// -------------------------------------------

KLAV_ERR KLAV_CALL CKLAVEngine::dl_open(const char *path, void **phdl)
{
	*phdl = 0;
	
	if( path == 0 || path[0] == 0 )
		return KLAV_EINVAL;

	cStrObj sPath;
	if( IsFileUpdated(path) )
	{
		cStrObj file(path);
		tDWORD pos = file.find_last_of(L".");
		if( pos != cStrObj::npos )
		{
			sPath.assign(file, 0, pos);
			sPath += m_pCurInfo->m_sSuffix;
			sPath.append(path + pos);
		}
	}

	cStrObj sFullPath(m_avs->m_settings.m_sBaseFolder);
	sFullPath.check_last_slash();

	if( !sPath.empty() )
		sFullPath += sPath;
	else
		sFullPath += path;

	PrHMODULE hmod = PrLoadLibrary(sFullPath.data(), cCP_UNICODE);
	if( hmod == 0 )
		return KLAV_ENOTFOUND;

	*phdl = (void*)hmod;
	return KLAV_OK;
}

void * KLAV_CALL CKLAVEngine::dl_sym(void *hdl, const char *name)
{
	if( hdl == 0 )
		return 0;

	if( name == 0 || name [0] == 0 )
		return 0;

	tFUNC_PTR sym = PrGetProcAddress ((PrHMODULE) hdl, name);
	return (void*)sym;
}

void KLAV_CALL CKLAVEngine::dl_close(void *hdl)
{
	if( hdl != 0 )
		PrFreeLibrary((PrHMODULE) hdl);
}

// -------------------------------------------

uint8_t* CKLAVEngine::Allocator::alloc(size_t size)
{
	if( size >= 0x8000 )
		return m_hEngine->MemCapture(m_hObject, size);

	tPTR ptr = 0;
	m_hObject->heapAlloc(&ptr, size);
	return (uint8_t*)ptr;
}

uint8_t* CKLAVEngine::Allocator::realloc(void *ptr, size_t newsize)
{
	m_hObject->heapRealloc(&ptr, ptr, newsize);
	return (uint8_t*)ptr;
}

void CKLAVEngine::Allocator::free(void *ptr)
{
	if( !ptr )
		return;

	if( ((uint32_t)ptr % m_hEngine->m_nPageSize) || !m_hEngine->MemRelease(ptr) )
		m_hObject->heapFree(ptr);
}

// -------------------------------------------

//#define _GUARD_PAGES

uint8_t* CKLAVEngine::MemCapture(cObj* obj, size_t size)
{
	cAutoCS locker(m_lock, true);

	size_t i, n = m_vMems.size();
	for(i = 0; i < n; i++)
	{
		tMemAlloc& mem = m_vMems[i];
		if( mem.m_lock )
			continue;

		if( mem.m_size == size )
		{
			mem.m_lock = cTRUE;
			mem.m_obj = obj;
			mem.m_timestamp = PrGetTickCount();
			memset(mem.m_ptr, 0, size);
			return (uint8_t*)mem.m_ptr;
		}
	}

	tPTR ptr = vm_allocate(size, PROTECTION_READ|PROTECTION_WRITE, NULL);
	if( !ptr )
		return NULL;

	tMemAlloc& mem = m_vMems.push_back();
	mem.m_lock = cTRUE;
	mem.m_size = size;
	mem.m_obj = obj;
	mem.m_timestamp = PrGetTickCount();
	mem.m_ptr = (tPTR)ptr;

	return (uint8_t*)mem.m_ptr;
}

void CKLAVEngine::MemFree(tMemAlloc& mem)
{
	if( mem.m_ptr )
		vm_deallocate(mem.m_ptr, mem.m_size);
}

bool CKLAVEngine::MemRelease(void *ptr)
{
	cAutoCS locker(m_lock, true);

	size_t i, n = m_vMems.size();
	for(i = 0; i < n; i++)
	{
		tMemAlloc& mem = m_vMems[i];
		if( mem.m_ptr == ptr )
		{
			mem.m_lock = cFALSE;
			mem.m_obj = NULL;
			return true;
		}
	}
	return false;
}

void CKLAVEngine::MemDestroyCtx(cObj* obj)
{
	tDWORD timestamp = PrGetTickCount();

	cAutoCS locker(m_lock, true);

	size_t i, n = m_vMems.size();
	for(i = 0; i < n; i++)
	{
		tMemAlloc& mem = m_vMems[i];
		if( !mem.m_lock )
		{
			if( timestamp - mem.m_timestamp > 10000 )
			{
				MemFree(mem);
				m_vMems.remove(i--); n--;
			}
			continue;
		}

		if( mem.m_obj == obj )
		{
			mem.m_lock = cFALSE;
			mem.m_obj = NULL;
		}
	}
}

// -------------------------------------------

uint32_t CKLAVEngine::CachedIO::read_cache(klav_filepos_t pos, void * buff, uint32_t size)
{
	uint32_t readed = 0;
	register CCachedBuff* pd = m_cache;
	for(;;++pd)
	{
		if( !pd->m_data )
		{
			if( pd < m_cache + IO_CACHE_SIZE )
			{
				pd->m_data = (uint8_t*)alloc(IO_CACHE_CHUNK_SIZE);
				if( !pd->m_data )
					return readed;
			}
			else
			{
				pd--;
				pd->m_size = 0;
			}

			if( !pd->m_size ) // not inited
			{
				pd->m_from = pos - (pos%IO_CACHE_CHUNK_SIZE);
				m_hio->SeekRead(&pd->m_size, pd->m_from, pd->m_data, IO_CACHE_CHUNK_SIZE);
				if( !pd->m_size )
					return readed;
			}
		}

		register int32_t offs = (int32_t)(pos - pd->m_from);
		if( offs < 0 || offs >= IO_CACHE_CHUNK_SIZE )
			continue;

		if( pd != m_cache )
		{
			CCachedBuff tmp = *m_cache;
			*m_cache = *pd;
			*pd = tmp;
			pd = m_cache;
		}

		int32_t rsize = pd->m_size - offs;

		if( rsize <= 0 )
			break;

		if( size <= (uint32_t)rsize )
		{
			memcpy(buff, pd->m_data+offs, size);
			readed += size;
			pos += size;
			break;
		}

		memcpy(buff, pd->m_data+offs, rsize);
		readed += rsize;
		pos += rsize;
		size -= rsize;
		buff = ((uint8_t*)buff + rsize);

		if( pd->m_size < IO_CACHE_CHUNK_SIZE )
			break;
	}

	return readed;
}

// -------------------------------------------
