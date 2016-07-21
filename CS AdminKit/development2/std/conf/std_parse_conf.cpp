/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	std_read_conf.cpp
 * \author	Andrew Kazachkov
 * \date	20.10.2006 18:50:49
 * \brief	
 * 
 */

#include <std/base/klstd.h>
#include <std/base/klbaseqi_imp.h>
#include <std/conf/std_conf.h>
#include <std/conf/errors.h>
#include <common/measurer.h>
#include <std/par/paramslogger.h>

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <wctype.h>
#include <stdarg.h>

#ifdef _WIN32
    #include <std/win32/klos_win32v40.h> //for MultiByte2WideChar only !!!
    #include <std/win32/klos_win32_errors.h>
    #include <tchar.h>
#endif

#define KLCS_MODULENAME L"KLSTDCONF"

#define KLSTDCONF_THROW1(_code, _arg1)  \
            KLERR_THROW1(KLCS_MODULENAME, _code, _arg1)

#define KLSTDCONF_THROW2(_code, _arg1, _arg2)   \
            KLERR_THROW2(KLCS_MODULENAME, _code, _arg1, _arg2)

#define KLSTDCONF_THROW3(_code, _arg1, _arg2, _arg3)    \
            KLERR_THROW3(KLCS_MODULENAME, _code, _arg1, _arg2, _arg3)

namespace KLSTDCONF
{
    const size_t c_nBufferSize = 1024*32;

    inline bool my_isspace(wchar_t ch)
    {
        return L'\n' != ch && iswspace(ch);
    };

    inline bool my_isalpha(wchar_t ch)
    {
        return L'_' == ch || iswalpha(ch);
    };

    inline bool my_isdigit(wchar_t ch)
    {
        return iswdigit(ch) != 0;
    };

    inline bool my_isalphanum(wchar_t ch)
    {
        return my_isalpha(ch) || my_isdigit(ch);
    };

/*
              +--([)--(OSPC)--(id)--(OSPC)--(])----+          +--(#)--(...)--+
              |                                    |          |              |
    --(OSPC)--+------------------------------------+--(OSPC)--+--------------+--(ENTER)-->
              |                                    |
              +--(id)--(OSPC)--(=)--(OSPC)--(VAL)--+


*/

    enum token_type_t
    {
        TT_UNKNOWN,
        TT_EOF,
        TT_EOL,
        TT_ID,
        TT_PREP,
        TT_STRING,
        TT_NUMBER,
        TT_COMMENT
    };

    struct token_t
    {
        token_t()
            :   m_nType(TT_UNKNOWN)
        {;};
        void clear()
        {
            m_nType = TT_UNKNOWN;
            m_wstrToken.clear();
        };

        token_type_t    m_nType;
        std::wstring    m_wstrToken;
    };

    class KLSTD_NOVTABLE CParseData
    {
    public:
        CParseData();
        virtual ~CParseData();
        void Start(const wchar_t* szwData);
    protected:
        bool getToken(token_t& oToken);
    protected:
        void Process();
        void Process_Section(token_t& oToken);
        void Process_Variable(token_t& oToken);
        virtual void OnStartSection(const wchar_t* szwName) = 0;
        virtual void OnStartVariable(const wchar_t* szwName, const wchar_t* szwValue) = 0;
    protected:
        std::wstring    m_wstrData;
        size_t          m_nOffset;
    };

    CParseData::CParseData()
        :   m_nOffset(0)
    {
        ;
    };

    CParseData::~CParseData()
    {
        ;
    };

