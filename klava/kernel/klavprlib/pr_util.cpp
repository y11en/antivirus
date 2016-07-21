// prgutil.cpp
//

#include <klava/klav_prague.h>
#include <iface/i_engine.h>

tERROR PrStr::alloc (hOBJECT heapobj, uint32_t size)
{
	clear ();
	tERROR error = heapobj->heapAlloc ((tPTR *)&m_str, size);
	if (PR_FAIL (error)) m_str = 0; else m_heapobj = heapobj;
	return error;
}

void PrStr::clear ()
{
	if (m_str != 0)
	{
		m_heapobj->heapFree (m_str);
		m_str = 0;
	}
	m_heapobj = 0;
}

bool PrStr::assign (hOBJECT heapobj, const char *s)
{
	uint32_t len = (s == 0) ? 0 : (uint32_t) strlen (s);

	tERROR err = alloc (heapobj, len + 1);
	if (PR_FAIL (err))
		return false;

	if (len != 0)
		memcpy (m_str, s, len);

	m_str [len] = 0;
	return true;
}

tERROR PrStr::get_from_property (hOBJECT object, tPROPID propid, tCODEPAGE cp, hOBJECT heapobj)
{
	clear ();

	tDWORD size = 0;
	tERROR error = object->propGetStr (&size, propid, 0, 0, cp);
	if (PR_FAIL (error)) clear ();

	if (size == 0) return errOK;
	size = size + 1;

	error = alloc (heapobj, size);
	if (PR_FAIL (error)) return error;

	error = object->propGetStr (0, propid, m_str, size, cp);
	if (PR_FAIL (error)) clear ();

	return error;
}

tERROR PrStr::get_from_hstring (hSTRING object, tCODEPAGE cp, hOBJECT heapobj)
{
	clear ();

	tDWORD size = 0;
	tERROR error = object->LengthEx (&size, cSTRING_WHOLE, cp, cSTRING_Z);
	if (PR_FAIL (error)) clear ();

	if (size == 0) return errOK;
	size = size + 1;  // just in case...

	error = alloc (heapobj, size);
	if (PR_FAIL (error)) return error;

	error = object->ExportToBuff (0, cSTRING_WHOLE, m_str, size, cp, cSTRING_Z);
	if (PR_FAIL (error)) clear ();

	return error;
}

////////////////////////////////////////////////////////////////
// KLAV_Pr_Detect_Info

void KLAV_Pr_Detect_Info::clear ()
{
	m_detect_name.clear ();
	m_detect_type = 0;
	m_detect_danger = ENGINE_DETECT_DANGER_HIGH;
	m_detect_confidence = ENGINE_DETECT_CERTANITY_SURE;
	m_detect_curability = ENGINE_DETECT_DISINFECTABILITY_UNKNOWN;
}

tERROR KLAV_Pr_Get_Detect_Info (hOBJECT object, hOBJECT engine_obj, struct KLAV_Pr_Detect_Info *pinfo)
{
	tERROR error = errOK;

	if (object == 0 || engine_obj == 0 || pinfo == 0)
		return errPARAMETER_INVALID;

	pinfo->clear ();

	hENGINE engine = (hENGINE) engine_obj;

	error = pinfo->m_detect_name.alloc (object, KLAV_AVP_DETECT_NAME_MAXSIZE);
	if (PR_SUCC (error))
	{
		error = engine->GetAssociatedInfo (object, ENGINE_DETECT_NAME, KLAV_CP_UTF8, (char *)(pinfo->m_detect_name.c_str ()), KLAV_AVP_DETECT_NAME_MAXSIZE-1, 0);
		if (PR_FAIL (error)) pinfo->m_detect_name.clear ();
	}

	tERROR err = engine->GetAssociatedInfo (object, ENGINE_DETECT_DANGER, 0, &pinfo->m_detect_danger, sizeof(tDWORD), 0);
	if (PR_FAIL (err)) { pinfo->m_detect_danger = ENGINE_DETECT_DANGER_HIGH; error = err; }

	err = engine->GetAssociatedInfo (object, ENGINE_DETECT_TYPE, 0, &pinfo->m_detect_type, sizeof(tDWORD), 0);
	if (PR_FAIL (err)) pinfo->m_detect_type = 0;

	err = engine->GetAssociatedInfo (object, ENGINE_DETECT_CERTANITY, 0, &pinfo->m_detect_confidence, sizeof(tDWORD), 0);
	if (PR_FAIL (err)) pinfo->m_detect_confidence = ENGINE_DETECT_CERTANITY_SURE;

	err = engine->GetAssociatedInfo (object, ENGINE_DETECT_DISINFECTABILITY, 0, &pinfo->m_detect_curability, sizeof(tDWORD), 0);
	if (PR_FAIL (err)) pinfo->m_detect_curability = ENGINE_DETECT_DISINFECTABILITY_UNKNOWN;

	return err;
}


