#ifndef __KLCONNECTIONTIEDOBJECTSHELPERIMP_H__
#define __KLCONNECTIONTIEDOBJECTSHELPERIMP_H__

#include <kca/contie/connectiontiedobjectshelper.h>
#include <std/base/klbaseqi_imp.h>
#include <std/thr/locks.h>
#include <std/par/params.h>
#include <transport/tr/transport.h>

#include <map>

namespace CONTIE
{
    /*
        connection --> objid_1 --> EntryData_1
                       objid_2 --> EntryData_2
                       objid_3 --> EntryData_3
    */

    typedef KLSTD::CAutoPtr<DisconnectSink> DisconnectSinkPtr;

    typedef KLSTD::KLAdapt<DisconnectSinkPtr> EntryData;
   
    //! maps objid to EntryData
    typedef std::map<std::wstring, EntryData > id2entry_t;

    //! maps connection name to objid
    typedef std::multimap<std::wstring, std::wstring> conn2id_t;

    class ConnectionTiedObjectsHelperImp
        :   public KLSTD::KLBaseImpl<ConnectionTiedObjectsHelper>
	{
    public:
        ConnectionTiedObjectsHelperImp();        
        virtual ~ConnectionTiedObjectsHelperImp();
        KLSTD_SINGLE_INTERAFCE_MAP(ConnectionTiedObjectsHelper);
    //ConnectionTiedObjectsHelper
		void Create();
        KLSTD_NOTHROW void Close() throw();

		void AddObject(
                    const std::wstring& wstrObjID,
                    DisconnectSink*     pSink,
			        const std::wstring& wstrLocalConnectionName,
			        const std::wstring& wstrRemoteConnectionName);

		KLSTD_NOTHROW void RemoveObject(
                    const std::wstring& wstrObjID) throw();
        bool IfConnectionExist(const std::wstring&     wstrConnection);
    protected:
        void Entry_Add(
                    const std::wstring&     wstrId,
                    const std::wstring&     wstrConnection,
                    DisconnectSink*         pSink);

        void Entry_Find(
                    const std::wstring&         wstrId,
                    DisconnectSink**            ppSink);

        /*!
          \brief	Removes all entries for connection and UNLOCKS instance
          \param	wstrConnection - connection name
        */
        void Entry_RemoveForConection(std::wstring wstrConnection);

        /*!
          \brief	Removes entry by id
          \param	wstrId - id
          \param	ppSink - optional
          \return	false if not found
        */
        bool Entry_Remove(
                    const std::wstring& wstrId,
                    DisconnectSink**    ppSink);

        void OnDisconnect(const std::wstring&     wstrConnection);
    public:
        static int ThisStatusNotifyCallback(
                                KLTRAP::Transport::ConnectionStatus status, 
			                    const wchar_t*              remoteComponentName,
                                const wchar_t*              localComponentName,
			                    bool                        remoteConnFlag);
    protected:
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pDataCS;//must be first
        KLSTD::ObjectLock       m_lckExtAdd;
        id2entry_t              m_mapId2Entry;
        conn2id_t               m_mapConn2Id;
	};
};

#endif //__KLCONNECTIONTIEDOBJECTSHELPERIMP_H__
