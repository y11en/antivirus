/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ss_srv.cpp
 * \author	Andrew Kazachkov
 * \date	05.05.2003 12:25:34
 * \brief	
 * 
 */

#ifdef _WIN32
# include <atlbase.h>
# include <avpregid.h>
#endif

#include <std/base/kldefs.h>
#include <std/par/paramsi.h>
#include <kca/prss/settingsstorage.h>
#include <transport/tr/transport.h>
#include <kca/ss_srv/ss_srv.h>
#include <kca/ss_srv/sssrv_server.h>

#include "common/measurer.h"
//#include "kl/ca/dbg/trace.h"

#include <kca/ss_srv/sssrv_proxy.h>
#include <kca/ss_srv/errors.h>
#include <std/stress/st_prefix.h>

#include "modulestub/ss_srvsoapstub.h"

#include <map>

#ifdef N_PLAT_NLM
	#include "std/conv/wcharcrt.h"
#endif

//using namespace std;
using namespace KLERR;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;

#define KLCS_MODULENAME L"KLSSS"

namespace KLSSS
{
    //! maps serverid to CAutoPtr<SettingsStorageServer>
    typedef std::map<
                std::wstring,
                KLSTD::KLAdapt<KLSTD::CAutoPtr<SettingsStorageServer> > >
                                                                    servers_t;
    KLSTD::LockCount            m_lckServer, m_lckProxy;
    volatile bool               g_bWasInitialized=false;
//  CAutoPtr<CriticalSection>   g_pCS;
    // Folowing vars are static as we don't want somebody to access them directly
    static servers_t            g_mapServers;    
    static KLSTD::CAutoPtr<KLSTD::CriticalSection> g_pServersCS;
    static std::wstring         g_wstrDefaultServer=c_szwDefaultServerId;
    static std::wstring         g_wstrDefaultProduct;
    static std::wstring         g_wstrDefaultVersion;

};

using namespace KLSSS;

static KLTRAP::Transport::StatusNotifyCallback g_pOldCallBack=NULL;

namespace KLSSS
{
    int ThisStatusNotifyCallback(
                            KLTRAP::Transport::ConnectionStatus status, 
			                const wchar_t*              remoteComponentName,
                            const wchar_t*              localComponentName,
			                bool                        remoteConnFlag);
};

IMPLEMENT_MODULE_INIT_DEINIT(KLSSS)

IMPLEMENT_MODULE_INIT_BEGIN2( KLCSKCA_DECL, KLSSS )
    KLSTD_CreateCriticalSection(&g_pServersCS);	
    KLERR_InitModuleDescriptions(
		KLCS_MODULENAME,
		KLSSS::c_errorDescriptions,
		KLSTD_COUNTOF(KLSSS::c_errorDescriptions));    
	KLSS_SRV::RegisterGSOAPStubFunctions();
    g_wstrDefaultServer=c_szwDefaultServerId;
    g_pOldCallBack=KLTR_GetTransport()->SetStatusCallback(ThisStatusNotifyCallback);
    g_bWasInitialized=true;
IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN2( KLCSKCA_DECL, KLSSS )    
    g_bWasInitialized=false;
    m_lckProxy.Wait();
    m_lckServer.Wait();    
    KLTR_GetTransport()->SetStatusCallback(g_pOldCallBack);    
	KLSS_SRV::UnregisterGSOAPStubFunctions();
    KLERR_DeinitModuleDescriptions(KLCS_MODULENAME);
    g_pServersCS=NULL;
IMPLEMENT_MODULE_DEINIT_END()

