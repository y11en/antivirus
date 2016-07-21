#ifndef __klapg_h
#define __klapg_h

#include <windows.h>
#include "avpgimpl.h"

#include <ProductCore/common/userban.h>

#include "wavpg.h"

class cKlAvpg : public cAvpgImpl
{
public:
	cKlAvpg( cContext* pContext, tAVPG2_WORKING_MODE WorkingMode );
	~cKlAvpg();

	tERROR CreateIO( hOBJECT* p_phObject, hSTRING p_hObjectName );
	bool AddFilters();

private:
	void AddSkipFName( PWCHAR pwch );

public:
	cContext*		m_pContext;
};

#endif // __klapg_h