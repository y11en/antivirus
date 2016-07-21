


#include <windows.h>



BOOL Encode(const char* p_pBuff, DWORD p_dwBuffSize, char** p_pResult, DWORD& p_dwResultSize)
{
    if (p_dwBuffSize == 0)
    {
        *p_pResult     = NULL ;
        p_dwResultSize = 0 ;

        return TRUE ;
    }

    int nAlphabetWeight[256] ;
    memset(nAlphabetWeight, 0, sizeof(int) * 256) ;
    for (DWORD dwIndex = 0; dwIndex < p_dwBuffSize; ++dwIndex)
    {
        ++nAlphabetWeight[p_pBuff[dwIndex]] ;
    }

    int nSymbolCount = 0 ;
    char pszAlphabet[256] ;
    char pszAlphabetForSave[256] ;
    for (;;)
    {
        int nMaxWeight = 0 ;
        int nIndexMaxWeight = 0 ;
        for (int nIndex = 0; nIndex < 256; ++nIndex)
        {
            if (nAlphabetWeight[nIndex] > nMaxWeight)
            {
                nIndexMaxWeight = nIndex ;
                nMaxWeight = nAlphabetWeight[nIndex] ;
            }
        }

        if (nMaxWeight == 0)
        {
            break ;
        }

        pszAlphabetForSave[nSymbolCount] = (char)nIndexMaxWeight ;
        pszAlphabet[nIndexMaxWeight] = (char)nSymbolCount ;
        nAlphabetWeight[nIndexMaxWeight] = 0 ;

        ++nSymbolCount ;
    }

    char* psz = new char[p_dwBuffSize + sizeof(int) + nSymbolCount] ;
    if (!psz)
    {
        return FALSE ;
    }

    memcpy(psz, &nSymbolCount, sizeof(int)) ;
    memcpy(psz + sizeof(int), pszAlphabetForSave, nSymbolCount) ;

    for (dwIndex = 0; dwIndex < p_dwBuffSize; ++dwIndex)
    {
        psz[sizeof(int) + nSymbolCount + dwIndex] = pszAlphabet[p_pBuff[dwIndex]] ;
    }

    *p_pResult     = psz ;
    p_dwResultSize = p_dwBuffSize + sizeof(int) + nSymbolCount ;

    return TRUE ;
}

BOOL Decode(const char* p_pBuff, DWORD p_dwBuffSize, char** p_pResult, DWORD& p_dwResultSize)
{
    if (p_dwBuffSize == 0)
    {
        *p_pResult     = NULL ;
        p_dwResultSize = 0 ;

        return TRUE ;
    }

    if (p_dwBuffSize < sizeof(int))
    {
        return FALSE ;
    }

    int nSymbolCount = 0 ;
    memcpy(&nSymbolCount, p_pBuff, sizeof(int)) ;

    if (p_dwBuffSize < (sizeof(int) + nSymbolCount))
    {
        return FALSE ;
    }

    char pszAlphabet[256] ;
    memcpy(pszAlphabet, p_pBuff + sizeof(int), nSymbolCount) ;

    int nBuffSize = p_dwBuffSize - sizeof(int) - nSymbolCount ;
    char* psz = new char[nBuffSize] ;
    if (!psz)
    {
        return FALSE ;
    }

    for (int nIndex = 0; nIndex < nBuffSize; ++nIndex)
    {
        psz[nIndex] = pszAlphabet[p_pBuff[sizeof(int) + nSymbolCount + nIndex]] ;
    }

    *p_pResult     = psz ;
    p_dwResultSize = p_dwBuffSize - sizeof(int) - nSymbolCount ;

    return TRUE ;
}