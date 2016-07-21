/*!
 * (C) 2007 "Kaspersky Lab"
 *
 * \file trafficmanageproxy.h
 * \author Дамир Шияфетдинов
 * \date 2007
 * \brief Прокси интерфейса контроля трафика
 */

#if !defined(KLTRFP_TRAFFICMANAGERPROXY)
#define KLTRFP_TRAFFICMANAGERPROXY

#include <std/base/klbase.h>
//#include <server/trf/trafficmanagerserver.h>
#include <transport/tr/trafficmanager.h>

namespace KLTRFP
{
	class KLSTD_NOVTABLE TrafficManagerProxy : public KLSTD::KLBaseQI 
	{
    public:

		/*!
		  \brief Добавляет ограничение траффика для указанного интервала IP адресов
		*/
		virtual long AddRestriction( const KLTR::TrafficManager::Restriction &restriction ) = 0;

		/*!
		  \brief Модифицирует ограничение траффика для указанного интервала IP адресов
		*/
		virtual long UpdateRestriction( long nRestrictionId, const KLTR::TrafficManager::Restriction &restriction ) = 0;

		/*!
		  \brief Удаляет указанного ограничение
		*/
		virtual void DeleteRestriction( long nRestrictionId ) = 0;

		/*!
		  \brief Возвращает все ограничения траффика 
		*/
		virtual void GetRestrictions( KLTR::TrafficManager::RestrictionsMap &restrictList ) = 0;
	};
}

#endif // !defined(KLTRFP_TRAFFICMANAGERPROXY)