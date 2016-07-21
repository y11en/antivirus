/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	adhst.h
 * \author	Andrew Kazachkov
 * \date	14.10.2004 14:24:50
 * \brief	
 * 
 */

#ifndef __KLADHST_H__
#define __KLADHST_H__

#include <vector>
#include <string>

#include <std/base/klbase.h>
#include <srvp/csp/klcsp.h>

namespace KLADHST
{
	//! Предопределенные идентификаторы специальных organizational unit'ов
	typedef enum{
		OU_ROOT_ID = 0,	//< Корень иерархии
		OU_INVALID_ID = -1 //< Неверный идентификатор organizational unit'а
	}KLGRP_GROUPID;

	//! Идентификатор
	const wchar_t c_adhst_id[]=L"adhst_id";		        //INT_T

	//! Идентификатор родителя
	const wchar_t c_adhst_idParent[]=L"adhst_idParent"; //INT_T
	
	//! Имя в AD
	const wchar_t c_adhst_Name[]=L"adhst_idComputer";   //INT_T

    //!Full name
    const wchar_t c_adhst_FullName[]=L"adhst_idFullName";   //STRING_T
    
    //! enable ad scan 
    const wchar_t c_adhst_enable_ad_scan[] = L"adhst_enable_ad_scan"; // BOOL_T

    typedef KLCSP::ForwardIterator ForwardIterator;

    class KLSTD_NOVTABLE AdHosts : public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Retrieves iterator for computers contained in given
                    organizational unit.

          \param	idOU       [in]  id of organizational unit
          \param	pFields    [in]  Array of propery names. Following 
                        properties may be specified here.
                    KLADHST::c_adhst_id
                    KLADHST::c_adhst_idParent
                    KLADHST::c_adhst_Name
                    KLHST::c_wks_name
                    KLHST::c_wks_id
                    KLHST::c_wks_groupId
                    KLHST::c_wks_lastVisible
                    KLHST::c_wks_lastInfoUpdate
                    KLHST::c_wks_status
                    KLHST::c_wks_lastUpdate
                    KLHST::c_wks_lastNagentConnected
                    KLHST::c_wks_display_name
                    KLHST::c_wks_win_name
                    KLHST::c_wks_win_domain
                    KLHST::c_wks_win_domain_type
                    KLHST::c_wks_dnsDomain
                    KLHST::c_wks_ctype
                    KLHST::c_wks_ptype
                    KLHST::c_wks_versionMajor
                    KLHST::c_wks_versionMinor
                    KLHST::c_wks_ip

                    Attention! Propeties KLHST::c_wks_* may absent.

          \param	nFields    [in]  length of array pFields
          \param	lTimeout   [in]  iterator's lifetime (in secs) on server
          \param	ppIterator [out] iterator
        */
        virtual void GetChildComputers(
                    long                idOU, 
                    const wchar_t**     pFields,
                    long                nFields,
                    long                lTimeout,
                    ForwardIterator**   ppIterator) = 0;


        /*!
          \brief	Retrieves iterator for child organizational units
                    contained in given organizational unit.

          \param	idOU       [in]  id of organizational unit
          \param	pFields    [in]  Array of property names. Following 
                        properties may be specified here.
                    KLADHST::c_adhst_id
                    KLADHST::c_adhst_idParent
                    KLADHST::c_adhst_Name

          \param	nFields    [in]  length of array pFields
          \param	lTimeout   [in]  iterator's lifetime (in secs) on server
          \param	ppIterator [out] iterator
        */
        virtual void GetChildOUs(
                    long                idOU, 
                    const wchar_t**     pFields,
                    long                nFields,
                    long                lTimeout,
                    ForwardIterator**   ppIterator)  = 0;
        
        /*!
          \brief	Retrieves organizational unit attributes

          \param	idOU       [in]  id of organizational unit
          \param	pFields    [in]  Array of propery names. Following 
                        properties may be specified here.
                    KLADHST::c_adhst_id
                    KLADHST::c_adhst_Name
                    KLADHST::c_adhst_autoAddHost
          \param	nFields    [in]  length of array pFields
          \param	ppResult   [out] required data
        */
        virtual void GetOU(
                    long            idOU, 
                    const wchar_t** pFields,
                    size_t          nFields,
                    KLPAR::Params** ppResult) = 0;
    };
    

    class KLSTD_NOVTABLE AdHosts2 : public AdHosts
    {
    public:
        /*!
          \brief	Updates OU proterties

          \param	idOU    IN  OU id
          \param	pData   IN  may contain following values
                                c_adhst_enable_ad_scan
        */
        virtual void UpdateOU(
                    long            idOU, 
                    KLPAR::Params*  pData) = 0;
    };





	//! Not supported any more
    const wchar_t c_adhst_autoAddHost[] = L"adhst_autoAddHost"; //INT_T
    
	//! Not supported any more
    const wchar_t c_adhst_autoAddNagentOnly[] = L"adhst_autoAddHostOnlyNagents"; //BOOL_T

};

#endif //__KLADHST_H__