// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  16 April 2007,  18:02 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- licensing.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

#pragma warning(disable : 4786)

// AVP Prague stamp begin( Interface version,  )
#define Licensing_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "licensing.h"
// AVP Prague stamp end



#include <pr_cpp.h>
#include <pr_time.h>

#include <memory>

#include <time.h>

/************************************************************************/
/* AppInfos                                                             */
/************************************************************************/

namespace KAV8BETA
{
	#undef __APPLICATION_INFO_H__
	#include "1264b.h"
}
namespace KAV8REL
{
	#undef __APPLICATION_INFO_H__
	#include "1264r.h"
}
namespace KIS8BETA
{
	#undef __APPLICATION_INFO_H__
	#include "1265b.h"
}
namespace KIS8REL
{
	#undef __APPLICATION_INFO_H__
	#include "1265r.h"
}

namespace KAV7BETA
{
	#undef __APPLICATION_INFO_H__
	#include "1229b.h"
}
namespace KAV7REL
{
	#undef __APPLICATION_INFO_H__
	#include "1229r.h"
}
namespace KIS7BETA
{
	#undef __APPLICATION_INFO_H__
	#include "1230b.h"
}
namespace KIS7REL
{
	#undef __APPLICATION_INFO_H__
	#include "1230r.h"
}

namespace WKS6BETA
{	
	#undef __APPLICATION_INFO_H__
	#include "109b.h"
}
namespace WKS6REL
{	
	#undef __APPLICATION_INFO_H__
	#include "109r.h"
}

namespace FS6BETA
{	
	#undef __APPLICATION_INFO_H__
	#include "115b.h"
}
namespace FS6REL
{		
	#undef __APPLICATION_INFO_H__
	#include "115r.h"
}

namespace SOS5BETA
{	
	#undef __APPLICATION_INFO_H__
	#include "1195b.h"
}
namespace SOS5REL
{	
	#undef __APPLICATION_INFO_H__
	#include "1195r.h"
}

namespace KAV7BETAJP
{	
	#undef __APPLICATION_INFO_H__
	#include "1241_0b.h"
}
namespace KAV7RELJP
{	
	#undef __APPLICATION_INFO_H__
	#include "1241_0r.h"
}
namespace KIS7BETAJP
{	
	#undef __APPLICATION_INFO_H__
	#include "1242_0b.h"
}
namespace KIS7RELJP
{	
	#undef __APPLICATION_INFO_H__
	#include "1242_0r.h"
}

/************************************************************************/
/************************************************************************/

#include "ver_mod.h"

#include <plugin/p_win32_reg.h>
#include <iface/i_reg.h>
#include <structs/s_bl.h>

#define IMPEX_IMPORT_LIB
    #include <iface/e_ktime.h>
    #include <iface/e_loader.h>
    #include <plugin/p_win32_nfio.h>

#include <atlbase.h>

#define H2E(h)  GetError(h)

#define _ERR_CONVERT(exp)                       \
    error = exp ;                               \
    if (PR_FAIL(error))                         \
    {                                           \
        return error ;                          \
    }

#define PRODUCT_TYPE            "%ProductType%"
#define PRODUCT_STATUS          "%ProductStatus%"
#define PRODUCT_LOCALIZATION	"%Localization%"
#define PRODUCT_BASES           "%Bases%"

#define PRODUCT_KAV             "kav"
#define PRODUCT_KIS             "kis"
#define PRODUCT_WKS             "wks"
#define PRODUCT_FS              "fs"
#define PRODUCT_SOS             "sos"
#define PRODUCT_STATUS_BETA     "beta"

#define PRODUCT_LOCALE_JP		"jp"
//#ifndef _DEBUG
    #define USES_BLACK_LIST
//#endif // _DEBUG

DataTree::string_t SkipRN(DataTree::string_t& str)
{
    DWORD l=0;
    if( !str.empty() )
        while( str[l] == ' ' || str[l] == '\t' || str[l] == '\r' || str[l] == '\n' )
            l++;

    DataTree::string_t strResult = str.c_str() + l ;
    int nPos = strResult.npos ;

    while ((nPos = strResult.find("\r\n\r\n")) != strResult.npos)
    {
        strResult.replace(nPos, 4, "\r\n") ;
    }

    nPos = strResult.npos ;
    while ((nPos = strResult.find("\n\n")) != strResult.npos)
    {
        strResult.replace(nPos, 2, "\n") ;
    }
    
    return strResult ;
}

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Licensing". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR CLicensing60::ObjectInit()
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::ObjectInit method") ;

    error = CALL_SYS_ObjectCreate(this, &m_hSync, IID_MUTEX, PID_ANY, SUBTYPE_ANY);
    if (PR_SUCC(error))
    {
        error = CALL_SYS_PropertySetStr(m_hSync, NULL, pgOBJECT_NAME, "avp6syncbla-blalic", sizeof("avp6syncbla-blalic"), cCP_ANSI);
        if (PR_SUCC(error))
            error = CALL_SYS_ObjectCreateDone(m_hSync);
    }

    if (PR_SUCC(error))
    {
        error = sysCreateObjectQuick((hOBJECT*)&m_hBasesPath, IID_STRING) ;
    }

    m_blUsesBlacklist = cTRUE ;

    return error ;
}
// AVP Prague stamp end



#define clicensing60_producttype(_name) (strProductType.compare(_name, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) == cSTRING_COMP_EQ)
#define clicensing60_productbeta()      (strProductStatus.compare(0, strlen(PRODUCT_STATUS_BETA), PRODUCT_STATUS_BETA, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE ) == cSTRING_COMP_EQ)
#define clicensing60_productlocale(_name)	(strProductLocale.compare(_name, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ)

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CLicensing60::ObjectInitDone()
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::ObjectInitDone method") ;

    if (!m_hStorage)
    {
        PR_TRACE((this, prtERROR, "lic\tm_hStorage==NULL, initialization failed with errOBJECT_NOT_INITIALIZED"));
        return errOBJECT_NOT_INITIALIZED ;
    }

    cStrObj strProductType(PRODUCT_TYPE) ;
    sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strProductType), 0, 0) ;

    cStrObj strProductStatus(PRODUCT_STATUS) ;
    sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strProductStatus), 0, 0) ;

	cStrObj strProductLocale(PRODUCT_LOCALIZATION);
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strProductLocale), 0, 0) ;

    m_appInfo = NULL ;
    m_nProdId = 0 ;
    if (clicensing60_producttype(PRODUCT_KAV))
    {
		m_appInfo = clicensing60_productbeta() ? &KAV8BETA::AppInfo : &KAV8REL::AppInfo;
		m_nProdId = 1264;
    }
    else if(clicensing60_producttype(PRODUCT_KIS))
    {
		m_appInfo = clicensing60_productbeta() ? &KIS8BETA::AppInfo : &KIS8REL::AppInfo;
		m_nProdId = 1265;
    }

    if (m_appInfo == NULL)
        return errOBJECT_NOT_INITIALIZED ;

    m_strListStg += strProductType;

    m_hSync->Lock(cSYNC_INFINITE);

    bool bListStgReset = !_CheckLicDataReg();

    m_licenseStorage.readSecureData  = readLicData ;
    m_licenseStorage.writeSecureData = writeLicData ;

    if (PR_FAIL(error = ReinitLicensing()))
        return error ;

    if( bListStgReset )
    {
        cCheckInfoList cKeys;
        GetInstalledKeysInfo(&cKeys);
        RevokeActiveKey();

        for(int i = 0, n = cKeys.m_listCheckInfo.size(); i < n; i++)
            RevokeKey(&cKeys.m_listCheckInfo[i].m_KeyInfo.m_KeySerNum);
    }

    m_hSync->Release();

	m_blGetPassForProxyFromUser = cTRUE;

    error = sysCreateObjectQuick((hOBJECT*)&m_hCSOnline, IID_CRITICAL_SECTION) ;
    if (PR_FAIL(error))
    {
        PR_TRACE((this, prtERROR, "lic\tm_hCSOnline initialization failed with %terr", error));
        return error ;
    }

    cStrObj strBlackListFile = PRODUCT_BASES ;
    sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strBlackListFile), 0, 0) ;
    strBlackListFile.add_path_sect(BLACK_LIST_FILE_NAME) ;
    m_strBlackListFile = (_TCHAR*)strBlackListFile.c_str(sizeof(_TCHAR) != 1 ? cCP_UNICODE : cCP_ANSI) ;

    sysRegisterMsgHandler(pmc_LICENSING, rmhDECIDER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
    return error;
}
// AVP Prague stamp end