namespace KLSSS
{
    int ThisStatusNotifyCallback(
                                KLTRAP::Transport::ConnectionStatus status, 
			                    const wchar_t*              remoteComponentName,
                                const wchar_t*              localComponentName,
			                    bool                        remoteConnFlag)
    {
        if(
            (status == KLTRAP::Transport::ComponentDisconnected ||
             status == KLTRAP::Transport::ConnectionBroken) &&
            remoteComponentName && remoteComponentName[0] &&
            localComponentName && localComponentName[0])
        {        
            KLERR_BEGIN
                CAutoPtr<SettingsStorageServer> pServer;
                KLPRCI::ComponentId idComponent;
                idComponent.GetFromString(localComponentName);                
                if(idComponent.componentName == KLSSS::c_szwSsServerComponent)
                {
                    AutoCriticalSection acs(g_pServersCS);
                    KLSSS::servers_t::iterator itServer=
                            KLSSS::g_mapServers.find(idComponent.instanceId);
                    if(itServer != KLSSS::g_mapServers.end())
                        pServer=itServer->second.m_T;
                };
                if(pServer)
                    pServer->OnCloseConnection(remoteComponentName);
            KLERR_ENDT(3)
        };
        if(g_pOldCallBack)
            return g_pOldCallBack(
                            status, 
                            remoteComponentName,
                            localComponentName,
                            remoteConnFlag);
        else
            return 0;
    };
};

KLCSKCA_DECL void KLSSS_SetDefaultServer(
                    const std::wstring& wstrProduct,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrID)
{
    KLSTD_ASSERT(g_bWasInitialized);
    if(!g_bWasInitialized)KLSTD_NOINIT(KLCS_MODULENAME);

    AutoCriticalSection acs(g_pServersCS);
    g_wstrDefaultServer=wstrID;
    g_wstrDefaultProduct=wstrProduct;
    g_wstrDefaultVersion=wstrVersion;
};

KLCSKCA_DECL void KLSSS_GetDefaultServer(
                    std::wstring&       wstrProduct,
                    std::wstring&       wstrVersion,
                    std::wstring&       wstrID)
{
    KLSTD_ASSERT(g_bWasInitialized);
    if(!g_bWasInitialized)KLSTD_NOINIT(KLCS_MODULENAME);

    AutoCriticalSection acs(g_pServersCS);
    wstrID=g_wstrDefaultServer;
    wstrProduct=g_wstrDefaultProduct;
    wstrVersion=g_wstrDefaultVersion;
};

namespace KLSSS
{
    bool GetSsServer(const std::wstring& wstrId, SettingsStorageServer** ppServer, bool bExceptIfNotFound)
    {
        CAutoPtr<SettingsStorageServer> pServer;
        AutoCriticalSection acs(g_pServersCS);
        servers_t::iterator it=g_mapServers.find(wstrId);
        if(it == g_mapServers.end())
        {
            KLSTD_TRACE1(3, L"Cannot find ss server '%ls'\n", wstrId.c_str());
            if(bExceptIfNotFound)
            {
                KLSSS_THROW_SERVER_NOT_FOUND(wstrId);
            }
            else
                return false;
        };
        pServer=it->second;
        KLSTD_ASSERT(pServer != NULL);
        pServer.CopyTo(ppServer);
        return true;
    };

    void PutSsServer(const std::wstring& wstrId, SettingsStorageServer* pServer)
    {
        CAutoPtr<SettingsStorageServer> pTmpServer;// there's a buggy global lock in VC's 'map' realization
        {
            AutoCriticalSection acs(g_pServersCS);
            servers_t::iterator it=g_mapServers.find(wstrId);
            if(pServer)
            {
                if(it != g_mapServers.end())
                    KLSSS_THROW_SERVER_EXISTS(wstrId);
                g_mapServers.insert(servers_t::value_type(wstrId, CAutoPtr<SettingsStorageServer>(pServer)));
            }
            else
            {            
                if(it == g_mapServers.end())
                    KLSSS_THROW_SERVER_NOT_FOUND(wstrId);
                pTmpServer=it->second;
                g_mapServers.erase(it);
            };
        };
        pTmpServer=NULL;
    };
};

