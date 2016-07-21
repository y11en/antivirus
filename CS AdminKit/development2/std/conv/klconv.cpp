/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file KlBase.cpp
 * \author Андрей Казачков
 * \date 2002
 * \brief Функции для конвертации UNICODE строк
 *
 */

#include "build/general.h"
#include "std/conv/klconv.h"

#ifdef __unix
//#  include <libgen.h> andkaz: what is ?
//#  include "_conv.h" andkaz: what is ?
#endif

#ifdef N_PLAT_NLM
# include <nunicode.h>
# include <nwnspace.h>
# include "wcharcrt.h"
#  include "_conv.h"
#endif

#ifdef _WIN32    

    #include "std/win32/klos_win32v50.h"

#endif



KLCSC_DECL size_t KLSTD_A2WHelper(wchar_t* pBuffer, const char* szaSrc, int nBuffer)
{
#ifdef _WIN32
	return ::MultiByteToWideChar(CP_ACP, 0, szaSrc, -1, pBuffer, nBuffer);
#else
    mbstate_t state;
    memset (&state, 0, sizeof (state));
    const char* src = szaSrc;
    size_t res = mbsrtowcs(pBuffer, &src, pBuffer?nBuffer:0, &state);
    if((size_t)(-1) == res)
    {
        if(pBuffer && nBuffer > 0)
        {
            for(size_t i = 0; i < nBuffer-1;++i)
            {
                pBuffer[i] = L'?';
            };
            pBuffer[nBuffer-1] = 0;
            res = nBuffer;
        }
        else
            res = 1;
    }
    else
        ++res;//mbsrtowcs returns number of symbols written without 0
    return res;
#endif
}; 

KLCSC_DECL size_t KLSTD_W2AHelper(char* pBuffer, const wchar_t* szwSrc, int nBuffer)
{
#ifdef _WIN32
	return ::WideCharToMultiByte(CP_ACP, 0, szwSrc, -1, pBuffer, nBuffer, NULL, NULL);
#else
    mbstate_t state;
    memset (&state, 0, sizeof (state));
    const wchar_t* src = szwSrc;
    size_t res = wcsrtombs(pBuffer, &src, pBuffer?nBuffer:0, &state);
    if((size_t)(-1) == res)
    {
        if(pBuffer && nBuffer > 0)
        {
            for(size_t i = 0; i < nBuffer-1;++i)
            {
                pBuffer[i] = '?';
            };
            pBuffer[nBuffer-1] = 0;
            res = nBuffer;
        }
        else
            res = 1;
    }
    else
        ++res;//wcsrtombs returns number of symbols written without 0
    return res;
#endif
};
