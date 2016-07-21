// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  03 September 2007,  15:39 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end

#pragma warning(disable:4345)

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Includes for interface,  )
#include "task.h"
// AVP Prague stamp end



#include <windows.h>
#include <stdio.h>

#include <pr_oid.h>
#include <pr_time.h>
#include <Updater/updater.h>

#include <iface/i_engine.h>
#include <iface/i_hash.h>
#include <i_updaterconfigurator.h>
#include <extract/iface/i_datacodec.h>

#include <plugin/p_hash_md5.h>
#include <extract/plugin/p_base64.h>

#include <structs/s_statistics.h>

#include "../../Components/Updater/Transport/PragueTransport/i_transport.h"
#include "../../Components/Updater/Transport/PragueTransport/p_transportimplementation.h"
#include "../../Components/Updater/Transport/ProxyServerAddressDetector/p_proxyserveraddressdetectorimplementation.h"



#define UPDATER_DLL             "Updater.dll"
#define GET_BEST_URL            "GetBestUrl"
#define USER_AGENT              "Mozilla"
#define POST_NETWORK_TIMEOUT    60 // seconds
#define pnBEST_URL              "BestUrl"
#define INFO_FILE_EXT           L".bin"
#define SERVER_FILE_NAME        "stat/stat.xml"
#define POST_DATA               "DATA="
#define ENV_PRODUCT_TYPE        L"%ProductType%"
#define ENV_PRODUCT_VERSION     L"%ProductVersion%"
#define AVZ_FILE                L"%AVZSysInfo%"

// xml constants
#define XML_HEADER              L"<?xml version=\"1.0\"?>\n"
#define COMMON_TAG_BEGIN        L"<StatReport PackageVersion=\"1\" UserID=\"%S\" TimeNow=\"%d\">\n\n"
#define COMMON_TAG_END          L"</StatReport>\n"
#define COMPONENT_TAG_BEGIN     L"    <%S>\n"
#define COMPONENT_TAG_END       L"    </%S>\n\n"
#define GENERAL_INFO_TAG_BEGIN  L"<GeneralInfo "
#define GENERAL_INFO_TAG_END    L"/>\n\n"
#define ATTR_END                L"\" "
#define OS_ATTR                 L"OS=\""
#define SP_ATTR                 L"SP=\""
#define APPID_ATTR              L"AppID=\""
#define BUILD_ATTR              L"Build=\""

// windows defines
#define SM_TABLETPC             86
#define SM_MEDIACENTER          87
#define SM_STARTER              88
#define SM_SERVERR2             89

// windows info
#define KERNEL_DLL             "kernel32.dll"
#define FUN_GET_SYS_INFO       "GetNativeSystemInfo"
#define WIN_31                 L"3.1"
#define WIN_95                 L"95"
#define WIN_98                 L"98"
#define WIN_ME                 L"Me"
#define WIN_SE                 L" Second Edition"
#define WIN_OSR2               L" OSR2"
#define WIN_NT_351             L"NT 3.51"
#define WIN_NT_40              L"NT 4.0"
#define WIN_2000               L"2000"
#define WIN_XP                 L"XP"
#define WIN_2002               L"2002"
#define WIN_2003               L"2003"
#define WIN_R2                 L" R2"
#define WIN_VISTA              L"Vista"
#define WIN_LONGHORN           L"Longhorn"
#define WIN_NT                 "NT %u.%u"
#define WIN_WORKSTATION        L" Workstation"
#define WIN_HOME_EDITION       L" Home Edition"
#define WIN_EMBEDDED           L" Embedded"
#define WIN_MEDIA_CENTER       L" Media Center Edition"
#define WIN_STARTER_EDITION    L" Starter Edition"
#define WIN_TABLE_PC           L" Tablet PC Edition"
#define WIN_PROFESSIONAL       L" Professional"
#define WIN_HOME               L" Home"
#define WIN_SERVER             L" Server"
#define WIN_ENTERPRISE         L" Enterprise Edition"
#define WIN_DATACENTER         L" Datacenter"
#define WIN_ADVANCED           L" Advanced"
#define WIN_WEB                L" Web"
#define WIN_STANDARD           L" Standard"
#define WIN_STORAGE            L" Storage"
#define WIN_DATACENTER_EDITION L" Datacenter Edition"
#define WIN_WEB_EDITION        L" Web Edition"
#define WIN_CLUSTER_EDITION    L" Compute Cluster Edition"
#define WIN_SMALL_BUSINESS     L" for Small Business Server"
#define WIN_STANDARD_EDITION   L" Standard Edition"
#define WIN_DOMAIN_CONTROLLER  L" Domain Controller"
#define WIN_X64                L" x64 Edition"


#define ERROR_TRACE(exps, err)    PR_TRACE((g_root, prtNOTIFY, "stat\tFile: %s Line: %d Error: %terr = %s", __FILE__, __LINE__, err, exps));