KLCSKCA_DECL void KLSSS_AddSsServer(
                    const std::wstring&             wstrProduct,
                    const std::wstring&             wstrVersion,
                    const std::wstring&             wstrID,
                    bool                            bLocalOnly)
{
    KLSTD_CHK(wstrProduct,  !wstrProduct.empty());
    KLSTD_CHK(wstrVersion,  !wstrVersion.empty());
    KLSTD_CHK(wstrID,       !wstrID.empty());

    KL_TMEASURE_BEGIN(L"KLSSS_AddSsServer", 2);
    KLSTD_TRACE3(2, L"KLSSS_AddSsServer('%ls', '%ls', '%ls')...\n", wstrProduct.c_str(), wstrVersion.c_str(), wstrID.c_str());    
    CAutoPtr<KLSSS::SettingsStorageServer> pServer;
    KLSSS_CreateSettingsStorageServer(wstrProduct, wstrVersion, wstrID, &pServer, bLocalOnly);
    KLSSS::PutSsServer(wstrID, pServer);
    KLSTD_TRACE3(2, L"... OK KLSSS_AddSsServer('%ls', '%ls', '%ls')\n", wstrProduct.c_str(), wstrVersion.c_str(), wstrID.c_str());
    KL_TMEASURE_END();
};

KLCSKCA_DECL bool KLSSS_RemoveSsServer(const std::wstring& wstrID)
{
    KLSTD_CHK(wstrID,       !wstrID.empty());
    bool bResult=false;
    KLERR_BEGIN
        KLSTD_TRACE1(2, L"KLSSS_RemoveSsServer('%ls')...\n", wstrID.c_str());    
        KLSSS::PutSsServer(wstrID, NULL);
        bResult=true;        
    KLERR_ENDT(3)
    if(bResult)
    {
        KLSTD_TRACE1(2, L"...OK KLSSS_RemoveSsServer('%ls')\n", wstrID.c_str());
    }
    else
    {
        KLSTD_TRACE1(2, L"...FAILED KLSSS_RemoveSsServer('%ls')\n", wstrID.c_str());
    }
    return bResult;
};

#ifdef _WIN32
    const TCHAR c_szPorts[]=_T("Ports");
#else
	const wchar_t c_szPorts[]=L"Ports";
#endif


/*void InitPortsConfigFileName(char * name){
	initPortsConfigFileName[0] = 0;
	wcscat(initPortsConfigFileName,g_programPathW);
	wcscat(initPortsConfigFileName,c_szPortsConfigFileName);
wprintf(L"ss_srv %s InitPortsConfigFileName %ls\n",name,initPortsConfigFileName);

}*/

int KLSSS_ReadPortValue(
            const std::wstring& wstrProduct,
            const std::wstring& wstrVersion,
            const std::wstring& wstrID)
{
    int nResult=0;
    KLSTD_TRACE3(
            3,
            L"Accessing ss server '%ls'-'%ls'-'%ls...'\n",
            wstrProduct.c_str(),
            wstrVersion.c_str(),
            wstrID.c_str());
    
#ifdef _WIN32
        KLSTD_USES_CONVERSION;
        CRegKey key;
        std::basic_string<TCHAR> strKey=KLSTD_StGetProductKey(wstrProduct.c_str(), wstrVersion.c_str());
        strKey+=_SEP_;
        strKey+=c_szPorts;
        
        long lResult=key.Open(HKEY_LOCAL_MACHINE, strKey.c_str(), KEY_READ);
        if(lResult)KLSTD_THROW_LASTERROR_CODE(lResult);
        DWORD dwPortNumber=0;
        lResult=key.QueryValue(dwPortNumber, KLSTD_W2CT(wstrID.c_str()));
        if(lResult)KLSTD_THROW_LASTERROR_CODE(lResult);
        nResult=(int)dwPortNumber;
#else
        /*  andkaz: we have only one process under unix and novell yet, so 
            there's no need to store ports numbers at all

            And commented code below is neither thread-safe nor fault-tolerant
        */

        /*
		KLSTD::CAutoPtr<KLPAR::Params> parSerFile;
#ifdef N_PLAT_NLM
		KLPAR_DeserializeFromFileName( initNovellPath(c_szPortsConfigFileName), &parSerFile );		
#else
		KLPAR_DeserializeFromFileName( c_szPortsConfigFileName, &parSerFile );		
#endif
		std::wstring strKey=KLSTD_StGetProductKey(wstrProduct.c_str(), wstrVersion.c_str());
		strKey+=std::wstring(L"_SEP_");
        strKey+=c_szPorts;

		KLSTD::CAutoPtr<KLPAR::IntValue> pVal = 
			(KLPAR::IntValue*)parSerFile->GetValue2( strKey, true);
		if (pVal) 
		{
			KLPAR_CHKTYPE(pVal, INT_T, strKey.c_str());
			nResult = (int) pVal->GetValue();
		}
        */		
#endif
    KLSTD_TRACE4(
            3,
            L"...accessing ss server '%ls'-'%ls'-'%ls'-'%u' OK\n",
            wstrProduct.c_str(),
            wstrVersion.c_str(),
            wstrID.c_str(),
            nResult);
    return nResult;
};

