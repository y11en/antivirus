#include <std/base/klstd.h>
#include <std/md5/md5util.h>
#include <common/bin2hex.h>
#include <avp/text2bin.h>
#include <openssl/md5.h>


#define KLCS_MODULENAME L"KLSTD"

namespace 
{
	void CalcMD5Hash_(const void* pData, size_t nData, unsigned char* pDigest)
	{
        MD5_CTX md5;
        MD5_Init(&md5);
        MD5_Update(&md5, pData, nData);
        MD5_Final(pDigest, &md5);
	};
}

namespace KLSTD
{
    KLCSC_DECL void CalcMD5Hash(
                    const void*     pData, 
                    size_t          nData, 
                    bool            bHex, 
                    std::wstring&   wstrHash)
    {
        wstrHash.clear();
        unsigned char digest[MD5_DIGEST_LENGTH];
		CalcMD5Hash_(pData, nData, digest);
        if(bHex)
        {
            wstrHash = MakeHexDataW(digest, MD5_DIGEST_LENGTH);
        }
        else
        {
            std::vector<AVP_byte> vecTextData;
            vecTextData.resize( B2T_LENGTH( nData ) + 2 );
            BinToText(digest, MD5_DIGEST_LENGTH, &vecTextData[0], vecTextData.size());
            wstrHash.assign((const wchar_t*)KLSTD_A2CW2((char*)&vecTextData[0]));
        };
    };

    KLCSC_DECL void CalcMD5Hash(
                    const void*     pData, 
                    size_t          nData, 
                    bool            bHex, 
                    std::string&    strHash)
    {
        strHash.clear();
        unsigned char digest[MD5_DIGEST_LENGTH];
		CalcMD5Hash_(pData, nData, digest);
        if(bHex)
        {
            strHash = MakeHexDataA(digest, MD5_DIGEST_LENGTH);
        }
        else
        {
            std::vector<AVP_byte> vecTextData;
            vecTextData.resize( B2T_LENGTH( nData ) + 2 );
            BinToText(digest, MD5_DIGEST_LENGTH, &vecTextData[0], vecTextData.size());
            strHash.assign((char*)&vecTextData[0]);
        };
    };
};
