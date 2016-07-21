/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sssrv_server.h
 * \author	Andrew Kazachkov
 * \date	05.05.2003 12:44:29
 * \brief	Settings Storage Server internal interface
 * 
 */

#ifndef __KLSSSRV_SERVER_H__
#define __KLSSSRV_SERVER_H__

#include <std/thr/locks.h>
#include <kca/prci/componentinstance.h>
#include <kca/prss/ss_bulkmode.h>

namespace KLSSS
{
    class KLSTD_NOVTABLE SettingsStorageServer : public KLSTD::KLBase
    {
    public:
        virtual bool AccessCheck(
                            const wchar_t*      szwProxy,
                            const wchar_t*      szwSS,
                            const wchar_t*      szwProduct,
                            const wchar_t*      szwVersion,
                            const wchar_t*      szwSection,
                            KLPRCI::SecContext* pContext,
							AVP_dword           dwAccessMask,
							bool                bThrow) = 0;

        virtual void OnCloseConnection(
                    const std::wstring& wstrConnection) =0;

        virtual void Close(
                    const std::wstring& wstrProxyId) =0;

        virtual void CreateSettingsStorage(
                    const std::wstring& wstrConnection,
                    const std::wstring& wstrLocation,
					AVP_dword           dwCreationFlags,
					AVP_dword           dwAccessFlags,
                    std::wstring&       wstrProxyId,
                    const std::wstring& wstrSSUser,
                    const KLPRSS::ss_format_t* pFormat) = 0;

        virtual void CreateSettingsStorage(
                    const std::wstring& wstrConnection,
                    const std::wstring& wstrLocation,
					AVP_dword           dwCreationFlags,
					AVP_dword           dwAccessFlags,
                    std::wstring&       wstrProxyId,
                    const std::wstring& wstrSSUser,
                    KLPAR::Params*      pFormat) = 0;

        virtual void Read(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection,
                    KLPAR::Params*      pExtra,
                    KLPAR::Params**     ppData) = 0;

        virtual void Write(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection,
                    AVP_dword           dwFlags,
                    KLPAR::Params*      pData) = 0;


        virtual void CreateSection(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection) = 0;

        virtual void DeleteSection(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection) = 0;

        virtual void SetTimeout(
                    const std::wstring& wstrProxyId,
                    AVP_dword           dwTimeout) = 0;

		virtual void GetNames(
                    const std::wstring&         wstrProxyId,
                    const std::wstring&         wstrName,
                    const std::wstring&         wstrVersion,
			        std::vector<std::wstring>&	vecNames) = 0;
        
        virtual void AttrRead(
                    const std::wstring& wstrProxyId,
                    const std::wstring& wstrName,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrSection,
                    const std::wstring& wstrAttr,
                    KLPAR::Value**      ppValue)=0;

        virtual void EnumAllWSections(
                    const std::wstring& wstrProxyId,
                    KLPRSS::sections_t& sections) = 0;

        virtual void ReadSections(
                    const std::wstring& wstrProxyId,
                    const KLPRSS::sections_t&   sections,
                    KLPAR::ArrayValue** ppData) = 0;

        virtual void WriteSections(
                    const std::wstring& wstrProxyId,
                    const KLPRSS::sections_t&   sections,
                    KLPAR::ArrayValue*  pData) = 0;
    };

    extern KLSTD::LockCount m_lckServer, m_lckProxy;
    extern volatile bool g_bWasInitialized;

    bool GetSsServer(const std::wstring& wstrId, SettingsStorageServer** ppServer, bool bExceptIfNotFound = true);
    void PutSsServer(const std::wstring& wstrId, SettingsStorageServer* pServer);
};

    KLCSKCA_DECL int KLSSS_ReadPortValue(
                const std::wstring& wstrProduct,
                const std::wstring& wstrVersion,
                const std::wstring& wstrID);

    KLCSKCA_DECL void KLSSS_WritePortValue(
                const std::wstring& wstrProduct,
                const std::wstring& wstrVersion,
                const std::wstring& wstrID,
                int                 nPortValue);

/*!
  \brief	Instantiates SettingsStorageServer.
  \param    wstrProduct 
  \param    wstrVersion
  \param    wstrID
  \param    bLocalOnly - if set then server cannot be accessed via IPC. You
            should set this flag, if you don't plan to use the server from
            another process.
*/
void KLSSS_CreateSettingsStorageServer(
                    const std::wstring&             wstrProduct,
                    const std::wstring&             wstrVersion,
                    const std::wstring&             wstrID,
                    KLSSS::SettingsStorageServer**  ppServer,
                    bool                            bLocalOnly);

#endif //__KLSSSRV_SERVER_H__

// Local Variables:
// mode: C++
// End:
