/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srv_certdata.h
 * \author	Andrew Kazachkov
 * \date	14.07.2004 17:22:36
 * \brief	
 * 
 */

#ifndef __KLSRV_CERTDATA_H__
#define __KLSRV_CERTDATA_H__

namespace KLSRV
{
    class CertData : public KLSTD::KLBaseImpl<KLSTD::KLBase>
    {
    public:
        CertData(KLPAR::BinaryValue* pPrivKey, const wchar_t* szwPassword)
            :   m_pPrivKey(pPrivKey)
            ,   m_wstrPassword(szwPassword)
        {;};
        virtual ~CertData(){;};
        const wchar_t*  get_Password()
        {
            return m_wstrPassword.c_str();
        };
        KLSTD::CAutoPtr<KLPAR::BinaryValue> get_PrivKey()
        {
            return m_pPrivKey;
        };
    protected:
        const std::wstring                  m_wstrPassword;
        KLSTD::CAutoPtr<KLPAR::BinaryValue> m_pPrivKey;
    };
}

#endif //__KLSRV_CERTDATA_H__