/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ts_server2.h
 * \author	Andrew Kazachkov
 * \date	21.03.2003 13:18:07
 * \brief	
 * 
 */

#ifndef __KLTS_SERVER2_H__
#define __KLTS_SERVER2_H__

#include <list>
#include <map>
#include <set>

#include <kca/prts/taskstorageserver.h>
#include <kca/prts2/taskstorage2.h>

#define KLPRTS2_COMPONENT_NAME L"KLPRTS2_COMPONENT_NAME"

namespace KLPRTS2
{
	template <class T>
		class CSet: public KLSTD::KLBaseImpl<KLSTD::KLBase>
	{
	public:
		std::set<T>	x;
	};

	typedef CSet<std::wstring> CStringSet;

	struct ts_server_data
	{
		KLSTD::CAutoPtr<KLPRTS::TaskStorageServer>		m_pServer;
		std::wstring									m_wstrObjectID;
		KLSTD::CAutoPtr<CStringSet>						m_pSet;
	};		

	class CTasksStorageServer2 : public KLSTD::KLBaseImpl<KLPRTS::TaskStorageServer>
	{
	public:
		CTasksStorageServer2();
		virtual ~CTasksStorageServer2();
        KLSTD_CLASS_DBG_INFO(KLPRTS2::CTasksStorageServer2);

		void Initialize(
							const KLPRCI::ComponentId&	cidMaster,
							const std::wstring&			wstrId,
							const ts_info_t*			pStoragesInfo,
							long						nStoragesInfo,
							bool						bLocalCopyOfServerStorage,
							KLPRTS2::TsSink*			pSink,
                            bool                        bConnectToAgent,
                            bool                        bDontPublishEvents,
                            const std::wstring&         wstrCommonTssId,
							const std::wstring&         wstrHostId	);

		KLSTD_NOTHROW void Deinitialize() throw();

        void AddTasksStorage(
                            const std::wstring&         wstrId,
                            const KLPRCI::ComponentId&  cidMaster,
                            const ts_info_t&            info,
                            bool						bLocalCopyOfServerStorage);

        bool RemoveTasksStorage(const KLPRSS::product_version_t& info);
        
        void Update(
					const ts_info_t*    pStoragesInfo,
					long                nStoragesInfo);

		void ResetTasksIterator(
								const KLPRCI::ComponentId&	cidFilter,
								const std::wstring&			sTaskNameFilter,
								KLPAR::Params**				ppTaskStorageParams);

		void GetTaskByID(		const std::wstring			TaskID,
								KLPAR::Params**				ppTaskParams);
        
        void GetTaskByIDWithPolicyApplied(  const std::wstring taskId,
											KLPAR::Params** ppTaskParams);

		std::wstring AddTask(	const KLPAR::Params* pTaskToAddParams);

		void UpdateTask(		const std::wstring TaskID,
								const KLPAR::Params* pTaskToUpdateParams);

        void ReplaceTask(       const std::wstring TaskID, 
                                const KLPAR::Params* pTaskToUpdateParams);

		void DeleteTask(		const std::wstring TaskId);

		void SetTaskStartEvent(	const std::wstring			TaskId,
								const KLPRCI::ComponentId&	filter,
								const std::wstring&			eventType,
								const KLPAR::Params*		bodyFilter);

		void GetTaskStartEvent(	const std::wstring			TaskId,
								KLPRCI::ComponentId&		filter,
								std::wstring&				eventType,
								KLPAR::Params**				bodyFilter);
		
		std::wstring& getID();
        
	    void PutTask(
							const std::wstring TaskID,
							const KLPAR::Params* pTaskParams);

		std::wstring GetHostId() const;
		
	protected:
		KLSTD_NOTHROW void UpdateCache(
						const KLPRSS::product_version_t&	pvi,
						const std::wstring&					TaskId,
						bool								bAdd=true) throw();
		bool FindServer(
						const std::wstring&				TaskId,
						KLPRTS::TaskStorageServer**		ppServer,
						bool							bFastSearchOnly=false);

		void ThrowTaskNotFound(const std::wstring& wstrTaskId);
        void ThrowTsNotFound(const KLPRSS::product_version_t& pv);
	protected:
        KLSTD::CAutoPtr<KLPRTS::TaskStorageServer>  m_pCommonTss;
        std::wstring                            m_wstrCommonTssId;
		typedef std::map<KLPRSS::product_version_t, ts_server_data> servers_t;
		typedef std::list<KLSTD::KLAdapt<KLSTD::CAutoPtr<KLPAR::Params> > > parlist_t;		
		servers_t								m_mapServers;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_pCS;
		bool									m_bInitialized;
		std::wstring							m_wstrID;
		KLSTD::CAutoPtr<KLPRTS2::TsSink>		m_pSink;
        KLPRCI::ComponentId                     m_cidMaster;
        bool                                    m_bLocalCopyOfServerStorage;
        bool                                    m_bConnectToAgent;
        bool                                    m_bDontPublishEvents;
		std::wstring							m_wstrHostId;
	};
   

    class DiffMaker
    {
    public:
        typedef std::map<KLPRSS::product_version_t, std::wstring> data_t;

        DiffMaker(
                CTasksStorageServer2*       pServer,
                const KLPRCI::ComponentId&  cidMaster,
                bool						bLocalCopyOfServerStorage)
            :   m_pServer(pServer)
            ,   m_cidMaster(cidMaster)
            ,   m_bLocalCopyOfServerStorage(bLocalCopyOfServerStorage)
        {
            ;
        }
        void NewInSrc(data_t::iterator itSrc);
        void NewInDst(data_t::iterator itDst);
        inline void Diff(
                data_t::iterator itSrc,
                data_t::iterator itDst)
        {
            ;// do nothing
        };
    protected:
        CTasksStorageServer2*       m_pServer;
        const KLPRCI::ComponentId&  m_cidMaster;
        const bool                  m_bLocalCopyOfServerStorage;        
    };

};


#endif //__KLTS_SERVER2_H__
