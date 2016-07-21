// KLE_SYS.h
//

#ifndef __KLE_SYS_h__
#define __KLE_SYS_h__

//#define POOL PagedPool
#define POOL NonPagedPool
   
class KLE_SYS : public KDriver
{
	SAFE_DESTRUCTORS
public:
	// Member functions
	virtual VOID Unload(VOID);
	virtual NTSTATUS DriverEntry(PUNICODE_STRING RegistryPath);

	// Member data

#if 0
    The following member functions are actually defined by macros.
    These comment-only definitions are for easier Studio navigation.
   ;
#endif
};

extern	ULONG		defTraceDevice;			// Default trace device
extern	TRACE_LEVEL defTraceOutputLevel;	// Default trace output level
extern	BREAK_LEVEL defTraceBreakLevel;		// Default trace break level
extern	KTrace		Tracer;					// Global driver trace object

#define _Trace Tracer.Trace

#ifdef _DEBUG
#define _DebugTrace Tracer.Trace
#else
#define _DebugTrace //
#endif


int WaitIfBusy();
void ReleaseWait();

#endif			// __KLE_SYS_h__