tERROR CLicensing60::ReinitLicensing()
{
    m_hSync->Lock(cSYNC_INFINITE);

    if( m_licPolicy )
    {
        delete m_licPolicy;
        m_licPolicy = NULL;
    }

    _LP(string_list_t) aBasesPath ;
    aBasesPath.push_back((_TCHAR*)cStrBuff(m_hBasesPath, sizeof(_TCHAR) != 1 ? cCP_UNICODE: cCP_ANSI)) ;

    m_licPolicy = new _LP(CLicensingPolicy)();
    if( !m_licPolicy )
    {
        m_hSync->Release();
        return errNOT_ENOUGH_MEMORY;
    }

    PR_TRACE((this, prtIMPORTANT, "lic\tInitializing from %tstr ", m_hBasesPath));
    tERROR error = H2E(m_licPolicy->init(&m_licenseStorage, *m_appInfo, this, aBasesPath, _ILP(cmKeyOnly), true)) ;
    if (PR_FAIL(error))
        PR_TRACE((this, prtERROR, "lic\tm_licPolicy initialization failed with %terr", error));

    m_hSync->Release();
    return error ;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CLicensing60::ObjectPreClose()
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::ObjectPreClose method") ;

    if ( m_hBasesPath )
    {
    m_hBasesPath->sysCloseObject() ;
    m_hBasesPath = NULL ;
    }

    if (m_hSync)
    {
        CALL_SYS_ObjectClose(m_hSync);
        m_hSync = 0;
    }

    if (m_hCSOnline)
    {
        m_hCSOnline->sysCloseObject() ;
    }

    if( m_licPolicy )
        delete m_licPolicy;

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
tERROR CLicensing60::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
    tERROR error = errOK;
    PR_TRACE_FUNC_FRAME( "Licensing::MsgReceive method" );

    if (p_msg_cls_id == pmc_LICENSING)
    {
        switch(p_msg_id)
        {
        case pm_LICENSE_REPLACE:
            {
                if (!p_par_buf && *p_par_buf_len != 0)
                    return errPARAMETER_INVALID;
                if (!writeLicData((const char*)p_par_buf, *p_par_buf_len, this))
                    return errUNEXPECTED;
                ReinitLicensing();

                if( m_licPolicy )
                {
                    m_hSync->Lock(cSYNC_INFINITE);
                    m_licPolicy->clearTrialLimitDate();
                    m_hSync->Release();
                }

                sysSendMsg(pmc_LICENSING, pm_DATA_CHANGED, NULL, NULL, NULL);
                return errOK_DECIDED;
            }
        case pm_GET_PRODUCT_IDENTIFIER:
            {
                if(!p_par_buf)
                    return errPARAMETER_INVALID;

                cLicProductIdentifier *productIdentifier = reinterpret_cast<cLicProductIdentifier *>(p_par_buf);
                if(!productIdentifier->isBasedOn(cLicProductIdentifier::eIID))
                    return errPARAMETER_INVALID;
                productIdentifier->m_nProdId = m_nProdId;
            }
            break;
        }
    }

    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetActiveKeyInfo )
// Parameters are:
tERROR CLicensing60::GetActiveKeyInfo( cSerializable* p_pResult )
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::GetActiveKeyInfo method") ;

    if (p_pResult == NULL || !p_pResult->isBasedOn(cKeyInfo::eIID))
    {
        PR_TRACE((this, prtERROR, "lic\tGetActiveKeyInfo: parameter invalid"));
        return errPARAMETER_INVALID ;
    }

    m_hSync->Lock(cSYNC_INFINITE);

    _LP(key_info_t) key_info ;
    error = H2E(m_licPolicy->getActiveKeyInfo(&key_info)) ;
    if (PR_FAIL(error))
    {
        PR_TRACE((this, prtERROR, "lic\tGetActiveKeyInfo: m_licPolicy->getActiveKeyInfo failed with %terr", error));
        m_hSync->Release();
        return error ;
    }

    cKeyInfo * pKeyInfo = (cKeyInfo*)p_pResult;

    error = ConvertKeyInfo_L(key_info, pKeyInfo) ;
    if (PR_FAIL(error))
    {
        PR_TRACE((this, prtERROR, "lic\tGetActiveKeyInfo: ConvertKeyInfo_L failed with %terr", error));
        m_hSync->Release();
        return error ;
    }

    m_hSync->Release();

    if( !IsKeyForProduct(pKeyInfo) )
    {
        *pKeyInfo = cKeyInfo();
        return errNO_ACTIVE_KEY;
    }

    return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetInstalledKeysInfo )
// Parameters are:
tERROR CLicensing60::GetInstalledKeysInfo( cSerializable* p_pResult )
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::GetInstalledKeysInfo method") ;

    PR_TRACE((this, prtNOTIFY, "lic\tCLicensing60::GetInstalledKeysInfo beginning..."));

    if (p_pResult == NULL || !p_pResult->isBasedOn(cCheckInfoList::eIID))
    {
        return errPARAMETER_INVALID ;
    }

    m_hSync->Lock(cSYNC_INFINITE);

    _ILP(check_info_list_t) check_info_list ;
    error = H2E(m_licPolicy->getInstalledKeysInfo(&check_info_list)) ;
    if (PR_FAIL(error))
    {
        m_hSync->Release();
        return error ;
    }

    _CLP(installed_key_info_list_t) installed_key_info_list ;
    error = H2E(m_licPolicy->getInstalledKeysInfo(&installed_key_info_list)) ;
    if (PR_FAIL(error))
    {
        m_hSync->Release();
        return error ;
    }

    cCheckInfoList * pList = (cCheckInfoList*)p_pResult;
    error = ConvertCheckInfoList_L(check_info_list, installed_key_info_list, pList) ;

    m_hSync->Release();

    for(tDWORD i = pList->m_listCheckInfo.size(); i > 0; i--)
        if( !IsKeyForProduct(&pList->m_listCheckInfo[i - 1].m_KeyInfo) )
            pList->m_listCheckInfo.remove(i - 1);

    return error ;
}
// AVP Prague stamp end



bool CLicensing60::IsKeyForProduct(cKeyInfo * pInfo)
{
    if( pInfo->m_dwAppID == m_appInfo->id )
        return true;

    for(_uint i = 0; i < m_appInfo->appCompatList.appsNumber; i++)
        if( m_appInfo->appCompatList.appsList[i].appId == pInfo->m_dwAppID )
            return true;
    
    return false;
}

bool CLicensing60::GetInstallKeyInfo(_ILP(check_info_t)& p_rCheckStruct, 
                                     _LP(installed_key_info_t)& p_rInstallStruct)
{
    tERROR error = errOK ;

    PR_TRACE((this, prtNOTIFY, "lic\tCLicensing60::GetInstallKeyInfo beginning..."));
    PR_TRACE((this, prtNOTIFY, "lic\tKey: %08X - %08X - %08X", p_rCheckStruct.keyInfo.keySerNum.number.parts.memberId, p_rCheckStruct.keyInfo.keySerNum.number.parts.appId, p_rCheckStruct.keyInfo.keySerNum.number.parts.keySerNum));

    _CLP(installed_key_info_list_t) installed_key_info_list ;
    error = H2E(m_licPolicy->getInstalledKeysInfo(&installed_key_info_list)) ;
    if (PR_FAIL(error))
    {
        PR_TRACE((this, prtNOTIFY, "lic\tCLicensing60::GetInstallKeyInfo end - failed 1"));
        return false ;
    }

    for (size_t unJndex = 0, unInstallCount = installed_key_info_list.size(); unJndex < unInstallCount; ++unJndex)
    {
        PR_TRACE((this, prtNOTIFY, "lic\tInstalled keys: %08X - %08X - %08X", installed_key_info_list[unJndex].serialNumber.number.parts.memberId, installed_key_info_list[unJndex].serialNumber.number.parts.appId, installed_key_info_list[unJndex].serialNumber.number.parts.keySerNum));

        if (p_rCheckStruct.keyInfo.keySerNum.number.parts.memberId  == installed_key_info_list[unJndex].serialNumber.number.parts.memberId &&
            p_rCheckStruct.keyInfo.keySerNum.number.parts.appId     == installed_key_info_list[unJndex].serialNumber.number.parts.appId &&
            p_rCheckStruct.keyInfo.keySerNum.number.parts.keySerNum == installed_key_info_list[unJndex].serialNumber.number.parts.keySerNum)
        {
            p_rInstallStruct = installed_key_info_list[unJndex] ;

            PR_TRACE((this, prtNOTIFY, "lic\tCLicensing60::GetInstallKeyInfo end - successful"));
            return true ;
        }
    }

    PR_TRACE((this, prtNOTIFY, "lic\tCLicensing60::GetInstallKeyInfo end - failed 2"));
    return false ;
}

