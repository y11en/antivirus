/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srvnetscan.h
 * \author	Andrew Kazachkov
 * \date	22.11.2006 16:11:39
 * \brief	
 * 
 */

#ifndef __SRVNETSCAN_H__
#define __SRVNETSCAN_H__

namespace SRVNETGSCAN
{
    class HostInfo
        :   public KLSTD::KLBaseQI
    {
    public:
		virtual void Execute(KLDB::DbConnectionPtr pCon) = 0;
	};

    class HostInfoNoDbConn
        :   public KLSTD::KLBaseQI
    {
    public:
		virtual void Execute() = 0;
	};

    class KLSTD_NOVTABLE GeneralNetScan
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void Create() = 0;
        KLSTD_NOTHROW virtual void Destroy() throw() = 0;
        virtual void AddHostinfo(
                            KLSTD::KLBaseQI*   pHostInfo) = 0;
    };
};

#endif //__SRVNETSCAN_H__
