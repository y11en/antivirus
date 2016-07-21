// KLH_SYSDevice.h

#ifndef __KLH_SYSDevice_h__
#define __KLH_SYSDevice_h__


class KLH_SYSDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	// Member Functions
	KLH_SYSDevice(ULONG unit=0);
	~KLH_SYSDevice();
	// Trace object
	KTrace t;
	// Member Data
	ULONG			m_Unit;			// Unit number for this device instance
	KUnitizedName	*m_RegPath;		// Used in constructor/ destructor to hold registry path
	DEVMEMBER_DISPATCHERS
#if 0
    The following member functions are actually defined by the
    DEVMEMBER_DISPATCHERS macro.  These comment-only definitions
    allow easy navigation to the functions within the Developer Studio.
   ;
COMMENT_ONLY CreateClose(KIrp I);
COMMENT_ONLY DeviceControl(KIrp I);
#endif
};


#endif		// __KLH_SYSDevice_h__
