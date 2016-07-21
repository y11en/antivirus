/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	acllib.h
 * \author	Andrew Kazachkov
 * \date	30.09.2003 14:10:43
 * \brief	
 * 
 */

#ifndef __KLACLLIB_H__
#define __KLACLLIB_H__

#include "transport/avt/accesscontrol.h"
#include "transport/klsecurity/exports.h"
#include "transport/wat/authserver.h"

#ifdef _WIN32
#include <tchar.h>
#endif

#ifdef _WIN32
#define AVTL_LIBPARH    _T("AclLibrary")
#endif

namespace AVTL
{
    class KLSTD_NOVTABLE AclLibrary : public KLSTD::KLBase
    {
    public:
        virtual void GetDllProduct(
                            std::wstring&   wstrProduct,
                            std::wstring&   wstrVersion) = 0;

        virtual void GetDllLocation(std::wstring& wstrLocation) = 0;

        virtual KLSEC::DLLType GetDLLType() = 0;

        virtual KLSEC::DLLVersion GetDLLVersion() = 0;

        virtual void FreeACL(const KLAVT::ACE_DECLARATION* pAceDeclList) = 0;

        virtual long GetACLLen(const KLAVT::ACE_DECLARATION* pAceDeclList) = 0;        

        virtual bool GetACLForComponent(
							const KLPRCI::ComponentId&			compId,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount) = 0;
        virtual bool GetACLForComponentTask(
							const KLPRCI::ComponentId&			compId,
							const std::wstring&					wstrTaskName,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount) = 0;
        virtual bool GetACLForSection(
                            const KLSEC_SECTION_DESC&           type,
                            const wchar_t*                      szwProduct,
                            const wchar_t*                      szwVersion,
                            const wchar_t*                      szwSection,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount) = 0;        
        virtual bool GetACLForAction(
							AVP_dword							dwActionGroupID,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount) = 0;
    };
    
	class AutoAclWat
	{
	public:
		AutoAclWat(
                AVTL::AclLibrary*               pSecLib,
				const KLAVT::ACE_DECLARATION*	pAcl,
				long							nAcl)
			:	m_pAcl(pAcl)
			,	m_nAcl(nAcl)
            ,   m_pSecLib(pSecLib)
		{
            ;
        };

		virtual ~AutoAclWat()
		{
			m_pSecLib->FreeACL(m_pAcl);
		};
		const KLAVT::ACE_DECLARATION* GetAcl()
		{
			return m_pAcl;
		};
		long GetAclLen()
		{
			return m_nAcl;
		};
	protected:
		const KLAVT::ACE_DECLARATION*	    m_pAcl;
		long							    m_nAcl;
        KLSTD::CAutoPtr<AVTL::AclLibrary>   m_pSecLib;
	};

}

KLCSTR_DECL void AVTL_AcquireAclLibraryForProduct(
                const std::wstring&     wstrProduct,
                const std::wstring&     wstrVersion,
                AVTL::AclLibrary**      ppLib);

KLCSTR_DECL void AVTL_AcquireAclLibrary(AVTL::AclLibrary** ppLib);


#endif //__KLACLLIB_H__
