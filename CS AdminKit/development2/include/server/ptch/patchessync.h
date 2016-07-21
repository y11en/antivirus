/*!
 * (C) 2007 Kaspersky Lab
 * 
 * \file	patchessync.h
 * \author	Eugene Rogozhnikov
 * \date	28.04.2007 16:41:12
 * \brief	Интерфейс для синхронизации информации о патчах для AdminKit
 *			с базой данных
 * 
 */

#ifndef __PATCHESSYNC_H__
#define __PATCHESSYNC_H__

#include <std/base/klbase.h>
#include <server/db/dbconnection.h>

namespace KLPTCH
{
	class KLSTD_NOVTABLE PatchesSyncSrv
        :   public KLSTD::KLBaseQI
    {
    public:
		virtual ~PatchesSyncSrv(){}

		/*
		 *	
			Обновить информацию в базе данных по поводу скаченных патчей для AdminKit.
			bServerAutoUpdates - если true, то AK-сервер будет автоматически обновлен, если будет подходящий патч
			bNagentAutoUpdates - если true, то будет запущена задача обновления нагентов, при условии если
									будут новые патчи для нагента ( то есть их не было в базе на момент
									вызова этого метода), либо bForceRunNagentUpdate == true
		 */
        virtual void Sync( 
			KLDB::DbConnectionPtr pDbConn = NULL,
			bool bServerAutoUpdates = false,
			bool bNagentAutoUpdates = false,
			bool bForceRunNagentUpdate = false ) = 0;
	};

}; // namespace KLPTCH

#endif //__PATCHESSYNC_H__