#define _ERROR(exp) \
    error = exp ;               \
    if (PR_FAIL(error))         \
    {                           \
    ERROR_TRACE(#exp, error) ;    \
    return error ;          \
    }

#define _VERROR(exp) \
    error = exp ;               \
    if (PR_FAIL(error))         \
    {                           \
    ERROR_TRACE(#exp, error) ;    \
    return ;          \
    }



typedef bool (*TGetBestUrl)(const char* p_pszXmlBuffer, 
                            int         p_nXmlBufferSize, 
                            const char* p_pszRegion, 
                            char*       p_pszResult, 
                            int         p_nResultSize,
                            char*       p_pszComponents,
                            int         p_nComponentsSize,
                            char*       p_pszTimeout,
                            int         p_nTimeoutSize) ;


typedef struct _SendIOInfo {
    tDWORD m_dwIndex ;
    tDWORD m_dwSize ;
} SSendIOInfo;

typedef struct _AttrInfo {
    enum EAttrType {
        eEnd       = 0x0,
        eWhen      = 0x1,
        eMD5       = 0x2,
        eSize      = 0x3,
        eVerdict   = 0x4,
        eFileName  = 0x5,
        eLocalPort = 0x6,
        eProto     = 0x7,
    };

    EAttrType      m_eType ;
    const wchar_t* m_pszAttrName ;
} SAttrInfo;

typedef struct _CompInfo {
    const char*      m_pszTaskName ;
    const wchar_t*   m_pszCompName ;
    const wchar_t*   m_pszCompEntryName ;
    const SAttrInfo* m_aAttrInfo ;
} SCompInfo;

const SAttrInfo g_aOAS[] = {
    { SAttrInfo::eWhen,    L"Time"    },
    { SAttrInfo::eMD5,     L"md5"     },
    { SAttrInfo::eSize,    L"size"    },
    { SAttrInfo::eVerdict, L"Verdict" },
    { SAttrInfo::eEnd,     L""        },
};

const SAttrInfo g_aODS[] = {
    { SAttrInfo::eWhen,    L"Time"    },
    { SAttrInfo::eMD5,     L"md5"     },
    { SAttrInfo::eSize,    L"size"    },
    { SAttrInfo::eVerdict, L"Verdict" },
    { SAttrInfo::eEnd,     L""        },
};

const SAttrInfo g_aMailAntiVirus[] = {
    { SAttrInfo::eWhen,    L"Time"    },
    { SAttrInfo::eMD5,     L"md5"     },
    { SAttrInfo::eSize,    L"size"    },
    { SAttrInfo::eVerdict, L"Verdict" },
    { SAttrInfo::eEnd,     L""        },
};

const SAttrInfo g_aWebAntiVirus[] = {
    { SAttrInfo::eWhen,     L"Time"    },
    { SAttrInfo::eMD5,      L"md5"     },
    { SAttrInfo::eSize,     L"size"    },
    { SAttrInfo::eVerdict,  L"Verdict" },
    { SAttrInfo::eFileName, L"URL"     },
    { SAttrInfo::eEnd,      L""        },
};

const SAttrInfo g_aAntiPhishing[] = {
    { SAttrInfo::eWhen,     L"Time"    },
    { SAttrInfo::eFileName, L"URL"     },
    { SAttrInfo::eEnd,      L""        },
};

const SAttrInfo g_aIDS[] = {
    { SAttrInfo::eWhen,      L"Time"      },
    { SAttrInfo::eFileName,  L"IP"        },
    { SAttrInfo::eLocalPort, L"LocalPort" },
    { SAttrInfo::eProto,     L"Protocol"  },
    { SAttrInfo::eVerdict,   L"Verdict"   },
    { SAttrInfo::eEnd,       L""          },
};

const SCompInfo g_aCompInfo[] = {
    { AVP_TASK_ONACCESSSCANNER,   L"OAS",           L"entryOAS",           g_aOAS           },
    { AVP_TASK_ONDEMANDSCANNER,   L"ODS",           L"entryODS",           g_aODS           },
    { AVP_TASK_MAILCHECKER,       L"MailAntiVirus", L"entryMailAntiVirus", g_aMailAntiVirus },
    { AVP_TASK_WEBMONITOR,        L"WebAntiVirus",  L"entryWebAntiVirus",  g_aWebAntiVirus  },
    { AVP_TASK_ANTIPHISHING,      L"AntiPhishing",  L"entryAntiPhishing",  g_aAntiPhishing  },
    { AVP_TASK_INTRUSIONDETECTOR, L"IDS",           L"entryIDS",           g_aIDS           },
};

const tDWORD g_dwCompInfoSize = sizeof(g_aCompInfo) / sizeof(SCompInfo) ;



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CKLStatistics::ObjectInitDone()
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Task::ObjectInitDone method") ;

    _ERROR(sysCreateObjectQuick((hOBJECT*)&m_hCSIORWFile, IID_CRITICAL_SECTION)) ;
    _ERROR(sysCreateObjectQuick((hOBJECT*)&m_hCSIOXml, IID_CRITICAL_SECTION)) ;
    _ERROR(sysCreateObjectQuick((hOBJECT*)&m_hCSIOEvent, IID_CRITICAL_SECTION)) ;

    _ERROR(cThreadPoolBase::init(this, 1, 0, TP_THREADPRIORITY_NORMAL)) ;

    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CKLStatistics::ObjectPreClose()
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Task::ObjectPreClose method") ;

    cThreadPoolBase::de_init() ;

    if (m_hCSIORWFile)
    {
        m_hCSIORWFile->sysCloseObject() ;
        m_hCSIORWFile = 0 ;
    }

    if (m_hCSIOXml)
    {
        m_hCSIOXml->sysCloseObject() ;
        m_hCSIOXml = 0 ;
    }

    if (m_hCSIOEvent)
    {
        m_hCSIOEvent->sysCloseObject() ;
        m_hCSIOEvent = 0 ;
    }

    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR CKLStatistics::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) 
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Task::MsgReceive method") ;

    switch (p_msg_cls_id)
    {
    case pmc_ENGINE_DETECT:
        break ;

    case pmc_UPDATER_CRITICAL_REPORT:
        switch (p_msg_id)
        {
        case pm_REP_UPDATE_END:
                error = OnUpdateEnd((cUpdaterEvent*)p_par_buf) ;
            break ;

        default:
            break ;
        }
        break ;

    case pmc_EVENTS_IMPORTANT:
        {
            if (p_par_buf && 
                p_par_buf_len == SER_SENDMSG_PSIZE)
            {
                if (((cSerializable*)p_par_buf)->isBasedOn(cAskObjectAction::eIID))
                {
                    error = OnAskAction((cAskObjectAction*)p_par_buf) ;
                }
                else if (((cSerializable*)p_par_buf)->isBasedOn(cIDSEventReport::eIID))
                {
                    error = OnIDSEventReport((cIDSEventReport*)p_par_buf) ;
                }
                else if (((cSerializable*)p_par_buf)->isBasedOn(cDetectObjectInfo::eIID))
                {
                    error = OnDetectObject((cDetectObjectInfo*)p_par_buf, p_ctx) ;
                }
            }
        }
        break ;

    default:
        break ;
    }

    return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR CKLStatistics::SetSettings( const cSerializable* p_settings )
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Task::SetSettings method") ;

    if (!p_settings || !p_settings->isBasedOn(cKLStatisticsSettings::eIID))
    {
        return errPARAMETER_INVALID ;
    }

    m_objSettings = *(cKLStatisticsSettings*)p_settings ;

    return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR CKLStatistics::GetSettings( cSerializable* p_settings )
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Task::GetSettings method") ;

    if (!p_settings || !p_settings->isBasedOn(cKLStatisticsSettings::eIID))
    {
        return errPARAMETER_INVALID ;
    }

    *(cKLStatisticsSettings*)p_settings = m_objSettings ;

    return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR CKLStatistics::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
    tERROR error = errNOT_IMPLEMENTED ;
    PR_TRACE_FUNC_FRAME("Task::AskAction method") ;

    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR CKLStatistics::SetState( tTaskRequestState p_state )
{
    tERROR error = warnTASK_STATE_CHANGED ;
    PR_TRACE_FUNC_FRAME("Task::SetState method") ;

    tDWORD dwState = TASK_STATE_UNKNOWN ;
    switch (p_state)
    {
    case TASK_REQUEST_RUN:
        dwState = TASK_STATE_RUNNING ;
        break ;
    case TASK_REQUEST_STOP:
        dwState = TASK_STATE_STOPPED ;
        break ;
    }

    if (dwState == m_dwTaskState)
    {
        return error ;
    }

    if (dwState == TASK_STATE_RUNNING)
    {
        _ERROR(StopSubscription()) ;
        _ERROR(StartSubscription()) ;
        _ERROR(UpdateSettings()) ;
    }
    else if (dwState == TASK_STATE_STOPPED)
    {
        _ERROR(StopSubscription()) ;
    }

    m_dwTaskState = dwState ;

    if (PR_SUCC(error))
    {
        error = warnTASK_STATE_CHANGED ;
    }

    return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR CKLStatistics::GetStatistics( cSerializable* p_statistics )
{
    tERROR error = errNOT_IMPLEMENTED;
    PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

    // Place your code here

    return error;
}
// AVP Prague stamp end



CKLStatistics::CKLStatistics()
  : cThreadPoolBase("CKLStatistics")
{

}

tERROR CKLStatistics::SendAVZFile()
{
    tERROR error = errOK ;

    PR_TRACE((this, prtNOTIFY, "stat\tSendAVZFile - begin")) ;

    cStrObj strAVZFile(AVZ_FILE) ;
    sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strAVZFile), 0, 0) ;
    PR_TRACE((this, prtNOTIFY, "stat\tSendAVZFile - AVZFile:%S", strAVZFile.data())) ;

    cAutoObj<cIO> hAVZFile ;
    _ERROR(sysCreateObject(hAVZFile, IID_IO, PID_NATIVE_FIO)) ;
    _ERROR(strAVZFile.copy(hAVZFile, pgOBJECT_FULL_NAME)) ;
    _ERROR(hAVZFile->set_pgOBJECT_ORIGIN(OID_GENERIC_IO)) ;
    _ERROR(hAVZFile->sysCreateObjectDone()) ;

    PR_TRACE((this, prtNOTIFY, "stat\tSendAVZFile - file created")) ;

    tQWORD qwFileSize = 0 ;
    hAVZFile->GetSize(&qwFileSize, IO_SIZE_TYPE_EXPLICIT) ;
    if (!qwFileSize)
    {
        return errOBJECT_NOT_FOUND ;
    }

    cBuff<tBYTE, 1024> pFileContent ;
    _ERROR(hAVZFile->SeekRead(0, 0, pFileContent.get((tUINT)qwFileSize, false), (tDWORD)qwFileSize)) ;
    hAVZFile.clean() ;

    cAutoObj<cDataCodec> objBase64 ;
    _ERROR(sysCreateObjectQuick(objBase64, IID_DATACODEC, PID_BASE64)) ;
    objBase64->propSetDWord(plUSE_PADDING, 1) ;

    tDWORD dwResult =  0 ;
    objBase64->Encode(&dwResult, (const tBYTE*)pFileContent, (tDWORD)qwFileSize, 0, 0) ;
    if (!dwResult)
    {
        PR_TRACE((this, prtNOTIFY, "stat\tSendAVZFile - error (content empty)")) ;
        return errUNEXPECTED ;
    }

    cBuff<tBYTE, 1024> buffAVZ ;
    memcpy(buffAVZ.get(strlen(POST_DATA) + dwResult, false), POST_DATA, strlen(POST_DATA)) ;
    _ERROR(objBase64->Encode(&dwResult, (const tBYTE*)pFileContent, (tDWORD)qwFileSize, (tBYTE*)buffAVZ + strlen(POST_DATA), dwResult)) ;

    PR_TRACE((this, prtNOTIFY, "stat\tSendAVZFile - post file")) ;

    // post date
    _ERROR(PostFile(SERVER_FILE_NAME, (const tBYTE*)buffAVZ, dwResult + strlen(POST_DATA))) ;

    PR_TRACE((this, prtNOTIFY, "stat\tSendAVZFile - end")) ;

    return error ;
}

