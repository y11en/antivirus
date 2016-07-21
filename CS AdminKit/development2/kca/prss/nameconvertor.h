// NameConvertor.h: interface for the CNameConvertor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAMECONVERTOR_H__C46B0CC9_81F1_42DE_8B63_BC82BFAA7BB3__INCLUDED_)
#define AFX_NAMECONVERTOR_H__C46B0CC9_81F1_42DE_8B63_BC82BFAA7BB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

namespace KLPRSS
{
    class NameConvertorNames
    {
    public:
        virtual void Add(const std::wstring& wstrName, const std::wstring& wstrValue) = 0;
    };
    
    class CNameConvertorBase
    {
    protected:
        void convert(const std::wstring& wstrData, NameConvertorNames* pData);
    protected:
		enum TOKEN_TYPE{
			TT_UNKNOWN,
			TT_OPERATOR,
			TT_STRING,
			TT_EOF
		};
        std::wstring	m_wstrToken;
		TOKEN_TYPE		m_nTokenType;
        std::wstring	m_wstrBody;
        int             m_nCurPos;
    protected:
        bool getToken();
        void clear(bool bInConstructor=false);
	    KLSTD_NORETURN void throwError(const wchar_t* szwTemplate, ...);
    };

    class CNameConvertor : public CNameConvertorBase
    {
    public:
        void convert(const std::wstring& wstrData, KLPAR::Params** ppData);
    };

    class CNameConvertor2 : public CNameConvertorBase
    {
    public:
        typedef std::map<std::wstring, std::wstring> data_t;
        void convert(const std::wstring& wstrData, data_t& data);
    };
};

#endif // !defined(AFX_NAMECONVERTOR_H__C46B0CC9_81F1_42DE_8B63_BC82BFAA7BB3__INCLUDED_)
