/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ss_bulkmodeimp.h
 * \author	Andrew Kazachkov
 * \date	27.10.2003 13:02:30
 * \brief	
 * 
 */

#ifndef __KL_SS_BULKMODEIMP_H__
#define __KL_SS_BULKMODEIMP_H__

#include "kca/prss/ss_bulkmode.h"
#include "std/base/klbaseqi_imp.h"

namespace KLPRSS
{
    class CBulkMode
        :   public KLPRSS::SsBulkMode
    {
    public:
        CBulkMode();

        KLSTD_INTERAFCE_MAP_REDIRECT(m_pOwner);
        
        void Initialize(
                    KLSTD::KLBaseQI*        pOwner,
                    KLSTD::CriticalSection* pCS,	
		            Storage*			    pStorage,
                    long*				    plTimeout);

        //KLPRSS::SsBulkMode
        void EnumAllWSections(sections_t& sections);

        void ReadSections(
                const sections_t&   sections,
                KLPAR::ArrayValue** ppData);

        void WriteSections(
                const sections_t&   sections,
                KLPAR::ArrayValue*  pData);
        //Internal
        void EnumAllWSectionsI(sections_t& sections, bool bOwnTransaction);

    protected:
        KLSTD::KLBaseQI*                        m_pOwner;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;		
		KLSTD::CAutoPtr<Storage>		        m_pStorage;
        long*						            m_plTimeout;
    };

}

#endif //__KL_SS_BULKMODEIMP_H__
