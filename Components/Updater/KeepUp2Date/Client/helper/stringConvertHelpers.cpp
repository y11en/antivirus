#include "stringConvertHelpers.h"




std::string KLUPD::wideCharToAscii(const wchar_t *input)
{
    std::string result;

    for(size_t index = 0; input[index]; ++index)
    {
		const char Result = wideCharToAscii( input[index] );
        result += Result;
    }
    return result;
}

char KLUPD::wideCharToAscii(const wchar_t input)
{
#ifdef WSTRING_SUPPORTED
    char wideCharBuffer[MB_LEN_MAX + 1];
    const int bytesConverted = wctomb(wideCharBuffer, input);
    if((-1 == bytesConverted)
        || (MB_LEN_MAX < bytesConverted))
    {
        return '?';
    }

    return wideCharBuffer[0];
#else
	return wctob(input);
#endif
}



#ifdef WSTRING_SUPPORTED

    std::wstring KLUPD::asciiToWideChar(const std::string &input)
    {
        std::wstring result;
        const size_t max_mbchars = sizeof(wchar_t) / sizeof(char);

        for(size_t index = 0; index < input.size() && input[index] != '\0';)
        {
            wchar_t  wideChar;
            memset(&wideChar, 0, sizeof(wideChar));

            char charBuffer[MB_LEN_MAX + 1];
            memset(&charBuffer, 0, sizeof(charBuffer));
            
            for(size_t j = 1; j <= max_mbchars; ++j)
            {  
                for(size_t k = 0; k < j; ++k)
                {	
                    if(index + k < input.size())
                        charBuffer[k] = input[index + k];
                }
                
                const int convertedSizeBytes = mbtowc(&wideChar, charBuffer, j);
                
                if(j == static_cast<size_t>(convertedSizeBytes))
                {
                    index += j;
                    break;
                }

                if(j == max_mbchars)
                {
                    // Upper limit reached, replace char with question mark symbol 
                    wideChar = L'?';
                    ++index;
                    break;
                }
            }
            result += wideChar;
        }
        
        return result;
    }

    wchar_t KLUPD::asciiToWideChar(const char input)
    {
        std::string str;
        str.push_back(input);
        return asciiToWideChar(str)[0];
    }


    std::wstring KLUPD::tStringToWideChar(const TCHAR *input)
    {
        #if (defined _UNICODE) || (defined UNICODE)
            return input;
        #else
            return asciiToWideChar(input);
        #endif
    }

    std::string KLUPD::tStringToAscii(const TCHAR *input)
    {
        #if (defined _UNICODE) || (defined UNICODE)
            return wideCharToAscii(input);
        #else
            return input;
        #endif
    }

    #if (defined _UNICODE) || (defined UNICODE)
        std::wstring KLUPD::asciiToTString(const std::string &input)
        {
            return asciiToWideChar(input);
        }

        std::wstring KLUPD::wideCharToTString(const std::wstring &input)
        {
            return input;
        }

    #else // _UNICODE || UNICODE
        std::string KLUPD::asciiToTString(const std::string &input)
        {
            return input;
        }

        std::string KLUPD::wideCharToTString(const std::wstring &input)
        {
            return wideCharToAscii(input);
        }
    #endif // _UNICODE || UNICODE


#else // WSTRING_SUPPORTED

    std::string KLUPD::asciiToWideChar(const std::string &input)
    {
        return input;
    }

    char KLUPD::asciiToWideChar(const char input)
    {
        return input;
    }


    #if (defined _UNICODE) || (defined UNICODE)
        std::string KLUPD::tStringToWideChar(const wchar_t *input)
        {
            return wideCharToAscii(input);
        }
        std::string KLUPD::tStringToAscii(const wchar_t *input)
        {
            return wideCharToAscii(input);
        }
    #else
        std::string KLUPD::tStringToWideChar(const char *input)
        {
            return input;
        }
        std::string KLUPD::tStringToAscii(const char *input)
        {
            return input;
        }
    #endif

    std::string KLUPD::asciiToTString(const std::string &input)
    {
        return input;
    }
    std::string KLUPD::wideCharToTString(const std::string &input)
    {
        return input;
    }

#endif  // WSTRING_SUPPORTED

