// GuardFilter.cpp: implementation of the CGuardFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuardFilter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuardFilter::CGuardFilter ()
{
	m_BufferAlloc = NULL;

	m_HookID = 0;						
	m_FunctionMj = 0;							// 
	m_FunctionMi = 0;							// 
	m_ProcessingType = PreProcessing;
	m_Flags = FLT_A_DEFAULT;				// see FLT_A_???
	m_Timeout = DEADLOCKWDOG_TIMEOUT;		// timeout (request window life time)
	m_Expiration = 0;						// filter time life
	m_szProcName = "*";
	m_FilterID = 0;
    m_GroupID = 0;
}

CGuardFilter::CGuardFilter (DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, DWORD dwFlags)
{
	m_BufferAlloc = NULL;

	m_HookID = HookID;						
	m_FunctionMj = FunctionMj;							// 
	m_FunctionMi = FunctionMi;							// 
	m_ProcessingType = PreProcessing;
	m_Flags = dwFlags;
	m_Timeout = DEADLOCKWDOG_TIMEOUT;		// timeout (request window life time)
	m_Expiration = 0;						// filter time life
	m_szProcName = "*";
	m_FilterID = 0;
    m_GroupID = 0;
}

CGuardFilter::CGuardFilter (const CGuardFilter& filter)
{
	m_HookID = filter.m_HookID;								// 
	m_FunctionMj = filter.m_FunctionMj;
	m_FunctionMi = filter.m_FunctionMi;
	m_ProcessingType = filter.m_ProcessingType;		// see PROCESSING_TYPE
	m_Flags = filter.m_Flags;
	m_Timeout = filter.m_Timeout;
	m_Expiration = filter.m_Expiration;
	m_szProcName = filter.m_szProcName;
	m_FilterID = filter.m_FilterID;
    m_GroupID = filter.m_GroupID;

	m_BufferAlloc = NULL;

	RemoveAllParams ();

	for (PARAMLIST::const_iterator i = filter.m_lstParams.begin (); i != filter.m_lstParams.end (); i++)
	{
		if (*i != NULL)
		{
			size_t MemSize = sizeof (FILTER_PARAM) + (*i)->m_ParamSize;

			PFILTER_PARAM pFltParam = (PFILTER_PARAM)malloc (MemSize);
			
			if (pFltParam == NULL)
				break;

			memcpy (pFltParam, *i, MemSize);

			m_lstParams.insert (m_lstParams.end (), pFltParam);
		}	
	}
}

CGuardFilter::CGuardFilter (const PVOID pFilter)
{
	DRV_GetFilterInfo (
		pFilter, &m_FilterID, &m_Timeout, &m_Flags,
		&m_HookID, &m_FunctionMj, &m_FunctionMi,
		&m_Expiration, &m_ProcessingType
		);

	m_szProcName = "*";

    m_GroupID = 0;
	m_BufferAlloc = NULL;
	
	RemoveAllParams ();
	
	ULONG cou = 0;

	PFILTER_PARAM pSingleParam = DRV_GetFilterNextParam( pFilter, cou++ );
	while (pSingleParam)
	{
		size_t MemSize = 0;
		MemSize = sizeof (FILTER_PARAM) + pSingleParam -> m_ParamSize;
		PFILTER_PARAM pFltParam = (PFILTER_PARAM)malloc (MemSize);
		if (!pFltParam)
			break;
		
		memcpy (pFltParam, pSingleParam, MemSize);
		m_lstParams.insert (m_lstParams.end (), pFltParam);

		pSingleParam = DRV_GetFilterNextParam( pFilter, cou++ );
	}
}

CGuardFilter::~CGuardFilter ()
{
	if (m_BufferAlloc)
	{
		free (m_BufferAlloc);
		m_BufferAlloc = NULL;
	}

	RemoveAllParams ();
}


CGuardFilter& CGuardFilter::operator= (const CGuardFilter& filter)
{
	if (m_BufferAlloc)
	{
		free (m_BufferAlloc);
	}
	
	m_HookID = filter.m_HookID;								// 
	m_FunctionMj = filter.m_FunctionMj;
	m_FunctionMi = filter.m_FunctionMi;
	m_ProcessingType = filter.m_ProcessingType;		// see PROCESSING_TYPE
	m_Flags = filter.m_Flags;
	m_Timeout = filter.m_Timeout;
	m_Expiration = filter.m_Expiration;
	m_szProcName = filter.m_szProcName;
	m_FilterID = filter.m_FilterID;
    m_GroupID = filter.m_GroupID;

	m_BufferAlloc = NULL;

	RemoveAllParams ();

	for (PARAMLIST::const_iterator i = filter.m_lstParams.begin (); i != filter.m_lstParams.end (); i++)
	{
		if (*i != NULL)
		{
			size_t MemSize = sizeof (FILTER_PARAM) + (*i)->m_ParamSize;

			PFILTER_PARAM pFltParam = (PFILTER_PARAM)malloc (MemSize);
			
			if (pFltParam == NULL)
				break;

			memcpy (pFltParam, *i, MemSize);

			m_lstParams.insert (m_lstParams.end (), pFltParam);
		}	
	}

	return *this;
	
}


