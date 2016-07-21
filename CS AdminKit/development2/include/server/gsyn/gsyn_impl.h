#ifndef __KLGSYN_IMPL_H__
#define __KLGSYN_IMPL_H__

#include <srvp/gsyn/groupsync.h>

void KLGSYN_InitiateGroupSync(
					const std::wstring&			wstrLocal,
					const std::wstring&			wstrRemote,
                    const std::wstring&			wstrType,
                    const std::wstring&			wstrFullPath,
					KLGSYN::GSYN_DIRECTION		nDirection,
					long						lLockTimeout,
					long						lLifeTime,
					long						lSectPakSize/*,
					AVP_qword&					qwSyncVersion*/);

#endif //__KLGSYN_IMPL_H__