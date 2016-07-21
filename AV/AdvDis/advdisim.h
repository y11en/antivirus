#ifndef __advdisim
#define __advdisim

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>

#include <AV/plugin/p_startupenum2.h>

#include <windows.h>

void
ReparseKeyName (
	cStringObj* pStrKey
	);

class cInternal
{
public:
	cInternal();
	virtual ~cInternal();

	bool Init( cObject* pThis );
	bool SetActive( bool bActive );
	bool ProtectFileMask( PWCHAR pwchFileMask );
	bool ProtectKey( PWCHAR KeyPattern, PWCHAR ValuePattern );

private:
	cObject*	m_pThis;
	PVOID		m_pClientContext;
};

#endif // __advdisim