#ifndef __MAIN_H
#define __MAIN_H
//-----------------------------------------------------------------
#include <fltKernel.h>

#define AcquireResourceExclusive(__r__) {	\
	KeEnterCriticalRegion();	\
	ExAcquireResourceExclusiveLite(__r__, TRUE);	\
}

#define AcquireResourceShared(__r__) {	\
	KeEnterCriticalRegion();	\
	ExAcquireResourceSharedLite(__r__, TRUE);	\
}

#define ReleaseResource(__r__) {	\
	ExReleaseResourceLite(__r__);	\
	KeLeaveCriticalRegion();	\
}

#define _dbg_break_		KdBreakPoint();

#ifdef __cplusplus
extern "C" {
#endif

// get this filter instance for volume on which this file resides
PFLT_INSTANCE GetInstanceForFile(
									  __in PUNICODE_STRING p_usFileName
									  );

#ifdef __cplusplus
}	// extern "C" {
#endif
//-----------------------------------------------------------------
#endif