void CKLStatistics::do_work()
{
    tERROR error = errOK ;

    PR_TRACE((this, prtNOTIFY, "stat\tdo_work - begin")) ;

    _VERROR(InitPostTransport()) ;

    // Sned AVZ-file
    SendAVZFile() ;

    // print xml-header
    cStrObj strXml(XML_HEADER) ;
    cDateTime dtNow = cDateTime::now_utc() ;

    // print common tag
    cStrObj strTemp ;
    strTemp.format(cCP_UNICODE, COMMON_TAG_BEGIN, m_objSettings.m_strUserID.data(), (time_t)dtNow) ;
    strXml += strTemp ;

    strXml += GetInfoString() ;

    cAutoCS autoCSIOXml(m_hCSIOXml, false) ;

    // print component info
    bool blSendData = false ;
    cVector<SSendIOInfo> aIOInfo ;
    for (tDWORD dwIndex = 0, dwCount = m_aComponents.size(); dwIndex < dwCount; ++dwIndex)
    {
        if (!m_aIO[dwIndex])
        {
            continue ;
        }

        // find component info
        tDWORD dwCompIndex = 0 ;
        for (;dwCompIndex < g_dwCompInfoSize; ++dwCompIndex)
        {
            if (m_aComponents[dwIndex].compare(g_aCompInfo[dwCompIndex].m_pszTaskName, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ)
            {
                break ;
            }
        }

        if (dwCompIndex == g_dwCompInfoSize)
        {
            continue ;
        }

        PR_TRACE((this, prtNOTIFY, "stat\tdo_work - data for '%s'", g_aCompInfo[dwCompIndex].m_pszTaskName)) ;

        // read component data
        tQWORD qwIOSize = 0 ;
        cBuff<tBYTE, 1024> buff ;
        {
            cAutoCS autoCSIORWFile(m_hCSIORWFile, true) ;

            m_aIO[dwIndex]->GetSize(&qwIOSize, IO_SIZE_TYPE_EXPLICIT) ;
            if (qwIOSize == 0)
            {
                continue ;
            }

            _VERROR(m_aIO[dwIndex]->SeekRead(0, 0, buff.get((tUINT)qwIOSize, false), (tDWORD)qwIOSize)) ;
        }

        PR_TRACE((this, prtNOTIFY, "stat\tdo_work - component tag begin")) ;

        strTemp.format(cCP_UNICODE, COMPONENT_TAG_BEGIN, g_aCompInfo[dwCompIndex].m_pszCompName) ;
        strXml += strTemp ;

        // print component date
        tDWORD dwPosition = 0 ;
        for (;dwPosition < buff.used();)
        {
            cStatisticsInfo* objEntryInfo = 0 ;
            tDWORD dwUsed = 0 ;
            if (PR_FAIL(g_root->StreamDeserialize((cSerializable**)&objEntryInfo, buff.ptr() + dwPosition, buff.used() - dwPosition, &dwUsed)))
            {
                PR_TRACE((this, prtNOTIFY, "stat\tdo_work - StreamDeserialize - error")) ;
                break ;
            }

            dwPosition += dwUsed ;

            cDateTime dtWhen(&objEntryInfo->m_dtWhen) ;
            cDateTime dtMaxPeriod(&m_objSettings.m_dtMaxPeriod) ;
            if (dtNow.diff(dtWhen) > dtMaxPeriod)
            {
                PR_TRACE((this, prtNOTIFY, "stat\tdo_work - skip event by time (now:'%tgdt')", dtNow)) ;
                PR_TRACE((this, prtNOTIFY, "stat\tdo_work - skip event by time (when:'%tgdt')", dtWhen)) ;
                PR_TRACE((this, prtNOTIFY, "stat\tdo_work - skip event by time (max_period:'%tgdt')", dtMaxPeriod)) ;
                g_root->DestroySerializable(objEntryInfo) ;
                continue ;
            }

            blSendData = true ;

            strXml += L"        <" ;
            strXml += g_aCompInfo[dwCompIndex].m_pszCompEntryName ;

            // print component attributes
            for (tDWORD dwAttrIndex = 0; g_aCompInfo[dwCompIndex].m_aAttrInfo[dwAttrIndex].m_eType != SAttrInfo::eEnd; ++dwAttrIndex)
            {
                strXml += L" " ;

                switch (g_aCompInfo[dwCompIndex].m_aAttrInfo[dwAttrIndex].m_eType)
                {
                case SAttrInfo::eWhen:
                    {
                        strXml += g_aCompInfo[dwCompIndex].m_aAttrInfo[dwAttrIndex].m_pszAttrName ;
                        strXml += L"=\"" ;
                        strTemp.format(cCP_UNICODE, L"%d", (time_t)cDateTime(&objEntryInfo->m_dtWhen)) ;
                        strXml += strTemp ;
                        strXml += L"\"" ;
                    }
                    break ;
                case SAttrInfo::eMD5:
                    {
                        strXml += g_aCompInfo[dwCompIndex].m_aAttrInfo[dwAttrIndex].m_pszAttrName ;
                        strXml += L"=\"" ;
                        strXml += objEntryInfo->m_strMD5 ;
                        strXml += L"\"" ;
                    }
                    break ;
                case SAttrInfo::eSize:
                    {
                        strXml += g_aCompInfo[dwCompIndex].m_aAttrInfo[dwAttrIndex].m_pszAttrName ;
                        strXml += L"=\"" ;
                        strTemp.format(cCP_UNICODE, L"%d", objEntryInfo->m_dwSize) ;
                        strXml += strTemp ;
                        strXml += L"\"" ;
                    }
                    break ;
                case SAttrInfo::eVerdict:
                    {
                        strXml += g_aCompInfo[dwCompIndex].m_aAttrInfo[dwAttrIndex].m_pszAttrName ;
                        strXml += L"=\"" ;
                        strXml += objEntryInfo->m_strVerdict ;
                        strXml += L"\"" ;
                    }
                    break ;
                case SAttrInfo::eFileName:
                    {
                        strXml += g_aCompInfo[dwCompIndex].m_aAttrInfo[dwAttrIndex].m_pszAttrName ;
                        strXml += L"=\"" ;
                        strXml += objEntryInfo->m_strFileName ;
                        strXml += L"\"" ;
                    }
                    break ;
                case SAttrInfo::eLocalPort:
                    {
                        strXml += g_aCompInfo[dwCompIndex].m_aAttrInfo[dwAttrIndex].m_pszAttrName ;
                        strXml += L"=\"" ;
                        strTemp.format(cCP_UNICODE, L"%d", objEntryInfo->m_dwLocalPort) ;
                        strXml += strTemp ;
                        strXml += L"\"" ;
                    }
                    break ;
                case SAttrInfo::eProto:
                    {
                        strXml += g_aCompInfo[dwCompIndex].m_aAttrInfo[dwAttrIndex].m_pszAttrName ;
                        strXml += L"=\"" ;
                        strTemp.format(cCP_UNICODE, L"%d", objEntryInfo->m_dwProto) ;
                        strXml += strTemp ;
                        strXml += L"\"" ;
                    }
                    break ;
                default:
                    break ;
                }
            }

            strXml += L"/>\n" ;

            g_root->DestroySerializable(objEntryInfo) ;
        }

        strTemp.format(cCP_UNICODE, COMPONENT_TAG_END, g_aCompInfo[dwCompIndex].m_pszCompName) ;
        strXml += strTemp ;

        PR_TRACE((this, prtNOTIFY, "stat\tdo_work - component tag end")) ;

        SSendIOInfo objSenIOInfo ;
        objSenIOInfo.m_dwIndex = dwIndex ;
        objSenIOInfo.m_dwSize  = (tDWORD)qwIOSize ;
        aIOInfo.push_back(objSenIOInfo) ;
    }

    strXml += COMMON_TAG_END ;

    if (!blSendData)
    {
        PR_TRACE((this, prtNOTIFY, "stat\tdo_work - data empty")) ;
        return ;
    }

    PR_TRACE((this, prtNOTIFY, "stat\tdo_work - send data")) ;

    cAutoObj<cDataCodec> objBase64 ;
    _VERROR(sysCreateObjectQuick(objBase64, IID_DATACODEC, PID_BASE64)) ;
    objBase64->propSetDWord(plUSE_PADDING, 1) ;

    tDWORD dwResult =  0 ;
    objBase64->Encode(&dwResult, (const tBYTE*)strXml.c_str(cCP_ANSI).ptr(), strXml.length(), 0, 0) ;
    if (!dwResult)
    {
        return ;
    }

    cBuff<tBYTE, 1024> buffXml ;
    memcpy(buffXml.get(strlen(POST_DATA) + dwResult, false), POST_DATA, strlen(POST_DATA)) ;
    _VERROR(objBase64->Encode(&dwResult, (const tBYTE*)strXml.c_str(cCP_ANSI).ptr(), strXml.length(), (tBYTE*)buffXml + strlen(POST_DATA), dwResult)) ;

    // post date
    _VERROR(PostFile(SERVER_FILE_NAME, (const tBYTE*)buffXml, dwResult + strlen(POST_DATA))) ;

    PR_TRACE((this, prtNOTIFY, "stat\tdo_work - sent data")) ;

    // clean storages
    tQWORD qwIOSize = 0 ;
    cBuff<tBYTE, 1024> buff ;
    for (tDWORD dwIndex = 0, dwCount = aIOInfo.size(); dwIndex < dwCount; ++dwIndex)
    {
        cAutoCS autoCSIORWFile(m_hCSIORWFile, true) ;

        PR_TRACE((this, prtNOTIFY, "stat\tdo_work - clean data for '%S'", m_aComponents[aIOInfo[dwIndex].m_dwIndex].data())) ;

        tERROR err = m_aIO[aIOInfo[dwIndex].m_dwIndex]->GetSize(&qwIOSize, IO_SIZE_TYPE_EXPLICIT) ;

        if (PR_FAIL(err) || (tDWORD)qwIOSize <= aIOInfo[dwIndex].m_dwSize)
        {
            m_aIO[aIOInfo[dwIndex].m_dwIndex]->SetSize(0) ;
            continue ;
        }

        err = m_aIO[aIOInfo[dwIndex].m_dwIndex]->SeekRead(0, aIOInfo[dwIndex].m_dwSize, buff.get((tUINT)qwIOSize - aIOInfo[dwIndex].m_dwSize, false), (tDWORD)qwIOSize - aIOInfo[dwIndex].m_dwSize) ;
        m_aIO[aIOInfo[dwIndex].m_dwIndex]->SetSize(0) ;
        if (PR_FAIL(err))
        {
            continue ;
        }

        m_aIO[aIOInfo[dwIndex].m_dwIndex]->SeekWrite(0, 0, (tPTR)buff.ptr(), (tDWORD)qwIOSize - aIOInfo[dwIndex].m_dwSize) ;
    }

    PR_TRACE((this, prtNOTIFY, "stat\tdo_work - end")) ;
}


tERROR CKLStatistics::InitPostTransport()
{
    tERROR error = errOK ;

    if (!!m_hTransport)
    {
        m_hTransport->sysCloseObject() ;
    }

    _ERROR(UpdateSettings()) ;

    cAutoObj<cTransport> hTransport ;
    _ERROR(sysCreateObjectQuick(hTransport, IID_TRANSPORT, PID_TRANSPORTIMPLEMENTATION)) ;
    _ERROR(hTransport->set_pgTRANSPORT_TM((hOBJECT)m_hTM)) ;
    hTransport->set_pgTRANSPORT_TASK((hOBJECT)this) ;
    _ERROR(hTransport->set_pgTRANSPORT_USER_AGENT(USER_AGENT, strlen(USER_AGENT), cCP_ANSI)) ;

    cAutoObj<cProxyServerAddressDetector> hProxyDetector ;
    _ERROR(hTransport->sysCreateObjectQuick(hProxyDetector, IID_PROXYSERVERADDRESSDETECTOR, PID_PROXYSERVERADDRESSDETECTORIMPLEMENTATION)) ;
    _ERROR(hTransport->set_pgPROXY_DETECTOR(hProxyDetector)) ;

    hTransport->set_pgTRANSPORT_NETWORK_TIMEOUT_SECONDS(POST_NETWORK_TIMEOUT) ;

    m_hTransport = hTransport ;
    hTransport.ownership(false) ;
    hProxyDetector.ownership(false) ;

    return error ;
}


tERROR CKLStatistics::PostFile(const char*          p_pszServerFileName, // example - statistics/12ef.txt
                               const unsigned char* p_pBuffer,
                               int                  p_nBufferSize)
{
    tERROR error = errOK ;

    if (!m_hTransport || !m_strBestUrl.length())
    {
        return errOBJECT_NOT_INITIALIZED ;
    }

    cAutoObj<cString> hRequest ;
    _ERROR(m_hTransport->sysCreateObjectQuick(hRequest, IID_STRING)) ;
    //_ERROR(hRequest->Format(cCP_ANSI, p_pszServerFileName)) ;

    error = m_hTransport->httpPost(cAutoString(m_strBestUrl), hRequest, (tPTR)p_pBuffer, p_nBufferSize) ;

    return error ;
}


tERROR CKLStatistics::OnUpdateEnd(cUpdaterEvent* p_pEvent)
{
    tERROR error = errOK ;

    PR_TRACE((this, prtNOTIFY, "stat\tOnUpdateEnd(Error:0x%08X)", p_pEvent->m_errError)) ;

    if (!p_pEvent || 
        !p_pEvent->isBasedOn(cUpdaterEvent::eIID) ||
        PR_FAIL(p_pEvent->m_errError) ||
        p_pEvent->m_blRollback)
    {
        return error ;
    }

    if (m_hToken)
    {
        m_hToken->sysCloseObject() ;
        m_hToken = 0 ;
    }

    sysCreateObjectQuick((hOBJECT*)&m_hToken, IID_TOKEN) ;

    PR_TRACE((this, prtNOTIFY, "stat\tFile: %s Line: %d CKLStatistics::OnUpdateResult this:0x%08X beging create thread", __FILE__, __LINE__, this)) ;

    error = cThreadTaskBase::start(*this) ;

    PR_TRACE((this, prtNOTIFY, "stat\tFile: %s Line: %d CKLStatistics::OnUpdateResult this:0x%08X end create thread", __FILE__, __LINE__, this)) ;

    return error ;
}

tERROR CKLStatistics::OnDetectObject(cDetectObjectInfo* p_pInfo, 
                                     hOBJECT p_pIO)
{
    tERROR error = errOK ;

    if (p_pInfo->m_nObjectStatus != OBJSTATUS_INFECTED)
    {
        return error ;
    }

    PR_TRACE((this, prtNOTIFY, "stat\tOnDetectObject(TaskType:%s)", (tSTRING)p_pInfo->m_strTaskType.c_str(cCP_ANSI)));

    cAutoCS autoCSIOEvent(m_hCSIOEvent, false) ;

    for (tDWORD dwIndex = 0, dwCount = m_aComponents.size(); dwIndex < dwCount; ++dwIndex)
    {
        if (m_aComponents[dwIndex].compare(p_pInfo->m_strTaskType, fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ)
        {
            continue ;
        }

        if (!m_aIO[dwIndex])
        {
            break ;
        }

        cStatisticsInfo objInfo ;
        memcpy(&objInfo.m_dtWhen, (const tDATETIME*)cDateTime(p_pInfo->m_tmTimeStamp), sizeof(tDATETIME)) ;
        objInfo.m_strVerdict  = p_pInfo->m_strDetectName ;
        objInfo.m_strFileName = p_pInfo->m_strObjectName ;
        if (p_pIO && 
            PR_SUCC(g_root->sysCheckObject(p_pIO, IID_IO)))
        {
            hIO hObjectIO = (hIO)p_pIO ;
            tQWORD qwObjectSize = 0 ;
            _ERROR(hObjectIO->GetSize(&qwObjectSize, IO_SIZE_TYPE_EXPLICIT)) ;
            objInfo.m_dwSize = (tDWORD)qwObjectSize ;

            cAutoObj<cHash> objMD5 ;
            _ERROR(sysCreateObjectQuick(objMD5, IID_HASH, PID_HASH_MD5)) ;
            objMD5->Reset() ;

            cBuff<tBYTE, 1024> buffIO ;
            for (tDWORD dwPos = 0, dwResult = 0; PR_SUCC(hObjectIO->SeekRead(&dwResult, dwPos, buffIO.get(1024, false), 1024)) && dwResult; dwPos += dwResult)
            {
                _ERROR(objMD5->Update((tBYTE*)buffIO, dwResult)) ;
            }

            tBYTE pMD5[16] ;
            _ERROR(objMD5->GetHash(pMD5, 16)) ;
            objInfo.m_strMD5.format(cCP_ANSI, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
                                    pMD5[0], pMD5[1], pMD5[2],  pMD5[3],  pMD5[4],  pMD5[5],  pMD5[6],  pMD5[7], 
                                    pMD5[8], pMD5[9], pMD5[10], pMD5[11], pMD5[12], pMD5[13], pMD5[14], pMD5[15]) ;
        }

        tDWORD dwUsed = 0 ;
        _ERROR(g_root->StreamSerialize(&objInfo, SERID_UNKNOWN, 0, 0, &dwUsed)) ;
        
        cBuff<tBYTE, 512> objBuff ;
        _ERROR(g_root->StreamSerialize(&objInfo, SERID_UNKNOWN, objBuff.get(dwUsed, false), dwUsed, &dwUsed)) ;

        cAutoCS autoCSIORWFile(m_hCSIORWFile, true) ;

        tQWORD qwIOSize = 0 ;
        m_aIO[dwIndex]->GetSize(&qwIOSize, IO_SIZE_TYPE_EXPLICIT) ;
        _ERROR(m_aIO[dwIndex]->SeekWrite(0, qwIOSize, (tPTR)objBuff.ptr(), dwUsed))

        break ;
    }

    return error ;
}

tERROR CKLStatistics::OnAskAction(cAskObjectAction* p_pInfo)
{
    tERROR error = errOK ;

    if (!p_pInfo || 
        p_pInfo->m_strTaskType.compare(AVP_TASK_ANTIPHISHING, fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ)
    {
        return error ;
    }

    PR_TRACE((this, prtNOTIFY, "stat\tOnAskAction(TaskType:%s)", (tSTRING)p_pInfo->m_strTaskType.c_str(cCP_ANSI))) ;

    cAutoCS autoCSIOEvent(m_hCSIOEvent, false) ;

    tDWORD dwIndex = 0 ;
    tDWORD dwCount = m_aComponents.size() ;
    for (; dwIndex < dwCount; ++dwIndex)
    {
        if (m_aComponents[dwIndex].compare(p_pInfo->m_strTaskType, fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ)
        {
            continue ;
        }

        break ;
    }

    if (dwIndex == dwCount || !m_aIO[dwIndex])
    {
        return error ;
    }

    cStatisticsInfo objInfo ;
    memcpy(&objInfo.m_dtWhen, (const tDATETIME*)cDateTime(p_pInfo->m_tmTimeStamp), sizeof(tDATETIME)) ;
    objInfo.m_strFileName = p_pInfo->m_strDetectObject ;

    tDWORD dwUsed = 0 ;
    _ERROR(g_root->StreamSerialize(&objInfo, SERID_UNKNOWN, 0, 0, &dwUsed)) ;

    cBuff<tBYTE, 512> objBuff ;
    _ERROR(g_root->StreamSerialize(&objInfo, SERID_UNKNOWN, objBuff.get(dwUsed, false), dwUsed, &dwUsed)) ;

    cAutoCS autoCSIORWFile(m_hCSIORWFile, true) ;

    tQWORD qwIOSize = 0 ;
    m_aIO[dwIndex]->GetSize(&qwIOSize, IO_SIZE_TYPE_EXPLICIT) ;
    _ERROR(m_aIO[dwIndex]->SeekWrite(0, qwIOSize, (tPTR)objBuff.ptr(), dwUsed))

    return error ;
}

tERROR CKLStatistics::OnIDSEventReport(cIDSEventReport* p_pInfo)
{
    tERROR error = errOK ;

    if (!p_pInfo || 
        p_pInfo->m_strTaskType.compare(AVP_TASK_INTRUSIONDETECTOR, fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ)
    {
        return error ;
    }

    PR_TRACE((this, prtNOTIFY, "stat\tOnIDSEventReport(TaskType:%s)", (tSTRING)p_pInfo->m_strTaskType.c_str(cCP_ANSI))) ;

    cAutoCS autoCSIOEvent(m_hCSIOEvent, false) ;

    tDWORD dwIndex = 0 ;
    tDWORD dwCount = m_aComponents.size() ;
    for (; dwIndex < dwCount; ++dwIndex)
    {
        if (m_aComponents[dwIndex].compare(p_pInfo->m_strTaskType, fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ)
        {
            continue ;
        }

        break ;
    }

    if (dwIndex == dwCount || !m_aIO[dwIndex])
    {
        return error ;
    }

    cStatisticsInfo objInfo ;
    memcpy(&objInfo.m_dtWhen, (const tDATETIME*)cDateTime(p_pInfo->m_Time), sizeof(tDATETIME)) ;
    objInfo.m_strVerdict  = p_pInfo->m_AttackDescription ;
    objInfo.m_dwLocalPort = p_pInfo->m_LocalPort ;
    objInfo.m_dwProto     = p_pInfo->m_Proto ;

    tSIZE_T unIPSize = p_pInfo->m_AttackerIP.ToStr(0, 0) ;
    cStrBuff strIP ;
    p_pInfo->m_AttackerIP.ToStr(strIP.get(unIPSize, false), unIPSize) ;
    objInfo.m_strFileName = (tCHAR*)strIP ;

    tDWORD dwUsed = 0 ;
    _ERROR(g_root->StreamSerialize(&objInfo, SERID_UNKNOWN, 0, 0, &dwUsed)) ;

    cBuff<tBYTE, 512> objBuff ;
    _ERROR(g_root->StreamSerialize(&objInfo, SERID_UNKNOWN, objBuff.get(dwUsed, false), dwUsed, &dwUsed)) ;

    cAutoCS autoCSIORWFile(m_hCSIORWFile, true) ;

    tQWORD qwIOSize = 0 ;
    m_aIO[dwIndex]->GetSize(&qwIOSize, IO_SIZE_TYPE_EXPLICIT) ;
    _ERROR(m_aIO[dwIndex]->SeekWrite(0, qwIOSize, (tPTR)objBuff.ptr(), dwUsed))

    return error ;
}

tERROR CKLStatistics::UpdateSettings()
{
    tERROR error = errOK ;

    if (!m_objSettings.m_strConfigFile.length())
    {
        return errOBJECT_NOT_INITIALIZED ;
    }

    sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(m_objSettings.m_strDataFolder), 0, 0) ;
    PR_TRACE((this, prtNOTIFY, "stat\tUpdateSettings - DataFolder:%S", m_objSettings.m_strDataFolder.data())) ;

    sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(m_objSettings.m_strConfigFile), 0, 0) ;
    PR_TRACE((this, prtNOTIFY, "stat\tUpdateSettings - ConfigFile:%S", m_objSettings.m_strConfigFile.data())) ;

    cAutoObj<cIO> hConfigFile ;
    _ERROR(sysCreateObject(hConfigFile, IID_IO, PID_NATIVE_FIO)) ;
    _ERROR(m_objSettings.m_strConfigFile.copy(hConfigFile, pgOBJECT_FULL_NAME)) ;
    _ERROR(hConfigFile->set_pgOBJECT_ORIGIN(OID_GENERIC_IO)) ;
    _ERROR(hConfigFile->sysCreateObjectDone()) ;

    tQWORD qwFileSize = 0 ;
    hConfigFile->GetSize(&qwFileSize, IO_SIZE_TYPE_EXPLICIT) ;
    if (!qwFileSize)
    {
        return errOBJECT_NOT_FOUND ;
    }

    cBuff<tCHAR, 1024> pFileContent ;
    _ERROR(hConfigFile->SeekRead(0, 0, pFileContent.get((tUINT)qwFileSize, false), (tDWORD)qwFileSize)) ;
    hConfigFile.clean() ;

    PrHMODULE hUpdater = PrLoadLibrary ? PrLoadLibrary(UPDATER_DLL, cCP_ANSI) : 0 ;
    if (!hUpdater)
    {
        return errMODULE_NOT_FOUND ;
    }

    TGetBestUrl pGetBestUrl = PrGetProcAddress ? (TGetBestUrl)PrGetProcAddress(hUpdater, GET_BEST_URL) : 0 ;
    if (!pGetBestUrl)
    {
        PrFreeLibrary(hUpdater) ;
        hUpdater = 0 ;
        return errOBJECT_NOT_FOUND ;
    }

    char pszUrl[MAX_PATH] ;
    memset(pszUrl, 0, MAX_PATH) ;
    char pszComponents[1024] ;
    memset(pszComponents, 0, 1024) ;
    char pszTimeout[32] ;
    memset(pszTimeout, 0, 32) ;
    if (!pGetBestUrl(pFileContent.ptr(), pFileContent.used(), m_objSettings.m_strRegion.c_str(cCP_ANSI), pszUrl, MAX_PATH, pszComponents, 1024, pszTimeout, 32))
    {
        PrFreeLibrary(hUpdater) ;
        hUpdater = 0 ;
        return errUNEXPECTED ;
    }

    PrFreeLibrary(hUpdater) ;
    pGetBestUrl = 0 ;
    hUpdater    = 0 ;

    PR_TRACE((this, prtNOTIFY, "stat\tComponents string:%s ; Timeout string:%s", pszComponents, pszTimeout));

    if (!strlen(pszUrl))
    {
        return errUNEXPECTED ;
    }

    m_strBestUrl = pszUrl ;

    bool blUpdate = false ;

    char* p_pszComponents = pszComponents ;
    if (p_pszComponents && strlen(p_pszComponents))
    {
        m_objSettings.m_aComponent.clear() ;
        for (char* pIndex = p_pszComponents; pIndex[0] != '\0'; ++pIndex)
        {
            if (pIndex[0] == ',')
            {
                pIndex[0] = '\0' ;
                ++pIndex ;
                m_objSettings.m_aComponent.push_back(p_pszComponents) ;
                p_pszComponents = pIndex ;
            }
            else if (pIndex[1] == '\0')
            {
                m_objSettings.m_aComponent.push_back(p_pszComponents) ;
            }
        }

        cAutoCS autoCSIOXml(m_hCSIOXml, true) ;
        cAutoCS autoCSIOEvent(m_hCSIOEvent, true) ;

        int nIndex, nICount, nJndex, nJCount ;

        // del components
        nJCount = m_objSettings.m_aComponent.size() ;
        for (nIndex = 0, nICount = m_aComponents.size(); nIndex < nICount; ++nIndex)
        {
            for (nJndex = 0; nJndex < nJCount; ++nJndex)
            {
                if (m_aComponents[nIndex].compare(m_objSettings.m_aComponent[nJndex], fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ)
                {
                    break ;
                }
            }

            if (nJndex != nJCount && !!m_aIO[nIndex])
            {
                continue ;
            }

            if (!!m_aIO[nIndex])
            {
                m_aIO[nIndex]->sysCloseObject() ;
                m_aIO[nIndex] = 0 ;
            }

            m_aIO.remove(nIndex) ;
            m_aComponents.remove(nIndex) ;
            --nIndex ;
            --nICount ;
        }

        // add components
        for (nIndex = 0, nICount = nJCount; nIndex < nICount; ++nIndex)
        {
            for (nJndex = 0, nJCount = m_aComponents.size(); nJndex < nJCount; ++nJndex)
            {
                if (m_objSettings.m_aComponent[nIndex].compare(m_aComponents[nJndex], fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ)
                {
                    break ;
                }
            }

            if (nJndex != nJCount)
            {
                continue ;
            }

            AddComponent(m_objSettings.m_aComponent[nIndex]) ;
        }

        blUpdate = true ;
    }

    if (pszTimeout && strlen(pszTimeout) && pr_strtoul)
    {
        int nMaxPeriod = pr_strtoul(pszTimeout, 0, 10) ;
        cDateTime dtMaxPeriod(cDateTime::zero) ;
        dtMaxPeriod.TimeShift(0, 0, nMaxPeriod) ;
        memcpy(m_objSettings.m_dtMaxPeriod, (const tDATETIME)dtMaxPeriod, sizeof(tDATETIME)) ;

        blUpdate = true ;
    }

    if (blUpdate)
    {
        // update settings
        cKLStatisticsSettings* pTaskSettings = 0 ;
        if (PR_SUCC(m_hTM->LockTaskSettings((hOBJECT)this, (cSerializable**)&pTaskSettings, cFALSE)))
        {
            if (pTaskSettings)
            {
                *pTaskSettings = m_objSettings ;
            }

            m_hTM->UnlockTaskSettings((hOBJECT)this, pTaskSettings, cTRUE) ;
        }
    }

    return error ;
}

tERROR CKLStatistics::StartSubscription()
{
    tERROR error = errOK ;

    _ERROR(StopSubscription()) ;

    tMsgHandlerDescr hdl[] = {
        { (hOBJECT)this, rmhLISTENER, pmc_EVENTS_IMPORTANT,        IID_ANY, PID_ANY, IID_ANY, PID_ANY },
        { (hOBJECT)this, rmhLISTENER, pmc_UPDATER_CRITICAL_REPORT, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
    };

    error = g_root->sysRegisterMsgHandlerList(hdl, countof(hdl)) ;

    sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(m_objSettings.m_strDataFolder), 0, 0) ;
    PR_TRACE((this, prtNOTIFY, "stat\tStartSubscription - DataFolder:%S", m_objSettings.m_strDataFolder.data())) ;

    cAutoCS autoCSIOXml(m_hCSIOXml, true) ;
    cAutoCS autoCSIOEvent(m_hCSIOEvent, true) ;

    for (tDWORD dwIndex = 0, dwCount = m_objSettings.m_aComponent.size(); dwIndex < dwCount; ++dwIndex)
    {
        AddComponent(m_objSettings.m_aComponent[dwIndex]) ;
    }

    PR_TRACE((this, prtNOTIFY, "stat\tStartSubscription - error=0x%08X", error)) ;

    return error ;
}

tERROR CKLStatistics::StopSubscription()
{
    tERROR error = errOK ;

    tMsgHandlerDescr hdl[] = {
        { (hOBJECT)this, rmhLISTENER, pmc_EVENTS_IMPORTANT,        IID_ANY, PID_ANY, IID_ANY, PID_ANY },
        { (hOBJECT)this, rmhLISTENER, pmc_UPDATER_CRITICAL_REPORT, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
    };

    sysUnregisterMsgHandlerList(hdl, countof(hdl)) ;

    cAutoCS autoCSIOXml(m_hCSIOXml, true) ;
    cAutoCS autoCSIOEvent(m_hCSIOEvent, true) ;

    for (tDWORD dwIndex = 0, dwCount = m_aIO.size(); dwIndex < dwCount; ++dwIndex)
    {
        if (!m_aIO[dwIndex])
        {
            continue ;
        }

        m_aIO[dwIndex]->sysCloseObject() ;
        m_aIO[dwIndex] = 0 ;
    }

    m_aIO.clear() ;
    m_aComponents.clear() ;

    return error ;
}

tERROR CKLStatistics::AddComponent(cStrObj& p_strComponent)
{
    tERROR error = errOK ;

    PR_TRACE((this, prtNOTIFY, "stat\tAddComponent(%s) - begin", (tSTRING)p_strComponent.c_str(cCP_ANSI))) ;

    cStrObj strFilePath(m_objSettings.m_strDataFolder) ;
    strFilePath.add_path_sect(p_strComponent) ;
    strFilePath += INFO_FILE_EXT ;

    m_aIO.push_back(0) ;
    tDWORD dwIndex =  m_aIO.size() - 1 ;

    tERROR err = errOK ;
    if (PR_SUCC(err = sysCreateObject((hOBJECT*)&m_aIO[dwIndex], IID_IO, PID_NATIVE_FIO)))
    {
        if (PR_SUCC(err = strFilePath.copy(m_aIO[dwIndex], pgOBJECT_FULL_NAME)))
        {
            m_aIO[dwIndex]->set_pgOBJECT_ORIGIN(OID_GENERIC_IO) ;
            m_aIO[dwIndex]->set_pgOBJECT_OPEN_MODE(fOMODE_OPEN_ALWAYS) ;
            m_aIO[dwIndex]->set_pgOBJECT_ACCESS_MODE(fACCESS_RW) ;
            err = m_aIO[dwIndex]->sysCreateObjectDone() ;
        }
    }

    if (PR_FAIL(err) && !!m_aIO[dwIndex])
    {
        m_aIO[dwIndex]->sysCloseObject() ;
        m_aIO[dwIndex] = 0 ;
    }

    PR_TRACE((this, prtNOTIFY, "stat\tAddComponent(err=0x%08X) - end", err)) ;

    m_aComponents.push_back(p_strComponent) ;

    return error ;
}

cStrObj CKLStatistics::GetInfoString()
{
    cStrObj strResult(GENERAL_INFO_TAG_BEGIN) ;

    /*** Get X64 Info ***/

    PrHMODULE hKernel32 = PrLoadLibrary(KERNEL_DLL, cCP_ANSI) ;
    if (!hKernel32)
    {
        return "" ;
    }

    VOID (WINAPI * pGetNativeSystemInfo)(LPSYSTEM_INFO) = (VOID (WINAPI *)(LPSYSTEM_INFO))PrGetProcAddress(hKernel32, FUN_GET_SYS_INFO) ;

    SYSTEM_INFO objSysInfo ;
    objSysInfo.wProcessorArchitecture = PROCESSOR_ARCHITECTURE_UNKNOWN ;
    if (pGetNativeSystemInfo)
    {
        pGetNativeSystemInfo(&objSysInfo) ;
    }
    else
    {
        ::GetSystemInfo(&objSysInfo) ;
    }

    bool blX64 = false ;
    switch (objSysInfo.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_IA64:
    case PROCESSOR_ARCHITECTURE_ALPHA64:
    case PROCESSOR_ARCHITECTURE_AMD64:
        blX64 = true ;
        break ;
    }

    /*** Get OS Info ***/

    OSVERSIONINFOEX objOSVer ;
    memset(&objOSVer, 0, sizeof(objOSVer)) ;
    objOSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX) ;
    if (!::GetVersionEx((OSVERSIONINFO*)&objOSVer))
    {
        objOSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;
        if (!::GetVersionEx((OSVERSIONINFO*)&objOSVer))
        {
            return "" ;
        }
    }

    char strCSDVersion[countof(objOSVer.szCSDVersion)] ;
    memcpy(strCSDVersion, objOSVer.szCSDVersion, sizeof(strCSDVersion)) ;

    strResult += OS_ATTR ;
    switch(objOSVer.dwPlatformId)
    {
    case VER_PLATFORM_WIN32s:
        strResult += WIN_31 ;
        break;

    case VER_PLATFORM_WIN32_WINDOWS:
        switch(objOSVer.dwMajorVersion)
        {
        case 4:
            switch(objOSVer.dwMinorVersion)
            {
            case 0:  strResult += WIN_95 ; break;
            case 10: strResult += WIN_98 ; break;
            case 90: strResult += WIN_ME ; break;
            }

            if (strCSDVersion[1] == 'A')
                strResult += WIN_SE ;
            else if (strCSDVersion[1] == 'B' || strCSDVersion[1] == 'C')
                strResult += WIN_OSR2 ;

            *strCSDVersion = L'\0' ;
            break;

        case 5:
            strResult += WIN_ME ;
            break;
        }
        break;

    case VER_PLATFORM_WIN32_NT:
        switch(objOSVer.dwMajorVersion)
        {
        case 3:
            strResult += WIN_NT_351 ;
            break;
        case 4:
            strResult += WIN_NT_40 ;
            break;
        case 5:
            switch(objOSVer.dwMinorVersion)
            {
            case 0:
                strResult += WIN_2000 ;
                break;

            case 1:
                if (objOSVer.wProductType == VER_NT_WORKSTATION)
                    strResult += WIN_XP ;
                else
                    strResult += WIN_2002 ;
                break;

            case 2:
                if (objOSVer.wProductType == VER_NT_WORKSTATION)
                    strResult += WIN_XP ;
                else
                {
                    strResult += WIN_2003 ;

                    if (::GetSystemMetrics(SM_SERVERR2))
                        strResult += WIN_R2 ;
                }

                break;
            }
            break;

        case 6:
            switch(objOSVer.dwMinorVersion)
            {
            case 0:
                if (objOSVer.wProductType == VER_NT_WORKSTATION)
                    strResult += WIN_VISTA ;
                else
                    strResult += WIN_LONGHORN ;
                break;
            }

            break;
        }

        if (strResult.at(strResult.length() - 1, cCP_ANSI) == '\"')
        {
            char strUnknownVer[100] ;
            _snprintf(strUnknownVer, countof(strUnknownVer), WIN_NT, objOSVer.dwMajorVersion, objOSVer.dwMinorVersion) ;
            strResult += strUnknownVer ;

        }

        switch(objOSVer.wProductType)
        {
        case VER_NT_WORKSTATION:
            if (objOSVer.dwMajorVersion < 5)
                strResult += WIN_WORKSTATION ;
            else if (objOSVer.dwMajorVersion == 5)
            {
                if (objOSVer.wSuiteMask & VER_SUITE_PERSONAL)
                    strResult += WIN_HOME_EDITION ;
                else if (objOSVer.wSuiteMask & VER_SUITE_EMBEDDEDNT)
                    strResult += WIN_EMBEDDED ;
                else if (::GetSystemMetrics(SM_MEDIACENTER))
                    strResult += WIN_MEDIA_CENTER ;
                else if( ::GetSystemMetrics(SM_STARTER) )
                    strResult += WIN_STARTER_EDITION ;
                else if (::GetSystemMetrics(SM_TABLETPC))
                    strResult += WIN_TABLE_PC ;
                else
                    strResult += WIN_PROFESSIONAL ;
            }
            else if (objOSVer.dwMajorVersion == 6)
            {
                if (objOSVer.wSuiteMask & VER_SUITE_PERSONAL)
                    strResult += WIN_HOME ;
            }

            break;

        case VER_NT_DOMAIN_CONTROLLER:
        case VER_NT_SERVER:
            if (objOSVer.dwMajorVersion < 5)
            {
                strResult += WIN_SERVER ;

                if (objOSVer.wSuiteMask & VER_SUITE_ENTERPRISE)
                    strResult += WIN_ENTERPRISE ;
            }
            else if (objOSVer.dwMajorVersion == 5 && objOSVer.dwMinorVersion == 0)
            {
                if (objOSVer.wSuiteMask & VER_SUITE_DATACENTER)
                    strResult += WIN_DATACENTER ;
                else if (objOSVer.wSuiteMask & VER_SUITE_ENTERPRISE)
                    strResult += WIN_ADVANCED ;
                else if (objOSVer.wSuiteMask & VER_SUITE_BLADE)
                    strResult += WIN_WEB ;
                else
                    strResult += WIN_STANDARD ;

                strResult += WIN_SERVER ;
            }
            else
            {
                if (objOSVer.wSuiteMask & VER_SUITE_STORAGE_SERVER)
                    strResult += WIN_STORAGE ;

                strResult += WIN_SERVER ;

                if (objOSVer.wSuiteMask & VER_SUITE_ENTERPRISE)
                    strResult += WIN_ENTERPRISE ;
                else if (objOSVer.wSuiteMask & VER_SUITE_DATACENTER)
                    strResult += WIN_DATACENTER_EDITION ;
                else if (objOSVer.wSuiteMask & VER_SUITE_BLADE)
                    strResult += WIN_WEB_EDITION ;
                else if (objOSVer.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
                    strResult += WIN_CLUSTER_EDITION ;
                else if ((objOSVer.wSuiteMask & (VER_SUITE_SMALLBUSINESS|VER_SUITE_SMALLBUSINESS_RESTRICTED)) == (VER_SUITE_SMALLBUSINESS|VER_SUITE_SMALLBUSINESS_RESTRICTED))
                    strResult += WIN_SMALL_BUSINESS ;
                else
                    strResult += WIN_STANDARD_EDITION ;
            }

            if (objOSVer.wProductType == VER_NT_DOMAIN_CONTROLLER)
                strResult += WIN_DOMAIN_CONTROLLER ;

            break;
        }

        break;
    }

    if (blX64)
    {
        strResult += WIN_X64 ;
    }

    strResult += ATTR_END ;

    /*** Get SP Info ***/

    if (strCSDVersion[0])
    {
        strResult += SP_ATTR ;
        strResult += strCSDVersion ;
        strResult += ATTR_END ;
    }

    /*** Get AppID Info ***/

    cStrObj strAppID(ENV_PRODUCT_TYPE) ;
    if (PR_SUCC(sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strAppID), 0, 0)) &&
        strAppID.length())
    {
        strResult += APPID_ATTR ;
        strResult += strAppID ;
        strResult += ATTR_END ;
    }

    /*** Get Build Info ***/

    cStrObj strBuild(ENV_PRODUCT_VERSION) ;
    if (PR_SUCC(sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strBuild), 0, 0)) &&
        strBuild.length())
    {
        strResult += BUILD_ATTR ;
        strResult += strBuild ;
        strResult += ATTR_END ;
    }

    strResult += GENERAL_INFO_TAG_END ;
    return strResult ;
}


// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR CKLStatistics::Register( hROOT root ) 
{
    tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
    mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
    mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Statistics module", 18 )
    mpLOCAL_PROPERTY_BUF( pgTASK_TM, CKLStatistics, m_hTM, cPROP_BUFFER_READ_WRITE )
    mpLOCAL_PROPERTY_BUF( pgTASK_STATE, CKLStatistics, m_dwTaskState, cPROP_BUFFER_READ )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
    mINTERNAL_METHOD(ObjectInitDone)
    mINTERNAL_METHOD(ObjectPreClose)
    mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Task)
    mEXTERNAL_METHOD(Task, SetSettings)
    mEXTERNAL_METHOD(Task, GetSettings)
    mEXTERNAL_METHOD(Task, AskAction)
    mEXTERNAL_METHOD(Task, SetState)
    mEXTERNAL_METHOD(Task, GetStatistics)
mEXTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

    PR_TRACE_FUNC_FRAME_( *root, "Task::Register method", &error );

    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_TASK,                               // interface identifier
        PID_KLSTATISTICS,                       // plugin identifier
        0x00000000,                             // subtype identifier
        Task_VERSION,                           // interface version
        VID_GUSCHIN,                            // interface developer
        &i_Task_vtbl,                           // internal(kernel called) function table
        (sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
        &e_Task_vtbl,                           // external function table
        (sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
        Task_PropTable,                         // property table
        mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
        sizeof(CKLStatistics)-sizeof(cObjImpl), // memory size
        IFACE_SYSTEM                            // interface flags
    );

    #ifdef _PRAGUE_TRACE_
        if ( PR_FAIL(error) )
            PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
    #endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
    return error;
}

tERROR pr_call Task_Register( hROOT root ) { return CKLStatistics::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_SESSION_ID
// You have to implement propetry: pgTASK_ID
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// You have to implement propetry: pgTASK_PARENT_ID
// AVP Prague stamp end



