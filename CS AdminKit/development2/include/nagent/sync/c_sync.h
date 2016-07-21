/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	c_sync.h
 * \author	Andrew Kazachkov
 * \date	12.05.2003 9:21:15
 * \brief	Common synchronization API
 * 
 */

#ifndef __KL_CSYNC_H__
#define __KL_CSYNC_H__

#include <std/base/klbase.h>
#include <std/trc/trace.h>
#include <std/par/paramsi.h>
#include <kca/prss/store.h>
#include <transport/tr/transport.h>
#include <transport/tr/transportproxy.h>
#include <transport/ev/eventsource.h>
#include <kca/prci/componentinstance.h>
#include <kca/prci/simple_params.h>
#include <kca/prcp/agentproxy.h>
#include <kca/prcp/internal.h>
#include <std/tmstg/timeoutstore.h>

#include <kca/prss/ss_sync.h>

#define SSSYNC_GROUPSYN L"SSSYNC_GROUPSYN"

namespace CSYNC
{
	/*!
	*
	*	\brief	Интерфасе обработчика синхронизации на клиентской стороне (т.е
                на стороне сетевого агента)
	*
	*/
	class KLSTD_NOVTABLE SyncHandler : public KLSTD::KLBase
	{
	public:
        //! Любой вызов 
		virtual void OnSyncCall(
						const wchar_t*	szwIDTo,
						const wchar_t*	szwIDFrom,
						KLPAR::Params*	pInData,
						KLPAR::Params**	ppOutData)=0;

        //! вызывается, если при синхронизации произошла ошибка
		KLSTD_NOTHROW virtual void OnSyncFailure(KLERR::Error* pError)=0;
	};


	/*!
	*
	*	\brief	Интерфейс инициатора синхронизации
	*
	*/
	
	class KLSTD_NOVTABLE SyncInitiator : public KLSTD::KLBase
	{
	public:
		virtual void DoSyncCall(KLPAR::Params* pData, KLPAR::Params** ppData)=0;
		KLSTD_NOTHROW virtual void DoFailure(KLERR::Error* pError) throw() = 0;
		virtual bool IsValid()=0;
		virtual const wchar_t*	GetRemote()=0;
		virtual const wchar_t*	GetLocal()=0;
		virtual const wchar_t*	GetIdFrom()=0;
		virtual const wchar_t*	GetIdTo()=0;

		virtual KLTRAP::TransportProxy* GetProxy()=0;
	};

	class ISyncDataQueue : public KLSTD::KLBase
	{
	public:
		virtual void AddData(KLPAR::Params* pParams)=0;
		KLSTD_NOTHROW virtual void Fail()=0;
	};


	/*!
	*
	*	\brief	Интерфейс инициатора синхронизации
	*
	*/

	class KLSTD_NOVTABLE SyncStarter : public KLSTD::KLBase
	{
	public:
		virtual void sync_start(
								wchar_t*		szwType,
								wchar_t*		szwIDFrom,
								KLPAR::Params*	pOptions,
								std::wstring&	wstrIDTo,
								KLPAR::Params** ppParams) = 0;
		virtual void sync_putData(
								wchar_t*		szwID,
								KLPAR::Params*	pData) = 0;

		virtual void sync_Call(
								wchar_t* szwIDTo,
								wchar_t* szwIDFrom,
								KLPAR::Params*	pInput,
								KLPAR::Params** ppOutput) = 0;

        virtual void AddSyncDataQueue(
                                const std::wstring& wstrID,
                                ISyncDataQueue* pQueue) = 0;

		virtual void RemoveSyncDataQueue(const std::wstring& wstrID) = 0;

		virtual void GetSyncDataQueue(const std::wstring& wstrID, ISyncDataQueue** ppQueue) = 0;

		virtual void SyncAddHandler(const std::wstring& wstrID, SyncHandler* pHandler) = 0;

		virtual void GetSyncHandler(const std::wstring& wstrID, SyncHandler** ppHandler) = 0;
	};

};

void CSYNC_SyncAddHandler(
                        const std::wstring& wstrID,
                        CSYNC::SyncHandler* pHandler);

void CSYNC_CreateSyncInitiator(
						const wchar_t*		    szwIDTo,
						const wchar_t*		    szwIDFrom,
						const wchar_t*		    localComponentName,
						const wchar_t*		    remoteComponentName,	
						CSYNC::SyncInitiator**	ppInitiator);

void CSYNC_SetStarter(CSYNC::SyncStarter* pStarter);

void CSYNC_GetStarter(CSYNC::SyncStarter** ppStarter);

void CSYNC_CreateSynchronizer(
							const std::wstring&		wstrID,
							const std::wstring&		wstrRemoteID,
							const wchar_t*			localComponentName,
							const wchar_t*			connName,
							long					lTimeout,
							KLPRSS::Synchronizer**	ppSync);

#endif //__KL_CSYNC_H__

// Local Variables:
// mode: C++
// End:

