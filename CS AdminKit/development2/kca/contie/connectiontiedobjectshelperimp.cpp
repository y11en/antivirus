#include "./connectiontiedobjectshelperimp.h"
#include <kca/prci/componentinstance.h>
#include <kca/prcp/componentproxy.h>

#include <std/tmstg/timeoutstore.h>

#include <set>

#define KLCS_MODULENAME L"CONTIE"

using namespace KLSTD;

namespace CONTIE
{
    static KLTRAP::Transport::StatusNotifyCallback g_pOldCallBack=NULL;

    static 	std::wstring MakeFullConnectionName(
                            const std::wstring& wstrLocalConnectionName,
                            const std::wstring& wstrRemoteConnectionName)
    {
	    return wstrRemoteConnectionName + L"/" + wstrLocalConnectionName;
    };

    typedef KLSTD::CAutoPtr<ConnectionTiedObjectsHelperImp> ConTiedObjHelperPtr;
    typedef std::set< KLSTD::KLAdapt<ConTiedObjHelperPtr> > set_helpers_t;

    static set_helpers_t g_setHelpers;
    static KLSTD::LockCount g_lckModule;

    int ConnectionTiedObjectsHelperImp::ThisStatusNotifyCallback(
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
            std::vector<KLSTD::KLAdapt<ConTiedObjHelperPtr> > vecObjects;
            {
                KLCS_GLOBAL_MODULE_LOCK(acs);
                vecObjects.reserve(g_setHelpers.size());
                for(    set_helpers_t::iterator it = g_setHelpers.begin();
                        it != g_setHelpers.end();
                        ++it)
                {
                    vecObjects.push_back(*it);
                };
            };
            if(!vecObjects.empty())
            {
                const std::wstring& wstrKey = MakeFullConnectionName(
                                        localComponentName,
                                        remoteComponentName);

                for(size_t i = 0; i < vecObjects.size(); ++i)
                {
                    KLERR_IGNORE(vecObjects[i].m_T->OnDisconnect(wstrKey));
                };
            };
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

    ConnectionTiedObjectsHelperImp::ConnectionTiedObjectsHelperImp()
        :   m_pDataCS(KLSTD::CreateCS())
        ,   m_lckExtAdd(m_pDataCS)
    {
        g_lckModule.Lock();
    }

    ConnectionTiedObjectsHelperImp::~ConnectionTiedObjectsHelperImp()
    {
        g_lckModule.Unlock();
    }

	void ConnectionTiedObjectsHelperImp::Create()
    {        
        {
            KLCS_GLOBAL_MODULE_LOCK(acs);
            g_setHelpers.insert(ConTiedObjHelperPtr(this));
        };
        m_lckExtAdd.Allow();            
    }

    KLSTD_NOTHROW void ConnectionTiedObjectsHelperImp::Close() throw()
    {
        {
            KLCS_GLOBAL_MODULE_LOCK(acs);
            g_setHelpers.erase(ConTiedObjHelperPtr(this));
        };
        m_lckExtAdd.Disallow();
        m_lckExtAdd.Wait();
        for(;;)
        {
            std::wstring wstrID;
            {
                KLSTD::AutoCriticalSection acs(m_pDataCS);
                id2entry_t::iterator it = m_mapId2Entry.begin();
                if(it == m_mapId2Entry.end())
                    break;
                wstrID = it->first;
            };
            Entry_Remove(wstrID, NULL);
        };
    };

	void ConnectionTiedObjectsHelperImp::AddObject(
                const std::wstring& wstrObjID,
                DisconnectSink*     pSink,
			    const std::wstring& wstrLocalConnectionName,
			    const std::wstring& wstrRemoteConnectionName)
    {
        Entry_Add(  wstrObjID, 
                    MakeFullConnectionName(
                        wstrLocalConnectionName, 
                        wstrRemoteConnectionName),
                    pSink);
    };

	KLSTD_NOTHROW void ConnectionTiedObjectsHelperImp::RemoveObject(
                const std::wstring& wstrObjID) throw()
    {
    KLERR_BEGIN
        Entry_Remove(wstrObjID, NULL);
    KLERR_ENDT(1)
    }

    void ConnectionTiedObjectsHelperImp::Entry_Add(
                const std::wstring&     wstrId,
                const std::wstring&     wstrConnection,
                DisconnectSink*         pSink)
    {
        KLSTD::CAutoObjectLock aol(m_lckExtAdd);
        if(!aol)
            KLSTD_THROW(KLSTD::STDE_UNAVAIL);
        ;
        KLSTD_CHK(wstrId, !wstrId.empty());
        KLSTD_CHKINPTR(pSink);
        ;
        KLSTD::AutoCriticalSection acs(m_pDataCS);
        id2entry_t::iterator it = m_mapId2Entry.find(wstrId);
        if(it != m_mapId2Entry.end())
            KLSTD_THROW(KLSTD::STDE_EXIST);

        m_mapId2Entry.insert(id2entry_t::value_type(
                                            wstrId,
                                            EntryData(pSink)));
        if(!wstrConnection.empty())
            m_mapConn2Id.insert(conn2id_t::value_type(wstrConnection, wstrId));

        KLSTD_TRACE3(
                3,
                L"Added object '%ls' (now %u stores and %u connections)\n",
                wstrId.c_str(),
                m_mapId2Entry.size(),
                m_mapConn2Id.size());
    }

    bool ConnectionTiedObjectsHelperImp::IfConnectionExist(
        const std::wstring&     wstrConnection)
    {
        KLSTD::CAutoObjectLock aol(m_lckExtAdd);
        if(!aol)
            KLSTD_THROW(KLSTD::STDE_UNAVAIL);
        KLSTD::AutoCriticalSection acs(m_pDataCS);
        conn2id_t::iterator itconn=m_mapConn2Id.find(wstrConnection);
        return itconn != m_mapConn2Id.end();
    };

    void ConnectionTiedObjectsHelperImp::Entry_Find(
                const std::wstring&         wstrId,
                DisconnectSink**            ppSink)
    {
        KLSTD::CAutoObjectLock aol(m_lckExtAdd);
        if(!aol)
            KLSTD_THROW(KLSTD::STDE_UNAVAIL);
        ;
        KLSTD_CHK(wstrId, !wstrId.empty());
        KLSTD_CHKOUTPTR(ppSink);
        ;
        KLSTD::AutoCriticalSection acs(m_pDataCS);
        id2entry_t::iterator it = m_mapId2Entry.find(wstrId);
        if(it == m_mapId2Entry.end())
            KLSTD_THROW(KLSTD::STDE_NOTFOUND);
        it->second.m_T.CopyTo(ppSink);
    }

    void ConnectionTiedObjectsHelperImp::Entry_RemoveForConection(std::wstring wstrConnection)
    {
        m_lckExtAdd.Unlock();
        KLSTD::CAutoObjectLock aol(m_lckExtAdd);
        if(!aol)
            return;

        KLSTD_CHK(wstrConnection, !wstrConnection.empty());
        ;
        for(;;)
        {
            DisconnectSinkPtr pTmp;
            std::wstring wstrID;
            {
                KLSTD::AutoCriticalSection acs(m_pDataCS);
                conn2id_t::iterator itconn=m_mapConn2Id.find(wstrConnection);
                if(itconn == m_mapConn2Id.end())
                    break;
                id2entry_t::iterator it=m_mapId2Entry.find(itconn->second);
                m_mapConn2Id.erase(itconn);
                if(it == m_mapId2Entry.end())
                    continue;                
                pTmp = it->second;
                wstrID = it->first;
                m_mapId2Entry.erase(it);
                KLSTD_TRACE3(
                        3,
                        L"Removed object '%ls' (now %u stores and %u connections)\n",
                        wstrID.c_str(),
                        m_mapId2Entry.size(),
                        m_mapConn2Id.size());
            };
            if(pTmp)
            {
                KLERR_IGNORE(pTmp->OnDisconnectObjectID(wstrID));
            };
            pTmp=NULL;
        };
    };

    bool ConnectionTiedObjectsHelperImp::Entry_Remove(
                const std::wstring& wstrId,
                DisconnectSink**    ppSink)
    {
        DisconnectSinkPtr pTmp;
        std::wstring wstrID;
        {
            KLSTD::AutoCriticalSection acs(m_pDataCS);
            id2entry_t::iterator it = m_mapId2Entry.find(wstrId);
            if(it == m_mapId2Entry.end())
                return false;
            pTmp = it->second;
            wstrID = it->first;
            m_mapId2Entry.erase(it);
                KLSTD_TRACE3(
                        3,
                        L"Removed object '%ls' (now %u stores and %u connections)\n",
                        wstrId.c_str(),
                        m_mapId2Entry.size(),
                        m_mapConn2Id.size());
        };
        if(pTmp)
        {
            KLERR_IGNORE(pTmp->OnDisconnectObjectID(wstrId));
        };
        if(ppSink)
            pTmp.CopyTo(ppSink);
        pTmp = NULL;        
        return true;
    };

    void ConnectionTiedObjectsHelperImp::OnDisconnect(
                                const std::wstring&     wstrConnection)
    {
        KLSTD::CAutoObjectLock aol(m_lckExtAdd);
        if(!aol)
            return;
        if(IfConnectionExist(wstrConnection) && m_lckExtAdd.Lock())
        {
            try{
                KLTMSG::AsyncCall1T<ConnectionTiedObjectsHelperImp, std::wstring>
                    ::Start(
                        this, 
                        &ConnectionTiedObjectsHelperImp::Entry_RemoveForConection,
                        wstrConnection);
            }
            catch(...)
            {
                m_lckExtAdd.Unlock();
                throw;
            };
        };
    };
};

using namespace CONTIE;

IMPLEMENT_MODULE_INIT_DEINIT(CONTIE)

IMPLEMENT_MODULE_INIT_BEGIN2(KLCSKCA_DECL, CONTIE)
    CONTIE::g_pOldCallBack = KLTR_GetTransport()->SetStatusCallback(
            CONTIE::ConnectionTiedObjectsHelperImp::ThisStatusNotifyCallback);
IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN2(KLCSKCA_DECL, CONTIE)
    KLTR_GetTransport()->SetStatusCallback(CONTIE::g_pOldCallBack);
    CONTIE::g_lckModule.Wait();
IMPLEMENT_MODULE_DEINIT_END()



KLCSKCA_DECL void CONTIE_CreateConnectionTiedObjectsHelper(
            CONTIE::ConnectionTiedObjectsHelper** ppHelper)
{
    KLSTD_CHKOUTPTR(ppHelper);
    ;
    KLSTD::CAutoPtr<CONTIE::ConnectionTiedObjectsHelper> pObject;
    CONTIE::ConnectionTiedObjectsHelperImp* p = new ConnectionTiedObjectsHelperImp;
    KLSTD_CHKMEM(p);
    pObject.Attach(p);
    p->Create();
    pObject.CopyTo(ppHelper);
};
