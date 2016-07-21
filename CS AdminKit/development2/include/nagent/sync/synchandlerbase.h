/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	synchandlerbase.h
 * \author	Andrew Kazachkov
 * \date	06.12.2002 17:38:09
 * \brief	
 * 
 */

#ifndef __KL_SYNCHANDLERBASE_H__
#define __KL_SYNCHANDLERBASE_H__

#include "./ssync.h"
#include "./c_sync.h"

namespace CSYNC
{	
    extern volatile long g_lTestSyncsAttempts;
    extern volatile long g_lTestSyncsAttemptsSucc;

	/*!
	*
	*	\brief	SyncHandlerBase Вспомогательный базовый класс
	*
	*/


	class KLSTD_NOVTABLE CSyncHandlerBase
		:	public SyncHandler
//		,	public KLTMSG::TimeoutObject
	{
	public:
		CSyncHandlerBase();
		virtual ~CSyncHandlerBase();
		virtual void Initialize(
					const std::wstring&	wstrID,
					const std::wstring& wstrStorage,
					long				lLockTimeout,
					bool				bReadOnly,
					bool				bPublishChangeNotifications,
                    const std::wstring&	wstrSsType);	
		virtual void OnCmdGetHeaders(
						KLPAR::Params*	pInData,
						KLPAR::Params**	ppOutData);
		virtual void OnCmdWriteSections(
						KLPAR::Params*	pInData,
						KLPAR::Params**	ppOutData);
		virtual void OnCmdDeleteSections(
						KLPAR::Params*	pInData,
						KLPAR::Params**	ppOutData);
		virtual void OnCmdEOF(
						KLPAR::Params*	pInData,
						KLPAR::Params**	ppOutData);
		virtual void OnPublishEventsChanged();
		virtual void OnCmdFinish(
						KLPAR::Params*	pInData,
						KLPAR::Params**	ppOutData);
		virtual void OnSyncCall(
						KLPAR::Params*	pInData,
						KLPAR::Params**	ppOutData);
		virtual void OnSyncCall(
						const wchar_t*	szwIDTo,
						const wchar_t*	szwIDFrom,
						KLPAR::Params*	pInData,
						KLPAR::Params**	ppOutData);
		virtual KLSTD_NOTHROW void OnSyncFailure(KLERR::Error* pError) throw();
	protected:
		std::wstring							m_wstrLocalID, m_wstrSsType;
		bool									m_bFailure;
		KLSTD::CAutoPtr<KLPRSS::Storage>		m_pStorage;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_CS;
		const KLPRCI::ComponentId				m_idAgent;
		std::list<sec_address_t>				m_lstChangedSections;
		bool									m_bPublishChangeNotifications;
	};

    /*
	class KLSTD_NOVTABLE SyncHandlerBase
		:	public SyncHandler
	{
	public:
		virtual ~SyncHandlerBase()
		{
			SyncAddHandler(m_wstrLocalID, NULL);
		};

	protected:
		virtual void Initialize(const std::wstring& wstrLocalID)
		{
			m_wstrLocalID=wstrLocalID;
			SyncAddHandler(m_wstrLocalID, this);
		};

	protected:
		std::wstring m_wstrLocalID;
	};
    */


	void CreateSyncInitiator(
						const wchar_t*		szwIDTo,
						const wchar_t*		szwIDFrom,
						const wchar_t*		localComponentName,
						const wchar_t*		remoteComponentName,	
						SyncInitiator**		ppInitiator);
	
};
#endif //__KL_SYNCHANDLERBASE_H__
