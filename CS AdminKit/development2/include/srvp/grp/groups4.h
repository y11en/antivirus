/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	groups4.h
 * \author	Andrew Lashchenkov
 * \date	13.05.2005 11:53:49
 * \brief	
 * 
 */

#ifndef KLGRP_GROUPS4_H
#define KLGRP_GROUPS4_H

#include "./groups3.h"
#include <srvp/csp/klcsp.h>

const wchar_t KLGRP_FAILED_SLAVES_PARAMS[] = L"KLGRP_FAILED_SLAVES_PARAMS";

namespace KLGRP
{
    /*!
        \brief Событие c_EventHstSrchState публикуется при завершении асинхронного поиска хостов, в т.ч. при ошибке, а так же для отображения процента выполнения
		    c_evpHstSrchId, STRING_T - Идентификатор запроса, глобально-уникальный ключ
			c_evpHstSrchResult, INT_T - Результат (c_nHstSrchResultOk - успешное завершение)
			c_evpHstSrchPercentComplete, INT_T - Процент выполнения (может отсутствовать)
    */
	const wchar_t c_EventHstSrchState[]=L"KLGRP_EventHstSrchState";

		const wchar_t c_evpHstSrchId[]=L"KLGRP_HstSrchId";			// STRING_T
		const wchar_t c_evpHstSrchResult[]=L"KLGRP_HstSrchResult";	// INT_T
			const long c_nHstSrchResultOk = 0;
			const long c_nHstSrchResultInProgress = 1;
			const long c_nHstSrchResultFail = -1;
		const wchar_t c_evpHstSrchPercentComplete[]=L"KLGRP_HstSrchPercentComplete"; // INT_T

	/*!
	* \brief See Groups.
	*/

    class Groups4 : public Groups3
	{
    public:
        /*!
          \brief	Searches for hosts meeting specified criteria.

          \param	wstrFilter - search filter
          \param	vecFieldsToReturn - names of attributes to return
          \param	vecFieldsToOrder - names of columns to sort by
          \param	lMaxLifeTime - max lifetime of accessor (sec)
          \return	number of records found
        */
        virtual void FindHostsEx(
                const std::wstring&                 wstrFilter,
                const wchar_t**                     ppFieldsToReturn,
                size_t                              nFieldsToReturn,
                const KLCSP::vec_field_order_t&     vecFieldsToOrder,
				KLPAR::Params*						pParams,
                long                                lMaxLifeTime,
                const std::wstring&					wstrRequestId,
				KLADMSRV::AdmServerAdviseSink*		pSink,
				KLADMSRV::HSINKID&					hSink) = 0;

        virtual void FindHostsExCancel(
            const std::wstring&		wstrRequestId) = 0;

        virtual long FindHostsExGetAccessor(
            const std::wstring&		wstrRequestId,
			KLCSP::ChunkAccessor**	ppAccessor,
			KLPAR::Params**			ppFailedSlavesInfo) = 0;
        

        /*!
          \brief	Returns group info as GetGroupInfo does but 
                    GetGroupInfo2 allows to limit result with required 
                    fields. For servers previous to 4 MP it behaves like
                    GetGroupInfo.

          \param	id      IN group id
          \param	fields  IN field names
          \param	size    IN field names amount
          \param	groups  OUT pointer to results
        */
        virtual void GetGroupInfo2 ( 
                        long id, 
                        const wchar_t** fields,
                        size_t          size,
                        KLPAR::Params ** groups)  = 0;
    };
    
    class Groups5 : public Groups4
	{
    public:
    /*!
      \brief	Deletes group with 
                    Supported IDs for AdmServerAdviseSink::OnNotify
                        c_nAdviseSinkFailure
                        c_nAdviseSinkOK
                        c_nAdviseSinkCompletion

                Requires Administration Server 6.0


      \param	id          IN Id of  group to delete. 
      \param	dwReserved  IN Reserved for future use. Must be 0. 
      \param	pSink       IN pointer to sink 
      \param	hSink       OUT sink id. Use AdmServer::Unadvise to unsubscribe

      \except       STDE_NOTEMPTY group contains objects that cannot be deleted

    */
        virtual void DelGroupWithSubgroups( 
                        long                            id, 
                        AVP_dword                       dwReserved,
                        KLADMSRV::AdmServerAdviseSink*  pSink,
                        KLADMSRV::HSINKID&              hSink) = 0;
    };
    

    //! groups belong to the specified group or it's subgroup
    const wchar_t c_grp_groupId_grandparent[] = L"KLGRP_GRP_GROUPID_GP";//INT_T

    
    class Groups6
        :   public Groups5
	{
    public:

        /*!
          \brief	Searches for groups meeting specified criteria.
                    In filter expression may be also used c_szwSrchNow.

          \param	wstrFilter - search filter
                        following attributes may be used in filter string
                            c_grp_groupId_grandparent
                            c_grp_id
                            c_grp_name
                            c_grp_parentId
                            c_grp_creationDate
                            c_grp_lastUpdate
                            
          \param	vecFieldsToReturn - names of attributes to return
                        following attributes may be used in vecFieldsToReturn
                            c_grp_full_name
                            c_grp_id
                            c_grp_name
                            c_grp_parentId
                            c_grp_creationDate
                            c_grp_lastUpdate

          \param	vecFieldsToOrder - names of columns to sort by,
                        same attribnutes as for vecFieldsToReturn may be used
          \param	lMaxLifeTime - max lifetime of accessor (sec)
          \param	ppAccessor - pointer to 
          \return	number of records found
        */
        virtual long FindGroups(
                const std::wstring&                 wstrFilter,
                const wchar_t**                     ppFieldsToReturn,
                size_t                              nFieldsToReturn,
                const KLCSP::vec_field_order_t&     vecFieldsToOrder,
                long                                lMaxLifeTime,
                KLCSP::ChunkAccessor**              ppAccessor) = 0;
    };

};

#endif // KLGRP_GROUPS4_H
