#ifndef __mklapg_h
#define __mklapg_h

#include <windows.h>
#include "avpgimpl.h"

#include <ProductCore/common/userban.h>

#include "wavpg.h"

class cMklAvpg : public cAvpgImpl
{
public:
	cMklAvpg( cContext* pContext, tAVPG2_WORKING_MODE WorkingMode );
	~cMklAvpg();

	tERROR CreateIO( hOBJECT* p_phObject, hSTRING p_hObjectName );
	bool AddFilters();

public:
	cContext*		m_pContext;
};

#endif // __mklapg_h