void KLSSS_WritePortValue(
            const std::wstring& wstrProduct,
            const std::wstring& wstrVersion,
            const std::wstring& wstrID,
            int                 nPortValue)
{
    KLSTD_TRACE4(
            3,
            L"Publishing ss server port '%ls'-'%ls'-'%ls-%u'...\n",
            wstrProduct.c_str(),
            wstrVersion.c_str(),
            wstrID.c_str(),
            nPortValue);
#ifdef _WIN32
        KLSTD_USES_CONVERSION;
        CRegKey keyBase;
        const std::basic_string<TCHAR>& strKey=KLSTD_StGetProductKey(wstrProduct.c_str(), wstrVersion.c_str());
        long lResult=keyBase.Create(
                            HKEY_LOCAL_MACHINE,
                            strKey.c_str(),
                            REG_NONE,
                            REG_OPTION_NON_VOLATILE,
                            KEY_WRITE);
        if(lResult)KLSTD_THROW_LASTERROR_CODE(lResult);
        CRegKey key;
        lResult=key.Create(
                            keyBase,
                            c_szPorts,
                            REG_NONE,
                            REG_OPTION_VOLATILE,
                            KEY_WRITE);
        if(lResult)KLSTD_THROW_LASTERROR_CODE(lResult);
        keyBase.Close();
        if(!nPortValue)
            lResult=key.DeleteValue(KLSTD_W2CT(wstrID.c_str()));
        else
            lResult=key.SetValue(nPortValue, KLSTD_W2CT(wstrID.c_str()));
        if(lResult)KLSTD_THROW_LASTERROR_CODE(lResult);
#else
        /*  andkaz: we have only one process under unix and novell yet, so 
            there's no need to store ports numbers at all

            And commented code below is neither thread-safe nor fault-tolerant
        */

        /*
		KLSTD::CAutoPtr<KLPAR::Params> parSerFile;

#ifdef N_PLAT_NLM
		KLERR_IGNORE(KLPAR_DeserializeFromFileName(initNovellPath(c_szPortsConfigFileName), &parSerFile ));
#else
		KLERR_IGNORE(KLPAR_DeserializeFromFileName(c_szPortsConfigFileName, &parSerFile ));
#endif
		if ( parSerFile==NULL ) KLPAR_CreateParams( &parSerFile );
		
		std::wstring strKey=KLSTD_StGetProductKey(wstrProduct.c_str(), wstrVersion.c_str());
		strKey+=std::wstring(L"_SEP_");
        strKey+=c_szPorts;

		KLSTD::CAutoPtr<KLPAR::IntValue> pVal;
		KLPAR::CreateValue(nPortValue, &pVal);
		(parSerFile)->ReplaceValue(strKey, pVal);
#ifdef N_PLAT_NLM
		KLPAR_SerializeToFileName(initNovellPath(c_szPortsConfigFileName), parSerFile );
#else
		KLPAR_SerializeToFileName(c_szPortsConfigFileName, parSerFile );
#endif
        */
#endif
    KLSTD_TRACE4(
            3,
            L"...publishing ss server port '%ls'-'%ls'-'%ls-%u' OK\n",
            wstrProduct.c_str(),
            wstrVersion.c_str(),
            wstrID.c_str(),
            nPortValue);
};
