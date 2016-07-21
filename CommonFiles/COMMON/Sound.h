#if !defined(AFX_SOUND_H__0852DD29_C30D_4415_805F_EE4BFEC8BB28__INCLUDED_)
#define AFX_SOUND_H__0852DD29_C30D_4415_805F_EE4BFEC8BB28__INCLUDED_

#include "defs.h"

namespace KAVSound
{
	enum Sounds
	{
		eInfected = 0,	
		eAttackDetected,
		
		ePasswordRequest,

		eProductStart,
		eProductFinish,
		
		eODSTaskStartedByUser,
		eODSTaskStartedByScheduler,
		eODSTaskCompleted,
		eODSTaskFailed,

		eUpdaterTaskStartedByUser,
		eUpdaterTaskStartedByScheduler,
		eUpdaterTaskCompleted,
		eUpdaterTaskFailed,

		eRTPTaskStarted,
		eRTPTaskCompleted,
		eRTPTaskFailed
	};

	bool COMMONEXPORT CheckCurrentUserSoundSchema ();
	bool COMMONEXPORT PlayKAVSound (Sounds sound, bool bWait = false);
}

#endif // !defined(AFX_SOUND_H__0852DD29_C30D_4415_805F_EE4BFEC8BB28__INCLUDED_)
