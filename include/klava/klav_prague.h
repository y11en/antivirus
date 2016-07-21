// klav_prague.h
//
//

#ifndef klav_prague_h_INCLUDED
#define klav_prague_h_INCLUDED

#define KLAV_PRAGUE 1

#include <kl_platform.h>
#ifdef KL_PLATFORM_WINDOWS
# define KL_UNICODE 1
#endif

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_mutex.h>

#include <klava/klavdb.h>
#include <klava/klavdef.h>
#include <klava/klavsys.h>
#include <klava/klav_string.h>
#include <klava/klav_utils.h>
#include <klava/klavstl/vector.h>

////////////////////////////////////////////////////////////////
// Error mapping

#ifdef __cplusplus
extern "C"
#endif
KLAV_ERR KLAV_From_Pr_Error (tERROR err);

#define KLAV_PR_ERROR(PR_ERR) KLAV_From_Pr_Error (PR_ERR)

#ifdef __cplusplus

////////////////////////////////////////////////////////////////
// Prague object - based allocator

class KLAV_Pr_Alloc : public KLAV_IFACE_IMPL(KLAV_Alloc)
{
public:
	KLAV_Pr_Alloc (hOBJECT h=0)
		: m_hObject (h) {}

	hOBJECT get_object () const
		{ return m_hObject; }

	void set_object (hOBJECT h)
		{ m_hObject = h; }

	virtual uint8_t * KLAV_CALL alloc (size_t size);
	virtual uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize);
	virtual void KLAV_CALL free (void *ptr);

private:
	hOBJECT m_hObject;
};

////////////////////////////////////////////////////////////////
// Helpers

// TODO: change to cCP_UTF8 when Petrovich implements UTF-8 support in Prague
const tCODEPAGE KLAV_CP_UTF8 = cCP_ANSI;

////////////////////////////////////////////////////////////////
// prague utils (consider moving to pr_cpp.h)

struct PrCustomProperty
{
	const tCHAR * m_propName;
	tDWORD m_propType;
	tDWORD m_propID;

	operator tPROPID ()
		{ return registerPropID (); }

	tPROPID id ()
		{ return registerPropID (); }

	tDWORD registerPropID ();
};

inline tPROPID PrCustomProperty::registerPropID ()
{
	if (m_propID == 0)
	{
		if (g_root != 0) g_root->RegisterCustomPropId (&m_propID, (tSTRING)m_propName, m_propType);
	}
	return m_propID;
}

#define USING_CUSTOM_PROPERTY(NAME, TYPE) static PrCustomProperty propid_##NAME = { NAME, TYPE, 0 }
#define USING_CUSTOM_PROPERTY_EX(NAME, PROPNAME, TYPE) static PrCustomProperty NAME = { PROPNAME, TYPE, 0 }


////////////////////////////////////////////////////////////////
// Lightweight string wrapper

class PrStr
{
public:
	PrStr () : m_str (0), m_heapobj (0) {}

	PrStr (hOBJECT heapobj, uint32_t size) : m_str (0), m_heapobj (0)
		{ alloc (heapobj, size); }

	~PrStr () { clear (); }

	void attach (hOBJECT heapobj, char *s)
		{ clear (); m_str = s; m_heapobj = heapobj; }

	char * detach ()
		{ char *s = m_str; m_str = 0; m_heapobj = 0; return s; }

	bool assign (hOBJECT heapobj, const char *s);

	tERROR alloc (hOBJECT heapobj, uint32_t size);
	void clear ();

	bool is_valid () const
		{ return m_str != 0; }

	const char * c_str () const
		{ return m_str; }

	char * c_str ()
		{ return m_str; }

	tERROR get_from_property (hOBJECT object, tPROPID prop, tCODEPAGE cp, hOBJECT heapobj);
	tERROR get_from_hstring (hSTRING object, tCODEPAGE cp, hOBJECT heapobj);

private:
	char *  m_str;
	hOBJECT m_heapobj;

	PrStr (const PrStr&);
	PrStr& operator= (const PrStr&);
};

////////////////////////////////////////////////////////////////
// Utility class: AVP detection info

#define KLAV_AVP_DETECT_NAME_MAXSIZE 1000

struct KLAV_Pr_Detect_Info
{
	PrStr    m_detect_name;
	uint32_t m_detect_type;
	uint32_t m_detect_danger;
	uint32_t m_detect_confidence;
	uint32_t m_detect_curability;

#ifdef __cplusplus
	KLAV_Pr_Detect_Info () { clear (); }

	void clear ();
#endif // __cplusplus
};

KLAV_EXTERN_C
tERROR KLAV_Pr_Get_Detect_Info (
			hOBJECT object,
			hOBJECT engine,  // hENGINE
			struct KLAV_Pr_Detect_Info *pinfo
		);

////////////////////////////////////////////////////////////////
// Prague mutex implementation

class KLAV_Pr_Mutex : public KLAV_IFACE_IMPL(KLAV_Mutex)
{
public:
	KLAV_Pr_Mutex (hMUTEX impl, KLAV_Alloc* alloc);
	virtual ~KLAV_Pr_Mutex ();
	
	virtual KLAV_ERR KLAV_CALL lock ();
	virtual KLAV_ERR KLAV_CALL unlock ();
	virtual void KLAV_CALL destroy ();
	virtual void destroy_obsolete ();

private:
	hMUTEX       m_impl;
	KLAV_Alloc * m_alloc;
};

////////////////////////////////////////////////////////////////
// Prague RWLock implementation