tDWORD CLicensing60::UpdateKeyAddMode(tDWORD p_dwKeyAddMode)
{
    if (p_dwKeyAddMode != KAM_REPLACE)
    {
        _LP(key_info_t) key_info ;
        cKeyInfo _ActiveInfo;

        if (PR_SUCC(H2E(m_licPolicy->getActiveKeyInfo(&key_info))) &&
            PR_SUCC(ConvertKeyInfo_L(key_info, &_ActiveInfo)))
        {
            if (!IsKeyForProduct(&_ActiveInfo))
            {
                p_dwKeyAddMode = KAM_REPLACE ;
            }

            switch (_ActiveInfo.m_dwKeyType)
            {
            case ektBeta:
            case ektTrial:
            case ektTest:
            case ektOEM:
                p_dwKeyAddMode = KAM_REPLACE ;
                break ;
            default:
                break ;
            }
        }
    }

    return p_dwKeyAddMode ;
}



// AVP Prague stamp begin( External (user called) interface method implementation, RevokeActiveKey )
// Parameters are:
tERROR CLicensing60::RevokeActiveKey()
{

    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::RevokeActiveKey method") ;

    m_hSync->Lock(cSYNC_INFINITE);
    error = H2E(m_licPolicy->revokeActiveKey()) ;
    m_hSync->Release();

    if (PR_SUCC(error))
    {
        sysSendMsg(pmc_LICENSING, pm_DATA_CHANGED, NULL, NULL, NULL) ;
    }

    return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, RevokeKey )
// Parameters are:
tERROR CLicensing60::RevokeKey( cSerializable* p_pKeySerialNumber )
{

    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::RevokeKey method") ;

    m_hSync->Lock(cSYNC_INFINITE) ;

    _LP(key_serial_number_t) key_serial_number ;
    error = ConvertKeySerialNumber_P(p_pKeySerialNumber, key_serial_number) ;
    if (PR_FAIL(error))
    {
        m_hSync->Release() ;
        return error ;
    }

    error = H2E(m_licPolicy->revokeKey(key_serial_number)) ;
    m_hSync->Release() ;

    if (PR_SUCC(error))
    {
        sysSendMsg(pmc_LICENSING, pm_DATA_CHANGED, NULL, NULL, NULL) ;
    }

    return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CheckKeyFile )
// Parameters are:
tERROR CLicensing60::CheckKeyFile( tWSTRING p_szKeyFileName, tDWORD p_dwKeyAddMode, cSerializable* p_pResult )
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::CheckKeyFile method" ) ;

    if (p_pResult == NULL || !p_pResult->isBasedOn(cCheckInfo::eIID))
    {
        return errPARAMETER_INVALID ;
    }

    cStringObj    strKeyFileName(p_szKeyFileName) ;
    cAutoObj<cOS> hos ;
    if (!wcsncmp(p_szKeyFileName, L"\\\\", 2)) // unc path
    {
        cStringObj strKeyLocalPathTemp ;
        PrGetTempDir(cAutoString(strKeyLocalPathTemp), NULL, cCP_ANSI) ;
        tDWORD nPos = strKeyFileName.find_last_of("\\/") ;
        strKeyLocalPathTemp.add_path_sect(nPos != cStringObj::npos ? strKeyFileName.substr(nPos + 1) : strKeyFileName) ;

        error = PrCopy(cAutoString(strKeyFileName), cAutoString(strKeyLocalPathTemp), fPR_FILE_REPLACE_EXISTING|fPR_FILE_NAME_PREPARSING) ;
        if (PR_FAIL(error))
        {
            PR_TRACE((this, prtERROR, "LIC\tCLicensing60::CheckKeyFile. can't copy lic file from %S to %S. (%terr)", p_szKeyFileName, strKeyLocalPathTemp.data(), error)) ;
            return error ;
        }

        strKeyFileName = strKeyLocalPathTemp ;

        sysCreateObjectQuick(hos, IID_OS, PID_NATIVE_FIO) ;
    }

    PR_TRACE((this, prtIMPORTANT, "LIC\tCLicensing60::CheckKeyFile. file name to check is %S", strKeyFileName.data())) ;

    p_dwKeyAddMode = UpdateKeyAddMode(p_dwKeyAddMode) ;

    m_hSync->Lock(cSYNC_INFINITE) ;

    _LP(string_t) key_file_name;
	if( sizeof(_TCHAR) != 1 )
		key_file_name = strKeyFileName.c_str(cCP_UNICODE) ;
	else
	{
		USES_CONVERSION;
		key_file_name = W2A(strKeyFileName.data());
	}

    _ILP(key_add_mode_t) key_add_mode = ConvertKeyAddMode_P(p_dwKeyAddMode) ;
    _ILP(check_info_t) check_info ;
    error = H2E(m_licPolicy->checkKeyFile(key_file_name, &check_info, key_add_mode)) ;
    if (PR_FAIL(error))
    {
        PR_TRACE((this, prtERROR, "LIC\tLicensing::checkKeyFile: library returned %terr", error)) ;
        m_hSync->Release() ;

        if (!!hos)
        {
            hos->Delete(cAutoString(strKeyFileName)) ;
        }

        return error ;
    }

    _LP(installed_key_info_t) install_info ;
    bool blInstallInfo = GetInstallKeyInfo(check_info, install_info) ;

    error = ConvertCheckInfo_L(check_info, blInstallInfo ? &install_info : NULL, (cCheckInfo*)p_pResult) ;
    if (PR_FAIL(error))
    {
        m_hSync->Release() ;

        if (!!hos)
        {
            hos->Delete(cAutoString(strKeyFileName)) ;
        }

        return error ;
    }

#ifdef USES_BLACK_LIST

    if (m_blUsesBlacklist)
    {
        error = H2E(m_licPolicy->checkKeyBlacklist(key_file_name, m_strBlackListFile)) ;
        if (PR_FAIL(error))
        {
            cCheckInfo* pInfo = (cCheckInfo*)p_pResult ;

            PR_TRACE((this, prtERROR, "LIC\tLicensing::checkKeyBlacklist: library returned %terr", error)) ;
            if (error != errKEY_IS_BLACKLISTED)
            {
                pInfo->m_dwFuncLevel = eflFullFunctionality ;
                pInfo->m_dwInvalidReason = ekirInvalidBlacklist ;
            }
            else
            {
                pInfo->m_dwFuncLevel = eflFullFunctionality ;
                pInfo->m_dwInvalidReason = ekirBlackListed ;
            }

            error = errOK ;
        }
    }

#endif // USES_BLACK_LIST

    m_hSync->Release() ;

    if (!!hos)
    {
        hos->Delete(cAutoString(strKeyFileName)) ;
    }

    return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CheckActiveKey )
// Parameters are:
tERROR CLicensing60::CheckActiveKey( cSerializable* p_pResult )
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::CheckActiveKey method") ;

    PR_TRACE((this, prtNOTIFY, "lic\tCLicensing60::CheckActiveKey beginning..."));

    if (p_pResult == NULL || !p_pResult->isBasedOn(cCheckInfo::eIID))
    {
        return errPARAMETER_INVALID ;
    }

    m_hSync->Lock(cSYNC_INFINITE) ;

    _ILP(check_info_t) check_info ;
    error = H2E(m_licPolicy->checkActiveKey(&check_info)) ;
    if (PR_FAIL(error))
    {
        PR_TRACE((this, prtERROR, "Licensing::checkActiveKey: library returned %terr", error)) ;
        m_hSync->Release() ;
        return error ;
    }

    cCheckInfo * pCheckInfo = (cCheckInfo*)p_pResult;

    _LP(installed_key_info_t) install_info ;
    bool blInstallInfo = GetInstallKeyInfo(check_info, install_info) ;

    error = ConvertCheckInfo_L(check_info, blInstallInfo ? &install_info : NULL, pCheckInfo) ;

    if (PR_FAIL(error) || pCheckInfo->m_dwInvalidReason != ekirValid )
    {
        m_hSync->Release() ;

        if( !PR_FAIL(error) && !IsKeyForProduct(&pCheckInfo->m_KeyInfo) )
        {
            *pCheckInfo = cCheckInfo();
            return errNO_ACTIVE_KEY;
        }

        return error ;
    }

