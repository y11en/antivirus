/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	NameConvertor.cpp
 * \author	Andrew Kazachkov
 * \date	07.04.2003 12:22:00
 * \brief	
 * 
 */

#include <std/base/klbase.h>
#include <std/err/klerrors.h>
#include <std/par/params.h>
#include "nameconvertor.h"
//#include <cwctype>
#include <ctype.h>
#include <stdarg.h>

#ifndef _WIN32
#include <std/conv/wcharcrt.h>
#endif

#define KLCS_MODULENAME L"KLPRSS"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;

namespace 
{
    class CNameConvertorNamesParams : public NameConvertorNames
    {
    public:
        CNameConvertorNamesParams()
        {
            KLPAR_CreateParams(&m_pData);
        };
        ;
        void Add(const std::wstring& wstrName, const std::wstring& wstrValue)
        {
            CAutoPtr<StringValue> p_wstrValue;
            KLPAR::CreateValue(wstrValue.c_str(), &p_wstrValue);
            m_pData->AddValue(wstrName, p_wstrValue);
        };
        ;
        KLSTD::CAutoPtr<KLPAR::Params> m_pData;
    };

    class CNameConvertorNamesMap : public NameConvertorNames
    {
    public:
        CNameConvertorNamesMap(CNameConvertor2::data_t& data)
            :   m_mapData(data)
        {
            ;
        };
        ;
        void Add(const std::wstring& wstrName, const std::wstring& wstrValue)
        {
            CNameConvertor2::data_t::iterator it = m_mapData.find(wstrName);
            KLSTD_ASSERT_THROW(it != m_mapData.end());
            m_mapData.insert(CNameConvertor2::data_t::value_type(wstrName, wstrValue));
        };
        ;
        CNameConvertor2::data_t& m_mapData;
    };
};

namespace KLPRSS
{
    
    void CNameConvertorBase::convert(const std::wstring& wstrData, NameConvertorNames* pData)
    {
       KLSTD_CHKINPTR(pData);
     clear();
        m_wstrBody=wstrData;
        while(getToken())
        {
            for(;
                m_nTokenType == TT_OPERATOR &&
                (m_wstrToken==L";" || m_wstrToken==L"," || m_wstrToken==L"|" );
                    getToken());

            if(m_nTokenType == TT_EOF)
                break;
            std::wstring wstrName;

            if(m_nTokenType!=TT_STRING)
                throwError(L"String expected");
            wstrName=m_wstrToken;
            getToken();
            if(m_nTokenType!=TT_OPERATOR && m_wstrToken!=L"=")
                throwError(L"Operator = expected");
            getToken();
            if(m_nTokenType!=TT_STRING)
                throwError(L"String expected");
            pData->Add(wstrName, m_wstrToken);
        };
    };

    static bool iswidchar(wchar_t ch)
    {
        if(!ch)
            return false;
       return  iswalnum(ch) || wcschr(L"_./\\%", ch);
    };

    bool CNameConvertorBase::getToken()
    {
		// cleaning
#ifdef N_PLAT_NLM
                m_wstrToken.clear();
#else
		m_wstrToken=L"";
#endif
		m_nTokenType=TT_UNKNOWN;
		// ignoring spaces and comments
		while((unsigned)m_nCurPos<m_wstrBody.size() && iswspace(m_wstrBody[m_nCurPos]))
            m_nCurPos++;
		// checking EOF
		if(m_nCurPos >= (int)m_wstrBody.size()){
			m_nTokenType=TT_EOF;
			return false;
		};
		// recognizing one-char operators
		if(wcschr(L",;=|", m_wstrBody[m_nCurPos])){
			m_nTokenType=TT_OPERATOR;
			m_wstrToken=m_wstrBody[m_nCurPos++];
		}
        // recognizing strings without quotations
        else if (iswidchar(m_wstrBody[m_nCurPos]))
        {
			int nEnd;
			for(nEnd=m_nCurPos; nEnd < m_wstrBody.size() && iswidchar(m_wstrBody[nEnd]); ++nEnd);
			m_wstrToken=std::wstring(&m_wstrBody[m_nCurPos], nEnd-m_nCurPos);
			m_nCurPos=nEnd;
			m_nTokenType=TT_STRING;
        }
		// recognizing strings with quotations
		else if(m_wstrBody[m_nCurPos]==L'\"'){
			int nEnd;
			for(nEnd=m_nCurPos+1; m_wstrBody[nEnd] && m_wstrBody[nEnd]!=L'\"'; ++nEnd);
			if(m_wstrBody[nEnd]!=L'\"')
				throwError(L"Unclosed string.");
			m_wstrToken=std::wstring(&m_wstrBody[m_nCurPos+1], nEnd-m_nCurPos-1);
			m_nCurPos=nEnd+1;
			m_nTokenType=TT_STRING;
		}
		else
			throwError(L"Unexpected symbol '%c'.", m_wstrBody[m_nCurPos]);
		return true;
    };
    
    void CNameConvertorBase::clear(bool bInConstructor)
    {
#ifdef N_PLAT_NLM
        m_wstrToken.clear();
        m_nTokenType=TT_UNKNOWN;
        m_nCurPos=0;
        m_wstrBody.clear();
#else
        m_wstrToken=L"";
        m_nTokenType=TT_UNKNOWN;
        m_nCurPos=0;
        m_wstrBody=L"";
#endif
    };

	KLSTD_NORETURN void CNameConvertorBase::throwError(const wchar_t* szwTemplate, ...)
	{
		va_list marker;
		va_start(marker, szwTemplate);
		wchar_t szwBuffer[1024];
		KLSTD_VSWPRINTF(szwBuffer, KLSTD_COUNTOF(szwBuffer), szwTemplate, marker);
		CAutoPtr<Error> pError;
		::KLERR_CreateError(&pError, L"KLSTD", STDE_BADFORMAT, __FILE__, __LINE__, szwBuffer);
		throw pError.Detach();
	};

    void CNameConvertor::convert(const std::wstring& wstrData, KLPAR::Params** ppData)
    {
        KLSTD_CHKOUTPTR(ppData);
        CNameConvertorNamesParams data;
        CNameConvertorBase::convert(wstrData, &data);
        data.m_pData.CopyTo(ppData);
    };

    void CNameConvertor2::convert(const std::wstring& wstrData, CNameConvertor2::data_t& data)
    {
        CNameConvertorNamesMap Data(data);
        CNameConvertorBase::convert(wstrData, &Data);
    }
};
