#ifndef __CONN_APPINST_H__
#define __CONN_APPINST_H__

#include <nagent/conn/connector.h>
#include <std/base/klbaseqi_imp.h>
#include <nagent/connapp/conn_constants.h>
#include <nagent/conn/app_firewall.h>

namespace KLCONNAPP
{
    enum
    {
        REPL_TYPE_SETTINGS = 1,
        REPL_TYPE_POLICY = 2,
        REPL_TYPE_TASKS = 4,
        REPL_TYPE_GTASKS = 8,
        REPL_TYPE_APP_PROPS = 16,
        REPL_TYPE_TSK_STATES = 32,
        REPL_TYPE_RTP_STATE = 64,
        REPL_TYPE_UNCURED = 128,
        REPL_TYPE_AUTOKEYS = 256,
        REPL_TYPE_ALL = 0xFFFF
    };

    enum{
        REPL_DF_CHECK_BASE = 1,
        REPL_DF_CHECK_COMPAT = 2
    };

    class KLSTD_NOVTABLE ConnectorAppInst
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	ForceReplication

          \param	dwType [in] REPL_* flags
          \param    bSync [in] if true force synchronously
        */
        virtual void ForceReplication(AVP_dword dwType, bool bSync) = 0;

        /*!
          \brief	SetDebugFlags

          \param	dwFlags [in] REPL_DF_*
          \param	dwMask  [in] REPL_DF_*
          \return	previos value
        */
        virtual AVP_dword SetDebugFlags(AVP_dword dwFlags, AVP_dword dwMask) = 0;

        /*!
          \brief	GetConnectorStatistics

          \param	ppData [out]
        */
        virtual void GetConnectorStatistics(KLPAR::Params** ppData) = 0;

        /*!
          \brief	List of ports to open in firewall might have been changed
        */
        virtual void PortsToOpenInFirewallChanged() = 0;
        
        /*!
          \brief	Checks whether product was replicated at least once

          \return	true if the product was replicated at least once
        */
        virtual bool WasReplicatedOnce() = 0;
        
        //! returns number of uncured items
        virtual bool AcquireNumberOfUncured(long& nUncured) = 0;
    };
    
    //! iplemented by 
    class ConnectorInfo
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void Conn_GetPortsToOpenInFirewall(
                            KLPAR::ParamsPtr& pPortsData) = 0;
    };
};

#endif //__CONN_APPINST_H__
