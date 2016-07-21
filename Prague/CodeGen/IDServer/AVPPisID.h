#ifndef __AVPPISID_H__
#define __AVPPISID_H__


#include <property/property.h>
#include <AVPComID.h>

#define PI_APP_ID										AVP_APID_GLOBAL //(1) Repaced to AVPCommID.h

// AVPPragueIDServer IDs and PIDs

// Information Root ID
// Vendor ID
#define PI_PID_ROOT_ID							  (1)
#define PI_PID_ROOT										MAKE_AVP_PID(PI_PID_ROOT_ID,PI_APP_ID, avpt_dword, 0)

// Information Root PROP
// Vendor name
#define PI_PID_VENDORNAME_ID					(1)
#define PI_PID_VENDORNAME							MAKE_AVP_PID(PI_PID_VENDORNAME_ID,PI_APP_ID,avpt_str,0)

// Information Root PROP
// Vendor mnemonic ID
#define PI_PID_VENDORMNEMONICID_ID		(2)
#define PI_PID_VENDORMNEMONICID				MAKE_AVP_PID(PI_PID_VENDORMNEMONICID_ID,PI_APP_ID,avpt_str,0)

// Interface Identifiers Root node
#define PI_PID_INTERFACEID_ID					(1)
#define PI_PID_INTERFACEID						MAKE_AVP_PID(PI_PID_INTERFACEID_ID,PI_APP_ID,avpt_dword,1)

// Plugin Identifiers Root node
#define PI_PID_PLUGINID_ID						(2)
#define PI_PID_PLUGINID								MAKE_AVP_PID(PI_PID_PLUGINID_ID,PI_APP_ID,avpt_dword,1)

// ID Root node
// ID Range
#define PI_PID_IDRANGE_ID							(1)
#define PI_PID_IDRANGE								MAKE_AVP_PID(PI_PID_IDRANGE_ID,PI_APP_ID,avpt_dword,1)

#endif
