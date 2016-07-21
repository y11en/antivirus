/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	srchfilter.h
 * \author	Andrew Kazachkov
 * \date	15.12.2002 11:06:59
 * \brief	Handling RFC2254-like filters
 * 
 */

#ifndef __KLSRCHFILTER_H__
#define __KLSRCHFILTER_H__

#include <string>
#include <map>
#include <srvp/csp/klcsp.h>
#include <server/db/dbliterals.h>

namespace KLFLT
{
    void Initialize_KLFLT();

    void Deinitialize_KLFLT();

	typedef std::map<std::wstring, std::wstring> str2str_map_t;

    typedef void FilterReaderCallBack(void* pContext, str2str_map_t::const_iterator it);

    void MakeSelectList(
                const std::vector<std::wstring>&    vecFields,
                const str2str_map_t&                conv,
                FilterReaderCallBack*               pCallback,
                void*                               pContext,
                std::wiostream&                     os,
                bool                                bDistinct = true);

    void MakeOrderList(
                const KLCSP::vec_field_order_t&     vecFieldsToOrder,
                const str2str_map_t&                conv,
                std::wiostream&                     os);
    
    void MakeOrderList(
                const KLCSP::vec_field_order_t&     vecFieldsToOrder,
                std::wiostream&                     os);
    

    class KLSTD_NOVTABLE FRCallback
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void OnValue(
                    str2str_map_t::const_iterator it) = 0;
    };

    class KLSTD_NOVTABLE FRInclusionCallback
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual bool IfInclusion(
                    KLDB::DbLiterals*   pLiterals,
                    const wchar_t*      szwAttribute,
                    const wchar_t*      szwValue,
                    std::wstring&       wstrInclusion) = 0;
    };

	class CFilterReader
	{
	public:
        CFilterReader();
        ~CFilterReader();
		std::wstring Parse(
                        const std::wstring&     wstrFilter,
                        const str2str_map_t*    pConv,
                        KLSTD::KLBaseQI*        pLiterals,
                        FilterReaderCallBack*   pCallback=NULL,
                        void*                   pContext=NULL);

		std::wstring Parse2(
                        const std::wstring&     wstrFilter,
                        const str2str_map_t*    pConv,
                        KLSTD::KLBaseQI*        pLiterals,
                        KLSTD::KLBaseQI*        pCallback);
    protected:
        static bool ConvertOp(
                        const std::wstring& wstrOp,
                        std::wstring&       wstrRes,
                        AVP_dword&          dwType);
	protected:
        KLSTD::CAutoPtr<KLDB::DbLiterals>       m_pLiterals;
        KLSTD::CAutoPtr<FRCallback>             m_pFRCallback;
        KLSTD::CAutoPtr<FRInclusionCallback>    m_pFRInclusionCallback;
		enum TOKEN_TYPE
		{
			TT_UNKNOWN,
			TT_OPERATOR,
			TT_STRING,
			TT_NUMBER,
            TT_BIGNUMBER,
            TT_DATETIME,
			TT_EOF
		};
		KLSTD_NORETURN void throwError(const wchar_t* szwTemplate, ...);
		bool getToken();
		void Process_Filter(bool bFirst=false);
		void Process_AttrOpVal();
		void Process_OpFlts();

        void Process_CurTime_Call(KLSTD::precise_time_t& tmValue);

        //token
		std::wstring	m_wstrToken;
		long			m_lToken;
        __int64			m_llToken;
        time_t          m_tmToken;
		TOKEN_TYPE		m_nTokenType;
        AVP_dword       m_dwSubtype;
        ///////////////
        bool            m_bUseWildCards;
		///////////////
		std::wstring			m_wstrSrc;
		size_t					m_nCurPos, m_nErrPos;
		std::wstring			m_wstrDst;
		const str2str_map_t*	m_pConv;
        ///////////////
        KLSTD::precise_time_t   m_tmCurrent;
	};
};


#endif //__KLSRCHFILTER_H__