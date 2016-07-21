// KLE_SYSDevice.cpp
// Implementation of KLE_SYSDevice device class
#include "stdafx.h"

#pragma code_seg("INIT")

// KLE_SYSDevice Constructor
//              The device constructor is typically responsible for allocating
//              any physical resources that are associated with the device.
//
//              The device constructor often reads the registry to setup
//              various configurable parameters.
//
KLE_SYSDevice::KLE_SYSDevice(ULONG Unit) :
	KDevice(
		KUnitizedName(L"KLE_SYSDevice", Unit),
		FILE_DEVICE_UNKNOWN,
		KUnitizedName(L"KLE_SYSDevice", Unit),
		0,
		DO_BUFFERED_IO)


	// Initialize KTrace object in device class
	,t(
		KUnitizedName(L"KLE_SYSDevice", Unit),
		defTraceDevice,
		defTraceOutputLevel,
		defTraceBreakLevel)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		_DebugTrace(TraceError, "Failed to create device KLE_SYSDevice unit number %d, status %x\n", Unit, m_ConstructorStatus);
		return;
	}
	m_Unit = Unit;

	m_RegPath = CreateRegistryPath(L"KLE_SYSDevice", Unit);
	if (m_RegPath == NULL)
	{
		// Error, cannot allocate memory for registry path
		_DebugTrace(TraceError, "Failed to create registry path\n");
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}


}
#pragma code_seg()

// KLE_SYSDevice Destructor
//
KLE_SYSDevice::~KLE_SYSDevice()
{

	delete m_RegPath;

}


// Member functions of KLE_SYSDevice
//

// Major function handlers
//
NTSTATUS KLE_SYSDevice::DeviceControl(KIrp I)
{
//    t.Trace(TraceInfo, "Entering DeviceControl, IRP= %x, IOCTL code= %x\n",(PIRP)I, I.IoctlCode());
    NTSTATUS status = STATUS_SUCCESS;
	I.Information()=0;


// TODO: Handle Internal Device Control request
    switch (I.IoctlCode())
    {
	case KLE_IOCTL_GET_FUNCTION_TABLE:
		//OutBuf: PVOID KLE_FUNCTION_TABLE*; OutBufSize 4;
		if(I.IoctlOutputBufferSize() < sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		*(DWORD*)I.IoctlBuffer() = (DWORD)&FunctionTable;
		I.Information() = sizeof(DWORD);
		break;
		
	default:
	    status = STATUS_INVALID_PARAMETER;
		break;
    }

    return I.Complete(status);
}


NTSTATUS KLE_SYSDevice::CreateClose(KIrp I)
{
//	t.Trace(TraceInfo, "Entering CreateClose, IRP= %x, Major Function= %x\n",
//		   (PIRP)I, I.MajorFunction());
    if (I.MajorFunction() == IRP_MJ_CREATE)
	{
		// TODO: Insert CREATE handler code
	}
	else
	{
		// Must be CLOSE since we are in CREATECLOSE handler
		// TODO: Insert CLOSE handler code
	}

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