#ifdef USES_BLACK_LIST

    if (m_blUsesBlacklist)
    {
        error = H2E(m_licPolicy->checkBlacklistFile(m_strBlackListFile)) ;
        if (PR_FAIL(error))
        {
            PR_TRACE((this, prtERROR, "Licensing::checkBlacklistFile: library returned %terr", error)) ;
            if (error != errKEY_IS_BLACKLISTED)
            {
                pCheckInfo->m_dwFuncLevel = eflFullFunctionality ;
                pCheckInfo->m_dwInvalidReason = ekirInvalidBlacklist ;
            }
            else
            {
                pCheckInfo->m_dwFuncLevel = eflFullFunctionality ;
                pCheckInfo->m_dwInvalidReason = ekirBlackListed ;
            }

            error = errOK ;
        }
    }

#endif // USES_BLACK_LIST

    m_hSync->Release() ;
    return error ;
}
// AVP Prague stamp end

class cCoIn
{
	bool m_bInitSucceeded;
public:
	cCoIn() { m_bInitSucceeded = SUCCEEDED(CoInitialize(NULL)); }
	~cCoIn() { if(m_bInitSucceeded) CoUninitialize(); }
};


// AVP Prague stamp begin( External (user called) interface method implementation, AddKey )
// Parameters are:
tERROR CLicensing60::AddKey( tWSTRING p_szFileName, tDWORD p_dwKeyAddMode )
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::AddKey method") ;

    PR_TRACE((this, prtNOTIFY, "CLicensing60::AddKey: adding '%S'", p_szFileName)) ;

    cStringObj strKeyFileName(p_szFileName);
    cAutoObj<cIO> pTmpIo;
    if (!wcsncmp(p_szFileName, L"\\\\", 2))
    {
        cStringObj strKeyLocalPathTemp;
        PrGetTempDir(cAutoString(strKeyLocalPathTemp), NULL, cCP_ANSI);
        tDWORD nPos = strKeyFileName.find_last_of("\\/");
        strKeyLocalPathTemp.add_path_sect(nPos != cStringObj::npos ? strKeyFileName.substr(nPos + 1) : strKeyFileName);

        error = g_root->sysCreateObject(pTmpIo, IID_IO, PID_NATIVE_FIO);
        if( PR_SUCC(error) ) error = strKeyLocalPathTemp.copy(pTmpIo, pgOBJECT_FULL_NAME);
        if( PR_SUCC(error) ) error = pTmpIo->set_pgOBJECT_ACCESS_MODE(fACCESS_WRITE);
        if( PR_SUCC(error) ) error = pTmpIo->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
        if( PR_SUCC(error) ) error = pTmpIo->set_pgOBJECT_OPEN_MODE(fOMODE_CREATE_ALWAYS|fOMODE_SHARE_DENY_DELETE|fOMODE_SHARE_DENY_WRITE);
        if( PR_SUCC(error) ) error = pTmpIo->sysCreateObjectDone();
        if( PR_SUCC(error) ) error = PrCopy(cAutoString(strKeyFileName), pTmpIo, 0);
        if (PR_FAIL(error))
        {
            PR_TRACE((this, prtERROR, "LIC\tCLicensing60::AddKey. can't copy lic file from %S to %S. (%terr)", p_szFileName, strKeyLocalPathTemp.data(), error));
            return error;
        }
        strKeyFileName = strKeyLocalPathTemp;
        p_szFileName = (tWSTRING)strKeyFileName.data();
    }

	cCoIn coin;

    PR_TRACE((this, prtNOTIFY, "Licensing::AddKey method started")) ;

    m_hSync->Lock(cSYNC_INFINITE) ;

    p_dwKeyAddMode = UpdateKeyAddMode(p_dwKeyAddMode) ;

// {{{{ Блокировка добавления просроченных коммерческих ключей
    cCheckInfo _AddKeyInfo;
    if( PR_FAIL(error = CheckKeyFile(p_szFileName, p_dwKeyAddMode, &_AddKeyInfo)) )
    {
        m_hSync->Release() ;
        return error;
    }

    if( _AddKeyInfo.m_KeyInfo.m_dwKeyType == ektCommercial && _AddKeyInfo.m_dwInvalidReason == ekirExpired )
    {
        m_hSync->Release() ;
        return cDateTime(&_AddKeyInfo.m_KeyInfo.m_dtKeyExpDate) > cDateTime::now_utc() ? errKEY_INST_FUTURE_EXP : errKEY_INST_EXP;
    }
// }}}} Блокировка добавления просроченных коммерческих ключей

    _LP(string_t) file_name;
	if( sizeof(_TCHAR) != 1 )
		file_name = strKeyFileName.c_str(cCP_UNICODE) ;
	else
	{
		USES_CONVERSION;
		file_name = W2A(strKeyFileName.data());
	}

    PR_TRACE((this, prtNOTIFY, "Licensing::AddKey: adding '%S'", strKeyFileName.data())) ;

    HRESULT hr = S_OK ;

#ifdef USES_BLACK_LIST

// Blacklist only for check
//    hr = m_licPolicy->checkKeyBlacklist(file_name, m_strBlackListFile) ;
//    error = H2E(hr) ;
//
//    PR_TRACE((this, prtNOTIFY, "Licensing::checkKeyBlacklist: library returned 0x%X (%d)", hr,
//             m_licPolicy->getStoredError())) ;
//
//    PR_TRACE((this, prtNOTIFY, "Licensing::checkKeyBlacklist: converted %terr", error)) ;
//    if (PR_FAIL(error) && error != errKEY_IS_BLACKLISTED)
//    {
//        error = errBLACKLIST_CORRUPTED ;
//    }

#endif // USES_BLACK_LIST

    if (PR_SUCC(error))
    {
        _ILP(key_add_mode_t) key_add_mode = ConvertKeyAddMode_P(p_dwKeyAddMode) ;
        hr = m_licPolicy->addKey(file_name, key_add_mode);
        error = H2E(hr) ;
        PR_TRACE((this, prtNOTIFY, "Licensing::AddKey: library returned 0x%X (%d)", hr,
                 m_licPolicy->getStoredError())) ;

        PR_TRACE((this, prtNOTIFY, "Licensing::AddKey: converted %terr", error)) ;
    }

    m_hSync->Release() ;

    if (PR_SUCC(error))
    {
        sysSendMsg(pmc_LICENSING, pm_DATA_CHANGED, NULL, NULL, NULL) ;
    }
    return error ;
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, AddOnlineKey )
// Parameters are:
tERROR CLicensing60::AddOnlineKey( cSerializable* p_pUserData, tDWORD p_dwKeyAddMode, cSerializable* p_pResult, tDWORD* p_pdwOLASrvErr )
{
	return errNOT_IMPLEMENTED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StopOnlineLoadKey )
// Parameters are:
tERROR CLicensing60::StopOnlineLoadKey()
{
	return errNOT_IMPLEMENTED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ClearTrialLimitDate )
// Parameters are:
tERROR CLicensing60::ClearTrialLimitDate()
{
	tERROR error = errOK ;
	PR_TRACE_FUNC_FRAME("Licensing::ClearTrialLimitDate method") ;

	if( m_licPolicy )
	{
		m_hSync->Lock(cSYNC_INFINITE);
		m_licPolicy->clearTrialLimitDate();
		m_hSync->Release();
		sysSendMsg(pmc_LICENSING, pm_DATA_CHANGED, NULL, NULL, NULL);
	}
	else
		error = errNOT_INITIALIZED;

	return error ;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetInfo )
