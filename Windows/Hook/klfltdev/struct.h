#ifndef __STRUCT_H
#define __STRUCT_H


typedef struct _GLOBAL_DATA_KLFLTDEV {

	//  Control Device Object for this filter
	PDEVICE_OBJECT		m_FilterControlDeviceObject;

	
} GLOBAL_DATA_KLFLTDEV, *PGLOBAL_DATA_KLFLTDEV;

extern GLOBAL_DATA_KLFLTDEV KLFltDev_Globals;

#endif