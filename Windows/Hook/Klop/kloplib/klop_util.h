#ifndef _KLOP_UTILS_H__
#define _KLOP_UTILS_H__

#include <list>
#include "kloplib.h"

bool
KLOP_isWinNT();

struct ADAPTER_INFO {
    ULONG			Version;    // Версия AdapterInfo.
	ULONG			LocalIp;
	ULONG			AdapterNameSize;
	char			AdapterBuffer[MAX_PATH];  // имя адаптера не может быть больше, чем MAX_PATH
};

std::list<ADAPTER_INFO>
KLOP_GetAdapterList();


BOOL
KLOP_Init( const char* Key, ULONG KeySize );

BOOL
KLOP_AddAdapter(ADAPTER_INFO* pAI);

BOOL
KLOP_SetBroadcastAddr();

#endif // _KLOP_UTILS_H__ 