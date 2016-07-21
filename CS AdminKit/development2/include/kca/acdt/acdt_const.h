#ifndef __ACDT_CONST_H__
#define __ACDT_CONST_H__

namespace KLACDTAPI
{
    const wchar_t c_szwProductNagent[] =    L"1103";
    const wchar_t c_szwProductAdmServer[] = L"1093";
    const wchar_t c_szwProductWksAny[] =    L"wks";
    const wchar_t c_szwProductWks50[] =     L"wks50";
    const wchar_t c_szwProductWks46[] =     L"wks46";
    const wchar_t c_szwProductWks60[] =     L"wks60";
    const wchar_t c_szwProductFsAny[] =     L"fs";
    const wchar_t c_szwProductFs50[] =      L"fs50";
    const wchar_t c_szwProductFs60[] =      L"fs60";
    const wchar_t c_szwProductFsEe60[] =    L"fsee60";

    const wchar_t c_szwStateHostDN[] = L"HostDN";   //STRING_T
    const wchar_t c_szwStateProducts[] = L"Products";   //PARAMS_T
    const wchar_t c_szwStateEventLogs[] = L"EventLogs";   //ARRAY_T
    const wchar_t c_szwStateProductDN[] = L"ProductDN";   //STRING_T
    const wchar_t c_szwStateProdProps[] = L"ProdProps";   //INT_T    , PP_MAY_*
    const wchar_t c_szwStateProductName[] = L"ProductName"; //STRING_T
    const wchar_t c_szwStateProductInstallPath[] = L"InstallPath"; //STRING_T
    const wchar_t c_szwStateProductVersion[] = L"ProductVersion";   //STRING_T
    const wchar_t c_szwStateProductBuild[] = L"ProductBuild";    //STRING_T
    const wchar_t c_szwStateProductRunning[] = L"ProductRunning";   //BOOL_T
    const wchar_t c_szwStateTraceFile[] = L"TraceFile";     //STRING_T
    const wchar_t c_szwStateTraceLevel[] = L"TraceLevel";   //INT_T
    const wchar_t c_szwStateTraceLimit_Min[] = L"TraceLimitMin";   //INT_T
    const wchar_t c_szwStateTraceLimit_Max[] = L"TraceLimitMax";   //INT_T
    const wchar_t c_szwStateTraceLimit_Default[] = L"TraceLimitDef";   //INT_T
    const wchar_t c_szwStateDiagLog[] = L"DiagLog";   //INT_T    
    const wchar_t c_szwStateDiagTrace[] = L"DiagTrace";   //INT_T
    const wchar_t c_szwStateProdDumps[] = L"ProdDumps";   //ARRAY_T
    const wchar_t c_szwStateLastActionResult[] = L"LastActionResult";   //STRING_T
    
    const int c_nInvalidTraceLevel = -1;

    typedef enum 
    {
        KLACDT_PA_NOTHING = 0,
        KLACDT_PA_START = 1,
        KLACDT_PA_STOP,
        KLACDT_PA_RESTART,
    }
    KLACDT_PRODUCT_ACTION;

    
    typedef enum
    {
        PP_MAY_BE_STOPPED = 1,
        PP_MAY_BE_STARTED = 2,
        PP_MAY_BE_RESTARTED = 4,
        PP_MAY_RUN_DIAG = 8,
        PP_MAY_USE_TRACE = 16
    }
    product_props_t;


    /*! 
        Acquire current state. 
        returns KLAR::Params
    */
    const wchar_t c_szwAcdtAction_StateAcquire[] =      L"KLACDT_STATE_GET";
    
    /*! 
        Changes tracelevel, turns on/off tracing 
            Par1 is product name -- one of c_szwProduct* constants.
            Par2 is a tracelevel.
        returns KLAR::Params
    */
    const wchar_t c_szwAcdtAction_TraceChangeLevel[] =      L"KLACDT_TRACE_CHLEVEL";

    /*! 
        Acquires a file. 
            Par1 is a file name
        returns ACDTFile*
    */
    const wchar_t c_szwAcdtAction_Trace_AcquireFile[] =L"KLACDT_FILE_GET";

    /*! 
        Removes a file. 
            Par1 is a file name
        returns KLAR::Params
    */
    const wchar_t c_szwAcdtAction_Trace_DropFile[] =   L"KLACDT_FILE_DROP";

    /*! 
        Starts Product.
            Par1 is a product name
        returns KLAR::Params
    */
    const wchar_t c_szwAcdtAction_Product_Start[] =     L"KLACDT_PRODUCT_START";

    /*! 
        Restarts Product.
            Par1 is a product name
        returns KLAR::Params
    */
    const wchar_t c_szwAcdtAction_Product_Restart[] =     L"KLACDT_PRODUCT_RESTART";


    /*! 
        Stops Product.
            Par1 is a product name
        returns KLAR::Params
    */
    const wchar_t c_szwAcdtAction_Product_Stop[] =     L"KLACDT_PRODUCT_STOP";

    /*! 
        Executes diag utility. 
            Par1 is a product name
        returns KLAR::Params
    */
    const wchar_t c_szwAcdtAction_Diag_Execute[] =   L"KLACDT_DIAG_EXECUTE";