ppPARAM CGuardFilter::AddParam (PFILTER_PARAM pParam)
{
	if (m_BufferAlloc)
	{
		return ppPARAM();
	}
	
	return AddParamInternal (pParam);
	
}

ppPARAM CGuardFilter::AddParam (ULONG uParamID, FLT_PARAM_OPERATION ParamFlt, ULONG uParamSize, LPVOID pParamValue, DWORD dwParamFlags /* = _FILTER_PARAM_FLAG_MUSTEXIST*/)
{
	if (m_BufferAlloc)
	{
		return ppPARAM();
	}
	
	return AddParamInternal (uParamID, ParamFlt, uParamSize, pParamValue, dwParamFlags);
}

PFILTER_PARAM CGuardFilter::FindParam (ULONG uParamID) const
{

	for (PARAMLIST::const_iterator i = m_lstParams.begin (); i != m_lstParams.end (); i ++)
	{
		if ((*i) -> m_ParamID == uParamID)
		{
			return (*i);
		}
	}

	return NULL;

}

bool CGuardFilter::RemoveParam (PARAMLIST::iterator iter)
{
	return false;
}

PFILTER_TRANSMIT CGuardFilter::AllocateFilter () const
{
	if (m_BufferAlloc)
	{
		return NULL;
	}

	PFILTER_TRANSMIT pFltTransmit = NULL;
	size_t	ParamsSize = 0;
	PARAMLIST::const_iterator i;
	BYTE* pPointer = NULL;

	if (m_szProcName.size () >= PROCNAMELEN)
	{
		return NULL;
	}

	pFltTransmit = (PFILTER_TRANSMIT)malloc (GetFilterSize ());

	if (NULL == pFltTransmit)
	{
		assert (FALSE);
		return NULL;
	}

	pFltTransmit->m_FltCtl			= 0;
	pFltTransmit->m_AppID			= 0;
	pFltTransmit->m_FilterID		= m_FilterID;
	pFltTransmit->m_HookID			= m_HookID;
	pFltTransmit->m_FunctionMj		= m_FunctionMj;
	pFltTransmit->m_FunctionMi		= m_FunctionMi;
	pFltTransmit->m_ProcessingType	= m_ProcessingType;
	pFltTransmit->m_Flags			= m_Flags;
	pFltTransmit->m_Timeout			= m_Timeout;
	pFltTransmit->m_Expiration		= m_Expiration;

	strcpy (pFltTransmit->m_ProcName, m_szProcName.c_str ());
	
	pFltTransmit->m_ParamsCount = GetParamCount ();

	pPointer = pFltTransmit->m_Params;

	for (i = m_lstParams.begin (); i != m_lstParams.end (); ++i)
	{
		if (NULL == *i)
		{
			assert (FALSE);
			return NULL;
		}
		else
		{
			ParamsSize = sizeof (FILTER_PARAM) + (*i)->m_ParamSize;

			memcpy (pPointer, *i, ParamsSize);

			pPointer += ParamsSize;
		}	
	}
	
	return pFltTransmit;	
}

size_t CGuardFilter::GetFilterSize () const
{
	if (m_BufferAlloc)
	{
		return 0;
	}

	size_t ParamsSize = 0;

	for (PARAMLIST::const_iterator i = m_lstParams.begin (); i != m_lstParams.end (); ++i)
	{
		if (NULL == *i)
		{
			assert (FALSE);
			return 0;
		}
		else
		{			
			ParamsSize += sizeof (FILTER_PARAM) + (*i)->m_ParamSize;
		}	
	}

	return (sizeof (FILTER_TRANSMIT) + ParamsSize);	
}

int CGuardFilter::GetParamCount () const
{
	if (m_BufferAlloc)
	{
		return NULL;
	}

	int iParamCount = 0;

	for (PARAMLIST::const_iterator i = m_lstParams.begin (); i != m_lstParams.end (); ++i)
	{
		if (NULL == *i)
		{
			assert (FALSE);
			return NULL;
		}
		else
		{			
			++ iParamCount;
		}	
	}

	return iParamCount;
}

