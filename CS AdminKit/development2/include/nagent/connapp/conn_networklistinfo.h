/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	conn_networklistinfo.h
 * \author	Andrew Kazachkov
 * \date	21.04.2006 11:23:20
 * \brief	
 * 
 */

#ifndef __KL_CONN_NETWORKLISTINFO_H__
#define __KL_CONN_NETWORKLISTINFO_H__

#include <nagent/conn/networklist.h>

namespace KLCONNAPP
{
    class KLSTD_NOVTABLE NetworkListInfo
        :   public KLCONN::NetworkList2
    {
    public:
        /*!
          \brief	Checks if interface NetworkList is supported by 
                    connector
          \return	true if supported
          \exception    KLSTD::STDE_UNAVAIL is thrown if connector 
                    is being initialized/deinitialized
        */
        virtual bool IfNlSupported() = 0;

        /*!
          \brief	Checks if interface NetworkList can be used just now
          \return	true if can
          \exception    KLSTD::STDE_UNAVAIL is thrown if connector 
                    is being initialized/deinitialized
        */
        virtual bool CanUseNlNow() = 0;

        /*!
          \brief	Checks if interface NetworkList can be used at any 
                    time or only when the product is active.
          \return	true if supported
          \exception    KLSTD::STDE_UNAVAIL is thrown if connector 
                    is being initialized/deinitialized
        */
        virtual bool CanUseNlAlways() = 0;
    };  
};

#endif //__KL_CONN_NETWORKLISTINFO_H__