    /*! 
        Save EventLog
            Par1 is an EventLog name
        returns KLAR::Params
    */
    const wchar_t c_szwAcdtAction_EL_Save[] =   L"KLACDT_EL_SAVE";

    //! returns KLAR::Params
    const wchar_t c_szwAcdtAction_MakeDump[] =   L"KLACDT_MAKE_DUMP";

    //! returns KLAR::Params
        // Par1 is short exename
        // Par2 is a cmdlin
        // Par3 is a cabname
        // Par4 compress results
    const wchar_t c_szwAcdtAction_RunCustomUtility[] =   L"KLACDT_CUSOM_UT";

    //! returns KLAR::Params
        // Par1 compress
    const wchar_t c_szwAcdtAction_SaveSettings[] =   L"KLACDT_SAVE_SETTINGS";

    //! file api

    const wchar_t c_szwFile_API[] = L"KLACDT_FILE";

    //! open, par1 filename, out: par1 'handle', par2 size
    const wchar_t c_szwFile_Open[] = L"KLACDT_FILE_OPEN";
    
    //! close par1 handle
    const wchar_t c_szwFile_Close[] = L"KLACDT_FILE_CLOSE";
    
    //! read; in: par1 offset, par2 base; out par1 res
    const wchar_t c_szwFile_Seek[] = L"KLACDT_FILE_SEEK";    
    
    //! read; in: par1 bytes, out: par1 data
    const wchar_t c_szwFile_Read[] = L"KLACDT_FILE_READ"; 
    
    //! open, par1 filename, must be KLACDT_CUSTUT_CAB, out: par1 'handle'
    const wchar_t c_szwFile_Upload[] = L"KLACDT_FILE_UPLOAD";

    //! write par1 data
    const wchar_t c_szwFile_Write[] = L"KLACDT_FILE_WRITE";
};


#define KLACDTAPI_Service       L"KLACDT_Service"
#define KLACDTAPI_WORKING_DIR   L"KLACDT"
#define KLACDT_ARG_SERVICE      L"service"
#define KLACDTAPI_STATE         L"state.dat"
#define KLACDTAPI_LASTRES       L"lastres.dat"
#define KLACDT_RESULTS          L"LastAction"
#define KLACDT_CUSTUT_CAB       L"utility.cab"
#define KLACDT_CUSTUT_DIR       L"utility"
#define KLACDT_CUSTUT_RESDIR    L"result"

#define KLACDT_CUSTUT_ENV_DST   L"KLACDT_DESTDIR"


#define KLACDT_FILESERVER_PRODNAME     L"Fileserver"
#define KLACDT_FILESERVER_PRODVER      L"5.0.0.0"
#define KLACDT_FILESERVER_UPGRCODE     L"{0C60718E-84F0-4BE1-BA71-978235450926}"

#define KLACDT_WORKSTATION_PRODNAME    L"Workstation"
#define KLACDT_WORKSTATION_PRODVER     L"5.0.0.0"
#define KLACDT_WORKSTATION_UPGRCODE    L"{05986875-CEDD-4E38-B2C6-359E2ED96860}"

#define KLACDT_NAGENT_PRODNAME         L"1103"
#define KLACDT_NAGENT_PRODVER          L"1.0.0.0"
#define KLACDT_NAGENT_UPGRCODE         L"{673D19BC-21AD-409E-AB56-09FD02182259}"

#define KLACDT_NAGENT_MSI_UPGRCODE     L"{B9518725-0B76-4793-A409-C6794442FB50}"

#define KLACDT_AK_PRODNAME             L"1093"
#define KLACDT_AK_PRODVER              L"1.0.0.0"
#define KLACDT_AK_UPGRCODE             L"{83A28D53-EEDD-49E3-AE83-C806EB513388}"

#define KLACDT_WORKSTATION6_PRODNAME    L"KAVWKS6"
#define KLACDT_WORKSTATION6_PRODVER     L"6.0.0.0"
#define KLACDT_WORKSTATION6_UPGRCODE    L"{0C73B606-17D8-4EFC-A8FB-9C37F35402BE}"
#define KLACDT_SOS6_UPGRCODE            L"{E1712E93-464B-4199-BF1B-72F8E407A312}"

#define KLACDT_FILESERVER6_PRODNAME     L"KAVFS6"
#define KLACDT_FILESERVER6_PRODVER      L"6.0.0.0"
#define KLACDT_FILESERVER6_UPGRCODE     L"{30100296-AE0A-455D-874F-F4B417F578C3}"

#define KLACDT_WORKSTATION46_UPGRCODE    L"{7AA5BE00-0A3C-4D6A-98CA-5A365D064DF5}"

#define KLACDT_KAVFSEE_PRODNAME         L"KAVFSEE"
#define KLACDT_KAVFSEE_PRODVER          L"6.0"
#define KLACDT_KAVFSEE_UPGRCODE         L"{53650C4C-C3AF-4AF1-A8D3-1D4669750322}"

#endif //__ACDT_CONST_H__
