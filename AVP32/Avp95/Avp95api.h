//  avp95api.h
//  Urmas Rahu, Data Fellows, 1997

//  AVP95 device API (services to VxDs, not usable from ring 3)


#ifndef __AVP95API_H
#define __AVP95API_H

#ifndef __VMM_H_
#include <vtoolscp.h>
#endif

/*
===============================================================================
===============================================================================
AVP95 API functions (defined in avp95api.cpp, which is to be linked with
calling VxD).  This top section in this header is the only one needed by the
caller.

Note about the return values: all the functions will return:
    -1     : the function is not available (probably AVP95.VXD not loaded)
    -2     : the function is not available (probably wrong version of the VxD not
             supporting that particular function)

Other return values are function-specific.
===============================================================================
===============================================================================
*/


/*
    =====================================================================
    AVP_CALLBACK_xxxx

    Callback message IDs.  These values are passed to AVPCallback as the 1st parameter.
    =====================================================================
*/
//#define AVP_CALLBACK_PUT_STRING     1
// add more later
/*
    =====================================================================
    AVPScanObject

    Scan object structure.
    =====================================================================
*/
#ifndef AVPSCANOBJECT
#include <ScanAPI/scanobj.h>
#include <ScanAPI/avp_msg.h>
#endif
/*
struct AVPScanObject
    {
    //add stuff later
    char        Name[0x200];
    DWORD       UserData;                           // this item must given back to callback as it is
    //add stuff later
    };
*/






/*
    =====================================================================
    AVP95VXD_Get_Version()

    Returns AVP95 version.

    Parameters:
        None

    Returns:
        -1              : AVP95.VXD not loaded
        -2              : function not found
        other           : version of AVP95.VXD
    =====================================================================
*/
DWORD AVP95VXD_Get_Version();



/*
    =====================================================================
    AVP95VXD_Initialize()

    Initializes the scan engine, loading the databases.

    Parameters:
        dbname_         : full pathname of databases set file

    Returns:
        -1              : AVP95.VXD not loaded
        -2              : function not found
        <specify other return values>
    =====================================================================
*/
DWORD AVP95VXD_Initialize(PCHAR dbname_);



/*
    =====================================================================
    AVPCallback

    AVP callback is called by AVP95VXD_ProcessObject().

    Parameters:
        msg_            : message ID (one of AVP_CALLBACK_xxxx values)
        param_          : message-specific parameter (not used for all messages; should be NULL if not used)
        obj_            : pointer to the scan object (not used for all messages; should be NULL if not used)

    Returns:
        Return value is message-specific, specify later.
    =====================================================================
*/
typedef DWORD (* AVPCallback) (DWORD /*msg_*/, void * /*param_*/, AVPScanObject * /*obj_*/);


/*
    =====================================================================
    AVP95VXD_RegisterCallback()

    Registers the AVP callback function.

    Parameters:
        callback_       : pointer to the AVP callback, NULL to disable callback

    Returns:
        -1              : AVP95.VXD not loaded
        -2              : function not found
        <specify other return values>
    =====================================================================
*/
DWORD AVP95VXD_RegisterCallback(AVPCallback callback_);



/*
    =====================================================================
    AVP95VXD_ProcessObject()

    Processes (scans or whatever) an object.

    Parameters:
        obj_            : pointer to the object information

    Returns:
        -1              : AVP95.VXD not loaded
        -2              : function not found
        <specify other return values>
    =====================================================================
*/
DWORD AVP95VXD_ProcessObject(AVPScanObject * obj_); // process object



/*
    =====================================================================
    AVP95VXD_MakeFullName()

    Fill the buffer with restored virus name.

    Parameters:
                buf_            : pointer to the buffer
        shnameptr_      : pointer to the short name (in the AVPScanObject)

    Returns:
        pointer to the restored full virus name
    =====================================================================
*/
DWORD AVP95VXD_MakeFullName(PCHAR buf_, PCHAR shnameptr_);  //make full virus name


