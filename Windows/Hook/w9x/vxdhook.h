// AVPG.h - include file for VxD AVPG

#include <vtoolsc.h>

#define DRIVER_Drp    KLIF_Drp
#define DRIVER_Ilb    KLIF_Ilb

#define KLIF_Major		0
#define KLIF_Minor		11

//#define KLIF_DeviceID		0x382E	//old one from Microsoft for AVP.VXD
//#define KLIF_DeviceID   0x323D //from iamdrv.vxd (@guard)
//#define AVPG_DeviceID		UNDEFINED_DEVICE_ID
#define KLIF_Init_Order	UNDEFINED_INIT_ORDER

//
// Partial undocumented IFSREQ structure
//
#include <pshpack1.h>
typedef struct {
    // embedded ioreq stucture
    ioreq            ifsir;
    // the structure isn't really defined this way, but
    // we take advantage of the layout for our purposes
    struct hndlfunc  *ifs_hndl;
    ULONG            reserved[10];
} ifsreq, *pifsreq;
#include <poppack.h>
