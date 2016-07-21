/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	Groups2.h
 * \author	Andrew Kazachkov
 * \date	15.10.2004 16:35:38
 * \brief	
 * 
 */

#ifndef KLGRP_GROUPS2_H
#define KLGRP_GROUPS2_H

#include <time.h>
#include <string>
#include <vector>

#include "./groups.h"

namespace KLGRP
{
    /*! 'Pseudovalue' that expresses current date/time in search filter
        may be used as <attr_name> <op> CURTIME([<delta>])
        Example:
        (&
	        (!KLHST_WKS_PRODUCT_DISPLAY_VERSION = "5*")
	        (|
		        (KLHST_WKS_GROUPID = 0 )
		        (KLHST_WKS_GROUPID = 1 )
	        )
	        (KLHST_WKS_STATUS_MASK_0 <> 0 )
	        (KLHST_WKS_LAST_INFOUDATE > CURTIME(-86400) )
        )
    */
    const wchar_t c_szwSrchNow[] = L"CURTIME";

	/*!
	* \brief Интерфейс для получения и редактирования информации о логических
	*        группах в Сервере Администрирования.
	*
	*   Группа - это набор компьютеров, объединенных администратором для
	*   удобства управления. 
	*
	*   Группы могут быть вложены в другие группы.
	*/

    class Groups2 : public Groups
	{
    public:
        /*!
          \brief	Searches for hosts meeting specified criteria.
                    In filter expression may be also used c_szwSrchNow.

          \param	wstrFilter - search filter
          \param	vecFieldsToReturn - names of attributes to return
          \param	vecFieldsToOrder - names of columns to sort by
          \param	lMaxLifeTime - max lifetime of accessor (sec)
          \param	ppAccessor - pointer to 
          \return	number of records found
        */
        virtual long FindHosts(
                const std::wstring&                 wstrFilter,
                const wchar_t**                     ppFieldsToReturn,
                size_t                              nFieldsToReturn,
                const KLCSP::vec_field_order_t&     vecFieldsToOrder,
                long                                lMaxLifeTime,
                KLCSP::ChunkAccessor**              ppAccessor) = 0;
    };

};

#endif // KLGRP_GROUPS2_H
