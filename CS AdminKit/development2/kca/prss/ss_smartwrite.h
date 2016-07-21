/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ss_smartwrite.h
 * \author	Andrew Kazachkov
 * \date	25.12.2003 17:45:38
 * \brief	
 * 
 */

#ifndef KLPRSS__SS_SMARTWRITE_H__
#define KLPRSS__SS_SMARTWRITE_H__
namespace KLPRSS
{
    class KLSTD_NOVTABLE SsExtendedWrite: public KLPRSS::SettingsStorage
    {
    public:
        virtual void WriteSectionUsingPolicy(
                        const std::wstring& wstrProduct,
                        const std::wstring& wstrVersion,
                        const std::wstring& wstrSection,
                        AVP_dword           dwFlags,
                        KLPAR::Params*      pData,
                        KLPAR::Params*      pPolicy) = 0;
    };
}

#endif //KLPRSS__SS_SMARTWRITE_H__