// Parameters are:
tERROR CLicensing60::GetInfo( cSerializable* p_pInfo )
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("Licensing::GetInfo method") ;

    if (!p_pInfo)
    {
        return errPARAMETER_INVALID ;
    }

    if (p_pInfo->isBasedOn(cLicInfo::eIID))
    {
        bool blOver = false ;
        error = H2E(m_licPolicy->isTrialPeriodOver(blOver)) ;
        ((cLicInfo*)p_pInfo)->m_blTrialPeriodOver = blOver ;
    }

    return error ;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, AddOLAKey )
// Parameters are:
tERROR CLicensing60::AddOLAKey( cSerializable* p_pUserData, tDWORD p_dwKeyAddMode, hSTRING p_pResForm, cSerializable* p_pResult, tDWORD* p_pdwOLAErrCode, tDWORD* p_pdwOLAErrCodeSpecific )
{
	return _AddOLAKey(p_pUserData, p_dwKeyAddMode, p_pResForm, p_pResult, p_pdwOLAErrCode, p_pdwOLAErrCodeSpecific);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SubmitOLAForm )
// Parameters are:
tERROR CLicensing60::SubmitOLAForm( cSerializable* p_pForm, tDWORD p_dwKeyAddMode, hSTRING p_pResForm, cSerializable* p_pResult, tDWORD* p_pdwOLAErrCode, tDWORD* p_pdwOLAErrCodeSpecific )
{
	return _SubmitOLAForm(p_pForm, p_dwKeyAddMode, p_pResForm, p_pResult, p_pdwOLAErrCode, p_pdwOLAErrCodeSpecific);
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StopOLA )
// Parameters are:
tERROR CLicensing60::StopOLA()
{
	return _StopOLA();
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, GetCustomerCredentials )
// Parameters are:
tERROR CLicensing60::GetCustomerCredentials( hSTRING strCustomerId, hSTRING strCustomerPassword )
{
	return _GetCustomerCredentials(strCustomerId, strCustomerPassword);
}
// AVP Prague stamp end



cRegistry * CLicensing60::_OpenLicDataReg(bool bModify, hOBJECT regRoot)
{
    cAutoObj<cRegistry> reg; sysCreateObject((hOBJECT *)&reg, IID_REGISTRY, PID_WIN32_REG);
    if( !reg )
        return NULL;

    cStrObj root;
    if( regRoot )
        root.assign(regRoot, pgROOT_POINT);
    else
        root = "HKLM\\" VER_COMPANY_REGISTRY_PATH;
    
    root.check_last_slash();
    root += L"LicStorage";
    root.copy((cObj*)reg, pgROOT_POINT);
    
    if( bModify )
        reg->propSetBool(pgOBJECT_RO, cFALSE);
    
    if( PR_FAIL(reg->sysCreateObjectDone()) )
        return reg = NULL, NULL;

    return reg.relinquish();
}

bool CLicensing60::_CheckLicDataReg(hOBJECT regRoot)
{
    // Check Ins_InitMode
    {
		cBLSettings blset;
		PR_VERIFY_SUCC(m_hTM->GetProfileInfo(AVP_PROFILE_PROTECTION, &blset));
        if( blset.m_Ins_InitMode == 0 || (blset.m_Ins_InitMode & BL_INITMODE_USE_REG_LIC/*issue 23210*/) == 0)
            return true;
    }

    cAutoObj<cRegistry> reg = _OpenLicDataReg(false, regRoot);
    if( !reg )
        return false;

    tDWORD dwResult = 0 ;
    tTYPE_ID typeID = tid_BINARY ;
    if( PR_FAIL(reg->GetValue(&dwResult, cRegRoot, m_strListStg.c_str(cCP_ANSI), &typeID, NULL, 0)) )
        dwResult = 0;

    if( !dwResult )
        return false;

    // Fictive read
    tBYTE * btBuff = new tBYTE[dwResult];
    if( !btBuff )
        return true;

    reg->GetValue(&dwResult, cRegRoot, m_strListStg.c_str(cCP_ANSI), &typeID, btBuff, dwResult);
    delete [] btBuff;
    return true;
}

int CLicensing60::_ReadLicDataReg(char** s, unsigned int* size, hOBJECT regRoot)
{
    cAutoObj<cRegistry> reg = _OpenLicDataReg(true, regRoot);
    if( !reg )
        return 0;

    tBYTE *  btBuff   = NULL;
    tDWORD   dwResult = 0;
    tTYPE_ID typeID   = tid_BINARY;
    
    if( PR_FAIL(reg->GetValue(&dwResult, cRegRoot, m_strListStg.c_str(cCP_ANSI), &typeID, NULL, 0)) )
        return 0;

    btBuff = reinterpret_cast<tBYTE*>(malloc(dwResult));
    if( !btBuff )
        return 0;

    if( PR_FAIL(reg->GetValue(&dwResult, cRegRoot, m_strListStg.c_str(cCP_ANSI), &typeID, btBuff, dwResult)) )
    {
        free(btBuff), btBuff = NULL;
        return 0;
    }

    *s    = (char*)btBuff;
    *size = dwResult;
    return dwResult;
}

int CLicensing60::_WriteLicDataReg(const char* s, unsigned int size, hOBJECT regRoot)
{
    cAutoObj<cRegistry> reg = _OpenLicDataReg(true, regRoot);
    if( !reg )
        return 0;

    if( PR_FAIL(reg->SetValue(cRegRoot, m_strListStg.c_str(cCP_ANSI), tid_BINARY, (tPTR)s, size, cTRUE)) )
        return 0;

    return size;
}

int CLicensing60::writeLicData(const char* s, unsigned int size, void* context)
{
    if( !context )
        return 0;

    CLicensing60 * pContext = (CLicensing60 *)context ;

    pContext->m_hSync->Lock(cSYNC_INFINITE) ;

    if( !pContext->m_hStorage )
    {
        pContext->m_hSync->Release() ;
        return 0 ;
    }

    tDWORD dwResult = 0 ;
    if( pContext->m_hStorage->propGetIID() == IID_CRYPTOHELPER )
    {
        if( PR_SUCC(((cCryptoHelper*)pContext->m_hStorage)->StoreData(esiLicensingData, (tPTR)s, size)) )
            dwResult = size;

        tDWORD dwResultReg = pContext->_WriteLicDataReg(s, size);
        if( !dwResult )
            dwResult = dwResultReg;
    }
    else if( pContext->m_hStorage->propGetIID() == IID_IO )
    {
        hIO file = (hIO)pContext->m_hStorage ;

        if (PR_FAIL(file->SetSize(size)))
        {
            pContext->m_hSync->Release() ;
            return 0 ;
        }

        if (PR_FAIL(file->SeekWrite(&dwResult, 0, (tPTR)s, size)))
        {
            pContext->m_hSync->Release() ;
            return 0 ;
        }
    }
    else if( pContext->m_hStorage->propGetIID() == IID_REGISTRY )
    {
        dwResult = pContext->_WriteLicDataReg(s, size, (cObj *)pContext->m_hStorage);
    }

    pContext->m_hSync->Release() ;

    return dwResult ;
}

