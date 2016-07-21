#ifndef __WIN32_UNIMPERSONATE_H__
#define __WIN32_UNIMPERSONATE_H__

namespace KLWAT
{
#ifdef KLSTD_WINNT
    class CAutoUnimpersonate
    {
    public:
        CAutoUnimpersonate();
        ~CAutoUnimpersonate();
    protected:
        HANDLE m_hToken;
    };

    CAutoUnimpersonate::CAutoUnimpersonate()
        :   m_hToken(NULL)
    {
        DWORD dwAccessMask=
                          STANDARD_RIGHTS_REQUIRED  |
                          TOKEN_ASSIGN_PRIMARY      |
                          TOKEN_DUPLICATE           |
                          TOKEN_IMPERSONATE         |
                          TOKEN_QUERY               |
                          TOKEN_QUERY_SOURCE;

        if(!OpenThreadToken(GetCurrentThread(), dwAccessMask , TRUE, &m_hToken))
        {
            DWORD dwErrCode=GetLastError();
            KLSTD_ASSERT(dwErrCode == ERROR_NO_TOKEN);
            if( dwErrCode != ERROR_NO_TOKEN )
            {
                KLSTD_THROW_LASTERROR_CODE2(dwErrCode);
            };
        }
        else
        {
            if(!SetThreadToken(NULL, NULL))
                KLSTD_THROW_LASTERROR2();
        };
    };

    CAutoUnimpersonate::~CAutoUnimpersonate()
    {
        if(m_hToken)
        {
            if(!SetThreadToken(NULL, m_hToken))
            {
                KLSTD_ASSERT(false);
                KLSTD::Trace(1, L"KLWAT", L"Cannot revert thread token !!!\n");
            }
            //else
                //m_hToken=NULL;
            //if(m_hToken)
            //{
            BOOL bResult = CloseHandle(m_hToken);
            KLSTD_ASSERT(bResult);
            m_hToken=NULL;
            //};
        };
    };
#else
    class CAutoUnimpersonate
    {
    public:
        KLSTD_INLINEONLY CAutoUnimpersonate(){;};
        KLSTD_INLINEONLY ~CAutoUnimpersonate(){;};
    };
#endif
};

#endif //__WIN32_UNIMPERSONATE_H__