    bool CParseData::getToken(token_t& oToken)
    {
        oToken.clear();
    // ignoring spaces
        while( m_nOffset < m_wstrData.size() &&  my_isspace(m_wstrData[m_nOffset]))
            ++m_nOffset;
    // checking EOF
        if(m_nOffset >= m_wstrData.size())
        {
            oToken.m_nType = TT_EOF;
            return false;
        };
    // recognizing one-char operators
        if(L'\n' == m_wstrData[m_nOffset])
        {
            oToken.m_nType = TT_EOL;
            ++m_nOffset;
        }
        else if(wcschr(L"[]=", m_wstrData[m_nOffset]) )
        {
            oToken.m_nType = TT_PREP;
            oToken.m_wstrToken = m_wstrData[m_nOffset];
            ++m_nOffset;
        }
    // recognizing ids
        else if( my_isalpha( m_wstrData[m_nOffset] ) )
        {
            while( m_nOffset < m_wstrData.size() && my_isalphanum( m_wstrData[m_nOffset] ) )
                oToken.m_wstrToken += m_wstrData[m_nOffset++];
            oToken.m_nType = TT_ID;
        }
    // recognizing numbers
        else if(    (   L'-' == m_wstrData[m_nOffset] &&
                        m_nOffset + 1 < m_wstrData.size() &&
                        my_isdigit( m_wstrData[m_nOffset+1]) )   ||
                    my_isdigit( m_wstrData[m_nOffset] ) )
        {
            if(L'-' == m_wstrData[m_nOffset])
            {
                oToken.m_wstrToken = m_wstrData[m_nOffset];
                ++m_nOffset;
            };
            while( m_nOffset < m_wstrData.size() && my_isdigit( m_wstrData[m_nOffset] ) )
                oToken.m_wstrToken += m_wstrData[m_nOffset++];
            oToken.m_nType = TT_NUMBER;
        }
    // quoted string, escaping is supported 
        else if(L'\"' == m_wstrData[m_nOffset])
        {
            bool bEscape = false;
            size_t nEnd;
            for(    nEnd = m_nOffset + 1;
                    nEnd < m_wstrData.size();
                    ++nEnd)
            {
                wchar_t ch = m_wstrData[nEnd];
                if( L'\\' == ch && !bEscape )
                {
                    bEscape = true;
                    ch = 0;
                }
                else if(L'\n' == ch)
                {
                    KLSTDCONF_THROW1(
                                FCONF_NEW_LINE_IN_CONSTANT,
                                std::wstring(&m_wstrData[m_nOffset], nEnd-m_nOffset).c_str());
                }
                else if(bEscape)
                {
                    switch(ch)
                    {
                    case L'a': //Bell (alert) 
                        ch = L'\a';
                        break;
                    case L'b': //Backspace 
                        ch = L'\b';
                        break;
                    case L'f': //Formfeed 
                        ch = L'\f';
                        break;
                    case L'n': //New line 
                        ch = L'\n';
                        break;
                    case L'r': //Carriage return 
                        ch = L'\r';
                        break;
                    case L't': //Horizontal tab 
                        ch = L'\t';
                        break;
                    case L'v': //Vertical tab 
                        ch = L'\v';
                        break;
                    case L'\'': //Single quotation mark 
                        ch = L'\'';
                        break;
                    case L'\"': //Double quotation mark 
                        ch = L'\"';
                        break;
                    case L'\\': //Backslash 
                        ch = L'\\';
                        break;
                    };
                    bEscape = false;
                }
                else
                {
                    if( L'\"' == ch )
                        break;
                };
                if(ch)
                    oToken.m_wstrToken.push_back(ch);
            };
            if(m_wstrData[nEnd] != L'\"')
                KLSTDCONF_THROW1(
                                FCONF_UNCLOSED_STRING,
                                std::wstring(&m_wstrData[m_nOffset], nEnd-m_nOffset).c_str());
            //oToken.m_wstrToken = std::wstring(&m_wstrData[m_nOffset+1], nEnd-m_nOffset-1);
            m_nOffset = nEnd+1;
            oToken.m_nType = TT_STRING;
        }
    // recognizing comments
        else if(L'#' == m_wstrData[m_nOffset])
        {
            ++m_nOffset;
            while( m_nOffset < m_wstrData.size() && L'\n' != m_wstrData[m_nOffset] )
            {
                if(L'\r' != m_wstrData[m_nOffset])
                    oToken.m_wstrToken += m_wstrData[m_nOffset++];
            };
            oToken.m_nType = TT_COMMENT;
        }
        else
            KLSTDCONF_THROW1(FCONF_UNKNOWN_STATEMENT, &m_wstrData[m_nOffset]);
        return true;
    };