int CLicensing60::readLicData(char** s, unsigned int* size, void* context)
{
    if( !context )
        return 0;

    CLicensing60 * pContext = (CLicensing60 *)context;

    pContext->m_hSync->Lock(cSYNC_INFINITE) ;

    if( !pContext->m_hStorage )
    {
        pContext->m_hSync->Release();
        return 0;
    }

    tDWORD dwResult = 0 ;
    tBYTE* btBuff   = NULL ;
    if( pContext->m_hStorage->propGetIID() == IID_CRYPTOHELPER )
    {
		cCryptoHelper* pCrHlp = (cCryptoHelper*) pContext->m_hStorage;
        tDWORD dwSize = 0;
        if( PR_SUCC(pCrHlp->GetData(esiLicensingData, NULL, &dwSize))
			&& (btBuff = reinterpret_cast<tBYTE*>(malloc(static_cast<size_t>(dwSize))))
            && (PR_SUCC(pCrHlp->GetData(esiLicensingData, btBuff, &dwSize))))
			dwResult = dwSize;

        if( !dwResult )
        {
			// bug fix 17529: Раньше лицензионное хранилище хранилось в месте, зависящем
			// от 1-го и 3-го числа версии, теперь оно хранится в месте, доступном всем версиям.
			if( PR_SUCC(pCrHlp->GetOldData(esiLicensingDataOld, NULL, &dwSize))
				&& (btBuff = reinterpret_cast<tBYTE*>(malloc(static_cast<size_t>(dwSize))))
				&& PR_SUCC(pCrHlp->GetOldData(esiLicensingDataOld, btBuff, &dwSize)) )
			{
				pCrHlp->StoreData(esiLicensingData, btBuff, dwSize);
                    dwResult = dwSize;
			}

        if( !dwResult )
        {
            if( btBuff )
                free(btBuff), btBuff = NULL;
            
            dwResult = pContext->_ReadLicDataReg((char **)&btBuff, &dwResult);
        }
    }
    }
    else if( pContext->m_hStorage->propGetIID() == IID_IO )
    {
        hIO file = (hIO)pContext->m_hStorage ;

        tQWORD qwIOSize = 0 ;
        if (PR_FAIL(file->GetSize(&qwIOSize,IO_SIZE_TYPE_EXPLICIT)))
        {
            pContext->m_hSync->Release() ;
            return 0 ;
        }

        btBuff = reinterpret_cast<tBYTE*>(malloc(static_cast<size_t>(qwIOSize))) ;
        if (!btBuff)
        {
            pContext->m_hSync->Release() ;
            return 0 ;
        }

        if (PR_FAIL(file->SeekRead(&dwResult, 0, btBuff, (tDWORD)qwIOSize)))
        {
            free(btBuff) ;
            btBuff = NULL ;
        
            pContext->m_hSync->Release() ;
            return 0 ;
        }
    }
    else if( pContext->m_hStorage->propGetIID() == IID_REGISTRY )
    {
        dwResult = pContext->_ReadLicDataReg((char **)&btBuff, &dwResult, (cObj *)pContext->m_hStorage);
    }

    pContext->m_hSync->Release() ;

    *s    = (char*)btBuff ;
    *size = dwResult ;

    return dwResult ;
}

tERROR CLicensing60::ConvertKeySerialNumber_L(_LP(key_serial_number_t)& p_rStruct, cKeySerialNumber* p_pSer)
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("CLicensing60::ConvertKeySerialNumber_L method") ;

    p_pSer->m_dwMemberID  = p_rStruct.number.parts.memberId ;
    p_pSer->m_dwAppID     = p_rStruct.number.parts.appId ;
    p_pSer->m_dwKeySerNum = p_rStruct.number.parts.keySerNum ;

    return error ;
}

tERROR CLicensing60::ConvertLicensedObject_L(_LP(dword_t) p_dwKey, _LP(dword_t) p_dwValue, cLicensedObject* p_pSer)
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("CLicensing60::ConvertLicensedObject_L method") ;

    p_pSer->m_dwKey   = p_dwKey ;
    p_pSer->m_dwValue = p_dwValue ;

    return error ;
}

tERROR CLicensing60::ConvertComponentsLevel_L(_LP(dword_t) p_dwKey, _LP(dword_t) p_dwValue, cComponentsLevel* p_pSer)
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("CLicensing60::ConvertComponentsLevel_L method") ;

    p_pSer->m_dwKey   = p_dwKey ;
    p_pSer->m_dwValue = p_dwValue ;

    return error ;
}

tERROR CLicensing60::ConvertKeyInfo_L(_LP(key_info_t)& p_rStruct, cKeyInfo* p_pSer)
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("CLicensing60::ConvertKeyInfo_L method") ;

    p_pSer->m_dwKeyVer = p_rStruct.keyVer ;
    PR_TRACE((this, prtNOTIFY, "lic\tFile: %s Line: %d ConvertDT_L(p_rStruct.keyCreationDate, &p_pSer->m_dtKeyCreationDate)", __FILE__, __LINE__)) ;
    _ERR_CONVERT(ConvertDT_L(p_rStruct.keyCreationDate, &p_pSer->m_dtKeyCreationDate)) ;
    _ERR_CONVERT(ConvertKeySerialNumber_L(p_rStruct.keySerNum, &p_pSer->m_KeySerNum)) ;

    switch (p_rStruct.keyType)
    {
    case _LP(ktBeta):
        p_pSer->m_dwKeyType = ektBeta ;
        break ;
    case _LP(ktTrial):
        p_pSer->m_dwKeyType = ektTrial ;
        break ;
    case _LP(ktTest):
        p_pSer->m_dwKeyType = ektTest ;
        break ;
    case _LP(ktOEM):
        p_pSer->m_dwKeyType = ektOEM ;
        break ;
    case _LP(ktCommercial):
        p_pSer->m_dwKeyType = ektCommercial ;
        break ;
    default:
        p_pSer->m_dwKeyType = ektUnknown ;
        break ;
    }

    p_pSer->m_dwKeyLifeSpan = p_rStruct.keyLifeSpan ;
    PR_TRACE((this, prtNOTIFY, "lic\tFile: %s Line: %d ConvertDT_L(p_rStruct.keyExpDate, &p_pSer->m_dtKeyExpDate)", __FILE__, __LINE__)) ;
    _ERR_CONVERT(ConvertDT_L(p_rStruct.keyExpDate, &p_pSer->m_dtKeyExpDate)) ;
    p_pSer->m_dwLicenseCount = p_rStruct.licenseCount ;
    p_pSer->m_strProductName = p_rStruct.productName.c_str() ;
    p_pSer->m_dwAppID = p_rStruct.appId ;
    p_pSer->m_dwProductID = p_rStruct.productId ;
    p_pSer->m_strProductVer = p_rStruct.productVer.c_str() ;

    for (_LP(licensed_object_list_t::iterator) it1 = p_rStruct.licensedObjLst.begin(); it1 != p_rStruct.licensedObjLst.end(); ++it1)
    {
        p_pSer->m_listLicensedObj.push_back() ;
        _ERR_CONVERT(ConvertLicensedObject_L((*it1).first, (*it1).second, &p_pSer->m_listLicensedObj[p_pSer->m_listLicensedObj.size() - 1])) ;
    }

    p_pSer->m_strLicenseInfo = SkipRN(p_rStruct.licenseInfo).c_str() ;
    p_pSer->m_strSupportInfo = SkipRN(p_rStruct.supportInfo).c_str() ;
    p_pSer->m_dwMarketSector = p_rStruct.marketSector ;

    for (_LP(components_level_list_t::iterator) it2 = p_rStruct.compFuncLevel.begin(); it2 != p_rStruct.compFuncLevel.end(); ++it2)
    {
        p_pSer->m_listCompFuncLevel.push_back() ;
        _ERR_CONVERT(ConvertComponentsLevel_L((*it2).first, (*it2).second, &p_pSer->m_listCompFuncLevel[p_pSer->m_listCompFuncLevel.size() - 1])) ;
    }

    p_pSer->m_strCustomerInfo = SkipRN(p_rStruct.customer_info).c_str() ;

    return error ;
}

tERROR CLicensing60::ConvertDT_L(_LP(date_t)& p_rStruct, tDT* p_pDT)
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("CLicensing60::ConvertDT_L method") ;

    if (!p_pDT)
    {
        return errPARAMETER_INVALID ;
    }

    PR_TRACE((this, prtNOTIFY, "lic\tFile: %s Line: %d year:%d month:%d day:%d", __FILE__, __LINE__, p_rStruct.m_year, p_rStruct.m_month, p_rStruct.m_day)) ;

    error = DTSet(p_pDT, p_rStruct.m_year, p_rStruct.m_month, p_rStruct.m_day, 0, 0, 0, 0) ;

    return error ;
}

