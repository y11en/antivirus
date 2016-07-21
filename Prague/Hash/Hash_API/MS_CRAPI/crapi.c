#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <wincrypt.h>
#include "crapi.h"

#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

//void HandleError(char *s);
#define HandleError(s)
BOOL GetHash(HCRYPTHASH hHash, unsigned long* hashlen, unsigned char* hashdata);

void ByteToStr(DWORD cb, void* pv, LPSTR sz)
			   //--------------------------------------------------------------------
			   // Parameters passed are:
			   //    pv is the array of BYTES to be converted.
			   //    cb is the number of BYTEs in the array.
			   //    sz is a pointer to the string to be returned.
			   
{
	//--------------------------------------------------------------------
	//  Declare and initialize local variables.
	
	BYTE* pb = (BYTE*) pv; // local pointer to a BYTE in the BYTE array
	DWORD i;               // local loop counter
	int b;                 // local variable
	
	//--------------------------------------------------------------------
	//  Begin processing loop.
	
	for (i = 0; i<cb; i++)
	{
		b = (*pb & 0xF0) >> 4;
		*sz++ = (b <= 9) ? b + '0' : (b - 10) + 'A';
		b = *pb & 0x0F;
		*sz++ = (b <= 9) ? b + '0' : (b - 10) + 'A';
		pb++;
		if ((i&3) == 3)
			*sz++ = ' ';

	}
	*sz++ = 0;
}

BOOL CRAPIInit (CRAPI_CTX *ctx, ALG_ID alg_id)
{
	ctx->hCryptProv = 0;
	ctx->hHash = 0;

	if(!CryptAcquireContext(&ctx->hCryptProv, NULL, NULL, PROV_RSA_FULL, 0)) 
	{
		HandleError("Error during CryptAcquireContext!");
		return FALSE;
	}

/*	if(!CryptGenKey(ctx->hCryptProv, CALG_RC2, CRYPT_EXPORTABLE, &ctx->hKey)) {
		HandleError("Error during CryptGenKey.");
		return FALSE;
	}
*/

	if (!CryptCreateHash(ctx->hCryptProv, alg_id, 0/*ctx->hKey*/, 0, &ctx->hHash))
	{
		HandleError("Error during CryptCreateHash.");
		return FALSE;
	}

	return TRUE;
}

BOOL CRAPIUpdate (CRAPI_CTX *ctx, unsigned char *data, unsigned int datalen)
{
	if (!CryptHashData(ctx->hHash, data, datalen, 0))
	{
		HandleError("Error during CryptHashData.");
		return FALSE;
	}
	return TRUE;
}

BOOL CRAPIGetHash(HCRYPTHASH hOHash, unsigned long* hashlen, unsigned char* hashdata)
{
	//--------------------------------------------------------------------
	//  Declare and initialize local variables. 
	
	HCRYPTHASH   hHash;
	DWORD        dwHashSize;
	DWORD        dwHashLen = sizeof(DWORD);
	
	//--------------------------------------------------------------------
	// Duplicate the hash passed in.
	// This function only works in Windows 2000 and later.
	// The hash is duplicated in order to leave the original hash intact.
	
	if (!CryptDuplicateHash(hOHash, NULL, 0, &hHash))
	{
		HandleError("Error during CryptDuplicateHash.");
		return FALSE;
	}
	
	if(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&dwHashSize, &dwHashLen, 0))
	{
		HandleError("CryptGetHashParam failed to get size.");
		return FALSE;
	}
	
	dwHashLen = dwHashSize;
	if (hashlen)
	{
		if (*hashlen < dwHashLen)
		{
			*hashlen = dwHashLen;
			return FALSE;
		}
		*hashlen = dwHashLen;
	}
	
	if (hashdata)
		memset(hashdata, 0, dwHashLen);
	
	if(!CryptGetHashParam(hHash, HP_HASHVAL, hashdata, &dwHashLen, 0)) 
	{
		HandleError("CryptGetHashParam failed to get hash data.");
		return FALSE;
	}
	
	if (!CryptDestroyHash(hHash))
	{
		HandleError("ERROR - CryptDestroyHash");
	}
	
	return TRUE;
} // End of GetHash

BOOL CRAPIFinal(CRAPI_CTX *ctx, unsigned long* hashlen, unsigned char* hashdata)
{
	BOOL bRes = CRAPIGetHash(ctx->hHash, hashlen, hashdata);

	if(ctx->hHash)
		CryptDestroyHash(ctx->hHash);

//	if(ctx->hKey) 
//		CryptDestroyKey(ctx->hKey);
	
	if(ctx->hCryptProv) 
		CryptReleaseContext(ctx->hCryptProv,0);

	return bRes;
}


//--------------------------------------------------------------------
//  This example uses the function HandleError, a simple error
//  handling function, to print an error message to the standard error 
//  (stderr) file and exit the program. 
//  For most applications, replace this function with one 
//  that does more extensive error reporting.

//void HandleError(char *s)
//{
//	DWORD err = GetLastError();
//    fprintf(stderr,"An error occurred in running the program. \n");
//    fprintf(stderr,"%s\n",s);
//    fprintf(stderr, "Error number %x.\n", GetLastError());
//    fprintf(stderr, "Program terminating. \n");
//    exit(1);
//} // End of HandleError