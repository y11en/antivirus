// prgcomp.cpp
//
// KLAV_Pr_Component, KlavPragueSyncFactory and related functions
//

#include <klav_prague.h>
#include <formatting.h>

////////////////////////////////////////////////////////////////
// KLAV_Pr_Component

#define KLAV_PRAGUE_COMPONENT_KEY_BUFLEN 80

static void format_prague_component_key (uint32_t pid, uint32_t iid, char *keybuf)
{
	kl_sprintf (keybuf, "PrComp:%08X:%08X", pid, iid);
}

KLAV_Pr_Component::KLAV_Pr_Component (const InitParms& is)
	: KLAV_Component_Impl (0, is.m_fragment_id, is.m_flags),
	  m_hObject (is.m_hObject)
{
	KLAV_CONTEXT_VAR (is.m_context);

	m_keybuf.assign (is.m_key, GetKlavKernelAlloc ());
	m_key = m_keybuf.c_str ();
}

KLAV_Pr_Component::~KLAV_Pr_Component ()
{
	if (m_hObject != 0)
	{
		m_hObject->sysCloseObject ();
		m_hObject = 0;
	}
}

KLAV_ERR KLAV_CALL KLAV_Pr_Component::on_component_load (KLAV_CONTEXT_ARG, KLAV_Properties *props)
{
	tERROR err = m_hObject->Load(KLAV_CONTEXT);
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_Component::on_component_unload (KLAV_CONTEXT_ARG)
{
	tERROR err = m_hObject->Unload(KLAV_CONTEXT);
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_Component::on_context_create (KLAV_CONTEXT_ARG, KLAV_Properties *props)
{
	tERROR err = m_hObject->InitInstance(KLAV_CONTEXT);
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_Component::on_context_destroy (KLAV_CONTEXT_ARG)
{
	tERROR err = m_hObject->DeinitInstance(KLAV_CONTEXT);
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_Component::on_context_activate (KLAV_CONTEXT_ARG, KLAV_Properties *props)
{
	tERROR err = m_hObject->ActivateInstance(KLAV_CONTEXT);
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_Component::on_context_deactivate (KLAV_CONTEXT_ARG)
{
	tERROR err = m_hObject->DeactivateInstance(KLAV_CONTEXT);
	if (PR_FAIL(err))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_Component::on_context_flush (KLAV_CONTEXT_ARG)
{
	return KLAV_OK;
}

////////////////////////////////////////////////////////////////
// KLAV_Pr_Component_Factory
/*
KLAV_Pr_Component_Factory::KLAV_Pr_Component_Factory (
		hOBJECT  hParent,
		uint32_t pid,
		uint32_t iid
	) : m_hParent (hParent), m_pid (pid), m_iid (iid)
{
}

KLAV_Pr_Component_Factory::~KLAV_Pr_Component_Factory ()
{
}

KLAV_Component * KLAV_CALL KLAV_Pr_Component_Factory::create_component (
		KLAV_CONTEXT_ARG,
		const char *key,
		uint32_t fragment_id
	)
{
	if (m_hParent == 0)
		return 0;

	hKLAVCOMPONENT comp = 0;
	tERROR err = m_hParent->sysCreateObject((hOBJECT *)&comp, m_iid, m_pid);

	if (PR_SUCC (err))
	{
		err = comp->propSetUInt(pgKLAV_COMPONENT_FRAGMENT_ID, fragment_id);
	}

	if (PR_SUCC(err))
	{
		err = comp->sysCreateObjectDone();
	}

	if (PR_FAIL(err))
	{
		KLAV_TRACE (KLAV_TRACE_LEVEL_ERROR, "Error creating Prague component (PID:%d,IID:%d), error 0x%x", m_pid, m_iid, err);
		comp->sysCloseObject();
		return 0;
	}

	KLAV_Pr_Component::InitParms is;
	is.m_hObject = comp;
	is.m_context = KLAV_CONTEXT;
	is.m_key = key;
	is.m_fragment_id = fragment_id;
	is.m_flags = comp->get_pgKLAV_COMPONENT_FLAGS()
	   | KLAV_COMPONENT_AUTO_DESTROY | comp->get_pgKLAV_COMPONENT_ORDER();

	KLAV_Alloc *alloc = GetKlavKernelAlloc ();

	KLAV_Pr_Component* klavComp = KLAV_NEW(alloc) KLAV_Dyn_Object <KLAV_Pr_Component> (alloc, is);
	if (! klavComp)
	{
		comp->sysCloseObject();
		return 0;
	}

	return klavComp;
}

////////////////////////////////////////////////////////////////
// API

KLAV_ERR KLAV_Pr_Component_Factory::get_component (
			KLAV_CONTEXT_ARG,
			uint32_t pid,
			uint32_t iid,
			uint32_t fragment_id,
			hOBJECT *phcomp
		)
{
	if (phcomp != 0)
		*phcomp = 0;

	char keybuf [KLAV_PRAGUE_COMPONENT_KEY_BUFLEN];
	format_prague_component_key (pid, iid, keybuf);

	hOBJECT hEngine = KlavGetEngine ();
	KLAV_Pr_Component_Factory factory (hEngine, pid, iid);

	KLAV_Component *comp = KLAV_KERNEL_IF->get_component (keybuf, fragment_id, &factory);
	if (comp == 0)
		return KLAV_EUNKNOWN;

	if (phcomp != 0)
	{
		KLAV_Pr_Component *prcomp = static_cast<KLAV_Pr_Component *>(comp);
		*phcomp = prcomp->get_object ();
	}

	return KLAV_OK;
}

KLAV_PROC_INTERNAL (KLAV_ERR, KlavGetPragueComponentImpl) (
		KLAV_CONTEXT_ARG, 
		tPID pid,
		tIID iid, 
		uint32_t fragment_id, 
		hOBJECT *phComponent
	)
{
	return KLAV_Pr_Component_Factory::get_component (
			KLAV_CONTEXT,
			pid,
			iid,
			fragment_id,
			phComponent
		);
}
*/