tERROR CLicensing60::ConvertCheckInfo_L(_ILP(check_info_t)& p_rStruct, 
                                        _LP(installed_key_info_t)* p_rInstallStruct,
                                        cCheckInfo* p_pSer)
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("CLicensing60::ConvertCheckInfo_L method") ;

    p_pSer->m_strKeyFileName = p_rStruct.keyFileName.c_str() ;
    _ERR_CONVERT(ConvertKeyInfo_L(p_rStruct.keyInfo, &p_pSer->m_KeyInfo)) ;

    switch (p_rStruct.invalidReason)
    {
    case _ILP(kirValid):
        p_pSer->m_dwInvalidReason = ekirValid ;
        break ;
    case _ILP(kirExpired):
        p_pSer->m_dwInvalidReason = ekirExpired ;
        break ;
    case _ILP(kirCorrupted):
        p_pSer->m_dwInvalidReason = ekirCorrupted ;
        break ;
    case _ILP(kirNotSigned):
        p_pSer->m_dwInvalidReason = ekirNotSigned ;
        break ;
    case _ILP(kirWrongProduct):
        p_pSer->m_dwInvalidReason = ekirWrongProduct ;
        break ;
    case _ILP(kirNotFound):
        p_pSer->m_dwInvalidReason = ekirNotFound ;
        break ;
    case _ILP(kirBlackListed):
        p_pSer->m_dwInvalidReason = ekirBlackListed ;
        break ;
    case _ILP(kirTrialAlredyUsed):
        p_pSer->m_dwInvalidReason = ekirTrialAlredyUsed ;
        break ;
    case _ILP(kirIllegalBaseUpdate):
        p_pSer->m_dwInvalidReason = ekirIllegalBaseUpdate ;
        break ;
    case _ILP(kirInvalidBases):
        p_pSer->m_dwInvalidReason = ekirInvalidBases ;
        break ;
    case _ILP(kirKeyCreationDateInvalid):
        p_pSer->m_dwInvalidReason = ekirKeyCreationDateInvalid ;
        break ;
    case _ILP(kirTrialAlreadyInUse):
        p_pSer->m_dwInvalidReason = ekirTrialAlreadyInUse ;
        break ;
    case _ILP(kirInvalidBlacklist):
        p_pSer->m_dwInvalidReason = ekirInvalidBlacklist ;
        break ;
    case _ILP(kirInvalidUpdateDescription):
        p_pSer->m_dwInvalidReason = ekirInvalidUpdateDescription ;
        break ;
    case _ILP(kirKeyInstallDateInvalid):
        p_pSer->m_dwInvalidReason = ekirKeyInstallDateInvalid ;
        break ;
    default:
        p_pSer->m_dwInvalidReason = ekirUnknown ;
        break ;
    }

    PR_TRACE((this, prtNOTIFY, "lic\tFile: %s Line: %d ConvertDT_L(p_rStruct.keyExpirationDate, &p_pSer->m_dtKeyExpirationDate)", __FILE__, __LINE__)) ;
    _ERR_CONVERT(ConvertDT_L(p_rStruct.keyExpirationDate, &p_pSer->m_dtKeyExpirationDate)) ;

    PR_TRACE((this, prtNOTIFY, "lic\tFile: %s Line: %d ConvertDT_L(p_rStruct.appLicenseExpDate, &p_pSer->m_dtAppLicenseExpDate)", __FILE__, __LINE__)) ;
    _ERR_CONVERT(ConvertDT_L(p_rStruct.appLicenseExpDate, &p_pSer->m_dtAppLicenseExpDate)) ;

    if (p_rInstallStruct)
    {
        PR_TRACE((this, prtNOTIFY, "lic\tFile: %s Line: %d ConvertDT_L(p_rInstallStruct.installDate, &p_pSer->m_dtKeyInstallDate)", __FILE__, __LINE__)) ;
        _ERR_CONVERT(ConvertDT_L(p_rInstallStruct->installDate, &p_pSer->m_dtKeyInstallDate)) ;
    }

    p_pSer->m_dwDaysTillExpiration = p_rStruct.daysTillExpiration ;

    switch (p_rStruct.funcLevel)
    {
    case _LP(flNoFeatures):
        p_pSer->m_dwFuncLevel = eflNoFeatures ;
        break ;
    case _LP(flOnlyUpdates):
        p_pSer->m_dwFuncLevel = eflOnlyUpdates ;
        break ;
    case _LP(flFunctionWithoutUpdates):
        p_pSer->m_dwFuncLevel = eflFunctionWithoutUpdates ;
        break ;
    case _LP(flFullFunctionality):
        p_pSer->m_dwFuncLevel = eflFullFunctionality ;
        break ;
    default:
        p_pSer->m_dwFuncLevel = eflUnknown ;
        break ;
    }

    p_pSer->m_dwComponentFuncBitMask = p_rStruct.componentFuncBitMask ;

    return error ;
}

tERROR CLicensing60::ConvertCheckInfoList_L(_ILP(check_info_list_t)& p_rCheckStruct, 
                                            _CLP(installed_key_info_list_t)& p_rInstallStruct, 
                                            cCheckInfoList* p_pSer)
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("CLicensing60::ConvertCheckInfoList_L method") ;

    PR_TRACE((this, prtNOTIFY, "lic\tCLicensing60::ConvertCheckInfoList_L beginning..."));

    size_t unInstallCount = p_rInstallStruct.size() ;

    for (size_t unIndex = 0, unCount = p_rCheckStruct.size(); unIndex < unCount; ++unIndex)
    {
        PR_TRACE((this, prtNOTIFY, "lic\tKey: %08X - %08X - %08X", p_rCheckStruct[unIndex].keyInfo.keySerNum.number.parts.memberId, p_rCheckStruct[unIndex].keyInfo.keySerNum.number.parts.appId, p_rCheckStruct[unIndex].keyInfo.keySerNum.number.parts.keySerNum));

        _LP(installed_key_info_t)* pInstallInfo = NULL ;
        for (size_t unJndex = 0; unJndex < unInstallCount; ++unJndex)
        {
            PR_TRACE((this, prtNOTIFY, "lic\tInstalled key: %08X - %08X - %08X", p_rInstallStruct[unJndex].serialNumber.number.parts.memberId, p_rInstallStruct[unJndex].serialNumber.number.parts.appId, p_rInstallStruct[unJndex].serialNumber.number.parts.keySerNum));
            if (p_rCheckStruct[unIndex].keyInfo.keySerNum.number.parts.memberId  == p_rInstallStruct[unJndex].serialNumber.number.parts.memberId &&
                p_rCheckStruct[unIndex].keyInfo.keySerNum.number.parts.appId     == p_rInstallStruct[unJndex].serialNumber.number.parts.appId &&
                p_rCheckStruct[unIndex].keyInfo.keySerNum.number.parts.keySerNum == p_rInstallStruct[unJndex].serialNumber.number.parts.keySerNum)
            {
                pInstallInfo = &p_rInstallStruct[unJndex] ;
                break ;
            }
        }

        p_pSer->m_listCheckInfo.push_back() ;
        _ERR_CONVERT(ConvertCheckInfo_L(p_rCheckStruct[unIndex], pInstallInfo, &p_pSer->m_listCheckInfo[p_pSer->m_listCheckInfo.size() - 1])) ;
    }

    return error ;
}

tERROR CLicensing60::ConvertKeySerialNumber_P(cSerializable* p_pSer, _LP(key_serial_number_t)& p_rStruct)
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("CLicensing60::ConvertKeySerialNumber_P method") ;

    if (!p_pSer || !p_pSer->isBasedOn(cKeySerialNumber::eIID))
    {
        return errPARAMETER_INVALID ;
    }

    cKeySerialNumber* pSer = (cKeySerialNumber*)p_pSer ;
    p_rStruct.number.parts.memberId = pSer->m_dwMemberID ;
    p_rStruct.number.parts.appId = pSer->m_dwAppID ;
    p_rStruct.number.parts.keySerNum = pSer->m_dwKeySerNum ;

    return error ;
}

_ILP(key_add_mode_t) CLicensing60::ConvertKeyAddMode_P(tDWORD p_dwValue)
{
    _ILP(key_add_mode_t) value = _ILP(kamUnknown) ;
    switch (p_dwValue)
    {
    case KAM_ADD:
        value = _ILP(kamAdd) ;
        break ;
    case KAM_REPLACE:
        value = _ILP(kamReplace) ;
        break ;
    }

    return value ;
}

