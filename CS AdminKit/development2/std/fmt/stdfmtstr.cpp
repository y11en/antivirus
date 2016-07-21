#include <std/base/klbase.h>
#include <std/base/klstdutils.h>
#include <std/fmt/stdfmtstr.h>

#include <sstream>

#include <wctype.h>


/*
[1]    [2]                                                                   [1]
--('%')--+--('0')----------------------------------------------------------+--
         |          [3]               [4]     [5]           [6]        [7] |
         +--(<digit>)--+--(<digit>)--+--+--('!')--(<fmtstring>)--('!')--+--+
         |             |             |  |                               |  |
         |             +-------------+  |                               |  |
         |                              |                               |  |
         |                              +-------------------------------+  |
         |                                                                 |
         +--('n')----------------------------------------------------------+
         |                                                                 |
         +--(' ')----------------------------------------------------------+
         |                                                                 |
         +--('.')----------------------------------------------------------+
         |                                                                 |
         +--('!')----------------------------------------------------------+
         |                                                                 |
         +--('%')----------------------------------------------------------+

*/

namespace
{
    enum dfm_state_t
    {
        dfmt_st_1 = 1,
        dfmt_st_2,
        dfmt_st_3,
        dfmt_st_4,
        dfmt_st_5,
        dfmt_st_6,
        dfmt_st_7
    };

    const wchar_t c_nPcnt = L'%';
    const wchar_t c_nNull = L'\0';
    const wchar_t c_nUnk = L'?';

    struct state_t
    {
        state_t()
            :   m_nIndex(0)
            ,   m_nState(dfmt_st_1)
        {
            m_szwIndexBuffer[0] = 
                m_szwIndexBuffer[1] = 
                m_szwIndexBuffer[2] = 0;
        };
        size_t          m_nIndex;
        dfm_state_t     m_nState;
        wchar_t         m_szwIndexBuffer[3];
    };
};

KLCSC_DECL bool KLSTD_FormatMessage(
                    const wchar_t*  szwTemplate,
                    const wchar_t** pszwArgs,
                    size_t          nArgs,
                    KLSTD::AKWSTR&  wstrResult)
{
    KLSTD_ASSERT(szwTemplate && szwTemplate[0]);
    bool bResult = true;
    if(KLSTD::c_nStdAutoArgs == nArgs)
    {
        size_t i = 0;
        for(;pszwArgs[i]; ++i);
        nArgs = i;
    };
    std::wostringstream os;
    state_t state;
    for(bool bStop = false; !bStop;)
    {
        wchar_t ch = szwTemplate[state.m_nIndex];
        bool bIncrementIndex = false;
        switch(state.m_nState)
        {
        case dfmt_st_1:
            if(c_nPcnt == ch)
                state.m_nState = dfmt_st_2;
            else if(ch)
                os <<  ch;
            bIncrementIndex = true;            
            break;
        case dfmt_st_2:
            switch(ch)
            {
            case '0':
                state.m_nState = dfmt_st_1;
                os << c_nNull;
                bIncrementIndex = true;
                break;
            case 'n':
                state.m_nState = dfmt_st_1;
                os << std::endl;
                bIncrementIndex = true;
                break;
            case ' ':
                state.m_nState = dfmt_st_1;
                os << L' ';
                bIncrementIndex = true;
                break;
            case '.':
                state.m_nState = dfmt_st_1;
                os << L'.';
                bIncrementIndex = true;
                break;
            case '!':
                state.m_nState = dfmt_st_1;
                os << L'!';
                bIncrementIndex = true;
                break;
            case '%':
                state.m_nState = dfmt_st_1;
                os << L'%';
                bIncrementIndex = true;
                break;
            default:
#ifdef N_PLAT_NLM
                if(isdigit((char)ch))
#else
                if(iswdigit(ch))
#endif
                {
                    state.m_nState = dfmt_st_3;
                    state.m_szwIndexBuffer[0] = ch;
                    bIncrementIndex = true;
                }
                else
                    state.m_nState = dfmt_st_1;
            };
            break;
        case dfmt_st_3:            
#ifdef N_PLAT_NLM
                if(isdigit((char)ch))
#else
                if(iswdigit(ch))
#endif
            {
                state.m_nState = dfmt_st_4;
                state.m_szwIndexBuffer[1] = ch;
                state.m_szwIndexBuffer[2] = 0;
                bIncrementIndex = true;
            }
            else
            {
                state.m_szwIndexBuffer[1] = 0;
                state.m_szwIndexBuffer[2] = 0;
                state.m_nState = dfmt_st_4;
            }
            break;
        case dfmt_st_4:
            if(L'!' == ch)
            {
                state.m_nState = dfmt_st_5;
                bIncrementIndex = true;
            }
            else
                state.m_nState = dfmt_st_7;
            break;
        case dfmt_st_5:
            if(L's' == ch)
            {
                state.m_nState = dfmt_st_6;
                bIncrementIndex = true;
            }
            else
                state.m_nState = dfmt_st_1;
            break;
        case dfmt_st_6:
            if(L'!' == ch)
            {
                state.m_nState = dfmt_st_7;
                bIncrementIndex = true;
            }
            else
                state.m_nState = dfmt_st_1;
            break;
        case dfmt_st_7:
            state.m_nState = dfmt_st_1;
            wchar_t * endptr = NULL;
            size_t nIndex = (size_t)wcstol(&state.m_szwIndexBuffer[0], &endptr, 10);
            if(nIndex <= nArgs)
                os << pszwArgs[ nIndex -1 ];
            else
                os << c_nUnk;
            break;
        };
        if(bIncrementIndex)
        {
            if(!ch)
                bStop = true;
            else
                ++state.m_nIndex;
        };
    };
    wstrResult = KLSTD::klwstr_t(os.str().c_str()).detach();
    return bResult;
};
