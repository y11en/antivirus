/////////////////////////////////////////////////////////////////////////////////////
// Flags.h

#ifndef AVPCCFLAGS_H
#define AVPCCFLAGS_H

//////////////////////////////////////////////////////////////////////
// Masks
#define PGF_FLAGS					0xFFFF0000	// Task flags
#define PGF_TASK_NUMBER				0x0000FFFF	// Task number within component

//////////////////////////////////////////////////////////////////////
// Common flags
#define PGF_ACCESS_OVER_NET			0x00010000	// If flag is set then access to destination computer over the network otherwise local access
#define PGF_NATIVE_COMPONENT		0x00080000	// If flag is set then pages is accessed by native component
#define PGF_MULTIPLE_OBJECTS		0x00100000	// If flag is set then pages is created for multiple objects
#define PGF_OBJECT_OFFLINE			0x00200000	// If flag is set then object is offline

#define CFGPAGE_IS_ACCESS_OVER_NET(dwType)		((dwType) & PGF_ACCESS_OVER_NET)
#define CFGPAGE_IS_NATIVE_COMPONENT(dwType)		((dwType) & PGF_NATIVE_COMPONENT)
#define CFGPAGE_IS_MULTIPLE_OBJECTS(dwType)		((dwType) & PGF_MULTIPLE_OBJECTS)
#define CFGPAGE_IS_OBJECT_OFFLINE(dwType)		((dwType) & PGF_OBJECT_OFFLINE)

//////////////////////////////////////////////////////////////////////
// Not used flags

//#define PGF_REPORT_SHOW_STATISTICS	0x00020000	// If flag is set then show statistics else show report
//#define PGF_WITH_AVPSERVER			0x00040000	// If flag is set then may work with AVPServer else not

//////////////////////////////////////////////////////////////////////
// Product flags
#define PRODF_USES_BASES 			0x0100

#endif