tERROR CLicensing60::GetError(HRESULT p_hError)
{
    tERROR error = errOK ;
    PR_TRACE_FUNC_FRAME("CLicensing60::GetError method") ;

    switch (p_hError)
    {
    case LIC_S_KEY_ALREADY_IN_USE:
        error = warnKEY_ALREADY_IN_USE ;
        break ;
    case LIC_E_KEY_NOT_FOUND:
        error = errKEY_NOT_FOUND ;
        break ;
    case LIC_E_NOT_SIGNED:
        error = errNOT_SIGNED ;
        break ;
    case LIC_E_SECURE_DATA_CORRUPT:
        error = errSECURE_DATA_CORRUPT ;
        break ;
    case LIC_E_KEY_CORRUPT:
        error = errKEY_CORRUPT ;
        break ;
    case LIC_E_KEY_INST_EXP:
        error = errKEY_INST_EXP ;
        break ;
    case LIC_E_NO_ACTIVE_KEY:
        error = errNO_ACTIVE_KEY ;
        break ;
    case LIC_E_KEY_IS_INVALID:
        error = errKEY_IS_INVALID ;
        break ;
    case LIC_E_WRITE_SECURE_DATA:
        error = errWRITE_SECURE_DATA ;
        break ;
    case LIC_E_READ_SECURE_DATA:
        error = errREAD_SECURE_DATA ;
        break ;
    case LIC_E_IO_ERROR:
        error = errIO_ERROR ;
        break ;
    case LIC_E_CANNOT_FIND_BASES:
        error = errCANNOT_FIND_BASES ;
        break ;
    case LIC_E_CANT_REG_MORE_KEYS:
        error = errCANT_REG_MORE_KEYS ;
        break ;
    case LIC_E_NOT_INITIALIZED:
        error = errNOT_INITIALIZED ;
        break ;
    case LIC_E_INVALID_BASES:
        error = errINVALID_BASES ;
        break ;
    case LIC_E_DATA_CORRUPTED:
        error = errDATA_CORRUPTED ;
        break ;
    case LIC_E_CANT_REG_EXPIRED_RESERVE_KEY:
        error = errCANT_REG_EXPIRED_RESERVE_KEY ;
        break ;
    case LIC_E_SYS_ERROR:
        error = errSYS_ERROR ;
        break ;
    case LIC_E_BLACKLIST_CORRUPTED:
        error = errBLACKLIST_CORRUPTED ;
        break ;
    case LIC_E_SIGN_DOESNT_CONFIRM_SIGNATURE:
        error = errSIGN_DOESNT_CONFIRM_SIGNATURE ;
        break ;
    case LIC_E_CANT_REG_BETA_KEY_FOR_RELEASE:
        error = errCANT_REG_BETA_KEY_FOR_RELEASE ;
        break ;
    case LIC_E_CANT_REG_NON_BETA_KEY_FOR_BETA:
        error = errCANT_REG_NON_BETA_KEY_FOR_BETA ;
        break ;
    case LIC_E_KEY_WRONG_FOR_PRODUCT:
        error = errKEY_WRONG_FOR_PRODUCT ;
        break ;
    case LIC_E_KEY_IS_BLACKLISTED:
        error = errKEY_IS_BLACKLISTED ;
        break ;
    case LIC_E_TRIAL_ALREADY_USED:
        error = errTRIAL_ALREADY_USED ;
        break ;
    case LIC_E_KEY_IS_CORRUPTED:
        error = errKEY_IS_CORRUPTED ;
        break;
    case LIC_E_DIGITAL_SIGN_CORRUPTED:
        error = errDIGITAL_SIGN_CORRUPTED ;
        break ;
    case LIC_E_CANNOT_REG_EXPIRED_NON_COMMERCIAL_KEY:
        error = errCANNOT_REG_EXPIRED_NON_COMMERCIAL_KEY ;
        break ;
    case LIC_W_KEY_DOESNT_PROVIDE_SUPPORT:
        error = warnKEY_DOESNT_PROVIDE_SUPPORT ;
        break ;
    case LIC_E_KEY_CREATION_DATE_INVALID:
        error = errKEY_CREATION_DATE_INVALID ;
        break ;
    case LIC_E_CANNOT_INSTALL_SECOND_TRIAL:
        error = errCANNOT_INSTALL_SECOND_TRIAL ;
        break ;
    case LIC_E_BLACKLIST_CORRUPTED_OR_NOT_FOUND:
        error = errBLACKLIST_CORRUPTED_OR_NOT_FOUND ;
        break ;
    case LIC_E_UPDATE_DESC_CORRUPTED_OR_NOT_FOUND:
        error = errUPDATE_DESC_CORRUPTED_OR_NOT_FOUND ;
        break ;
    case LIC_E_LICENSE_INFO_WRONG_FOR_PRODUCT:
        error = errLICENSE_INFO_WRONG_FOR_PRODUCT ;
        break ;
    case LIC_E_CANNOT_REG_NONVALID_RESERVE_KEY:
        error = errCANNOT_REG_NONVALID_RESERVE_KEY ;
        break ;
    case LIC_E_KEY_INSTALL_DATE_INVALID:
        error = errKEY_CREATION_DATE_INVALID ;
        break ;
	case LIC_E_TRIAL_PERIOD_OVER:
		error = errTRIAL_PERIOD_OVER ;
		break ;
	case LIC_E_KEY_CONFLICTS_WITH_COMMERCIAL:
		error = errKEY_CONFLICTS_WITH_COMMERCIAL ;
		break ;
    }
    if( error == errSYS_ERROR )
    {
        // convert some win32 errors
        switch(m_licPolicy->getStoredError())
        {
        case ERROR_SHARING_VIOLATION: error = errOBJECT_IS_LOCKED; break;
        }
    }
    return error ;
}

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Licensing". Register function
tERROR CLicensing60::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Licensing_PropTable)
	mpLOCAL_PROPERTY_BUF( pgSTORAGE, CLicensing60, m_hStorage, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgBASES_PATH, CLicensing60, m_hBasesPath, cPROP_BUFFER_READ_WRITE | cPROP_BUFFER_HSTRING )
	mpLOCAL_PROPERTY_BUF( pgTM, CLicensing60, m_hTM, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( plUSES_BLACKLIST, CLicensing60, m_blUsesBlacklist, cPROP_BUFFER_READ_WRITE )
mpPROPERTY_TABLE_END(Licensing_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Licensing)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Licensing)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Licensing)
	mEXTERNAL_METHOD(Licensing, GetActiveKeyInfo)
	mEXTERNAL_METHOD(Licensing, GetInstalledKeysInfo)
	mEXTERNAL_METHOD(Licensing, RevokeActiveKey)
	mEXTERNAL_METHOD(Licensing, RevokeKey)
	mEXTERNAL_METHOD(Licensing, CheckKeyFile)
	mEXTERNAL_METHOD(Licensing, CheckActiveKey)
	mEXTERNAL_METHOD(Licensing, AddKey)
	mEXTERNAL_METHOD(Licensing, AddOnlineKey)
	mEXTERNAL_METHOD(Licensing, StopOnlineLoadKey)
	mEXTERNAL_METHOD(Licensing, ClearTrialLimitDate)
	mEXTERNAL_METHOD(Licensing, GetInfo)
	mEXTERNAL_METHOD(Licensing, AddOLAKey)
	mEXTERNAL_METHOD(Licensing, SubmitOLAForm)
	mEXTERNAL_METHOD(Licensing, StopOLA)
	mEXTERNAL_METHOD(Licensing, GetCustomerCredentials)
mEXTERNAL_TABLE_END(Licensing)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

    PR_TRACE_FUNC_FRAME_( *root, "Licensing::Register method", &error );

    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_LICENSING,                          // interface identifier
        PID_LICENSING60,                        // plugin identifier
        0x00000000,                             // subtype identifier
        Licensing_VERSION,                      // interface version
        VID_GUSCHIN,                            // interface developer
        &i_Licensing_vtbl,                      // internal(kernel called) function table
        (sizeof(i_Licensing_vtbl)/sizeof(tPTR)),// internal function table size
        &e_Licensing_vtbl,                      // external function table
        (sizeof(e_Licensing_vtbl)/sizeof(tPTR)),// external function table size
        Licensing_PropTable,                    // property table
        mPROPERTY_TABLE_SIZE(Licensing_PropTable),// property table size
        sizeof(CLicensing60)-sizeof(cObjImpl),  // memory size
        IFACE_SYSTEM                            // interface flags
    );

    #ifdef _PRAGUE_TRACE_
        if ( PR_FAIL(error) )
            PR_TRACE( (root,prtDANGER,"Licensing(IID_LICENSING) registered [%terr]",error) );
    #endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Licensing_Register( hROOT root ) { return CLicensing60::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end



