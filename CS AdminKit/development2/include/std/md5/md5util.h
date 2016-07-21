#ifndef __KLMD5UTIL2_H__
#define __KLMD5UTIL2_H__

namespace KLSTD
{
    /*!
      \brief	Calculates MD5 hash

      \param	pData       data pointer
      \param	nData       data size
      \param	bHex        hash if true, base64-like otherwise
      \retval	wstrHash    resulting hash (22 or 32 symbols depending on bHex value)
    */
    KLCSC_DECL void CalcMD5Hash(
                    const void*     pData, 
                    size_t          nData, 
                    bool            bHex, 
                    std::wstring&   wstrHash);

    KLCSC_DECL void CalcMD5Hash(
                    const void*     pData, 
                    size_t          nData, 
                    bool            bHex, 
                    std::string&    strHash);

    inline std::wstring CalcMD5HashW(
                    const void*     pData, 
                    size_t          nData, 
                    bool            bHex)
    {
        std::wstring wstrResult;
        CalcMD5Hash(pData, nData, bHex, wstrResult);
        return wstrResult;
    };

    inline std::string CalcMD5HashA(
                    const void*     pData, 
                    size_t          nData, 
                    bool            bHex)
    {
        std::string strResult;
        CalcMD5Hash(pData, nData, bHex, strResult);
        return strResult;
    };

    inline std::wstring CalcStringMD5HashW(
                    const std::wstring&         wstrData, 
                    bool                        bHex)
    {
        return CalcMD5HashW(
                    wstrData.c_str(), 
                    wstrData.size()*sizeof(wchar_t), 
                    bHex);
    };

    inline std::string CalcStringMD5HashA(
                    const std::wstring&         wstrData, 
                    bool                        bHex)
    {
        return CalcMD5HashA(
                    wstrData.c_str(), 
                    wstrData.size()*sizeof(wchar_t), 
                    bHex);
    };

    inline std::wstring CalcStringMD5HashW(
                    const std::string&          strData, 
                    bool                        bHex)
    {
        return CalcMD5HashW(
                    strData.c_str(), 
                    strData.size()*sizeof(char), 
                    bHex);
    };

    inline std::string CalcStringMD5HashA(
                    const std::string&          strData, 
                    bool                        bHex)
    {
        return CalcMD5HashA(
                    strData.c_str(), 
                    strData.size()*sizeof(char), 
                    bHex);
    };
};

#endif //__KLMD5UTIL2_H__
