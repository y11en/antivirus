//  avp95api.cpp
//  Urmas Rahu, Data Fellows, 1997

//  AVP95 API implementation

//  Link this module with VxDs which use services provided by AVP95.VXD


#include "avp95api.h"



/*
    ===========================================================================
    Private function declarations
    ===========================================================================
*/

//  Returns pointer to AVP95 object or function
static DWORD GetAVP95Pointer(int pointerid_);




/*
    ===========================================================================
    Public functions

    See avp95api.h for parameter and return value descriptions.
    ===========================================================================
*/

DWORD AVP95VXD_Get_Version()
    {                                               // get version
    DWORD avp95device = GetAVP95Pointer(AVP95CTRL_GETPTR_AVP95_OBJ);
    if (!avp95device)
        return -1;                                  // -1: AVP95Device pointer not found

    fnptrAVP95VXD_Get_Version fnptr = (fnptrAVP95VXD_Get_Version)GetAVP95Pointer(AVP95CTRL_GETPTR_GET_VERSION);
    if (!fnptr)
        return -2;                                  // -2: function pointer not found

    DWORD ret = fnptr(avp95device);

    return ret;
    }


DWORD AVP95VXD_Initialize(PCHAR dbname_)
    {                                               // initialize, load databases
    DWORD avp95device = GetAVP95Pointer(AVP95CTRL_GETPTR_AVP95_OBJ);
    if (!avp95device)
        return -1;                                  // -1: AVP95Device pointer not found

    fnptrAVP95VXD_Initialize fnptr = (fnptrAVP95VXD_Initialize)GetAVP95Pointer(AVP95CTRL_GETPTR_INITIALIZE);
    if (!fnptr)
        return -2;                                  // -2: function pointer not found

    DWORD ret = fnptr(avp95device, dbname_);

    return ret;
    }


DWORD AVP95VXD_RegisterCallback(AVPCallback callback_)
    {                                               // register AVP callback
    DWORD avp95device = GetAVP95Pointer(AVP95CTRL_GETPTR_AVP95_OBJ);
    if (!avp95device)
        return -1;                                  // -1: AVP95Device pointer not found

    fnptrAVP95VXD_RegisterCallback fnptr = (fnptrAVP95VXD_RegisterCallback)GetAVP95Pointer(AVP95CTRL_GETPTR_REGISTERCALLBACK);
    if (!fnptr)
        return -2;                                  // -2: function pointer not found

    DWORD ret = fnptr(avp95device, callback_);

    return ret;
    }


DWORD AVP95VXD_ProcessObject(AVPScanObject * obj_)
    {                                               // process object
    DWORD avp95device = GetAVP95Pointer(AVP95CTRL_GETPTR_AVP95_OBJ);
    if (!avp95device)
        return -1;                                  // -1: AVP95Device pointer not found

    fnptrAVP95VXD_ProcessObject fnptr = (fnptrAVP95VXD_ProcessObject)GetAVP95Pointer(AVP95CTRL_GETPTR_PROCESSOBJECT);
    if (!fnptr)
        return -2;                                  // -2: function pointer not found

    DWORD ret = fnptr(avp95device, obj_);

    return ret;
    }

DWORD AVP95VXD_MakeFullName(PCHAR buf_, PCHAR shnameptr_)
    {                                               // process object
    DWORD avp95device = GetAVP95Pointer(AVP95CTRL_GETPTR_AVP95_OBJ);
    if (!avp95device)
        return -1;                                  // -1: AVP95Device pointer not found

    fnptrAVP95VXD_MakeFullName fnptr = (fnptrAVP95VXD_MakeFullName)GetAVP95Pointer(AVP95CTRL_GETPTR_MAKEFULLNAME);
    if (!fnptr)
        return -2;                                  // -2: function pointer not found

    return (DWORD)fnptr(avp95device, buf_, shnameptr_);
    }


/*
    ===========================================================================
    Private functions
    ===========================================================================
*/

/*
    =====================================================================
    GetAVP95Pointer()

    Returns pointer to AVP95 object or function by getting the DDB of AVP95
    device and sending the private AVP95CTRL_GETPTR control message to the
    control procedure.  Returns NULL if either the DDB was not found (probably
    AVP95.VXD not loaded), or the control procedure returned the NULL pointer.

    See Avp95Device::OnControlMessage() in avp95.cpp for more information.
    =====================================================================
*/

static DWORD GetAVP95Pointer(int pointerid_)
    {
    DWORD ret = 0;

    PDDB ddb = Get_DDB(0, "AVP95   ");              // get DDB for AVP95.VXD, note that the device name must be space-padded

    if (ddb)
        {
        ALLREGS regs;
        memset(&regs, 0, sizeof(regs));

        regs.REDX = pointerid_;                     // pointer ID is passed in edx

        Directed_Sys_Control(ddb, AVP95CTRL_GETPTR, &regs);

#ifdef DEBUGPRINT
        dprintf("Directed_Sys_Control() returned %x, %x, %x", (int)regs.REBX, (int)regs.RECX, (int)regs.REDX);
#endif

        ret = regs.REDX;                            // pointer returned in edx
        }

    return ret;
    }


