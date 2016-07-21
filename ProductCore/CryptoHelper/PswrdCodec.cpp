// CryptoHelper.cpp: implementation of the CPasswordCodec class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "PswrdCodec.h"
#include <pr_cpp.h>
#include <iface/i_token.h>

EXTERN_C hROOT g_root ;

#pragma comment(lib, "Advapi32.lib")
//
////////////////////////////////////////////////////////////////////////////////
CPasswordCodec::CPasswordCodec()
{
    cAutoObj<cToken> hToken ;
    g_root->sysCreateObjectQuick(hToken, IID_TOKEN) ;
    cStrObj strUserName ;
    if (!!hToken)
    {
        strUserName.init() ;
        strUserName.assign(hToken, pgOBJECT_NAME) ;
    }

    memset(m_szContainerName, 0, sizeof(m_szContainerName)) ;
	strcpy(m_szContainerName, "KAV60_DATA_");

    if (strUserName.length())
    {
        DWORD dwCopyMax = sizeof(m_szContainerName) - 1 - strlen(m_szContainerName) ;
        strncpy(m_szContainerName + strlen(m_szContainerName), (tCHAR*)strUserName.c_str(cCP_ANSI), dwCopyMax > strUserName.length() ? strUserName.length() : dwCopyMax) ;
    }

    PR_TRACE((g_root, prtNOTIFY, "crypt\tContainer name: %s", m_szContainerName)) ;
}
//
////////////////////////////////////////////////////////////////////////////////
CPasswordCodec::CryptoErr CPasswordCodec::encryptPassword(DATA_BLOB PasswordBlob, DATA_BLOB &EncryptedPasswordBlob)
{
	HCRYPTPROV hCryptProv;         // -- Cryptographic CSP
	// Get the handle to the provider.
	if( !CryptAcquireContext(&hCryptProv, m_szContainerName, NULL, PROV_RSA_FULL, 0) )
	{
	// Create a new default key container. 
		if( !CryptAcquireContext(&hCryptProv, m_szContainerName, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET) ) 
		{
            DWORD dwError = GetLastError();
            PR_TRACE((g_root, prtERROR, "crypt\tError %x during CryptAcquireContext", dwError)) ;
 

			return CErr_CreateKey;
		}
	}

	// A cryptographic context with a key container is available.
	// Get the handle to the encrypter's exchange public key. 
	HCRYPTKEY hExchangeKey;  // Exchange public key for encrypting session key 

	if( !CryptGetUserKey( hCryptProv, AT_KEYEXCHANGE, &hExchangeKey ) ) 
	{
		if( GetLastError() == NTE_NO_KEY )
		{
			// -- A user's exchange key pair doesn't exist. Generate a new exchange key pair. 
			// -- Add CRYPT_USER_PROTECTED if you want to have message box asking user's confirmation
			if( !CryptGenKey( hCryptProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hExchangeKey ) ) 
			{
				if( hCryptProv ) 
					CryptReleaseContext( hCryptProv, 0 );
				return CErr_GenKey;
			}
		}
		else
		{
			if( hCryptProv ) 
				CryptReleaseContext( hCryptProv, 0 );
			return CErr_GetUserKey;
		}
	}
	
	HCRYPTKEY hKey; // -- Session key for encrypting password
	// No password available. Create a new random session key for encryption. 
	if( !CryptGenKey( hCryptProv, CALG_RC4,  CRYPT_EXPORTABLE, &hKey ) )
	{
		if( hExchangeKey )
			CryptDestroyKey( hExchangeKey );
	
		if( hCryptProv ) 
			CryptReleaseContext( hCryptProv, 0 );

		return CErr_GenSessionKey;
	}

	PBYTE pbKeyBlob;               // -- Session key buffer 
	DWORD dwKeyBlobLength;         // -- it's length

	// Determine size of the session key buffer. 
	if( !CryptExportKey( hKey, hExchangeKey, SIMPLEBLOB, 0, NULL, &dwKeyBlobLength ) )
	{
		// MyHandleError("Error computing BLOB length!\n");
		if( hExchangeKey )
			CryptDestroyKey( hExchangeKey );

		if( hCryptProv ) 
			CryptReleaseContext( hCryptProv, 0 );

		return CErr_CompBLOB;
	}

	// Allocate memory for session key buffer. 
	pbKeyBlob = (BYTE *)malloc( dwKeyBlobLength );
	if( NULL == pbKeyBlob ) 
	{
		// MyHandleError( "Out of memory.\n" ); 
		if( hExchangeKey )
			CryptDestroyKey( hExchangeKey );

		if( hKey ) 
			CryptDestroyKey( hKey);

		if( hCryptProv ) 
			CryptReleaseContext( hCryptProv, 0 );

		return CErr_OutOfMem;
	}

	// Encrypt and export the session key into a simple key BLOB. 
	if( !CryptExportKey( hKey, hExchangeKey, SIMPLEBLOB, 0, pbKeyBlob, &dwKeyBlobLength ) )
	{
		//MyHandleError( "Error while encrypting session key.\n" );
		if( pbKeyBlob ) 
			free( pbKeyBlob );

		if( hExchangeKey )
			CryptDestroyKey( hExchangeKey );

		if( hKey ) 
			CryptDestroyKey( hKey);
		
		if( hCryptProv ) 
			CryptReleaseContext( hCryptProv, 0 );
		
		return CErr_EncryptSession;
	}

	PBYTE pbEncryptedBuffer;       // -- Encrypted password buffer 
	DWORD dwEncryptedBufferLength; // -- it's length

	// -- Release the key exchange key handle. 
	if( hExchangeKey )
	{
		if( !CryptDestroyKey( hExchangeKey ) )
		{
			//MyHandleError( "Error while desroying exchange public key." ); 
			if( pbKeyBlob ) 
				free( pbKeyBlob );
			if( hKey ) 
				CryptDestroyKey( hKey);

			if( hCryptProv ) 
				CryptReleaseContext( hCryptProv, 0 );
			
			return CErr_DestroyExKey;
		}
		hExchangeKey = 0;
	}

	// Encrypt the password with a session key. 
	// Find out encrytion block size first.
	
	pbEncryptedBuffer = (PBYTE)malloc( PasswordBlob.cbData );
	memcpy( pbEncryptedBuffer, PasswordBlob.pbData, PasswordBlob.cbData );
	dwEncryptedBufferLength = PasswordBlob.cbData;

	CryptoErr _ToReturn = CErr_Ok; 

	if( CryptEncrypt( hKey, 0, TRUE, 0, pbEncryptedBuffer, &dwEncryptedBufferLength, PasswordBlob.cbData ) )
	{ 
	    // Password encrypted. 
		// Now store encrypted password in a buffer along with the encrypted (exported) session key.
		// A format of the buffer containing encrypted data is as follows:
		//  | sizeof( session key ) | session key | sizeof( encrypted password ) | encrypted password |
        EncryptedPasswordBlob.cbData = dwEncryptedBufferLength + sizeof( dwEncryptedBufferLength ) + dwKeyBlobLength + sizeof( dwKeyBlobLength );
		EncryptedPasswordBlob.pbData = (BYTE *)malloc( EncryptedPasswordBlob.cbData );

		DWORD dwOffset = 0;
		memcpy( EncryptedPasswordBlob.pbData + dwOffset, &dwKeyBlobLength, sizeof( dwKeyBlobLength ) );
		dwOffset += sizeof( dwKeyBlobLength );
		memcpy( EncryptedPasswordBlob.pbData + dwOffset, pbKeyBlob, dwKeyBlobLength );
		dwOffset += dwKeyBlobLength;
		memcpy( EncryptedPasswordBlob.pbData + dwOffset, &dwEncryptedBufferLength, sizeof( dwEncryptedBufferLength ) );
		dwOffset += sizeof( dwEncryptedBufferLength );
		memcpy( EncryptedPasswordBlob.pbData + dwOffset, pbEncryptedBuffer, dwEncryptedBufferLength );
		dwOffset += dwEncryptedBufferLength;
	}
	else
		_ToReturn = CErr_Encrypting;

	// Free memory.
	if( pbEncryptedBuffer ) 
		free( pbEncryptedBuffer ); 
	if( pbKeyBlob ) 
		free( pbKeyBlob );

	// Destroy the session key. 
	if( hKey ) 
		CryptDestroyKey( hKey);
	// Release the provider handle. 
	if( hCryptProv ) 
		CryptReleaseContext( hCryptProv, 0 );

	return _ToReturn;
}
//
////////////////////////////////////////////////////////////////////////////////
CPasswordCodec::CryptoErr CPasswordCodec::decryptPassword(DATA_BLOB EncryptedPasswordBlob, DATA_BLOB &PasswordBlob)
{
    PBYTE pbKeyBlob;					// -- Session key buffer 
    DWORD dwKeyBlobLength = 0;			// -- it's length
    PBYTE pbEncryptedBuffer;			// -- Encrypted password buffer 
    DWORD dwEncryptedBufferLength = 0;	// -- it's length

    // Retrieve all data from encrypted buffer.
    // Note: buffer format is:
    // | sizeof( session key ) | session key | sizeof( encrypted password ) | encrypted password |
	if( EncryptedPasswordBlob.cbData < sizeof( dwKeyBlobLength ) )
		return CErr_Decrypting;

    const BYTE* p = (const BYTE*)EncryptedPasswordBlob.pbData;
    memcpy( &dwKeyBlobLength, p, sizeof( dwKeyBlobLength ) );

	if( EncryptedPasswordBlob.cbData < dwKeyBlobLength )
		return CErr_Decrypting;

    p += sizeof( dwKeyBlobLength );
    pbKeyBlob = (PBYTE)malloc( dwKeyBlobLength );
    memcpy( pbKeyBlob, p, dwKeyBlobLength );
    p += dwKeyBlobLength;
    memcpy( &dwEncryptedBufferLength, p, sizeof( dwEncryptedBufferLength ) );
    p += sizeof( dwEncryptedBufferLength );
    pbEncryptedBuffer = (PBYTE)malloc( dwEncryptedBufferLength );
    memcpy( pbEncryptedBuffer, p, dwEncryptedBufferLength );
    p += dwEncryptedBufferLength;

	HCRYPTPROV hCryptProv;         // -- Cryptographic CSP
    // Get the handle to the provider.
	if( !CryptAcquireContext( &hCryptProv, m_szContainerName, NULL, PROV_RSA_FULL, 0 ) )
	{
		//MyHandleError( "Could not obtain a key container.\n" );
		return CErr_GetKeyContainer;
	}
	
	// Get the user's exchange public key from key container.
    HCRYPTKEY hExchangeKey;  // -- Exchange public key for encrypting session key 
	if( !CryptGetUserKey( hCryptProv, AT_KEYEXCHANGE, &hExchangeKey ) )
	{
		// MyHandleError( "Could not obtain a user's public key from key container.\n" );
		if( hCryptProv ) 
			CryptReleaseContext( hCryptProv, 0 );

		return CErr_GetUserKey;
	}

	// Decrypt (import) encrypted session key via exchange key pair
	// (its private key more precisely).
	HCRYPTKEY hKey;  // Session key for encrypting password
	
	if( !CryptImportKey( hCryptProv, (PBYTE)pbKeyBlob, dwKeyBlobLength, hExchangeKey, 0, &hKey ) )
	{
		// MyHandleError( "Could not decrypt session key.\n" );
		if( hExchangeKey )
			CryptDestroyKey( hExchangeKey );

		if( hCryptProv ) 
			CryptReleaseContext(hCryptProv, 0);

		return CErr_DecryptSession;
	}
	
	// Decrypt encrypted password via decrypted session key.
	void* pTempBuffer = malloc( dwEncryptedBufferLength );
	DWORD dwTempBuffer = dwEncryptedBufferLength;
	memcpy( pTempBuffer, pbEncryptedBuffer, dwEncryptedBufferLength );

	CryptoErr _ToReturn = CErr_Ok; 

	if( CryptDecrypt( hKey, NULL, TRUE, 0, (BYTE *)pTempBuffer, &dwTempBuffer ) )
	{
		PasswordBlob.pbData = (BYTE *)pTempBuffer;
		PasswordBlob.cbData = dwTempBuffer;
		pTempBuffer = NULL;
	}
	else 
		_ToReturn = CErr_Decrypting;

    // -- Free memory.
    if( pbEncryptedBuffer ) 
		free( pbEncryptedBuffer ); 
    if( pbKeyBlob ) 
		free( pbKeyBlob );
    // Release the key exchange key handle. 
    if( hExchangeKey )
        CryptDestroyKey( hExchangeKey );
	hExchangeKey = 0;

    // Destroy the session key. 
    if( hKey ) 
		CryptDestroyKey( hKey);

    // Release the provider handle. 
    if( hCryptProv ) 
		CryptReleaseContext( hCryptProv, 0 );
	
	return _ToReturn;
}
//
tERROR CPasswordCodec::CryptoErr2PragueError(CryptoErr cerr)
{
	if( cerr == CErr_Ok )
		return errOK;

	tERROR error = errOK;
	if( cerr >= CErr_CreateKey && cerr <= CErr_DestroyExKey )
		error = errBAD_SIGNATURE;
	else if( cerr >= CErr_CompBLOB && cerr <= CErr_OutOfMem )
		error = errOUT_OF_SPACE;
	else 
		error = errOPERATION_CANCELED;
	return error;
}
//
TCHAR* CPasswordCodec::CryptoErrDescr(CryptoErr cerr)
{
	switch(cerr)
	{
	case CErr_CreateKey			: return "Could not create a new default key container.";
	case CErr_GetKeyContainer	: return "Could not obtain a key container.";
	case CErr_GenKey			: return "Could not generate a new exchange key pair.";
	case CErr_GetUserKey		: return "Could not obtain a user's public key from key container.";
	case CErr_GenSessionKey		: return "Could not create a new random session key for encryption.";
	case CErr_DestroyExKey		: return "Error while desroying exchange public key.";
	case CErr_CompBLOB			: return "Error computing BLOB length!";
	case CErr_OutOfMem			: return "Could not allocate memory for session key buffer.";
	case CErr_EncryptSession	: return "Could not encrypt session key.";
	case CErr_Encrypting		: return "Error while encrypting!";
	case CErr_DecryptSession	: return "Could not decrypt session key.";
	case CErr_Decrypting		: return "Error while decrypting!";
	}
	return "";
}