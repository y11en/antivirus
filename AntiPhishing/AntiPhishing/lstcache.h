#ifndef __UF_LIST_CACHE_
#define __UF_LIST_CACHE_

#include <Prague/prague.h>
#include <Prague/pr_vector.h>
#include <ProductCore/structs/s_settings.h>

typedef struct {
	tDWORD  dwFlags;
	tUINT   nLen;
	tWCHAR* pName;
} tCacheItemAttr;

class cListCache {
public:
	cListCache();
	~cListCache();

	tERROR Init(hOBJECT obj);
	tERROR Build(const cVector<cEnabledStrItem>& list);
	bool   IsBuilt();
	tERROR Free();
	bool   Find(tWCHAR* sURL, tUINT nUrlLen, tWCHAR* sDomain, tUINT nDomainLen, tWCHAR** ppRule);
	void   Dump();

public:
	cVector<tCacheItemAttr> m_vItems;
private:
	tPTR    m_pCacheData;
	hOBJECT m_obj;
};


#endif // __UF_LIST_CACHE_