// pr_proc_ldr.cpp
//
// Prague-based procedure resolver
//

#include <klava/klav_prague.h>

KLAV_Pr_Proc_Resolver::KLAV_Pr_Proc_Resolver (
		KLAV_Alloc *allocator,
		tPID own_pid,
		KLAV_Proc_Resolver *ext_resolver
	)
	: m_allocator (allocator),
	  m_ext_resolver (ext_resolver),
	  m_own_pid (own_pid),
	  m_ppls (allocator)
{
}

KLAV_Pr_Proc_Resolver::~KLAV_Pr_Proc_Resolver ()
{
	unload ();
}

void KLAV_Pr_Proc_Resolver::set_ext_resolver (KLAV_Proc_Resolver *ext_resolver)
{
	m_ext_resolver = ext_resolver;
}

void KLAV_Pr_Proc_Resolver::set_own_pid (tPID pid)
{
	m_own_pid = pid;
}

KLAV_ERR KLAV_CALL KLAV_Pr_Proc_Resolver::resolve_procedure (
			KLAV_CONTEXT_TYPE ctx,
			const KLAV_Proc_Info *proc_info,
			KLAV_PFN *res
		)
{
	KLAV_ERR err = KLAV_OK;
	*res = 0;

	if (proc_info->type != KLAV_PROCTYPE_PRAGUE)
	{
		if (m_ext_resolver != 0)
			return m_ext_resolver->resolve_procedure (ctx, proc_info, res);

		KLAV_CONTEXT_TRACE (ctx, (KLAV_TRACE_LEVEL_ERROR, "ProcResolver: invalid procedure type: %d", proc_info->type));
		return KLAV_ENOTFOUND;
	}

	tDWORD pid    = proc_info->parm1.num;
	tDWORD procID = proc_info->parm2.num;

	g_root->ResolveImportFunc((tFUNC_PTR *)res, pid, procID, m_own_pid);
	if (*res == 0)
	{
		KLAV_CONTEXT_TRACE (ctx, (KLAV_TRACE_LEVEL_ERROR, "Can't resolve Prague export: 0x%08x", procID));
		return KLAV_ENOTFOUND;
	}

	err = register_ppl (pid);

	g_root->ReleaseImportFunc (pid, procID, m_own_pid);

	return err;
}

void KLAV_Pr_Proc_Resolver::unload ()
{
	size_t i;

	for (i = m_ppls.size (); i > 0; --i)
	{
		PPL_Info& info = m_ppls [i - 1];
		info.m_plugin->Unload ();
	}
	m_ppls.resize (0);
}

KLAV_ERR KLAV_Pr_Proc_Resolver::register_ppl (tPID pid)
{
	size_t i, cnt = m_ppls.size ();

	for (i = 0; i < cnt; ++i)
	{
		const PPL_Info& ppl_info = m_ppls [i];
		if (ppl_info.m_pid == pid)
			return KLAV_OK;
	}

	hPLUGIN hPlugin = 0;
	tERROR error = g_root->GetPluginInstance (&hPlugin, 0, pid);
	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	error = hPlugin->Load ();
	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	if (m_ppls.push_back (PPL_Info (pid, hPlugin)) == 0)
	{
		hPlugin->Unload ();
		return KLAV_ENOMEM;
	}

	return KLAV_OK;
}

