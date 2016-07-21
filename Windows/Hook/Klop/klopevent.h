#ifndef _KLOP_EVENT_H__
#define _KLOP_EVENT_H__

#include "kldef.h"
#include "kl_object.h"

class CKl_KlopEvent : public CKl_Object
{
public:
	KL_EVENT	m_Event;
	ULONG		m_Event9x;
	
	CKl_KlopEvent();
	virtual ~CKl_KlopEvent();

	void	Clear();
	void	Wait();
	void	Set();
};

#endif 