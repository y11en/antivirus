#ifndef __HOST_MESSAGES__
#define __HOST_MESSAGES__

// service start types
#define cSERVICE_BOOT_START             0x00000000
#define cSERVICE_SYSTEM_START           0x00000001
#define cSERVICE_AUTO_START             0x00000002
#define cSERVICE_DEMAND_START           0x00000003
#define cSERVICE_DISABLED               0x00000004

// Set service start type, msg_id - one of the service start types (cSERVICE_xxx)
// return: errOK_DECIDED if changed sucessfully or WIN32 error converted to prague format
#define pmc_SERVICE_SET_START_TYPE 0xa2066ad2
#define pmc_SERVICE_CONTROL 0xa3066ad2
#define   pm_CHECK_SERVICE_ACCESS_RIGHTS 0xa3066ad3
#define   pm_PROCPROTECTION_ONREBOOT     0xa3066acf

#endif // __HOST_MESSAGES__