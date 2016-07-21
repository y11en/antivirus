#ifndef __KLAPP_FIREWALL_H__
#define __KLAPP_FIREWALL_H__
/*KLCSAK_PUBLIC_HEADER*/

#include "./conn_types.h"

namespace KLCONN
{
    typedef enum
    {
        ConnFwPT_UDP = 0,
        ConnFwPT_TCP = 1
    }
    ConnFwPortType;

    typedef enum
    {
        ConnFwPT_Dir_Inbound =  1,
        ConnFwPT_Dir_Outbound = 2,
        ConnFwPT_Dir_InOut =    3
    }
    ConnFwPT_Direction;

    //! port number
    const wchar_t c_szwConnFwPort_Port[] = L"port";             //INT_T

    //! port type (one of ConnFwPT_*)
    const wchar_t c_szwConnFwPort_PortType[] = L"port_type";    //INT_T

    //! direction (one of ConnFwPT_Dir_*)
    const wchar_t c_szwConnFwPort_Direction[] = L"direction";    //INT_T

    //! ports info array
    const wchar_t c_szwConnFwPort_Ports[] = L"ports";           //ARRAY_T|PARAMS_T

    /*!
        \brief  Allows to open specified ports in the product firewall
                Interface is used only if CIF_HAS_FIREWALL flag is set
                in installation info.
                Interface is implemented by connector.
                Interface is acquired with QueryInterface from KLCONN::AppSynchronizer.
    */
    class KLSTD_NOVTABLE AppFirewallHook
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Sets list of ports to leave opened in firewall.

          \param	pData [in] container with ports list
                        +-c_szwConnFwPort_Ports             (ARRAY_T)
                            +-[i]                           (PARAMS_T)
                                +-c_szwConnFwPort_Port      port number (INT_T)
                                +-c_szwConnFwPort_PortType  port type: UDP, TCP (INT_T)
                                +-c_szwConnFwPort_Direction direction
        */
        virtual void SetPortsToOpenList(KLPAR::Params* pData) = 0;
    };
};

#endif //__KLAPP_FIREWALL_H__