    void CParseData::Start(const wchar_t* szwData)
    {
        KLSTD_CHKINPTR(szwData);
        KLSTD_TRACE1(5, L"CParseData::Start('%ls')\n", szwData);
        m_wstrData = szwData;
        m_nOffset = 0;
        Process();
    };

    void CParseData::Process()
    {
        token_t oToken;
        //each iteration processes one line of file
        while( getToken(oToken) )
        {
            if(TT_ID == oToken.m_nType)
            {// variable declared
                Process_Variable(oToken);
            }
            else if(TT_PREP == oToken.m_nType && L"[" == oToken.m_wstrToken)
            {//section declared
                Process_Section(oToken);
            }
            else if(    TT_EOL == oToken.m_nType ||
                        TT_EOF == oToken.m_nType ||
                        TT_COMMENT == oToken.m_nType)
            {
                ;//do nothing
            }
            else
                KLSTDCONF_THROW2(
                            FCONF_EXPECTED_OTHER,
                            oToken.m_wstrToken.c_str(),
                            L"section or variable declaration");
        };
    };

    void CParseData::Process_Section(token_t&)
    {
    // already have [. must be ID (section name) now
        token_t oSectionName;
        if(!getToken(oSectionName))
            KLSTDCONF_THROW2(
                    FCONF_EXPECTED_OTHER,
                    "<EOF>",
                    L"section name");
        if(TT_ID != oSectionName.m_nType)
            KLSTDCONF_THROW2(
                    FCONF_EXPECTED_OTHER,
                    oSectionName.m_wstrToken.c_str(),
                    L"section name");
    // must be ] now
        token_t oToken;
        if(!getToken(oToken))
            KLSTDCONF_THROW2(
                FCONF_EXPECTED_OTHER,
                "<EOF>",
                L"]");
        if(TT_PREP != oToken.m_nType || L"]" != oToken.m_wstrToken)
            KLSTDCONF_THROW2(
                    FCONF_EXPECTED_OTHER,
                    oToken.m_wstrToken.c_str(),
                    L"]");
    // must EOF or EOL or comment
        if(getToken(oToken))
        {//must be comment or EOL
            if( TT_COMMENT == oToken.m_nType)
            {
                getToken(oToken);
            };
            if( TT_EOL != oToken.m_nType && TT_EOF != oToken.m_nType)
                KLSTDCONF_THROW2(
                        FCONF_EXPECTED_OTHER,
                        oToken.m_wstrToken.c_str(),
                        L"<EOL>");
        };
        OnStartSection(oSectionName.m_wstrToken.c_str());
    };

    void CParseData::Process_Variable(token_t& oVarName)
    {
        // already have name. so must be = now
        token_t oToken;
        if(!getToken(oToken))
            KLSTDCONF_THROW2(
                    FCONF_EXPECTED_OTHER,
                    "<EOF>",
                    L"=");
        if(TT_PREP != oToken.m_nType || L"=" != oToken.m_wstrToken)
            KLSTDCONF_THROW2(
                    FCONF_EXPECTED_OTHER,
                    oToken.m_wstrToken.c_str(),
                    L"=");
        token_t oValue;
        if(getToken(oValue))
        {
            switch(oValue.m_nType)
            {
            case TT_ID:     // unquoted string
            case TT_STRING: // quoted string
            case TT_NUMBER: // number
            case TT_PREP: // preposition
            case TT_EOL: // EOL
                break;
            case TT_COMMENT: // comment
                oValue.m_wstrToken.clear();
                break;
            default:
                KLSTDCONF_THROW2(
                        FCONF_EXPECTED_OTHER,
                        oValue.m_wstrToken.c_str(),
                        L"<value>");
                break;
            };
        };
        if( TT_EOF != oValue.m_nType &&
            TT_EOL != oValue.m_nType &&
            getToken(oToken))
        {//must be comment or EOL
            if( TT_COMMENT == oToken.m_nType)
            {
                getToken(oToken);
            };
            if( TT_EOF != oToken.m_nType && TT_EOL != oToken.m_nType )
                KLSTDCONF_THROW2(
                        FCONF_EXPECTED_OTHER,
                        oToken.m_wstrToken.c_str(),
                        L"<EOL>");
        };
        OnStartVariable(oVarName.m_wstrToken.c_str(), oValue.m_wstrToken.c_str());
    };

    typedef enum
    {
        TFT_UNKNOWN = 0,
        TFT_CURRENT_CODEPAGE,
        TFT_UNICODE_UCS16,
        TFT_UNICODE_UCS16_BIGENDIAN,
        TFT_UNICODE_UTF8
    }
    TextFileType;

    class CTextFileReader
        :   public KLSTD::KLBaseImpl<KLSTD::TextFileLineReader>
    {
    protected:    
        CTextFileReader(const wchar_t* szwFileName);
        virtual ~CTextFileReader();
        virtual void ReadLine(std::wstring& wstrLine);
        virtual bool IsEOF();

        KLSTD_SINGLE_INTERAFCE_MAP(KLSTD::TextFileLineReader);

    public:
        static KLSTD::TextFileLineReaderPtr Instantiate(const wchar_t* szwFileName)
        {
            KLSTD::TextFileLineReaderPtr pResult;
            pResult.Attach(new CTextFileReader(szwFileName));
            return pResult;
        };
    protected:
        FILE*               m_pFile;
        TextFileType        m_nType;
        bool                m_bEOF;
    };

#ifdef _WIN32
    namespace
    {
        TextFileType RecognizeText(FILE* pFile)
        {
            TextFileType nRes = TFT_UNKNOWN;
            ;
            char pUnicode[4] = {0};
            size_t nWasRead = fread(&pUnicode, 1, KLSTD_COUNTOF(pUnicode), pFile);
            if(nWasRead != KLSTD_COUNTOF(pUnicode))
            {
                int nError = ferror(pFile);
                if(nError)
                {
                    KLSTD_THROW_ERRNO_CODE(nError);
                };
            };

            const AVP_byte c0 = pUnicode[0];
            const AVP_byte c1 = pUnicode[1];
            const AVP_byte c2 = pUnicode[2];
            const AVP_byte c3 = pUnicode[3];

            size_t nBytes2Return = nWasRead;

            if(nWasRead >= 3 && 0xEF == c0 && 0xBB == c1 && 0xBF == c2)
            {
                nRes = TFT_UNICODE_UTF8;
                nBytes2Return = nWasRead - 3;
            }
            else if(nWasRead == 4 && 0xFF == c0 && 0xFE == c1 && 0x00 == c2 && 0x00 == c3)
            {
                //UCS-4, little endian, not supported
                KLSTD_THROW(KLSTD::STDE_BADFORMAT);
            }
            else if(nWasRead == 4 && 0x00 == c0 && 0x00 == c1 && 0xFE == c2 && 0xFF == c3)
            {
                //UCS-4, big endian, not supported
                KLSTD_THROW(KLSTD::STDE_BADFORMAT);
            }
            else if( nWasRead >= 2 && 0xFF == c0 && 0xFE == c1 )
            {
                nRes = TFT_UNICODE_UCS16;
                nBytes2Return = nWasRead - 2;
            }
            else if( nWasRead >= 2 && 0xFE == c0 && 0xFF == c1 )
            {
                nRes = TFT_UNICODE_UCS16_BIGENDIAN;
                nBytes2Return = nWasRead - 2;
            }
            else
                nRes = TFT_CURRENT_CODEPAGE;

            for(size_t i = 0; i < nBytes2Return; ++i)
            {
                const char ch2Push = pUnicode[nWasRead - i - 1];
                KLSTD_ASSERT_THROW( EOF != ungetc(ch2Push, pFile) );
            };
            return nRes;
        };
    };
#else
        TextFileType RecognizeText(FILE* pFile)
        {
            TextFileType nRes = TFT_CURRENT_CODEPAGE;
        };
#endif


    CTextFileReader::CTextFileReader(const wchar_t* szwFileName)
        :   m_pFile(NULL)
        ,   m_nType(TFT_UNKNOWN)
        ,   m_bEOF(false)
    {
        KLERR_TRY
            KLSTD_TRACE1(4, L"CTextFileReader::CTextFileReader('%ls')\n", szwFileName);
#ifdef _WIN32
            m_pFile = _tfopen(KLSTD_W2CT2(szwFileName), _T("rb"));
#else
            m_pFile = fopen(KLSTD_W2CA2(szwFileName), "r");
#endif
            if(!m_pFile)
            {
                KLSTD_THROW_ERRNO();
            };            
            int nRes = setvbuf(m_pFile, NULL, _IOFBF, c_nBufferSize);
            KLSTD_ASSERT(nRes == 0);
            m_nType = RecognizeText(m_pFile);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(4, pError);
            if(m_pFile)
            {
                fclose(m_pFile);
                m_pFile = NULL;
            };
        KLERR_FINALLY
            KLERR_RETHROW();
        KLERR_ENDTRY
    };

    CTextFileReader::~CTextFileReader()
    {
        if(m_pFile)
        {
            fclose(m_pFile);
            m_pFile = NULL;
        };
    };

    bool CTextFileReader::IsEOF()
    {
        return m_bEOF;
    };

    /*
    MBCS rule
        Trail byte ranges overlap the ASCII character set in many cases. 
        You can safely use bytewise scans for any control characters 
        (less than 32).

        andkaz: the same is true for UTF8. 
    */

    class Functor4UCS16
    {
    public:
        wchar_t operator()(char x)
        {
            return wchar_t(AVP_byte(x));
        };
    };

    class Functor4UCS16_BE
    {
    public:
        wchar_t operator()(char x)
        {
            return wchar_t(AVP_word(AVP_byte(x)) << 8);
        };
    };

    class Functor4Mbcs
    {
    public:
        char operator()(char x)
        {
            return x;
        };
    };

    template<class T, class F>
        void DoReadLineT(FILE* pFile, std::vector<T>& vecBuffer, bool& bEOF, F f)
    {
        vecBuffer.clear();
        for(bool bStopFlag = false; !bStopFlag; )
        {
            T ch = 0;
            if( sizeof(ch) != fread(&ch, 1, sizeof(ch), pFile) )
            {
                bEOF = true;
                int nError = ferror(pFile);
                if(nError)
                {
                    KLSTD_THROW_ERRNO_CODE(nError);
                };
            }
            else
            {
                if( ch && ch != f('\r') && ch != f('\n') )
                    vecBuffer.push_back(ch);
            };
            ;
            if(bEOF || f('\n') == ch)
                bStopFlag = true;
        };
    };

    void CTextFileReader::ReadLine(std::wstring& wstrLine)
    {
        wstrLine.clear();
        if(!m_bEOF)
        {
            switch(m_nType)
            {
            case TFT_CURRENT_CODEPAGE:
    #ifdef _WIN32
            case TFT_UNICODE_UTF8:
    #endif
            {
                std::vector<char> vecBuffer;
                vecBuffer.reserve(256);
                DoReadLineT(m_pFile, vecBuffer, m_bEOF, Functor4Mbcs());
                if(!vecBuffer.empty())
                {
                    std::vector<wchar_t> vecLine;
        #ifdef _WIN32                
                    vecLine.resize(vecBuffer.size());
                    int nChars = MultiByteToWideChar(
                                        (TFT_CURRENT_CODEPAGE == m_nType)
                                            ?   CP_ACP
                                            :   CP_UTF8, 
                                        0,
                                        &vecBuffer[0], 
                                        vecBuffer.size(), 
                                        &vecLine[0], 
                                        vecLine.size());
                    vecLine.resize(nChars);
        #else
                    vecBuffer.push_back(0);
                    vecLine.resize(vecBuffer.size());
                    KLSTD_A2WHelper(&vecLine[0], &vecBuffer[0], vecLine.size());
                    vecLine.resize(wstrLine.size()-1);
        #endif
                    if(!vecLine.empty())
                        wstrLine.assign(&vecLine[0], vecLine.size());
                };
            };
                break;
    #ifdef _WIN32
            case TFT_UNICODE_UCS16:
            case TFT_UNICODE_UCS16_BIGENDIAN:
            {
                std::vector<wchar_t> vecBuffer;
                vecBuffer.reserve(256);
                if(TFT_UNICODE_UCS16_BIGENDIAN == m_nType)
                    DoReadLineT(m_pFile, vecBuffer, m_bEOF, Functor4UCS16_BE());
                else
                    DoReadLineT(m_pFile, vecBuffer, m_bEOF, Functor4UCS16());
                if(!vecBuffer.empty())
                {
                    if(TFT_UNICODE_UCS16_BIGENDIAN == m_nType)
                    {//swap
                        const size_t c_nSize = vecBuffer.size();
                        for(size_t i = 0; i < c_nSize; ++i)
                        {
                            wchar_t ch = vecBuffer[i];
                            vecBuffer[i] = MAKEWORD(HIBYTE(ch), LOBYTE(ch));
                        };
                    };
                    wstrLine.assign(&vecBuffer[0], vecBuffer.size());
                };
            };
                break;
    #endif
            default:
                KLSTD_THROW(KLSTD::STDE_BADFORMAT);
                break;
            };
        };
    };

    class CParseFile
        :   public CParseData
    {
    public:
        CParseFile();
        virtual ~CParseFile();
        void DoParse(
                const wchar_t*      szwFileName, 
                const wchar_t**     ppSections,
                KLSTD::KLBaseQI*    pFilter,
                KLPAR::Params**     ppResult);
    protected:
        void OnStartSection(const wchar_t* szwName);
        void OnStartVariable(const wchar_t* szwName, const wchar_t* szwValue);
    protected:
        const wchar_t**     m_ppSections;
        KLSTD::KLBaseQI*    m_pFilter;        
        KLPAR::ParamsPtr    m_pLastSection;
        std::wstring        m_wstrLastSectionName;
        KLPAR::ParamsPtr    m_pResult;
        KLSTD::CAutoPtr<KLSTD::ReadConfigFilter>   m_pReadConfigFilter;
    };

    CParseFile::CParseFile()
        :   m_ppSections(NULL)
        ,   m_pFilter(NULL)
    {
        ;
    };

    CParseFile::~CParseFile()
    {
        ;
    };

    void CParseFile::DoParse(
                const wchar_t*      szwFileName, 
                const wchar_t**     ppSections,
                KLSTD::KLBaseQI*    pFilter,
                KLPAR::Params**     ppResult)
    {
    KL_TMEASURE_BEGIN(L"CParseFile::DoParse", 3)
        KLSTD_CHK(szwFileName, szwFileName && szwFileName[0]);
        KLSTD_CHKOUTPTR(ppResult);
        ;
        m_ppSections = ppSections;
        m_pFilter = pFilter;
        m_pLastSection = m_pResult = NULL;
        KLPAR_CreateParams(&m_pResult);
        m_pReadConfigFilter = NULL;
        if( pFilter)
        {
            pFilter->QueryInterface(
                        KLSTD_IIDOF(KLSTD::ReadConfigFilter),
                        (void**)&m_pReadConfigFilter);
        };
        if(m_pReadConfigFilter)
        {
            KLSTD_TRACE0(3, L"m_pReadConfigFilter was specified. \n");
        }
        else
        {
            KLSTD_TRACE0(3, L"m_pReadConfigFilter was NOT specified. \n");
        };
        ;
        KLSTD_TRACE1(3, L"szwFileName='%ls'\n", szwFileName);
        ;
        KLSTD::TextFileLineReaderPtr pReader = 
                        CTextFileReader::Instantiate(szwFileName);

        std::wstring wstrLine;            
        for(bool bStopFlag = false; !bStopFlag;)
        {
            if(pReader->IsEOF())
                bStopFlag = true;
            else
            {               
                pReader->ReadLine(wstrLine);
                if(!wstrLine.empty())
                    CParseData::Start(wstrLine.c_str());
                wstrLine.clear();
            };
        };
        pReader = NULL;
        m_pResult.CopyTo(ppResult);
        KLPARLOG_LogParams(4, m_pResult);
    KL_TMEASURE_END()
    };

    void CParseFile::OnStartSection(
                const wchar_t* szwName)
    {
        KLSTD_CHK(szwName, szwName && szwName[0]);
        KLSTD_ASSERT_THROW(m_pResult);
        ;
        m_wstrLastSectionName.clear();
        m_pLastSection = NULL;
        ;
        //search in sections list
        bool bMustBeUsed = false;
        if(m_ppSections)
        {            
            for(size_t i =0; !bMustBeUsed && m_ppSections[i]; ++i)
            {
                if(wcscmp(szwName, m_ppSections[i]) == 0)
                    bMustBeUsed = true;
            };
        }
        else
            bMustBeUsed = true;
        ;
        if(bMustBeUsed)
        {
            m_wstrLastSectionName = szwName;
            if( m_pResult->DoesExist(szwName) )
            {
                m_pLastSection = KLPAR::GetParamsValue(m_pResult, szwName);
                KLSTD_ASSERT_THROW(m_pLastSection);
            }
            else
            {
                m_pLastSection = NULL;
                KLPAR_CreateParams(&m_pLastSection);
                KLSTD::CAutoPtr<KLPAR::ParamsValue> p_parVal;
                KLPAR::CreateValue(m_pLastSection, &p_parVal);
                m_pResult->AddValue(szwName, p_parVal);
            };
        };
    };

    void CParseFile::OnStartVariable(
                const wchar_t* szwName, 
                const wchar_t* szwValue)
    {
        KLSTD_ASSERT_THROW(m_pResult);
        KLPAR::ParamsPtr pData = m_pLastSection;
        if(pData)
        {
            if(m_pReadConfigFilter)
            {
                KLSTD::CAutoPtr<KLPAR::Value> pVal;
                if(m_pReadConfigFilter->Filter(
                        m_wstrLastSectionName.c_str(),
                        szwName,
                        szwValue,
                        &pVal) && pVal)
                {
                    pData->ReplaceValue(szwName, pVal);
                };
            }
            else
            {
                KLSTD::CAutoPtr<KLPAR::StringValue> pVal;
                KLPAR::CreateValue(szwValue, &pVal);
                pData->ReplaceValue(szwName, pVal);
            };
        };
    };
};

using namespace KLSTDCONF;

KLCSC_DECL void KLSTD_ReadConfig(
            const wchar_t*      szwFileName, 
            const wchar_t**     ppSections,
            KLSTD::KLBaseQI*    pFilter,
            KLPAR::Params**     ppResult)
{
    KLSTDCONF::CParseFile().DoParse(
                    szwFileName, 
                    ppSections, 
                    pFilter, 
                    ppResult);
};

KLCSC_DECL void KLSTD_CreateTextFileLineReader(
                    const wchar_t*              szwFileName, 
                    KLSTD::TextFileLineReader** ppResult)
{
    KLSTD_CHK(szwFileName, szwFileName && szwFileName[0]);
    KLSTD_CHKOUTPTR(ppResult);
    ;
    KLSTD::TextFileLineReaderPtr pResult = 
                CTextFileReader::Instantiate(szwFileName);
    pResult.CopyTo(ppResult);
};
