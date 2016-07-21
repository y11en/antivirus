/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	gprts_server.h
 * \author	Andrew Kazachkov
 * \date	20.10.2003 11:42:56
 * \brief	
 * 
 */

#ifndef __KL_GPRTS_SERVER_H__
#define __KL_GPRTS_SERVER_H__

#include <kca/prts/taskstorageserver.h>
#include <server/tsk/grouptaskcontrolsrv.h>

namespace KLSRV
{
    class CGTaskStorageServer : public KLPRTS::TaskStorageServer
    {
    public:
        CGTaskStorageServer(){;};
        
        KLSTD_CLASS_DBG_INFO(KLSRV::CGTaskStorageServer);

        static void CreateCGTaskStorageServer(
                KLSRV::SrvData*                pSrvData,
                KLTSK::GroupTaskControlSrv*    pGtc,
                const std::wstring&            wstrId,
                const std::wstring&            wstrHost);

        static void DeleteCGTaskStorageServer(
                const std::wstring&                         wstrId);

        void Initialize(
                KLSRV::SrvData*                pSrvData,
                KLTSK::GroupTaskControlSrv*    pGtc,
                const std::wstring&            wstrId,
                const std::wstring&            wstrHost);

        //KLPRTS::TaskStorageServer
	    void ResetTasksIterator(
							    const KLPRCI::ComponentId &cidFilter,
							    const std::wstring &sTaskNameFilter,
							    KLPAR::Params** ppTaskStorageParams);

		void GetTaskByIDWithPolicyApplied (const std::wstring TaskID,
											   KLPAR::Params** ppTaskParams);
	
	    void GetTaskByID(
							    const std::wstring TaskID,
							    KLPAR::Params** ppTaskParams);

	    std::wstring AddTask(
							    const KLPAR::Params* pTaskToAddParams);

	    void UpdateTask(
							    const std::wstring TaskID,
							    const KLPAR::Params* pTaskToUpdateParams);

	    void DeleteTask(
							    const std::wstring TaskId);

	    void SetTaskStartEvent(
							    const std::wstring TaskId,
							    const KLPRCI::ComponentId& filter,
							    const std::wstring& eventType,
							    const KLPAR::Params* bodyFilter);

	    void GetTaskStartEvent(
							    const std::wstring TaskId,
							    KLPRCI::ComponentId& filter,
							    std::wstring& eventType,
							    KLPAR::Params** bodyFilter);

	    std::wstring& getID();
        
	    void PutTask(
						const std::wstring TaskID,
						const KLPAR::Params* pTaskParams);
		
		std::wstring GetHostId() const;

protected:
	    bool    GetTaskByID_I(
                                KLDB::DbConnectionPtr       pCon, 
							    long                        nId,
  							    const KLPRCI::ComponentId&  cidFilter,
							    const std::wstring&         sTaskNameFilter,
                                std::wstring&               wstrTsId,
							    KLPAR::Params**             ppTaskParams);

        int IdToNumber(KLDB::DbConnectionPtr pCon, const wchar_t* szwId);

        std::wstring NumberToId(KLDB::DbConnectionPtr pCon, int nId);
protected:
        KLSTD::CAutoPtr< KLTSK::GroupTaskControlSrv >   m_pGtc;
        const std::wstring  m_wstrId;
        const std::wstring  m_wstrHost;
        KLSRV::SrvData*     m_pSrvData;
    };
};

#endif //__KL_GPRTS_SERVER_H__