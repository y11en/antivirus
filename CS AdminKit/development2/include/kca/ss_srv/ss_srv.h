/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ss_srv.h
 * \author	Andrew Kazachkov
 * \date	06.05.2003 14:28:19
 * \brief	SettingsStorage Server public interface
 * 
 */


#ifndef __KLSS_SRV_H__
#define __KLSS_SRV_H__

namespace KLSSS
{    
    const wchar_t c_szwDefaultServerId[]=L"KLSSS_DEFAULT_SERVER";
};

KLCSKCA_DECL void KLSSS_AddSsServer(
                    const std::wstring& wstrProduct,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrID,
                    bool                bLocalOnly=false);

KLCSKCA_DECL bool KLSSS_RemoveSsServer(
                    const std::wstring& wstrID);


KLCSKCA_DECL void KLSSS_SetDefaultServer(
                    const std::wstring& wstrProduct,
                    const std::wstring& wstrVersion,
                    const std::wstring& wstrID);

KLCSKCA_DECL void KLSSS_GetDefaultServer(
                    std::wstring&       wstrProduct,
                    std::wstring&       wstrVersion,
                    std::wstring&       wstrID);

#endif //__KLSS_SRV_H__

// Local Variables:
// mode: C++
// End:
