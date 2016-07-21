#include <std/base/klstd.h>
#include <sstream>
#include <std/base/klstdutils.h>

#include <std/par/paramslogger.h>
#include <common/bin2hex.h>

#include <ctime>

#define KLCS_MODULENAME L"KLPARLOG"

namespace
{
    #define PARLOG_PREFIX_PARAM			L"+---"
    #define PARLOG_PREFIX_INDENT		L"|   "
    #define PARLOG_PREFIX_INDENT_LAST	L"    "

    void _LogValue(
                std::wstring    prefix, 
                std::wstring    name, 
                KLPAR::Value*   pVal, 
                bool            bIsLastChild,
                std::wostream&  os)
    {
	    if(prefix.empty())
		    os << L"\n";
	    os << prefix << PARLOG_PREFIX_PARAM << name;
        if(!pVal)
        {
            os << L" = <null>\n";
            return;
        };

	    //KLSTD_USES_CONVERSION;

	    switch(pVal->GetType())
	    {
	    case KLPAR::Value::STRING_T:
		    {
			    os << L" = ";
			    std::wstring strval = ((KLPAR::StringValue *)pVal)->GetValue();
			    if(strval.empty())
				    os << L"<empty string>";
			    else
			    {
				    os << L"(string)";
				    os << strval;
			    }
			    os << L"\n";
		    }
		    break;
	    case KLPAR::Value::BOOL_T:
		    os << L" = (bool)";
		    os << ((KLPAR::BoolValue *)pVal)->GetValue() ? L"true" : L"false";
		    os << L"\n";
		    break;
	    case KLPAR::Value::INT_T:
		    os << L" = (long)";
		    os << ((KLPAR::IntValue *)pVal)->GetValue();
		    os << L"\n";
		    break;
	    case KLPAR::Value::LONG_T:
		    os << L" = (long long)";
		    os << ((KLPAR::LongValue *)pVal)->GetValue();
		    os << L"\n";
		    break;
	    case KLPAR::Value::DATE_TIME_T:
		    {
                std::vector<wchar_t> vecBuffer;
                const time_t tmval = ((KLPAR::DateTimeValue *)pVal)->GetValue();
                {
                    struct tm gmtmbuffer;
                    struct tm* pTm = KLSTD_gmtime(&tmval, &gmtmbuffer);
                    if(pTm)
                    {
                        vecBuffer.resize(256);
                        wcsftime(
                                &vecBuffer[0], 
                                vecBuffer.size()-1, 
                                L"%d-%m-%Y %H-%M-%S", 
                                pTm);
                        vecBuffer[vecBuffer.size()-1] = 0;
                    };
                };
			    os << L" = ";
                if(!vecBuffer.empty())
			        os << &vecBuffer[0];
                else
                    os << L"<null> (" << tmval << L")";
			    os << L"\n";
		    };
		    break;
	    case KLPAR::Value::DATE_T:
		    os << L" = (date)";
		    os << ((const wchar_t*)KLSTD_A2W2(((KLPAR::DateValue *)pVal)->GetValue()));
		    os << L"\n";
		    break;
	    case KLPAR::Value::BINARY_T:
		    os << L" = ";
            {
                const size_t nSize = (size_t)((KLPAR::BinaryValue *)pVal)->GetSize();
                const size_t nMax = 64;
                os  << L"BLOB (size = " << nSize << L"): "
                    << KLSTD::MakeHexDataW(
                                        (void*)((KLPAR::BinaryValue *)pVal)->GetValue(),
                                        std::min(nSize, nMax));
                if( nSize > nMax )
                    os << L"...";
            }
		    //os << L"BLOB (size = " << ((KLPAR::BinaryValue *)pVal)->GetSize() << L")";
		    os << L"\n";
		    break;
	    case KLPAR::Value::FLOAT_T:
		    os << L" = (float)";
		    os << ((KLPAR::FloatValue *)pVal)->GetValue();
		    os << L"\n";
		    break;
	    case KLPAR::Value::DOUBLE_T:
		    os << L" = (double)";
		    os << ((KLPAR::DoubleValue *)pVal)->GetValue();
		    os << L"\n";
		    break;
	    case KLPAR::Value::PARAMS_T:
		    {
			    KLPAR::Params *v = ((KLPAR::ParamsValue *)pVal)->GetValue();
                KLSTD::klwstrarr_t arr;
                if(v)
                {
                    v->GetNames(arr.outref());
			        os << L" (Params)\n";
			        for(size_t i = 0, size = arr.size(); i < size; ++i)
			        {
				        KLSTD::CAutoPtr<KLPAR::Value> p;
				        v->GetValue(arr[i], &p);
				        _LogValue(
                                bIsLastChild 
                                    ?   (prefix + PARLOG_PREFIX_INDENT_LAST) 
                                    :   (prefix + PARLOG_PREFIX_INDENT), 
                                arr[i], 
                                p, 
                                i == size - 1,
                                os);
			        };
                }
                else
                {
                    os << L" (Params) <null> \n";
                };
		    }
		    break;
	    case KLPAR::Value::ARRAY_T:
		    {
			    KLPAR::ArrayValue *v = (KLPAR::ArrayValue *)pVal;

			    os << L" (Array)\n";
			    for(size_t i = 0, size = v->GetSize(); i < size; ++i)
			    {
				    KLSTD::CAutoPtr<KLPAR::Value> p;
				    v->GetAt(i, &p);
                    std::wostringstream osTemp;
                    osTemp << i;
				     _LogValue(
                            bIsLastChild
                                ?   (prefix + PARLOG_PREFIX_INDENT_LAST) 
                                :   (prefix + PARLOG_PREFIX_INDENT), 
                            osTemp.str(), 
                            p, 
                            i == size - 1,
                            os);
			    };
		    }
		    break;
	    }
    }
};

KLCSC_DECL void KLPARLOG_Params2Stream(
                        std::wostream&      os,
                        const KLPAR::Params *pParams)
{
	KLSTD::CAutoPtr<KLPAR::ParamsValue> pVal;
	KLPAR::CreateValue(const_cast<KLPAR::Params*>(pParams), &pVal);
	_LogValue(L"", L"", pVal, true, os);
};

KLSTD_NOTHROW KLCSC_DECL void KLPARLOG_LogParams(
                        int nTraceLevel,
                        const KLPAR::Params *pParams) throw()
{
    KLERR_TRY
        int nCurrentTraceLevel = 0;
        if( KLSTD::IsTraceStarted(&nCurrentTraceLevel) && 
            nCurrentTraceLevel >= nTraceLevel )
        {
            std::wostringstream os;
            KLPARLOG_Params2Stream(os, pParams);
            KLSTD_TRACE1(nTraceLevel, L"%ls", os.str().c_str());
        };
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(nTraceLevel, pError);
    KLERR_ENDTRY
};
