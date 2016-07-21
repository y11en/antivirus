#include "heur_hosts.h"
#include "heur_av.h"

#define LCASE(c) (c>='A'? (c<='Z'? (c|0x20) : c) : c)

cEventHandlerHeurHosts::cEventHandlerHeurHosts()
{
	m_pHosts = m_pHostsParsed = 0;
	m_nHostsSize = m_nHostsSizeParsed = 0;
}

cEventHandlerHeurHosts::~cEventHandlerHeurHosts()
{
	if (m_pHosts)
		m_pEnvironmentHelper->Free(m_pHosts);
	if (m_pHostsParsed)
		m_pEnvironmentHelper->Free(m_pHostsParsed);
}

bool cEventHandlerHeurHosts::Init()
{
	GatherHostsFile(&m_pHosts, &m_nHostsSize, &m_pHostsParsed, &m_nHostsSizeParsed);
	return true;
}

void cEventHandlerHeurHosts::OnFileCloseModifiedPost(cEvent& event, cFile& file)
{
	char*  pNewHosts;
	size_t nNewHostsSize;
	char*  pNewHostsParsed;
	size_t nNewHostsSizeParsed;

	if (!tstrMatchWithPattern(file.getFull(), _T("*\\SYSTEM32\\DRIVERS\\ETC\\HOSTS"), false))
		return;

	if (!GatherHostsFile(&pNewHosts, &nNewHostsSize, &pNewHostsParsed, &nNewHostsSizeParsed))
		return;
	
	cAutoLock lock(m_lock);
	
	for (size_t i = 0; i < countof(arrAntiVirusDomains); i++)
	{
		if (tstristrA(pNewHostsParsed, arrAntiVirusDomains[i].name))	// found arrAntiVirusDomains[i] in New
		{
			if (!m_pHostsParsed || !tstristrA(m_pHostsParsed, arrAntiVirusDomains[i].name)) // not found arrAntiVirusDomains[i] in Old
			{
				m_pEventMgr->OnHostsAdded(event, arrAntiVirusDomains[i].name);
			}
		}
	}
	
	// replace with new version
	if (m_pHosts)
		m_pEnvironmentHelper->Free(m_pHosts);
	if (m_pHostsParsed)
		m_pEnvironmentHelper->Free(m_pHostsParsed);
	m_pHosts = pNewHosts;
	m_nHostsSize = nNewHostsSize;
	m_pHostsParsed = pNewHostsParsed;
	m_nHostsSizeParsed = nNewHostsSizeParsed;

	return;
}

void cEventHandlerHeurHosts::OnHostsAdded(cEvent& event, const char* domen)
{
	for (size_t i = 0; i < countof(arrAntiVirusDomains); i++)
	{
		if (tstristrA(domen, arrAntiVirusDomains[i].name))	// found arrAntiVirusDomains[i] in domen
		{
			return m_pEventMgr->OnAntiAV(event, arrAntiVirusDomains[i].id);
		}
	}
	return;
}

bool cEventHandlerHeurHosts::GatherHostsFile(char** ppHosts, size_t* pHostsSize, char** ppHostsParsed, size_t* pHostsSizeParsed)
{
	uint64_t nHostsSize64;
	size_t nHostsSize;
	cAutoEnvMem<char>  pHosts(m_pEnvironmentHelper);

	*ppHosts = *ppHostsParsed = 0;
	*pHostsSize = *pHostsSizeParsed = 0;

	cFile hosts_file(m_pEnvironmentHelper, _T("%systemroot%\\system32\\drivers\\etc\\hosts"));

	nHostsSize64 = hosts_file.Size();
	if (nHostsSize64 == 0)
		return false;
	if (nHostsSize64 > 64*1024)
		return false;
	nHostsSize = (size_t)nHostsSize64;

	pHosts.Alloc(nHostsSize + 1);	//+1 for NULL terminating
	if (!pHosts)
		return false;

	uint32_t nBytesRead;
	if (false == hosts_file.Read(0, pHosts, (uint32_t)nHostsSize, &nBytesRead))
		return false;
	nHostsSize = nBytesRead;

	if (!ParseHostsFile(pHosts, nHostsSize, NULL, pHostsSizeParsed))
		return false;
	if (!*pHostsSizeParsed)
		return false;

	cAutoEnvMem<char>  pHostsParased(m_pEnvironmentHelper, *pHostsSizeParsed);
	if (!pHostsParased)
		return false;
	
	if (!ParseHostsFile(pHosts, nHostsSize, pHostsParased, pHostsSizeParsed))
		return false;

	*ppHosts = pHosts.relinquish();
	*pHostsSize = nHostsSize;
	*ppHostsParsed = pHostsParased.relinquish();

	return true;
}

bool cEventHandlerHeurHosts::ParseHostsFile(char* pBuffer, size_t dwBufferSize, char* pParsedBuffer, size_t* pdwParseSize)
{
	if (!pBuffer || !dwBufferSize)
		return false;

	unsigned int ParseSize = 0;
	unsigned int Pos = 0;

	char curChar;
	bool bSkipLine = false;

	while (Pos < dwBufferSize)
	{
		//skip remark and spaces
		curChar = pBuffer[Pos++];
		if (curChar == '\r' || curChar == '\n')
		{
			bSkipLine = false;
			continue;
		}

		if (curChar == ' ' || curChar == '\t')
			continue;
		
		if (curChar == '#')
		{
			bSkipLine = true;
			continue;
		}

		if (bSkipLine)
			continue;

		if (pParsedBuffer)
		{
			if (ParseSize >= *pdwParseSize)
				return false;
			
			pParsedBuffer[ParseSize] = LCASE(curChar);
		}
		
		ParseSize++;
	}

	*pdwParseSize = ParseSize;

	return true;
}