class KLAV_Pr_RWLock : public KLAV_IFACE_IMPL(KLAV_RWLock)
{
public:
	KLAV_Pr_RWLock (hMUTEX impl, KLAV_Alloc* alloc);
	virtual ~KLAV_Pr_RWLock ();

	virtual KLAV_ERR KLAV_CALL lock_read ();
	virtual KLAV_ERR KLAV_CALL lock_write ();
	virtual KLAV_ERR KLAV_CALL unlock ();
	virtual void KLAV_CALL destroy ();

private:
	hMUTEX m_impl;
	KLAV_Alloc* m_alloc;
};

////////////////////////////////////////////////////////////////
// Prague synchronization object factory

class KLAV_Pr_Sync_Factory : public KLAV_IFACE_IMPL(KLAV_Sync_Factory)
{
public:
	KLAV_Pr_Sync_Factory (hOBJECT root, KLAV_Alloc* alloc);
	virtual ~KLAV_Pr_Sync_Factory ();
	
	virtual KLAV_Mutex  * KLAV_CALL create_mutex (int recursive);
	virtual KLAV_RWLock * KLAV_CALL create_rwlock ();
	virtual KLAV_CondVar * KLAV_CALL create_condvar ();

private:
	hOBJECT m_root;
	KLAV_Alloc* m_alloc;
};

////////////////////////////////////////////////////////////////
// Prague-based virtual memory interface

class KLAV_Pr_Virtual_Memory : public KLAV_IFACE_IMPL(KLAV_Virtual_Memory)
{
public:
	KLAV_Pr_Virtual_Memory ();
	virtual ~KLAV_Pr_Virtual_Memory ();

	virtual uint32_t KLAV_CALL vm_pagesize ();
	virtual KLAV_ERR KLAV_CALL vm_alloc (void **pptr, uint32_t size, uint32_t prot, void * pref_addr);
	virtual KLAV_ERR KLAV_CALL vm_free (void *ptr, uint32_t size);
	virtual KLAV_ERR KLAV_CALL vm_protect (void *ptr, uint32_t size, uint32_t newprot);
};

////////////////////////////////////////////////////////////////
// Prague-based KFB DBLoader

class KLAV_Pr_KFB_Loader : public KLAV_KFB_Loader
{
public:
		KLAV_Pr_KFB_Loader ();
		~KLAV_Pr_KFB_Loader ();

	KLAV_ERR open (hOBJECT hParent, const char *filepath);
	KLAV_ERR close ();

private:
	hOBJECT  m_hBuffer;

	KLAV_Pr_KFB_Loader (const KLAV_Pr_KFB_Loader&);
	KLAV_Pr_KFB_Loader& operator= (const KLAV_Pr_KFB_Loader&);
};

////////////////////////////////////////////////////////////////
// Prague-based procedure resolver

class KLAV_Pr_Proc_Resolver : public KLAV_IFACE_IMPL(KLAV_Proc_Resolver)
{
public:
	KLAV_Pr_Proc_Resolver (
			KLAV_Alloc *allocator,
			tPID        own_pid,
			KLAV_Proc_Resolver *ext_resolver = 0
		);

	virtual ~KLAV_Pr_Proc_Resolver ();

	void set_ext_resolver (KLAV_Proc_Resolver *ext_resolver);
	void set_own_pid (tPID pid);

	// unload all DLLs and PPLs
	void unload ();

	virtual KLAV_ERR KLAV_CALL resolve_procedure (
			KLAV_CONTEXT_TYPE,
			const KLAV_Proc_Info *proc,
			KLAV_PFN *res
		);

private:
	KLAV_Alloc         * m_allocator;
	KLAV_Proc_Resolver * m_ext_resolver;
	tPID                 m_own_pid;

	struct PPL_Info
	{
		tPID    m_pid;
		hPLUGIN m_plugin;

		PPL_Info (tPID pid = 0, hPLUGIN plugin = 0)
			: m_pid (pid), m_plugin (plugin) {}
	};

	typedef klavstl::trivial_vector <PPL_Info, klav_allocator> PPL_Info_Array;
	PPL_Info_Array m_ppls;

	KLAV_ERR register_ppl (tPID pid);

	KLAV_Pr_Proc_Resolver (const KLAV_Pr_Proc_Resolver&);
	KLAV_Pr_Proc_Resolver& operator= (const KLAV_Pr_Proc_Resolver&);
};

////////////////////////////////////////////////////////////////

#endif //__cplusplus

// special IO interface IDs

// get Prague hOBJECT interface
#define KLAV_IFIO_PR_HOBJECT  MAKE_KDBID('h','O','B','J')
// get Prague hIO interface
#define KLAV_IFIO_PR_HIO      MAKE_KDBID('h','I','O',0)

KLAV_ERR KLAV_CALL
KLAV_Pr_Create_IO_Object (
		hOBJECT parent,
		hIO hio,
		klav_bool_t owner,
		struct KLAV_IO_Object **ppobj
	);

KLAV_EXTERN_C
hKLAV_IO_Library KLAV_CALL
KLAV_Pr_IO_Library_Create (hOBJECT parent, struct KLAV_IO_Library_Create_Parms * parms);

KLAV_EXTERN_C
void KLAV_CALL
KLAV_Pr_IO_Library_Destroy (hKLAV_IO_Library hdl);

KLAV_EXTERN_C
hKLAV_Temp_Object_Manager KLAV_CALL
KLAV_Pr_Temp_Object_Manager_Create (hOBJECT parent);

KLAV_EXTERN_C
void KLAV_CALL
KLAV_Pr_Temp_Object_Manager_Destroy (hKLAV_Temp_Object_Manager h);

////////////////////////////////////////////////////////////////

#endif // klav_prague_h_INCLUDED