/*
===============================================================================
===============================================================================
Stuff below used privately by avp95api.cpp and avp95.cpp
===============================================================================
===============================================================================
*/
/*
    =====================================================================
    AVP95 private system control messages.
    =====================================================================
*/
                                                    // get function or object pointer
#define AVP95CTRL_GETPTR                  (BEGIN_RESERVED_PRIVATE_SYSTEM_CONTROL +  0)


/*
    =====================================================================
    Object/function pointer IDs, used with the AVP95CTRL_GETPTR control
    message.
    =====================================================================
*/

#define AVP95CTRL_GETPTR_AVP95_OBJ         0        // pointer to Avp95Device object

#define AVP95CTRL_GETPTR_GET_VERSION       1        // pointer to AVP95VXD_Get_Version()
#define AVP95CTRL_GETPTR_INITIALIZE        2        // pointer to AVP95VXD_Initialize()
#define AVP95CTRL_GETPTR_REGISTERCALLBACK  3        // pointer to AVP95VXD_RegisterCallback()
#define AVP95CTRL_GETPTR_PROCESSOBJECT     4        // pointer to AVP95VXD_ProcessObject()
#define AVP95CTRL_GETPTR_MAKEFULLNAME      5        // pointer to AVP95VXD_MakeFullName()

#define AVP95CTRL_FPICTRL_GETPTR_GET_MODULE_INFORMATION   11
#define AVP95CTRL_FPICTRL_GETPTR_INITIALIZE               12
#define AVP95CTRL_FPICTRL_GETPTR_UNINITIALIZE             13
#define AVP95CTRL_FPICTRL_GETPTR_SCAN_FILE                14
#define AVP95CTRL_FPICTRL_GETPTR_SCAN_BOOT_BLOCK          15
#define AVP95CTRL_FPICTRL_GETPTR_SCAN_MEMORY              16
#define AVP95CTRL_FPICTRL_GETPTR_VALIDATE_DATABASES       17
#define AVP95CTRL_FPICTRL_GETPTR_REPORT_SCANNING_STATUS   18
#define AVP95CTRL_FPICTRL_GETPRT_RELOAD_DATABASES         19
#define AVP95CTRL_FPICTRL_GETPTR_AUTO_RELOAD_OFF          20
#define AVP95CTRL_FPICTRL_GETPTR_AUTO_RELOAD_ON           21
#define AVP95CTRL_FPICTRL_GETPTR_CUSTOM_SETTINGS          22
/*
#define FPICTRL_GETPTR_GET_MODULE_INFORMATION   1
#define FPICTRL_GETPTR_INITIALIZE               2
#define FPICTRL_GETPTR_UNINITIALIZE             3
#define FPICTRL_GETPTR_SCAN_FILE                4
#define FPICTRL_GETPTR_SCAN_BOOT_BLOCK          5
#define FPICTRL_GETPTR_SCAN_MEMORY              6
#define FPICTRL_GETPTR_VALIDATE_DATABASES       7
#define FPICTRL_GETPTR_REPORT_SCANNING_STATUS   8
#define FPICTRL_GETPRT_RELOAD_DATABASES         9
#define FPICTRL_GETPTR_AUTO_RELOAD_OFF          10
#define FPICTRL_GETPTR_AUTO_RELOAD_ON           11
#define FPICTRL_GETPTR_CUSTOM_SETTINGS          12
*/

/*
    =====================================================================
    Internal function pointer typedefs; note that all functions have
    pointer to AVP95Device object as the 1st parameter.
    =====================================================================
*/
typedef DWORD (* fnptrAVP95VXD_Get_Version) (DWORD);
typedef DWORD (* fnptrAVP95VXD_Initialize) (DWORD, PCHAR);
typedef DWORD (* fnptrAVP95VXD_RegisterCallback) (DWORD, AVPCallback);
typedef DWORD (* fnptrAVP95VXD_ProcessObject) (DWORD, AVPScanObject *);
typedef PCHAR (* fnptrAVP95VXD_MakeFullName) (DWORD, PCHAR, PCHAR);



#endif //__AVP95API_H
