/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	hstinfo2db.h
 * \author	
 * \date	12.05.2006 14:30:45
 * \brief	Internal interface
 * 
 */

#ifndef __KL_HSTINFO2DB_H__
#define __KL_HSTINFO2DB_H__

namespace HST2DB
{
    class KLSTD_NOVTABLE HostInfo2Db
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Method is called if list of competitor's products 
                    installed on specified host was changed.

          \param	pCon        IN db connection
          \param	lHostId     IN host id
          \param	wstrHostId  IN string host id
        */
        virtual void OnCompetitorProductsListChanged(
                            KLDB::DbConnectionPtr   pCon,
                            long                    lHostId,
                            const std::wstring&     wstrHostId) = 0;
    };
};

#endif //__KL_HSTINFO2DB_H__
