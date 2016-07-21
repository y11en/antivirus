// dso_proc_resolver.cpp
//
// Default KLAV_Proc_Resolver implementation
// handles DLL-exported functions, if DSO interface is available
//

#include <klava/klaveng.h>
#include <klava/klav_utils.h>
#include <klava/klavstl/string.h>

////////////////////////////////////////////////////////////////

struct KLAV_DSO_Proc_Resolver : public KLAV_IFACE_IMPL(KLAV_Proc_Resolver)
{
public:
         KLAV_DSO_Proc_Resolver (
				KLAV_Alloc *alloc,
				KLAV_DSO_Loader * dso_loader,
				KLAV_Proc_Resolver * ext_resolver = 0
			);

	virtual ~KLAV_DSO_Proc_Resolver ();

	virtual KLAV_ERR KLAV_CALL resolve_procedure (
			KLAV_CONTEXT_TYPE,
			const KLAV_Proc_Info *proc,
			KLAV_PFN *res
		);

	void set_dso_loader (KLAV_DSO_Loader *ldr);
	void set_ext_resolver (KLAV_Proc_Resolver *ext);
	void unload ();

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

private:
	struct DSO
	{
		klav_string  m_name;
		void *       m_hdl;
		
		DSO ()
			: m_hdl (0) {}

		DSO (const char *name, KLAV_Alloc *alloc, void *hdl)
			: m_name (name, alloc), m_hdl (hdl) {}
	};

	KLAV_Alloc         * m_allocator;
	KLAV_DSO_Loader    * m_dso_loader;
	KLAV_Proc_Resolver * m_ext_resolver;
	typedef klavstl::trivial_vector <DSO, klav_allocator> DSO_Array;

	DSO_Array m_loaded_dso;
};

////////////////////////////////////////////////////////////////

KLAV_DSO_Proc_Resolver::KLAV_DSO_Proc_Resolver (
			KLAV_Alloc *alloc,
			KLAV_DSO_Loader *dso_loader,
			KLAV_Proc_Resolver *ext_resolver
		) :
		m_allocator (alloc),
		m_dso_loader (dso_loader),
		m_ext_resolver (ext_resolver),
		m_loaded_dso (alloc)
{
}

KLAV_DSO_Proc_Resolver::~KLAV_DSO_Proc_Resolver ()
{
	unload ();
}

KLAV_ERR KLAV_CALL KLAV_DSO_Proc_Resolver::resolve_procedure (
			KLAV_CONTEXT_TYPE ctx,
			const KLAV_Proc_Info *proc_info,
			KLAV_PFN *res
		)
{
	*res = 0;

	if (proc_info->type != KLAV_PROCTYPE_EXPORT)
	{
		if (m_ext_resolver != 0)
			return m_ext_resolver->resolve_procedure (ctx, proc_info, res);

		KLAV_CONTEXT_TRACE(ctx, (KLAV_TRACE_LEVEL_ERROR, "ProcResolver: invalid procedure type: %d", proc_info->type));

		return KLAV_ENOTFOUND;
	}

	const char * lib_name = proc_info->parm1.str;
	const char * proc_name = proc_info->parm2.str;

	void * hlib = 0;

	size_t i, cnt = m_loaded_dso.size ();
	for (i = 0; i < cnt; ++i)
	{
		if (strcmp (m_loaded_dso [i].m_name.c_str (), lib_name) == 0)
		{
			hlib = m_loaded_dso [i].m_hdl;
			break;
		}
	}

	if (hlib == 0)
	{
		if (m_dso_loader == 0)
		{
			// DSO loader unavailable;
			return KLAV_ENOINIT;
		}

		klavstl::string <klav_allocator> dso_name (m_allocator);
		dso_name = lib_name;
		// if extension is missing, append default DSO extension
		size_t fn_off = KLAV_Fname_Ext_Offset (dso_name.c_str ());
		if (fn_off == dso_name.length ())
			dso_name += KLAV_DSO_SUFFIX;

		KLAV_ERR err = m_dso_loader->dl_open (dso_name.c_str (), &hlib);
		if (KLAV_FAILED (err))
		{
			KLAV_CONTEXT_TRACE(ctx, (KLAV_TRACE_LEVEL_ERROR, "ProcResolver: cannot load module:%s", lib_name));
			return KLAV_ENOTFOUND;
		}

		m_loaded_dso.push_back (DSO (lib_name, m_allocator, hlib));
	}

	*res = (KLAV_PFN) m_dso_loader->dl_sym (hlib, proc_name);
	if (*res == 0)
	{
		KLAV_CONTEXT_TRACE(ctx, (KLAV_TRACE_LEVEL_ERROR, "ProcResolver: cannot find symbol:%s in library:%s", proc_name, lib_name));
		return KLAV_ENOTFOUND;
	}

    return KLAV_OK;
}

void KLAV_DSO_Proc_Resolver::set_dso_loader (KLAV_DSO_Loader *ldr)
{
	m_dso_loader = ldr;
}

void KLAV_DSO_Proc_Resolver::set_ext_resolver (KLAV_Proc_Resolver *ext)
{
	m_ext_resolver = ext;
}

void KLAV_DSO_Proc_Resolver::unload ()
{
	if (m_dso_loader != 0)
	{
		size_t i, cnt = m_loaded_dso.size ();
		for (i = 0; i < cnt; ++i)
		{
			void *hdl = m_loaded_dso [i].m_hdl;
			if (hdl != 0)
			{
				m_dso_loader->dl_close (hdl);
			}
		}
	}

	m_loaded_dso.clear ();
}

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
hKLAV_Proc_Resolver KLAV_CALL KLAV_Create_DSO_Proc_Resolver (
			hKLAV_Alloc      alloc,
			hKLAV_DSO_Loader dso_loader,
			hKLAV_Proc_Resolver chain
		)
{
	KLAV_DSO_Proc_Resolver * impl =
		KLAV_NEW (alloc) KLAV_DSO_Proc_Resolver (alloc, dso_loader, chain);

	return impl;
}

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_Destroy_DSO_Proc_Resolver (
			hKLAV_Proc_Resolver obj
		)
{
	if (obj == 0)
		return KLAV_ENOINIT;

	KLAV_DSO_Proc_Resolver *impl = static_cast <KLAV_DSO_Proc_Resolver *> (obj);
	KLAV_DELETE (impl, impl->allocator ());

	return KLAV_OK;
}

