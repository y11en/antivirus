#ifndef __CONN_RECOVER_H__
#define __CONN_RECOVER_H__


#define KLCONNAPP_RECOVER_ASYNCID   L"rcvr_loadconnector"
#define KLCONNAPP_RECOVER_PRODUCT   L"rcvr_product"
#define KLCONNAPP_RECOVER_VERSION   L"rcvr_version"

namespace KLCONNAPPINST
{

    enum connext_cmd_t
    {
        CONEXCMD_Nothing = 0,
        CONEXCMD_GetAppState = 1,
        CONEXCMD_Start,
        CONEXCMD_Stop,
        CONEXCMD_GetRtpState,
        CONEXCMD_NlIfNlSupported,
        CONEXCMD_NlCanUseNlNow,
        CONEXCMD_NlCanUseNlAlways,
        CONEXCMD_NlGetItemInfo,
        CONEXCMD_NlGetItemsList,
        CONEXCMD_NlDoItemsAction,
        CONEXCMD_ForceReplication,
        CONEXCMD_SetDebugFlags,
        CONEXCMD_GetConnectorStatistics,
        CONEXCMD_PortsToOpenInFirewallChanged,
        CONEXCMD_WasReplicatedOnce,
        CONEXCMD_AcquireNumberOfUncured,
        CONEXCMD_NlGetItemFile,
        CONEXCMD_NlGetFileInfo,
        CONEXCMD_NlGetFileChunk,
        CONEXCMD_NlDestroyKLNLIF
    };

    const wchar_t c_szwMethodName[] = L"KLCONNECX_CALL";
    const wchar_t c_szwCmd[] = L"cmd";
    const wchar_t c_szwComponent[] = L".KLCONNECX_NagentLoader";

    const wchar_t c_szwConn_GetPortsToOpenInFirewall[] = L"KLCONNECX_Conn_GetPortsToOpenInFirewall";

    #define KLCONNAPPINST_MAKE_INSTANCE_NAME(_prod, _ver)   \
                            (std::wstring(L"KLCONNECX_RECOVER_") + \
                            std::wstring(_prod) + L"_" + (_ver) + \
                            KLSTD_StGetPrefixW())

    #define KLCONNAPPINST_MAKE_SSS_INSTANCE_NAME(_prod, _ver)   \
                            (std::wstring(L"KLCONNECX_RECOVER_SSS_") + \
                            std::wstring(_prod) + L"_" + (_ver) + \
                            KLSTD_StGetPrefixW())

    #define KLCONNAPPINST_MAKE_MUTEX_NAME   KLCONNAPPINST_MAKE_INSTANCE_NAME

    const wchar_t c_szwMustLoadOutside[] = L"KLCONNAPPINST_MUST_LOAD_OUTSIDE";

    const wchar_t c_szwRgFlagMayLoadOutside[] = L"KLCONNAPPINST_RG_MAY_LOAD_OUTSIDE";
    
    const wchar_t c_szwRgFlagMustLoadOutside[] = L"KLCONNAPPINST_RG_MUST_LOAD_OUTSIDE";
};

#endif //__CONN_RECOVER_H__
