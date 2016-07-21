/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prci_inprocess.h
 * \author	Andrew Kazachkov
 * \date	06.01.2004 12:53:12
 * \brief	
 * 
 */

#ifndef __KLPRCI_INPROCESS_H__
#define __KLPRCI_INPROCESS_H__

#include "std/tp/threadspool.h"

namespace KLPRCI
{
	class KLSTD_NOVTABLE InProcessComponents  : public KLTP::ThreadsPool::Worker
	{
	public:
		virtual void UnloadAll() = 0;

		virtual void Shutdown() = 0;

		virtual void LoadComponent(
					ComponentId&	id,
					KLPAR::Params*		pParameters, 
					std::wstring&		asyncId) = 0;

		KLSTD_NOTHROW virtual void OnComponentClosed(
									const std::wstring& instanceId) throw() = 0;
		virtual void Find(ComponentId&	filter, std::list<ComponentId>& ids) = 0;

        virtual bool UnloadComponent(std::wstring& wstrInstanceId) = 0;
        
        virtual void SetShutdownFlag() = 0;
	};

    KLSTD::CAutoPtr<InProcessComponents> Create_InProcess();
};
#endif //__KLPRCI_INPROCESS_H__
