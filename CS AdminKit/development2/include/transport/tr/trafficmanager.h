/*!
* (C) 2007 "Kaspersky Lab"
*
* \file trafficmanager.h
* \author Шияфетдинов Дамир
* \date 2007
* \brief Интерфейс класса подсистемы ограничения трафика 
*
*/
#ifndef TR_TRAFFICMANAGER_H
#define TR_TRAFFICMANAGER_H

#include <map>

#include "std/base/klbase.h"
#include "std/thr/sync.h"

#include "transport/tr/transport.h"
#include "transport/tr/common.h"

namespace KLTR {

	class TrafficManager : public KLSTD::KLBase
	{
	public:

		struct Restriction
		{
			struct IPInterval
			{	
				IPInterval()
				{
					nMaskOrLo = nSubnetOrHi = 0;
					bIsSubnet = false;
				}
				unsigned long nMaskOrLo;
				unsigned long nSubnetOrHi;
				bool bIsSubnet;
			};

			struct	TimedLimit
			{
				TimedLimit()
				{
					iStartHour = iStartMin = 0;
					iEndHour = iEndMin = 0;
					iLimit = 0;
				}
				int iStartHour, iStartMin;
				int iEndHour, iEndMin;	//! Времмя начала и конца действия литима трафика в часах и минутах ( 0 - 23, 0 - 59 )

				int iLimit;					//! Лимит трафика в kb/sec
			};

			Restriction()
			{
				nId = 0;
				bUseTotalLimit = false;
				iTotalLimit = 0;
			}

			long	nId;				//! Идентификатор ограничения

			bool	bUseTotalLimit;		//! Флаг использования общего ограничения трафиика 
										// ( на временный участки который не опеределены в остальных ограничениях )
			int		iTotalLimit;		//! Общий лимит трафика в kb/sec


			TimedLimit	timedLimit;		//! Лимит с указанием времени действия

			IPInterval	interval;		//! IP интервал
		};


		typedef std::map<long,Restriction> RestrictionsMap;
		typedef std::list<Restriction> RestrictionsList;

		/*!
		  \brief Добавляет ограничение траффика для указанного интервала IP адресов
		*/
		virtual void ReplaceRestriction( const Restriction &restriction ) = 0;

		/*!
		  \brief Удаляет указанного ограничение
		*/
		virtual void DeleteRestriction( long nRestrictionId ) = 0;
	};

} // namespace KLTR

#endif // TR_TRAFFICMANAGER_H