// FSAVPDevice.h

#ifndef __FSAVPDevice_h__
#define __FSAVPDevice_h__


class FSAVPDevice : public KDevice
{
	SAFE_DESTRUCTORS
public:
	// Member Functions
	FSAVPDevice(ULONG unit=0);
	~FSAVPDevice();
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


#endif		// __FSAVPDevice_h__
