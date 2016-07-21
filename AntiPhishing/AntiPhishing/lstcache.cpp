#include "lstcache.h"
#include "pattern.h"

#define LIST_FLAG_DOMAIN_NAME     0x01
#define LIST_FLAG_HAS_MASK        0x02
#define LIST_FLAG_HAS_PROTO       0x04
#define LIST_FLAG_ENABLED         0x08

cListCache::cListCache() 
{
	m_pCacheData = NULL;
}

cListCache::~cListCache()
{
	Free();
};

tERROR cListCache::Init(hOBJECT obj)
{
	m_obj = obj;
	return errOK;
}

tERROR cListCache::Build(const cVector<cEnabledStrItem>& list)
{
	tERROR error = errOK;
	tDWORD i, c, size = 0;
	tWCHAR* ptr;
	if (!m_obj)
		return errOBJECT_NOT_INITIALIZED;
	Free();
	c = list.count();
	if (0 == c)
		return errOK;
	for (i=0; i<c; i++)
	{
		tCacheItemAttr& attr = m_vItems.push_back();
		memset(&attr, 0, sizeof(tCacheItemAttr));
		attr.nLen = list[i].m_sName.length();
		size += attr.nLen;
		if (list[i].m_bEnabled) 
			attr.dwFlags |= LIST_FLAG_ENABLED;
		if (list[i].m_sName.find_first_of("\\/") == cStrObj::npos)
			attr.dwFlags |= LIST_FLAG_DOMAIN_NAME;
		else if (list[i].m_sName.find("://") != cStrObj::npos)
			attr.dwFlags |= LIST_FLAG_HAS_PROTO;
		if (list[i].m_sName.find_first_of("*?") != cStrObj::npos)
		{
			attr.dwFlags |= LIST_FLAG_HAS_MASK;
			if (attr.dwFlags & LIST_FLAG_DOMAIN_NAME)
			{
				tDWORD pos = -1, dotcount = 0;
				for (;;) 
				{
					pos = list[i].m_sName.find(".", cCP_ANSI, pos+1, cStrObj::whole, fSTRING_FIND_FORWARD);
					if (pos == cStrObj::npos)
						break;
					dotcount++;
				} 
				if (dotcount < 2) // this is not a domain or 1st level domain
					attr.dwFlags &= ~LIST_FLAG_DOMAIN_NAME;
			}
		}
	}
	size += c;
	size += 10;
	size *= sizeof(tWCHAR);
	error = m_obj->heapAlloc(&m_pCacheData, size);
	if (PR_SUCC(error))
	{
		PR_TRACE((m_obj, prtNOTIFY, "urlf\tListCache::Build allocated %d bytes for %d urls", size, c));
		ptr = (tWCHAR*)m_pCacheData;
		for (i=0; i<c; i++)
		{
			error = list[i].m_sName.copy(ptr, size);
			if (PR_FAIL(error))
				break;
			_wcslwr(ptr);
			m_vItems[i].pName = ptr;
			ptr += m_vItems[i].nLen + 1;
			size -= (m_vItems[i].nLen + 1) * sizeof(tWCHAR);
		}
		PR_TRACE((m_obj, prtNOTIFY, "urlf\tListCache::Build unused buffer size %d bytes", size));
	}

	if (PR_FAIL(error))
		Free();
	return error;
}

bool cListCache::IsBuilt()
{
	return (m_pCacheData != NULL);
}

tERROR cListCache::Free()
{
	if (!m_obj)
		return errOBJECT_NOT_INITIALIZED;
	m_vItems.clear();
	if (m_pCacheData)
	{
		m_obj->heapFree(m_pCacheData);
		m_pCacheData = NULL;
	}
	return errOK;
}

bool cListCache::Find(tWCHAR* sURL, tUINT nUrlLen, tWCHAR* sDomain, tUINT nDomainLen, tWCHAR** ppRule)
{
	tDWORD i, c;
	tWCHAR* pUrlWithoutProto;
	if (!m_pCacheData)
		return false;
	pUrlWithoutProto = wcsstr(sURL, L"://");
	if (pUrlWithoutProto)
		pUrlWithoutProto += 3;
	else
		pUrlWithoutProto = sURL;
	if (ppRule)
		*ppRule = NULL;
	c = m_vItems.count();
	for (i=0; i<c; i++)
	{
		tCacheItemAttr& item = m_vItems[i];
		if (!item.nLen || !item.pName || !(item.dwFlags & LIST_FLAG_ENABLED))
			continue;
		if (item.dwFlags & LIST_FLAG_DOMAIN_NAME)
		{
			if (item.dwFlags & LIST_FLAG_HAS_MASK)
			{
				if (MatchWithPatternW((unsigned short*)item.pName, (unsigned short*)sDomain, true, false))
				{
					if (ppRule)
						*ppRule = item.pName;
					return true;
				}
			}
			else
			{
				if (nDomainLen < item.nLen) 
					continue;
				if (nDomainLen > item.nLen) // with sub-domains
				{
					if (sDomain[nDomainLen-item.nLen-1] != '.')
						continue;
				}

				if (0 == wcscmp(item.pName, sDomain+nDomainLen-item.nLen))
				{
					if (ppRule)
						*ppRule = item.pName;
					return true;
				}
			}
		}
		else
		{
			tWCHAR* pUrlCompare = (item.dwFlags & LIST_FLAG_HAS_PROTO ? sURL : pUrlWithoutProto);
			if (item.dwFlags & LIST_FLAG_HAS_MASK)
			{
				if (MatchWithPatternW((unsigned short*)item.pName, (unsigned short*)pUrlCompare, true, false))
				{
					if (ppRule)
						*ppRule = item.pName;
					return true;
				}
			}
			else
			{
				tDWORD proto_len = pUrlCompare - sURL;
				if (nUrlLen - proto_len != item.nLen) 
					continue;
				if (0 == wcscmp(item.pName, pUrlCompare))
				{
					if (ppRule)
						*ppRule = item.pName;
					return true;
				}
			}
		}
	}
	return false;
}

void cListCache::Dump()
{
	tDWORD i, c;
	c = m_vItems.count();
	for (i=0; i<c; i++)
	{
		tCacheItemAttr& item = m_vItems[i];
		if (!item.nLen || !item.pName)
		{
			PR_TRACE((m_obj, prtNOT_IMPORTANT, "urlf\tItem #d <empty>", i));
			continue;
		}
		PR_TRACE((m_obj, prtNOT_IMPORTANT, "urlf\tItem #%d %s%s%s %8d %S", i,
		item.dwFlags & LIST_FLAG_DOMAIN_NAME ? "D" : " ",
		item.dwFlags & LIST_FLAG_HAS_MASK    ? "M" : " ",
		item.dwFlags & LIST_FLAG_HAS_PROTO   ? "P" : " ",
		item.nLen,
		item.pName));
	}
}