PFILTER_TRANSMIT CGuardFilter::GetInternalBuffer (DWORD dwBufSize)
{
	if (m_BufferAlloc)
	{
		return NULL;
	}

	RemoveAllParams ();

	m_BufferAlloc = (PFILTER_TRANSMIT)calloc (dwBufSize, 1);

	return m_BufferAlloc;
}

void CGuardFilter::ReleaseInternalBuffer ()
{
	if (!m_BufferAlloc)
	{
		return;
	}

	RemoveAllParams ();

	m_HookID			= m_BufferAlloc->m_HookID;						
	m_FunctionMj		= m_BufferAlloc->m_FunctionMj;			
	m_FunctionMi		= m_BufferAlloc->m_FunctionMi;			
	m_ProcessingType	= m_BufferAlloc->m_ProcessingType;
	m_Flags				= m_BufferAlloc->m_Flags;
	m_Timeout			= m_BufferAlloc->m_Timeout;				
	m_Expiration		= m_BufferAlloc->m_Expiration;
	m_szProcName		= m_BufferAlloc->m_ProcName;

	m_FilterID			= m_BufferAlloc->m_FilterID;

	AddParamsFromFilterTransmit (m_BufferAlloc);

	free (m_BufferAlloc);

	m_BufferAlloc = NULL;
}

void CGuardFilter::RemoveAllParams ()
{
	for (ppPARAM i = m_lstParams.begin (); i != m_lstParams.end (); i++)
	{
		if (*i != NULL)
		{
			free (*i);
		}	
	}

	m_lstParams.clear ();
}

ppPARAM CGuardFilter::AddParamInternal (PFILTER_PARAM pParam)
{
	if (NULL == pParam)
	{
		assert (FALSE);
		return ppPARAM();
	}

	if ((0 == pParam->m_ParamID) || (NULL == pParam->m_ParamValue) || (0 == pParam->m_ParamSize))
	{
		assert (FALSE);
		return ppPARAM();
	}

	size_t MemSize = sizeof (FILTER_PARAM) + pParam->m_ParamSize;

	PFILTER_PARAM pFltParam = (PFILTER_PARAM)malloc (MemSize);
	
	assert (pFltParam);

	memcpy (pFltParam, pParam, MemSize);

	m_lstParams.insert (m_lstParams.end (), pFltParam);

	return m_lstParams.end ();
}

int CGuardFilter::AddParamsFromFilterTransmit (PFILTER_TRANSMIT pFlt)
{
	if (!pFlt)
	{
		assert (FALSE);
		return 0;
	}

	BYTE* pParam = pFlt->m_Params;

	for (ULONG i = 0; i < pFlt->m_ParamsCount; ++i)
	{
		AddParamInternal ((PFILTER_PARAM)pParam);
		pParam += sizeof (FILTER_PARAM) + ((PFILTER_PARAM)pParam)->m_ParamSize;
	}

	return pFlt->m_ParamsCount;

}

const PARAMLIST& CGuardFilter::GetParamList () const
{
	assert (!IsBufferLocked ());

	return m_lstParams;
}

BOOL CGuardFilter::IsBufferLocked () const
{
	return (m_BufferAlloc != NULL);
}

ppPARAM CGuardFilter::AddParamInternal (ULONG uParamID, FLT_PARAM_OPERATION ParamFlt, ULONG uParamSize, LPVOID pParamValue, DWORD dwParamFlags)
{
	size_t MemSize = sizeof (FILTER_PARAM) + uParamSize;
	
	PFILTER_PARAM pFltParam = (PFILTER_PARAM)malloc (MemSize);
	
	if (!pFltParam)
	{
		assert (FALSE);
		return ppPARAM();
	}
	
	pFltParam->m_ParamID = uParamID;
	pFltParam->m_ParamFlt = ParamFlt;
	pFltParam->m_ParamSize = uParamSize;
	pFltParam->m_ParamFlags = dwParamFlags;
	
	memcpy (pFltParam->m_ParamValue, pParamValue, uParamSize);
	
	m_lstParams.insert (m_lstParams.end (), pFltParam);
	
	return m_lstParams.end ();
	
}

void CGuardFilter::Enable ()
{
	m_Flags &= (~FLT_A_DISABLED);
}

void CGuardFilter::Disable ()
{
	m_Flags |= FLT_A_DISABLED;
}

void CGuardFilter::SetGroupID (ULONG GroupID)
{
    m_GroupID = GroupID;
}

ULONG CGuardFilter::GetGroupID ()
{
    return m_GroupID;
}
