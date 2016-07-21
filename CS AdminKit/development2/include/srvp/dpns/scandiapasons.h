/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	scandiapasons.h
 * \author	Andrew Kazachkov
 * \date	06.09.2005 11:49:05
 * \brief	
 * 
 */

#ifndef __SCANDIAPASONS_H__
#define __SCANDIAPASONS_H__

#include <std/base/klbase.h>
#include <srvp/csp/klcsp.h>

namespace KLDPNS
{
    /*!
        Network scan diapason contains following attibutes: unique id, 
        unique display name, group where found hosts will be put, 
        ip-addres validity period and array of ip-address intervals or 
        subnets.
    */
    //! Diapason id
    KLSTD_DECLARE_HANDLE(diapason_id);
    
    //! Invalid diapason id value
    const diapason_id c_idDpsnInvalid = diapason_id(0);

    //! Diapason id 
    const wchar_t c_szwDpsn_Id[]= L"KLDPNS_ID";//INT_T

    //! Diapason display name 
    const wchar_t c_szwDpsn_DN[]= L"KLDPNS_DN";//INT_T

    //! Diapason ip-addres validity period, in seconds 
    const wchar_t c_szwDpsn_LifeTime[]= L"KLDPNS_LF";//INT_T

    //! Diapason group 
    const wchar_t c_szwDpsn_Group[]= L"KLDPNS_GRP";//INT_T
    
    //! Move if nagent installed only
    const wchar_t c_szwDpsn_GroupNagent[]= L"KLDPNS_GRP_NAGENTS";//BOOL_T
    
    //! Diapason enabled for scan
    const wchar_t c_szwDpsn_ScanEnabled[]= L"KLDPNS_ScanEnabled";//BOOL_T

    //! Diapason intervals array. Intervals of all diapason cannot intersect.
    const wchar_t c_szwDpsn_Intervals[]= L"KLDPNS_ILS";//ARRAY_T|PARAMS_T

    /*! 
        Diapason intervals subnet mask or low interval end in network format.
        Subnet mask must be contigious.
    */
    const wchar_t c_szwDpsnIl_MaskOrLo[]= L"KLDPNS_IL_MASKORLOW";// INT_T

    /*!
        Diapason intervals subnet address or hi interval end in network format.
        Subnet address must correspond subnet mask.
    */
    const wchar_t c_szwDpsnIl_SubnetOrHi[]= L"KLDPNS_IL_SUBNETORHI";   // INT_T

    /*!
        Subnet flag. If true then MaskOrLo and SubnetOrHi are subnet mask 
        and subnet address otherwise thay are hi an low ends of interval         
    */
    const wchar_t c_szwDpsnIl_IsSubnet[]= L"KLDPNS_IL_ISSUBNET";   //BOOL_T

    typedef enum
    {
        KLDPNS_ERR_IL_INVALID_SUBNET_MASK = 1,//!subnet mask is not contigious
        KLDPNS_ERR_IL_INVALID_SUBNET_ADDRESS,//!subnet address does not correspond to the subnet mask
        KLDPNS_ERR_IL_INTERSECTION, //! interval intersects with one of existing intervals
        KLDPNS_ERR_IL_TOO_LARGE //! interval is too large
    };

    /*!
        For diagnostics purposes. May be returned in ppInvalidIntervals by
        method AddDiapason.
    */
    const wchar_t c_szwDpsnIl_Invalid[]= L"KLDPNS_IL_Invalid";   //INT_T

    //! Network scan diapasons 
    class KLSTD_NOVTABLE ScanDiapasons
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Adds new diapason with specified parameters. If at least
                    one of diapasons intersects with any of existing 
                    diapasons or is invalid then c_idDpsnInvalid is returned
                    and such intetval is added to c_szwDpsn_Intervals 
                    array in ppInvalidIntervals.          

          \param	pData   IN conatainer with diapason attributes.
          \param	ppInvalidIntervals  OUT
          \return	id of a new diapason
          \exception   *
        */
        virtual diapason_id AddDiapason(
                            KLPAR::Params*  pData,
                            KLPAR::Params** ppInvalidIntervals) = 0;

        /*!
          \brief	Updates one or more attributes of diapason. If at least
                    one of diapasons intersects with any of existing 
                    diapasons or is invalid then false is returned and such
                    intetval is  added to c_szwDpsn_Intervals array in 
                    ppInvalidIntervals.

          \param	id  IN  id of diapason
          \param	pData   IN conatainer with diapason attributes.
          \param	ppInvalidIntervals  OUT 
          \return	true if if the diapason was successfully updated
          \exception   *
        */
        virtual bool UpdateDiapason(
                            diapason_id     id,
                            KLPAR::Params*  pData,
                            KLPAR::Params** ppInvalidIntervals) = 0;


        /*!
          \brief	Removes diapason

          \param	id  IN  id of diapason
          \exception   *
        */
        virtual void RemoveDiapason(
                            diapason_id     id) = 0;


        /*!
          \brief	Returns diapason attributes

          \param	id  IN  id of diapason
          \param	pFields IN Array of propery names.
          \param	nFields IN length of array pFields
          \param	ppResults OUT results
          \return	virtual void 
        */
        virtual void GetDiapason(
                            diapason_id     id,
                            const wchar_t** pFields,
                            long            nFields,
                            KLPAR::Params** ppResults) = 0;

        /*!
          \brief	Returns iterator to diapasons.

          \param	pFields IN Array of propery names.
          \param	nFields IN length of array pFields
          \param	lTimeout IN iterator's lifetime (in secs) on server
          \param	ppIterator OUT iterator
          \exception   *
        */
        virtual void GetDiapasons(
                            const wchar_t**             pFields,
                            long                        nFields,
                            long                        lTimeout,
                            KLCSP::ForwardIterator**    ppIterator) = 0;
        

        /*!
          \brief	GetHosts

          \param	id  IN  id of diapason
          \param	pFields IN  Array of propery names.
          \param	nFields IN  length of array pFields
          \param	vecFieldsToOrder IN  names of columns to sort by
          \param	lMaxLifeTime IN  max lifetime of accessor (sec)
          \param	ppAccessor OUT 
          \return	number of records found
        */
        virtual long GetHosts(
                    diapason_id                         id,
                    const wchar_t**                     pFields,
                    long                                nFields,
                    const KLCSP::vec_field_order_t&     vecFieldsToOrder,
                    long                                lMaxLifeTime,
                    KLCSP::ChunkAccessor**              ppAccessor) = 0;

    };
    
    class KLSTD_NOVTABLE ScanDiapasons2
        :   public ScanDiapasons
    {
    public:
        virtual void RestartDpnsScan() = 0;
    };
	
};

namespace KLSRV
{
	long CheckInterval(
					unsigned long&      ulMaskOrLo,
					unsigned long&      ulSubnetOrHi,
					bool&               bIsSubnet );
};

#endif //__SCANDIAPASONS_H__
