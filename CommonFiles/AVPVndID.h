#if !defined (__AVPVNDIDS_H__)
#define __AVPVNDIDS_H__

#include "Property/Property.h"
#include "AVPComID.h"

// AVP Vnd File IDs and PIDs
#define AVP_PID_VNDROOT_ID            AVP_DC_VENDOR_LIST
#define AVP_PID_VNDROOT               MAKE_AVP_PID(AVP_PID_VNDROOT_ID, AVP_APID_GLOBAL, avpt_nothing, 0)

#define AVP_PID_VNDINFO_ID            (1)
#define AVP_PID_VNDINFO								MAKE_AVP_PID(AVP_PID_VNDINFO_ID, AVP_APID_GLOBAL, avpt_nothing, 0)	

#define AVP_PID_VNDEXPDATE_ID         (1)
#define AVP_PID_VNDEXPDATE            MAKE_AVP_PID(AVP_PID_VNDEXPDATE_ID, AVP_DC_VENDOR_LIST, avpt_date, 0)	

#define AVP_PID_VNDDESCRIPTION_ID     (2)
#define AVP_PID_VNDDESCRIPTION				MAKE_AVP_PID(AVP_PID_VNDDESCRIPTION_ID, AVP_DC_VENDOR_LIST, avpt_str, 0)	

#define AVP_PID_VNDKERNELDATA_ID      (AVP_APID_KERNEL)
#define AVP_PID_VNDKERNELDATA				  MAKE_AVP_PID(AVP_PID_VNDKERNELDATA_ID, AVP_APID_GLOBAL, avpt_bin, 1)	

#endif
