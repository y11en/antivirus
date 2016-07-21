/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	cmp_helpers.h
 * \author	Andrew Kazachkov
 * \date	10.05.2003 12:35:39
 * \brief	
 * 
 */

#ifndef __KLCMP_HELPERS_H__
#define __KLCMP_HELPERS_H__

namespace KLPRCI
{
    
    class ConnectionInfo
        :   public KLSTD::KLBase
    {
    public:
        virtual void GetConnectionInfo(
                            std::wstring&   wstrLocalName,
                            std::wstring&   wstrRemoteName) = 0;
    };

    typedef enum
    {
        ILT_UNKNOWN,
        ILT_SOCKET,
        ILT_RPC

    }InstanceListenerType;
};
KLCSKCA_DECL int KLPRCI_GetInstancePort(
                    const std::wstring&         storage,                    
                    const KLPRCI::ComponentId&  id,
                    KLPRSS::SettingsStorage*	pBase = NULL);

KLCSKCA_DECL std::wstring KLPRCI_FindInstance(
                    const KLPRCI::ComponentId& id,
                    KLPRSS::SettingsStorage*	pBase = NULL);

KLCSKCA_DECL std::wstring KLPRCI_MakeInstance(
                    const KLPRCI::ComponentId&  id,
                    KLPAR::Params*              pRuntimeInfo);

KLCSKCA_DECL void KLPRCI_SetShutdownFlag();

#endif //__KLCMP_HELPERS_H__

// Local Variables:
// mode: C++
// End:
