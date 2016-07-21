#include <kl_byteswap.h>

#include "../eventmgr/include/eventhandler.h"
#include "../heuristic/heur_av.h"

#include "heur_dumb.h"

bool cEventHandlerHeurDumb::Init()
{
	if (!m_pProcessList->AllocSlot(&m_slot_index))
		return false;

	return true;
}

struct ControlledSzValue
{
	tcstring key_name;
	tcstring val_name;
	tcstring data;
};

struct ControlledDwValue
{
	tcstring key_name;
	tcstring val_name;
	uint32_t data;
};

static const ControlledSzValue arrForbiddenSzValues[] =
{
	{
		_T("SYSTEM\\CONTROLSET???\\SERVICES\\SHAREDACCESS\\PARAMETERS\\FIREWALLPOLICY\\*PROFILE\\AUTHORIZEDAPPLICATIONS\\LIST"),
		NULL,
		_T("*:ENABLED:*")
	},
	{
		_T("SYSTEM\\CURRENTCONTROLSET\\SERVICES\\SHAREDACCESS\\PARAMETERS\\FIREWALLPOLICY\\*PROFILE\\AUTHORIZEDAPPLICATIONS\\LIST"),
		NULL,
		_T("*:ENABLED:*")
	},
	{
		_T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\DOWNLOAD"),
		_T("CHECKEXESIGNATURES"),
		_T("NO")
	},
};

#define NOT_NUL 0xFEFEFEFE

static const ControlledDwValue arrForbiddenDwValues[] =
{
	{
		_T("SOFTWARE\\POLICIES\\MICROSOFT\\WINDOWSFIREWALL\\*PROFILE"),
		_T("ENABLEFIREWALL"),
		0
	},
	{
		_T("SYSTEM\\CONTROLSET???\\SERVICES\\SHAREDACCESS\\PARAMETERS\\FIREWALLPOLICY\\*PROFILE"),
		_T("ENABLEFIREWALL"),
		0
	},
	{
		_T("SYSTEM\\CURRENTCONTROLSET\\SERVICES\\SHAREDACCESS\\PARAMETERS\\FIREWALLPOLICY\\*PROFILE"),
		_T("ENABLEFIREWALL"),
		0
	},
	{
		_T("SOFTWARE\\MICROSOFT\\SECURITY CENTER"),
		_T("ANTIVIRUSDISABLENOTIFY"),
		NOT_NUL
	},
	{
		_T("SOFTWARE\\MICROSOFT\\SECURITY CENTER"),
		_T("FIREWALLDISABLENOTIFY"),
		NOT_NUL
	},
	{
		_T("SYSTEM\\CONTROLSET???\\SERVICES\\WSCSVC"),
		_T("START"),
		4
	},
	{
		_T("SYSTEM\\CURRENTCONTROLSET\\SERVICES\\WSCSVC"),
		_T("START"),
		4
	},
	{
		_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\POLICIES\\SYSTEM"),
		_T("DISABLE*"),
		NOT_NUL
	},
	{
		_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\INTERNET SETTINGS\\ZONES\\3"),
		_T("1001"),
		0
	},
	{
		_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\INTERNET SETTINGS\\ZONES\\3"),
		_T("1004"),
		0
	},
	{
		_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\INTERNET SETTINGS\\ZONES\\3"),
		_T("1201"),
		0
	},
	{
		_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\INTERNET SETTINGS\\ZONES\\3"),
		_T("2200"),
		0
	},
	{
		_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\INTERNET SETTINGS\\ZONES\\3"),
		_T("2201"),
		0
	},
	{
		_T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\DOWNLOAD"),
		_T("RUNINVALIDSIGNATURES"),
		NOT_NUL
	},
};

void cEventHandlerHeurDumb::OnRegValueSetPre(cEvent& event, const cRegKey& key, tcstring sValueName, 
												 uint32_t nNewType, const void* pNewData, size_t nNewDataSize, 
												 uint32_t nValueType, const void* pValueData, size_t nValueDataSize)
{
	assert(pNewData);
	for (int i=0; i<countof(arrForbiddenDwValues); i++)
	{
		if (!arrForbiddenDwValues[i].val_name || (sValueName && tstrMatchWithPattern(sValueName, arrForbiddenDwValues[i].val_name, false)))
		{
			if (!arrForbiddenDwValues[i].key_name || tstrMatchWithPattern(key.m_name, arrForbiddenDwValues[i].key_name, false))
			{
				uint32_t new_value;
				if (nNewDataSize >= 4)
				{
					new_value = kl_get_unaligned_32_le(pNewData);
				}
				else
				{
					new_value = 0;
					for (uint32_t i=0; i<4 && i<nNewDataSize; i++)
						new_value |= ((uint32_t)((uint8_t*)pNewData)[i]) << i*8;
				}
				if (arrForbiddenDwValues[i].data == new_value || (arrForbiddenDwValues[i].data == NOT_NUL && new_value!=0))
					return m_pEventMgr->OnSecurityViolation(event, 100);
			}
		}
	}

	return;
}

void cEventHandlerHeurDumb::OnRegValueSetStrPre(cEvent& event, const cRegKey& key, tcstring sValueName, 
													tcstring sNewData, 
													tcstring sValueData)
{
	for (int i=0; i<countof(arrForbiddenSzValues); i++)
	{
		if (!arrForbiddenSzValues[i].key_name || tstrMatchWithPattern(key.m_name, arrForbiddenSzValues[i].key_name, false))
			if (!arrForbiddenSzValues[i].val_name || (sValueName && tstrMatchWithPattern(sValueName, arrForbiddenSzValues[i].val_name, false)))
				if (tstrMatchWithPattern(sNewData, arrForbiddenSzValues[i].data, false))
					return m_pEventMgr->OnSecurityViolation(event, 80);
	}

	if (sValueName != NULL && 0 == tstricmp(sValueName, _T("START PAGE"))
		&& 0 == tstrcmp(key.m_name, _T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\MAIN"))
		&& sNewData != NULL && 0 != tstricmp(sNewData, _T("ABOUT:BLANK")))
	{
		return m_pEventMgr->OnIEStartPage(event,  sNewData);
	}

	return;
}

void cEventHandlerHeurDumb::OnFindWindowFail(cEvent& event, tcstring sClassName, tcstring sTitle)
{
	if (!sClassName || tstrncmp(sClassName, _T("AVP."), 4) != 0)
		return;

	uint32_t counter = 0;

	{
		cProcess pProcess = event.GetProcess();
		if (!pProcess)
			return;

		uint32_t * pData = (uint32_t *) pProcess->GetSlotData(m_slot_index, sizeof(uint32_t));
		if (!pData)
			return;

		counter = ++*pData;
	}

	if (counter > 10)
		m_pEventMgr->OnAntiAV(event, avKaspersky);
}
