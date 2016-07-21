#ifndef __KL_SRVHRCH_FIND_H__
#define __KL_SRVHRCH_FIND_H__

#include <srvp/srvhrch/srvhrch.h>
#include <srvp/csp/klcsp.h>

namespace KLSRVH
{
    //! slave server belongs to the specified group or it's subgroup
    const wchar_t c_srv_group_id_grandparent[] = L"KLSRVH_SRV_GROUPID_GP";//INT_T


    class KLSTD_NOVTABLE SrvHrchFind
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Searches for slave servers meeting specified criteria.
                    In filter expression may be also used c_szwSrchNow.

          \param	wstrFilter - search filter
                        following attributes may be used in filter string
                            c_srv_group_id_grandparent
                            c_srv_id
                            c_srv_display_name
                            c_srv_group_id
                            c_srv_status
                            c_wks_Created
                            
          \param	vecFieldsToReturn - names of attributes to return
                        following attributes may be used in vecFieldsToReturn
                            KLGRP::c_grp_name
                            c_srv_id
                            c_srv_display_name
                            c_srv_group_id
                            c_srv_status
                            KLHST::c_wks_Created

          \param	vecFieldsToOrder - names of columns to sort by,
                        same attributes as for vecFieldsToReturn may be used
          \param	lMaxLifeTime - max lifetime of accessor (sec)
          \param	ppAccessor - pointer to 
          \return	number of records found
        */

        virtual long FindSlaveServers(
                const std::wstring&                 wstrFilter,
                const wchar_t**                     ppFieldsToReturn,
                size_t                              nFieldsToReturn,
                const KLCSP::vec_field_order_t&     vecFieldsToOrder,
                long                                lMaxLifeTime,
                KLCSP::ChunkAccessor**              ppAccessor) = 0;

    };
};

#endif //__KL_SRVHRCH_FIND_H__
